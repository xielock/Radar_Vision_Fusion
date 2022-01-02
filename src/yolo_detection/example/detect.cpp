//
// Created by xielock on 21-4-28.
//


#include"detect.h"

using namespace cv;
using namespace std;


std::string engine = "/home/nvidia/Desktop/ws/src/yolo_detection/model/yolov4.engine";
yolodet::yoloNet net(engine);
std::unique_ptr<float[]> outputData(new float[net.mBindBufferSizes[1]]);



void yoloDetect::callback_yolo(const mymsg::CameraRadarSpaceFusionConstPtr& msg)
{
//取得msg中的rosImage并转化为Mat
    cur ++;
    //cout<<"调用次数 "<<cur<<endl;
    cv_bridge::CvImagePtr cam_image;
    cam_image = cv_bridge::toCvCopy(msg->camera_data.rosImage[0], sensor_msgs::image_encodings::BGR8);
    cam_image->image.copyTo(img);
    fusion_data.box0 = msg->box0;
    unique_ptr<yolo_detection::origin_and_result_yolo> pub_img(new yolo_detection::origin_and_result_yolo);
    unique_ptr<yolo_detection::yolo_single> single_box(new yolo_detection::yolo_single);
    //绘制雷达检测到的点
    for(auto box: fusion_data.box0)
    {
        
        int x = box.centper_pt_x;
        int y = box.centper_pt_y;
        circle(img, Point(x,y), 2,Scalar(255,0,255), -1); 
    }

        
    //开始推断等等
     long long i = 0; 
     double runtime = 0;
     char showText[128] = {0};
     int base_line;

        if(!img.empty())
    {

        auto start = std::chrono::system_clock::now();
        net.infer(img, outputData.get());
        auto end = std::chrono::system_clock::now();
        i++;
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        runtime+=double(duration.count());
        std::cout <<  "cost: "
                  << runtime/i << "  ms"<< std::endl;

        //show img
        float label_scale = img.rows * 0.0009;
        int box_think = (img.rows+img.cols) * .001 ;
        int num_det = static_cast<int>(outputData[0]);
        count += num_det;
        cout<<"current     "<<cur<<endl;
        cout<<num_det<<endl;
        if(cur>100 && count <= 10 )
        {
        cout<<"-------------------------";
        count += 11;
        system("./run_det.sh");
	//usleep(5000000);

        }

        float *temp = outputData.get() + 1;
        for (int  j=0; j<num_det ; ++j){
            //所以temp[3]实际上就是我们的y啦
            //cv::rectangle(img,cv::Point(temp[0],temp[1]),cv::Point(temp[2],temp[3]),cv::Scalar(255,0,0),2);
            //temp[0]  左上 x  temp[1] 左上y   temp[2] 右下x  temp[3] 右下y 
            //前两个数即中心点
            //使用公共数据集抑制一下
            //vector_boxes为一帧当中的检测结果
            if(temp[5]!=0&& temp[5]!= 7 && temp[5]!= 8 &&temp[5]!=1)
            continue;
            s_box_copy.x = (temp[0]+temp[2])/2;
            s_box_copy.y = (temp[1]+temp[3])/2 ;
            s_box_copy.w = temp[2] - temp[0] ;
            s_box_copy.h =  temp[3] - temp[1] ; 
            s_box_copy.id = int(temp[5]);
            s_box_copy.confi = temp[4];
            vector_boxes.push_back(s_box_copy);
            temp+=7;
        }

        //雷达框同yolo框的决策融合
        //首先保存yolo之前帧中的检测结果
        //xx_boxes相当于保存多帧当中的结果
        xx_boxes.push_back(vector_boxes);
        vector_boxes.clear();
        if(xx_boxes.size()>15)
        {
        xx_boxes2.push_back(xx_boxes[xx_boxes.size()-5]);
        xx_boxes2.push_back(xx_boxes[xx_boxes.size()-4]);
        xx_boxes2.push_back(xx_boxes[xx_boxes.size()-3]);
        xx_boxes2.push_back(xx_boxes[xx_boxes.size()-2]);
        xx_boxes2.push_back(xx_boxes[xx_boxes.size()-1]);

        xx_boxes.clear();
        xx_boxes = xx_boxes2;
        xx_boxes2.clear();
        // xx_boxes2.clear();
        // copy(xx_boxes.end()-6, xx_boxes.end(), xx_boxes2.begin());
        // swap(xx_boxes2, xx_boxes);
        }


        //决策融合
        box  box_copy2;
        if(xx_boxes.size() >= 5){
        int flag = 0;
        int n_save = 0;
        for(size_t i = xx_boxes.size()-1; i> xx_boxes.size()-5; i--)
        {
            //  获取过去的几帧当中  通过视觉检测出的最多的框数量的情况 并进行记录
            if(n_save < xx_boxes[i].size())
            { 
            n_save = xx_boxes[i].size();
            flag = i;
            }
        }
        //过去五帧中检测出最多的一帧  且>0
        if(flag>0){
        //   cout<<"标志位置   "<<flag<<endl;
        n_save = xx_boxes[flag].size();
        while(n_save> 0)
        {

            if(xx_boxes.size()>1)
            {
                //存储这一帧检测结果中检测出的一个结果
                box_copy2 = xx_boxes[flag][xx_boxes[flag].size()-n_save];
                    
                int flag = 0;
                int distance = INT_MAX;
                int distance_temp;
            for(size_t j = 0; j <fusion_data.box0.size();j++)
            {
                    distance_temp = pow(fusion_data.box0[j].centper_pt_x- box_copy2.x, 2) + pow(fusion_data.box0[j].centper_pt_y- box_copy2.y,2);
                    if(distance_temp< distance)
                    { distance = distance_temp;
                        flag = j;
                    }
            }

                distance  = INT_MAX;
                int x_1 = box_copy2.x;
                int y_1 = box_copy2.y;
                int width = box_copy2.w;
                int height = box_copy2.h;

                int x_2 = fusion_data.box0.size()== 0 ? 0: fusion_data.box0[flag].centper_pt_x;
                int y_2 = fusion_data.box0.size()== 0 ? 0: fusion_data.box0[flag].centper_pt_y;
                float dislong = fusion_data.box0.size()== 10 ? 0: fusion_data.box0[flag].dislong;
                float dislat = fusion_data.box0.size()== 0 ? 0: fusion_data.box0[flag].dislat;
                int id  = fusion_data.box0.size()== 0 ? 100: fusion_data.box0[flag].id;
	
                //保存到ros消息当中
                single_box->type = box_copy2.id;
                single_box->confidence = box_copy2.confi;
                single_box->dislong = dislong;
                single_box->dislat = dislat;
                single_box->id = id;
                pub_img->name_and_dis.push_back( *(single_box) );
		can_.send_can_message(id,dislong, dislat, box_copy2.confi, box_copy2.id);
                sprintf(showText,"cls_%d : %0.2f",box_copy2.id,box_copy2.confi);
                auto size = cv::getTextSize(showText,cv::FONT_HERSHEY_COMPLEX,label_scale,1,&base_line);
                cv::putText(img,showText, cv::Point(x_1 - width/2, y_1 - height/2 - size.height),
                    cv::FONT_HERSHEY_COMPLEX, label_scale , cv::Scalar(0,0,255), box_think*2/3, 8, 0);
                    
                rectangle(img, Point(x_1-width/2, y_1-height/2), Point(x_1 +width/2, y_1 + height/2), Scalar(255,0,255),3);       
                circle(img, Point(x_2, y_2), 4, Scalar(0,255,0), -1, 8);
                putText(img, to_string(dislong), Point(  x_1,  y_1 - size.height), FONT_HERSHEY_COMPLEX, label_scale, Scalar(0,0, 255), box_think*2/3, 8,0);

                n_save--;

            }
        }
    }
}
        pub_img->origin_img = msg->camera_data.rosImage[0];

	    frame->image = img.clone();
	    frame->header.stamp = ros::Time::now();
        pub_img->result_img = *(frame->toImageMsg());
	    yolo_pub.publish(*pub_img);
        //cv::imshow("result",img);
        if((cv::waitKey(1)& 0xff) == 27){
            cv::destroyAllWindows();
            return;
        };

    }


}

