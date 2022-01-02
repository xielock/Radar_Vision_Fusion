#pragma once
#include<iostream>
#include<string>
#include<string.h>
#include<vector>
#include<linux/can.h>
#include<sys/socket.h>
#include<linux/can/raw.h>
#include<arpa/inet.h>
#include<net/if.h>
#include<termios.h>
#include<sys/ioctl.h>

#define CAN_EFF_FLAG 0x80000000U //扩展帧的标识
#define CAN_RTR_FLAG 0x40000000U //远程帧的标识
#define CAN_ERR_FLAG 0x20000000U //错误帧的标识，用于错误检查


class socket_can{

private:
    int socket_ = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    ifreq ifr;
    sockaddr_can addr;
    can_frame frame;
    double resolution_dis = 0.2;
    double resolution_con = 0.01;
    int bias = 100;
public:
socket_can(){
    strcpy(ifr.ifr_name, "can0");
    ioctl(socket_, SIOCGIFINDEX, &ifr);//指定 can1    
    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;
    bind(socket_, (struct sockaddr *)&addr, sizeof(addr));
    }

    void send_can_message(int id, double dislong ,double dislat, double confidence, int type);
};

		




