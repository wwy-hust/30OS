
[FORMAT "WCOFF"]
[INSTRSET "i486p"]
[BITS 32]
[FILE "app_funcUtils.nas"]

		GLOBAL	_api_putchar, _api_puts, _api_put_n_char, _api_end
		GLOBAL	_api_create_wnd, _api_wnd_fillbox, _api_wnd_puts, _api_wnd_point, _api_wnd_line, _api_wnd_close, _api_wnd_refresh
		GLOBAL	_api_wnd_get_keyboard, _api_memcpy, _api_timer_alloc, _api_timer_settime, _api_timer_isTimeout, _api_timer_free
		GLOBAL	_api_beep, _api_wnd_setContentBuf
		
		GLOBAL	_api_open, _api_close, _api_seek, _api_read, _api_file_size
		
[SECTION	.text]
_api_putchar:				;void api_putchar(int8 data);
		MOV EDX, 1
		MOV	AL, [ESP + 4]
		INT	0x40
		RET

_api_puts:					;void api_puts(int8* pData);
		PUSH EBX
		MOV EDX, 2
		MOV	EBX, [ESP + 8]
		INT	0x40
		POP	EBX
		RET

_api_put_n_char:		;void api_put_n_char(int8* pData, int32 len)
		PUSH	EBX
		MOV	EDX, 3
		MOV	EBX, [ESP + 8]
		MOV	ECX, [ESP + 12]
		INT 0x40
		POP	EBX
		RET

_api_end:						;void api_end(void);
		MOV	EDX, 4
		INT	0x40
		
_api_create_wnd:		;WND* api_create_wnd(int32 width, int32 height, int8* title, uint8* icon);
		PUSH	ESI
		PUSH	EDI
		MOV		EDX, 5
		MOV		ESI, [ESP + 12]		;width
		MOV		EDI, [ESP + 16]		;height
		MOV		ECX, [ESP + 20]		;title
		MOV		EAX, [ESP + 24]		;icon
		INT		0x40
		POP		EDI
		POP		ESI
		RET

_api_wnd_close:			;boolean api_wnd_close(WND* wnd);
		PUSH	EBX
		MOV		EDX, 6
		MOV		EBX, [ESP + 8]
		INT		0x40
		POP		EBX
		RET
	
_api_wnd_fillbox:		;boolean api_wnd_fillbox(WND* wnd, int32 width, int32 height, int32 topLeftX, int32 topLeftY, uint8 color);
		PUSH	EBX
		PUSH	ESI
		PUSH	EDI
		PUSH	EBP
		MOV		EDX, 7
		MOV		EBX, [ESP + 20]
		MOV		EAX, [ESP + 24]	;width
		MOV		ECX, [ESP + 28]	;height
		MOV		ESI, [ESP + 32]	;topLeftX
		MOV		EDI, [ESP + 36]	;topLeftY
		MOV		EBP, [ESP + 40]	;color
		INT		0x40
		POP		EBP
		POP		EDI
		POP		ESI
		POP		EBX
		RET

_api_wnd_puts:			;boolean api_wnd_puts(WND* wnd, int8* str, int32 topLeftX, int32 topLeftY, uint8 color);
		PUSH	EBX
		PUSH	ESI
		PUSH	EDI
		MOV		EDX, 8
		MOV		EBX, [ESP + 16]	;wnd
		MOV		EAX, [ESP + 20]	;str
		MOV		ESI, [ESP + 24]	;topLeftX
		MOV		EDI, [ESP + 28]	;topLeftY
		MOV		ECX, [ESP + 32]	;color
		INT		0x40
		POP		EDI
		POP		ESI
		POP		EBX
		RET
		
_api_wnd_point:			;boolean api_wnd_point(WND* wnd, int32 x, int32 y, uint8 color);
		PUSH	ESI
		PUSH	EDI
		PUSH	EBX
		MOV		EDX, 9
		MOV		EBX, [ESP + 16]	;WND
		MOV		ESI, [ESP + 20]	;x
		MOV		EDI, [ESP + 24]	;y
		MOV		EAX, [ESP + 28]	;color
		INT		0x40
		POP		EBX
		POP		EDI
		POP		ESI
		RET

_api_wnd_line:			;boolean api_wnd_line(WND* wnd, int32 x0, int32 y0, int32 x1, int32 y1, uint8 color);
		PUSH	ESI
		PUSH	EDI
		PUSH	EBX
		PUSH	EBP
		MOV		EDX, 10
		MOV		EBX, [ESP + 20]	;WND
		MOV		ESI, [ESP + 24]	;x0
		MOV		EDI, [ESP + 28] ;y0
		MOV		EAX, [ESP + 32]	;x1
		MOV		ECX, [ESP + 36]	;y1
		MOV		EBP, [ESP + 40]	;color
		INT		0x40
		POP		EBP
		POP		EBX
		POP		EDI
		POP		ESI
		RET

_api_wnd_setContentBuf:		;boolean api_wnd_setContentBuf(WND* wnd, uint8* localBuffer);
		PUSH	EBX
		MOV		EDX, 11
		MOV		EBX, [ESP + 8]
		MOV		EAX, [ESP + 12]
		INT		0x40
		POP		EBX
		RET

_api_wnd_get_keyboard:		;int8 api_wnd_get_keyboard(WND* wnd, int32 mode);
		PUSH	EBX
		MOV		EDX, 12
		MOV		EBX, [ESP + 8]	;wnd
		MOV		EAX, [ESP + 12]	; mode
		INT		0x40
		POP		EBX
		RET

_api_wnd_refresh:					;boolean api_wnd_refresh(WND* wnd);
		PUSH	EBX
		MOV		EBX, [ESP + 8]
		MOV		EDX, 13
		INT		0x40
		POP		EBX
		RET

_api_memcpy:					;void api_memcpy(void* srcBase, int32 size, void* dstBase);
		PUSH	EBX
		MOV		EDX, 14
		MOV		EBX, [ESP + 8]		;srcBase
		MOV		EAX, [ESP + 12]		;size
		MOV		ECX, [ESP + 16]		;dstBase
		INT		0x40
		POP		EBX
		RET

_api_timer_alloc:				;TIM* api_timer_alloc(uint32 timeout);
		MOV		EAX, [ESP + 4]
		MOV		EDX, 15
		INT		0x40
		RET

_api_timer_free:				;boolean api_timer_free(TIM* timer);
		PUSH	EBX
		MOV		EBX, [ESP + 8]
		MOV		EDX, 16
		INT		0x40
		POP		EBX
		RET

_api_timer_settime:			;boolean api_timer_settime(TIM* timer, uint32 timeout);
		PUSH	EBX
		MOV		EBX, [ESP + 8]
		MOV		EAX, [ESP + 12]
		MOV		EDX, 17
		INT		0x40
		POP		EBX
		RET

_api_timer_isTimeout:		;boolean api_timer_isTimeout(TIM* timer);
		PUSH	EBX
		MOV		EBX, [ESP + 8]
		MOV		EDX, 18
		INT		0x40
		POP		EBX
		RET

_api_beep:						;void api_beep(int32 tone);
		MOV		EAX, [ESP + 4]
		MOV		EDX, 19
		INT		0x40
		RET

_api_open:				;FILE_HANDLE* api_open(uint8* fileName);
		PUSH	EBX
		MOV		EDX, 20
		MOV		EBX, [ESP + 8]	;fileName
		INT		0x40
		POP		EBX
		RET

_api_close:				;boolean api_close(FILE_HANDLE* file);
		MOV		EDX, 21
		MOV		EAX, [ESP + 4]	;file handle
		INT		0x40
		RET

_api_seek:				;boolean api_seek(FILE_HANDLE* file, uint8 mode, uint32 pos);
		PUSH	EBX
		MOV		EDX, 22
		MOV		EBX, [ESP + 8]	;file handle
		MOV		EAX, [ESP + 12]	;mode
		MOV		ECX, [ESP + 16]	;pos
		INT		0x40
		POP		EBX
		RET

_api_read:				;int32 api_read(FILE_HANDLE* file, uint8* buf, int32 bufMaxLen);
		PUSH	EBX
		MOV		EDX, 23
		MOV		EBX, [ESP + 8]	;file handle
		MOV		EAX, [ESP + 12]	;buf
		MOV		ECX, [ESP + 16]	;bufMaxLen
		INT		0x40
		POP		EBX
		RET


_api_file_size:		;int32 api_file_size(FILE_HANDLE* file);
		MOV		EDX, 24
		MOV		EAX, [ESP + 4]	;file handle
		INT		0x40
		RET

