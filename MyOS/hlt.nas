[INSTRSET "i486p"]
[BITS 32]

;	MOV EDX, 1
;	MOV AX, 'H'
;	INT 0x40
;	MOV EDX, 1
;	MOV AX, 'E'
;	INT 0x40
;	MOV EDX, 1
;	MOV AX, 'H'
;	INT 0x40
;	MOV EDX, 1
;	MOV AX, 'E'
;	INT 0x40
;	RETF

	MOV EDX, 2
	MOV	EBX, msg
	INT 0x40
	MOV	EDX, 4
	INT 0x40
msg:
	DB	"Hello, World!", 0x0A, 0