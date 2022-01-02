#include"ros/ros.h"
#include<iostream>
#include<stdio.h>
#include<vector>
#include<mutex>
#include <thread>
#include<memory>
#include<image_transport/image_transport.h>
#include<cv_bridge/cv_bridge.h>
#include<sensor_msgs/image_encodings.h>
#include<opencv2/opencv.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<mymsg/CameraDataPerSec.h>
#include<mymsg/RadarData.h>
#include<mymsg/RadarDataPerSec.h>
#include<mymsg/CameraRadarTimeFusion.h>

using namespace std;
using namespace cv;

class TimeFusion
{
private:
    ros::NodeHandle n;
    ros::Subscriber sub_radar;
    ros::Subscriber sub_camera;
    ros::Publisher pub_time_fusion;
    vector<mymsg::RadarDataPerSec> cache_radar;
    mutex mutex_radar;
public:
    TimeFusion()
    {
        pub_time_fusion = n.advertise<mymsg::CameraRadarTimeFusion>("camera_radar_time_fusion", 10);
        sub_radar = n.subscribe("radar_data",20,&TimeFusion::Callback_radar,this);
        sub_camera = n.subscribe("camera_data",1,&TimeFusion::Callback_camera,this);
    }
    void Callback_radar(const mymsg::RadarDataPerSecConstPtr &msg_radar);
    void Callback_camera(const mymsg::CameraDataPerSecConstPtr &msg_camera);
};

//相机频率低，认为相机数据与前一帧雷达数据形成时间融合
void TimeFusion::Callback_radar(const mymsg::RadarDataPerSecConstPtr &msg_radar)
{
    lock_guard<mutex> lk(mutex_radar);
    if(cache_radar.size() >= 1)
        cache_radar.clear();
    cache_radar.push_back(*msg_radar);
}

void TimeFusion::Callback_camera(const mymsg::CameraDataPerSecConstPtr &msg_camera)
{
    unique_ptr<mymsg::CameraRadarTimeFusion> msg(new mymsg::CameraRadarTimeFusion);
    {
        lock_guard<mutex> lk(mutex_radar);
        if(cache_radar.empty())
            return;
        msg->radar_data = cache_radar[0];
        cache_radar.clear();
    }
    msg->camera_data = *msg_camera;
    pub_time_fusion.publish(*msg);
    cout<<"radar: "<<msg->radar_data.radar_data_per_sec[0].header.stamp<<" ;camera: "<<msg->camera_data.rosImage[0].header.stamp<<endl;
}

int main(int argc, char **argv)
{
    ros::init(argc, argv, "CameraRadarTimeFusion");
    TimeFusion time_fusion;
    ros::MultiThreadedSpinner s(4);
    s.spin();
    return 0;
}