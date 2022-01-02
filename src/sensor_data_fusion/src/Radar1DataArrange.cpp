#include"../include/RadarDataArrange.hpp"

using namespace std;

int main(int argc, char **argv)
{
    ros::init(argc, argv, "Radar1DataArrange");
    RadarArrange radar0("radar1", "radar1_data_arrange");
    return 0;
}