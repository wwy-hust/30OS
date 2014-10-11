
#ifndef _MULTI_TASK_H_
#define _MULTI_TASK_H_

#include "string.h"

#include "typedef.h"
#include "tim.h"
#include "globalTbl.h"
#include "mem.h"
#include "dataStruct.h"
#include "fat.h"

#define SWITCH_TIME		5
#define TASK_LIST_MAX	128

typedef enum TASK_STATUS{
	running = 0, sleep = 1, stopped = 2, blocked = 3
}TASK_STATUS;

typedef struct TSS32 {
	int32 backlink, esp0, ss0, esp1, ss1, esp2, ss2, cr3;
	int32 eip, eflags, eax, ecx, edx, ebx, esp, ebp, esi, edi;
	int32 es, cs, ss, ds, fs, gs;
	int ldtr, iomap;
}TSS32;

typedef struct TASK{
	int32				taskId;		//GDT���еı��
	int8*				taskName;
	int8				langMode;	//0:en; 1:cn
	uint32			sleepTimeCnt_10ms;
	boolean			isUsing;
	uint8				priority;	// 0 - 255, 0���ȼ����
	TASK_STATUS	status;
	TSS32				tss;
	SEGMENT_DESCRIPTOR	ldt[2];
}TASK;

typedef struct TASK_CTL{
	int32	taskIdBase;			//�޷�����
	int32	taskNum;
	
	int32	stoppedTasksEnd;
	int32	sleepTasksEnd;
	int32	runningTasksEnd;
	
	int32	nowRunningTaskPos;
	
	TASK*	stoppedTasks[TASK_LIST_MAX];
	TASK*	sleepTasks[TASK_LIST_MAX];
	TASK*	runningTasks[TASK_LIST_MAX];
	TASK	tasks[TASK_LIST_MAX];
}TASK_CTL;


boolean task_alloc(IN int8* taskName, IN int32 taskRoutine, IN int32 stackLocation, IN int32 eflags, IN uint8 priority, OUT TASK** task);
boolean task_free(IN TASK* task);

boolean task_run(IN TASK* task);
boolean task_sleep(IN TASK* task, IN int32 sleepTimeCnt_10ms);
boolean task_stop(IN TASK* task);
boolean task_stop_free(IN TASK* task);

void task_sort_running_by_priority();
void task_sort_sleep_by_sleepTime();

void tss_init(TSS32* tss, int32 taskId, int32 eip, int32 esp, int32 eflags);	//return task num

void multiTask_init();
void multiTask_switch();
TASK* multiTask_NowRunning();


#endif