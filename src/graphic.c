/** @file graphic.c
 *  @brief basic functions related to painting.
 *
 *  The graphic include init palette, print rectangle, print characters, draw point & line.
 */

#include "graphic.h"

// font 
extern int8 hankaku[4096]; ///< chracter matrix of ascii & number.
extern uint8 icon_close[256]; ///< picture matrix of close icon of windows.

/** @brief init palette.
 *  After the palette is initialized. we could draw the color with index in the palette directly.
 *  Only 16 color available in current environment.
 *  @param NULL
 *  @return NULL
 */
void init_palette(void)
{
	static uint8 table_rgb[16 * 3] = {
	/*  R		  G			B  */
		0x00, 0x00, 0x00,	/*  0:黑 */
		0xff, 0x00, 0x00,	/*  1:亮红 */
		0x00, 0xff, 0x00,	/*  2:亮绿 */
		0xff, 0xff, 0x00,	/*  3:亮黄 */
		0x00, 0x00, 0xff,	/*  4:亮蓝 */
		0xff, 0x00, 0xff,	/*  5:亮紫 */
		0x00, 0xff, 0xff,	/*  6:浅亮蓝 */
		0xff, 0xff, 0xff,	/*  7:白 */
		0xc6, 0xc6, 0xc6,	/*  8:亮灰 */
		0x84, 0x00, 0x00,	/*  9:暗红 */
		0x00, 0x84, 0x00,	/* 10:暗绿 */
		0x84, 0x84, 0x00,	/* 11:暗黄 */
		0x00, 0x00, 0x84,	/* 12:暗青 */
		0x84, 0x00, 0x84,	/* 13:暗紫 */
		0x00, 0x84, 0x84,	/* 14:浅暗蓝 */
		0x84, 0x84, 0x84	/* 15:暗灰 */
	};
	static uint8 table_rgb2[216 * 3];
	int r, g, b;
	set_palette(0, 15, table_rgb);
	for(b = 0; b < 6; b++) {
		for(g = 0; g < 6; g++) {
			for(r = 0; r < 6; r++) {
				table_rgb2[(r + g * 6 + b * 36) * 3 + 0] = r * 51;
				table_rgb2[(r + g * 6 + b * 36) * 3 + 1] = g * 51;
				table_rgb2[(r + g * 6 + b * 36) * 3 + 2] = b * 51;				
			}
		}
	}
	set_palette(16, 231, table_rgb2);
	return;
}

/** @brief set palette.
 *  Set the palette from start to end with the value of rgb.
 *  @note The length of array rgb should equals to 3 * (end - start).
 *  @param start the start address of palette, could be int from 0 to 15.
 *  @param end the end address of palette, could be int from 0 to 15.
 *  @param rgb the rgb array used to put into the palette. watch out for the length of rgb, length of rgb should equals 3 * (end - start)
 *  @return NULL
 */
void set_palette(int32 start, int32 end, uint8 *rgb)
{
	int32 i, eflags;
	eflags = io_load_eflags();	/* 获得标志寄存器EFLAG的值 */
	io_cli(); 					/* 将中断许可标志置为0，禁止中断 */
	io_out8(0x03c8, start);
	for (i = start; i <= end; i++) {
		io_out8(0x03c9, rgb[0] / 4);
		io_out8(0x03c9, rgb[1] / 4);
		io_out8(0x03c9, rgb[2] / 4);
		rgb += 3;
	}
	io_store_eflags(eflags);	/* 恢复中断许可标志 */
	return;
}

/** @brief draw a rectangle.
 *  Draw a rectangle with assigned color and assigned area on given sheet.
 *  @note The sheet can not be invalid. That would cause unexpected error.
 *  @param sheet The sheet to draw on. Can get from sheet_add.
 *  @param color The index of color in palette.
 *  @param xtopLeft x value of left top position.
 *  @param ytopLeft y value of left top position.
 *  @param xBottomRight x value of right bottom position.
 *  @param yBottomRight y value of right bottom position.
 *  @return NULL
 */
void fill_box(SHEET* sheet, uint8 color, int32 xtopLeft, int32 ytopLeft, int32 xBottomRight, int32 yBottomRight)
{
	int32 x, y;
	for(y = ytopLeft; y < yBottomRight; y++) {
		for(x = xtopLeft; x < xBottomRight; x++) {
			sheet->buf[y * sheet->width + x] = color;
		}
	}
}

/** @brief draw a rectangle with given buf.
 *  Draw a rectangle with assigned buf and assigned area on given sheet.
 *  @note The sheet can not be invalid. That would cause unexpected error.
 *  @param sheet The sheet to draw on. Can get from sheet_add.
 *  @param buf The buffer instead of color index, That could be used draw some picture on sheet.
 *  @param xtopLeft x value of left top position.
 *  @param ytopLeft y value of left top position.
 *  @param xBottomRight x value of right bottom position.
 *  @param yBottomRight y value of right bottom position.
 *  @return NULL
 */
void fill_box_with_buf(SHEET* sheet, uint8* buf, int32 xtopLeft, int32 ytopLeft, int32 xBottomRight, int32 yBottomRight)
{
	int32 x, y, m = 0;
	for(y = ytopLeft; y < yBottomRight; y++) {
		for(x = xtopLeft; x < xBottomRight; x++) {
			if(buf[m] != sheet->flags)
				sheet->buf[y * sheet->width + x] = buf[m++];
			else
				m++;
		}
	}
}

/** @brief print a ascii character on specific position.
 *  Draw a ascii character with assigned color and assigned pos on given sheet.
 *  @note The sheet can not be invalid. That would cause unexpected error.
 *  @param sheet The sheet to draw on. Can get from sheet_add.
 *  @param ascii The ascii character to draw.
 *  @param colorNum The index of color in palette.
 *  @param x x value of left top position.
 *  @param y y value of left top position.
 *  @return NULL;
 */
void put_ascii(SHEET* sheet, uint8 ascii, uint8 colorNum, int32 x, int32 y)
{
	int32 tmpX, tmpY;
	
	for(tmpY = y; tmpY < y + 16; tmpY++) {
		for(tmpX = 0; tmpX < 8; tmpX++) {
			if((((hankaku + (ascii * 16))[tmpY - y] << tmpX) & 0x80) == 0x80) {
				sheet->buf[tmpY * sheet->width + x + tmpX] = colorNum;
			}
		}
	}
}

extern FAT_CTL fatCtl; /**< Used to load GBK character matrix for drawing chinese*/
/** @brief print a chinese gbk character on specific position.
 *  Draw a chinese gbk character with assigned color and assigned pos on given sheet.
 *  @note The sheet can not be invalid. That would cause unexpected error.
 *  @param sheet The sheet to draw on. Can get from sheet_add.
 *  @param gbk The chinese character to draw.
 *  @param colorNum The index of color in palette.
 *  @param x x value of left top position.
 *  @param y y value of left top position.
 *  @return NULL;
 */
void put_gbk(SHEET* sheet, uint16 gbk, uint8 colorNum, int32 x, int32 y)
{
	int32 tmpX, tmpY;
  uint32 offset;  
	uint8* hzk16Base;
	
	if((gbk & 0x00FF) >= 0xA1 && ((gbk >> 8) & 0x00FF) >= 0xA1) {
		hzk16Base = fatCtl.fontFile->buf;
	  offset = (((gbk & 0x00FF) - 0xa1) * 94 + (((gbk >> 8) & 0x00FF) - 0xa1)) * 32;     //根据内码找出汉字在HZK16中的偏移位置  
	
		for(tmpY = y; tmpY < 16 + y; tmpY++) {
			for(tmpX = 0; tmpX < 8; tmpX++) {
				if(((hzk16Base[offset] << tmpX) & 0x80) == 0x80) {
					sheet->buf[tmpY * sheet->width + x + tmpX] = colorNum;
				}
			}
			offset++;
			for(tmpX = 0; tmpX < 8; tmpX++) {
				if(((hzk16Base[offset] << tmpX) & 0x80) == 0x80) {
					sheet->buf[tmpY * sheet->width + x + tmpX + 8] = colorNum;
				}
			}
			offset++;
		}
	}
	else {
		put_ascii(sheet, (gbk & 0x00FF), colorNum, x, y);
		put_ascii(sheet, ((gbk >> 8) & 0x00FF), colorNum, x + 8, y);
	}
}

/** @brief print strings on specific position, the string could be mixture of gbk & ascii.
 *  Draw a string with assigned color and assigned pos on given sheet.
 *  @note The sheet can not be invalid. That would cause unexpected error.
 *  @param sheet The sheet to draw on. Can get from sheet_add.
 *  @param str The string to draw, the str could be mixture of gbk & ascii.
 *  @param colorNum The index of color in palette.
 *  @param x x value of left top position.
 *  @param y y value of left top position.
 *  @return NULL;
 */
void puts_str(SHEET* sheet, uint8* str, uint8 colorNum, int32 x, int32 y)
{
	int32 i = 0, rowLetterCnt, xTmp;
	uint16 wStr;
	
	rowLetterCnt = _strlen(str);
	for(i = 0, xTmp = x; i < rowLetterCnt;) {
		if(str[i] < 0xA1) {
			put_ascii(sheet, str[i], colorNum, xTmp, y);
			xTmp += 8;
			i++;
		}
		else {
			wStr = (str[i] & 0x00FF) | ((((uint16)str[i + 1]) << 8) & 0xFF00);
			put_gbk(sheet, wStr, colorNum, xTmp, y);
			xTmp += 16;
			i += 2;
		}
	}
}

/** @brief draw a point on specific position.
 *  Draw a dot with assigned color and assigned pos on given sheet.
 *  @note The sheet can not be invalid. That would cause unexpected error.
 *  @param sheet The sheet to draw on. Can get from sheet_add.
 *  @param x x value of left top position.
 *  @param y y value of left top position.
 *  @param color The index of color in palette.
 *  @return NULL;
 */
void draw_point(SHEET* sheet, int32 x, int32 y, uint8 color)
{
	sheet->buf[y * sheet->width + x] = color;
}

/** @brief draw a line on specific position.
 *  Draw a line with assigned color and assigned pos on given sheet.
 *  @note The sheet can not be invalid. That would cause unexpected error.
 *  @param sheet The sheet to draw on. Can get from sheet_add.
 *  @param x0 x value of one end.
 *  @param y0 y value of one end.
 *  @param x1 x value of the other end.
 *  @param y1 y value of the other end.
 *  @param color The index of color in palette.
 *  @return NULL;
 */
void draw_line(SHEET* sheet, int32 x0, int32 y0, int32 x1, int32 y1, uint8 color)
{
	int32 dx, dy, x, y, len, i;
	dx = x1 - x0;
	dy = y1 - y0;
	x = x0 << 10;
	y = y0 << 10;
	dx = (dx < 0) ? -dx : dx;
	dy = (dy < 0) ? -dy : dy;
	if(dx >= dy) {
		len = dx + 1;
		dx = (x1 > x0) ? 1024 : -1024;
		dy = (y1 >= y0) ? (((y1 - y0 + 1) << 10) / len) : (((y1 - y0 - 1) << 10) / len);
	}
	else {
		len = dy + 1;
		dy = (y1 > y0) ? 1024 : -1024;
		dx = (x1 >= x0) ? (((x1 - x0 + 1) << 10) / len) : (((x1 - x0 - 1) << 10) / len);
	}
	for(i = 0; i < len; i++) {
		sheet->buf[(y >> 10) * sheet->width + (x >> 10)] = color;
		y += dy;
		x += dx;
	}
}

/** @brief copy buffer from one area to another.
 *  Copy buffer on given sheet from one area to another.
 *  @note The sheet can not be invalid. That would cause unexpected error.
 *  @param sheet The sheet to draw on. Can get from sheet_add.
 *  @param width the width of copyed area.
 *  @param height the height of copyed area.
 *  @param toTopLeftX x value of left top pos of dst area.
 *  @param toTopLeftY y value of left top pos of dst area.
 *  @param fromTopLeftX x value of left top pos of src area.
 *  @param fromTopLeftY y value of left top pos of src area.
 *  @return NULL;
 */
void copy_buf(SHEET* sheet, int32 width, int32 height, int32 toTopLeftX, int32 toTopLeftY, int32 fromTopLeftX, int32 fromTopLeftY)
{
	uint8* buf = sheet->buf;
	int32 x, y, width1;
	width1 = sheet->width;
	for(x = 0; x < width; x++) {
		for(y = 0; y < height; y++) {
			buf[width1 * (y + toTopLeftY) + toTopLeftX + x] = buf[width1 * (y + fromTopLeftY) + fromTopLeftX + x];
		}
	}
}

/** @brief draw a window on given sheet with specific title ,icon and position.
 *  draw a window similar to Microsoft Window's about window.
 *  @note The sheet can not be invalid. That would cause unexpected error.
 *  @param sheet The sheet to draw on. Can get from sheet_add.
 *  @param title the title draw on the window title bar.
 *  @param icon the icon of the window, would be drawn on top left of the window.
 *  @param topLeftX x value of left top pos.
 *  @param topLeftY y value of left top pos.
 *  @param bottomRightX x value of right bottom pos.
 *  @param bottomRightY y value of right bottom pos.
 *  @return NULL;
 */
void draw_default_wnd(SHEET* sheet, int8* title, uint8* icon, int32 topLeftX, int32 topLeftY, int32 bottomRightX, int32 bottomRightY)
{
	int8 title_len = 0;
	int32 max_title_len = 0;
	for(; title[title_len] != 0; title_len++);
	max_title_len = bottomRightX - topLeftX - 40;
	max_title_len /= 8;
	if(title_len > max_title_len) {
		title[max_title_len - 3] = '.';
		title[max_title_len - 2] = '.';
		title[max_title_len - 1] = 0;
	}
	
	fill_box(sheet, LIGHTGREY, topLeftX, topLeftY, bottomRightX, topLeftY + 1);
	fill_box(sheet, WHITE, topLeftX + 1, topLeftY + 1, bottomRightX - 1, topLeftY + 2);
	fill_box(sheet, LIGHTGREY, topLeftX, topLeftY, topLeftX + 1, bottomRightY);
	fill_box(sheet, WHITE, topLeftX + 1, topLeftY + 1, topLeftX + 2, bottomRightY - 1);
	fill_box(sheet, GREY, bottomRightX - 2, topLeftY + 1, bottomRightX - 1, bottomRightY - 1);
	fill_box(sheet, BLACK, bottomRightX - 1, topLeftY, bottomRightX, bottomRightY);
	fill_box(sheet, LIGHTGREY, topLeftX + 2, topLeftY + 2, bottomRightX - 3, bottomRightY - 3);
	fill_box(sheet, DARKBLUE, topLeftX + 3, topLeftY + 3, bottomRightX - 4, topLeftY + 21);
	fill_box(sheet, GREY, topLeftX + 1, bottomRightY - 2, bottomRightX - 2, bottomRightY - 1);

	fill_box_with_buf(sheet, icon, topLeftX + 4, topLeftY + 4, topLeftX + 20, topLeftY + 20);
	fill_box_with_buf(sheet, icon_close, bottomRightX - 20, topLeftY + 4, bottomRightX - 4, topLeftY + 20);
	puts_str(sheet, title, WHITE, 4 + 17, 4);
}

/** @brief draw a console window on given sheet with specific title ,icon and position.
 *  draw a window similar to Microsoft Window's cmd.
 *  @note The sheet can not be invalid. That would cause unexpected error.
 *  @param sheet The sheet to draw on. Can get from sheet_add.
 *  @param title the title draw on the window title bar.
 *  @param icon the icon of the window, would be drawn on top left of the window.
 *  @param topLeftX x value of left top pos.
 *  @param topLeftY y value of left top pos.
 *  @param bottomRightX x value of right bottom pos.
 *  @param bottomRightY y value of right bottom pos.
 *  @return NULL;
 */
void draw_console_wnd(SHEET* sheet, int8* title, uint8* icon, int32 topLeftX, int32 topLeftY, int32 bottomRightX, int32 bottomRightY)
{
	int8 title_len = 0;
	int32 max_title_len = 0;
	for(; title[title_len] != 0; title_len++);
	max_title_len = bottomRightX - topLeftX - 40;
	max_title_len /= 8;
	if(title_len > max_title_len) {
		title[max_title_len - 3] = '.';
		title[max_title_len - 2] = '.';
		title[max_title_len - 1] = 0;
	}
	
	fill_box(sheet, LIGHTGREY, topLeftX, topLeftY, bottomRightX, topLeftY + 1);
	fill_box(sheet, WHITE, topLeftX + 1, topLeftY + 1, bottomRightX - 1, topLeftY + 2);
	fill_box(sheet, LIGHTGREY, topLeftX, topLeftY, topLeftX + 1, bottomRightY);
	fill_box(sheet, WHITE, topLeftX + 1, topLeftY + 1, topLeftX + 2, bottomRightY - 1);
	fill_box(sheet, GREY, bottomRightX - 2, topLeftY + 1, bottomRightX - 1, bottomRightY - 1);
	fill_box(sheet, BLACK, bottomRightX - 1, topLeftY, bottomRightX, bottomRightY);

	// 将 LIGHTGREY 的底色拆开绘制，减少绘制的面积
	fill_box(sheet, LIGHTGREY, topLeftX + 2, topLeftY + 2, bottomRightX - 3, topLeftY + 25);				// UP
	fill_box(sheet, LIGHTGREY, topLeftX + 2, bottomRightY - 6, bottomRightX - 3, bottomRightY - 3);	// DOWN
	fill_box(sheet, LIGHTGREY, topLeftX + 2, topLeftY + 2, topLeftX + 5, bottomRightY - 3);					// LEFT
	fill_box(sheet, LIGHTGREY, bottomRightX - 8, topLeftY + 2, bottomRightX - 3, bottomRightY - 3);	// RIGHT
		
	fill_box(sheet, GREY, topLeftX + 3, topLeftY + 24, topLeftX + 4, bottomRightY - 6);			// 左边灰色条
	fill_box(sheet, GREY, topLeftX + 4, topLeftY + 23, bottomRightX - 6, topLeftY + 24);		// 上边灰色条
	fill_box(sheet, GREY, bottomRightX - 6, topLeftY + 23, bottomRightX - 5, topLeftY + 24);// 下边灰色条
	
	fill_box(sheet, BLACK, topLeftX + 4, topLeftY + 24, bottomRightX - 6, bottomRightY - 6);				// 中间的黑色
	
	fill_box(sheet, DARKBLUE, topLeftX + 3, topLeftY + 3, bottomRightX - 4, topLeftY + 21);
	fill_box(sheet, GREY, topLeftX + 1, bottomRightY - 2, bottomRightX - 2, bottomRightY - 1);
	fill_box_with_buf(sheet, icon, topLeftX + 4, topLeftY + 4, topLeftX + 20, topLeftY + 20);
	fill_box_with_buf(sheet, icon_close, bottomRightX - 20, topLeftY + 4, bottomRightX - 4, topLeftY + 20);
	puts_str(sheet, title, WHITE, 4 + 17, 4);
}

/** @brief test if a point is in a rectangle.
 *  @param rect the rectangle.
 *  @param pos the position of pos.
 *  @return whether the point is in the rectangle, TRUE if in else FALSE;
 */
boolean pos_inside_rect(IN RECT* rect, IN POS* pos)
{
	if(pos->x < rect->topLeftX || pos->x > rect->bottomRightX) {
		return FALSE;
	}
	if(pos->y < rect->topLeftY || pos->y > rect->bottomRightY) {
		return FALSE;
	}
	return TRUE;
}

/** @brief test if a rectangle is in another rectangle.
 *  @param smallRect the rectangle.
 *  @param bigRect the rectangle probably contain the small one.
 *  @return whether the small rect is in the big rect, TRUE if in else FALSE;
 */
boolean rect_inside_rect(IN RECT* smallRect, IN RECT* bigRect)
{
	if((smallRect->topLeftX < bigRect->topLeftX) || (smallRect->topLeftY < bigRect->topLeftY)) {
		return FALSE;
	}
	else if((smallRect->bottomRightX > bigRect->bottomRightX) || (bigRect->bottomRightY > bigRect->bottomRightY)) {
		return FALSE;
	}
	else {
		return TRUE;
	}
}

/** @brief get the area value of a rectangle.
 *  @param rect the rectangle.
 *  @return the area value of the given rectangle;
 */
int32 rect_get_area(IN RECT* rect)
{
	return (rect->bottomRightX - rect->topLeftX) * (rect->bottomRightY - rect->topLeftY);
}
