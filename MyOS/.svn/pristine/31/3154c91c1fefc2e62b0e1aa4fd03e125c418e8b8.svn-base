
#include "wnd.h"
#include "wnd_console.h"

WND_CTL wndCtl;

void wnd_init()
{
	int8 i;
	wndCtl.wndCnt = 0;
	for(i = 0; i < WND_ARRAY_SIZE; i++) {
		wndCtl.sortedWnd[i] = NULL;
		wndCtl.wnd[i].isUsing = FALSE;
	}
}

boolean wnd_create(IN RECT wndSize, IN int8* title, IN int8* icon, OUT WND** wnd, draw_wnd func)
{
	int32 i, width, height;
	uint8* buf;
	RECT tmpRect;
	if(title == NULL || icon == NULL || wndCtl.wndCnt == WND_ARRAY_SIZE)
		return FALSE;

	width = wndSize.bottomRightX - wndSize.topLeftX;
	height = wndSize.bottomRightY - wndSize.topLeftY;
	if(mem_alloc(width * height, &i) == FALSE)
		return FALSE;
	else 
		buf = (uint8*)i;
	
	for(i = 0; wndCtl.wnd[i].isUsing == TRUE; i++);
	*wnd = &wndCtl.wnd[i];
	
	if(mem_alloc(sizeof(LoopArray), &i) == FALSE) {
		mem_free((uint32)buf, width * height);
		return FALSE;
	}
	
	if(sheet_add(buf, width, height, wndSize.topLeftX, wndSize.topLeftY, NONE_COL, &((*wnd)->wndSheet)) == FALSE)
		return FALSE;
	
	wndCtl.sortedWnd[wndCtl.wndCnt++] = *wnd;
	(*wnd)->isUsing = TRUE;
	(*wnd)->z = wndCtl.wndCnt - 1;
	(*wnd)->title = title;
	(*wnd)->icon = icon;
	(*wnd)->wndRect = wndSize;
	(*wnd)->keyboardBuffer = (LoopArray*)i;
	
	tmpRect.topLeftX = 3;
	tmpRect.topLeftY = 3;
	tmpRect.bottomRightX = wndSize.bottomRightX - 4;
	tmpRect.bottomRightY = 21;
	(*wnd)->titleRect = tmpRect;
	
	tmpRect.topLeftX = 4;
	tmpRect.topLeftY = 24;
	tmpRect.bottomRightX = wndSize.bottomRightX - wndSize.topLeftX - 5;
	tmpRect.bottomRightY = wndSize.bottomRightY - wndSize.topLeftY - 5;
	(*wnd)->contentRect = tmpRect;
	
	tmpRect.topLeftX = (*wnd)->contentRect.bottomRightX - 20;
	tmpRect.topLeftY = 4;
	tmpRect.bottomRightX = (*wnd)->contentRect.bottomRightX - 4;
	tmpRect.bottomRightY = 20;
	(*wnd)->closeRect = tmpRect;
	func(*wnd);
	
	return TRUE;
}

boolean wnd_del(IN WND* wnd)
{
	int32 i;
	if(wnd == NULL)
		return FALSE;
	for(i = 0; i < WND_ARRAY_SIZE && wnd != &(wndCtl.wnd[i]); i++);		//找到该wnd在 wnd[] 中的位置
	if(wnd == &(wndCtl.wnd[i])) {		//确定已找到
		wnd->isUsing = FALSE;
		mem_free((uint32)wnd->keyboardBuffer, sizeof(LoopArray));
		sheet_del(wnd->wndSheet);
		for(i = 0; i < wndCtl.wndCnt && wnd != wndCtl.sortedWnd[i]; i++);		//找该wnd在 sortedWnd[] 中的位置
		if(wnd == wndCtl.sortedWnd[i]) {		//确定已找到
			for(; i < wndCtl.wndCnt; i++) {
				wndCtl.sortedWnd[i] = wndCtl.sortedWnd[i + 1];
			}
			wndCtl.wndCnt--;
			return TRUE;
		}
		else {		//未在 sortedWnd[] 中找到
			return FALSE;
		}
	}
	else {		//未在 wnd[] 中找到
		return FALSE;
	}
}

boolean wnd_is_click_on_wnd(OUT WND** wnd)
{
	RECT tmpRect;
	POS pos;
	int32 i;
	
	MouseGetPos(&pos);
	for(i = wndCtl.wndCnt - 1; i >= 0; i--) {
		tmpRect = wndCtl.sortedWnd[i]->wndRect;
		if(pos_inside_rect(&tmpRect, &pos) == TRUE) {
			*wnd = wndCtl.sortedWnd[i];
			return TRUE;
		}
	}
	return FALSE;
}

boolean wnd_is_click_on_title(OUT WND** wnd)
{
	RECT tmpRect;
	POS pos;
	WND* wnd_top;
	
	MouseGetPos(&pos);
	wnd_top = wnd_get_top();

	tmpRect = wnd_top->titleRect;
	tmpRect.topLeftX += wnd_top->wndRect.topLeftX;
	tmpRect.topLeftY += wnd_top->wndRect.topLeftY;
	tmpRect.bottomRightX += wnd_top->wndRect.topLeftX;
	tmpRect.bottomRightY += wnd_top->wndRect.topLeftY;
	if(pos_inside_rect(&tmpRect, &pos) == TRUE) {
		*wnd = wnd_top;
		return TRUE;
	}
	else {
		return FALSE;
	}
}

boolean wnd_is_click_on_close(OUT WND** wnd)
{
	RECT tmpRect;
	POS pos;
	WND* wnd_top;
//	WND_CONSOLE* console = (WND_CONSOLE*)*((int32*)0x0FEC);
	
	MouseGetPos(&pos);
	wnd_top = wnd_get_top();
	tmpRect = wnd_top->closeRect;
	tmpRect.topLeftX += wnd_top->wndRect.topLeftX;
	tmpRect.topLeftY += wnd_top->wndRect.topLeftY;
	tmpRect.bottomRightX += wnd_top->wndRect.topLeftX;
	tmpRect.bottomRightY += wnd_top->wndRect.topLeftY;
	
//	console_puts(console, "((%d,%d),(%d,%d)) | \n", wnd_top->closeRect.topLeftX, wnd_top->closeRect.topLeftY, wnd_top->closeRect.bottomRightX, wnd_top->closeRect.bottomRightY);
//	console_puts(console, "((%d,%d),(%d,%d)) | \n", wnd_top->wndRect.topLeftX, wnd_top->wndRect.topLeftY, wnd_top->wndRect.bottomRightX, wnd_top->wndRect.bottomRightY);
//	console_puts(console, "[%d,%d]~((%d,%d),(%d,%d))\n", pos.x, pos.y, tmpRect.topLeftX, tmpRect.topLeftY, tmpRect.bottomRightX, tmpRect.bottomRightY);
	
	if(pos_inside_rect(&tmpRect, &pos) == TRUE) {
		*wnd = wnd_top;
		return TRUE;
	}
	else {
		return FALSE;
	}
}

boolean wnd_move(IN WND* wnd, int32 x, int32 y)
{
	if(wnd == NULL)
		return FALSE;
		
	wnd->wndRect.topLeftX += x;
	wnd->wndRect.topLeftY += y;
	wnd->wndRect.bottomRightX += x;
	wnd->wndRect.bottomRightY += y;
	
	wnd->wndSheet->topLeftX += x;
	wnd->wndSheet->topLeftY += y;
	return TRUE;
}

boolean wnd_pin_top(IN WND* wnd)
{
	int32 i, j;
	if(wnd == NULL)
		return FALSE;
	
	for(i = 0; i < wndCtl.wndCnt && wnd != wndCtl.sortedWnd[i]; i++);
	if(wnd == wndCtl.sortedWnd[i]) {
		wnd->z = wndCtl.wndCnt - 1;
		for(j = i + 1; j < wndCtl.wndCnt; j++) {
			wndCtl.sortedWnd[j]->z--;
			wndCtl.sortedWnd[j - 1] = wndCtl.sortedWnd[j];
		}
		wndCtl.sortedWnd[wndCtl.wndCnt - 1] = wnd;
		return sheet_pin_top(wnd->wndSheet);
	}
	return FALSE;
}

boolean wnd_is_valid(IN WND* wnd)
{
	int32 i;
	for(i = 0; i < wndCtl.wndCnt && wnd != wndCtl.sortedWnd[i]; i++);
	if(i < wndCtl.wndCnt)
		return TRUE;
	else
		return FALSE;
}

WND* wnd_get_top()
{
	if(wndCtl.wndCnt == 0) {
		return NULL;
	}
	return wndCtl.sortedWnd[wndCtl.wndCnt - 1];
}

void draw_wnd1(WND* wnd)
{
	draw_default_wnd(wnd->wndSheet, wnd->title, wnd->icon, 0, 0, wnd->wndRect.bottomRightX - wnd->wndRect.topLeftX, wnd->wndRect.bottomRightY - wnd->wndRect.topLeftY);
}

void draw_wnd2(WND *wnd)
{
	draw_console_wnd(wnd->wndSheet, wnd->title, wnd->icon, 0, 0, wnd->wndRect.bottomRightX - wnd->wndRect.topLeftX, wnd->wndRect.bottomRightY - wnd->wndRect.topLeftY);
}

