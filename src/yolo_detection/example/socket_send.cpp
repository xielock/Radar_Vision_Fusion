#include"socket_send.h"

using namespace std;

void socket_can::send_can_message(int id, double dislong ,double dislat, double confidence, int type)
{	
	int obs_ID = id;
	int transobs_x = dislong/resolution_dis;
	int transobs_y = (dislat + bias)/resolution_dis;
	int trans_confi = confidence/ resolution_con;
	int obs_type = type;


	int a[100]= {0};
   	for(int j=0; obs_ID > 0; j++)
	{
	    a[j] = obs_ID %2;
	    obs_ID /=2;
	}
	int b[100] = {0};
	for(int j=0; transobs_x> 0;  j++)
	{
	    b[j] = transobs_x %2;
	    transobs_x /=2;
	}
	int c[100] = { 0 };
	for (int j = 0; transobs_y > 0; j++)
	{
		c[j] = transobs_y % 2;
		transobs_y /= 2;
	}
	int d[100] = { 0 };
	for (int j = 0; trans_confi > 0; j++)
	{
		d[j] = trans_confi % 2;
		trans_confi /= 2;
	}
	int e[100] = { 0 };
	for (int j = 0; obs_type > 0; j++)
	{
		e[j] = obs_type % 2;
		obs_type /= 2;
	}
	 
	frame.data[0] = 0x00;
	frame.data[1] = 0x00;
    	frame.data[2]= 0x00;
	frame.data[3] = 0x00;
	frame.data[5] = 0x00;
	frame.data[6] = 0x00;
	frame.data[7] = 0x00;
	frame.data[4] = 0x00;
	for(int i = 0; i< 8; i++)
	{
		frame.data[0] |= a[i]<<i;
	}
	for(int i = 0; i< 8; i++)
	{
		frame.data[1] |= b[i+5]<<i;
	}
	for (int i = 0; i < 3; i++)
	{
		frame.data[2] |= c[i + 8] << i;
	}
	for (int i = 3; i < 8; i++)
	{
		frame.data[2] |= b[i-3] << i;
	}
	for (int i = 0; i < 8; i++)
	{
		frame.data[3] |= c[i] << i;
	}
	for (int i = 0; i < 8; i++)
	{
		frame.data[4] |= d[i] << i;
	}
	for (int i = 0; i < 8; i++)
	{
		frame.data[5] |= e[i] << i;
	}

    	frame.can_dlc = 8;
    	frame.can_id = 0x0CFF451D;
    	int nbyte = sendto(socket_, &frame, sizeof(can_frame), 0, (sockaddr*)&addr, sizeof(addr));
}
   
