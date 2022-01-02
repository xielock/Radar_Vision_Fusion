#include "../include/radar.hpp"
#include <iostream>
#include "ros/ros.h"

using namespace std;

enum protocol {p_0x60A, p_0x60B, p_0x600, p_0x701, others };

socket_can::SocketCAN radarDataReading::can1("can1");

//从CAN总线读入数据，并根据can_id不同发送不同类型的数据
void radarDataReading::ReadCAN(struct can_frame frame)
{
    can1.read(&frame.can_id, &frame.can_dlc, frame.data);
    protocol flag = others;
    if(frame.can_id == 0x60A || frame.can_id == 0x61A)//目前假设最多装两个雷达
    {
        if(radar_id == (frame.can_id - 1546) / 16)
            flag = p_0x60A;
    }
    else if(frame.can_id == 0x60B || frame.can_id == 0x61B)
    {
        if(radar_id == (frame.can_id - 1547) / 16)
            flag = p_0x60B;
    }
    else if(frame.can_id == 0x600 || frame.can_id == 0x610)
    {
        if(radar_id == (frame.can_id - 1536) / 16)
            flag = p_0x600;
    }
    else if(frame.can_id == 0x701|| frame.can_id == 0x711)
    {
        if(radar_id == (frame.can_id - 1793) / 16)
            flag = p_0x701;
    }
    switch(flag){
			case p_0x60A:
				Get0x60A(frame);
				break;
			case p_0x60B:
				Get0x60B(frame);
				break;
            case  p_0x600:
                Get0x600(frame);
                break;
             case p_0x701:
                Get0x701(frame);
                break;           
			default:
				break;
		}
    return ;
}

//解析并发送0x60A的数据
void radarDataReading::Get0x60A(struct can_frame frame)
{
    stamp = ros::Time::now();
    num_points = frame.data[0]>>0;
    cout<<"radar"<<radar_id;
    cout<<"; there are "<<num_points<<" points:"<<stamp<<endl;
    if(num_points == 0)
    {
        mymsg::RadarData *msg = new mymsg::RadarData;
        msg->header.stamp = stamp;
        msg->num_objects = num_points;
        pub_radar.publish(*msg);
        delete msg;
    }
    return ;
}

//解析并发送0x60B的数据
void radarDataReading::Get0x60B(struct can_frame frame)
{
    if(num_points > -1)
    {
        mymsg::RadarData *msg = new mymsg::RadarData;
        msg->header.stamp = stamp;
        msg->num_objects = num_points;
	    msg->id = frame.data[0]>>0;
	    msg->distLong = ((frame.data[1]<<5)|(frame.data[2]>>3))*0.2-500;
	    msg->distLat = ((((frame.data[2]&0x07)<<8)|(frame.data[3]>>0))*0.2)-204.6;
	    msg->vrelLong = (((frame.data[4]&0xFF)<<2)|(frame.data[5]>>6))*0.25 - 128;
	    msg->vrelLat = (((frame.data[5]&0x3F)<<3)| ((frame.data[6]&0xE0)>>5))*0.25-64; 
        pub_radar.publish(*msg);
        cout<<"id:"<<((msg->id)>>0)<<";"<<((msg->distLong))<<","<<((msg->distLat))<<endl;
        delete msg;      
    }
    return ;
}

void radarDataReading::Get0x600(struct can_frame frame)
{
    stamp = ros::Time::now();
    num_points = (frame.data[0]>>0) + (frame.data[1]>>0);
    cout<<"radar"<<radar_id;
    cout<<"there are "<<num_points<<" points:"<<endl;
    return;
}

void radarDataReading::Get0x701(struct can_frame frame)
{
    if(num_points > -1)
    {
        mymsg::RadarData *msg = new mymsg::RadarData;
        msg->header.stamp = stamp;
        msg->num_objects = num_points;
	    msg->id = frame.data[0]>>0;
	    msg->distLong = ((frame.data[1]<<5)|(frame.data[2]>>3))*0.2-500;
	    msg->distLat = ((((frame.data[2]&0x03)<<8)|(frame.data[3]>>0))*0.2)-102.3;
	    msg->vrelLong = (((frame.data[4]&0xFF)<<2)|(frame.data[5]>>6))*0.25 - 128;
	    msg->vrelLat = (((frame.data[5]&0x3F)<<3)| ((frame.data[6]&0xE0)>>5))*0.25-64;
        pub_radar.publish(*msg);
        cout<<"id:"<<((msg->id)>>0)<<";"<<((msg->distLong))<<","<<((msg->distLat))<<endl;
        delete msg;      
    }
    return ;
}
