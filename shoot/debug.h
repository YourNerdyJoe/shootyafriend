#ifndef _DEBUG_H_
#define _DEBUG_H_
#include <ctr/types.h>

/* Simple Debug Output to SD Card */

Handle dbgInit();
void dbgExit();

void dbgPrint(const char* s);

#endif
