#pragma once

typedef enum {

	M_walk_forward_good_5 = 3,//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	M_walk_forward_repeat_5 =4,

	M_walk_left_small = 6,	
	M_walk_right_small = 7,
	M_walk_left_middle = 8,
	M_walk_right_middle = 9,
	M_walk_left_large = 10,
	M_walk_right_large = 11,

	M_walk_mine_20 = 23,// ����20�����ȱ�
	M_walk_mine_30 = 25,// ����30�����ȱ�

	M_walk_forward_1 = 61,
	M_walk_forward_2 = 62,
	M_walk_forward_3 = 63,
	M_walk_forward_4 = 64,
	M_walk_forward_5 = 65,//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	M_walk_forward_6 = 66,
	M_walk_forward_7 = 67,
	M_walk_forward_8 = 68,
	M_walk_forward_9 = 69,
	M_walk_forward_10 = 70,

	M_huro_camera_up_left_walk = 91,
	M_huro_camera_up_right_walk = 92,

	M_huro_camera_down_walk_golf = 94,
	M_huro_camera_down_left_walk_golf = 95,
	M_huro_camera_down_right_walk_golf = 96,

	M_huro_camera_right_left_walk_large = 98,
	M_huro_camera_right_right_walk_large = 99,

/////////////////////////////////////////////////
	M_huro_CAM_Right_walk_left = 224,
	M_huro_CAM_Right_walk_right = 225,

	M_huro_ball_walk_forward_2 = 251,//252�� = ���̰� 2���� ����
	M_huro_ball_walk_forward = 252,//252�� = ���̰� ����
	M_huro_ball_walk_left = 253,//253�� = ���̰� �°ȱ�
	M_huro_ball_walk_right = 254,//254�� = ���̰� ��ȱ�

	M_huro_CAM_right_walk_left_large = 125,//�����ʺ��� �°ȱ� ũ��
	M_huro_CAM_right_walk_right_large = 126,//�����ʺ��� ��ȱ� ũ��

	M_huro_walk_forward_in_competition = 212,
}Robot_Motion_walk;