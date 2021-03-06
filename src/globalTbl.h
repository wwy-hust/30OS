/** @file globalTbl.h
 *  @brief store segment descriptor & gate descriptor related function
 *
 *  These features are provided by BIOS & x86.
 */
 
#ifndef _GLOBAL_TBL_H_
#define _GLOBAL_TBL_H_

#include "typedef.h"
#include "funcUtils.h"
#include "interrupt.h"
#include "funcUtils.h"

/** @addtogroup hal
 *  @brief used to abstract hardware, you could use BIOS function by call wraper function in C.
 *  @{
 */

/// @addtogroup access_right
/// @brief used as param when ar needed
/// @{
#define AR_DATA32_RW		0x4092
#define AR_CODE32_ER		0x409a
#define AR_INTGATE32		0x008e
#define AR_TSS32			0x0089
#define AR_LDT				0x0082
/// @}

/// Segment Descriptor
typedef struct SEGMENT_DESCRIPTOR {
	short limit_low, base_low;
	char base_mid, access_right;
	char limit_high, base_high;
}SEGMENT_DESCRIPTOR;

/// Gate Descriptor
typedef struct GATE_DESCRIPTOR {
	short offset_low, selector;
	char dw_count, access_right;
	short offset_high;
}GATE_DESCRIPTOR;


void init_gdtidt(void);
void set_segmdesc(struct SEGMENT_DESCRIPTOR *sd, uint32 limit, int32 base, int32 ar);
void set_gatedesc(struct GATE_DESCRIPTOR *gd, int32 offset, int32 selector, int32 ar);
/// @}

#endif
