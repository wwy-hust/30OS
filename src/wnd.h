/** @file wnd.h
 *  @brief basic functions related to windows management.
 *
 *  The wnd management including create wnd, move wnd, put wnd on top, etc.
 */
 
#ifndef _WND_H_
#define _WND_H_

#include "typedef.h"
#include "graphic.h"
#include "mem.h"
#include "sheet.h"
#include "mouse_keyboard.h"
#include "dataStruct.h"

/** @addtogroup windows
 *  @brief this module manage the windows. It depends on many modules, graphic, mem, sheet, etc.
 *  @{
 */
#define WND_ARRAY_SIZE	64

/** @brief struct of one wnd. */
typedef struct WND{
	boolean	isUsing;			///< is the WND used
	int32		z;				///< z layer value, top or between wnds
	int8*		title;			///< windows title
	int8* 	icon;				///< windows icon
	RECT		wndRect;		///< windows rectangle, can not be changed currently
	RECT		titleRect;		///< relative rect with wndRect
	RECT		closeRect;		///< relative rect with wndRect
	RECT		contentRect;	///< relative rect with wndRect
	SHEET*	wndSheet;			///< sheet handle which hold the pixel of the window
	LoopArray*	keyboardBuffer;	///< keyboard buffer to receive input
}WND;

/** @brief struct of window management system. */
typedef struct WND_CTL {
	int32 wndCnt;						///< num of window
	WND*	sortedWnd[WND_ARRAY_SIZE];	///< windows sorted by z
	WND		wnd[WND_ARRAY_SIZE];		///< store all WND structure.
}WND_CTL;

/** @brief this typedef simplify the draw function. 
 *  So, in order to use this simplicity, you need to write a wraper. 
 *  examples are in wnd.c draw_wnd1(), draw_wnd2().
 */
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

/// @}

#endif
