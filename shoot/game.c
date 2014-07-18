#include "game.h"
#include "image.h"
#include <stdio.h>
#include "3dsdefines.h"
#include <ctr/HID.h>
#include <malloc.h>
#include "input.h"
#include "debug.h"

#define DAMAGE 10
#define PLAYER_SPEED 4

int barrierDirection[2] = {1,-1};

int playerLeftScore = 160;
int playerRightScore = 160;

void updateBarriers(Rect* b)
{
	int i;
	for(i = 0; i < 2; i++)
	{
		b[i].y += barrierDirection[i];
		if(b[i].y <= 0 || b[i].y+b[i].h >= 240)
			barrierDirection[i] *= -1;
	}
}

static LinkedList* deleteNode(LinkedList* prev,LinkedList* it)
{
	LinkedList* next = it->next;
	if(prev)
	{
		//check valid
		if(prev->next != it)
			return NULL;

		prev->next = next;
	}

	free(it);

	if(!prev)
		return next;
	else
		return NULL;
}

static void deleteNodeFromList(LinkedList** lst,LinkedList* prev,LinkedList* it)
{
	//not NULL if new start of list
	LinkedList* result = deleteNode(prev,it);
	if(!prev) *lst = result;
}

void updateBullets(LinkedList** lst,Rect* enemy,Rect* barriers,int speed)
{
	LinkedList* it;
	LinkedList* prev = NULL;
	for(it = *lst; it != NULL;)
	{
		Rect* r = (Rect*)it->data;
		r->x += speed;

		LinkedList* cur = it;
		it=it->next;

		if(r->x+r->w < 0 || r->x > 400)
		{
			//destroy bullet
			deleteNodeFromList(lst,prev,cur);
		}
		else if(checkCollision(&barriers[0],r))
		{
			//destroy bullet
			deleteNodeFromList(lst,prev,cur);
		}
		else if(checkCollision(&barriers[1],r))
		{
			//destroy bullet
			deleteNodeFromList(lst,prev,cur);
		}
		else if(checkCollision(enemy,r))
		{
			if(speed > 0)
			{
				playerRightScore -= DAMAGE;
			}
			else
			{
				playerLeftScore -= DAMAGE;
			}
			//destroy bullet
			deleteNodeFromList(lst,prev,cur);
		}
		else
		{
			prev = cur;
		}
	}
}

#define BULLET_SIZE 8

static void createBullet(LinkedList** lst,u32 x,u32 y)
{
	//find end
	LinkedList* end;
	if(*lst)
	{
		end = *lst;
		while(end->next!=NULL) end = end->next;
	}

	LinkedList* node = (LinkedList*)malloc(sizeof(LinkedList*) + sizeof(Rect));
	node->next = NULL;
	Rect* r = (Rect*)node->data;
	r->x = x;
	r->y = y;
	r->w = BULLET_SIZE;
	r->h = BULLET_SIZE;

	//dbgPrint("create bullet ");
	//char s[32];
	//sprintf(s,"%p %d %d %d %d\n",node,r->x,r->y,r->w,r->h);
	//dbgPrint(s);

	if(*lst)
		end->next = node;
	else
		*lst = node;
}

void updatePlayer(Rect* player,LinkedList** lst,int right)
{
	//playerRight
	if(right)
	{
		if(isKeyDown(PAD_B))
		{
			player->y -= PLAYER_SPEED;
		}
		if(isKeyDown(PAD_X))
		{
			player->y += PLAYER_SPEED;
		}
		if(isKeyPressed(PAD_R))
		{
			//shoot
			createBullet(lst,player->x-3,player->y+25);
		}
	}
	else	//playerLeft
	{
		if(isKeyDown(PAD_DOWN))
		{
			player->y -= PLAYER_SPEED;
		}
		if(isKeyDown(PAD_UP))
		{
			player->y += PLAYER_SPEED;
		}
		if(isKeyPressed(PAD_L))
		{
			//shoot
			createBullet(lst,player->x+27,player->y+25);
		}
	}

	if((s32)player->y < 0) player->y = 0;
	else if(player->y+player->h > 240) player->y = 240-player->h;
}

void renderBullets(u8* buf,LinkedList* lst,u8 r,u8 g,u8 b)
{
	LinkedList* it;
	for(it = lst; it!=NULL; it=it->next)
	{
		Rect* rec = (Rect*)it->data;
		drawRect(buf,rec,r,g,b);
	}
}

void deleteAllBullets(LinkedList** lst)
{
	LinkedList *it,*next;
	for(it = *lst; it!=NULL; it=next)
	{
		next = it->next;
		
		//char s[32];
		//sprintf(s,"delete bullet %p\n",it);
		//dbgPrint(s);

		free(it);
	}
	*lst = NULL;
}

void drawScore(u8* buf)
{
	u32 w1,w2;
	w1 = playerLeftScore > 0 ? playerLeftScore : 0;
	w2 = playerRightScore > 0 ? playerRightScore : 0;

	Rect s1 = {0,224,w1,16};
	Rect s2 = {320-w2,224,w2,16};
	drawRectSub(buf,&s1,255,255,255);
	drawRectSub(buf,&s2,255,255,255);
}
