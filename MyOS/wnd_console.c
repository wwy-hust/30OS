
#include "wnd_console.h"

int32 interrupt_handler_0x0c(int32 esp)
{
	TASK* task = multiTask_NowRunning();
	WND_CONSOLE* console = (WND_CONSOLE*)*((int32*)0x0FEC);
	
	console_puts(console, "%s\n", "INT 0x0C, Stack Out-Bound Exception.");
	return (int)&(task->tss.esp0);			//让程序强制结束
}

int32 interrupt_handler_0x0d(int32 esp)
{
	TASK* task = multiTask_NowRunning();
	WND_CONSOLE* console = (WND_CONSOLE*)*((int32*)0x0FEC);
	
	console_puts(console, "%s\n", "INT 0x0D, General Protected Exception.");
	return (int)&(task->tss.esp0);			//让程序强制结束
}


static uint8 wnd_console_printBuf[256];

boolean wnd_console_create(IN TASK* curTask, IN RECT wndSize, IN int8* title, IN int8* icon, INOUT WND_CONSOLE* console)
{
	boolean re = FALSE;
	
	if(console == NULL)
		return FALSE;
	
	re = timer_add(CONSOLE_CURSOR_BLINK_TIM, &(console->cursorTim));
	console->cursorCnt = 0;
	if(re == FALSE)
		return FALSE;

	re = wnd_create(wndSize, title, icon, &(console->wnd), draw_wnd2);
	if(re == FALSE)
		return FALSE;
	
	console->task = curTask;
	console->cmd_cnt = 0;
	console->cmd_set = NULL;
	console->x = 0;
	console->y = 0;
	console->lastIsCR = TRUE;
	console->inputCmdLength = 0;
	
	for(console->xMax = 0; console->wnd->contentRect.topLeftX + 9 + console->xMax < console->wnd->contentRect.bottomRightX; console->xMax += 8);
	for(console->yMax = 0; console->wnd->contentRect.topLeftY + 16 + console->yMax < console->wnd->contentRect.bottomRightY; console->yMax += 16);
	
	return TRUE;
}

boolean wnd_console_add_cmd(WND_CONSOLE* console, int8* nameOfCmd, int8* cmdDetail, CONSOLE_CMD_FUNCTION routine)
{
	boolean re;
	uint32 addr;
	CMD_NODE* cmdNode;
	re = mem_alloc(sizeof(CMD_NODE), &addr);
	if(re == FALSE)
		return FALSE;
	
	console->cmd_cnt++;
	cmdNode = (CMD_NODE*)addr;
	strcpy(cmdNode->cmd.cmdName, nameOfCmd);
	cmdNode->cmd.cmdRoutine = routine;
	cmdNode->cmd.cmdDetail = cmdDetail;
	cmdNode->nextCmd = NULL;
	
	if(console->cmd_set == NULL)
		console->cmd_set = cmdNode;
	else {
		for(cmdNode = console->cmd_set; cmdNode->nextCmd != NULL; cmdNode = cmdNode->nextCmd);
		cmdNode->nextCmd = (CMD_NODE*)addr;
	}
	return TRUE;
}

boolean wnd_console_del_cmd(WND_CONSOLE* console, int8* nameOfCmd)
{
	CMD_NODE* tmpNode = console->cmd_set;
	
	if(strcmp(tmpNode->cmd.cmdName, nameOfCmd) == 0) {
		tmpNode = tmpNode->nextCmd;
		mem_free((uint32)(console->cmd_set), sizeof(CMD_NODE));
		console->cmd_set = tmpNode;
		return TRUE;
	}
	else {
		while(tmpNode->nextCmd != NULL) {
			if(strcmp(tmpNode->nextCmd->cmd.cmdName, nameOfCmd) == 0) {
				tmpNode = tmpNode->nextCmd->nextCmd;
				mem_free((uint32)(tmpNode->nextCmd), sizeof(CMD_NODE));
				return TRUE;
			}
			tmpNode = tmpNode->nextCmd;
		}
		return FALSE;
	}
	return FALSE;
}

void wnd_console_daemon(WND_CONSOLE* console)
{
	uint8 data;
	boolean findCmd = FALSE;
	WND* wnd = console->wnd;
	CMD_NODE* tmpCmdNode = NULL;
	int32 argc, tmp, tmpStartPos;
	int8* argv[CONSOLE_INPUT_CMD_MAX_PARM];
	
	while(console->wnd->isUsing == TRUE) {
		if(console->lastIsCR == TRUE) {
			console_put_ascii(console,'>');
			console->lastBracketX = console->x;
			console->lastBracketY = console->y;
			console->lastIsCR = FALSE;
		}
		
		if(console->wnd == wnd_get_top() && console->cursorTim->isUsing == TRUE && console->cursorTim->isTimeout == TRUE) {
			console->cursorCnt++;
			if(console->cursorCnt % 2 == 0) {
				console_put_ascii(console, '_');
				console->x -= 8;
			}
			else
				console_clear_current_block(console);
			timer_set_timeout(console->cursorTim, CONSOLE_CURSOR_BLINK_TIM);
		}
		else if(console->wnd != wnd_get_top()) {
			console_clear_current_block(console);
		}
		
		if(GetFromLoopArray(wnd->keyboardBuffer, &data) == TRUE) {
			if(data == ASC_CR) {			//接收到回车
				console->lastIsCR = TRUE;
				
				console_clear_current_block(console);
				console->x = 0;
				console->y += 16;
				console_check_rollback(console);
				
				console->inputCmd[console->inputCmdLength] = 0;
				
				for(argc = 0, tmp = 0; console->inputCmd[tmp] == ' ' && tmp < console->inputCmdLength + 1; tmp++);		//忽略空格
				if(tmp < console->inputCmdLength) {			//字符串非空格
					//解析字符串，并将参数填入 argc  argv
					for(tmpStartPos = tmp; tmp < console->inputCmdLength + 1; tmp++) {
						if(console->inputCmd[tmp] == ' ' || console->inputCmd[tmp] == '\0') {
							console->inputCmd[tmp++] = 0;
							argv[argc++] = &(console->inputCmd[tmpStartPos]);
							for(; (tmp < console->inputCmdLength + 1) && (console->inputCmd[tmp] == ' '); tmp++);		//忽略空格
							tmpStartPos = tmp;						
						}
					}
					//与命令队列中的命令进行匹配
					tmpCmdNode = console->cmd_set;
					while(tmpCmdNode != NULL) {
						if(strcmp(tmpCmdNode->cmd.cmdName, argv[0]) == 0) {
							tmpCmdNode->cmd.cmdRoutine(console, argc, argv);
							findCmd = TRUE;
							break;
						}
						tmpCmdNode = tmpCmdNode->nextCmd;
					}
					if(findCmd == FALSE) {
						if(FALSE == exec_bin(console, argc, argv)) {
							console_puts(console, "Command not found!\n");
						}
					}
					else {
						findCmd = FALSE;
					}
					console->inputCmdLength = 0;
				}
			}
			else if(data == ASC_BS) {			//接收到退格
				if(console->lastBracketX == console->x && console->lastBracketY == console->y) {
					
				}
				else {
					console->inputCmdLength--;
					if(console->inputCmdLength < 0) console->inputCmdLength = 0;
					// 删掉光标
					console_clear_current_block(console);
					// 删掉之前的字母
					console->x -= 8;
					if(console->x <= 0 && console->y == 0) {
						console->x = 0;
					}
					if(console->x < 0 && console->y > 0) {
						console->y -= 16;
						console->x = console->xMax - 8;
					}
					console_clear_current_block(console);
				}
			}
			else {
				console->inputCmd[console->inputCmdLength++] = data;
				console_clear_current_block(console);
				console_put_ascii(console, data);
			}
		}
		sheet_add_redraw_region_rect(console->wnd->wndRect);
		task_sleep(console->task, 0);
	}
	wnd_console_free(console);
}

void wnd_console_free(WND_CONSOLE* console)
{
	CMD_NODE* node = console->cmd_set;
	if(console->wnd->isUsing == TRUE) {
		wnd_del(console->wnd);
	}
	timer_del(console->cursorTim);
	while(console->cmd_set != NULL) {
		node = console->cmd_set->nextCmd;
		mem_free(console->cmd_set, sizeof(CMD_NODE));
		console->cmd_set = node;
	}
	task_stop_free(console->task);		//关闭窗口时删除该任务
}


void console_clear_current_block(WND_CONSOLE* console)
{
	WND* wnd = console->wnd;
	fill_box(wnd->wndSheet, BLACK, wnd->contentRect.topLeftX + 1 + console->x, wnd->contentRect.topLeftY + 1 + console->y, wnd->contentRect.topLeftX + 1 + console->x + 8, wnd->contentRect.topLeftY + 1 + console->y + 16);
}

void console_clear_current_line(WND_CONSOLE* console)
{
	WND* wnd = console->wnd;
	fill_box(wnd->wndSheet, BLACK, wnd->contentRect.topLeftX + 1, wnd->contentRect.topLeftY + 1 + console->y, wnd->contentRect.topLeftX + 1 + console->xMax, wnd->contentRect.topLeftY + 1 + console->y + 16);
}

void console_check_rollback(WND_CONSOLE* console)
{
	WND* wnd = console->wnd;
	
	if(console->y + wnd->contentRect.topLeftY + 17 > wnd->contentRect.bottomRightY) {
		copy_buf(wnd->wndSheet, console->xMax, console->yMax - 16, wnd->contentRect.topLeftX + 1, wnd->contentRect.topLeftY + 1, wnd->contentRect.topLeftX + 1, wnd->contentRect.topLeftY + 1 + 16);
		console->y -= 16;
		fill_box(wnd->wndSheet, BLACK, wnd->contentRect.topLeftX + 1, wnd->contentRect.topLeftY + 1 + console->y, wnd->contentRect.topLeftX + 1 + console->xMax, wnd->contentRect.topLeftY + 1 + console->y + 16);
	}
}

void console_put_ascii(WND_CONSOLE* console, uint8 data)
{
	WND* wnd = console->wnd;

	if(data == '\n') {
		console->x = 0;
		console->y += 16;
		console_check_rollback(console);
	}
	else if(data == 0x0D) {
	}
	else if(data == 0x09) {
		do{
			put_ascii(wnd->wndSheet, ' ', WHITE, wnd->contentRect.topLeftX + 1 + console->x, wnd->contentRect.topLeftY + 1 + console->y);
			console->x += 8;
		}while(((console->x / 8) % 4) != 0);
	}
	else {
		put_ascii(wnd->wndSheet, data, WHITE, wnd->contentRect.topLeftX + 1 + console->x, wnd->contentRect.topLeftY + 1 + console->y);
		console->x += 8;
	}
	
	if(console->x >= console->xMax) {
		console->x = 0;
		console->y += 16;
		console_check_rollback(console);
	}
}

void console_put_gbk(WND_CONSOLE* console, uint16 data)
{
	WND* wnd = console->wnd;

	put_gbk(wnd->wndSheet, data, WHITE, wnd->contentRect.topLeftX + 1 + console->x, wnd->contentRect.topLeftY + 1 + console->y);
	console->x += 16;
	
	if(console->x >= console->xMax) {
		console->x = 0;
		console->y += 16;
		console_check_rollback(console);
	}
}

void console_puts(WND_CONSOLE* console, const char* format, ...)
{
	va_list arg_list;
	int32 rowLetterCnt, i = 0;
	uint16 wStr;
	
	va_start(arg_list, format);
	vsprintf((char*)wnd_console_printBuf, format, arg_list);
	va_end(arg_list);
	rowLetterCnt = strlen(wnd_console_printBuf);

	if(console->x >= console->xMax) {
		console->x = 0;
		console->y += 16;
		console_check_rollback(console);
	}
	
	for(i = 0; i < rowLetterCnt;) {
		if(wnd_console_printBuf[i] < 0xA1) {
			console_put_ascii(console, wnd_console_printBuf[i]);
			i++;
		}
		else {
			wStr = (wnd_console_printBuf[i] & 0x00FF) | ((((uint16)wnd_console_printBuf[i + 1]) << 8) & 0xFF00);
			console_put_gbk(console, wStr);
			i += 2;
		}
	}
}

void console_puts1(WND_CONSOLE* console, uint8* str)
{
	int32 rowLetterCnt, i = 0;
	uint16 wStr;
	
	rowLetterCnt = strlen(str);
	if(console->x >= console->xMax) {
		console->x = 0;
		console->y += 16;
		console_check_rollback(console);
	}
	
	for(i = 0; i < rowLetterCnt;) {
		if(str[i] < 0xA1) {
			console_put_ascii(console, str[i]);
			i++;
		}
		else {
			wStr = (str[i] & 0x00FF) | ((((uint16)str[i + 1]) << 8) & 0xFF00);
			console_put_gbk(console, wStr);
			i += 2;
		}
	}
}

void console_put_n_chars(WND_CONSOLE* console, const char* format, int32 size)
{
	int32 i;
	if(console->x >= console->xMax) {
		console->x = 0;
		console->y += 16;
		console_check_rollback(console);
	}
	for(i = 0; i < size; i++) {
			console_put_ascii(console, format[i]);
	}
}

/*
	功能号1 显示单个字符（AL=字符编码）
	功能号2 显示字符串0 （EBX=字符串地址）
	功能号3 显示字符串1 （EBX=字符串地址，ECX=字符串长度）
*/
extern uint8 icon_heart[256];

static API_ROUNTINE api_routine_array[25] = 
{
/*0*/	NULL,
/*1*/	API_putchar,
/*2*/	API_puts,
/*3*/	API_putn_chars,
/*4*/	API_end_app,
/*5*/	API_create_wnd,
/*6*/	API_delete_wnd,
/*7*/	API_wnd_fillbox,
/*8*/	API_wnd_draw_text,
/*9*/	API_wnd_draw_point,
/*10*/	API_wnd_draw_line,
/*11*/	API_wnd_draw_content,
/*12*/	API_wnd_get_keyboard,
/*13*/	API_wnd_refresh,
/*14*/	API_cp_os_mem,
/*15*/	API_timer_alloc,
/*16*/	API_timer_delete,
/*17*/	API_timer_set_timeout,
/*18*/	API_timer_is_timeout,
/*19*/	API_beep,
/*20*/	API_open,
/*21*/	API_close,
/*22*/	API_seek,
/*23*/	API_read,
/*24*/	API_file_size,
};

int32 os_console_api(int32 edi, int32 esi, int32 ebp, int32 esp, int32 ebx, int32 edx, int32 ecx, int32 eax)
{
	WND_CONSOLE* console = (WND_CONSOLE*)*((int32*)0x0FEC);
	if(edx <= 24) {
		return api_routine_array[edx](edi, esi, ebp, esp, ebx, edx, ecx, eax);
	}
	else {
		console_puts(console, "%s\n", "Unknown API.");
		return 0;
	}
}

