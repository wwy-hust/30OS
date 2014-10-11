
#include "mouse_keyboard.h"


// ** keyboard & mouse control block **
MOUSE_CTL mouseCtl;
KEYBOARD_CTL keyboardCtl;

LoopArray keyboardLoopArray;
LoopArray mouseLoopArray;

extern SHEET* bgSheet;

#define KEY_TBL_SIZE		0x59
static char keytable0[KEY_TBL_SIZE] = {
	0,   ASC_ESC,  '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', ASC_BS, ASC_HT,
	'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', ASC_CR, 0,  'a', 's',
	'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'','`', 0, '\\', 'z', 'x', 'c', 'v',
	'b', 'n', 'm', ',', '.', '/', 0, '*', 0, ' ', 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, '7', '8', '9', '-', '4', '5', '6', '+', '1',
	'2', '3', '0', '.', 0, 0, 0, 0, 0
};
static char keytable1[KEY_TBL_SIZE] = {
	0,   ASC_ESC,  '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', ASC_BS, ASC_HT,
	'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', ASC_CR, 0,  'A', 'S',
	'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '\"','~', 0, '|', 'Z', 'X', 'C', 'V',
	'B', 'N', 'M', '<', '>', '?', 0, '*', 0, ' ', 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, '7', '8', '9', '-', '4', '5', '6', '+', '1',
	'2', '3', '0', '.', 0, 0, 0, 0, 0
};

void interrupt_handler_0x21(int32* esp)
{
	uint8 data = 0;
	
	io_out8(PIC0_OCW2, 0x61);		//通知 PIC 可编程中断控制器IRQ-01已经完成
	data = io_in8(PORT_KEYDAT);
	PutIntoLoopArray(&keyboardLoopArray, data);
}

void interrupt_handler_0x2c(int32 *esp)
{
	uint8 data = 0;
	io_out8(PIC1_OCW2, 0x64);
	io_out8(PIC0_OCW2, 0x62);
	data = io_in8(PORT_KEYDAT);
	PutIntoLoopArray(&mouseLoopArray, data);
}

void interrupt_handler_0x27(int32 *esp)
{
	io_out8(PIC0_OCW2, 0x67); 
	return;
}

void waitForKBCSendReady()
{
	while(1) {
		if((io_in8(PORT_KEYSTA) & KEYSTA_SEND_NOTREADY) == 0) {
			break;
		}
	}
}

void enableMouse()
{
	uint8 data;
	waitForKBCSendReady();
	io_out8(PORT_KEYCMD, KEYCMD_SENDTO_MOUSE);
	waitForKBCSendReady();
	io_out8(PORT_KEYDAT, MOUSECMD_ENABLE);
	data = io_in8(PORT_KEYDAT);
}

// *********
void InitPeripheralBuffer()
{
	InitLoopArray(&keyboardLoopArray);
	InitLoopArray(&mouseLoopArray);
}

boolean InitMouse()
{
	boolean re;
	uint32 tmpAddr;

	enableMouse();
	mouseCtl.x = 100;
	mouseCtl.y = 100;
	mouseCtl.key = 0x00;
	mouseCtl.isMouseOff = TRUE;
	re = mem_alloc(MOUSE_HEIGHT * MOUSE_WIDTH, &tmpAddr);
	mouseCtl.buffer = (uint8*)tmpAddr;
	if(re == TRUE) {
		re = sheet_add_mouse(mouseCtl.buffer, MOUSE_WIDTH, MOUSE_HEIGHT, (mouseCtl.x - (MOUSE_WIDTH / 2)), (mouseCtl.y - (MOUSE_HEIGHT / 2)), NONE_COL, &(mouseCtl.sheet));
		if(re == TRUE) {
			fill_box(mouseCtl.sheet, NONE_COL, 0, 0, MOUSE_WIDTH, MOUSE_HEIGHT);
			fill_box(mouseCtl.sheet, WHITE, 3, 3, 7, 7);
			fill_box(mouseCtl.sheet, WHITE, 0, 4, 10, 6);
			fill_box(mouseCtl.sheet, WHITE, 4, 0, 6, 10);
			return TRUE;
		}
		return FALSE;
	}
	return FALSE;
}

void MouseKeyboardDeamon()
{
	BOOTINFO* info = (BOOTINFO*)ADDR_BOOTINFO;
	uint8 data, tmp;
	int32 x, y, topLeftX, topLeftY, bottomRightX, bottomRightY;
	WND* wnd;
	
	io_cli();
	if(GetLoopArrayDataCnt(&mouseLoopArray) >= 3) {
		io_sti();
		// decode mouse
		if(mouseCtl.isMouseOff == TRUE) {
			GetFromLoopArray(&mouseLoopArray, &data);
			if(data == 0xFA) {
				mouseCtl.isMouseOff = FALSE;
			}
		}
		else {
			GetFromLoopArray(&mouseLoopArray, &data);
			mouseCtl.key = data & 0x07;
			tmp = data;
			GetFromLoopArray(&mouseLoopArray, &data);
			x = data;
			if((tmp & 0x10) != 0) {
				x |= 0xffffff00;
			}
			GetFromLoopArray(&mouseLoopArray, &data);
			y = data;
			if((tmp & 0x20) != 0) {
				y |= 0xffffff00;
			}
			y = -y;
		
			// move mouse
			mouseCtl.x += x;
			mouseCtl.x = (mouseCtl.x > info->screenWidth) ? info->screenWidth : mouseCtl.x;
			mouseCtl.x = (mouseCtl.x < 0) ? 0 : mouseCtl.x;
			mouseCtl.y += y;
			mouseCtl.y = (mouseCtl.y > info->screenHeight) ? info->screenHeight : mouseCtl.y;
			mouseCtl.y = (mouseCtl.y < 0) ? 0 : mouseCtl.y;
			mouseCtl.sheet->topLeftX = (mouseCtl.x - (MOUSE_WIDTH / 2));
			mouseCtl.sheet->topLeftY = (mouseCtl.y - (MOUSE_HEIGHT / 2));
			
			// 窗口的叠加与移动处理
			if(mouseCtl.key != 0) {
				if(wnd_is_click_on_wnd(&wnd) == TRUE) {
					wnd_pin_top(wnd);
					topLeftX = ((wnd->wndRect.topLeftX - x) < wnd->wndRect.topLeftX) ? wnd->wndRect.topLeftX - x : wnd->wndRect.topLeftX;
					topLeftY = ((wnd->wndRect.topLeftY - y) < wnd->wndRect.topLeftY) ? wnd->wndRect.topLeftY - y : wnd->wndRect.topLeftY;
					bottomRightX = ((wnd->wndRect.bottomRightX - x) > wnd->wndRect.bottomRightX) ? wnd->wndRect.bottomRightX - x : wnd->wndRect.bottomRightX;
					bottomRightY = ((wnd->wndRect.bottomRightY - y) > wnd->wndRect.bottomRightY) ? wnd->wndRect.bottomRightY - y : wnd->wndRect.bottomRightY;
					sheet_add_redraw_region(topLeftX, topLeftY, bottomRightX, bottomRightY);
				}
				if(wnd_is_click_on_close(&wnd) == TRUE) {
					wnd_del(wnd);
				}
				else if(wnd_is_click_on_title(&wnd) == TRUE) {
					wnd_move(wnd, x, y);
				}
			}
			
			// redraw whole sheet 
			sheet_draw_sub_withoutMouse(mouseCtl.sheet->topLeftX - x, mouseCtl.sheet->topLeftY - y, mouseCtl.sheet->topLeftX + MOUSE_WIDTH - x, mouseCtl.sheet->topLeftY + MOUSE_HEIGHT - y);
			sheet_draw_sub(mouseCtl.sheet->topLeftX, mouseCtl.sheet->topLeftY, mouseCtl.sheet->topLeftX + MOUSE_WIDTH, mouseCtl.sheet->topLeftY + MOUSE_HEIGHT);
		}
	}
	else if(GetFromLoopArray(&keyboardLoopArray, &data) == TRUE) {
		io_sti();
		
		fill_box(bgSheet, BLACK, 0, 500, 100, 516);
		put_ascii(bgSheet, ((data & 0xF0) >> 4) < 0x0A ? ((data & 0xF0) >> 4) + '0' : (((data & 0xF0) >> 4) - 10) + 'A', WHITE, 0, 500);
		put_ascii(bgSheet, (data & 0x0F) < 0x0A ? (data & 0x0f) + '0' : ((data & 0x0f) - 10) + 'A', WHITE, 8, 500);
		sheet_draw_sub(0, 500, 100, 516);
		
		switch(data) {
			case KEY_CTRL_D:
				keyboardCtl.CtrlOn = TRUE;
				break;
			case KEY_LSHIFT_D:
			case KEY_RSHIFT_D:
				keyboardCtl.ShiftOn = TRUE;
				break;
			case KEY_ALT_D:
				keyboardCtl.AltOn = TRUE;
				break;
			case KEY_CAPSLOCK_D:
				keyboardCtl.CapsLockOn = !(keyboardCtl.CapsLockOn);
				keyboardCtl.keyleds ^= 4;
				io_out8(PORT_KEYDAT, keyboardCtl.keyleds);
				break;
			case KEY_F1_D:
				keyboardCtl.F1_On = TRUE;
				break;
			case KEY_F2_D:
				keyboardCtl.F2_On = TRUE;
				break;
			case KEY_F3_D:
				keyboardCtl.F3_On = TRUE;
				break;
			case KEY_F4_D:
				keyboardCtl.F4_On = TRUE;
				break;
			case KEY_F5_D:
				keyboardCtl.F5_On = TRUE;
				break;
			case KEY_F6_D:
				keyboardCtl.F6_On = TRUE;
				break;
			case KEY_F7_D:
				keyboardCtl.F7_On = TRUE;
				break;
			case KEY_F8_D:
				keyboardCtl.F8_On = TRUE;
				break;
			case KEY_F9_D:
				keyboardCtl.F9_On = TRUE;
				break;
			case KEY_F10_D:
				keyboardCtl.F10_On = TRUE;
				break;
			case KEY_DEL_D:
				keyboardCtl.DelOn = TRUE;
				break;
			case KEY_NUMLOCK_D:
				keyboardCtl.NumLockOn = !(keyboardCtl.CapsLockOn);
				keyboardCtl.keyleds ^= 2;
				io_out8(PORT_KEYDAT, keyboardCtl.keyleds);
				break;
			case KEY_F11_D:
				keyboardCtl.F11_On = TRUE;
				break;
			case KEY_F12_D:
				keyboardCtl.F12_On = TRUE;
				break;
			case KEY_WIN_D:
				keyboardCtl.WinOn = TRUE;
				break;
			case KEY_CTRL_U:
				keyboardCtl.CtrlOn = FALSE;
				break;
			case KEY_LSHIFT_U:
			case KEY_RSHIFT_U:
				keyboardCtl.ShiftOn = FALSE;
				break;
			case KEY_ALT_U:
				keyboardCtl.AltOn = FALSE;
				break;
			case KEY_F1_U:
				keyboardCtl.F1_On = FALSE;
				break;
			case KEY_F2_U:
				keyboardCtl.F2_On = FALSE;
				break;
			case KEY_F3_U:
				keyboardCtl.F3_On = FALSE;
				break;
			case KEY_F4_U:
				keyboardCtl.F4_On = FALSE;
				break;
			case KEY_F5_U:
				keyboardCtl.F5_On = FALSE;
				break;
			case KEY_F6_U:
				keyboardCtl.F6_On = FALSE;
				break;
			case KEY_F7_U:
				keyboardCtl.F7_On = FALSE;
				break;
			case KEY_F8_U:
				keyboardCtl.F8_On = FALSE;
				break;
			case KEY_F9_U:
				keyboardCtl.F9_On = FALSE;
				break;
			case KEY_F10_U:
				keyboardCtl.F10_On = FALSE;
				break;
			case KEY_DEL_U:
				keyboardCtl.DelOn = FALSE;
				break;
			case KEY_F11_U:
				keyboardCtl.F11_On = FALSE;
				break;
			case KEY_F12_U:
				keyboardCtl.F12_On = FALSE;
				break;
			case KEY_WIN_U:
				keyboardCtl.WinOn = FALSE;
				break;
		}
		if(data <= KEY_TBL_SIZE && keytable0[data]) {
			if(keyboardCtl.CapsLockOn == TRUE) {
				keyboardCtl.buf[keyboardCtl.bufPos++] = (keyboardCtl.ShiftOn == FALSE) ? keytable1[data] : keytable0[data];
			}
			else {
				keyboardCtl.buf[keyboardCtl.bufPos++] = (keyboardCtl.ShiftOn == TRUE) ? keytable1[data] : keytable0[data];
			}
		}
	}
	else {
		io_stihlt();
	}
}

void MouseGetPos(POS* pos)
{
	pos->x = mouseCtl.x;
	pos->y = mouseCtl.y;
}

void MouseGetKey(KEY_DOWN* keydown)
{
	keydown->LKeyDown = ((mouseCtl.key & 0x01) != 0) ? TRUE : FALSE; 
	keydown->CKeyDown = ((mouseCtl.key & 0x02) != 0) ? TRUE : FALSE; 
	keydown->RKeyDown = ((mouseCtl.key & 0x04) != 0) ? TRUE : FALSE; 
}

void InitKeyboard()
{
	BOOTINFO* info = (BOOTINFO*)ADDR_BOOTINFO;
	keyboardCtl.bufPos = 0;
	keyboardCtl.ShiftOn = FALSE;
	keyboardCtl.AltOn = FALSE;
	keyboardCtl.CtrlOn = FALSE;
	keyboardCtl.WinOn = FALSE;
	keyboardCtl.CapsLockOn = FALSE;
	keyboardCtl.NumLockOn = FALSE;
	keyboardCtl.keyleds = (info->leds >> 4) & 7;
}

int32 KeyboardGetChar(int8* buffer, int32 cnt)
{
	int32 re = 0;
	io_cli();
	for(; re < keyboardCtl.bufPos && re < cnt; re++) {
		buffer[re] = keyboardCtl.buf[re];
	}
	if(re == keyboardCtl.bufPos) {
		keyboardCtl.bufPos = 0;
		io_sti();
		return re;
	}
	for(; re < keyboardCtl.bufPos; re++) {
		keyboardCtl.buf[re - cnt] = keyboardCtl.buf[re];
	}
	io_sti();
	return cnt;
}

