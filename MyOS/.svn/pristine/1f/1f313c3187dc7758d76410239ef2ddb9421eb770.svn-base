
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

