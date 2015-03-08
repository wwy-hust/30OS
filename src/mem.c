/** @file mem.c
 *  @brief basic memory management.
 *
 *  This files contains basic memory management utils.
 *  
 */
 
#include "mem.h"

static MEM_MANAGEMENT mem;

/** @brief test whether the cpu is exactly I386.
 *  @param NULL
 *  @return TRUE or FALSE
 */
boolean isCpu_i386()
{
	uint32 eflag;
	eflag = io_load_eflags();
	eflag |= EFLAGS_AC_BITS;
	io_store_eflags(eflag);
	eflag = io_load_eflags();
	if((eflag & EFLAGS_AC_BITS) != 0) {
		return FALSE;		//i486 或以上
	}
	return TRUE;
}

/** @brief enable cache of CPU.
 *  @param NULL
 *  @return NULL
 */
void enableCache()
{
	uint32 cr0;
	cr0 = load_cr0();
	cr0 |= CR0_CACHE_DISABLE;
	store_cr0(cr0);
}

/** @brief disable cache of CPU.
 *  @param NULL
 *  @return NULL
 */
void disableCache()
{
	uint32 cr0;
	cr0 = load_cr0();
	cr0 &= ~CR0_CACHE_DISABLE;
	store_cr0(cr0);
}

/** @brief test the existed memory.
 *  used assembly function to do actually test.
 *  @param start start from this address to test.
 *  @param end test end at this address.
 *  @return the existed memory size in byte.
 */
uint32 memtest(uint32 start, uint32 end)
{
	uint32 re = 0;
	boolean isI386;
	isI386 = isCpu_i386();
	if(isI386 == FALSE) {
		disableCache();
	}
	re = memtest_sub(start, end);
	if(isI386 == FALSE) {
		enableCache();
	}
	return re;
}


/** @brief init memory management control block.
 *  Should be called in front of all other component, after mem_init(), you could alloc mem.
 *  @param NULL
 *  @return NULL
 */
void mem_init()
{
	int32 i;
	MEM_INFO empty_mem = {0, 0};
	mem.totalSize = memtest(0x00400000, 0xffffffff);
	mem.totalSize -= 0x00400000;
	mem.freeSum = mem.totalSize;
	mem.usedSum = 0;
	mem.free_cnt = 1;
	mem.used_cnt = 0;
	for(i = 0; i < MEM_INFO_BLK_SIZE; i++) {
		mem.free[i] = empty_mem;
		mem.used[i] = empty_mem;
	}
	mem.free[0].addr = 0x00400000;
	mem.free[0].size = mem.freeSum;
}

/** @brief alloc memory.
 *  @param size size in byte you want to get.
 *  @param addr the start addr which hold size of bytes for you to use.
 *  @return whether the alloc operation successful.
 */
boolean mem_alloc(IN uint32 size, OUT uint32* addr)
{
	uint8 i;
	uint32 tmpAddr;
	if(size > mem.freeSum){		//内存不够分配
		*addr = 0xffffffff;
		return FALSE;
	}
	for(i = 0; i < mem.free_cnt; i++) {
		if(mem.free[i].size >= size) {	//找到一个可以分配的块
			tmpAddr = mem.free[i].addr;
			mem.free[i].addr += size;
			mem.free[i].size -= size;
			mem.used[mem.used_cnt].addr = tmpAddr;
			mem.used[mem.used_cnt].size = size;
			mem.used_cnt++;
			mem.freeSum -= size;
			mem.usedSum += size;
			*addr = tmpAddr;
			return TRUE;
		}
	}
	*addr = 0xffffffff;
	return FALSE;		//没有找到一块可以用于分配的块
}

/** @brief free memory.
 *  @param addr the start addr you want to free, it should be the addr get from mem_alloc().
 *  @param size size in byte you want to return.
 *  @return whether the free operation successful.
 */
boolean mem_free(IN uint32 addr, IN uint32 size)
{
	uint8 i, j;

	for(i = 0; i < mem.used_cnt; i++) {
		if(mem.used[i].addr == addr && mem.used[i].size == size) {
			for(; i < mem.used_cnt - 1; i++) {		//从已用列表中删除该块
				mem.used[i] = mem.used[i + 1];
			}
			mem.used_cnt--;
			
			for(i = 0; i < mem.free_cnt; i++) {	//找寻应该插入的位置
				if(addr < mem.free[i].addr) {
					break;
				}
			}
			if(i == 0) {	//放在块列表首位
				if(mem.free[0].addr == addr + size) {	//可以和空余表首节点合并
					mem.free[0].addr = addr;
					mem.free[0].size += size;
				}
				else {	//不能和空余表首节点合并，只能插入进来
					for(i = mem.free_cnt; i > 0; i--) {
						mem.free[i] = mem.free[i - 1];
					}
					mem.free[0].addr = addr;
					mem.free[0].size = size;
					mem.free_cnt++;
				}
			}
			else {	//放在块列表中间，即[i-1] [i](i - 1 >= 0)
				if(((addr + size) == mem.free[i].addr) &&
					(mem.free[i - 1].addr + mem.free[i - 1].size == addr)) {		//可以把[i-1]和[i]位置的块联合起来
					mem.free[i - 1].size += mem.free[i].size;
					mem.free[i - 1].size += size;
					for(; i < mem.free_cnt - 1; i++) {
						mem.free[i] = mem.free[i + 1];
					}
					mem.free_cnt--;
				}
				else if(addr + size == mem.free[i].addr) {	//可以和i位置的块合并
					mem.free[i].addr = addr;
					mem.free[i].size += size;
				}
				else if(mem.free[i - 1].addr + mem.free[i - 1].size == addr) { //可以和i-1位置的块合并
					mem.free[i - 1].size += size;
				}
				else {	//不能合并，只能插在[i-1]和[i]之间
					for(j = mem.free_cnt; j > i; j--) {
						mem.free[j] = mem.free[j - 1];
					}
					mem.free[i].addr = addr;
					mem.free[i].size = size;
					mem.free_cnt++;
				}
			}
			mem.freeSum += size;
			mem.usedSum -= size;
			return TRUE;
		}
	}
	// 没有找到该块
	return FALSE;
}

/** @brief get free memory size in byte.
 *  @param NULL
 *  @return number free bytes.
 */
uint32	mem_getFreeSum()
{
	return mem.freeSum;
}

/** @brief get used memory size in byte.
 *  @param NULL
 *  @return number used bytes.
 */
uint32	mem_getUsedSum()
{
	return mem.usedSum;
}

/** @brief get total memory size in byte.
 *  @param NULL
 *  @return number of all bytes.
 */
uint32	mem_getTotalSize()
{
	return mem.totalSize;
}



/* not implemented now, mem used by application */
void app_mem_init()
{
}

void* app_mem_alloc(IN uint32 size)
{
}

void app_mem_free(IN void* base)
{
}

