
#include "app_funcUtils.h"

void func_main()
{
/*	char *p = (char*)123;
	
	p[0] = 'A';
	p[1] = 'B';
	p[2] = 0;
	
	api_puts(p);
	*/
	*((int*)0x00102600) = 0;
	api_end();
	
}

