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

//#define BLUE_GATE_FINDING_GATE_DIRECTION 0//5번 장애물, 9번 장애물
//#define BLUE_GATE_NEXT_OBSTACLE 1


U8 IN_blue_gate(U16* imageIn, U8 order)
{
	U8 left_bluecount = 0, right_bluecount = 0, green_cnt = 0;
	sleep(1);
	RobotAction(M_walk_forward_good_5);
	sleep(1);
	RobotAction(M_camera_up_golf);	// 머리만 드는 동작인지...
	sleep(1);

	U8 y, x;
	for (y = 30; y < HEIGHT; y++) {
		for (x = 1; x < WIDTH / 2 - 1; x++) {
			if ((GetPtr(imageIn, y - 1, x - 1, WIDTH) == RGB565BLUE ? 1 : 0) + (GetPtr(imageIn, y - 1, x, WIDTH) == RGB565BLUE ? 1 : 0)
				+ (GetPtr(imageIn, y - 1, x + 1, WIDTH) == RGB565BLUE ? 1 : 0)
				+ (GetPtr(imageIn, y, x - 1, WIDTH) == RGB565BLUE ? 1 : 0) + (GetPtr(imageIn, y, x, WIDTH) == RGB565BLUE ? 1 : 0)
				+ (GetPtr(imageIn, y, x + 1, WIDTH) == RGB565BLUE ? 1 : 0)
				+ (GetPtr(imageIn, y + 1, x - 1, WIDTH) == RGB565BLUE ? 1 : 0) + (GetPtr(imageIn, y + 1, x, WIDTH) == RGB565BLUE ? 1 : 0)
				+ (GetPtr(imageIn, y + 1, x + 1, WIDTH) == RGB565BLUE ? 1 : 0) > 4) {
				left_bluecount++;
			}
			if ((GetPtr(imageIn, y - 1, WIDTH - (x - 1), WIDTH) == RGB565BLUE ? 1 : 0) + (GetPtr(imageIn, y - 1, WIDTH - (x), WIDTH) == RGB565BLUE ? 1 : 0)
				+ (GetPtr(imageIn, y - 1, WIDTH - (x + 1), WIDTH) == RGB565BLUE ? 1 : 0)
				+ (GetPtr(imageIn, y, WIDTH - (x - 1), WIDTH) == RGB565BLUE ? 1 : 0) + (GetPtr(imageIn, y, WIDTH - (x), WIDTH) == RGB565BLUE ? 1 : 0)
				+ (GetPtr(imageIn, y, WIDTH - (x + 1), WIDTH) == RGB565BLUE ? 1 : 0)
				+ (GetPtr(imageIn, y + 1, WIDTH - (x - 1), WIDTH) == RGB565BLUE ? 1 : 0) + (GetPtr(imageIn, y + 1, WIDTH - (x), WIDTH) == RGB565BLUE ? 1 : 0)
				+ (GetPtr(imageIn, y + 1, WIDTH - (x + 1), WIDTH) == RGB565BLUE ? 1 : 0) > 4) {
				right_bluecount++;
			}
			if ((GetPtr(imageIn, y - 1, x - 1, WIDTH) == RGB565GREEN ? 1 : 0) + (GetPtr(imageIn, y - 1, x, WIDTH) == RGB565GREEN ? 1 : 0)
				+ (GetPtr(imageIn, y - 1, x + 1, WIDTH) == RGB565GREEN ? 1 : 0)
				+ (GetPtr(imageIn, y, x - 1, WIDTH) == RGB565GREEN ? 1 : 0) + (GetPtr(imageIn, y, x, WIDTH) == RGB565GREEN ? 1 : 0)
				+ (GetPtr(imageIn, y, x + 1, WIDTH) == RGB565GREEN ? 1 : 0)
				+ (GetPtr(imageIn, y + 1, x - 1, WIDTH) == RGB565GREEN ? 1 : 0) + (GetPtr(imageIn, y + 1, x, WIDTH) == RGB565GREEN ? 1 : 0)
				+ (GetPtr(imageIn, y + 1, x + 1, WIDTH) == RGB565GREEN ? 1 : 0) > 5)
			{
				green_cnt++;
			}
		}
	}


	sleep(1);
	if (green_cnt > 100) {
		return BLUE_GATE_NEXT_OBSTACLE;
	}

	if (left_bluecount > 10 && right_bluecount < 5) {
		RobotAction(M_walk_right_small);
	}
	else if (left_bluecount < 5 && right_bluecount > 10) {
		RobotAction(M_walk_left_small);
	}
	else {
		sleep(2);

		RobotAction(M_walk_forward_good_5);
		sleep(2);
		RobotAction(M_turn_right_30);

	}
	return BLUE_GATE_FINDING_GATE_DIRECTION;
}

U8 flag_p = 1;

U8 BLACK_line_check_blue(int action, U8 order, U8 done, U8 isTurn) {

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
	U8 white = 0, blue = 0;
	for (x = 0; x < WIDTH - 30; x += 2) {
		is_first = 1; white = 0;
		for (y = 1; y < HEIGHT - 1; y++)
		{
			if (x > 30) {
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
			if ((GetPtr(imageIn, y - 1, x - 1, WIDTH) == RGB565BLUE ? 1 : 0) + (GetPtr(imageIn, y - 1, x, WIDTH) == RGB565BLUE ? 1 : 0)
				+ (GetPtr(imageIn, y - 1, x + 1, WIDTH) == RGB565BLUE ? 1 : 0)
				+ (GetPtr(imageIn, y, x - 1, WIDTH) == RGB565BLUE ? 1 : 0) + (GetPtr(imageIn, y, x, WIDTH) == RGB565BLUE ? 1 : 0)
				+ (GetPtr(imageIn, y, x + 1, WIDTH) == RGB565BLUE ? 1 : 0)
				+ (GetPtr(imageIn, y + 1, x - 1, WIDTH) == RGB565BLUE ? 1 : 0) + (GetPtr(imageIn, y + 1, x, WIDTH) == RGB565BLUE ? 1 : 0)
				+ (GetPtr(imageIn, y + 1, x + 1, WIDTH) == RGB565BLUE ? 1 : 0) > 4)
			{
				blue++;
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

	if (blue > 70) {
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
				RobotAction(M_huro_CAM_Right_turn_right);	// 머리 오른쪽 보면서 turn 하는 동작 필요
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
	}
	else { RobotAction(M_huro_CAM_Right_walk_right); }


	return 10;
}



/*/
U8 IN_blue_gate(U16* imageIn, U8 order)
{
sleep(1);
RobotAction(M_camera_up);


U8 nMaskW = 4;
U8 nMaskH = 4;
U8 hW = nMaskW / 2;
U8 hH = nMaskH / 2;

U16  B_G_CNT = 0, blue_cnt = 0;
U16  L_CNT = 0, black_cnt = 0;
U8 BLUE_is_first = 1, BLUE_first, BLUE_last;
U8 BLACK_is_first = 1, BLACK_first, BLACK_last;
U16 LEFTCNT = 0, RIGHTCNT = 0;


U8 BLUE_left[100] = { 0, }, BLUE_right[100] = { 0, }, BLUE_Y[100] = { 0, };
U8 BLACK_left[100] = { 0, }, BLACK_right[100] = { 0, }, BLACK_Y[100] = { 0, };

U8 up = 0, down = 100, left = hW, right = 180 - hW;
U8 x, y;
for (y = up; y < down; y++) // 입력 영상의 세로 방향
{
blue_cnt = 0;
black_cnt = 0;
BLUE_is_first = 1;
BLACK_is_first = 1;
for (x = left; x < right; x++) // 입력 영상의 가로 방향
{
U8 c, r;
U16 BLUE = 0, BLACK=0;
for (r = 0; r < nMaskH; r++) // 마스크의 세로 방향
{
for (c = 0; c < nMaskW; c++) // 마스크의 가로 방향
{
if (GetPtr(imageIn, r - hH + y, c - hW + x, WIDTH) == RGB565BLUE)
{
BLUE++;
}

if (GetPtr(imageIn, r - hH + y, c - hW + x, WIDTH) == RGB565BLACK)
{
BLACK++;
}
}
}

if ((float)BLUE / ((float)nMaskH*(float)nMaskW) > 0.7)
{
if (BLUE_is_first) { BLUE_is_first = 0; BLUE_first = x; }
BLUE_last = x;
blue_cnt++;
}
if ((float)BLACK/ ((float)nMaskH*(float)nMaskW) > 0.8)
{
if (BLACK_is_first) { BLACK_is_first = 0; BLACK_first = x; }
BLACK_last = x;
black_cnt++;
}

}//x
//printf("BLUE_cnt %d/ %d/%d/%d\n", blue_cnt, BLUE_first, BLUE_last, y);

if ( BLUE_last -BLUE_first>60) {

BLUE_left[B_G_CNT] = BLUE_first;
BLUE_right[B_G_CNT] = BLUE_last;
BLUE_Y[B_G_CNT++] = y;
}
else if (blue_cnt > 5) {

if (BLUE_last< 90) LEFTCNT++;
if (BLUE_first > 90) RIGHTCNT++;

}
if (black_cnt > 5) {
BLACK_left[L_CNT] = BLACK_first;
BLACK_right[L_CNT] = BLACK_last;
BLACK_Y[L_CNT++] = y;
}
}//y

if (LEFTCNT > 5&& RIGHTCNT<3) {
RobotAction(M_camera_right);
sleep(1);
free(imageIn);

U16* fpga_videodata = (U16*)malloc(180 * 120 * 2);

clear_screen();
read_fpga_video_data(fpga_videodata);
for (y = up; y < down; y++) // 입력 영상의 세로 방향
{
blue_cnt = 0;
for (x = left; x < right; x++) // 입력 영상의 가로 방향
{
U8 c, r;
U16 BLUE = 0, BLACK = 0;
for (r = 0; r < nMaskH; r++) // 마스크의 세로 방향
{
for (c = 0; c < nMaskW; c++) // 마스크의 가로 방향
{
if (GetPtr(imageIn, r - hH + y, c - hW + x, WIDTH) == RGB565BLUE)
{
BLUE++;
}
}
}

if ((float)BLUE / ((float)nMaskH*(float)nMaskW) > 0.7)
{
if (BLUE_is_first) { BLUE_is_first = 0; BLUE_first = x; }
BLUE_last = x;
blue_cnt++;
}
}//x

if (blue_cnt > 5) {
if (BLUE_last<= 90) LEFTCNT++;
if (BLUE_first > 90) RIGHTCNT++;
}
}//y
if (LEFTCNT + RIGHTCNT > 10) RobotAction(B_right);
else { RobotAction(M_huro_turn_left_40); }

draw_fpga_video_data_full(fpga_videodata);
flip();
free(fpga_videodata);
}
if (RIGHTCNT > 50000 && LEFTCNT<3) {
RobotAction(1);
sleep(1);
free(imageIn);

U16* fpga_videodata = (U16*)malloc(180 * 120 * 2);

clear_screen();
read_fpga_video_data(fpga_videodata);
for (y = up; y < down; y++) // 입력 영상의 세로 방향
{
blue_cnt = 0;
for (x = left; x < right; x++) // 입력 영상의 가로 방향
{
U8 c, r;
U16 BLUE = 0, BLACK = 0;
for (r = 0; r < nMaskH; r++) // 마스크의 세로 방향
{
for (c = 0; c < nMaskW; c++) // 마스크의 가로 방향
{
if (GetPtr(imageIn, r - hH + y, c - hW + x, WIDTH) == RGB565BLUE)
{
BLUE++;
}
}
}

if ((float)BLUE / ((float)nMaskH*(float)nMaskW) > 0.7)
{
if (BLUE_is_first) { BLUE_is_first = 0; BLUE_first = x; }
BLUE_last = x;
blue_cnt++;
}
}//x

if (blue_cnt > 5) {
if (BLUE_last <= 90) LEFTCNT++;
if (BLUE_first > 90) RIGHTCNT++;
}
}//y
if (LEFTCNT + RIGHTCNT > 10) RobotAction(B_left);

draw_fpga_video_data_full(fpga_videodata);
flip();
free(fpga_videodata);
}
//printf("--%d---\n", B_G_CNT);
U16 BLUE_CENTER = 0;
if (B_G_CNT > 5) {
if (LEFTCNT>10 || RIGHTCNT>10){//(float)B_G_CNT / (float)BLUE_Y[B_G_CNT - 1] < 0.5) {
if (LEFTCNT > RIGHTCNT) { RobotAction(B_right); sleep(3); }
else { RobotAction(B_left); sleep(3);
}
}
else {

for (x = 2; x < B_G_CNT - 2; x++)
{
BLUE_CENTER += BLUE_left[x] + (BLUE_right[x] - BLUE_left[x]) / 2;
}
BLUE_CENTER /= (B_G_CNT - 4);

//	printf("%d   %d\n", BLUE_CENTER, BLUE_Y[B_G_CNT - 1]);

if (BLUE_Y[B_G_CNT - 1] < 20) {
if (BLUE_CENTER < 90 - 20) { RobotAction(B_left); }
else if (BLUE_CENTER > 90 + 20) { RobotAction(B_right); }
else { RobotAction(M_huro_walk_forward); sleep(1); RobotAction(M_camera_up); }
}
else if (BLUE_Y[B_G_CNT - 1] < 70) {
if (BLUE_CENTER < 90 - 15) { RobotAction(B_left); }
else if (BLUE_CENTER > 90 + 15) { RobotAction(B_right); }
else { RobotAction(M_huro_walk_forward); sleep(1); RobotAction(M_camera_up); }
}
else {
if (BLUE_CENTER < 90 - 15) { RobotAction(B_left); }
else if (BLUE_CENTER > 90 + 15) { RobotAction(B_right); }
else { RobotAction(B_walk_forward); return 1; }
}
}
}
else {
if (LEFTCNT > RIGHTCNT) RobotAction(B_right);
else RobotAction(B_left);
sleep(1);

RobotAction(M_huro_walk_forward_good);
sleep(3);
//	printf("why\n");
}
//	printf("---------------------------------------\n");

return 10;
}
*/
