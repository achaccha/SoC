#ifndef __AMAZON2_GRAPHIC_API_H__
#define __AMAZON2_GRAPHIC_API_H__


#include "amazon2_sdk.h"

void clear_screen(void);
void flip(void);
void flipwait(void);
SURFACE* create_surface(int w, int h, int bpp);
void read_fpga_video_data(U16* buf);
void draw_fpga_video_data(U16* buf, int dx, int dy);
void draw_fpga_video_data_full(U16* buf);

//---------------------------------------------------------------------
void direct_camera_display_on(void);
void direct_camera_display_off(void);

void Getframe(U16* fpga_videodata);
void Showframe(U16* fpga_videodata);

void draw_ROI(U16* fpga_videodata, int UP_y, int DOWN_y, int LEFT_x, int RIGHT_x);
void draw_line_vertical(U16* fpga_videodata, int x);
void draw_line_horizon(U16* fpga_videodata, int y);
//---------------------------------------------------------------------

int open_graphic(void); 
void close_graphic(void);

#endif //__AMAZON2_GRAPHIC_API_H__
