/** @file sheet.c
 *  @brief basic functions related to sheet.
 *
 *  All drawing action operate on sheet. The sheet is similiar to Layer. It can hold pixels.
 *  All movement of action also based on sheet, The sheet is managed by SHEET_CTL. These file contains
 *  some utils functions used to operate the sheet. 
 *  There are two ways to redraw the screen. 1st, directly call sheet_draw_sub(), sheet_draw_all() or sheet_draw_sub_withoutMouse().
 *  That would be redrawn immediately. 2nd, call sheet_add_redraw_region_rect() or sheet_add_redraw_region() to add redraw request.
 *  That would be redrawn by sheet_daemon when CPU is idle. 
 */

#include "sheet.h"

SHEET_CTL sheetCtl; /**< The Struct contain all the sheets */

/** @brief init sheetCtl.
 *  Called by OS to initialize sheet module, double buffer scheme is used to assure anti-shake.
 *  @param resVram the address of reserved video ram. It is used to accomplish double buffer scheme. 
 *                 First draw everything on resVram, then draw resVram to the real video ram.
 *  @return NULL
 */
void sheet_init(uint8* resVram)
{
	sheetCtl.vram = ((BOOTINFO*)ADDR_BOOTINFO)->vram;
	sheetCtl.resVram = resVram;
	sheetCtl.width = ((BOOTINFO*)ADDR_BOOTINFO)->screenWidth;
	sheetCtl.height = ((BOOTINFO*)ADDR_BOOTINFO)->screenHeight;
	sheetCtl.topZ = -1;
	
	sheetCtl.mouseSheet.no = 0;
	
	sheetCtl.redraw_rect.topLeftX = sheetCtl.width;
	sheetCtl.redraw_rect.topLeftY = sheetCtl.height;
	sheetCtl.redraw_rect.bottomRightX = 0;
	sheetCtl.redraw_rect.bottomRightY = 0;
}

/** @brief refresh the whole screen, redraw all the sheet existed.
 *  The function is inefficiency. So, It only be called when necessary.
 *  @param NULL
 *  @return NULL
 */
void sheet_draw_all()
{
	int32 i = 0, j, k;
	int32 x, y, w, h;
	uint8* buf, tmp;
	for(; i <= sheetCtl.topZ; i++) {		// 从 z = 0 到 z = topZ
		if(sheetCtl.sortedSheet[i]->z == i) {
			w = sheetCtl.sortedSheet[i]->width;
			h = sheetCtl.sortedSheet[i]->height;
			x = sheetCtl.sortedSheet[i]->topLeftX;
			y = sheetCtl.sortedSheet[i]->topLeftY;
			buf = sheetCtl.sortedSheet[i]->buf;
			for(k = (y > 0) ? y : 0; (k < h + y) && (k <= sheetCtl.height); k++) {
				for(j = (x > 0) ? x : 0; (j < w + x) && (j <= sheetCtl.width); j++) {
					tmp = buf[(k - y) * w + (j - x)];
					if(tmp != NONE_COL)	//透明则不画
						sheetCtl.resVram[k * sheetCtl.width + j] = tmp;
				}
			}
		}
	}
	w = sheetCtl.mouseSheet.width;
	h = sheetCtl.mouseSheet.height;
	x = sheetCtl.mouseSheet.topLeftX;
	y = sheetCtl.mouseSheet.topLeftY;
	buf = sheetCtl.mouseSheet.buf;
	for(k = (y > 0) ? y : 0; (k < h + y) && (k <= sheetCtl.height); k++) {
		for(j = (x > 0) ? x : 0; (j < w + x) && (j <= sheetCtl.width); j++) {
			tmp = buf[(k - y) * w + (j - x)];
			if(tmp != NONE_COL)	//透明则不画
				sheetCtl.resVram[k * sheetCtl.width + j] = tmp;
		}
	}
	for(i = 0; i < sheetCtl.width * sheetCtl.height; i++) {
		sheetCtl.vram[i] = sheetCtl.resVram[i];
	}
}

/** @brief refresh the given rectangle area, redraw all the sheet if in this area.
 *  The function is usually called when do refresh. The sheet including sheet which hold mouse.
 *  @param topLeftX x value of top left pos of the area.
 *  @param topLeftY y value of top left pos of the area.
 *  @param bottomRightX x value of bottom right pos of the area.
 *  @param bottomRightY y value of bottom right pos of the area.
 *  @return NULL
 */
void sheet_draw_sub(IN int32 topLeftX, IN int32 topLeftY, IN int32 bottomRightX, IN int32 bottomRightY)
{
	int32 l, vx, vy, bx0, bx1, by0, by1, sx, sy;
	SHEET* sheet;
	uint8 tmp;
	
	topLeftX = (topLeftX <= 0) ? 0 : topLeftX;
	topLeftY = (topLeftY <= 0) ? 0 : topLeftY;
	bottomRightX = (bottomRightX >= sheetCtl.width) ? sheetCtl.width : bottomRightX;
	bottomRightY = (bottomRightY >= sheetCtl.height) ? sheetCtl.height : bottomRightY;
	
	for(l = 0; l <= sheetCtl.topZ; l++) {
		sheet = sheetCtl.sortedSheet[l];
		bx0 = topLeftX - sheet->topLeftX;
		by0 = topLeftY - sheet->topLeftY;
		bx1 = bottomRightX - sheet->topLeftX;
		by1 = bottomRightY - sheet->topLeftY;
		if(bx0 < 0) bx0 = 0;
		if(by0 < 0) by0 = 0;
		if(bx1 > sheet->width) bx1 = sheet->width;
		if(by1 > sheet->height) by1 = sheet->height;

		for(sx = bx0; sx < bx1; sx++) {
			vx = sx + sheet->topLeftX;
			for(sy = by0; sy < by1; sy++) {
				vy = sy + sheet->topLeftY;
				tmp = sheet->buf[sy * sheet->width + sx];
				if(tmp != NONE_COL)
					sheetCtl.resVram[vy * sheetCtl.width + vx] = tmp;
			}
		}
	}
	
	// 重绘 mouse 部分
	sheet = &(sheetCtl.mouseSheet);
	bx0 = topLeftX - sheet->topLeftX;
	by0 = topLeftY - sheet->topLeftY;
	bx1 = bottomRightX - sheet->topLeftX;
	by1 = bottomRightY - sheet->topLeftY;
	if(bx0 < 0) bx0 = 0;
	if(by0 < 0) by0 = 0;
	if(bx1 > sheet->width) bx1 = sheet->width;
	if(by1 > sheet->height) by1 = sheet->height;
	for(sx = bx0; sx < bx1; sx++) {
		vx = sx + sheet->topLeftX;
		for(sy = by0; sy < by1; sy++) {
			vy = sy + sheet->topLeftY;
			tmp = sheet->buf[sy * sheet->width + sx];
			if(tmp != NONE_COL)
				sheetCtl.resVram[vy * sheetCtl.width + vx] = tmp;
		}
	}
	
	// 将双缓冲区的图像刷新至显存
	for(vy = topLeftY; vy < bottomRightY; vy++) {
		for(vx = topLeftX; vx < bottomRightX; vx++) {
			sheetCtl.vram[vy * sheetCtl.width + vx] = sheetCtl.resVram[vy * sheetCtl.width + vx];
		}
	}
}

/** @brief refresh the given rectangle area, redraw all the sheet if in this area without sheet which hold mouse.
 *  The function is usually called when do refresh.
 *  @param topLeftX x value of top left pos of the area.
 *  @param topLeftY y value of top left pos of the area.
 *  @param bottomRightX x value of bottom right pos of the area.
 *  @param bottomRightY y value of bottom right pos of the area.
 *  @return NULL
 */
void sheet_draw_sub_withoutMouse(IN int32 topLeftX, IN int32 topLeftY, IN int32 bottomRightX, IN int32 bottomRightY)
{
	int32 l, vx, vy, bx0, bx1, by0, by1, sx, sy;
	SHEET* sheet;
	uint8 tmp;
	
	topLeftX = (topLeftX <= 0) ? 0 : topLeftX;
	topLeftY = (topLeftY <= 0) ? 0 : topLeftY;
	bottomRightX = (bottomRightX >= sheetCtl.width) ? sheetCtl.width : bottomRightX;
	bottomRightY = (bottomRightY >= sheetCtl.height) ? sheetCtl.height : bottomRightY;
	
	for(l = 0; l <= sheetCtl.topZ; l++) {
		sheet = sheetCtl.sortedSheet[l];
		bx0 = topLeftX - sheet->topLeftX;
		by0 = topLeftY - sheet->topLeftY;
		bx1 = bottomRightX - sheet->topLeftX;
		by1 = bottomRightY - sheet->topLeftY;
		if(bx0 < 0) bx0 = 0;
		if(by0 < 0) by0 = 0;
		if(bx1 > sheet->width) bx1 = sheet->width;
		if(by1 > sheet->height) by1 = sheet->height;

		for(sx = bx0; sx < bx1; sx++) {
			vx = sx + sheet->topLeftX;
			for(sy = by0; sy < by1; sy++) {
				vy = sy + sheet->topLeftY;
				tmp = sheet->buf[sy * sheet->width + sx];
				if(tmp != NONE_COL)
					sheetCtl.resVram[vy * sheetCtl.width + vx] = tmp;
			}
		}
	}
	// 将双缓冲区的图像刷新至显存
	for(vy = topLeftY; vy < bottomRightY; vy++) {
		for(vx = topLeftX; vx < bottomRightX; vx++) {
			sheetCtl.vram[vy * sheetCtl.width + vx] = sheetCtl.resVram[vy * sheetCtl.width + vx];
		}
	}
}

/** @brief create one sheet, add the related value into sheetCtl, You can get the sheet handle from param sheet.
 *  Create one sheet with given buffer, area, flag and get the sheet handle with sheet. The sheet would be placed on the top layer.
 *  @param buf the buffer to copy into the sheet.
 *  @param width width of the area.
 *  @param height height of the area.
 *  @param topLeftX x value of top left pos of the area.
 *  @param topLeftY y value of top left pos of the area.
 *  @param flags currently, it specify the transparency color number of this sheet.
 *  @param sheet the sheet handle returned. Save it for future use.
 *  @return whether add operation is successful, if yes return TRUE else FALSE.
 */
boolean sheet_add(IN uint8* buf, IN int32 width, IN int32 height, IN int32 topLeftX, IN int32 topLeftY, IN int8 flags, OUT SHEET** sheet)
{
	if(sheetCtl.topZ >= (MAX_SHEET_NUM - 1) || (sheet == NULL)) {
		return FALSE;
	}
	sheetCtl.topZ++;
	*sheet = &(sheetCtl.sheets[sheetCtl.topZ]);
	(*sheet)->buf = buf;
	(*sheet)->width = width;
	(*sheet)->height = height;
	(*sheet)->topLeftX = topLeftX;
	(*sheet)->topLeftY = topLeftY;
	(*sheet)->flags = flags;
	(*sheet)->no = sheetCtl.topZ;
	(*sheet)->z = sheetCtl.topZ;
	sheetCtl.sortedSheet[sheetCtl.topZ] = (*sheet);
	sheet_sort(sheetCtl);
	return TRUE;
}

/** @brief add the mouse into sheetCtl, You can get the sheet handle from param sheet.
 *  Create one sheet with given mouse buffer, area, flag and get the sheet handle with sheet.
 *  @param buf the buffer to copy into the sheet, the buffer have to be pixels of mouse.
 *  @param width width of the area.
 *  @param height height of the area.
 *  @param topLeftX x value of top left pos of the area.
 *  @param topLeftY y value of top left pos of the area.
 *  @param flags currently, it specify the transparency color number of this sheet.
 *  @param sheet the sheet handle returned. Save it for future use.
 *  @return whether add operation is successful, if yes return TRUE else FALSE.
 */
boolean sheet_add_mouse(IN uint8* buf, IN int32 width, IN int32 height, IN int32 topLeftX, IN int32 topLeftY, IN int8 flags, OUT SHEET** sheet)
{
	if(sheetCtl.mouseSheet.no == 0) {
		*sheet = &(sheetCtl.mouseSheet);
		
		(*sheet)->no = MAX_SHEET_NUM - 1;
		(*sheet)->buf = buf;
		(*sheet)->width = width;
		(*sheet)->height = height;
		(*sheet)->topLeftX = topLeftX;
		(*sheet)->topLeftY = topLeftY;
		(*sheet)->flags = flags;
		(*sheet)->z = MAX_SHEET_NUM - 1;
		
		return TRUE;
	}
	return FALSE;
}

/** @brief delete on sheet from sheetCtl.
 *  @param sheet the sheet to be removed.
 *  @return whether add operation is successful, if yes return TRUE else FALSE.
 */
boolean sheet_del(IN SHEET* sheet)
{
	int32 i;
	for(i = 0; i <= sheetCtl.topZ; i++) {
		if(sheet->no == sheetCtl.sheets[i].no && sheet->z == sheetCtl.sheets[i].z) {	// 已经找到应该删除的层
			mem_free((uint32)(sheet->buf), sheet->width * sheet->height);
			sheet_add_redraw_region(sheet->topLeftX, sheet->topLeftY, sheet->topLeftX + sheet->width, sheet->topLeftY + sheet->height);
			for(; i < sheetCtl.topZ; i++) {
				sheetCtl.sheets[i] = sheetCtl.sheets[i + 1];
			}
			sheetCtl.topZ--;
			return TRUE;
		}
	}
	return FALSE;
}

/** @brief sort the sheet acccording to their Zvalue, z describe the layer of the sheet.
 *  @param NULL
 *  @return NULL
 */
void sheet_sort()
{
	SHEET* pTmpSheet;
	int32 i, j;
	for(i = 0; i < sheetCtl.topZ; i++) {
		for(j = 0; j < sheetCtl.topZ - i; j++) {
			if(sheetCtl.sheets[j].z > sheetCtl.sheets[j + 1].z) {
				pTmpSheet = sheetCtl.sortedSheet[j];
				sheetCtl.sortedSheet[j] = sheetCtl.sortedSheet[j + 1];
				sheetCtl.sortedSheet[j + 1] = pTmpSheet;
			}
		}
	}
}

/** @brief put the given sheet on the top layer.
 *  @param sheet the sheet to be operated.
 *  @return whether add operation is successful, if yes return TRUE else FALSE.
 */
boolean sheet_pin_top(IN SHEET* sheet)
{
	int32 i, j;
	if(sheet == NULL)
		return FALSE;
	
	for(i = 0; i <= sheetCtl.topZ && sheet != sheetCtl.sortedSheet[i]; i++);
	if(sheet == sheetCtl.sortedSheet[i]) {		//找到了该Sheet
		sheet->z = sheetCtl.topZ;
		for(j = i + 1; j <= sheetCtl.topZ; j++) {
			sheetCtl.sortedSheet[j]->z--;
			sheetCtl.sortedSheet[j - 1] = sheetCtl.sortedSheet[j];
		}
		sheetCtl.sortedSheet[sheetCtl.topZ] = sheet;

		return TRUE;
	}
	else {
		return FALSE;
	}
}

/** @brief add the redraw request.
 *  @param topLeftX x value of top left pos of the area.
 *  @param topLeftY y value of top left pos of the area.
 *  @param bottomRightX x value of bottom right pos of the area.
 *  @param bottomRightY y value of bottom right pos of the area.
 *  @return whether add operation is successful, always return TRUE.
 */
boolean sheet_add_redraw_region(IN int32 topLeftX, IN int32 topLeftY, IN int32 bottomRightX, IN int32 bottomRightY)
{
	sheetCtl.redraw_rect.topLeftX = (topLeftX < sheetCtl.redraw_rect.topLeftX) ? topLeftX : sheetCtl.redraw_rect.topLeftX;
	sheetCtl.redraw_rect.topLeftY = (topLeftY < sheetCtl.redraw_rect.topLeftY) ? topLeftY : sheetCtl.redraw_rect.topLeftY;
	sheetCtl.redraw_rect.bottomRightX = (bottomRightX > sheetCtl.redraw_rect.bottomRightX) ? bottomRightX : sheetCtl.redraw_rect.bottomRightX;
	sheetCtl.redraw_rect.bottomRightY = (bottomRightY > sheetCtl.redraw_rect.bottomRightY) ? bottomRightY : sheetCtl.redraw_rect.bottomRightY;
	return TRUE;
}

/** @brief add the redraw request.
 *  This function is a wraper of sheet_add_redraw_region().
 *  @param rect the area need redrawn.
 *  @return whether add operation is successful, always return TRUE.
 */
boolean sheet_add_redraw_region_rect(IN RECT rect)
{
	sheet_add_redraw_region(rect.topLeftX, rect.topLeftY, rect.bottomRightX, rect.bottomRightY);
	return TRUE;
}

/** @brief the redraw daemon.
 *  It will test if the area is not empty. If the area existed. It will redraw the area.
 *  @param NULL.
 *  @return NULL.
 */
void sheet_draw_deamon()
{
	if(sheetCtl.redraw_rect.topLeftX != sheetCtl.width) {
		sheet_draw_sub(sheetCtl.redraw_rect.topLeftX, sheetCtl.redraw_rect.topLeftY, sheetCtl.redraw_rect.bottomRightX, sheetCtl.redraw_rect.bottomRightY);
		sheetCtl.redraw_rect.topLeftX = sheetCtl.width;
		sheetCtl.redraw_rect.topLeftY = sheetCtl.height;
		sheetCtl.redraw_rect.bottomRightX = 0;
		sheetCtl.redraw_rect.bottomRightY = 0;
	}
}
