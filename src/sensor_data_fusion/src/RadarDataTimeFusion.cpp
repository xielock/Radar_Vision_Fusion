#include<ros/ros.h>
#include"../include/TwoRadarTimeFusion.hpp"

using namespace std;

int main(int argc, char **argv)
{
    ros::init(argc, argv, "RadarDataTimeFusion");
    MultiRadarTimeFusion multi_radar_time_fusion("radar0_data_arrange", "radar1_data_arrange", "radar0_radar1_time_fusion");
    ros::MultiThreadedSpinner s(4);
    s.spin();
    return 0;
}