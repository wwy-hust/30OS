
#ifndef _APP_FUNCUTILS_H_
#define _APP_FUNCUTILS_H_

#include "typedef.h"
#include "wnd.h"
#include "tim.h"

#include "stdio.h"

typedef struct FILE_HANDLE {
	uint8* buf;
	int32 size;
	int32 pos;
}FILE_HANDLE;

void api_putchar(uint8 data);
void api_printf(const char* format, ...);
void api_puts(uint8* pData);
void api_put_n_char(uint8* pData, int32 len);
void api_end(void);

WND* api_create_wnd(int32 width, int32 height, int8* title, uint8* icon);
boolean api_wnd_fillbox(WND* wnd, int32 width, int32 height, int32 topLeftX, int32 topLeftY, uint8 color);
boolean api_wnd_puts(WND* wnd, int8* str, int32 topLeftX, int32 topLeftY, uint8 color);
boolean api_wnd_point(WND* wnd, int32 x, int32 y, uint8 color);
boolean api_wnd_line(WND* wnd, int32 x0, int32 y0, int32 x1, int32 y1, uint8 color);
boolean api_wnd_setContentBuf(WND* wnd, uint8* localBuffer);
boolean api_wnd_close(WND* wnd);
boolean api_wnd_refresh(WND* wnd);
int8 api_wnd_get_keyboard(WND* wnd, int32 mode);		//mode == 0:没有输入时返回-1，不休眠；mode == 1休眠之道有输入
void api_memcpy(void* srcBase, int32 size, void* dstBase);	//可用于拷贝WND结构体，用以获知结构体中的参数

TIM* api_timer_alloc(uint32 timeout);
boolean api_timer_settime(TIM* timer, uint32 timeout);
boolean api_timer_isTimeout(TIM* timer);
boolean api_timer_free(TIM* timer);

void api_beep(int32 tone);

FILE_HANDLE* api_open(uint8* fileName);
boolean api_close(FILE_HANDLE* file);
boolean api_seek(FILE_HANDLE* file, uint8 mode, uint32 pos);
int32 api_read(FILE_HANDLE* file, uint8* buf, int32 bufMaxLen);
int32 api_file_size(FILE_HANDLE* file);

#endif
