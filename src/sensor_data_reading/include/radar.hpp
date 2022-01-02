#ifndef  _RADAR_H_
#define _RADAR_H_

#include "ros/ros.h"
#include <iostream>
#include "mymsg/RadarData.h"
#include <string>
#include "../include/socket_can.hpp"

using namespace std;

class radarDataReading
{
public:
    radarDataReading(string topic_name, int input_radar_id)
    {
        pub_radar = n.advertise<mymsg::RadarData>(topic_name,1000);
        num_points = -1;//防止出现先给雷达通电，再开程序，从而导致没有先收到0x600或0x60A就收到0x60B和0x701
        radar_id = input_radar_id;
    }
    void ReadCAN(struct can_frame frame);
    void Get0x60A(struct can_frame frame);
    void Get0x60B(struct can_frame frame);
    void Get0x600(struct can_frame frame);
    void Get0x701(struct can_frame frame);
    ~radarDataReading(){};
private:
    static socket_can::SocketCAN can1;
    ros::NodeHandle n;
    ros::Publisher pub_radar;
    ros::Time stamp;
    int num_points;
    int radar_id;
};

#endif