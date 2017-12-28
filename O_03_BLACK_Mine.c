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


//#define BLACKMINE_SELECT_BSETWAY 0//3번 장애물
//#define BLACKMINE_MOVING 1
//#define BLACKMINE_NEXT_OBSTACLE 2


U8 flag = 3, bcnt = 0, ten = 0;
U8 IN_Black_mine(U8 order)
{
	RobotAction(M_camera_up_golf);	//머리만 드는 동작이 이게 맞는지..?
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
	//------------------------------------------------
	U8 nMaskW = 5, nMaskH = 5, hW = nMaskW / 2, hH = nMaskH / 2;

	U8 is_first = 1;

	U8 blue_count = 0;
	U8 blue_height = 0;

	U8 y, x;
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
					if (GetPtr(imageIn, r - hH + y, c - hW + x, WIDTH) == RGB565YELLOW)
					{
						if (is_first) {
							blue_count++;
							blue_height = y;
							GetPtr(imageIn, y, x - 1, WIDTH) = RGB565YELLOW;
							GetPtr(imageIn, y, x, WIDTH) = RGB565YELLOW;
							GetPtr(imageIn, y, x + 1, WIDTH) = RGB565YELLOW;
						}
					}
				}
				if (!is_first) x = WIDTH;
			}
		}

	}
	if (blue_count > 100) {
		flag = 1;
		sleep(1);
		RobotAction(M_walk_forward_good_5);	//머리 들고 걷는 동작 필요
		sleep(1);
		RobotAction(M_turn_right_30);
		sleep(1);
	}
	if (blue_count < 20 && flag == 1) {
		bcnt++;
		RobotAction(M_walk_forward_good_5);
	}
	else { bcnt = 0; }


	if (blue_count < 20 && flag == 1 && bcnt>2) {
		RobotAction(M_huro_hudle_set1);
		sleep(1);
		return BLACKMINE_NEXT_OBSTACLE;
		//return 10;//허들 라인트레이싱
	}
	sleep(1);
	RobotAction(M_walk_forward_good_5);	//머리 들고 걷는 동작 필요

	sleep(1);
	return 9;//움직이는 중에 라인 트레이싱
}

