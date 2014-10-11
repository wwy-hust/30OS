
#include "cmd.h"


extern TASK_CTL taskCtl;
boolean top(WND_CONSOLE* console, int32 argc, int8* argv[])
{
	int32 i, j, nameLen;
	console_puts(console, "Task Cnt (%d), Running (%d), Stoped (%d), Sleeping (%d).\n", taskCtl.taskNum, taskCtl.runningTasksEnd, taskCtl.stoppedTasksEnd, taskCtl.sleepTasksEnd);
	console_puts(console, "Task Name       Id   priority   status\n");
	for(i = 0; i < taskCtl.runningTasksEnd; i++) {
		nameLen = strlen(taskCtl.runningTasks[i]->taskName);
		console_puts(console, "%s", taskCtl.runningTasks[i]->taskName);
		for(j = 0; j < (16 - nameLen) && j >= 0; j++) console_put_ascii(console, ' ');
		console_puts(console, "%d\t\t%d       running\n", taskCtl.runningTasks[i]->taskId, taskCtl.runningTasks[i]->priority);
	}
	for(i = 0; i < taskCtl.stoppedTasksEnd; i++) {
		nameLen = strlen(taskCtl.stoppedTasks[i]->taskName);
		console_puts(console, "%s", taskCtl.stoppedTasks[i]->taskName);
		for(j = 0; j < (16 - nameLen) && j >= 0; j++) console_put_ascii(console, ' ');
		console_puts(console, "%d\t\t%d       stopped\n", taskCtl.stoppedTasks[i]->taskId, taskCtl.stoppedTasks[i]->priority);
	}
	for(i = 0; i < taskCtl.sleepTasksEnd; i++) {
		nameLen = strlen(taskCtl.sleepTasks[i]->taskName);
		console_puts(console, "%s", taskCtl.sleepTasks[i]->taskName);
		for(j = 0; j < (16 - nameLen) && j >= 0; j++) console_put_ascii(console, ' ');
		console_puts(console, "%d\t\t%d       stopped\n", taskCtl.sleepTasks[i]->taskId, taskCtl.sleepTasks[i]->priority);
	}
	
	return TRUE;
}

extern WND_CTL wndCtl;
extern SHEET_CTL sheetCtl;
boolean showWnd(WND_CONSOLE* console, int32 argc, int8* argv[])
{
	int32 i, nameLen;
	console_puts(console, "Wnd Cnt (%d)\n", wndCtl.wndCnt);
	console_puts(console, "Sheet Cnt (%d)\n", sheetCtl.topZ);
	for(i = 0; i < wndCtl.wndCnt; i++) {
		console_puts(console, "%d  %s  %d - %d, %d\n", wndCtl.sortedWnd[i]->isUsing, wndCtl.sortedWnd[i]->title, wndCtl.sortedWnd[i]->z, wndCtl.sortedWnd[i]->wndSheet->no, wndCtl.sortedWnd[i]->wndSheet->z);
	}
	return TRUE;
}

boolean mem(WND_CONSOLE* console, int32 argc, int8* argv[])
{
	console_puts(console, "Mem totalSize :%d KB\n", mem_getTotalSize() / 1024);
	console_puts(console, "Mem freeSize  :%d KB\n", mem_getFreeSum() / 1024);
	console_puts(console, "Mem usedSize  :%d KB\n", mem_getUsedSum() / 1024);

	return TRUE;
}

boolean help(WND_CONSOLE* console, int32 argc, int8* argv[])
{
	CMD_NODE* tmpNode;
	int32 bufLen;
	int8 buf[256] = "";
	
	console_puts(console, "Supported Cmd (%d) :\n", console->cmd_cnt);

	for(tmpNode = console->cmd_set; tmpNode != NULL; tmpNode = tmpNode->nextCmd) {
		strcat(buf, tmpNode->cmd.cmdName);
		bufLen = strlen(buf);
		buf[bufLen] = ' ';
		buf[bufLen + 1] = 0;
	}
	console_puts(console, "%s\n", buf);
	
	if(argc >= 2) {
		for(tmpNode = console->cmd_set; tmpNode != NULL; tmpNode = tmpNode->nextCmd) {
			if(strcmp(tmpNode->cmd.cmdName, argv[1]) == 0) {
				console_puts(console, "%s : %s\n", tmpNode->cmd.cmdName, tmpNode->cmd.cmdDetail);
				return TRUE;
			}
		}
		console_puts(console, "Unknown Command.\n");
	}
	return TRUE;
}

boolean ls(WND_CONSOLE* console, int32 argc, int8* argv[])
{
	uint8 tmp, endPos, blankCnt;
	int8 name[9], ext[4];
	int8 blank[10] = "         ";
	FILEINFO* finfo = (FILEINFO*)(ADR_DISKIMG + 0x002600);
	
	for(tmp = 0; tmp < 224; tmp++) {
		if(finfo[tmp].name[0] == 0x00) break;
		if(finfo[tmp].name[0] != 0xe5) {
			blankCnt = 0;
			for(endPos = 7; finfo[tmp].name[endPos] == ' ' && endPos > 0; endPos--);
			strncpy(name, finfo[tmp].name, endPos + 1);
			name[endPos + 1] = 0;
			blankCnt += (7 - endPos);		//空格的数目
			for(endPos = 2; finfo[tmp].ext[endPos] == ' ' && endPos > 0; endPos--);
			strncpy(ext, finfo[tmp].ext, endPos + 1);
			ext[endPos + 1] = 0;
			blankCnt += (2 - endPos);		//空格的数目
			blank[blankCnt] = 0;				//空格
			console_puts(console, "%s.%s%s  %7d\n", name, ext, blank, finfo[tmp].size);
			blank[blankCnt] = ' ';
		}
	}

	return TRUE;
}

boolean clear(WND_CONSOLE* console, int32 argc, int8* argv[])
{
	WND* wnd = console->wnd;
	console->x = 0;
	console->y = 0;
	fill_box(wnd->wndSheet, BLACK, wnd->contentRect.topLeftX + 1, wnd->contentRect.topLeftY + 1, wnd->contentRect.bottomRightX, wnd->contentRect.bottomRightY);
	return TRUE;
}

extern FAT_CTL fatCtl;
boolean type(WND_CONSOLE* console, int32 argc, int8* argv[])
{
	FILE_NODE* file;
	
	if(argc < 2) {
		console_puts(console, "Please input filename\n");
		return FALSE;
	}

	file = fat_searchFile1(argv[1]);
	if(file != NULL && fat_readFile(file) == TRUE) {
		console_puts1(console, file->buf);
		return TRUE;
	}
	console_puts(console, "file not found.\n");
	return FALSE;
}

boolean hex(WND_CONSOLE* console, int32 argc, int8* argv[])
{
	FILE_NODE* file;
	int32 i;
	
	if(argc < 2) {
		console_puts(console, "Please input filename\n");
		return FALSE;
	}
	file = fat_searchFile1(argv[1]);
	if(file != NULL && fat_readFile(file) == TRUE) {
		for(i = 0; i < file->fileSize; i++) {
			console_puts(console, "%02x ", file->buf[i]);
			if(i % 16 == 15) {
				console_put_ascii(console, 0x0A);
			}
		}
		console_put_ascii(console, 0x0A);
		return TRUE;
	}
	console_puts(console, "file not found.\n");
	return FALSE;
}

boolean exit(WND_CONSOLE* console, int32 argc, int8* argv[])
{
	wnd_console_free(console);
	return TRUE;
}

boolean exec_bin(WND_CONSOLE* console, int32 argc, int8* argv[])
{
	FILEINFO* finfo = (FILEINFO*)(ADR_DISKIMG + 0x002600);
	struct SEGMENT_DESCRIPTOR *gdt = (struct SEGMENT_DESCRIPTOR *) ADDR_GDT;
	uint32 size, tmp, endPos, tmpAddr, blankCnt, *fatBase, dataSize, dataAddr, segSize, esp, i;
	uint8 *fileBuf;
	int8 name[9], ext[4], argvName[9];
	TASK	*task = multiTask_NowRunning();
	
	if(mem_alloc(4 * 2880, &fatBase) == FALSE)
		return FALSE;
	fat_decodeFat(fatBase, 2880, ADR_DISKIMG + 0x000200);
	
	for(endPos = strlen(argv[0]); argv[0][endPos] != '.' && endPos > 0; endPos--);
	if(endPos != 0) {
		strncpy(argvName, argv[0], endPos);
		argvName[endPos] = 0;
	}
	else
		strcpy(argvName, argv[0]);
	
	for(tmp = 0; tmp < 224; tmp++) {
		if(finfo[tmp].name[0] == 0x00) break;
		if(finfo[tmp].name[0] != 0xe5) {
			blankCnt = 0;
			for(endPos = 7; finfo[tmp].name[endPos] == ' ' && endPos > 0; endPos--);
			strncpy(name, finfo[tmp].name, endPos + 1);
			name[endPos + 1] = 0;
			blankCnt += (7 - endPos);		//空格的数目
			for(endPos = 2; finfo[tmp].ext[endPos] == ' ' && endPos > 0; endPos--);
			strncpy(ext, finfo[tmp].ext, endPos + 1);
			ext[endPos + 1] = 0;
			blankCnt += (2 - endPos);		//空格的数目
			
			if((strcmp(argvName, name) == 0) && (strcmp(ext, "BIN") == 0)) {
				if(mem_alloc(finfo[tmp].size, &fileBuf) == FALSE)
					return FALSE;
				fat_loadFile(finfo[tmp].clustno, finfo[tmp].size, fileBuf, fatBase, ADR_DISKIMG + 0x003e00);
				if(strncmp(fileBuf + 4, "Hari", 4) == 0) {
						
					segSize = *((int*) (fileBuf + 0x0000));		//向操作系统请求的数据段大小
					if(mem_alloc(segSize, &tmpAddr) == FALSE)
						return FALSE;
					esp = *((int*) (fileBuf + 0x000C));				//ESP初始值
					dataSize = *((int*) (fileBuf + 0x0010));	//数据部分大小
					dataAddr = *((int*) (fileBuf + 0x0014));	//数据部分首地址
					*((int*)0x0FEC) = (int)console;						//console
					*((int*)0xFE8) = (int)tmpAddr;						//ds_base
	
					set_segmdesc(task->ldt + 0, finfo[tmp].size - 1, (int)fileBuf, AR_CODE32_ER + 0x60);
					set_segmdesc(task->ldt + 1, segSize - 1, (int)tmpAddr, AR_DATA32_RW + 0x60);
					for(i = 0; i < dataSize; i++) {
						((int8*)tmpAddr)[esp + i] = ((int8*)fileBuf)[dataAddr + i];
					}
					start_bin(0x1b, 0 * 8 + 4, esp, 1 * 8 + 4, &(task->tss.esp0));
					//void start_bin(int eip, int cs, int esp, int ds);
					//far_call(0, 1005 * 8);
					
					return TRUE;
				}
				else {
					console_puts(console, "%s\n", "executable binary file format error.");
					return FALSE;
				}
			}
		}
	}
	return FALSE;
}
