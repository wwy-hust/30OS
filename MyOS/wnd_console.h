
#ifndef _WND_CONSOLE_H_
#define _WND_CONSOLE_H_

#include "string.h"
#include "stdio.h"
#include "stdarg.h"


#include "typedef.h"
#include "api.h"
#include "funcUtils.h"
#include "wnd.h"
#include "tim.h"
#include "fat.h"
#include "multiTask.h"



#define CONSOLE_CURSOR_BLINK_TIM		100

#define CONSOLE_INPUT_CMDNAME_LEN		32
#define CONSOLE_INPUT_CMD_MAX				128
#define CONSOLE_INPUT_CMD_MAX_PARM	32

typedef boolean (*CONSOLE_CMD_FUNCTION)(void* console, int32 argc, int8* argv[]);

typedef struct CMD {
	int8 cmdName[CONSOLE_INPUT_CMDNAME_LEN];
	int8* cmdDetail;
	CONSOLE_CMD_FUNCTION cmdRoutine;
}CMD;

typedef struct CMD_NODE {
	CMD cmd;
	struct CMD_NODE* nextCmd;
}CMD_NODE;

typedef struct WND_CONSOLE {
	WND* wnd;
	TASK* task;
	int32 cmd_cnt;
	CMD_NODE* cmd_set;
	
	TIM* cursorTim;						//cursor
	int8 cursorCnt;
	int32 lastBracketX;
	int32 lastBracketY;

	int32 x;
	int32 y;
	int32 xMax;
	int32 yMax;		//control cursor position
	boolean lastIsCR;
		
	int8 inputCmd[CONSOLE_INPUT_CMD_MAX];
	int32 inputCmdLength;
}WND_CONSOLE;

int32 interrupt_handler_0x0c(int32 esp);
int32 interrupt_handler_0x0d(int32 esp);
int32 os_console_api(int32 edi, int32 esi, int32 ebp, int32 esp, int32 ebx, int32 edx, int32 ecx, int32 eax);


boolean wnd_console_create(IN TASK* curTask, IN RECT wndSize, IN int8* title, IN int8* icon, INOUT WND_CONSOLE* console);
boolean wnd_console_add_cmd(WND_CONSOLE* console, int8* nameOfCmd, int8* cmdDetail, CONSOLE_CMD_FUNCTION routine);
boolean wnd_console_del_cmd(WND_CONSOLE* console, int8* nameOfCmd);
void wnd_console_daemon(WND_CONSOLE* console);
void wnd_console_free(WND_CONSOLE* console);

// used only in private function
void console_clear_current_block(WND_CONSOLE* console);
void console_clear_current_line(WND_CONSOLE* console);
void console_check_rollback(WND_CONSOLE* console);
void console_put_ascii(WND_CONSOLE* console, uint8 data);
void console_put_gbk(WND_CONSOLE* console, uint16 data);
void console_puts(WND_CONSOLE* console, const char* format, ...);
void console_puts1(WND_CONSOLE* console, uint8* str);
void console_put_n_chars(WND_CONSOLE* console, const char* format, int32 size);

#endif
