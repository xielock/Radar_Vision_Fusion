//若仅使用单个相机，用此程序改变起话题名称
//修改对上层程序不可见
//不要改这个节点话题名称，双相机用的也是这个名字
#include"ros/ros.h"
#include<iostream>
#include<image_transport/image_transport.h>
#include<cv_bridge/cv_bridge.h>
#include<sensor_msgs/image_encodings.h>
#include<opencv2/opencv.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include<opencv2/highgui/highgui.hpp>
#include"mymsg/CameraDataPerSec.h"

using namespace std;
using namespace cv;

ros::Publisher pub;

void CallBack(const sensor_msgs::ImageConstPtr &msg)
{
    mymsg::CameraDataPerSec *pub_msg = new mymsg::CameraDataPerSec;
    pub_msg->rosImage.push_back(*msg);
    pub.publish(*pub_msg);
    cout<<"Trans camera0 topic name: "<<msg->header.stamp<<endl;
    delete pub_msg;
}

int main(int argc, char **argv)
{
    ros::init(argc, argv, "TransOneCameraTopicName");
    ros::NodeHandle n;
    ros::Subscriber sub;
    //不要改这个话题名字
    pub = n.advertise<mymsg::CameraDataPerSec>("camera_data", 20);
    sub = n.subscribe("camera0", 20, &CallBack);
    ros::spin();
    return 0;
}