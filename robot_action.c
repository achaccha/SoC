//영상처리
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
#include "robot_action.h"
#include <termios.h>


//장애물
#include "O_01_YELLOW_Gate_first.c"
#include "O_02_RED_Bridge.c"
#include "O_03_BLACK_Mine.c"
#include "O_04_BLUE_Hurdle.c"
#include "O_05_BLUE_Gate_first.c"
#include "O_06_GREEN_Bridge.c"
#include "O_07_ORANGE_Ball.c"
#include "O_08_YELLOW_Bridge.c"
#include "O_09_BLUE_Gate_last.c"
#include "O_10_YELLOW_Gate_last.c"

#include "robot_action.h"

#include "Line_tracing_test.c"
static struct termios inittio, newtio;

#define AMAZON2_GRAPHIC_VERSION		"v0.5"

U8 check_over_idx = 0;
U8 Idx = 0;

U8 BLACK_line_check(int action, U8 order, U8 done, U8 isTurn) {

	RobotAction(M_camera_right);
	sleep(1);

	U16* imageIn = (U16*)malloc(180 * 120 * 2);

	clear_screen();
	read_fpga_video_data(imageIn);

	U8 p = 1;
	while (p--)
	{
		draw_fpga_video_data_full(imageIn);
		flip();
	}

	int Y_B_cnt = 0;
	U8 Y_length = 2;
	U8 first, is_first = 1;
	U8 F = 0, L = 0;
	float bridge_X[100] = { 0, }, bridge_Y[100] = { 0, };
	//좌표 찾기
	U8 y, x;
	U8 white = 0;
	for (x = 30; x < WIDTH - 30; x += 2) {
		is_first = 1; white = 0;
		for (y = 1; y < HEIGHT - 1; y++)
		{
			if ((GetPtr(imageIn, y - 1, x - 1, WIDTH) == RGB565WHITE ? 1 : 0) + (GetPtr(imageIn, y - 1, x, WIDTH) == RGB565WHITE ? 1 : 0)
				+ (GetPtr(imageIn, y - 1, x + 1, WIDTH) == RGB565WHITE ? 1 : 0)
				+ (GetPtr(imageIn, y, x - 1, WIDTH) == RGB565WHITE ? 1 : 0) + (GetPtr(imageIn, y, x, WIDTH) == RGB565WHITE ? 1 : 0)
				+ (GetPtr(imageIn, y, x + 1, WIDTH) == RGB565WHITE ? 1 : 0)
				+ (GetPtr(imageIn, y + 1, x - 1, WIDTH) == RGB565WHITE ? 1 : 0) + (GetPtr(imageIn, y + 1, x, WIDTH) == RGB565WHITE ? 1 : 0)
				+ (GetPtr(imageIn, y + 1, x + 1, WIDTH) == RGB565WHITE ? 1 : 0) > 4)
			{
				white++;
				if (is_first) { is_first = 0; first = y; }
			}
		}
		if (((float)(white) / (float)((float)HEIGHT)) > 0.1)
		{
			//if (x < 60 && first < 3) F++;
			//else if (x > 150 && first < 6) L++;
			bridge_X[Y_B_cnt] = x;
			bridge_Y[Y_B_cnt++] = first;
		}
	}

	float avg_Y = 0;

	free(imageIn);

	if ((Idx++) % 5 == 0 && isTurn == 3) {
		sleep(1);
		Idx = 0;
		return done;
	}


	if (Y_B_cnt > 5) {
		int slope = bridge_Y[Y_B_cnt - 1] - bridge_Y[0];
		for (x = Y_B_cnt / 2 - 2; x < Y_B_cnt / 2 + 2; x++) {
			avg_Y += bridge_Y[x];
		}
		avg_Y /= 4;


		if (isTurn == 1) {
			if (slope > 20) {
				RobotAction(M_huro_CAM_Right_turn_right);
				sleep(1);
			}
			else if (slope < -10) {
				RobotAction(M_huro_CAM_Right_turn_left);
				sleep(1);
			}
			else if (avg_Y < 40) {
				RobotAction(M_huro_CAM_Right_walk_right);
			}
			else if (avg_Y > 50) {
				RobotAction(M_huro_CAM_Right_walk_left);
			}
			else {
				RobotAction(action);
				return done;
			}
		}
		else if (isTurn == 0)
		{

			if (avg_Y < 40) {
				RobotAction(M_huro_CAM_Right_walk_right);
			}
			else if (avg_Y > 50) {
				RobotAction(M_huro_CAM_Right_walk_left);
			}
			else {
				RobotAction(action);
				return done;
			}
		}
		else if (isTurn == 2) {
			if (slope > 20) {
				RobotAction(M_huro_CAM_Right_turn_right);
				sleep(1);
			}
			else if (slope < -10) {
				RobotAction(M_huro_CAM_Right_turn_left);
				sleep(1);
			}
			else {
				RobotAction(action);
				return done;
			}
		}
		else if (isTurn == 3)
		{
			if (check_over_idx < 4)
			{
				if (slope > 25) {
					RobotAction(M_huro_CAM_Right_turn_right);
					sleep(1);
				}
				else if (slope < -15) {
					RobotAction(M_huro_CAM_Right_turn_left);
					sleep(1);
				}
				else if (avg_Y < 30) {
					RobotAction(M_huro_CAM_Right_walk_right);
				}
				else if (avg_Y > 55) {
					RobotAction(M_huro_CAM_Right_walk_left);
				}
				else {
					sleep(1);
					RobotAction(action);
					sleep(1);
					return done;
				}
				check_over_idx++;
			}
			else
			{
				check_over_idx = 0;
				sleep(1);
				RobotAction(action);
				sleep(1);
				return done;
			}
		}
	}
	else { RobotAction(M_huro_CAM_Right_walk_right); }


	return 10;
}



void HURO()
{
	U8 order = 0;
	U8 Obstacle_idx = 1;

	int in_mine_cnt = 0;

	int firstIn = 1;

	RobotAction(M_base);

	while (1)
	{
		U16* fpga_videodata = (U16*)malloc(180 * 120 * 2);

		//---------------------------------------------------
		Getframe(fpga_videodata);
		//---------------------------------------------------

		switch (Obstacle_idx)
		{
		case 1:
			/*if (order == 10)
			{
				printf("i am in 1");
				order = BLACK_line_check(fpga_videodata, order, YELLOW_GATE_NEXT_OBSTACLE, 0);
			}*/
			if (order == YELLOW_GATE_NEXT_OBSTACLE)
			{
				sleep(1);
				/*RobotAction(M_RED_bridge_set);
				sleep(8);
				//order = 10;*/
				order = ORDER_BASE;
				Obstacle_idx = 2;
			}
			else
				order = IN_Yellow_gate(fpga_videodata, order);
			break;

		case 2:
			if (order == 10)
			{
				order = BLACK_line_check(fpga_videodata, order, RED_BRIDGE_NEXT_OBSTACLE, 0);  //red 용으로 바꿔야함. 음 
			}
			else if (order == RED_BRIDGE_NEXT_OBSTACLE)
			{
				RobotAction(M_walk_forward_good_5); sleep(2);
				sleep(1);
				RobotAction(M_RED_bridge_set);
				sleep(8);
				//order = 10;
				order = ORDER_BASE;
				Obstacle_idx = 3;
			}
			else
				order = IN_Red_bridge(fpga_videodata, order);
			break;

		case 3:
			/*
			if (order == 9)
			{
				printf("i am in 3");
				order = BLACK_line_check(M_camera_up, order, BLACKMINE_MOVING, 3);
			}
			else if (order == 10)
			{
				printf("i am in 4");
				order = BLACK_line_check(fpga_videodata, order, BLACKMINE_NEXT_OBSTACLE, 1);
			}
			else */if (order == BLACKMINE_NEXT_OBSTACLE || in_mine_cnt > 10)
			{
				sleep(2);
				RobotAction(M_huro_hudle_set2);
				sleep(10);
				order = 10;
				Obstacle_idx = 5;
			}
			else {
				order = IN_Black_mine(order);
				sleep(1);
				//	order = BLACK_line_check(fpga_videodata, order, order, 0);
			}
			in_mine_cnt++;
			break;
		case 4 : 


			break;
		case 5:
			if (order == 10)
			{
				order = BLACK_line_check_blue(fpga_videodata, order, BLUE_GATE_FINDING_GATE_DIRECTION, 1);
			}
			else order = IN_blue_gate(fpga_videodata, order);
			if (order == BLUE_GATE_NEXT_OBSTACLE)
			{
				order = ORDER_BASE;
				Obstacle_idx++;
			}
			break;
		case 6:
			if (order == GREEN_BRIDGE_FIND)//GREEN_BRIDGE_NEXT_OBSTACLE
			{
				order = CLOSE_TO_GREEN_Bridge(fpga_videodata, order);
			}
			else if (order == GREEN_BRIDGE_ON_THE_BRIDGE)
			{
				order = IN_GREEN_Bridge(fpga_videodata, order);
			}
			else
			{
				order = ORDER_BASE;
				Obstacle_idx++;
			}
			break;
		case 7:
			if (order == 10)
			{
				order = BLACK_line_check(fpga_videodata, order, BLUE_HOLE_KICK, 1);
			}
			else order = IN_Blue_hole(fpga_videodata);
			if (order == BLUE_HOLE_KICK)
			{
				order = ORDER_BASE;
				Obstacle_idx++;
			}
			break;
		case 8:
			if (order == YELLOW_BRIDGE_BEFORE_THE_STAIRS)
			{
				order = CLOSE_TO_YELLOW_Bridge(fpga_videodata, order);
			}
			else if (order == YELLOW_BRIDGE_ON_THE_BRIDGE)
			{
				order = IN_YELLOW_Bridge(fpga_videodata, order);
			}
			else if (order == YELLOW_BRIDGE_DOWN)
			{
				order = DOWN_YELLOW_Bridge(fpga_videodata, order);
			}
			else
			{
				order = ORDER_BASE;
				Obstacle_idx++;
			}
			break;
		case 9:
			if (order == 10)
			{
				order = BLACK_line_check(fpga_videodata, order, BLUE_GATE_NEXT_OBSTACLE, 1);
			}
			else order = IN_blue_gate(fpga_videodata, order);
			if (order == BLUE_GATE_NEXT_OBSTACLE)
			{
				order = ORDER_BASE;
				Obstacle_idx++;
			}
			break;
		case 10:
			order = IN_Yellow_gate_last(fpga_videodata, order);
			break;
		case 11:
			if (order == 10)
			{
				order = BLACK_line_check(fpga_videodata, order, BLUE_GATE_NEXT_OBSTACLE, TRUE);
			}
			else
			{
				goto out;
			}
		case 12:
			Line_tracing_forward();
		default:
			break;
		}
		free(fpga_videodata);

	}
out:
	//free(fpga_videodata);
	printf("huro end\n");
}

U16 previous_image[180 * 120];
U16 older_previous_image[180 * 120];
void robot_fall_down_check(U16* imageIn,U8* firstIn)
{
	if (firstIn == 1)//맨처음에는 이미지가 비어있는 상태니까 복사하고 끝남
	{
		memcpy(older_previous_image, imageIn, WIDTH * HEIGHT * sizeof(U16));
		memcpy(previous_image, imageIn, WIDTH * HEIGHT * sizeof(U16));
		*firstIn = 0;
		return;
	}

	U16 change_count1 = 0, change_count2 = 0;
	int y, x;
	for (y = 0; y < HEIGHT; y++)
	{
		for (x = 0; x < WIDTH; x++)
		{
			if (GetPtr(previous_image, y, x, WIDTH) - GetPtr(imageIn, y, x, WIDTH) != 0)
				change_count1++;
			if (GetPtr(older_previous_image, y, x, WIDTH) - GetPtr(imageIn, y, x, WIDTH) != 0)
				change_count2++;
		}
	}

	memcpy(older_previous_image, previous_image, WIDTH * HEIGHT * sizeof(U16));//이전 이미지들을 현재 이미지로 갱신
	memcpy(previous_image, imageIn, WIDTH * HEIGHT * sizeof(U16));

	if (change_count1 <= WIDTH*HEIGHT / 10 && change_count2 <= WIDTH*HEIGHT / 10)//이미지가 1/10이하로 변화가 없으면 움직여도 화면이 안 바뀌는 것이라고 판단하고 일어서는 동작 수행
	{
		RobotAction(M_base);//일어서는 동작^^^^^^^^^^^^^^^^^^^^^^^6
	}
}//^^^^^^수정필요




