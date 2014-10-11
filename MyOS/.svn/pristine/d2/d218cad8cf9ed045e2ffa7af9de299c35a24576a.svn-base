
#include "stdio.h"
#include "string.h"

#include "graphic.h"
#include "app_funcUtils.h"
#include "icon.c"
#include "tim.h"
#include "wnd.h"

uint8 buf[256 * 256];
void func_main()
{
	WND* wnd = NULL;
	WND local;
	TIM* tim;
	uint8 data;
	int32 x, y, blkX, blkY;
	int32 tone = 20000;

	wnd = api_create_wnd(265, 283, "Platte.", icon_heart);
	api_memcpy(wnd, sizeof(WND), &local);
	sprintf(buf, "%d, %d    %d, %d\n", local.contentRect.topLeftX, local.contentRect.topLeftY, local.contentRect.bottomRightX, local.contentRect.bottomRightY);
	api_puts(buf);
	x = local.contentRect.topLeftX;
	y = local.contentRect.topLeftY;
//	api_wnd_point(wnd, 100, 50, RED);
//	api_wnd_line(wnd, 100, 10, 130, 35, RED);
	tim = api_timer_alloc(100);
	
	tone = 16;
	for(blkX =0; blkX < 16; blkX++) {
		for(blkY = 0; blkY < 16; blkY++) {
			for(x = blkX * 16; x < (blkX * 16 + 16); x++) {
				for(y = blkY * 16; y < (blkY * 16 + 16); y++) {
					buf[x + y * 256] = tone;
				}
			}
			tone++;
			if(tone >= 232)
				goto end;
		}
	}
end:
//	boolean api_wnd_setContentBuf(WND* wnd, uint8* localBuffer);
	api_wnd_setContentBuf(wnd, buf);
	api_end();
}
