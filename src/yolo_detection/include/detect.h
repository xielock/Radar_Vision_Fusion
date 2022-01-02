#pragma once
#ifndef DETECT_H
#define DETECT_H
//trt相关
#include <trt.h>
#include <memory>
#include <getopt.h>
#include <string>
#include <chrono> 
#include<opencv2/opencv.hpp>
#include<algorithm>

//ROS
#include <ros/ros.h>
#include <actionlib/server/simple_action_server.h>
#include <geometry_msgs/Point.h>
#include <image_transport/image_transport.h>
#include <sensor_msgs/Image.h>
#include <sensor_msgs/image_encodings.h>
#include <std_msgs/Header.h>
#include <cv_bridge/cv_bridge.h>


//自定义
#include "mymsg/CameraRadarSpaceFusion.h"
#include "mymsg/CameraDataPerSec.h"
#include "mymsg/SingleBox.h"
#include "yolo_detection/origin_and_result_yolo.h"
#include "yolo_detection/yolo_single.h"
#include "socket_send.h"


class  yoloDetect{
    private:
    ros::NodeHandle n;
    ros::Publisher yolo_pub;
    ros::Subscriber yolo_sub;
    cv_bridge::CvImagePtr frame;
    ros::Time stamp;
    cv::Mat img;
    cv::Mat img_copy;
    float rectangle_width;
    float rectangle_height;
    cv::Point rectangle_pos;
    mymsg::CameraRadarSpaceFusion fusion_data;
    typedef struct{float x,y, w,h; int id, cls; float confi;} box;
    box s_box_copy;
    std::vector<box>vector_boxes;
    std::vector<std::vector<box>>xx_boxes;
    std::vector<std::vector<box>>xx_boxes2;
    long long count = 0;
    long long cur = 0;
    socket_can can_;

    public:
    //constructor
    yoloDetect()
    {
    
    yolo_sub = n.subscribe("camera_radar_space_fusion", 3 , &yoloDetect::callback_yolo,this );
	yolo_pub = n.advertise<yolo_detection::origin_and_result_yolo>("yolo_result",10);
	frame = boost::make_shared<cv_bridge::CvImage>();
	frame->encoding = sensor_msgs::image_encodings::RGB8;
    }
    //deconstructor
    ~yoloDetect(){}
    //callback_yolo
    void callback_yolo( const mymsg::CameraRadarSpaceFusionConstPtr& msg );

};

#endif

