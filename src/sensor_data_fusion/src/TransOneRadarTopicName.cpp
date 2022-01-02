//若仅使用单个雷达，用此程序改变起话题名称
//修改对上层程序不可见
//不要改这个节点话题名称，双雷达用的也是这个名字
//单个雷达使用radar0，不用radar1
#include"ros/ros.h"
#include<iostream>
#include"mymsg/RadarData.h"
#include"mymsg/RadarDataPerSec.h"

using namespace std;

ros::Publisher pub;

void CallBack(const mymsg::RadarDataPerSecConstPtr &msg)
{
    pub.publish(*msg);
    cout<<"Trans radar0 topic name: "<<msg->radar_data_per_sec[0].header.stamp<<endl;
}

int main(int argc, char **argv)
{
    ros::init(argc, argv, "TransOneRadarTopicName");
    ros::NodeHandle n;
    ros::Subscriber sub;
    //不要改这个话题名字
    pub = n.advertise<mymsg::RadarDataPerSec>("radar_data", 20);
    sub = n.subscribe("radar0_data_arrange", 20, &CallBack);
    ros::spin();
    return 0;
}