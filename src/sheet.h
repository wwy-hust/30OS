/** @file sheet.h
 *  @brief basic functions related to sheet.
 *
 *  All drawing action operate on sheet. The sheet is similiar to Layer. It can hold pixels.
 *  All movement of action also based on sheet, The sheet is managed by SHEET_CTL. These file contains
 *  some utils functions used to operate the sheet. 
 *  There are two ways to redraw the screen. 1st, directly call sheet_draw_sub(), sheet_draw_all() or sheet_draw_sub_withoutMouse().
 *  That would be redrawn immediately. 2nd, call sheet_add_redraw_region_rect() or sheet_add_redraw_region() to add redraw request.
 *  That would be redrawn by sheet_daemon when CPU is idle. 
 */
 
#ifndef _SHEET_H_
#define _SHEET_H_

#include "typedef.h"
#include "asmhead.h"
#include "funcUtils.h"
#include "mem.h"

/** @addtogroup sheet
 *  @brief this module manage the sheets on screen. with this module, you could paint on different sheet, move and do other operation.
 *  @{
 */
#define NONE_COL		16		 /**< 16:transparency*/
#define MAX_SHEET_NUM			256
#define REDRAW_LIST_LEN		1024

/** @brief struct of one sheet. All draw operation working on the structure. */
typedef struct SHEET{
	uint8* buf;   /**< the buffer of this sheet */
	int32 width;  /**< the width of this sheet. in current design, It can not be modified after created. */
	int32 height; /**< the height of this sheet. in current design, It can not be modified after created. */
	int32 topLeftX;  /**< the x value of top left pos. */
	int32 topLeftY;  /**< the y value of top left pos. */
	int32	z;      /**< the layer index of this sheet. higher value means in more front. */
	int8	flags;  /**< the transparency color of this sheet. */
	uint8	no;     /**< the number of this sheet. */
}SHEET;

/** @brief struct of sheetCtl. OS use this structure to manage all sheets. This struct contains mouse sheet, normal sheets, redraw regions and so on. */
typedef struct SHEET_CTL{
	uint8* vram;  /**< the real video ram. draw in this address will display on the screen. */
	uint8* resVram;  /**< the buffer to draw before draw into real ram. */
	int32 width;  /**< the width of the whole screen. */
	int32 height; /**< the height of the whole screen. */
	int32	topZ;   /**< the biggest value of z among current sheets. */
	SHEET*	sortedSheet[MAX_SHEET_NUM];  /**< the sheets in ascendent order. */
	SHEET		sheets[MAX_SHEET_NUM];  /**< all the sheets. */
	SHEET		mouseSheet;  /**< mouse sheet. */
	
	RECT		redraw_rect;  /**< the redraw region requested by user. It would be redraw when redraw_daemon is running. */
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
/// @}

#endif
