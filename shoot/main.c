#include <string.h>
#include <malloc.h>
#include <ctr/types.h>
#include <ctr/APT.h>
#include <ctr/GSP.h>
#include <ctr/GX.h>
#include <ctr/HID.h>
#include <ctr/FS.h>
#include <ctr/srv.h>
#include <ctr/svc.h>
#include "3dsdefines.h"
#include "debug.h"
#include "image.h"
#include "game.h"
#include "LinkedList.h"
#include "input.h"

#define MAIN_FRAMEBUFFER_SIZE (MAIN_SCREEN_SIZE*3)	//400*240@24bpp
#define SUB_FRAMEBUFFER_SIZE (SUB_SCREEN_SIZE*3)	//320*240@24bpp

//fb vars
u8* topLeftFramebuffers[2];
u8* topRightFramebuffers[2];
u8* bottomFramebuffers[2];
u8 currentBuffer = 0;
u8* curBufAdr = NULL;
u8* leftBuf;
u8* rightBuf;
u8* bottomBuf;

//contains 1 if we have both left and right fb
//if 3d is off then left and right both point to 
//the left fb
//used in getRightFramebufferIfNecessary()
u8 is3d = 0;

u8* gspHeap;
u32* gxCmdBuf;
Handle gspEvent,gspSharedMemory;

Handle fsuHandle;
FS_archive sdmcArchive = {ARCID_SDMC, {PATH_EMPTY, 0, (u8*)""}};
//FS_archive romfsArchive = {ARCID_ROMFS, {PATH_EMPTY, 0, (u8*)""}};

//game objects
Image playerLeft={{16,88,32,64},{0,0,32,64},0};
Image playerRight={{352,88,32,64},{0,0,32,64},0};
Rect barriers[2] = {
	{64,96,16,48},
	{320,96,16,48},
};
LinkedList* bulletsLeft = NULL;
LinkedList* bulletsRight = NULL;

#define LEFT_WIN 2
#define RIGHT_WIN 3
Image winscreen={{20,0,360,240},{0,0,360,240},2};
u8 won = 0;

void gspGpuInit();
void gspGpuExit();
void getRightFramebufferIfNecessary();
void update();
void render();
void copyBuffer();
void swapBuffer();
void win(u32 imgindex);

int main(int argc,char* argv[])
{
	//Initialize
	initSrv();
	aptInit(APPID_APPLICATION);

	srv_getServiceHandle(NULL,&fsuHandle,"fs:USER");
	FSUSER_Initialize(fsuHandle);

	dbgInit();

	gspGpuInit();

	hidInit(NULL);
	aptSetupEventHandler();

	imageBankInit();

	u32 imgIndex = 1;
	playerLeft.imgIndex = playerRight.imgIndex = imgIndex;

	//Main Loop
	APP_STATUS status;
	while((status=aptGetStatus()) != APP_EXITING)
	{
		switch(status)
		{
		case APP_RUNNING:
			updatePAD();
			getRightFramebufferIfNecessary();
			if(!won)
			{
				update();
				render();
				if(playerLeftScore==0) won = RIGHT_WIN;
				if(playerRightScore==0) won = LEFT_WIN;
			}
			else
			{
				win(won);
			}
			copyBuffer();
			swapBuffer();
			break;

		case APP_SUSPENDING:
			aptReturnToMenu();
			break;

		case APP_SLEEPMODE:
			aptWaitStatusEvent();
			break;
		}
		svc_sleepThread(1000000000/60);
	}

	//Clean up and Exit
	deleteAllBullets(&bulletsLeft);
	deleteAllBullets(&bulletsRight);

	hidExit();
	gspGpuExit();
	dbgExit();
	aptExit();
	svc_exitProcess();
	return 0;
}

void gspGpuInit()
{
	gspInit();

	GSPGPU_AcquireRight(NULL,0x0);
	GSPGPU_SetLcdForceBlack(NULL,0x0);

	//get framebuffers
	GSPGPU_ReadHWRegs(NULL,REG_MAINLEFTFB,(u32*)topLeftFramebuffers,8);
	FB_PA_TO_VA(topLeftFramebuffers[0]);
	FB_PA_TO_VA(topLeftFramebuffers[1]);

	GSPGPU_ReadHWRegs(NULL,REG_MAINRIGHTFB,(u32*)topRightFramebuffers,8);
	FB_PA_TO_VA(topRightFramebuffers[0]);
	FB_PA_TO_VA(topRightFramebuffers[1]);

	GSPGPU_ReadHWRegs(NULL,REG_SUBFB,(u32*)bottomFramebuffers,8);
	FB_PA_TO_VA(bottomFramebuffers[0]);
	FB_PA_TO_VA(bottomFramebuffers[1]);

	u8 threadId;
	svc_createEvent(&gspEvent,RT_ONESHOT);
	GSPGPU_RegisterInterruptRelayQueue(NULL,gspEvent,0x1,&gspSharedMemory,&threadId);
	svc_mapMemoryBlock(gspSharedMemory,SHARED_MEM+0x2000,MEMPER_READWRITE,MEMPER_DONTCARE);

	svc_controlMemory((u32*)&gspHeap,0,0,GSP_HEAP_SIZE,MEMOP_MAP_GSP_HEAP,MEMPER_READWRITE);

	svc_waitSynchronization1(gspEvent,0x55bcb0);	//not sure why 0x55bcb0 nanoseconds
													//but thats what smea used

	gxCmdBuf = (u32*)(SHARED_MEM+0x2000 + 0x800+threadId*0x200);

	leftBuf	  = &gspHeap[MAIN_FRAMEBUFFER_SIZE*0];
	rightBuf  = &gspHeap[MAIN_FRAMEBUFFER_SIZE*1];
	bottomBuf = &gspHeap[MAIN_FRAMEBUFFER_SIZE*2];

	is3d = (REG_3D_SLIDERSTATE) ? 1 : 0;
}

void gspGpuExit()
{
	GSPGPU_UnregisterInterruptRelayQueue(NULL);
	svc_unmapMemoryBlock(gspSharedMemory,SHARED_MEM+0x2000);
	svc_closeHandle(gspSharedMemory);
	svc_closeHandle(gspEvent);

	gspExit();

	svc_controlMemory((u32*)&gspHeap,(u32)gspHeap,0,GSP_HEAP_SIZE,MEMOP_FREE,MEMPER_NONE);
}

/*
get right fb if we dont have it and 3d is enabled

(doesn't quite work but makes it less of a spaz)
(w/o this function left screen draws correctly
but right screen flashes black on alternating frames)
w/ this function:
(when game launches in 2d and switches to 3d, the
left screen alternates b/t left and right fb and 
the right screen consistantly draws the right fb)
(this works better b/c other than the 'you win'
screen the left and right frame buffers contain
the same data)
(when it launches in 3d it works regardless of 
this function)
*/
void getRightFramebufferIfNecessary()
{
	if(!is3d)
	{
		if(REG_3D_SLIDERSTATE)
		{
			GSPGPU_ReadHWRegs(NULL,REG_MAINLEFTFB,(u32*)topLeftFramebuffers,8);
			FB_PA_TO_VA(topLeftFramebuffers[0]);
			FB_PA_TO_VA(topLeftFramebuffers[1]);

			GSPGPU_ReadHWRegs(NULL,REG_MAINRIGHTFB,(u32*)topRightFramebuffers,8);
			FB_PA_TO_VA(topRightFramebuffers[0]);
			FB_PA_TO_VA(topRightFramebuffers[1]);

			if(	topLeftFramebuffers[0] != topRightFramebuffers[0] &&
				topLeftFramebuffers[1] != topRightFramebuffers[1] )
			{
				is3d = 1;
			}
		}
	}
}

void update()
{
	updatePlayer(&playerLeft.pos,&bulletsLeft,0);
	updatePlayer(&playerRight.pos,&bulletsRight,1);

	updateBarriers(barriers);

	updateBullets(&bulletsLeft,&playerRight.pos,barriers,4);
	updateBullets(&bulletsRight,&playerLeft.pos,barriers,-4);
}

/*void render()	//3d test
{
	int offset = 10*REG_3D_SLIDERSTATE;
	Rect fillRect = {0,0,400,240};
	Rect testRect = {50-offset,50,300,140};
	Rect testRect2 = {50+offset,50,300,140};
	Rect r1 = { 200,100,32,32 };
	Rect r2 = { 232,100,32,32 };

	drawRect(leftBuf,&fillRect,0,255,0);
	drawRect(leftBuf,&testRect,255,0,0);
	drawRect(leftBuf,&r1,0,0,255);

	drawRect(rightBuf,&fillRect,0,255,0);
	drawRect(rightBuf,&testRect2,255,0,0);
	drawRect(rightBuf,&r1,0,0,255);
}/**/

void render()
{
	Rect fillRect = {0,0,400,240};
	drawRect(leftBuf,&fillRect,0,0,0);
	drawRect(rightBuf,&fillRect,0,0,0);

	drawImage(leftBuf,&playerLeft,0);
	drawImage(rightBuf,&playerLeft,0);

	drawImage(leftBuf,&playerRight,1);
	drawImage(rightBuf,&playerRight,1);

	drawRect(leftBuf,&barriers[0],0,255,0);
	drawRect(rightBuf,&barriers[0],0,255,0);
	drawRect(leftBuf,&barriers[1],0,255,0);
	drawRect(rightBuf,&barriers[1],0,255,0);

	renderBullets(leftBuf,bulletsLeft,255,0,0);
	renderBullets(rightBuf,bulletsLeft,255,0,0);
	renderBullets(leftBuf,bulletsRight,255,0,0);
	renderBullets(rightBuf,bulletsRight,255,0,0);

	Rect fillSubRect = {0,0,320,240};
	drawRectSub(bottomBuf,&fillSubRect,0,0,0);
	drawScore(bottomBuf);
}/**/

void copyBuffer()
{
	u32 copiedBuffer = currentBuffer^1;
	GSPGPU_FlushDataCache(NULL,gspHeap,MAIN_FRAMEBUFFER_SIZE*2);

	//Left Main Screen
	GX_RequestDma(gxCmdBuf,(u32*)leftBuf,(u32*)topLeftFramebuffers[copiedBuffer],MAIN_FRAMEBUFFER_SIZE);

	//Right Main Screen
	//if(REG_3D_SLIDERSTATE != 0.0f)
		GX_RequestDma(gxCmdBuf,(u32*)rightBuf,(u32*)topRightFramebuffers[copiedBuffer],MAIN_FRAMEBUFFER_SIZE);

	//Sub Screen
	GX_RequestDma(gxCmdBuf,(u32*)bottomBuf,(u32*)bottomFramebuffers[copiedBuffer],SUB_FRAMEBUFFER_SIZE);
}

void swapBuffer()
{
	u32 regData;
	GSPGPU_ReadHWRegs(NULL,REG_MAINFB_INDEX,&regData,4);
	regData^=1;
	currentBuffer=regData&1;
	GSPGPU_WriteHWRegs(NULL,REG_MAINFB_INDEX,&regData,4);
	GSPGPU_WriteHWRegs(NULL,REG_SUBFB_INDEX,&regData,4);
}

void win(u32 imgindex)
{
	Rect fillRect = {0,0,400,240};

	drawRect(leftBuf,&fillRect,0,0,0);
	drawRect(rightBuf,&fillRect,0,0,0);

	//for crumy 3d effect
	int offset = 0;
	if(REG_3D_SLIDERSTATE) offset = 5*REG_3D_SLIDERSTATE;

	winscreen.imgIndex = imgindex;
	winscreen.pos.x = 20 + offset;

	drawImage(leftBuf,&winscreen,0);

	winscreen.pos.x = 20 - offset;

	drawImage(rightBuf,&winscreen,0);

	if(isKeyPressed(PAD_START))
	{
		won = 0;
		playerLeftScore = 160;
		playerRightScore = 160;
		deleteAllBullets(&bulletsLeft);
		deleteAllBullets(&bulletsRight);
	}
}
