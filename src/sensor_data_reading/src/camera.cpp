#include"ros/ros.h"
#include<iostream>
#include"../include/camera.hpp"

using namespace std;
using namespace cv;

//图像发布
void camera_driver::image_publish()
{ 
    Mat src;
    cap>>src;
    frame->image = src;
    //判断是否获取到图像，若获取到图像，将其转化为ROS图片
    if(!(frame->image.empty()))
    {
        frame->header.stamp = ros::Time::now();//给图片加时间戳
        pub_img.publish(frame->toImageMsg());
    }
	else
	{
		exit(0);
	}
    //imshow("input", src);
    //waitKey(10);
    cout<<"picture:"<<frame->header.stamp<<"; size: "<<(cap.get(CV_CAP_PROP_FRAME_WIDTH))<<"*"<<(cap.get(CV_CAP_PROP_FRAME_HEIGHT))<<endl;
    return ; 
}
