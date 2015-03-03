; haribote-os boot asm
; TAB=4
[INSTRSET "i486p"]

BOTPAK	EQU		0x00280000		; bootpack的拷贝目的地址
DSKCAC	EQU		0x00100000		; IPL的拷贝目的地址 IPL的大小不超过512Byte
DSKCAC0	EQU		0x00008000		; IPL从软盘中读出数据的源拷贝地址 0x00008200


; BOOT_INFO
CYLS	EQU		0x0ff0
LEDS	EQU		0x0ff1			; 键盘上的各个指示灯
VMODE	EQU		0x0ff2			; 显示模式
SCRNX	EQU		0x0ff4
SCRNY	EQU		0x0ff6
VRAM	EQU		0x0ff8			; 图线开始缓冲区

;;;; resolution BX = 0x4000 + below value
;	0x100			640x400 8bit
; 0x101			640x480 8bit
; 0x103			800x600 8bit
; 0x105			1024x768 8bit
; 0x107			1280x1024 8bit
VBEMODE	EQU	0x105

	ORG		0xC200

	MOV		AX, 0x9000
	MOV		ES, AX
	MOV		DI, 0
	MOV		AX, 0x4f00
	INT		0x10
	CMP		AX, 0x004f		; 如果有VBE的话，AX会变为0x004f
	JNE		scrn320
	MOV		AX, [ES:DI + 4]	; 检查VBE的版本
	CMP		AX, 0x0200
	JB		scrn320				; if (AX < 0x0200) goto scrn320
	MOV		CX, VBEMODE
	MOV		AX, 0x4f01
	INT		0x10
	CMP		AX, 0x004f
	JNE		scrn320
	
	CMP		BYTE[ES:DI + 0x19], 8			; 颜色数，应该为8
	JNE		scrn320
	CMP		BYTE[ES:DI + 0x1b], 4			; 颜色的指定方法，必须为4，4为调色板模式
	JNE		scrn320
	MOV		AX, [ES:DI + 0x00]				;	模式属性，bit7必须为1
	AND		AX, 0x0080
	JZ		scrn320		
	
	MOV		BX, VBEMODE + 0x4000
	MOV		AX, 0x4f02
	INT		0x10
	MOV		BYTE[VMODE],8
	MOV		AX, [ES:DI + 0x12]	; X 的分辨率
	MOV		[SCRNX],AX
	MOV		AX, [ES:DI + 0x14]	; Y 的分辨率
	MOV		[SCRNY],AX
	MOV		EAX, [ES:DI + 0x28]
	MOV		DWORD[VRAM],EAX
	JMP		get_led

scrn320:
	MOV		AL,0x13			; VGA 显卡，320x200x18位彩色
	MOV		AH,0x00
	INT		0x10
	MOV		BYTE[VMODE],8
	MOV		WORD[SCRNX],320
	MOV		WORD[SCRNY],200
	MOV		DWORD[VRAM],0x000a0000

get_led:	
; 取得键盘上各种LED指示灯
	MOV		AH,0x02
	INT		0x16
	MOV		[LEDS],AL
	
;;;;;;;;;; COPY ;;;;;;;;;;;
	
		MOV		AL,0xff
		OUT		0x21,AL	; 相当于 io_out(PIC0_IMR, 0xff), 禁止主PIC的全部中断
		NOP						; 如果连续执行OUT指令，有些机器会无法正常运行
		OUT		0xa1,AL ; 相当于 io_out(PIC1_IMR, 0xff), 禁止从PIC的全部中断
	
		CLI						; 禁止CPU级别的中断

		CALL	waitkbdout
		MOV		AL,0xd1
		OUT		0x64,AL
		CALL	waitkbdout
		MOV		AL,0xdf			; enable A20，为了使1M以上的内存可以使用
		OUT		0x60,AL
		CALL	waitkbdout

		LGDT	[GDTR0]			; 巄掕GDT傪愝掕
		MOV		EAX,CR0
		AND		EAX,0x7fffffff	; bit31傪0偵偡傞乮儁乕僕儞僌嬛巭偺偨傔乯
		OR		EAX,0x00000001	; bit0傪1偵偡傞乮僾儘僥僋僩儌乕僪堏峴偺偨傔乯
		MOV		CR0,EAX
		JMP		pipelineflush
pipelineflush:
		MOV		AX,1*8			;  撉傒彂偒壜擻僙僌儊儞僩32bit
		MOV		DS,AX
		MOV		ES,AX
		MOV		FS,AX
		MOV		GS,AX
		MOV		SS,AX

; bootpack的拷贝

		MOV		ESI,bootpack	; 转送源
		MOV		EDI,BOTPAK		; 转送目的地
		MOV		ECX,512*1024/4
		CALL	memcpy

; 磁盘数据最终转送到它本来的位置去

; 首先从启动扇区开始，IPL

		MOV		ESI,0x7c00		; 转送源
		MOV		EDI,DSKCAC		; 转送目的地
		MOV		ECX,512/4
		CALL	memcpy

; IPL从软盘中读出的数据的拷贝

		MOV		ESI,DSKCAC0+512	; 转送源
		MOV		EDI,DSKCAC+512	; 转送目的地
		MOV		ECX,0
		MOV		CL,BYTE [CYLS]
		IMUL	ECX,512*18*2/4	; 从柱面数转换为字节数
		SUB		ECX,512/4				; 减去IPL
		CALL	memcpy

; asmhead偱偟側偗傟偽偄偗側偄偙偲偼慡晹偟廔傢偭偨偺偱丄
;	偁偲偼bootpack偵擟偣傞

; bootpack的启动

		MOV		EBX,BOTPAK
		MOV		ECX,[EBX+16]
		ADD		ECX,3			; ECX += 3;
		SHR		ECX,2			; ECX /= 4;	ECX	>> 2
		JZ		skip			; 没有要转送的东西时
		MOV		ESI,[EBX+20]	; 转送源
		ADD		ESI,EBX
		MOV		EDI,[EBX+12]	; 转送目的地
		CALL	memcpy
skip:
		MOV		ESP,[EBX+12]	; 栈初始地址
		JMP		DWORD 2*8:0x0000001b

waitkbdout:
		IN		AL,0x64
		AND		AL,0x02
		JNZ		waitkbdout		; AND偺寢壥偑0偱側偗傟偽waitkbdout傊
		RET

memcpy:
		MOV		EAX,[ESI]
		ADD		ESI,4
		MOV		[EDI],EAX
		ADD		EDI,4
		SUB		ECX,1
		JNZ		memcpy			; 堷偒嶼偟偨寢壥偑0偱側偗傟偽memcpy傊
		RET
; memcpy偼傾僪儗僗僒僀僘僾儕僼傿僋僗傪擖傟朰傟側偗傟偽丄僗僩儕儞僌柦椷偱傕彂偗傞

		ALIGNB	16	;;添加零直到16字节对齐
GDT0:
		RESB	8				; NULL sector
		DW		0xffff,0x0000,0x9200,0x00cf	; 可以读写的段 32bit
		DW		0xffff,0x0000,0x9a28,0x0047	; 可以执行的段 32bit bootpac 用

		DW		0
GDTR0:
		DW		8*3-1
		DD		GDT0

		ALIGNB	16
bootpack:
