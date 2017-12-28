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

//#define GREEN_BRIDGE_FIND 0
//#define GREEN_BRIDGE_ON_THE_BRIDGE 1
//#define GREEN_BRIDGE_DOWN 2
//#define GREEN_BRIDGE_NEXT_OBSTACLE 3

U8 CLOSE_TO_GREEN_Bridge(U16* imageIn, U8 order) {
	//RobotAction(M_camera_up);
	RobotAction(M_camera_up_golf);

	U8 nMaskW = 5, nMaskH = 5, hW = nMaskW / 2, hH = nMaskH / 2;

	U8 close_to_bridge_Y[2] = { 40,60 };
	U8 green_bridge_X[40] = { 0, };
	U8 green_bridge_Y[40] = { 0, };
	U8 G_B_cnt = 0;

	U8 first;
	U8 last;
	U8 is_first = 1;

	U8 y, x;
	U16 green_cnt = 0;
	if (order == GREEN_BRIDGE_FIND) {
		for (y = close_to_bridge_Y[0]; y < close_to_bridge_Y[1]; y += 2)
		{
			is_first = 1;
			green_cnt = 0;

			for (x = 1; x < WIDTH - 1; x += 2)
			{
				U8 c, r;
				U16 cnt = 0;
				for (r = 0; r < nMaskH; r++) // 마스크의 세로 방향
				{
					for (c = 0; c < nMaskW; c++) // 마스크의 가로 방향
					{
						if (GetPtr(imageIn, r - hH + y, c - hW + x, WIDTH) == RGB565GREEN)
						{
							cnt++;
							if (is_first) { is_first = 0; first = x; }
							last = x;
						}
					}
				}

				if ((float)cnt / ((float)nMaskH*(float)nMaskW) > 0.8) { green_cnt++;
				GetPtr(imageIn, y, x, WIDTH) = RGB565BLUE;
				}
			}
			if (green_cnt > 20) {
				green_bridge_Y[G_B_cnt] = y; green_bridge_X[G_B_cnt++] = first;
			}
		}
		if (G_B_cnt > 3) {
			float avg_top_X = 0;
			for (y = 0; y < 3; y++) {
				avg_top_X += green_bridge_X[y];
			}
			avg_top_X /= 3.0;


			if (green_bridge_Y[G_B_cnt - 1] < 30) {

				if (avg_top_X - 10 > green_bridge_X[G_B_cnt - 1]) {
					RobotAction(M_turn_left_30);

				}
				else if (avg_top_X + 10 < green_bridge_X[G_B_cnt - 1]) {
					RobotAction(M_turn_right_30);
				}
				else {
					RobotAction(M_walk_forward_good_5);
				}
			}

			else if (green_bridge_X[G_B_cnt - 1] < 30) {
				sleep(1);
				RobotAction(M_walk_left_small);
			}
			else if (green_bridge_X[G_B_cnt - 1] > 50) {
				sleep(1);
				RobotAction(M_walk_right_small);

			}
			else {
				sleep(1);
				RobotAction(M_walk_forward_good_5);
				sleep(1);
				RobotAction(M_up_2cm);
				return GREEN_BRIDGE_ON_THE_BRIDGE;
			}
		}
	}
	sleep(1);
	RobotAction(M_walk_forward_good_5);
	return GREEN_BRIDGE_FIND;
}


U8  bridge_flag = 0;
U8 IN_GREEN_Bridge(U16* imageIn, U8 order) {//, int *firstIn) {


	int G_B_cnt = 0;
	U8 Y_length = 2;
	U8 first, last, is_first = 1;
	U8 F = 0, L = 0;
	float green_bridge_X_first[100] = { 0, }, green_bridge_Y[100] = { 0, };
	//좌표 찾기
	U8 y, x;
	U8 green_cnt = 0;
	if (order == GREEN_BRIDGE_ON_THE_BRIDGE) {
		for (y = 1; y < HEIGHT - 1; y += Y_length) {
			is_first = 1; green_cnt = 0;
			for (x = 1; x < WIDTH - 1; x += 2)
			{
				if ((GetPtr(imageIn, y - 1, x - 1, WIDTH) == RGB565GREEN ? 1 : 0) + (GetPtr(imageIn, y - 1, x, WIDTH) == RGB565GREEN ? 1 : 0)
					+ (GetPtr(imageIn, y - 1, x + 1, WIDTH) == RGB565GREEN ? 1 : 0)
					+ (GetPtr(imageIn, y, x - 1, WIDTH) == RGB565GREEN ? 1 : 0) + (GetPtr(imageIn, y, x, WIDTH) == RGB565GREEN ? 1 : 0)
					+ (GetPtr(imageIn, y, x + 1, WIDTH) == RGB565GREEN ? 1 : 0)
					+ (GetPtr(imageIn, y + 1, x - 1, WIDTH) == RGB565GREEN ? 1 : 0) + (GetPtr(imageIn, y + 1, x, WIDTH) == RGB565GREEN ? 1 : 0)
					+ (GetPtr(imageIn, y + 1, x + 1, WIDTH) == RGB565GREEN ? 1 : 0) > 4)
				{
					green_cnt++;
					if (is_first) { is_first = 0; first = x; }
					last = x;
				}
			}
			if (((float)(green_cnt * 2) / (float)WIDTH) > 0.3)
			{
				green_bridge_Y[G_B_cnt] = y;
				if (first < 10) F++;
				if (last > 170) L++;
				green_bridge_X_first[G_B_cnt++] = first;
			}
		}
	}
	if (green_bridge_Y[0] > 100) {
		bridge_flag = 1;
		sleep(1);
		RobotAction(M_walk_forward_good_5);//5번 걷기
		sleep(2);
		RobotAction(M_down_2cm);
		return GREEN_BRIDGE_DOWN;
	}
	else if (green_bridge_Y[0] > 110 && bridge_flag != 1) {//100
		sleep(1);
		RobotAction(M_walk_forward_good_5);//5번 걷기
		sleep(2);
		RobotAction(M_down_2cm);
		return GREEN_BRIDGE_DOWN;
	}
	else if (F>2) {
		RobotAction(M_turn_left_30);
	}
	else if (L>2) {
		RobotAction(M_turn_right_30);
	}
	else if (G_B_cnt>2 && green_bridge_X_first[G_B_cnt - 1] < 38) {
		RobotAction(M_walk_left_small);
		sleep(1);
	}
	else if (G_B_cnt>2 && green_bridge_X_first[G_B_cnt - 1] > 48) {
		RobotAction(M_walk_right_small);
		sleep(1);
	}
	else if (green_bridge_Y[0] > 90) {//100
		sleep(1);
		RobotAction(M_walk_forward_repeat_5);
	}
	else {
		RobotAction(M_walk_forward_good_5);
	}
	return GREEN_BRIDGE_ON_THE_BRIDGE;
}
//튀는 거 제거.... 나중에 짜보기


