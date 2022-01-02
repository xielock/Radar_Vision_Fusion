#include"../include/RadarDataArrange.hpp"

using namespace std;

int main(int argc, char **argv)
{
    ros::init(argc, argv, "Radar0DataArrange");
    RadarArrange radar0("radar0", "radar0_data_arrange");
    return 0;
}