#include "../include/TwoRadarTimeFusion.hpp"
#include<iostream>
#include<ros/ros.h>

using namespace std;

void MultiRadarTimeFusion::CallBackRadar0(const mymsg::RadarDataPerSecConstPtr &msg)
{
    //radar0必须先于radar1启动！
    lock_guard<mutex> lk(mutex_radar0);
    if(!cache_radar0.empty())
        cache_radar0.clear();
    cache_radar0.push_back(*msg);
    return;
}

void MultiRadarTimeFusion::CallBackRadar1(const mymsg::RadarDataPerSecConstPtr &msg)
{
    mymsg::RadarDataTimeFusion *msgFusion = new  mymsg::RadarDataTimeFusion;
    {
        lock_guard<mutex> lk(mutex_radar0);
        if(cache_radar0.empty())
        {
            delete msgFusion;
            return;
        }
        msgFusion->multi_radar_time_fusion.push_back(cache_radar0[0]);
        cache_radar0.clear();
    }
    msgFusion->multi_radar_time_fusion.push_back(*msg);
    pub.publish(*msgFusion);
    cout<<"radar0:"<<msgFusion->multi_radar_time_fusion[0].radar_data_per_sec[0].header.stamp<<","<<"radar1:"<<msgFusion->multi_radar_time_fusion[1].radar_data_per_sec[0].header.stamp<<endl;
    cout<<msgFusion->multi_radar_time_fusion[0].radar_data_per_sec.size()<<", "<<msgFusion->multi_radar_time_fusion[1].radar_data_per_sec.size()<<endl;
    delete msgFusion;
    return;
}