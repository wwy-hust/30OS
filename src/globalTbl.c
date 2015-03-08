/** @file globalTbl.c
 *  @brief store segment descriptor & gate descriptor related function
 *
 *  These features are provided by BIOS & x86.
 *  GDT is global (segment) descriptor table
 *  IDT is interrupt descriptor table
 *  LDT is local (segment) descriptor table
 */
 
#include "globalTbl.h"

/** @brief init gdt and idt
 *  GDT is global (segment) descriptor table
 *  IDT is interrupt descriptor table
 *  @param NULL
 *  @return NULL
 */
void init_gdtidt(void)
{
	struct SEGMENT_DESCRIPTOR *gdt = (struct SEGMENT_DESCRIPTOR *) ADDR_GDT;
	struct GATE_DESCRIPTOR    *idt = (struct GATE_DESCRIPTOR    *) ADDR_IDT;
	int32 i;

	/* GDT初始化 */
	for (i = 0; i <= LIMIT_GDT / 8; i++) {
		set_segmdesc(gdt + i, 0, 0, 0);
	}
	set_segmdesc(gdt + 1, 0xffffffff, 0x00000000, AR_DATA32_RW);
	set_segmdesc(gdt + 2, LIMIT_BOTPAK, ADDR_BOTPAK, AR_CODE32_ER);
	load_gdtr(LIMIT_GDT, ADDR_GDT);

	/* IDT初始化 */
	for (i = 0; i <= LIMIT_IDT / 8; i++) {
		set_gatedesc(idt + i, 0, 0, 0);
	}
	load_idtr(LIMIT_IDT, ADDR_IDT);
	
	//注册中断向量表
	set_gatedesc(idt + 0x0c, (int)asm_interrupt_handler_0x0c, 2 * 8, AR_INTGATE32);
	set_gatedesc(idt + 0x0d, (int)asm_interrupt_handler_0x0d, 2 * 8, AR_INTGATE32);
	set_gatedesc(idt + 0x20, (int)asm_interrupt_handler_0x20, 2 * 8, AR_INTGATE32);
	set_gatedesc(idt + 0x21, (int)asm_interrupt_handler_0x21, 2 * 8, AR_INTGATE32);
	set_gatedesc(idt + 0x27, (int)asm_interrupt_handler_0x27, 2 * 8, AR_INTGATE32);
	set_gatedesc(idt + 0x2c, (int)asm_interrupt_handler_0x2c, 2 * 8, AR_INTGATE32);

	set_gatedesc(idt + 0x40, (int)asm_os_console_api, 2 * 8, AR_INTGATE32 + 0x60);		///< this interrupt vector support api
	return;
}

/** @brief interval functions. set segment descriptor
 *  @param sd segment descriptor
 *  @param limit limit of the descriptor
 *  @param base base address
 *  @param ar access right.
 *  @return NULL
 */
void set_segmdesc(struct SEGMENT_DESCRIPTOR *sd, uint32 limit, int32 base, int32 ar)
{
	if (limit > 0xfffff) {
		ar |= 0x8000; /* G_bit = 1 */
		limit /= 0x1000;
	}
	sd->limit_low    = limit & 0xffff;
	sd->base_low     = base & 0xffff;
	sd->base_mid     = (base >> 16) & 0xff;
	sd->access_right = ar & 0xff;
	sd->limit_high   = ((limit >> 16) & 0x0f) | ((ar >> 8) & 0xf0);
	sd->base_high    = (base >> 24) & 0xff;
	return;
}

/** @brief interval functions. set gate descriptor
 *  @param gd gate descriptor
 *  @param offset offset of interrupt handle
 *  @param selector selector of gate descriptor
 *  @param ar access right.
 *  @return NULL
 */
void set_gatedesc(struct GATE_DESCRIPTOR *gd, int32 offset, int32 selector, int32 ar)
{
	gd->offset_low   = offset & 0xffff;
	gd->selector     = selector;
	gd->dw_count     = (ar >> 8) & 0xff;
	gd->access_right = ar & 0xff;
	gd->offset_high  = (offset >> 16) & 0xffff;
	return;
}
