
#ifndef _GRAPHIC_H_
#define _GRAPHIC_H_

#include "typedef.h"
#include "asmhead.h"
#include "funcUtils.h"
#include "sheet.h"
#include "multiTask.h"
#include "fat.h"

/** @brief Color used in drawing, only 16 kinds of color */
#define BLACK 			0      /*  0:ºÚ */      
#define RED					1      /*  1:ÁÁºì */    
#define GREEN				2      /*  2:ÁÁÂÌ */    
#define YELLOW			3      /*  3:ÁÁ»Æ */    
#define BLUE				4      /*  4:ÁÁÀ¶ */    
#define PURPLE			5      /*  5:ÁÁ×Ï */    
#define LIGHTBLUE		6      /*  6:Ç³ÁÁÀ¶ */  
#define WHITE				7      /*  7:°× */      
#define LIGHTGREY		8      /*  8:ÁÁ»Ò */    
#define DARKRED			9      /*  9:°µºì */    
#define DARKGREEN		10     /* 10:°µÂÌ */    
#define DARKYELLOW	11     /* 11:°µ»Æ */    
#define DARKBLUE		12     /* 12:°µÀ¶ */    
#define DARKPURPLE	13     /* 13:°µ×Ï */    
#define DARKGRASS		14     /* 14:°µÇà */  
#define GREY				15     /* 15:°µ»Ò */
#define NONE_COL		16		 /* 16:Í¸Ã÷É« */

void init_palette(void);
void set_palette(int32 start, int32 end, uint8 *rgb);
void fill_box(SHEET* sheet, uint8 color, int32 xtopLeft, int32 ytopLeft, int32 xBottomRight, int32 yBottomRight);
void fill_box_with_buf(SHEET* sheet, uint8* buf, int32 xtopLeft, int32 ytopLeft, int32 xBottomRight, int32 yBottomRight);
void put_ascii(SHEET* sheet, uint8 ascii, uint8 colorNum, int32 x, int32 y);
void put_gbk(SHEET* sheet, uint16 gbk, uint8 colorNum, int32 x, int32 y);
void puts_str(SHEET* sheet, uint8* str, uint8 colorNum, int32 x, int32 y);
void draw_point(SHEET* sheet, int32 x, int32 y, uint8 color);
void draw_line(SHEET* sheet, int32 x0, int32 y0, int32 x1, int32 y1, uint8 color);
void copy_buf(SHEET* sheet, int32 width, int32 height, int32 toTopLeftX, int32 toTopLeftY, int32 fromTopLeftX, int32 fromTopLeftY);


void draw_default_wnd(SHEET* sheet, int8* title, uint8* icon, int32 topLeftX, int32 topLeftY, int32 bottomRightX, int32 bottomRightY);
void draw_console_wnd(SHEET* sheet, int8* title, uint8* icon, int32 topLeftX, int32 topLeftY, int32 bottomRightX, int32 bottomRightY);


//**** basic function ****//
boolean pos_inside_rect(IN RECT* rect, IN POS* pos);
boolean rect_inside_rect(IN RECT* smallRect, IN RECT* bigRect);
int32 rect_get_area(IN RECT* rect);


#endif
