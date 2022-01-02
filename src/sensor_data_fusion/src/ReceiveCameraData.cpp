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

void CallBack(const mymsg::CameraDataPerSecConstPtr &msg)
{
    cv_bridge::CvImageConstPtr ptr;
    ptr = cv_bridge::toCvCopy(msg->rosImage[0], sensor_msgs::image_encodings::BGR8);
    cv::Mat src = ptr->image.clone();
    imshow("src",src);
    waitKey(30);
}

int main(int argc, char **argv)
{
    ros::init(argc, argv, "ReceiveCameraData");
    ros::NodeHandle n;
    ros::Subscriber sub;
    sub = n.subscribe("camera_data", 20, &CallBack);
    ros::spin();
    return 0;
}