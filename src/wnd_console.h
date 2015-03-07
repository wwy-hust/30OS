/** @file wnd_console.h
 *  @brief advance wraper of window, the console could be used to interact with user.
 */
 
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


/** @addtogroup console 
 *  @brief a wrapper of window
 *  @{
 */
#define CONSOLE_CURSOR_BLINK_TIM		100

#define CONSOLE_INPUT_CMDNAME_LEN		32
#define CONSOLE_INPUT_CMD_MAX				128
#define CONSOLE_INPUT_CMD_MAX_PARM	32

typedef boolean (*CONSOLE_CMD_FUNCTION)(void* console, int32 argc, int8* argv[]);

/** @brief command info. */
typedef struct CMD {
	int8 cmdName[CONSOLE_INPUT_CMDNAME_LEN];
	int8* cmdDetail;
	CONSOLE_CMD_FUNCTION cmdRoutine;
}CMD;

/** @brief command node in console, managed as list. */
typedef struct CMD_NODE {
	CMD cmd;
	struct CMD_NODE* nextCmd;
}CMD_NODE;

/** @brief console info. 
 *  wrapper of windows, you could use this structure directly to show a console on screen.
 */
typedef struct WND_CONSOLE {
	WND* wnd;								///< the wnd used to display on the screen and manage throw windows module.
	TASK* task;							///< task handler running the console
	int32 cmd_cnt;					///< number of command 
	CMD_NODE* cmd_set;			///< command lists
	
	TIM* cursorTim;					///< cursor blink timer
	int8 cursorCnt;
	int32 lastBracketX;
	int32 lastBracketY;

	int32 x;								///< control cursor position, current x pos of cursor
	int32 y;								///< control cursor position, current y pos of cursor
	int32 xMax;							///< control cursor position, Max X in console
	int32 yMax;							///< control cursor position, Max Y in console
	boolean lastIsCR;				///< test whether user type ENTER, if yes, exec the command
		
	int8 inputCmd[CONSOLE_INPUT_CMD_MAX];		///< buffer to store the input chars
	int32 inputCmdLength;										///< size of chars in buffer
}WND_CONSOLE;


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
/// @}

/// @addtogroup hal
/// @{
int32 interrupt_handler_0x0c(int32 esp);
int32 interrupt_handler_0x0d(int32 esp);
int32 os_console_api(int32 edi, int32 esi, int32 ebp, int32 esp, int32 ebx, int32 edx, int32 ecx, int32 eax);
/// @}

#endif
