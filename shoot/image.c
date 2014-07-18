#include "image.h"
#include <ctr/FS.h>
#include <string.h>
#include <malloc.h>
#include <ctr/svc.h>
#include "3dsdefines.h"
#include "debug.h"
#include <stdio.h>
#include "shooter_bmp.h"
#include "winleft_bmp.h"
#include "winright_bmp.h"

extern Handle fsuHandle;
extern FS_archive romfsArchive,sdmcArchive;
extern u8* curBufAdr;

const u32 imageBankSize = NUM_OF_IMAGES+1;
BmpFile* imageBank[NUM_OF_IMAGES+1];

int imageBankInit()
{
	imageBank[0] = NULL;	//make index zero invalid
	imageBank[1] = (BmpFile*)shooter_bmp;
	imageBank[2] = (BmpFile*)winleft_bmp;
	imageBank[3] = (BmpFile*)winright_bmp;
}

void imageBankExit()
{
	//used to have a purpose but no more
}

void drawImage(u8* buf,Image* img,u8 flipped)
{
	BmpFile* pBmp = imageBank[img->imgIndex];
	if(pBmp == NULL)
	{
		return drawRect(buf,&img->pos,255,255,255);
	}
	u32 img_width = pBmp->info.width;
	u8* img_data = pBmp->image;

	u32 x,y;
	for(x = 0; x < img->src.w; x++)
	{
		for(y = 0; y < img->src.h; y++)
		{
			u32 scr_x = img->pos.x + x;
			u32 scr_y = img->pos.y + y;
			u32 scr_pix = (scr_x*MAIN_SCREEN_HEIGHT+scr_y)*3;
			if(scr_x < 0 || scr_y < 0 || scr_x >= MAIN_SCREEN_WIDTH || scr_y >= MAIN_SCREEN_HEIGHT) continue;

			u32 img_x;
			if(flipped) img_x = img->src.x + (img->src.w - 1) - x;
			else img_x = img->src.x + x;
			u32 img_y = img->src.y + y;
			u32 img_pix = (img_x + img_y*pBmp->info.width)*3;

			buf[scr_pix+0] = img_data[img_pix+0];
			buf[scr_pix+1] = img_data[img_pix+1];
			buf[scr_pix+2] = img_data[img_pix+2];
		}
	}
}

static inline void drawRect_common(u8* buf,u32 bufWidth,u32 bufHeight,Rect* rect,u8 r,u8 g,u8 b)
{
	u32 x,y;
	for(x = rect->x; x < rect->x+rect->w; x++)
	{
		for(y = rect->y; y < rect->y+rect->h; y++)
		{
			if(x < 0 || y < 0 || x >= bufWidth || y >= bufHeight) continue;
			u32 scr_pix = (x*bufHeight+y)*3;
			buf[scr_pix+0] = b;
			buf[scr_pix+1] = g;
			buf[scr_pix+2] = r;
		}
	}
}

void drawRect(u8* buf,Rect* rect,u8 r,u8 g,u8 b)
{
	drawRect_common(buf,MAIN_SCREEN_WIDTH,MAIN_SCREEN_HEIGHT,rect,r,g,b);
}

void drawRectSub(u8* buf,Rect* rect,u8 r,u8 g,u8 b)
{
	drawRect_common(buf,SUB_SCREEN_WIDTH,SUB_SCREEN_HEIGHT,rect,r,g,b);
}
