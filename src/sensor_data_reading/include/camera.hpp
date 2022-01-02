#ifndef _CAMERA_H_
#define _CAMERA_H_

#include"ros/ros.h"
#include<iostream>
#include<image_transport/image_transport.h>
#include<cv_bridge/cv_bridge.h>
#include<sensor_msgs/image_encodings.h>
#include<opencv2/opencv.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include<opencv2/highgui/highgui.hpp>

using namespace std;
using namespace cv;

class camera_driver
{
public:
    camera_driver(string topic_name, int video_source = 0):
    cap(video_source){
	//cap.open("/home/nvidia/Desktop/ws/src/sensor_data_reading/video/20211021-160640-748.mp4");
        image_transport::ImageTransport it(n);
        while(ros::ok())
        {
            //判断相机是否打开
            if(!cap.isOpened())
            {
                ROS_ERROR("Cannot open camera!");
                cap = VideoCapture(video_source);
            }
            else
            {
                break;
            }
            
        }

        //初始化相机参数
        cap.set(CV_CAP_PROP_FRAME_WIDTH,800);//宽度 
        cap.set(CV_CAP_PROP_FRAME_HEIGHT, 600);//高度
        cap.set(CV_CAP_PROP_AUTO_EXPOSURE, 0.25) ;//先设置自动曝光模式为手动曝光***，不然无法设置该参数
        cap.set(CV_CAP_PROP_EXPOSURE, 0.03);//曝光，单位是秒
        cap.set(CV_CAP_PROP_FPS, 9);//这个参数目前调不了
        cout<<"camera "<<video_source<<":"<<endl;
        printf("width = %.2f\n",cap.get(CV_CAP_PROP_FRAME_WIDTH));
        printf("height = %.2f\n",cap.get(CV_CAP_PROP_FRAME_HEIGHT));
        printf("fps = %.2f\n",cap.get(CV_CAP_PROP_FPS));

        pub_img = it.advertise(topic_name, 10);//设置主题名和缓冲区，注意消息订阅者订阅主题名应该和这个一样
        frame = boost::make_shared<cv_bridge::CvImage>();//初始化CvImage智能指针，make_shared更安全高效
        frame->encoding = sensor_msgs::image_encodings::BGR8;//设置ROS图片为BGR且每个像素点用1个字节来表示类似与CV_8U
    }

    //void Callback_camera1_para_set(const sensor_data_reading::RadarData0x60AConstPtr &msg_camera1_para_seting);//接受相机参数设置函数
    void image_publish();//图像发布函数
    void para_set_subscribe();//相机设置参数订阅函数，目前逻辑好像有点蠢
    void Callback_camera1_para_set();//订阅话题的回调函数
    ~camera_driver(){}

private:    
    image_transport::Publisher pub_img;//设置图片的发布者
    cv_bridge::CvImagePtr frame;//设置cvImage的智能指针
    VideoCapture cap;//设置存放摄像头图像的变量
    ros::NodeHandle n;
    ros::Subscriber sub_camera1_para;//接受相机参数设置话题
};

#endif
