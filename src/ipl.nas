; hello-os
; TAB=4

CYLS	EQU		40				; 共要读的柱面数20个柱面，一个柱面包括两面磁头，每面磁头包括18个扇区，每个扇区512字节；一个柱面：2*18*512=18K

		ORG		0x7c00			; 指明程序的装载地址

; 以下内容用于标准FAT12格式的软盘

		JMP		entry
		DB		0x90
		DB		"HELLOIPL"		; 启动区的名称可以是任意的字符串（8字节）
		DW		512						; 每个扇区（sector）的大小（必须为512字节）
		DB		1							; 簇（Cluster）的大小，必须为一个扇区
		DW		1							; FAT的起始地址（一般从第一个扇区开始）
		DB		2							; FAT的个数（必须为2）
		DW		224						; 根目录的大小，一般设置成224项
		DW		2880					; 该磁盘的大小（必须是2880扇区）
		DB		0xf0					; 磁盘的种类（必须是0xF0）
		DW		9							; FAT的长度（必须是9扇区）
		DW		18						; 1个磁道（track）有几个扇区（必须是18）
		DW		2							; 磁头数（必须是2）
		DD		0							; 不使用分区，必须是0
		DD		2880					; 重写一次磁盘大小
		DB		0,0,0x29			; 固定，意义不明
		DD		0xffffffff		; （可能是）卷标号码
		DB		"HELLO-OS   "	; 磁盘的名称（11字节）
		DB		"FAT12   "		; 磁盘格式名称（8字节）
		RESB	18						; 先空出18字节
		
; 核心部分

entry:
		MOV		AX,0			; 初始化
		MOV		SS,AX
		MOV		SP,0x7c00
		MOV		DS,AX
		
		MOV		AX,0x0820
		MOV		ES,AX
		MOV		CH,0			; 柱面0
		MOV		DH,0			; 磁头0
		MOV		CL,2			; 扇区2
		
readloop:
		MOV		SI,0			;	记录读盘失败的次数

retry:							; 读软盘
		MOV		AH,0x02		; AH=0x02 : 读盘
		MOV		AL,1			; 1个扇区
		MOV		BX,0			; ES:BX = 缓冲区地址
		MOV		DL,0x00		; A驱动器
		INT		0x13			; 调用磁盘BIOS
		JNC		next
		ADD		SI,1
		CMP		SI,5
		JAE		error
		MOV		AH,0x00
		MOV		DL,0x00		; 驱动器A
		INT		0x13			; 重置驱动器
		JMP		retry

next:
		MOV		AX,ES			; 把内存地址后移0x200
		ADD		AX,0x0020
		MOV		ES,AX			; 因为没有ADD ES, 0x020的指令，这里绕个弯
		ADD		CL,1
		CMP		CL,18			;
		JBE		readloop	; CL小于18时跳转到 readloop
		MOV		CL,1
		ADD		DH,1
		CMP		DH,2			; 由于软盘只有两面，所以磁头数最多为1
		JB		readloop
		MOV		DH,0			; 重置磁头为0
		ADD		CH,1
		CMP		CH,CYLS		; 读到CYLS个柱面
		JB		readloop
		
success:
		JMP		fin1
		
error:
		MOV		SI,f_msg
		JMP		putloop
		
fin1:
		MOV		[0x0ff0],CH
		JMP		0xC200
		HLT							; 让CPU停止
		JMP		fin1				
		
putloop:
		MOV		AL,[SI]
		ADD		SI,1			
		CMP		AL,0
		JE		fin1
		MOV		AH,0x0e		; 显示一个文字
		MOV		BX,15			; 指定字符颜色
		INT		0x10			; 调用显卡BIOS
		JMP		putloop

f_msg:
		DB		0x0d, 0x0a	; 换行2次
		DB		"load floppy disk error"
		DB		0x0a				; 换行
		DB		0

	
		RESB	0x7dfe-$		;

		DB		0x55, 0xaa
