[FORMAT "WCOFF"]
[INSTRSET "i486p"]
[OPTIMIZE 1]
[OPTION 1]
[BITS 32]
	EXTERN	_api_end
[FILE "crack1.c"]
[SECTION .text]
	GLOBAL	_func_main
_func_main:
	PUSH	EBP
	MOV	DWORD [1058304],0
	MOV	EBP,ESP
	POP	EBP
	JMP	_api_end
