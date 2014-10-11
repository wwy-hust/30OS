
#include "sheet.h"

SHEET_CTL sheetCtl;

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

void sheet_draw_all()
{
	int32 i = 0, j, k;
	int32 x, y, w, h;
	uint8* buf, tmp;
	for(; i <= sheetCtl.topZ; i++) {		// �� z = 0 �� z = topZ
		if(sheetCtl.sortedSheet[i]->z == i) {
			w = sheetCtl.sortedSheet[i]->width;
			h = sheetCtl.sortedSheet[i]->height;
			x = sheetCtl.sortedSheet[i]->topLeftX;
			y = sheetCtl.sortedSheet[i]->topLeftY;
			buf = sheetCtl.sortedSheet[i]->buf;
			for(k = (y > 0) ? y : 0; (k < h + y) && (k <= sheetCtl.height); k++) {
				for(j = (x > 0) ? x : 0; (j < w + x) && (j <= sheetCtl.width); j++) {
					tmp = buf[(k - y) * w + (j - x)];
					if(tmp != NONE_COL)	//͸���򲻻�
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
			if(tmp != NONE_COL)	//͸���򲻻�
				sheetCtl.resVram[k * sheetCtl.width + j] = tmp;
		}
	}
	for(i = 0; i < sheetCtl.width * sheetCtl.height; i++) {
		sheetCtl.vram[i] = sheetCtl.resVram[i];
	}
}

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
	
	// �ػ� mouse ����
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
	
	// ��˫��������ͼ��ˢ�����Դ�
	for(vy = topLeftY; vy < bottomRightY; vy++) {
		for(vx = topLeftX; vx < bottomRightX; vx++) {
			sheetCtl.vram[vy * sheetCtl.width + vx] = sheetCtl.resVram[vy * sheetCtl.width + vx];
		}
	}
}

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
	// ��˫��������ͼ��ˢ�����Դ�
	for(vy = topLeftY; vy < bottomRightY; vy++) {
		for(vx = topLeftX; vx < bottomRightX; vx++) {
			sheetCtl.vram[vy * sheetCtl.width + vx] = sheetCtl.resVram[vy * sheetCtl.width + vx];
		}
	}
}

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

boolean sheet_del(IN SHEET* sheet)
{
	int32 i;
	for(i = 0; i <= sheetCtl.topZ; i++) {
		if(sheet->no == sheetCtl.sheets[i].no && sheet->z == sheetCtl.sheets[i].z) {	// �Ѿ��ҵ�Ӧ��ɾ���Ĳ�
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

boolean sheet_pin_top(IN SHEET* sheet)
{
	int32 i, j;
	if(sheet == NULL)
		return FALSE;
	
	for(i = 0; i <= sheetCtl.topZ && sheet != sheetCtl.sortedSheet[i]; i++);
	if(sheet == sheetCtl.sortedSheet[i]) {		//�ҵ��˸�Sheet
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

boolean sheet_add_redraw_region(IN int32 topLeftX, IN int32 topLeftY, IN int32 bottomRightX, IN int32 bottomRightY)
{
	sheetCtl.redraw_rect.topLeftX = (topLeftX < sheetCtl.redraw_rect.topLeftX) ? topLeftX : sheetCtl.redraw_rect.topLeftX;
	sheetCtl.redraw_rect.topLeftY = (topLeftY < sheetCtl.redraw_rect.topLeftY) ? topLeftY : sheetCtl.redraw_rect.topLeftY;
	sheetCtl.redraw_rect.bottomRightX = (bottomRightX > sheetCtl.redraw_rect.bottomRightX) ? bottomRightX : sheetCtl.redraw_rect.bottomRightX;
	sheetCtl.redraw_rect.bottomRightY = (bottomRightY > sheetCtl.redraw_rect.bottomRightY) ? bottomRightY : sheetCtl.redraw_rect.bottomRightY;
	return TRUE;
}

boolean sheet_add_redraw_region_rect(IN RECT rect)
{
	sheet_add_redraw_region(rect.topLeftX, rect.topLeftY, rect.bottomRightX, rect.bottomRightY);
}

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