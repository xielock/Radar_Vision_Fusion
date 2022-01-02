#include"../include/RadarDataArrange.hpp"

using namespace std;

void RadarArrange::CallBackArrange(const mymsg::RadarDataConstPtr &msg)
{
    if(data_per_frame.radar_data_per_sec.empty())//第一次接受数据，初始化
    {
        data_per_frame.radar_data_per_sec.push_back(*msg);
        stamp_before_frame = msg->header.stamp;
    }
    else
    {
        if(msg->header.stamp == stamp_before_frame)
        {
            data_per_frame.radar_data_per_sec.push_back(*msg);
        }
        else
        {
            pub.publish(data_per_frame);
            cout<<((data_per_frame.radar_data_per_sec[0].num_objects)>>0)<<":";
            for(int i = 0; i < data_per_frame.radar_data_per_sec.size(); ++i)
                cout<<((data_per_frame.radar_data_per_sec[i].id)>>0)<<" ";
            cout<<endl;
            data_per_frame.radar_data_per_sec.clear();
            data_per_frame.radar_data_per_sec.push_back(*msg);
            stamp_before_frame = msg->header.stamp;
        }
    }
    return;
}