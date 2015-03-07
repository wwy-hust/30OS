#include "stdio.h"
#include "string.h"

#include "graphic.h"
#include "app_funcUtils.h"
#include "icon.c"
#include "tim.h"
#include "wnd.h"

uint8 buf[35];
uint8 buf1[128];
void func_main()
{
	FILE_HANDLE* file, *file1;
	FILE_HANDLE fileStruct;
	int i, j;
	
	api_puts("噗，接下来要读文件啦..\n");
	file = api_open("README.TXT");
	api_memcpy(file, sizeof(FILE_HANDLE), &fileStruct);

//	file = file1;
	do {
		j = api_read(file, buf, 30);
		api_printf("[%d]", j);
		buf[j] = 0;
		api_puts(buf);
	}while(j > 0);
	api_end();
}
