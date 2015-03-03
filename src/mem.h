
#ifndef _MEM_H_
#define _MEM_H_

#include "funcUtils.h"

#define EFLAGS_AC_BITS			0x00040000
#define CR0_CACHE_DISABLE		0x60000000

#define MEM_INFO_BLK_SIZE		0x00000100

typedef struct MEM_INFO{
	uint32 addr;
	uint32 size;
}MEM_INFO;

typedef struct MEM_MANAGEMENT{
	uint32 totalSize;
	uint32 freeSum;
	uint32 usedSum;
	uint8 free_cnt;
	uint8 used_cnt;
	MEM_INFO free[MEM_INFO_BLK_SIZE];
	MEM_INFO used[MEM_INFO_BLK_SIZE];
}MEM_MANAGEMENT;


boolean isCpu_i386();
void enableCache();
void disableCache();
uint32 memtest(uint32 start, uint32 end);		//返回当前内存数目

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

#endif
