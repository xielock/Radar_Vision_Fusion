//将radar0与radar1的数据进行空间融合
#include<iostream>
#include<fstream>
#include<cstdio>
#include<cmath>
#include<algorithm>
#include"ros/ros.h"
#include"mymsg/RadarData.h"
#include"mymsg/RadarDataPerSec.h"
#include"mymsg/RadarDataTimeFusion.h"
#include<opencv2/opencv.hpp>

using namespace std;
using namespace cv;

double dx_radar0, dy_radar0 = 0, tz_radar0;//角度的单位是弧度，距离的单位是m
double dx_radar1, dy_radar1 = 0, tz_radar1;
string result_file_path = "./src/cfg/two_radar_calibration_result.txt";
ros::Publisher pub;
double threshold_distLong = 1.0;
double threshold_distLat = 1.0;
double threshold_vrelLong = 0.1;
double threshold_vrelLat = 0.1;

Mat background = imread("./src/calibration_tools/img/background.jpg");
double max_radar_imag_distance = 20.0;
int radar_imag_width = background.rows;
int radar_imag_height = background.cols;

Point CalShowPoint(const double &x,const double &y)
{
	if((abs(x) > max_radar_imag_distance) || (y > 2*max_radar_imag_distance))//y轴是x轴的两倍
	{
		//ROS_ERROR("OUT OF RANGE");
		return Point(0,0);
	}
	Point temp;
    //发送来的数据，y为distlong；x为distlat且右向为正，这里目前没有确定
	temp.x = (int)(x/max_radar_imag_distance*9/20*radar_imag_width+0.5*radar_imag_width);
	temp.y = (int)(-y/max_radar_imag_distance*9/20*radar_imag_width + 0.95*radar_imag_height);
	return temp;
}

void ReadInCalibrationResult()
{
    ifstream radar_calibration_result(result_file_path);
    if(!radar_calibration_result.is_open())
    {
        cout<<"No radar calibration result!"<<endl;
        exit(0);
    }
    string temp_line;
    getline(radar_calibration_result, temp_line);
    dx_radar0 = stod(temp_line);
    getline(radar_calibration_result, temp_line);
    tz_radar0 = stod(temp_line);
    getline(radar_calibration_result, temp_line);
    dx_radar1 = stod(temp_line);
    getline(radar_calibration_result, temp_line);
    tz_radar1 = stod(temp_line);
    cout<<dx_radar0<<","<<tz_radar0<<endl;
    cout<<dx_radar1<<","<<tz_radar1<<endl;
}

void CallBack(const mymsg::RadarDataTimeFusionConstPtr &msg)
{
    Mat background_clone = background.clone();

    int size0 = msg->multi_radar_time_fusion[0].radar_data_per_sec.size();
    int size1 = msg->multi_radar_time_fusion[1].radar_data_per_sec.size();
    int count_fusion = 0;
    int count_radar0 = 0;
    int count_radar1 = 0;
    //辅助判断radar1中的点是否已经匹配成功，若已匹配则不需要重复匹配
    vector<bool> radar1_visited(size1, false);
    //辅助保存radar1中已经计算过的转换数据，避免重复计算
    vector<vector<double>> radar1_trans_data(size1, vector<double>(4, 50.0));
    mymsg::RadarDataPerSec *msg_pub = new mymsg::RadarDataPerSec;
    int new_id = 0;
    for(int i = 0; i < size0; ++i)
    {
        mymsg::RadarData temp_msg;
        temp_msg.num_objects = -1;//用来表示是融合数据还是单个雷达数据
        temp_msg.id = new_id;
        new_id++;
        auto itptr0 = msg->multi_radar_time_fusion[0].radar_data_per_sec[i];
        //为了保证融合后的数据和雷达原始数据坐标系的一致性
        //这里的坐标系与雷达坐标系一致！！！
        //自定义的雷达坐标系与原始雷达坐标系相比：
        //y轴方向与distlong一致，x轴方向与distlat相反
        double radar0_trans_distLong = (-itptr0.distLat)*sin(tz_radar0) + (itptr0.distLong)*cos(tz_radar0)+dy_radar0;
        //这里前面有个负号是因为自己定义的坐标系的x轴与雷达distlat方向相反
        double radar0_trans_distLat = -((-itptr0.distLat)*cos(tz_radar0) - (itptr0.distLong)*sin(tz_radar0) + dx_radar0);
        double radar0_trans_vrelLong = (-itptr0.vrelLat)*sin(tz_radar0) + (itptr0.vrelLong)*cos(tz_radar0);
        double radar0_trans_vrelLat =  -((-itptr0.vrelLat)*cos(tz_radar0) - (itptr0.vrelLong)*sin(tz_radar0));
        int j = 0;
        for(; j < size1; ++j)
        {
            //雷达设置检测范围为32m
            //用50m作为标志，若radar1对应位置数据为50，说明以前没有转化计算过
            if(!radar1_visited[j])
            {
                if(abs(radar1_trans_data[j][0] - 50.0) < 1e-6)
                {
                    auto itptr1 = msg->multi_radar_time_fusion[1].radar_data_per_sec[j];
                    radar1_trans_data[j][0] = (-itptr1.distLat)*sin(tz_radar1) + (itptr1.distLong)*cos(tz_radar1)+dy_radar1;
                    radar1_trans_data[j][1] = -((-itptr1.distLat)*cos(tz_radar1) - (itptr1.distLong)*sin(tz_radar1) + dx_radar1);
                    radar1_trans_data[j][2] = (-itptr1.vrelLat)*sin(tz_radar1) + (itptr1.vrelLong)*cos(tz_radar1);
                    radar1_trans_data[j][3] = -((-itptr1.vrelLat)*cos(tz_radar1) - (itptr1.vrelLong)*sin(tz_radar1));
                }
            }
            //目前的融合思路只是单纯的判断两个雷达的目标点在虚拟雷达的坐标系中是否距离很近
            if(abs(radar0_trans_distLong - radar1_trans_data[j][0]) < threshold_distLong && abs(radar0_trans_distLat - radar1_trans_data[j][1]) < threshold_distLat)
            {
                //if(abs(radar0_trans_vrelLong - radar1_trans_data[j][2]) < threshold_vrelLong && abs(radar0_trans_vrelLat - radar1_trans_data[j][3]) < threshold_vrelLat)
                {
                    temp_msg.distLong = (radar0_trans_distLong + radar1_trans_data[j][0]) / 2.0;
                    temp_msg.distLat = (radar0_trans_distLat + radar1_trans_data[j][1]) / 2.0;
                    temp_msg.vrelLong = (radar0_trans_vrelLong + radar1_trans_data[j][2]) / 2.0;
                    temp_msg.vrelLat = (radar0_trans_vrelLat + radar1_trans_data[j][3]) / 2.0;
                    radar1_visited[j] = true;//radar1与radar0的数据匹配成功

                    count_fusion++;

                    Point pt = CalShowPoint(-temp_msg.distLat, temp_msg.distLong);
                    circle(background_clone, pt, 2, Scalar(0,255,0), 2);
                    break;
                }
            }
        }
        if(j >= size1)
        {
            temp_msg.distLong = radar0_trans_distLong ;
            temp_msg.distLat = radar0_trans_distLat;
            temp_msg.vrelLong = radar0_trans_vrelLong;
            temp_msg.vrelLat = radar0_trans_vrelLat;

            count_radar0++;

            Point pt = CalShowPoint(-temp_msg.distLat, temp_msg.distLong);
            circle(background_clone, pt, 2, Scalar(0,0,255), 2);
        }
        msg_pub->radar_data_per_sec.push_back(temp_msg);
    }
    //把radar1中没有匹配成功的数据也加入到新消息中
    for(int j = 0; j < size1; ++j)
    {
        if(radar1_visited[j])
            continue;
        mymsg::RadarData temp_msg;
        temp_msg.id = new_id;
        temp_msg.num_objects = -1;//用来表示是融合数据还是单个雷达数据
        new_id++;
        if(abs(radar1_trans_data[j][0] - 50.0) < 1e-6)
        {
            auto itptr1 = msg->multi_radar_time_fusion[1].radar_data_per_sec[j];
            radar1_trans_data[j][0] = (-itptr1.distLat)*sin(tz_radar1) + (itptr1.distLong)*cos(tz_radar1)+dy_radar1;
            radar1_trans_data[j][1] = -((-itptr1.distLat)*cos(tz_radar1) - (itptr1.distLong)*sin(tz_radar1) + dx_radar1);
            radar1_trans_data[j][2] = (-itptr1.vrelLat)*sin(tz_radar1) + (itptr1.vrelLong)*cos(tz_radar1);
            radar1_trans_data[j][3] = -((-itptr1.vrelLat)*cos(tz_radar1) - (itptr1.vrelLong)*sin(tz_radar1));
        }
        temp_msg.distLong = radar1_trans_data[j][0];
        temp_msg.distLat =  radar1_trans_data[j][1];
        temp_msg.vrelLong = radar1_trans_data[j][2];
        temp_msg.vrelLat = radar1_trans_data[j][3];
        radar1_visited[j] = true;//radar1与radar0的数据匹配成功
        msg_pub->radar_data_per_sec.push_back(temp_msg);

        count_radar1++;

        Point pt = CalShowPoint(-temp_msg.distLat, temp_msg.distLong);
        circle(background_clone, pt, 2, Scalar(255,0,0), 2);
    }
    pub.publish(*msg_pub);
    cout<<"input -radar0:"<<size0<<", radar1: "<<size1;
    cout<<" ;output -fusion: "<<count_fusion<<", radar0: "<<count_radar0<<",radar1: "<<count_radar1<<endl;
    //imshow("fusion", background_clone);
    waitKey(5);
    delete msg_pub;
}

int main(int argc, char **argv)
{
    ReadInCalibrationResult();
    namedWindow("fusion", CV_WINDOW_NORMAL);
    ros::init(argc, argv, "RadarDataSpaceFusion");
    ros::NodeHandle n;
    ros::Subscriber sub;
    //不要改这个话题名字，有可能使用单雷达数据，也用的是这个话题名字
    pub = n.advertise<mymsg::RadarDataPerSec>("radar_data", 20);
    sub = n.subscribe("radar0_radar1_time_fusion", 20, &CallBack);
    ros::spin();
    return 0;
}
