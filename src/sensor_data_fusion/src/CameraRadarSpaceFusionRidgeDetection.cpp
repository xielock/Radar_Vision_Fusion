//以相机坐标系为基础，将雷达与相机数据空间融合
#include"ros/ros.h"
#include"std_msgs/String.h"
#include<image_transport/image_transport.h>
#include<cv_bridge/cv_bridge.h>
#include<sensor_msgs/image_encodings.h>
#include<stdio.h>
#include <iostream>
#include<string>
#include<cmath>
#include<fstream>
#include <vector>
#include<deque>
#include<eigen3/Eigen/Core>
#include<eigen3/Eigen/Dense>
#include<opencv2/opencv.hpp>
#include<opencv2/core/eigen.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<mymsg/CameraDataPerSec.h>
#include<mymsg/RadarData.h>
#include<mymsg/RadarDataPerSec.h>
#include<mymsg/CameraRadarTimeFusion.h>
#include<mymsg/CameraRadarSpaceFusion.h>

using namespace std;
using namespace cv;

ros::Publisher pub_space_fusion;

//若有需要可以自己加camera1的参数
Eigen::Matrix<double,3,3> camera0Matrix_eigen;//相机内参矩阵的eigen形式
Eigen::Matrix<double,4,4> radarToCamera0Matrix;//毫米波雷达向相机坐标转化的矩阵
//string camera0_radar_calibration_result = "./src/cfg/camera0_radar_calibration_result.txt";
string camera0_radar_calibration_result = "./src/cfg/camera0_radar_calibration_result_ridge_detection.txt";
string camera0_calibration_result = "./src/cfg/camera0_calibration_result_ridge.txt";
double camera0_radar_dx, camera0_radar_dy, camera0_radar_dz;
double camera0_radar_tx, camera0_radar_ty, camera0_radar_tz;
int radar_install_height = 3000;

//读入相机与雷达的标定结果
void ReadInCameraRadarCalibrationResult(string &result_path, double &dx, double &dy, double &dz, double &tx, double &ty, double &tz)
{
    ifstream calibration_result(result_path);
    if(!calibration_result.is_open())
    {
        cout<<"No camera -radar calibration result!"<<endl;
        exit(0);
    }
    string temp_line;
    getline(calibration_result, temp_line);
    dx = stod(temp_line);
    getline(calibration_result, temp_line);
    dy = stod(temp_line);
    getline(calibration_result, temp_line);
    dz = stod(temp_line);
    getline(calibration_result, temp_line);
    tx = stod(temp_line);
    getline(calibration_result, temp_line);
    ty = stod(temp_line);
    getline(calibration_result, temp_line);
    tz = stod(temp_line);
    calibration_result.close();
}

//读入相机内参
void ReadInCameraCalibrationResult(string &result_path, Eigen::Matrix<double,3,3> &cameraMatrix)
{
    ifstream read_in(result_path);
    if(!read_in.is_open())
    {
        cout<<"can open camera_inter_para."<<endl;
        return;
    }
    string line;
    deque<double> temp_para;
    while (!read_in.eof())
    {
        getline(read_in, line);
        if(line != "")
        {
            char *temp = (char *)line.c_str();
            double a,b,c;
            sscanf(temp,"%lf %lf %lf",&a,  &b, &c);
            temp_para.push_back(a);
            temp_para.push_back(b);
            temp_para.push_back(c);
        }
    }
    read_in.close();
    for(int i = 0; i < 3; ++i)
    {
        for(int j = 0; j < 3; ++j)
        {
            cameraMatrix(i, j) = temp_para[0];
            temp_para.pop_front();
        }
    }
}

//由相机与雷达的标定结果计算雷达到相机的空间转换矩阵
void CalRadarToCameraMatrix(Eigen::Matrix<double,4,4> &Matrix, const double &dx, const double &dy, const double &dz, const double &tx, const double &ty, const double &tz)
{
    Eigen::Matrix<double,3,1> Translation_Matrix;
    Eigen::Matrix<double,3,3> Rotation_Matrix;
    Eigen::Matrix<double,3,3> Rotation_X_Matrix;
    Eigen::Matrix<double,3,3> Rotation_Y_Matrix;
    Eigen::Matrix<double,3,3> Rotation_Z_Matrix;

    Translation_Matrix << dx, dy, dz;
    Rotation_X_Matrix << 1,0,0,
                        0,cos(tx),-sin(tx),
                        0,sin(tx),cos(tx);
    Rotation_Y_Matrix << cos(ty),0,sin(ty),
                        0,1,0,
                        -sin(ty),0,cos(ty);
    Rotation_Z_Matrix << cos(tz),-sin(tz),0,
                        sin(tz),cos(tz),0,
                        0,0,1;
    Rotation_Matrix = Rotation_X_Matrix * Rotation_Y_Matrix * Rotation_Z_Matrix;
    Matrix <<  Rotation_Matrix(0,0),Rotation_Matrix(0,1),Rotation_Matrix(0,2),Translation_Matrix(0,0),
                        Rotation_Matrix(1,0),Rotation_Matrix(1,1),Rotation_Matrix(1,2),Translation_Matrix(1,0),
                        Rotation_Matrix(2,0),Rotation_Matrix(2,1),Rotation_Matrix(2,2),Translation_Matrix(2,0),
                        0,0,0,1;
    
    return;
}

void CalBox(const mymsg::CameraRadarTimeFusionConstPtr &msg, vector<mymsg::SingleBox> &box, const Eigen::Matrix<double,3,3> &cameraMatrix_eigen, const Eigen::Matrix<double,4,4> &radarToCameraMatrix)
{
    for(auto itptr = msg->radar_data.radar_data_per_sec.begin(); itptr != msg->radar_data.radar_data_per_sec.end(); itptr++)
    {
        Eigen::Matrix<double,4,1> radar_temp;//把雷达得到的目标点转换成空间坐标系中的点，为计算该变量为4维
        Eigen::Matrix<double,4,1> camera_temp;//中间结果，雷达目标点在相机坐标系中的位置，4维
        Eigen::Matrix<double,3,1> temp;//中间结果，雷达目标点在相机坐标系中的位置，3维
        Eigen::Matrix<double,3,1> temp_1m;//在雷达目标点旁x轴方向假想有一条1m长的线
        
        radar_temp << 0, (itptr->distLat*1000), (itptr->distLong*1000), 1;
        cout<<itptr->distLong<<","<<itptr->distLat<<endl;
        //radar_temp << 0, (radar_install_height), (itptr->distLong*1000), 1;
        camera_temp = radarToCameraMatrix * radar_temp;
        temp << camera_temp(0,0), camera_temp(1,0), camera_temp(2,0);
        temp_1m << (camera_temp(0,0) + 1000), camera_temp(1,0), camera_temp(2,0);

        Eigen::Matrix<double,3,1> temp_pixel;//利用针孔成像模型计算目标点在图像坐标系中的坐标
        Eigen::Matrix<double,3,1> temp_1m_pixel;
        temp_pixel = (1 / temp(2, 0)) * camera0Matrix_eigen * temp;
        temp_1m_pixel = (1 / temp_1m(2, 0)) * camera0Matrix_eigen * temp_1m;

        mymsg::SingleBox temp_box;
        temp_box.distance = sqrt(pow(itptr->distLat, 2.0) + pow(itptr->distLong, 2.0));//计算距离辅助判断滑窗步长及范围
        temp_box.centper_pt_x = static_cast<int>(temp_pixel(0, 0));
        temp_box.centper_pt_y = static_cast<int>(temp_pixel(1, 0));
        temp_box.reference_1m = abs(static_cast<int>(temp_pixel(0, 0) - temp_1m_pixel(0, 0)));

        box.push_back(temp_box);
    }
    cout<<"*****************"<<endl;
}

void CallBack(const mymsg::CameraRadarTimeFusionConstPtr &msg)
{
    //如果处理速度比发布速度快，会神奇的出现一副空图像，不写这个判断就会卡死在这里
    if(msg->camera_data.rosImage[0].width == 0)
        return;
    unique_ptr<mymsg::CameraRadarSpaceFusion> pub_msg(new mymsg::CameraRadarSpaceFusion);
    pub_msg->camera_data = msg->camera_data;
    CalBox(msg, pub_msg->box0, camera0Matrix_eigen, radarToCamera0Matrix);
    //如果有使用双相机的需要，可使用此函数将转换结果存到box1中，如下：
    //CalBox(pub_msg->box1, camera1Matrix_eigen, radarToCamera1Matrix);
    pub_space_fusion.publish(*pub_msg);

    cv_bridge::CvImageConstPtr ptr = cv_bridge::toCvCopy(msg->camera_data.rosImage[0], sensor_msgs::image_encodings::BGR8);
    Mat src = ptr->image.clone();
    for(auto itptr = pub_msg->box0.begin(); itptr != pub_msg->box0.end(); itptr++)
    {
        circle(src, Point(itptr->centper_pt_x, itptr->centper_pt_y), 2, Scalar(0,0,255), 2);
        Rect rect(itptr->centper_pt_x - itptr->reference_1m / 2, itptr->centper_pt_y - itptr->reference_1m / 2, itptr->reference_1m, itptr->reference_1m);
        rectangle(src, rect, Scalar(0,0,255), 1);
    }
    imshow("src", src);
    waitKey(5);
}

int main(int argc, char **argv)
{
    ReadInCameraRadarCalibrationResult(camera0_radar_calibration_result, camera0_radar_dx, camera0_radar_dy, camera0_radar_dz, camera0_radar_tx, camera0_radar_ty, camera0_radar_tz);
    cout<<camera0_radar_dx<<","<<camera0_radar_dy<<","<< camera0_radar_dz<<","<<camera0_radar_tx<<","<<camera0_radar_ty<<","<<camera0_radar_tz<<endl;

    CalRadarToCameraMatrix(radarToCamera0Matrix, camera0_radar_dx, camera0_radar_dy, camera0_radar_dz, camera0_radar_tx, camera0_radar_ty, camera0_radar_tz);

    ReadInCameraCalibrationResult(camera0_calibration_result, camera0Matrix_eigen);
    cout<<setprecision(10)<<camera0Matrix_eigen<<endl;

    ros::init(argc, argv, "CameraRadarSpaceFusionRidgeDetection");
    ros::NodeHandle n;
    pub_space_fusion = n.advertise<mymsg::CameraRadarSpaceFusion>("camera_radar_space_fusion", 20);
    ros::Subscriber sub = n.subscribe("camera_radar_time_fusion", 10, CallBack);
    ros::spin();

    return 0;
}
