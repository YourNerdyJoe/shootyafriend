#ifndef _GAME_H_
#define _GAME_H_
#include <ctr/types.h>
#include "rect.h"
#include "LinkedList.h"

extern int playerLeftScore;
extern int playerRightScore;

void updateBarriers(Rect* b);
void updateBullets(LinkedList** lst,Rect* enemy,Rect* barriers,int speed);
void updatePlayer(Rect* player,LinkedList** lst,int right);
void renderBullets(u8* buf,LinkedList* lst,u8 r,u8 g,u8 b);
void deleteAllBullets(LinkedList** lst);
void drawScore(u8* buf);

#endif
