#ifndef _IMAGE_H_
#define _IMAGE_H_
#include "bmp.h"
#include <ctr/types.h>
#include "rect.h"

#define NUM_OF_IMAGES 3
/*
	0 - <invalid>
	1 - shoot.bmp
	2 - winleft.bmp
	3 - winright.bmp
*/

extern const u32 imageBankSize;
extern BmpFile* imageBank[NUM_OF_IMAGES+1];

typedef struct Image {
	Rect pos;
	Rect src;
	u32 imgIndex;
} Image;

int imageBankInit();
//u32 loadBmp(const char* path);
void drawImage(u8* buf,Image* img,u8 flipped);
void drawRect(u8* buf,Rect* rect,u8 r,u8 g,u8 b);
void drawRectSub(u8* buf,Rect* rect,u8 r,u8 g,u8 b);

#endif
