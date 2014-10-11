
#ifndef _SHEET_H_
#define _SHEET_H_

#include "typedef.h"
#include "asmhead.h"
#include "funcUtils.h"
#include "mem.h"

#define NONE_COL		16		 /* 16:Í¸Ã÷É« */
#define MAX_SHEET_NUM			256
#define REDRAW_LIST_LEN		1024

typedef struct SHEET{
	uint8* buf;
	int32 width;
	int32 height;
	int32 topLeftX;
	int32 topLeftY;
	int32	z;
	int8	flags;
	uint8	no;
}SHEET;

typedef struct SHEET_CTL{
	uint8* vram;
	uint8* resVram;
	int32 width;
	int32 height;
	int32	topZ;
	SHEET*	sortedSheet[MAX_SHEET_NUM];
	SHEET		sheets[MAX_SHEET_NUM];
	SHEET		mouseSheet;
	
	RECT		redraw_rect;
}SHEET_CTL;

void sheet_init(uint8* resVram);
boolean sheet_add(IN uint8* buf, IN int32 width, IN int32 height, IN int32 topLeftX, IN int32 topLeftY, IN int8 flags, OUT SHEET** sheet);
boolean sheet_add_mouse(IN uint8* buf, IN int32 width, IN int32 height, IN int32 topLeftX, IN int32 topLeftY, IN int8 flags, OUT SHEET** sheet);
boolean sheet_del(IN SHEET* sheet);
void sheet_sort();
boolean sheet_pin_top(IN SHEET* sheet);

void sheet_draw_all();
void sheet_draw_sub(IN int32 topLeftX, IN int32 topLeftY, IN int32 bottomRightX, IN int32 bottomRightY);
void sheet_draw_sub_withoutMouse(IN int32 topLeftX, IN int32 topLeftY, IN int32 bottomRightX, IN int32 bottomRightY);

boolean sheet_add_redraw_region(IN int32 topLeftX, IN int32 topLeftY, IN int32 bottomRightX, IN int32 bottomRightY);
boolean sheet_add_redraw_region_rect(IN RECT rect);
void sheet_draw_deamon();

#endif
