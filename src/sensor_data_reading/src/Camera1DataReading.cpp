#include"ros/ros.h"
#include<iostream>
#include"../include/camera.hpp"

using namespace std;
using namespace cv;

int main(int argc,char**argv){
    ros::init(argc, argv, "Camera1DataReading");
    camera_driver camera("camera1", 1);//神仙，插两个相机有1~4共4个号，一个相机对应两个号
    while(ros::ok())
    {
        camera.image_publish();
        ros::spinOnce();
    }
    return 0;
}