#ifndef _TWO_RADAR_TIME_FUSION_
#define _TWO_RADAR_TIME_FUSION_

//若使用双雷达的方式，利用此程序将两雷达的数据进行时间对齐
//切记将radar1连接到radar0后面的电源接口上！！！
//这样可以保证radar0先于radar1启动
//程序默认radar0的数据早于radar1到来
#include<ros/ros.h>
#include<iostream>
#include<mutex>
#include<vector>
#include"mymsg/RadarData.h"
#include"mymsg/RadarDataPerSec.h"
#include"mymsg/RadarDataTimeFusion.h"

using namespace std;

class MultiRadarTimeFusion
{
private:
    ros::NodeHandle n;
    ros::Subscriber sub_radar0;
    ros::Subscriber sub_radar1;
    ros::Publisher pub;
    vector<mymsg::RadarDataPerSec> cache_radar0;//正常情况是radar0的数据先来，radar1的数据紧随其后
    mutex mutex_radar0;
public:
    MultiRadarTimeFusion(string radar0_topic_name, string radar1_topic_name, string pub_topic_name)
    {
        pub = n.advertise<mymsg::RadarDataTimeFusion>(pub_topic_name, 20);
        sub_radar0 = n.subscribe(radar0_topic_name, 20, &MultiRadarTimeFusion::CallBackRadar0,this);
        sub_radar1 = n.subscribe(radar1_topic_name, 20, &MultiRadarTimeFusion::CallBackRadar1,this);
    }
    void CallBackRadar0(const mymsg::RadarDataPerSecConstPtr &msg);
    void CallBackRadar1(const mymsg::RadarDataPerSecConstPtr &msg);
};

#endif