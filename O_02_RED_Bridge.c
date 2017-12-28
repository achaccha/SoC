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


//#define RED_BRIDGE_BEFORE_THE_STAIRS 0
//#define RED_BRIDGE_NEXT_OBSTACLE 1

int CNT = 0;
U8 IN_Red_bridge(U16* imageIn, U8 order)
{
	U8 nMaskW = 5, nMaskH = 5, hW = nMaskW / 2, hH = nMaskH / 2;

	float bridge_X_F[40] = { 0, }, bridge_X_L[40] = { 0, }, bridge_Y[40] = { 0, };
	int first = 0, last = 0;
	U8 R_B_cnt = 0, cnt = 0;
	U8 is_first = 1, is_last = 1;
	U8 y, x;
	U8 FIRST_Y = 0, LAST_Y = 0, UP_Y = 0;

	for (y = hH; y < 50; y += 1)
	{
		is_first = 1;
		for (x = hW; x < WIDTH - hW; x += 2)
		{
			U8 c, r;
			U16 cnt = 0;
			for (r = 0; r < nMaskH; r++) // 마스크의 세로 방향
			{
				for (c = 0; c < nMaskW; c++) // 마스크의 가로 방향
				{
					if (GetPtr(imageIn, r - hH + y, c - hW + x, WIDTH) == RGB565RED)
					{
						cnt++;
					}

					if ((float)cnt / ((float)nMaskH*(float)nMaskW) > 0.8)
					{
						if (is_first) {
							is_first = 0; 
							first = x;
							GetPtr(imageIn, y, first - 1, WIDTH) = RGB565BLUE;
							GetPtr(imageIn, y, first, WIDTH) = RGB565BLUE;
							GetPtr(imageIn, y, first + 1, WIDTH) = RGB565BLUE;
							x = WIDTH;
						}
					}
				}
			}
		}//x

		is_last = 1;
		for (x = WIDTH - hW; x > hW; x -= 2)
		{
			U8 c, r;
			U16 cnt = 0;
			for (r = 0; r < nMaskH; r++) // 마스크의 세로 방향
			{
				for (c = 0; c < nMaskW; c++) // 마스크의 가로 방향
				{
					if (GetPtr(imageIn, r - hH + y, c - hW + x, WIDTH) == RGB565RED)
					{
						cnt++;
					}
					if ((float)cnt / ((float)nMaskH*(float)nMaskW) > 0.8)
					{

						if (is_last) {
							is_last = 0; 
							last = x;							
							GetPtr(imageIn, y, last - 1, WIDTH) = RGB565BLUE;
							GetPtr(imageIn, y, last, WIDTH) = RGB565BLUE;
							GetPtr(imageIn, y, last + 1, WIDTH) = RGB565BLUE;
							x = 0;	
						}
					}
				}
			}
		}//x2

		if (is_first && is_last) { // 빨강 없음. 끝쪽에 다다름????
			UP_Y = y;
		}

		bridge_X_F[R_B_cnt] = first;
		bridge_X_L[R_B_cnt] = last;
		bridge_Y[R_B_cnt++] = y;

		if (first < 3) FIRST_Y = y;
		if (last > 180 - 3) LAST_Y = y;
	}//y


	if (UP_Y) { // 아직멀어~
		U8 avg_X = 0;
		for (x = 0; x < R_B_cnt; x++) {
			avg_X += bridge_X_F[x] + (bridge_X_L[x] - bridge_X_F[x]) / 2;
		}
		avg_X /= (R_B_cnt - 1);

		if (avg_X > 70) { RobotAction(M_walk_left_small); }
		else if (avg_X < 50) { RobotAction(M_walk_left_small); }
		else { RobotAction(M_walk_forward_good_5); }

	}
	else { // 거의 붙음

		if (CNT > 8) { CNT = 0; return 10; }
		else if (LAST_Y > 3) { RobotAction(M_walk_right_small); }	//
		else if (FIRST_Y > 3) { RobotAction(M_walk_left_small); } //
		else if (LAST_Y + 5 < FIRST_Y) { RobotAction(M_walk_left_small); }//M_huro_walk_left_small
		else if (LAST_Y - 5 > FIRST_Y) { RobotAction(M_walk_right_small); }//M_huro_walk_right_small
		else { CNT = 0; return 10; }
	}
	sleep(1);
	RobotAction(M_walk_forward_good_5);
	printf("%d,%d\n", FIRST_Y, LAST_Y);

	CNT++;
	return RED_BRIDGE_BEFORE_THE_STAIRS;
}
