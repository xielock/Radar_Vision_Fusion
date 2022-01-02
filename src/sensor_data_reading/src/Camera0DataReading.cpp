#include"ros/ros.h"
#include<iostream>
#include"../include/camera.hpp"

using namespace std;
using namespace cv;

int main(int argc,char**argv){
    ros::init(argc, argv, "Camera0DataReading");
    camera_driver camera("camera0", 0);//定义摄像机对象
    //camera_driver camera("camera0", "/home/nvidia/Documents/catkin_ws/src/sensor_data_reading/farm_machine.mp4");
    ros::Rate loop_rate(10);
    while(ros::ok())
    {
        camera.image_publish();
        ros::spinOnce();
	loop_rate.sleep();
    }
    return 0;
}
