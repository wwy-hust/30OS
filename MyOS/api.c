
#include "api.h"

int32 API_putchar(int32 edi, int32 esi, int32 ebp, int32 esp, int32 ebx, int32 edx, int32 ecx, int32 eax)
{
	WND_CONSOLE* console = (WND_CONSOLE*)*((int32*)0x0FEC);
	console_put_ascii(console, (eax & 0x00FF));
	return 0;
}

int32 API_puts(int32 edi, int32 esi, int32 ebp, int32 esp, int32 ebx, int32 edx, int32 ecx, int32 eax)
{
	WND_CONSOLE* console = (WND_CONSOLE*)*((int32*)0x0FEC);
	int32 ds_base = *((int32*)0xFE8);

	console_puts(console, "%s", (int8*)(ebx + ds_base));
	return 0;
}

int32 API_putn_chars(int32 edi, int32 esi, int32 ebp, int32 esp, int32 ebx, int32 edx, int32 ecx, int32 eax)
{
	WND_CONSOLE* console = (WND_CONSOLE*)*((int32*)0x0FEC);
	int32 ds_base = *((int32*)0xFE8);

	console_put_n_chars(console, (int8*)(ebx + ds_base), ecx);
	return 0;
}

int32 API_end_app(int32 edi, int32 esi, int32 ebp, int32 esp, int32 ebx, int32 edx, int32 ecx, int32 eax)
{
	TASK* task = multiTask_NowRunning();
	return (int32)&(task->tss.esp0);
}

int32 API_create_wnd(int32 edi, int32 esi, int32 ebp, int32 esp, int32 ebx, int32 edx, int32 ecx, int32 eax)
{
	RECT rect;
	WND* wnd;
	int32 ds_base = *((int32*)0xFE8);

	rect.topLeftX = 80;
	rect.topLeftY = 80;
	rect.bottomRightX = 80 + esi;
	rect.bottomRightY = 80 + edi;
	wnd_create(rect, ds_base + ecx, ds_base + eax, &wnd, draw_wnd1);
	*(((int*)(&eax) + 18)) = wnd;
	return 0;
}

int32 API_delete_wnd(int32 edi, int32 esi, int32 ebp, int32 esp, int32 ebx, int32 edx, int32 ecx, int32 eax)
{
	if(wnd_is_valid((WND*)ebx)) {
		wnd_del((WND*)ebx);
		sheet_add_redraw_region_rect(((WND*)ebx)->wndRect);
		API_RETURN(TRUE);
	}
	else {
		API_RETURN(FALSE);
	}
	return 0;
}

int32 API_wnd_fillbox(int32 edi, int32 esi, int32 ebp, int32 esp, int32 ebx, int32 edx, int32 ecx, int32 eax)
{
	if(wnd_is_valid((WND*)ebx)) {
		fill_box(((WND*)ebx)->wndSheet, ebp, esi, edi, esi + eax, edi + ecx);
		API_RETURN(TRUE);
	}
	else {
		API_RETURN(FALSE);
	}
	return 0;
}

int32 API_wnd_draw_text(int32 edi, int32 esi, int32 ebp, int32 esp, int32 ebx, int32 edx, int32 ecx, int32 eax)
{
	int32 ds_base = *((int32*)0xFE8);
	if(wnd_is_valid((WND*)ebx)) {
		puts_str(((WND*)ebx)->wndSheet, eax + ds_base, ecx, esi, edi);
		API_RETURN(TRUE);
	}
	else {
		API_RETURN(FALSE);
	}
	return 0;
}

int32 API_wnd_draw_point(int32 edi, int32 esi, int32 ebp, int32 esp, int32 ebx, int32 edx, int32 ecx, int32 eax)
{
	if(wnd_is_valid((WND*)ebx)) {
		draw_point(((WND*)ebx)->wndSheet, esi, edi, eax);
		API_RETURN(TRUE);
	}
	else {
		API_RETURN(FALSE);
	}
	return 0;
}

int32 API_wnd_draw_line(int32 edi, int32 esi, int32 ebp, int32 esp, int32 ebx, int32 edx, int32 ecx, int32 eax)
{
	if(wnd_is_valid((WND*)ebx)) {
		draw_line(((WND*)ebx)->wndSheet, esi, edi, eax, ecx, ebp);
		API_RETURN(TRUE);
	}
	else {
		API_RETURN(FALSE);
	}
	return 0;
}

int32 API_wnd_draw_content(int32 edi, int32 esi, int32 ebp, int32 esp, int32 ebx, int32 edx, int32 ecx, int32 eax)
{
	int32 ds_base = *((int32*)0xFE8);
	if(wnd_is_valid((WND*)ebx)) {
		fill_box_with_buf(((WND*)ebx)->wndSheet, eax + ds_base, ((WND*)ebx)->contentRect.topLeftX, ((WND*)ebx)->contentRect.topLeftY, ((WND*)ebx)->contentRect.bottomRightX, ((WND*)ebx)->contentRect.bottomRightY);
		sheet_add_redraw_region_rect(((WND*)ebx)->wndRect);
		API_RETURN(TRUE);
	}
	else {
		API_RETURN(FALSE);
	}
	return 0;
}

int32 API_wnd_get_keyboard(int32 edi, int32 esi, int32 ebp, int32 esp, int32 ebx, int32 edx, int32 ecx, int32 eax)
{
	uint8 data;
	TASK* task = multiTask_NowRunning();
	if(wnd_is_valid((WND*)ebx)) {
		while(1) {
			io_cli();
			if(GetFromLoopArray(((WND*)ebx)->keyboardBuffer, &data) == TRUE) {
				*(((int*)(&eax) + 18)) = data;
				task_run(task);
				io_sti();
				break;
			}
			else if(eax != 0){
				task_sleep(task, 0);
			}
			else {
				*(((int*)(&eax) + 18)) = -1;
				io_sti();
				break;
			}
			io_sti();
		}
	}
	else {
		API_RETURN(FALSE);
	}
	return 0;
}

int32 API_wnd_refresh(int32 edi, int32 esi, int32 ebp, int32 esp, int32 ebx, int32 edx, int32 ecx, int32 eax)
{
	if(wnd_is_valid((WND*)ebx)) {
		sheet_add_redraw_region_rect(((WND*)ebx)->wndRect);
		API_RETURN(TRUE);
	}
	else {
		API_RETURN(FALSE);
	}
	return 0;
}

int32 API_cp_os_mem(int32 edi, int32 esi, int32 ebp, int32 esp, int32 ebx, int32 edx, int32 ecx, int32 eax)
{
	int32 ds_base = *((int32*)0xFE8);
	/*
	WND_CONSOLE* console = (WND_CONSOLE*)*((int32*)0x0FEC);
	int32 i;
	console_puts(console, "#%d#\n", ebx);
	for(i = 0; i < eax; i++) {
		console_puts(console, "%02x,", *(char*)(ebx + i));
	}
	*/
	memcpy(ecx + ds_base, ebx, eax);
	return 0;
}

int32 API_timer_alloc(int32 edi, int32 esi, int32 ebp, int32 esp, int32 ebx, int32 edx, int32 ecx, int32 eax)
{
	TIM* tim;
	if(timer_add(eax, &tim) == TRUE) {
		*(((int*)(&eax) + 18)) = tim;
	}
	else {
		*(((int*)(&eax) + 18)) = NULL;
	}
	return 0;
}

int32 API_timer_delete(int32 edi, int32 esi, int32 ebp, int32 esp, int32 ebx, int32 edx, int32 ecx, int32 eax)
{
	if(timer_is_valid((TIM*)ebx) == TRUE) {
		timer_del((TIM*)ebx);
	}
	else {
		API_RETURN(FALSE);
	}
	return 0;
}

int32 API_timer_set_timeout(int32 edi, int32 esi, int32 ebp, int32 esp, int32 ebx, int32 edx, int32 ecx, int32 eax)
{
	if(timer_is_valid((TIM*)ebx) == TRUE) {
		timer_set_timeout((TIM*)ebx, eax);
		API_RETURN(TRUE);
	}
	else {
		API_RETURN(FALSE);
	}
	return 0;
}

int32 API_timer_is_timeout(int32 edi, int32 esi, int32 ebp, int32 esp, int32 ebx, int32 edx, int32 ecx, int32 eax)
{
	if(timer_is_valid((TIM*)ebx) == TRUE) {
		if(((TIM*)ebx)->isTimeout == TRUE)
			API_RETURN(TRUE);
		else
			API_RETURN(FALSE);
	}
	else {
		API_RETURN(FALSE);
	}
	return 0;
}

int32 API_beep(int32 edi, int32 esi, int32 ebp, int32 esp, int32 ebx, int32 edx, int32 ecx, int32 eax)
{
	int32 i;
	if(eax == 0) {
		i = io_in8(0x61);
		io_out8(0x61, i & 0x0d);
	}
	else {
		i = 1193180000 / eax;
		io_out8(0x43, 0xb6);
		io_out8(0x42, i & 0xff);
		io_out8(0x42, 1 >> 8);
		i = io_in8(0x61);
		io_out8(0x61, (i | 0x03) & 0x0f);
	}
	return 0;
}


int32 API_open(int32 edi, int32 esi, int32 ebp, int32 esp, int32 ebx, int32 edx, int32 ecx, int32 eax)
{
	int32 ds_base = *((int32*)0xFE8);
	FILE_HANDLE* file;
	FILE_NODE* fileNode;
	
	fileNode = fat_searchFile1(ds_base + ebx);
	if(fileNode != NULL && fat_readFile(fileNode) == TRUE) {
		if(mem_alloc(sizeof(FILE_HANDLE), &file) == FALSE) {
			API_RETURN(NULL);
			return 0;
		}
		file->buf = fileNode->buf;
		file->size = fileNode->fileSize;
		file->pos = 0;
		API_RETURN(file);
		return 0;
	}
	API_RETURN(NULL);
	return 0;
}

int32 API_close(int32 edi, int32 esi, int32 ebp, int32 esp, int32 ebx, int32 edx, int32 ecx, int32 eax)
{
	FILE_HANDLE* file = eax;
	if(file == NULL) {
		API_RETURN(FALSE);
	}
	else {
//		mem_free(file->buf, file->size);
		mem_free(file, sizeof(FILE_HANDLE));
		API_RETURN(TRUE);
	}
	return 0;
}

int32 API_seek(int32 edi, int32 esi, int32 ebp, int32 esp, int32 ebx, int32 edx, int32 ecx, int32 eax)
{
	FILE_HANDLE* file = ebx;
	if(file == NULL) {
		API_RETURN(FALSE);
	}
	else {
		switch(eax) {
			case 0:
				file->pos = ecx;
				break;
			case 1:
				file->pos += ecx;
				break;
			case 2:
				file->pos = file->size - ecx;
				break;				
		}
		file->pos = (file->pos > file->size) ? file->size : file->pos;
		file->pos = (file->pos < 0) ? 0 : file->pos;			
		API_RETURN(TRUE);
	}
	return 0;
}

int32 API_read(int32 edi, int32 esi, int32 ebp, int32 esp, int32 ebx, int32 edx, int32 ecx, int32 eax)
{
//	WND_CONSOLE* console = (WND_CONSOLE*)*((int32*)0x0FEC);
	int32 ds_base = *((int32*)0xFE8);
	FILE_HANDLE* file = ebx;
	if(file == NULL) {
		API_RETURN(0);
	}
	else {
		//	memcpy(ecx + ds_base, ebx, eax);
//		console_puts(console, "[%d]%d,%d\n", file, file->size, file->pos);
		ecx = ((file->size - file->pos) > ecx) ? ecx : (file->size - file->pos);
		memcpy(eax + ds_base, file->buf + file->pos, ecx);
		file->pos += ecx;
		API_RETURN(ecx);
	}
	return 0;
}

int32 API_file_size(int32 edi, int32 esi, int32 ebp, int32 esp, int32 ebx, int32 edx, int32 ecx, int32 eax)
{
	FILE_HANDLE* file = ebx;
	if(file == NULL) {
		API_RETURN(0);
	}
	else {
		API_RETURN(file->size);
	}
	return 0;
}
