/** @file wnd.c
 *  @brief basic functions related to windows management.
 *
 *  The wnd management including create wnd, move wnd, put wnd on top, etc.
 */

#include "wnd.h"
#include "wnd_console.h"


/// @addtogroup windows
/// @{
WND_CTL wndCtl;		///< global variable used to store information about windows.
/// @}

/** @brief init wndCtl.
 *  Init wndCtl structure.
 *  @param NULL
 *  @return NULL
 */
void wnd_init()
{
	int8 i;
	wndCtl.wndCnt = 0;
	for(i = 0; i < WND_ARRAY_SIZE; i++) {
		wndCtl.sortedWnd[i] = NULL;
		wndCtl.wnd[i].isUsing = FALSE;
	}
}

/** @brief create a window, with specified size, title, icon, draw function.
 *  If create successful, the window would be put into sortedWnd[] of wndCtl.
 *  @param wndSize RECT value of window size.
 *  @param title the title of window, would display on title bar.
 *  @param icon the icon of window, would display on top left of window's title bar. 
 *  @param wnd the window handle, could be usd in future.
 *  @param func the draw function used to draw this window, this could be drawed to a default window or console. 
 *         you could add your own draw function to draw your own window.
 *  @return whether the operation is successful. if yes, return TRUE els FALSE.
 */
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

/** @brief delete window.
 *  delete a window from sortedWindow[].
 *  @param wnd the window handle you want to use.
 *  @return whether the operation is successful. if yes, return TRUE els FALSE.
 */
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

/** @brief test if user click in specified window.
 *  @param wnd return the window handle you that user clicked on.
 *  @return TRUE if user clicked on a wnd ,FALSE if user clicked on none wnd.
 */
boolean wnd_is_click_on_wnd(OUT WND** wnd)
{
	RECT tmpRect;
	POS pos;
	int32 i;
	@{
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

/** @brief test if user click in title bar of specified window.
 *  @param wnd return the window handle you that user clicked on.
 *  @return TRUE if user clicked on a title bar ,FALSE if user clicked on none title bar.
 */
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

/** @brief test if user click in close btn of specified window.
 *  @param wnd return the window handle you that user clicked on.
 *  @return TRUE if user clicked on a close btn ,FALSE if user clicked on none close btn.
 */
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

/** @brief move window with (x, y).
 *  @param wnd the window you want move.
 *  @param x x offset to move. negative number mean move left, positive means move right.
 *  @param y y offset to move. negative number mean move up, positive means move down.
 *  @return TRUE if move successful, FALSE if failed.
 */
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

/** @brief put wnd on top.
 *  @param wnd the window you want put in front of.
 *  @return TRUE if successful, FALSE if failed.
 */
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

/** @brief test if wnd is valid.
 *  @param wnd the window you want use.
 *  @return TRUE if successful, FALSE if failed.
 */
boolean wnd_is_valid(IN WND* wnd)
{
	int32 i;
	for(i = 0; i < wndCtl.wndCnt && wnd != wndCtl.sortedWnd[i]; i++);
	if(i < wndCtl.wndCnt)
		return TRUE;
	else
		return FALSE;
}

/** @brief get the wnd on top.
 *  @param NULL.
 *  @return the window which on top of screen.
 */
WND* wnd_get_top()
{
	if(wndCtl.wndCnt == 0) {
		return NULL;
	}
	return wndCtl.sortedWnd[wndCtl.wndCnt - 1];
}

/** @brief draw window with default style.
 *  @param wnd the window handle you want to use.
 *  @return NULL
 */
void draw_wnd1(WND* wnd)
{
	draw_default_wnd(wnd->wndSheet, wnd->title, wnd->icon, 0, 0, wnd->wndRect.bottomRightX - wnd->wndRect.topLeftX, wnd->wndRect.bottomRightY - wnd->wndRect.topLeftY);
}

/** @brief draw window with console style.
 *  @param wnd the window handle you want to use.
 *  @return NULL
 */
void draw_wnd2(WND *wnd)
{
	draw_console_wnd(wnd->wndSheet, wnd->title, wnd->icon, 0, 0, wnd->wndRect.bottomRightX - wnd->wndRect.topLeftX, wnd->wndRect.bottomRightY - wnd->wndRect.topLeftY);
}

