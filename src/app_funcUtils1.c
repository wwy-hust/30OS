/** @file app_funcUtils1.c
 *  @brief application could call system function through function listed here.
 *
 *  Everytime application call os api. It actually trigger an interrupt with INT 0x40.
 *  The request would redirect to api.c to do actually work.
 */
 
#include "app_funcUtils.h"

uint8 printfBuf[256];

void api_printf(const char* format, ...)
{
	va_list arg_list;
	va_start(arg_list, format);
	vsprintf((char*)printfBuf, format, arg_list);
	va_end(arg_list);

	api_puts(printfBuf);
}

