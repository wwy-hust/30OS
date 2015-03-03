
#include "stdio.h"
#include "string.h"

#include "graphic.h"
#include "app_funcUtils.h"
#include "icon.c"
#include "tim.h"
#include "wnd.h"

void func_main()
{
	WND* wnd = NULL;
	char buf[256];
	WND local;
	TIM* tim;
	uint8 data;
	int32 x = 40, y = 40;
	int32 tone = 20000;

	api_puts("Hello, World!...\n");
	wnd = api_create_wnd(300, 100, "This is a game.", icon_heart);
	api_memcpy(wnd, sizeof(WND), &local);
	sprintf(buf, "%d\n", local.wndRect.topLeftX);
	api_puts(buf);
	x = local.contentRect.topLeftX;
	y = local.contentRect.topLeftY;
//	api_wnd_point(wnd, 100, 50, RED);
//	api_wnd_line(wnd, 100, 10, 130, 35, RED);
	tim = api_timer_alloc(100);

	while(1) {
		api_wnd_fillbox(wnd, 20, 20, x, y, GREEN);
		api_wnd_refresh(wnd);
		data = api_wnd_get_keyboard(wnd, 1);
		api_wnd_fillbox(wnd, 20, 20, x, y, LIGHTGREY);
		switch(data) {
			case 'w':
				y -= 20;
				y = (y < local.contentRect.topLeftY) ? local.contentRect.topLeftY : y;
				break;
			case 'a':
				x -= 20;
				x = (x < local.contentRect.topLeftX) ? local.contentRect.topLeftX : x;
				break;
			case 's':
				y += 20;
				y = (y > local.contentRect.bottomRightY - 20) ? local.contentRect.bottomRightY - 20 : y;
				break;
			case 'd':
				x += 20;
				x = (x > local.contentRect.bottomRightX - 20)? local.contentRect.bottomRightX - 20 : x;
				break;
			default:
				break;
		}
		api_putchar(data);
		api_wnd_fillbox(wnd, 20, 20, x, y, GREEN);
		if(data == 0x1B) {
			api_wnd_close(wnd);
			break;
		}
		else if(data == ASC_CR) {
			api_timer_free(tim);
		}
		if(api_timer_isTimeout(tim) == TRUE) {
			api_wnd_puts(wnd, "Game end is not game over", 50, 50, RED);
			api_beep(tone);
			tone -= (tone / 100);
			api_timer_settime(tim, 100);
		}
	}
//	api_putchar(wnd->title[0]);
	
/*	buf[1] = wnd->title[1];
	buf[2] = wnd->title[2];
	buf[3] = wnd->title[3];
	buf[4] = wnd->title[4];
	buf[5] = 0;
	api_puts(buf);
	*/
	api_end();
	
}
