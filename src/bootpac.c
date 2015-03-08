/** @file bootpac.c
 *  @brief Main source of the OS
 *
 *  This file contains the main function, initOS.
 */
 
#include "asmhead.h"
#include "funcUtils.h"
#include "graphic.h"
#include "globalTbl.h"
#include "interrupt.h"
#include "dataStruct.h"
#include "mouse_keyboard.h"
#include "mem.h"
#include "tim.h"
#include "multiTask.h"
#include "wnd_console.h"
#include "cmd.h"
#include "fat.h"

#include "stdio.h"

extern uint8 icon_heart[256];
extern TIM_CTL timerCtl;
extern SHEET_CTL sheetCtl;
extern TASK_CTL taskCtl;
extern KEYBOARD_CTL keyboardCtl;


SHEET* bgSheet, *wndSheet, *testSheet;
extern TIM* task_timer;
TIM *bTim;
int a = 0, b = 0, c = 0;
char printBuf[64];
extern int32 mt_tr;
int kk = 0, t = 0;

typedef struct AFILEINFO{
	uint8 name[8], ext[3], type;
	uint8 reserve[10];
	uint16 time, date, clustno;
	uint32 size;
}AFILEINFO;

extern FAT_CTL fatCtl;
/** @brief The Task of Console.
 *  Program like windows CMD
 *  @param bgSheet sheet of the console
 *  @param curTask handle of current task
 *  @return NULL
 */
void task_console_main(SHEET *bgSheet, TASK* curTask)
{
	int32 offset, i;
	uint8* hzk16Base;
	RECT wndRect;
	WND_CONSOLE wnd;
	boolean re;
	
	wndRect.topLeftX = 150;
	wndRect.topLeftY = 150;
	wndRect.bottomRightX = 750;
	wndRect.bottomRightY = 700;

	re = wnd_console_create(curTask, wndRect, "Cmd", icon_heart, &wnd);
//	*((int*)0x0FEC) = (int)&wnd;						//console

	wnd_console_add_cmd(&wnd, "mem", "Show the memory useage.", (CONSOLE_CMD_FUNCTION)mem);
	wnd_console_add_cmd(&wnd, "wnd", "Show the wnd state.", (CONSOLE_CMD_FUNCTION)showWnd);
	wnd_console_add_cmd(&wnd, "help", "Show cmd reference", (CONSOLE_CMD_FUNCTION)help);
	wnd_console_add_cmd(&wnd, "ls", "Show files under directory", (CONSOLE_CMD_FUNCTION)ls);
	wnd_console_add_cmd(&wnd, "clear", "Clear screen", (CONSOLE_CMD_FUNCTION)clear);
	wnd_console_add_cmd(&wnd, "type", "Show the Content of file", (CONSOLE_CMD_FUNCTION)type);
	wnd_console_add_cmd(&wnd, "hex", "Show the Hex Content of file", (CONSOLE_CMD_FUNCTION)hex);
	wnd_console_add_cmd(&wnd, "top", "Show the tasks.", (CONSOLE_CMD_FUNCTION)top);
	wnd_console_add_cmd(&wnd, "exit", "Quit the Console.", (CONSOLE_CMD_FUNCTION)exit);

	wnd_console_daemon(&wnd);
}

struct SEGMENT_DESCRIPTOR *gdt = (struct SEGMENT_DESCRIPTOR *) ADDR_GDT;

/** @brief Init OS after load.
 *  @param NULL
 *  @return NULL
 */
void OS_Init(void)
{
	BOOTINFO* info;
	
	int32 screenWidth, screenHeight;
	uint8* bgBuf, *resVram;
	boolean re[2];
	uint32 tmpAddr;

	InitPeripheralBuffer();
	
	init_gdtidt();
	init_pic();
	io_sti();		// 恢复中断
	tim_init();
	io_out8(PIC0_IMR, 0xf8); /* PIT和PIC1和键盘设置为许可(11111000) */
	io_out8(PIC1_IMR, 0xef); /* 鼠标设置为许可(11101111) */
	
	info = (BOOTINFO*) ADR_BOOTINFO;
	screenHeight = info->screenHeight;
	screenWidth = info->screenWidth;
	
	init_palette();
	mem_init();
	fat_init();
	re[0] = mem_alloc(768 * 1024, &tmpAddr);
	resVram = (uint8*)tmpAddr;
	sheet_init(resVram);
	
	InitMouse();
	InitKeyboard();
	
	re[1] = mem_alloc(768 * 1024, &tmpAddr);
	bgBuf = (uint8*)tmpAddr;
	
	//screenWidth * screenHeight
	sheet_add(bgBuf, screenWidth, screenHeight, 0, 0, NONE_COL, &bgSheet);

	fill_box(bgSheet, DARKGRASS, 0, 0, info->screenWidth, info->screenHeight);
	if(re[0] == FALSE || re[1] == FALSE) {
		fill_box(bgSheet, RED, 0, 0, info->screenWidth, info->screenHeight);
	}
}

void OS_Main(void)
{
	uint32 tmpAddr;
	WND* topWnd;
	int8 buf[10];
	TASK	*task;// *taskB, *taskC;
	WND_CONSOLE* console;
	char* wStr[64];
	
	OS_Init();
	
	fill_box(bgSheet, BLACK, 0, 0, 160, 16);
	sprintf(printBuf, "MemVol:%dMB", mem_getTotalSize() / (1024 * 1024));
	puts_str(bgSheet, printBuf, WHITE, 0, 0);

	multiTask_init();
//	console_puts(&wnd, "%s\n", "学挖掘机^_^，还到山东蓝翔！wwy");
	strcpy(wStr, "a学挖掘机到底!にほんご哪家强？");
	puts_str(bgSheet, wStr, WHITE, 32, 32);
	strcpy(wStr, "学挖掘机^_^，还到山东蓝翔！");
	puts_str(bgSheet, wStr, GREEN, 32, 48);
//	put_ascii(bgSheet, 0xD0D6, WHITE, 32, 32);
//	put_ascii(bgSheet, 'w', BLACK, 32, 48);
	sheet_draw_all();
		
	while(1){
		if(KeyboardGetChar(buf, 1)) {
			fill_box(bgSheet, BLACK, 0, 232, 100, 248);
			puts_str(bgSheet, buf, WHITE, 0, 232);
			sheet_add_redraw_region(0, 232, 100, 248);
			topWnd = wnd_get_top();
			if(topWnd != NULL) {
				PutIntoLoopArray(topWnd->keyboardBuffer, buf[0]);
			}
		}
		MouseKeyboardDeamon();
		sheet_draw_deamon();
		if(keyboardCtl.AltOn == TRUE && keyboardCtl.F4_On == TRUE) {
			console = (WND_CONSOLE*)*((int*)0xFEC);
			if(console->task->tss.ss0 != 0) {
				console_puts(console, "\nBreak(key) :\n");
				io_cli();
				console->task->tss.eax = (int) &(console->task->tss.esp0);
				console->task->tss.eip = (int) asm_end_app;
				io_sti();
				keyboardCtl.AltOn = FALSE;
				keyboardCtl.F4_On = FALSE;
			}
		}
		else if(keyboardCtl.WinOn == TRUE && keyboardCtl.F1_On == TRUE) {
			//Console Task
			mem_alloc(64 * 1024, &tmpAddr);
			tmpAddr += 64 * 1024 - 12;
			*((int*)(tmpAddr + 4)) = (int)bgSheet;
			task_alloc("Task Console", (int32)&task_console_main, tmpAddr, 0x00000202, 1, &task);
			*((int*)(tmpAddr + 8)) = (int)task;
			task->tss.cs = 2 * 8;		//目前必须为 2 × 8
			task_run(task);
				keyboardCtl.WinOn = FALSE;
				keyboardCtl.F1_On = FALSE;
		}
	}
	// 不能有 return 
}

