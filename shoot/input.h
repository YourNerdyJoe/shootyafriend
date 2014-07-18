#ifndef _INPUT_H_
#define _INPUT_H_
#include <ctr/types.h>
#include "3dsdefines.h"
#include <ctr/HID.h>

extern u32 PAD,prevPAD;

static inline void updatePAD() {
	prevPAD = PAD;
	PAD = WORD(SHARED_MEM+PAD_STATE);
}
static inline u32 isKeyDown(u32 key) {
	return PAD & key;
}
static inline u32 isKeyPressed(u32 key) {
	return (PAD & key) & (~prevPAD & key);
}
static inline u32 isKeyReleased(u32 key) {
	return (~PAD & key) & (prevPAD & key);
}

#endif
