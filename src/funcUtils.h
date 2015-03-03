
#ifndef _FUNC_UTILS_H_
#define _FUNC_UTILS_H_

#include "typedef.h"

#define ADDR_BOOTINFO		0x00000ff0

#define ADDR_IDT				0x0026f800
#define LIMIT_IDT				0x000007ff

#define ADDR_GDT				0x00270000
#define LIMIT_GDT				0x0000ffff

#define ADDR_BOTPAK			0x00280000
#define LIMIT_BOTPAK		0x0007ffff


void io_hlt(void);
void io_cli(void);		//屏蔽所有中断
void io_sti(void);		//打开所有中断
void io_stihlt(void);

int8 io_in8(int32 port);
int16 io_in16(int32 port);
int32 io_in32(int32 port);
void io_out8(int32 port, int8 data);
void io_out16(int32 port, int16 data);
void io_out32(int32 port, int32 data);

int32 io_load_eflags(void);
void io_store_eflags(int32 eflags);
void io_cli(void);

void load_gdtr(int32 limit, int32 addr);
void load_idtr(int32 limit, int32 addr);

void asm_interrupt_handler_0x0c(void);
void asm_interrupt_handler_0x0d(void);
void asm_interrupt_handler_0x20(void);
void asm_interrupt_handler_0x21(void);
void asm_interrupt_handler_0x27(void);
void asm_interrupt_handler_0x2c(void);

// emerge in mem management
int32 load_cr0();
void store_cr0(int32 val);
int32 memtest_sub(int32 start, int32 end);

// emerge in multitask
void load_tr(int tr);
void far_jmp(int eip, int cs);
void far_call(int eip, int cs);
void start_bin(int eip, int cs, int esp, int ds, int* tss_esp0);

// char display API
void asm_end_app(void);
void asm_os_console_api(void);

#endif
