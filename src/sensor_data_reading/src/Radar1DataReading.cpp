#include "../include/radar.hpp"
#include <ros/ros.h>

using namespace std;

int main(int argc, char **argv)
{
    ros::init(argc, argv, "Radar1DataReading");
    radarDataReading radarRead("radar1", 1);
    while (ros::ok())
    {
       struct can_frame frame;
       radarRead.ReadCAN(frame);
       ros::spinOnce();
    }
    return 0;
}