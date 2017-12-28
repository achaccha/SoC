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


//#define BLUE_HOLE_NOT_KICK 0
//#define BLUE_HOLE_KICK 1


U8 isFlag = 0;
U8 IN_Blue_hole(U16* imageIn)
{
	U8 BALL_up, BALL_down, BALL_left, BALL_right, BALL_size;
	U8 BALL_x, BALL_y;
	U8 isFInd_yellow_ball = FALSE;


	U8 y, x;
	//공찾기
	if (isFlag == 0) {
		for (y = 60; y < HEIGHT - 1; y++) // 입력 영상의 세로 방향
		{
			for (x = 1; x < WIDTH - 1; x++) // 입력 영상의 가로 방향
			{
				if ((GetPtr(imageIn, y - 1, x - 1, WIDTH) == RGB565YELLOW ? 1 : 0) + (GetPtr(imageIn, y - 1, x, WIDTH) == RGB565YELLOW ? 1 : 0)
					+ (GetPtr(imageIn, y - 1, x + 1, WIDTH) == RGB565YELLOW ? 1 : 0)
					+ (GetPtr(imageIn, y, x - 1, WIDTH) == RGB565YELLOW ? 1 : 0) + (GetPtr(imageIn, y, x, WIDTH) == RGB565YELLOW ? 1 : 0)
					+ (GetPtr(imageIn, y, x + 1, WIDTH) == RGB565YELLOW ? 1 : 0)
					+ (GetPtr(imageIn, y + 1, x - 1, WIDTH) == RGB565YELLOW ? 1 : 0) + (GetPtr(imageIn, y + 1, x, WIDTH) == RGB565YELLOW ? 1 : 0)
					+ (GetPtr(imageIn, y + 1, x + 1, WIDTH) == RGB565YELLOW ? 1 : 0)

					+ (GetPtr(imageIn, y - 2, x, WIDTH) == RGB565YELLOW ? 1 : 0) + (GetPtr(imageIn, y + 2, x, WIDTH) == RGB565YELLOW ? 1 : 0)
					+ (GetPtr(imageIn, y, x - 2, WIDTH) == RGB565YELLOW ? 1 : 0) + (GetPtr(imageIn, y, x + 2, WIDTH) == RGB565YELLOW ? 1 : 0)

					+ (GetPtr(imageIn, y - 2, x - 1, WIDTH) == RGB565YELLOW ? 1 : 0) + (GetPtr(imageIn, y - 2, x + 1, WIDTH) == RGB565YELLOW ? 1 : 0)
					+ (GetPtr(imageIn, y - 2, x - 2, WIDTH) == RGB565YELLOW ? 1 : 0) + (GetPtr(imageIn, y - 2, x + 2, WIDTH) == RGB565YELLOW ? 1 : 0)
					+ (GetPtr(imageIn, y + 2, x - 1, WIDTH) == RGB565YELLOW ? 1 : 0) + (GetPtr(imageIn, y + 2, x + 1, WIDTH) == RGB565YELLOW ? 1 : 0)
					+ (GetPtr(imageIn, y + 2, x - 2, WIDTH) == RGB565YELLOW ? 1 : 0) + (GetPtr(imageIn, y + 2, x + 2, WIDTH) == RGB565YELLOW ? 1 : 0)
					+ (GetPtr(imageIn, y - 1, x + 2, WIDTH) == RGB565YELLOW ? 1 : 0) + (GetPtr(imageIn, y + 1, x + 2, WIDTH) == RGB565YELLOW ? 1 : 0)
					+ (GetPtr(imageIn, y - 2, x + 2, WIDTH) == RGB565YELLOW ? 1 : 0) + (GetPtr(imageIn, y + 2, x + 2, WIDTH) == RGB565YELLOW ? 1 : 0)
					+ (GetPtr(imageIn, y - 1, x - 2, WIDTH) == RGB565YELLOW ? 1 : 0) + (GetPtr(imageIn, y + 1, x - 2, WIDTH) == RGB565YELLOW ? 1 : 0)
					+ (GetPtr(imageIn, y - 2, x - 2, WIDTH) == RGB565YELLOW ? 1 : 0) + (GetPtr(imageIn, y + 2, x - 2, WIDTH) == RGB565YELLOW ? 1 : 0) > 20)
				{
					if (!isFInd_yellow_ball) {
						BALL_up = y;
						BALL_down = y;
						BALL_left = x;
						BALL_right = x;
					}
					if (x > BALL_right) BALL_right = x;
					else if (x < BALL_left) BALL_left = x;

					if (y > BALL_down) BALL_down = y;
					else if (y < BALL_up) BALL_up = y;

					BALL_y = BALL_up + (BALL_down - BALL_up) / 2;
					if (BALL_y < 75)
						isFlag = 1;
				}
			}
		}
	}
	else if (isFlag == 1) {
		for (y = 1; y < HEIGHT - 1; y++) // 입력 영상의 세로 방향
		{
			for (x = 1; x < WIDTH - 1; x++) // 입력 영상의 가로 방향
			{
				if ((GetPtr(imageIn, y - 1, x - 1, WIDTH) == RGB565YELLOW ? 1 : 0) + (GetPtr(imageIn, y - 1, x, WIDTH) == RGB565YELLOW ? 1 : 0)
					+ (GetPtr(imageIn, y - 1, x + 1, WIDTH) == RGB565YELLOW ? 1 : 0)
					+ (GetPtr(imageIn, y, x - 1, WIDTH) == RGB565YELLOW ? 1 : 0) + (GetPtr(imageIn, y, x, WIDTH) == RGB565YELLOW ? 1 : 0)
					+ (GetPtr(imageIn, y, x + 1, WIDTH) == RGB565YELLOW ? 1 : 0)
					+ (GetPtr(imageIn, y + 1, x - 1, WIDTH) == RGB565YELLOW ? 1 : 0) + (GetPtr(imageIn, y + 1, x, WIDTH) == RGB565YELLOW ? 1 : 0)
					+ (GetPtr(imageIn, y + 1, x + 1, WIDTH) == RGB565YELLOW ? 1 : 0)

					+ (GetPtr(imageIn, y - 2, x, WIDTH) == RGB565YELLOW ? 1 : 0) + (GetPtr(imageIn, y + 2, x, WIDTH) == RGB565YELLOW ? 1 : 0)
					+ (GetPtr(imageIn, y, x - 2, WIDTH) == RGB565YELLOW ? 1 : 0) + (GetPtr(imageIn, y, x + 2, WIDTH) == RGB565YELLOW ? 1 : 0)

					+ (GetPtr(imageIn, y - 2, x - 1, WIDTH) == RGB565YELLOW ? 1 : 0) + (GetPtr(imageIn, y - 2, x + 1, WIDTH) == RGB565YELLOW ? 1 : 0)
					+ (GetPtr(imageIn, y - 2, x - 2, WIDTH) == RGB565YELLOW ? 1 : 0) + (GetPtr(imageIn, y - 2, x + 2, WIDTH) == RGB565YELLOW ? 1 : 0)
					+ (GetPtr(imageIn, y + 2, x - 1, WIDTH) == RGB565YELLOW ? 1 : 0) + (GetPtr(imageIn, y + 2, x + 1, WIDTH) == RGB565YELLOW ? 1 : 0)
					+ (GetPtr(imageIn, y + 2, x - 2, WIDTH) == RGB565YELLOW ? 1 : 0) + (GetPtr(imageIn, y + 2, x + 2, WIDTH) == RGB565YELLOW ? 1 : 0)
					+ (GetPtr(imageIn, y - 1, x + 2, WIDTH) == RGB565YELLOW ? 1 : 0) + (GetPtr(imageIn, y + 1, x + 2, WIDTH) == RGB565YELLOW ? 1 : 0)
					+ (GetPtr(imageIn, y - 2, x + 2, WIDTH) == RGB565YELLOW ? 1 : 0) + (GetPtr(imageIn, y + 2, x + 2, WIDTH) == RGB565YELLOW ? 1 : 0)
					+ (GetPtr(imageIn, y - 1, x - 2, WIDTH) == RGB565YELLOW ? 1 : 0) + (GetPtr(imageIn, y + 1, x - 2, WIDTH) == RGB565YELLOW ? 1 : 0)
					+ (GetPtr(imageIn, y - 2, x - 2, WIDTH) == RGB565YELLOW ? 1 : 0) + (GetPtr(imageIn, y + 2, x - 2, WIDTH) == RGB565YELLOW ? 1 : 0) > 20)
				{
					if (!isFInd_yellow_ball) {
						BALL_up = y;
						BALL_down = y;
						BALL_left = x;
						BALL_right = x;
					}
					if (x > BALL_right) BALL_right = x;
					else if (x < BALL_left) BALL_left = x;

					if (y > BALL_down) BALL_down = y;
					else if (y < BALL_up) BALL_up = y;

					isFInd_yellow_ball = TRUE;
				}
			}
		}
	}
	BALL_size = BALL_right - BALL_left;
	BALL_x = BALL_left + BALL_size / 2;
	BALL_y = BALL_up + (BALL_down - BALL_up) / 2;


	if (isFInd_yellow_ball && BALL_y < 75) { //공이 멀리 있다 - > 앞으로
		sleep(1);
		RobotAction(M_walk_forward_good_5);
		//if (BALL_x < 60) {//113
		//	sleep(1);
		//	RobotAction(M_huro_ball_walk_left);
		//	//sleep(1);
		//	//RobotAction(M_huro_turn_right_30);
		//}
		//else if (BALL_x > 150) {//150
		//	sleep(1);
		//	RobotAction(M_huro_ball_walk_right);
		//	//sleep(1);
		//	//RobotAction(M_huro_turn_left_30);
		//}
		sleep(1);
	}
	/*
	else if (isFInd_yellow_ball && BALL_y > 100 ) { //공이 없거나 가깝다 - > 뒤로
	sleep(1);
	RobotAction(M_huro_walk_back_repeat1);
	sleep(1);
	}*/
	else if (isFInd_yellow_ball) {
		if (BALL_x < 110) {//113
			sleep(1);
			RobotAction(M_huro_ball_walk_left);
			//sleep(1);
			//RobotAction(M_huro_turn_right_30);
		}
		else if (BALL_x > 145) {//150
			sleep(1);
			RobotAction(M_huro_ball_walk_right);
			//sleep(1);
			//RobotAction(M_huro_turn_left_30);
		}
		else {
			sleep(1);
			RobotAction(M_shoot);
			sleep(5);
			RobotAction(M_huro_turn_left_90);
			sleep(3);
			printf("shoot\n");
			return 10;
		}
	}
	else
	{
		sleep(1);
		RobotAction(M_huro_ball_walk_forward_2);
		sleep(1);
	}

	for (y = BALL_up; y < BALL_down; y++)
	{
		for (x = BALL_left; x < BALL_right; x++)
		{
			if ((y == BALL_up || y == BALL_down - 1)) {

				GetPtr(imageIn, y, x, WIDTH) = RGB565GREEN;
				GetPtr(imageIn, y + 1, x, WIDTH) = RGB565GREEN;
			}

			if (x == BALL_left || x == BALL_right - 1) {

				GetPtr(imageIn, y, x, WIDTH) = RGB565GREEN;
				GetPtr(imageIn, y, x + 1, WIDTH) = RGB565GREEN;
			}
		}
	}
	printf("%d %d\n", BALL_x, BALL_y);

	draw_fpga_video_data_full(imageIn);
	int e;
	for (e = 0; e<10; e++)
		flip();
	return BLUE_HOLE_NOT_KICK;
}
