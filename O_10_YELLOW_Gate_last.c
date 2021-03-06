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

U8 IN_Yellow_gate_last(U16* imageIn, U8 order)
{
	U8 nMaskW = 5;
	U8 nMaskH = 5;
	U8 hW = nMaskW / 2;
	U8 hH = nMaskH / 2;

	U16 Yellow_count = 0;
	U8 up = 10, down = 60, left = 50, right = 130;
	U8 r, c;
	for (r = up; r < down; r++) // 입력 영상의 세로 방향
	{
		for (c = left; c < right; c++) // 입력 영상의 가로 방향
		{
			U8 x, y;
			U16 cnt = 0;
			for (y = 0; y < nMaskH; y++) // 마스크의 세로 방향
			{
				for (x = 0; x < nMaskW; x++) // 마스크의 가로 방향
				{
					if (GetPtr(imageIn, r - hH + y, c - hW + x, WIDTH) == RGB565YELLOW)
					{
						cnt++;
					}
				}
			}

			if ((float)cnt / ((float)nMaskH*(float)nMaskW) > 0.8)
			{
				Yellow_count++;
			}
		}
	}
	/*
	처음 시작하자마자 열려있다 => 한 번 내려올때까지 대기
	처음 시작하자마자 닫혀있다 => 열리면 간다
	*/
	if (order == YELLOW_GATE_WAITING)
	{
		if (Yellow_count / (down - up) *(right - left) <= 0.2)//바리게이트가 열려있지만 언제 열린건지 모르니까 대기
			return YELLOW_GATE_WAITING;//order에 0를 돌려줘서 계속 대기하게
		else//바리게이트가 닫힘
			return YELLOW_GATE_READY_TO_GO;//order에 1를 돌려줘서 else로 들어가게 함
	}
	else
	{
		if (Yellow_count / (down - up) *(right - left) <= 0.2)//바리게이트가 열리면 전진
		{
			sleep(2);
			RobotAction(M_huro_first_yellow_set);//^^^^^^^^^^^^^^^^^^^^^^^^2초동안 올라가는 거 고려해서 앞에 기다리게 하는게 필요한지 검토
			sleep(2);
			RobotAction(M_walk_forward_good_5);
			sleep(1);
			RobotAction(M_walk_forward_good_5);//벽에 붙어라@@@@@@@@@@@@@@@@@
			sleep(1);

			return 10;//YELLOW_GATE_NEXT_OBSTACLE@@@@@@@@@@@@@@@@@@@@
		}
	}
}

//#define YELLOW_GATE_WAITING 0//1번, 10번 장애물
//#define YELLOW_GATE_READY_TO_GO 1
//#define YELLOW_GATE_NEXT_OBSTACLE 2

