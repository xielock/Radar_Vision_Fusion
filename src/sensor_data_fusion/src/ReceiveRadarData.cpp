#include"ros/ros.h"
#include<iostream>
#include"mymsg/RadarData.h"
#include"mymsg/RadarDataPerSec.h"

using namespace std;

void CallBack(const mymsg::RadarDataPerSecConstPtr &msg)
{
    cout<<msg->radar_data_per_sec.size()<<endl;
}

int main(int argc, char **argv)
{
    ros::init(argc, argv, "ReceiveRadarData");
    ros::NodeHandle n;
    ros::Subscriber sub;
    sub = n.subscribe("radar_data", 20, &CallBack);
    ros::spin();
    return 0;
}