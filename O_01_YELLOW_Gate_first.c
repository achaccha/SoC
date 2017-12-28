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

//�κ� ����
//#include <getopt.h>
#include "uart_api.h"
#include "robot_protocol.h"
//#include <termios.h>

static struct termios inittio, newtio;

//#define YELLOW_GATE_WAITING 0//1��, 10�� ��ֹ�
//#define YELLOW_GATE_READY_TO_GO 1
//#define YELLOW_GATE_NEXT_OBSTACLE 2

U8 IN_Yellow_gate(U16* imageIn, U8 order)
{
	U8 nMaskW = 5;
	U8 nMaskH = 5;
	U8 hW = nMaskW / 2;
	U8 hH = nMaskH / 2;

	U16 Yellow_count = 0;
	U8 up = 10, down = 60, left = 50, right = 130;
	U8 y, x;
	for (y = up - hH; y < down - hH; y++) // �Է� ������ ���� ����
	{
		for (x = left-hW; x < right-hW; x++) // �Է� ������ ���� ����
		{
			U8 c, r;
			U16 cnt = 0;
			for (r = 0; r < nMaskH; r++) // ����ũ�� ���� ����
			{
				for (c = 0; c < nMaskW; c++) // ����ũ�� ���� ����
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
				GetPtr(imageIn, y, x, WIDTH) = RGB565BLUE;
			}
		}
	}

	//-----------------------------------------------------
	draw_ROI(imageIn, up, down, left, right);
	Showframe(imageIn);
	//-----------------------------------------------------

	float density = (float)Yellow_count / (float)((down - up) *(right - left));
	printf("density: %f \n", density);

	/*
	ó�� �������ڸ��� �����ִ� => �� �� �����ö����� ���
	ó�� �������ڸ��� �����ִ� => ������ ����
	*/
	if (order == YELLOW_GATE_WAITING)
	{
		printf("111\n");
		if (density <= 0.2)//�ٸ�����Ʈ�� ���������� ���� �������� �𸣴ϱ� ���
			return YELLOW_GATE_WAITING;//order�� 0�� �����༭ ��� ����ϰ�
		else//�ٸ�����Ʈ�� ����
			return YELLOW_GATE_READY_TO_GO;//order�� 1�� �����༭ else�� ���� ��
	}
	else
	{
		if (density <= 0.2)//�ٸ�����Ʈ�� ������ ����
		{
			printf("222\n");
			//sleep(2);
			//RobotAction(M_huro_first_yellow_set);//^^^^^^^^^^^^^^^^^^^^^^^^2�ʵ��� �ö󰡴� �� ����ؼ� �տ� ��ٸ��� �ϴ°� �ʿ����� ����
			//sleep(1);
			RobotAction(M_walk_forward_10);//���� �پ��@@@@@@@@@@@@@@@@@
			//sleep(1);
			RobotAction(M_walk_forward_10);//���� �پ��@@@@@@@@@@@@@@@@@
			//sleep(1);

			return YELLOW_GATE_NEXT_OBSTACLE;//YELLOW_GATE_NEXT_OBSTACLE@@@@@@@@@@@@@@@@@@@@
		}
	}
}

