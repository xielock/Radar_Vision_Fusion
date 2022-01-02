//
// Created by xielock on 21-4-28.
//


#include"detect.h"

using namespace cv;
using namespace std;


int main(int argc , char**argv)
{
    ros::init(argc, argv, "yolo_detect");
    yoloDetect detector;
    ros::MultiThreadedSpinner s(4);
    ros::spin();
    return 0;
    
}



