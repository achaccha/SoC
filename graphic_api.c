#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <math.h>

#include "graphic_api.h"
#include "num.h"

#define PRINTLINE	printf("%s , %d \r\n",__FILE__,__LINE__)
#define PRINTVAR(A)	do{int ch;printf("%s=0x%x(%d)\r\n",#A,A,A);ch = getchar();} while (0);

#define AMAZON2_GRAPHIC_NAME  "/dev/amazon2_graphic"

#define SCREEN_WIDTH		320
#define SCREEN_HEIGHT		480

static int graphic_handle = -1;

static float __sintab[91] =
{
	0.0000000000000000f,	0.0174524064372835f,	0.0348994967025010f,	0.0523359562429438f,
	0.0697564737441253f,	0.0871557427476582f,	0.1045284632676535f,	0.1218693434051475f,
	0.1391731009600654f,	0.1564344650402309f,	0.1736481776669303f,	0.1908089953765448f,
	0.2079116908177593f,	0.2249510543438650f,	0.2419218955996677f,	0.2588190451025208f,
	0.2756373558169992f,	0.2923717047227367f,	0.3090169943749474f,	0.3255681544571567f,
	0.3420201433256687f,	0.3583679495453003f,	0.3746065934159120f,	0.3907311284892738f,
	0.4067366430758002f,	0.4226182617406994f,	0.4383711467890774f,	0.4539904997395468f,
	0.4694715627858908f,	0.4848096202463370f,	0.5000000000000000f,	0.5150380749100542f,
	0.5299192642332050f,	0.5446390350150271f,	0.5591929034707468f,	0.5735764363510461f,
	0.5877852522924731f,	0.6018150231520483f,	0.6156614753256583f,	0.6293203910498375f,
	0.6427876096865393f,	0.6560590289905073f,	0.6691306063588582f,	0.6819983600624985f,
	0.6946583704589973f,	0.7071067811865475f,	0.7193398003386511f,	0.7313537016191705f,
	0.7431448254773942f,	0.7547095802227720f,	0.7660444431189780f,	0.7771459614569709f,
	0.7880107536067220f,	0.7986355100472928f,	0.8090169943749474f,	0.8191520442889918f,
	0.8290375725550417f,	0.8386705679454240f,	0.8480480961564260f,	0.8571673007021123f,
	0.8660254037844386f,	0.8746197071393958f,	0.8829475928589269f,	0.8910065241883679f,
	0.8987940462991670f,	0.9063077870366500f,	0.9135454576426009f,	0.9205048534524403f,
	0.9271838545667874f,	0.9335804264972017f,	0.9396926207859084f,	0.9455185755993168f,
	0.9510565162951536f,	0.9563047559630355f,	0.9612616959383189f,	0.9659258262890683f,
	0.9702957262759965f,	0.9743700647852352f,	0.9781476007338056f,	0.9816271834476640f,
	0.9848077530122081f,	0.9876883405951377f,	0.9902680687415703f,	0.9925461516413220f,
	0.9945218953682733f,	0.9961946980917455f,	0.9975640502598242f,	0.9986295347545739f,
	0.9993908270190957f,	0.9998476951563912f,	1.0000000000000000f,
};

void clear_screen(void)
{
	ioctl(graphic_handle, AMAZON2_IOCTL_CLEAR_SCREEN, 0);
}

void flip(void)
{
	if (graphic_handle < 0)
		return;
	ioctl(graphic_handle, AMAZON2_IOCTL_FLIP, 0);
}
void flipwait(void)
{
	if (graphic_handle < 0)
		return;
	ioctl(graphic_handle, AMAZON2_IOCTL_FLIPWAIT, 0);
}

SURFACE* create_surface(int w, int h, int bpp)
{
	SURFACE* surf;
	CreateSurfaceArg arg;
	arg.w = w;
	arg.h = h;
	arg.bpp = bpp;
	surf = (SURFACE*)ioctl(graphic_handle, AMAZON2_IOCTL_CREATE_SURFACE, &arg);
	return surf;
}



static float mysin(U32 angle)
{
	angle %= 360;

	if(angle <= 90)
		return __sintab[angle];
	else if(angle <= 180)
		return __sintab[180 - angle];
	else if(angle <= 270)
		return -__sintab[angle - 180];
	else
		return -__sintab[360 - angle];
}

static float mycos(U32 angle)
{
	return mysin(angle + 90);
}


void read_fpga_video_data(U16* buf)
{
	ioctl(graphic_handle, AMAZON2_IOCTL_READ_FPGA_VIDEO_DATA, buf);
}

void draw_fpga_video_data(U16* buf, int dx, int dy)
{
	DrawFPGADataArg ar;
	ar.buf = buf;
	ar.dx = dx;
	ar.dy = dy;
	ioctl(graphic_handle, AMAZON2_IOCTL_DRAW_FPGA_VIDEO_DATA, &ar);
}

void draw_fpga_video_data_full(U16* buf)
{
	ioctl(graphic_handle, AMAZON2_IOCTL_DRAW_FPGA_VIDEO_DATA_FULL, buf);
}

//---------------------------------------------------------------------
void direct_camera_display_on(void)
{
	ioctl(graphic_handle, AMAZON2_IOCTL_CAM_DISP_ON, 0);
}

void direct_camera_display_off(void)
{
	ioctl(graphic_handle, AMAZON2_IOCTL_CAM_DISP_OFF, 0);
	clear_screen();
}

void Getframe(U16* fpga_videodata)
{
	direct_camera_display_on();

	clear_screen();
	read_fpga_video_data(fpga_videodata);

	direct_camera_display_off();
}

void Showframe(U16* fpga_videodata)
{
	draw_fpga_video_data_full(fpga_videodata);
	flip();
}

void draw_ROI(U16* fpga_videodata, int UP_y, int DOWN_y, int LEFT_x, int RIGHT_x)
{
	int x, y;
	for (x = LEFT_x; x < RIGHT_x; x++)
	{
		GetPtr(fpga_videodata, UP_y, x, WIDTH) = RGB565BLUE;
		GetPtr(fpga_videodata, DOWN_y, x, WIDTH) = RGB565BLUE;
	}
	for (y = UP_y; y < DOWN_y; y++)
	{
		GetPtr(fpga_videodata, y, LEFT_x, WIDTH) = RGB565BLUE;
		GetPtr(fpga_videodata, y, RIGHT_x, WIDTH) = RGB565BLUE;
	}
}

void draw_line_vertical(U16* fpga_videodata, int x)
{
	int y;
	for (y = 0; y < HEIGHT; y++)
	{
		GetPtr(fpga_videodata, y, x, WIDTH) = RGB565BLUE;
	}
}

void draw_line_horizon(U16* fpga_videodata, int y)
{
	int x;
	for (x = 0; x < WIDTH; x++)
	{
		GetPtr(fpga_videodata, y, x, WIDTH) = RGB565BLUE;
	}
}
//---------------------------------------------------------------------


/******************************************************************
		BMP load
******************************************************************/

#define BI_RGB        0L
typedef struct  {
	U32   bfSize;
	U32    bfReserved;
	U32   bfOffBits;
	U32  biSize;
	S32   biWidth;
	S32   biHeight;
	U16   biPlanes;
	U16   biBitCount;
	U32  biCompression;
	U32  biSizeImage;
	S32   biXPelsPerMeter;
	S32   biYPelsPerMeter;
	U32  biClrUsed;
	U32  biClrImportant;
} BITMAPFILEHEADER;

typedef struct  {
	U8    rgbBlue;
	U8    rgbGreen;
	U8    rgbRed;
	U8    rgbReserved;
} RGBQUAD;

#define EXTRACT_READ32(startaddr,offset) (U32)(startaddr[offset] + (U32)(startaddr[offset+1]<<8) + (U32)(startaddr[offset+2]<<16) + (U32)(startaddr[offset+3]<<24))

static BITMAPFILEHEADER bmpfh;

SURFACE* LoadSurfaceInfoFromRGB(U8* bmpdata, U8 bpp, U32 w, U32 h, U32 bmpdatasize, U8* pal)
{
	SURFACE *surface = NULL;
	long i;
	U32  j;
	if (!((bpp == 24) || (bpp == 8) || (bpp == 4))) {
		return 0;
	}
	if ((bpp == 8) || (bpp == 4))
	{
		if (!(surface = create_surface(w, h, bpp)))
		{
			return 0;
		}
		U32 ibpl = (bmpfh.bfSize - bmpfh.bfOffBits) / h;
		surface->pitch = ibpl;
		if (bpp == 8)
		{
			for (i = 0; i < h; i++)
			{
				memcpy(surface->pixels + i*ibpl, bmpdata + (h - 1 - i)*ibpl, surface->w);
			}
			surface->pal->nColors = 256;
			memcpy(surface->pal->colors, pal, 256 * 4);
		}
		else
		{
			for (i = 0; i < h; i++)
			{
				memcpy(surface->pixels + i*ibpl, bmpdata + (h - 1 - i)*ibpl, surface->w / 2);
			}
			surface->pal->nColors = 16;
			memcpy(surface->pal->colors, pal, 16 * 4);
		}

		return surface;
	}
	else if (bpp == 24)
	{
		U32 screenbpp = 16;
		if (!(surface = create_surface(w, h, screenbpp)))
		{
			return 0;
		}

		if (screenbpp == 32)//screen: 4byte(1byte reserved) per pixel
		{
			U32 ibpl;
			U32* rgb32;
			ibpl = ((w * 3) + 3) & 0xfffffffc;
			rgb32 = (U32*)surface->pixels;
			bmpdata += ((h - 1)*ibpl);
			for (i = 0; i < h; i++)
			{
				for (j = 0; j < w; j++)
				{
					rgb32[j] = MAKE_RGB888(bmpdata[j * 3 + 2], bmpdata[j * 3 + 1], bmpdata[j * 3 + 0]);
				}
				bmpdata -= ibpl; //4byte align
				rgb32 += (surface->pitch / 4);
			}
			surface->pixtype = PIX_FMT_RGB888;
		}
		else // convert to rgb888 to rgb565
		{
			U32 ibpl;
			U16* rgb565data;
			ibpl = ((w * 3) + 3) & 0xfffffffc;
			rgb565data = (U16*)surface->pixels;

			bmpdata += ((h - 1)*ibpl);
			for (i = 0; i < h; i++)
			{
				for (j = 0; j < w; j++)
				{
					rgb565data[j] = MAKE_RGB565(bmpdata[j * 3 + 2], bmpdata[j * 3 + 1], bmpdata[j * 3 + 0]);
				}
				bmpdata -= ibpl; //4byte align
				rgb565data += (surface->pitch / 2);
			}
			surface->pixtype = PIX_FMT_RGB565;
		}

	}
	return surface;
}


int open_graphic(void)
{
	if ((graphic_handle = open(AMAZON2_GRAPHIC_NAME, O_RDWR)) < 0) {
		printf("Open Error %s\n", AMAZON2_GRAPHIC_NAME);
		return -1;
	}
	return 0;
}

void close_graphic(void)
{
	if (graphic_handle != -1)
		close(graphic_handle);
}
