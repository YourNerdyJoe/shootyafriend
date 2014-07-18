#include "debug.h"
#include <ctr/types.h>
#include <ctr/FS.h>
#include <ctr/svc.h>
#include <stdio.h>
#include <string.h>
#include "3dsdefines.h"

Handle log;
extern Handle fsuHandle;
extern FS_archive sdmcArchive;
u64 log_pos = 0;

//see CFLAGS in Makefile for _DEBUG

Handle dbgInit()
{
#ifdef _DEBUG
	FS_path path = {PATH_CHAR, 11, (u8*)"/debug.txt"};
	FSUSER_OpenFileDirectly(fsuHandle,&log,sdmcArchive,path,FS_OPEN_WRITE|FS_OPEN_CREATE,FS_ATTRIBUTE_NONE);

	FSFILE_SetSize(log,0);

	dbgPrint("***** Debug Started *****\n");
#endif
	return log;
}

void dbgExit()
{
#ifdef _DEBUG
	dbgPrint("******** Exiting ********\n");
	FSFILE_Close(log);
#endif
}

void dbgPrint(const char* s)
{
#ifdef _DEBUG
	u32 len = strlen(s);
	u32 bytesWritten;
	FSFILE_Write(log,&bytesWritten,log_pos,(u32*)s,len,FS_ENABLE_FLUSH);
	log_pos += bytesWritten;
#endif
}
