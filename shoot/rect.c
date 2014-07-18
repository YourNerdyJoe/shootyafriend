#include "rect.h"

int checkCollision(Rect* r1,Rect* r2)
{
	int _x=0,_y=0;

	u32 r1left = r1->x;
	u32 r1right = r1->x+r1->w;
	u32 r1top = r1->y;
	u32 r1bottom = r1->y+r1->h;

	u32 r2left = r2->x;
	u32 r2right = r2->x+r2->w;
	u32 r2top = r2->y;
	u32 r2bottom = r2->y+r2->h;

	if(r1left <= r2left && r1right > r2left)
	{
		_x = 1;
	}
	else if(r1left > r2left && r1left < r2right)
	{
		_x = 1;
	}

	if(r1top <= r2top && r1bottom > r2top)
	{
		_y = 1;
	}
	else if(r1top > r2top && r1top < r2bottom)
	{
		_y = 1;
	}

	return (_x && _y);
}
