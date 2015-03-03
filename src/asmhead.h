
#ifndef _ASM_HEAD_H_
#define _ASM_HEAD_H_

#include "typedef.h"

typedef struct BOOTINFO {
	int8 cylinders;
	int8 leds;
	int8 vmode;
	int8 reserved;
	int16 screenWidth;
	int16 screenHeight;
	int8* vram;
}BOOTINFO;

#define ADR_DISKIMG			0x00100000
#define ADR_BOOTINFO		0x00000ff0

#endif

