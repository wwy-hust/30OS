;	nask

[FORMAT "WCOFF"]			; 目标文件的模式
[INSTRSET "i486p"]				; 486偺柦椷傑偱巊偄偨偄偲偄偆婰弎
[BITS 32]							; 制作32为模式用的机械语言

[FILE "funcUtils.nas"]	; 源文件名信息
		; 程序中包含的全局函数名
		GLOBAL	_io_hlt, _io_cli, _io_sti, _io_stihlt
		GLOBAL	_io_in8,  _io_in16,  _io_in32
		GLOBAL	_io_out8, _io_out16, _io_out32
		GLOBAL	_io_load_eflags, _io_store_eflags
		GLOBAL	_load_gdtr, _load_idtr
		GLOBAL	_load_cr0, _store_cr0, _memtest_sub
		GLOBAL	_load_tr, _far_jmp, _far_call, _start_bin
		GLOBAL	_asm_interrupt_handler_0x0c, _asm_interrupt_handler_0x0d, _asm_interrupt_handler_0x20, _asm_interrupt_handler_0x21, _asm_interrupt_handler_0x27, _asm_interrupt_handler_0x2c
		GLOBAL	_asm_os_console_api, _asm_end_app
		
		EXTERN	_interrupt_handler_0x0d, _interrupt_handler_0x0d, _interrupt_handler_0x20, _interrupt_handler_0x21, _interrupt_handler_0x2c, _interrupt_handler_0x27
		EXTERN	_os_console_api
		
[SECTION .text]
_io_hlt:					; void io_hlt(void)
		HLT
		RET
		
_io_cli:	; void io_cli(void);
		CLI
		RET

_io_sti:	; void io_sti(void);
		STI
		RET

_io_stihlt:	; void io_stihlt(void);
		STI
		HLT
		RET

_io_in8:	; int8 io_in8(int32 port);
		MOV		EDX,[ESP+4]		; port
		MOV		EAX,0
		IN		AL,DX
		RET

_io_in16:	; int16 io_in16(int32 port);
		MOV		EDX,[ESP+4]		; port
		MOV		EAX,0
		IN		AX,DX
		RET

_io_in32:	; int32 io_in32(int32 port);
		MOV		EDX,[ESP+4]		; port
		IN		EAX,DX
		RET

_io_out8:	; void io_out8(int32 port, int8 data);
		MOV		EDX,[ESP+4]		; port
		MOV		AL,[ESP+8]		; data
		OUT		DX,AL
		RET

_io_out16:	; void io_out16(int32 port, int16 data);
		MOV		EDX,[ESP+4]		; port
		MOV		EAX,[ESP+8]		; data
		OUT		DX,AX
		RET

_io_out32:	; void io_out32(int32 port, int32 data);
		MOV		EDX,[ESP+4]		; port
		MOV		EAX,[ESP+8]		; data
		OUT		DX,EAX
		RET

_io_load_eflags:	; int io_load_eflags(void);
		PUSHFD		; PUSH EFLAGS 
		POP		EAX
		RET

_io_store_eflags:	; void io_store_eflags(int eflags);
		MOV		EAX,[ESP+4]
		PUSH	EAX
		POPFD		; POP EFLAGS 
		RET
		
_load_gdtr:		; void load_gdtr(int limit, int addr);
		MOV		AX,[ESP+4]		; limit
		MOV		[ESP+6],AX
		LGDT	[ESP+6]
		RET

_load_idtr:		; void load_idtr(int limit, int addr);
		MOV		AX,[ESP+4]		; limit
		MOV		[ESP+6],AX
		LIDT	[ESP+6]
		RET

_asm_interrupt_handler_0x0c:
		STI
		PUSH	ES
		PUSH	DS
		PUSHAD
		MOV		EAX, ESP
		PUSH	EAX
		MOV		AX, SS
		MOV		DS, AX
		MOV		ES, AX
		CALL	_interrupt_handler_0x0c
		CMP		EAX, 0
		JNE		end_app
		POP		EAX
		POPAD
		POP		DS
		POP		ES
		ADD		ESP, 4					; INT 0x0c 中需要这句话
		IRETD

_asm_interrupt_handler_0x0d:
		STI
		PUSH	ES
		PUSH	DS
		PUSHAD
		MOV		EAX, ESP
		PUSH	EAX
		MOV		AX, SS
		MOV		DS, AX
		MOV		ES, AX
		CALL	_interrupt_handler_0x0d
		CMP		EAX, 0
		JNE		end_app
		POP		EAX
		POPAD
		POP		DS
		POP		ES
		ADD		ESP, 4					; INT 0x0D 中需要这句话
		IRETD
		
_asm_interrupt_handler_0x20:
		PUSH	ES
		PUSH	DS
		PUSHAD
		MOV		EAX,ESP
		PUSH	EAX
		MOV		AX,SS
		MOV		DS,AX
		MOV		ES,AX
		CALL	_interrupt_handler_0x20
		POP		EAX
		POPAD
		POP		DS
		POP		ES
		IRETD
		
_asm_interrupt_handler_0x21:
		PUSH		ES
		PUSH		DS
		PUSHAD
		MOV			EAX, ESP
		PUSH		EAX
		MOV			AX, SS
		MOV			DS, AX
		MOV			ES, AX
		CALL		_interrupt_handler_0x21
		POP			EAX
		POPAD
		POP			DS
		POP			ES
		IRETD
				
_asm_interrupt_handler_0x2c:
		PUSH	ES
		PUSH	DS
		PUSHAD
		MOV		EAX,ESP
		PUSH	EAX
		MOV		AX,SS
		MOV		DS,AX
		MOV		ES,AX
		CALL	_interrupt_handler_0x2c
		POP		EAX
		POPAD
		POP		DS
		POP		ES
		IRETD
		
_asm_interrupt_handler_0x27:
		PUSH	ES
		PUSH	DS
		PUSHAD
		MOV		EAX,ESP
		PUSH	EAX
		MOV		AX,SS
		MOV		DS,AX
		MOV		ES,AX
		CALL	_interrupt_handler_0x27
		POP		EAX
		POPAD
		POP		DS
		POP		ES
		IRETD

_load_cr0:
		MOV		EAX, CR0
		RET
		
_store_cr0:
		MOV		EAX, [ESP + 4]
		MOV		CR0, EAX
		RET
		
_memtest_sub:		;; memtest_sub(int start, int end)
		PUSH	EDI
		PUSH	ESI
		PUSH	EBX
		MOV		ESI, 0xaa55aa55
		MOV		EDI, 0x55aa55aa
		MOV		EAX, [ESP + 12 + 4]		;; start
mts_loop:
		MOV		EBX, EAX		
		ADD		EBX, 0xffc
		MOV		EDX, [EBX]		;back up *p to EDX
		MOV		[EBX], ESI
		XOR		DWORD [EBX], 0xffffffff
		CMP		EDI, [EBX]
		JNE		mts_fin									;检查XOR的结果是否正确
		XOR		DWORD	[EBX], 0xffffffff
		CMP		ESI, [EBX]
		JNE		mts_fin									;检查XOR的结果是否正确
		MOV		[EBX], EDX							;将之前的备份恢复
		ADD		EAX, 0x1000	
		CMP		EAX, [ESP + 12 + 8]
		JBE		mts_loop
		POP		EBX
		POP		ESI
		POP		EDI
		RET
mts_fin:
		MOV		[EBX], EDX
		POP		EBX
		POP		ESI
		POP		EDI
		RET
		
_load_tr:		; void load_tr(int tr);
		LTR		[ESP + 4]
		RET
		
_far_jmp:		; void far_jmp(int eip, int cs);
		JMP		FAR [ESP + 4]
		RET
		
_far_call:	;	void far_call(int eip, int cs);
		CALL	FAR [ESP + 4]
		RET
		
_asm_os_console_api: 
		STI
		PUSH	DS
		PUSH	ES
		PUSHAD								;	将值存入应用程序的栈中,这次把FS,GS的设置给省了
		PUSHAD								; 用于向 os_console_api 传值
		MOV		AX, SS
		MOV		DS, AX
		MOV		ES, AX
		CALL	_os_console_api
		CMP		EAX, 0
		JNE		end_app
		ADD		ESP, 32
		POPAD
		POP		ES
		POP		DS
		IRETD								;这个命令会自动执行STI
end_app:
		MOV		ESP, [EAX]
		POPAD
		RET		
		
_start_bin:						;void start_bin(int eip, int cs, int esp, int ds, int* tss_esp0);
		PUSHAD
		MOV		EAX, [ESP + 36]	;应用程序用EIP
		MOV		ECX, [ESP + 40]	;应用程序用CS
		MOV		EDX, [ESP + 44]	;应用程序用ESP
		MOV		EBX, [ESP + 48]	;应用程序用DS/SS
		MOV		EBP, [ESP + 52] ;tss.esp0
		MOV		[EBP], ESP			;保存系统的ESP
		MOV		[EBP + 4], SS		;保存系统的SS
		MOV		ES, BX
		MOV		DS, BX
		MOV		FS, BX
		MOV		GS, BX
		;调整栈，用RETF跳转到应用程序
		OR		ECX, 3			;;CS
		OR		EBX, 3			;;DS/SS
		PUSH	EBX
		PUSH	EDX					;;ESP
		PUSH	ECX					;;CS
		PUSH	EAX					;;EIP
		RETF
		;应用程序结束后不会回到这里
		
_asm_end_app:
; EAX 为 tss.esp0 的地址
		MOV		ESP, [EAX]
		MOV		DWORD [EAX + 4], 0
		POPAD
		RET
		