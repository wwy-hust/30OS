/** @file mem.h
 *  @brief basic memory management.
 *
 *  This files contains basic memory management utils.
 *  
 */
#ifndef _MEM_H_
#define _MEM_H_

#include "funcUtils.h"

/** @addtogroup mem
 *  @brief this module provide basic function related to memory management.
 *  @{
 */
#define MEM_INFO_BLK_SIZE		0x00000100

/** @brief memory management structure */
typedef struct MEM_INFO{
	uint32 addr;	///< start addr of memory block
	uint32 size;	///< size of memory block
}MEM_INFO;

/** @brief memory management structure */
typedef struct MEM_MANAGEMENT{
	uint32 totalSize;		///< total memory size
	uint32 freeSum;
	uint32 usedSum;
	uint8 free_cnt;			///< cnt of free MEM_INFO in free[]
	uint8 used_cnt;			///< cnt of used MEM_INFO in used[]
	MEM_INFO free[MEM_INFO_BLK_SIZE];	///< array to store free memory block
	MEM_INFO used[MEM_INFO_BLK_SIZE];	///< array to store used memory block
}MEM_MANAGEMENT;

// ***** mem management *****
void mem_init();
boolean mem_alloc(IN uint32 size, OUT uint32* addr);
boolean mem_free(IN uint32 addr, IN uint32 size);
uint32	mem_getFreeSum();
uint32	mem_getUsedSum();
uint32	mem_getTotalSize();

void app_mem_init();
void* app_mem_alloc(IN uint32 size);
void app_mem_free(IN void* base);
/// @}

/// @addtogroup hal
/// @{
#define EFLAGS_AC_BITS			0x00040000
#define CR0_CACHE_DISABLE		0x60000000

boolean isCpu_i386();
void enableCache();
void disableCache();
uint32 memtest(uint32 start, uint32 end);		//返回当前内存数目
/// @}

#endif
