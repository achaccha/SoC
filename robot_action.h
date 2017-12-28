#ifndef __AMAZON2_GRAPHIC_API_H__
#define __AMAZON2_GRAPHIC_API_H__


#include "amazon2_sdk.h"
void RobotAction(int RobotArea);

void HURO();

void robot_fall_down_check(U16* imageIn, U8* firstIn);


U8 BLACK_line_check(int action, U8 order, U8 done, U8 isTurn);

#endif //__AMAZON2_GRAPHIC_API_H__
