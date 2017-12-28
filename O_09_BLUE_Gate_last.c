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

static struct termios inittio, newtio;


//#define BLUE_GATE_FINDING_GATE_DIRECTION 0//5번 장애물, 9번 장애물
//#define BLUE_GATE_NEXT_OBSTACLE 1

