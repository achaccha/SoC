#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <math.h>

#include "amazon2_sdk.h"
#include "graphic_api.h"
#include "num.h"

//로봇 제어
#include <getopt.h>
#include "uart_api.h"
#include "robot_protocol.h"
#include <termios.h>

static struct termios inittio, newtio;


//#define YELLOW_BRIDGE_BEFORE_THE_STAIRS 0//8번 장애물
//#define YELLOW_BRIDGE_ON_THE_BRIDGE 1
//#define YELLOW_BRIDGE_DOWN 2
//#define YELLOW_BRIDGE_NEXT_OBSTACLE 3



U8 CLOSE_TO_YELLOW_Bridge(U16* imageIn, U8 order) {
	U8 close_to_bridge_Y[2] = { 1,119 };
	U8 bridge_X[100] = { 0, }, bridge_Y[100] = { 0, };
	U8 Y_B_cnt = 0;

	U8 first, last;
	U8 is_first = 1;

	U8 y, x;
	U16 yellow_cnt = 0;

	if (order == YELLOW_BRIDGE_BEFORE_THE_STAIRS) {
		for (y = close_to_bridge_Y[0]; y < close_to_bridge_Y[1]; y += 2)
		{
			is_first = 1;
			yellow_cnt = 0;

			for (x = 1; x < WIDTH - 1; x += 2)
			{
				if ((GetPtr(imageIn, y - 1, x - 1, WIDTH) == RGB565YELLOW ? 1 : 0) + (GetPtr(imageIn, y - 1, x, WIDTH) == RGB565YELLOW ? 1 : 0)
					+ (GetPtr(imageIn, y - 1, x + 1, WIDTH) == RGB565YELLOW ? 1 : 0)
					+ (GetPtr(imageIn, y, x - 1, WIDTH) == RGB565YELLOW ? 1 : 0) + (GetPtr(imageIn, y, x, WIDTH) == RGB565YELLOW ? 1 : 0)
					+ (GetPtr(imageIn, y, x + 1, WIDTH) == RGB565YELLOW ? 1 : 0)
					+ (GetPtr(imageIn, y + 1, x - 1, WIDTH) == RGB565YELLOW ? 1 : 0) + (GetPtr(imageIn, y + 1, x, WIDTH) == RGB565YELLOW ? 1 : 0)
					+ (GetPtr(imageIn, y + 1, x + 1, WIDTH) == RGB565YELLOW ? 1 : 0) > 5)
				{
					yellow_cnt++;
					if (is_first) { is_first = 0; first = x; }
					last = x;
				}
			}
			if (yellow_cnt > 20) {
				bridge_Y[Y_B_cnt] = y;
				bridge_X[Y_B_cnt++] = last;
			}
		}
		if (Y_B_cnt > 3) {
			float avg_top_X = 0;
			for (y = 0; y < 3; y++) {
				avg_top_X += bridge_X[y];
			}
			avg_top_X /= 3.0;


			if (bridge_Y[Y_B_cnt - 1] < 80) {

				if (avg_top_X - 10 > bridge_X[Y_B_cnt - 1]) {
					RobotAction(M_turn_left_30);

				}
				else if (avg_top_X + 10 < bridge_X[Y_B_cnt - 1]) {
					RobotAction(M_turn_right_30);
				}
				else {
					RobotAction(M_walk_forward_good_5);
				}
			}

			else if (bridge_X[Y_B_cnt - 1] >175) {
				sleep(2);
				RobotAction(M_walk_right_small);
				sleep(1);
				RobotAction(M_walk_forward_good_5);
			}
			else if (bridge_X[Y_B_cnt - 1] < 165) {
				sleep(2);
				RobotAction(M_walk_left_small);
				sleep(1);
				RobotAction(M_walk_forward_good_5);

			}
			else if (bridge_Y[Y_B_cnt - 1] < 100) {
				RobotAction(M_walk_forward_good_5);
			}
			else {
				sleep(2);
				RobotAction(M_walk_forward_good_5);
				sleep(2);
				RobotAction(M_up_2cm);
				return YELLOW_BRIDGE_ON_THE_BRIDGE;
			}
		}
		else RobotAction(M_walk_forward_good_5);

	}
	return YELLOW_BRIDGE_BEFORE_THE_STAIRS;
}

U8 IN_YELLOW_Bridge(U16* imageIn, U8 order) {
	int Y_B_cnt = 0;
	U8 Y_length = 2;
	U8 first, is_first = 1;
	U8 F = 0, L = 0;
	float bridge_X[100] = { 0, }, bridge_Y[100] = { 0, };
	//좌표 찾기
	U8 y, x;
	U8 yellow_cnt = 0;
	if (order == YELLOW_BRIDGE_ON_THE_BRIDGE) {
		for (x = 30; x < WIDTH - 30; x += 2) {
			is_first = 1; yellow_cnt = 0;
			for (y = 1; y < HEIGHT - 20; y++)
			{
				if ((GetPtr(imageIn, y - 1, x - 1, WIDTH) == RGB565YELLOW ? 1 : 0) + (GetPtr(imageIn, y - 1, x, WIDTH) == RGB565YELLOW ? 1 : 0)
					+ (GetPtr(imageIn, y - 1, x + 1, WIDTH) == RGB565YELLOW ? 1 : 0)
					+ (GetPtr(imageIn, y, x - 1, WIDTH) == RGB565YELLOW ? 1 : 0) + (GetPtr(imageIn, y, x, WIDTH) == RGB565YELLOW ? 1 : 0)
					+ (GetPtr(imageIn, y, x + 1, WIDTH) == RGB565YELLOW ? 1 : 0)
					+ (GetPtr(imageIn, y + 1, x - 1, WIDTH) == RGB565YELLOW ? 1 : 0) + (GetPtr(imageIn, y + 1, x, WIDTH) == RGB565YELLOW ? 1 : 0)
					+ (GetPtr(imageIn, y + 1, x + 1, WIDTH) == RGB565YELLOW ? 1 : 0) > 4)
				{
					yellow_cnt++;
					if (is_first) { is_first = 0; first = y; }
				}
			}
			if (((float)(yellow_cnt) / (float)((float)HEIGHT - 20.0)) > 0.1)
			{
				//if (x < 60 && first < 3) F++;
				//else if (x > 150 && first < 6) L++;
				bridge_X[Y_B_cnt] = x;
				bridge_Y[Y_B_cnt++] = first;
			}
		}

		float avg_Y = 0;

		if (Y_B_cnt > 5) {
			int slope = bridge_Y[Y_B_cnt - 1] - bridge_Y[0];
			for (x = Y_B_cnt / 2 - 2; x < Y_B_cnt / 2 + 2; x++) {
				avg_Y += bridge_Y[x];
			}
			avg_Y /= 4;


			if (bridge_X[Y_B_cnt - 1] <110) {
				sleep(1);
				RobotAction(M_walk_left_large);
				sleep(2);
				RobotAction(M_walk_forward_good_5);//5번 걷기
				sleep(2);
				RobotAction(M_down_2cm);
				return YELLOW_BRIDGE_DOWN;
			}
			if (slope>20) {
				RobotAction(Y_turn_right);
				sleep(1);
			}
			else if (slope <-5) {
				RobotAction(Y_turn_left);
				sleep(1);
			}
			else if (avg_Y < 30) {
				RobotAction(Y_walk_left);
			}
			else if (avg_Y > 35) {
				RobotAction(Y_walk_right);
			}
			else {
				RobotAction(M_walk_forward_good_5);
			}
		}
		else { RobotAction(M_walk_forward_good_5); }
		return YELLOW_BRIDGE_ON_THE_BRIDGE;
	}
}

U8 first = 1;
U8 DOWN_YELLOW_Bridge(U16* imageIn, U8 order) {

	if (first) {
		RobotAction(M_base);
		first = 0;
	}

	float yellow_Y[40] = { 0, }, yellow_X[40] = { 0, };

	U8 B_cnt = 0;
	U8 first;
	U8 is_first = 1;
	U8 x, y;
	U8 yellow_cnt = 0;

	//좌표 찾기
	if (order == YELLOW_BRIDGE_DOWN) {
		for (x = 1; x < WIDTH - 1; x += 5)
		{
			is_first = 1; yellow_cnt = 0;
			for (y = 1; y < HEIGHT - 1; y += 2)
			{
				if ((GetPtr(imageIn, x - 1, y - 1, WIDTH) == RGB565YELLOW ? 1 : 0) + (GetPtr(imageIn, x - 1, y, WIDTH) == RGB565YELLOW ? 1 : 0)
					+ (GetPtr(imageIn, x - 1, y + 1, WIDTH) == RGB565YELLOW ? 1 : 0)
					+ (GetPtr(imageIn, x, y - 1, WIDTH) == RGB565YELLOW ? 1 : 0) + (GetPtr(imageIn, x, y, WIDTH) == RGB565YELLOW ? 1 : 0)
					+ (GetPtr(imageIn, x, y + 1, WIDTH) == RGB565YELLOW ? 1 : 0)
					+ (GetPtr(imageIn, x + 1, y - 1, WIDTH) == RGB565YELLOW ? 1 : 0) + (GetPtr(imageIn, x + 1, y, WIDTH) == RGB565YELLOW ? 1 : 0)
					+ (GetPtr(imageIn, x + 1, y + 1, WIDTH) == RGB565YELLOW ? 1 : 0) > 4)
				{
					yellow_cnt++;
					if (is_first) { is_first = 0; first = y; }
				}
			}
			if (((float)(yellow_cnt * 2) / (float)HEIGHT) > 0.1)
			{
				yellow_X[B_cnt] = x;
				yellow_Y[B_cnt++] = first;
			}
		}

		//기울기 구하기
		float slope[41] = { 0, };
		float avr_slope = 0;
		for (x = 0; x < B_cnt - 1; x++) {
			slope[x] = (float)(yellow_Y[x + 1] - yellow_Y[x]) / (float)(yellow_X[x + 1] - yellow_X[x]);
			avr_slope += slope[x];
		}
		avr_slope /= (float)(B_cnt);


		if (avr_slope > 0.35) {
			RobotAction(M_turn_left_30);
		}
		else if (avr_slope < -0.35) {
			RobotAction(M_turn_right_30);
		}
		else if (yellow_Y[B_cnt - 1] < 100) {
			RobotAction(M_huro_walk_forward_in_competition);
		}
		else {
			RobotAction(M_down_2cm);
			return YELLOW_BRIDGE_NEXT_OBSTACLE;
		}
	}

	return 	YELLOW_BRIDGE_DOWN;
}


U8 Hidden_Yellow_Bridge(U16* imageIn, U8 order) {

	U8 Bridge_half_LENGTH = 70; //^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^조정필요
	U8 Y_length = 1; // 5 칸마다 점 검사

					 //좌표 찾기
	float bridge_X[40] = { 0, }, bridge_Y[40] = { 0, };
	int first = 0, last = 0;
	U8 Y_B_cnt = 0, cnt = 0;
	U8 is_first = 1;

	U8 y, x;
	U8 FIRST = 0, LAST = 0;

	for (y = 0; y < 50; y += Y_length)
	{
		is_first = 1;
		for (x = 1; x < WIDTH - 1; x += 2)
		{
			if ((GetPtr(imageIn, y - 1, x - 1, WIDTH) == RGB565YELLOW ? 1 : 0) + (GetPtr(imageIn, y - 1, x, WIDTH) == RGB565YELLOW ? 1 : 0)
				+ (GetPtr(imageIn, y - 1, x + 1, WIDTH) == RGB565YELLOW ? 1 : 0)
				+ (GetPtr(imageIn, y, x - 1, WIDTH) == RGB565YELLOW ? 1 : 0) + (GetPtr(imageIn, y, x, WIDTH) == RGB565YELLOW ? 1 : 0)
				+ (GetPtr(imageIn, y, x + 1, WIDTH) == RGB565YELLOW ? 1 : 0)
				+ (GetPtr(imageIn, y + 1, x - 1, WIDTH) == RGB565YELLOW ? 1 : 0) + (GetPtr(imageIn, y + 1, x, WIDTH) == RGB565YELLOW ? 1 : 0)
				+ (GetPtr(imageIn, y + 1, x + 1, WIDTH) == RGB565YELLOW ? 1 : 0) > 4)
			{
				if (is_first) { is_first = 0; first = x; }
				last = x;
			}
			if (is_first == 0) { x = 100; }
		}
		if (first < 3) FIRST = y;
		if (last > 177) LAST = y;
		if (first < 3 && last>177) y = 50;
	}
	if (CNT > 8) { RobotAction(M_walk_forward_good_5); sleep(2); RobotAction(M_YELLOW_bridge); CNT = 0; return 1; }
	else if (LAST > 3) { RobotAction(M_walk_right_small); }
	else if (FIRST > 3) { RobotAction(M_walk_left_small); }
	else if (LAST + 5 < FIRST) { RobotAction(M_walk_left_small); }
	else if (LAST - 5 > FIRST) { RobotAction(M_walk_right_small); }
	else { RobotAction(M_walk_forward_good_5); sleep(2); RobotAction(M_YELLOW_bridge); return 1; }
	sleep(1);
	RobotAction(M_walk_forward_good_5);
	printf("%d,%d\n", FIRST, LAST);

	CNT++;
	return 0;

}


U8 IN_Yellow_bridge_Crawling(U16* imageIn, U8 order)//구멍에 머리박고 기어간다는 가정, order == 0은 계단 아래, order == 1은 계단 위에서
{
	U8 Width_length[5] = { 25,35,60,35,25 };//35는 각각 영상의 왼쪽끝과 오른쪽끝(여기에 있는 노란색은 신경 안쓸거)
	U8 Height_length[2] = { 90,30 };//90이 영상 위쪽, 30이 영상 아래쪽
	U16 Yellow_count[4] = { 0,0,0,0 };//인덱스 [1]은 가운데 위쪽, [2]는 가운데 아래쪽

	U8  y, x;
	for (y = 0; y < HEIGHT; y++) // 입력 영상의 세로 방향
	{
		for (x = Width_length[0]; x < Width_length[0] + Width_length[1] + Width_length[2] + Width_length[3]; x++) // 입력 영상의 가로 방향
		{
			if (GetPtr(imageIn, y, x, WIDTH) == RGB565YELLOW)
			{
				if (x < Width_length[0] + Width_length[1])
					Yellow_count[0]++;
				else if (x < Width_length[0] + Width_length[1] + Width_length[2])
				{
					if (y < Height_length[0])
						Yellow_count[1]++;
					else
						Yellow_count[2]++;
				}
				else
					Yellow_count[3]++;
			}
		}
	}

	if ((float)Yellow_count[2] / (float)(Width_length[2] * Height_length[1]) < 0.8)
	{
		RobotAction(M_walk_forward_1);
		return YELLOW_BRIDGE_BEFORE_THE_STAIRS;
	}

	if ((float)Yellow_count[0] / (float)Yellow_count[3] < 0.8)//왼쪽에 가까이 있으니까 오른쪽으로
	{
		RobotAction(M_walk_right_small);
		return order;
	}
	else if ((float)Yellow_count[0] / (float)Yellow_count[3] > 1.2)//오른쪽에 가까이 있으니까 왼쪽으로
	{
		RobotAction(M_walk_left_small);
		return order;
	}
	else//중심
	{
		if (order == YELLOW_BRIDGE_BEFORE_THE_STAIRS)
		{
			RobotAction(M_up_2cm);//계단 오르기*****************************
			return YELLOW_BRIDGE_ON_THE_BRIDGE;
		}
		else if (order == YELLOW_BRIDGE_ON_THE_BRIDGE)
		{
			if ((float)Yellow_count[2] / ((float)Width_length[2] * Height_length[2]) > 0.5)//앞에구멍이 있으나 아직 여유 있어서 한 걸음 전진
			{
				RobotAction(M_walk_forward_good_5);
				return YELLOW_BRIDGE_BEFORE_THE_STAIRS;
			}
			else
			{
				RobotAction(M_YELLOW_bridge);//기어가기
				return YELLOW_BRIDGE_NEXT_OBSTACLE;
			}
		}
	}

}

