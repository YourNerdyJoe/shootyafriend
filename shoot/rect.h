#ifndef _RECT_H_
#define _RECT_H_
#include <ctr/types.h>

typedef struct Rect {
	u32 x,y,w,h;
} Rect;

int checkCollision(Rect* r1,Rect* r2);

#endif
