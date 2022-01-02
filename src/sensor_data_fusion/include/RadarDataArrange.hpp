//将雷达的原始数据整合
//整合结果为一帧中包含多少目标点及对应目标点信息
#ifndef _RADAR_DATA_ARRANGE_H_
#define _RADAR_DATA_ARRANGE_H_

#include<iostream>
#include"ros/ros.h"
#include"mymsg/RadarData.h"
#include"mymsg/RadarDataPerSec.h"

using namespace std;

class RadarArrange
{
private:
    string sub_topic_name;
    string pub_topic_name;
    ros::Publisher pub;
    mymsg::RadarDataPerSec data_per_frame;
    ros::Time stamp_before_frame;
    void CallBackArrange(const mymsg::RadarDataConstPtr &msg);
    ros::NodeHandle n;
    ros::Subscriber sub;
public:
    RadarArrange(string in_topic_name, string out_topic_name)
    {
        pub_topic_name = out_topic_name;
        sub_topic_name = in_topic_name;
        pub = n.advertise<mymsg::RadarDataPerSec>(pub_topic_name, 20);
        sub = n.subscribe(sub_topic_name, 20, &RadarArrange::CallBackArrange, this);
        ros::spin();
        return;
    }
};

#endif