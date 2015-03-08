/** @file multiTask.h
 *  @brief functions related to task, The OS supports Multi Task.
 *
 *  The multiTask include create task, run task, sleep task, stop task, switch task. etc.
 */

#ifndef _MULTI_TASK_H_
#define _MULTI_TASK_H_

#include "string.h"

#include "typedef.h"
#include "tim.h"
#include "globalTbl.h"
#include "mem.h"
#include "dataStruct.h"
#include "fat.h"

/** @addtogroup hal
 *  @{
 */
/// TSS is task status segment, feature provided by BIOS
typedef struct TSS32 {
	int32 backlink, esp0, ss0, esp1, ss1, esp2, ss2, cr3;
	int32 eip, eflags, eax, ecx, edx, ebx, esp, ebp, esi, edi;
	int32 es, cs, ss, ds, fs, gs;
	int ldtr, iomap;
}TSS32;

void tss_init(TSS32* tss, int32 taskId, int32 eip, int32 esp, int32 eflags);	//return task num
/// @}

/** @addtogroup tasks
 *  @brief this module include function to create, start, sleep, stop, switch tasks.
 *  @{
 */

#define SWITCH_TIME		5
#define TASK_LIST_MAX	128

typedef enum TASK_STATUS{
	running = 0, sleep = 1, stopped = 2, blocked = 3
}TASK_STATUS;

/// task structure. 
typedef struct TASK{
	int32				taskId;		///< ID in GDT table. GDT表中的编号.
	int8*				taskName;
	int8				langMode;	///< task supported language mode 0:en; 1:cn
	uint32			sleepTimeCnt_10ms;
	boolean			isUsing;
	uint8				priority;	///< priority between 0 and 255, zero has highest priority.(0优先级最高)
	TASK_STATUS	status;				///< status of one task
	TSS32				tss;
	SEGMENT_DESCRIPTOR	ldt[2];		///< descriptor of the segment used by this task
}TASK;

/// global tasks structure. OS use this structure to manage all the tasks
typedef struct TASK_CTL{
	int32	taskIdBase;			///< base ID used to create task, add every time and can not be decrease. need fix. (无法回收)
	int32	taskNum;			///< number of all tasks
	
	int32	stoppedTasksEnd;	///< number of stopped tasks
	int32	sleepTasksEnd;		///< number of sleep task
	int32	runningTasksEnd;	///< number of running task
	
	int32	nowRunningTaskPos;	///< running task's position in running task list
	
	TASK*	stoppedTasks[TASK_LIST_MAX];	///< stopped task list
	TASK*	sleepTasks[TASK_LIST_MAX];		///< sleep task list
	TASK*	runningTasks[TASK_LIST_MAX];	///< running task list
	TASK	tasks[TASK_LIST_MAX];			///< all tasks store in this list
}TASK_CTL;


boolean task_alloc(IN int8* taskName, IN int32 taskRoutine, IN int32 stackLocation, IN int32 eflags, IN uint8 priority, OUT TASK** task);
boolean task_free(IN TASK* task);

boolean task_run(IN TASK* task);
boolean task_sleep(IN TASK* task, IN int32 sleepTimeCnt_10ms);
boolean task_stop(IN TASK* task);
boolean task_stop_free(IN TASK* task);

void task_sort_running_by_priority();
void task_sort_sleep_by_sleepTime();

void multiTask_init();
void multiTask_switch();
TASK* multiTask_NowRunning();
/// @}


#endif
