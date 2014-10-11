
#ifndef _WND_H_
#define _WND_H_

#include "typedef.h"
#include "graphic.h"
#include "mem.h"
#include "sheet.h"
#include "mouse_keyboard.h"
#include "dataStruct.h"

#define WND_ARRAY_SIZE	64

typedef struct WND{
	boolean	isUsing;
	int32		z;
	int8*		title;
	int8* 	icon;
	RECT		wndRect;
	RECT		titleRect;		//relative
	RECT		closeRect;		//relative
	RECT		contentRect;	//relative
	SHEET*	wndSheet;
	LoopArray*	keyboardBuffer;
}WND;

typedef struct WND_CTL {
	int32 wndCnt;
	WND*	sortedWnd[WND_ARRAY_SIZE];
	WND		wnd[WND_ARRAY_SIZE];
}WND_CTL;

typedef void (draw_wnd)(WND* wnd);

void wnd_init();
boolean wnd_create(IN RECT wndSize, IN int8* title, IN int8* icon, OUT WND** wnd, draw_wnd func);
boolean wnd_del(IN WND* wnd);
boolean wnd_is_click_on_wnd(OUT WND** wnd);
boolean wnd_is_click_on_title(OUT WND** wnd);
boolean wnd_is_click_on_close(OUT WND** wnd);
boolean wnd_move(IN WND* wnd, int32 x, int32 y);
boolean wnd_pin_top(IN WND* wnd);
boolean wnd_is_valid(IN WND* wnd);
WND* wnd_get_top();

// draw Wnd method
void draw_wnd1(WND* wnd);		// draw a default window
void draw_wnd2(WND* wnd);		// draw a console window

#endif
