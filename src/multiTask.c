/** @file multiTask.c
 *  @brief functions related to task, The OS supports Multi Task.
 *
 *  The multiTask include create task, run task, sleep task, stop task, switch task. etc.
 */

#include "multiTask.h"

TASK_CTL taskCtl;		///< global structure used to store all information related to tasks
TIM* task_timer = NULL;
int32 mt_tr;

extern FAT_CTL fatCtl;

/** @brief create task.
 *  This function alloc a task control block on taskCtl, and return it using OUT TASK** task.
 *  You could use task in future use, for run, stop, sleep.
 *  @param taskName name of this task
 *  @param taskRoutine the start address of the task. It should be a casted function pointer.
 *  @param stackLocation stack start address for the task.
 *  @param eflags eflags used internal
 *  @param priority task running priority. less value means higher priority.
 *  @param task the task handle output for future use.
 *  @return whether the function executed successful.
 */
boolean task_alloc(IN int8* taskName, IN int32 taskRoutine, IN int32 stackLocation, IN int32 eflags, IN uint8 priority, OUT TASK** task)
{
	struct SEGMENT_DESCRIPTOR *gdt = (struct SEGMENT_DESCRIPTOR *) ADDR_GDT;
	int32 taskNameLen = 0;
	uint32 tmpAddr;
	if(taskCtl.taskNum >= TASK_LIST_MAX)
		return FALSE;
	taskNameLen = strlen(taskName);
	if(mem_alloc(taskNameLen + 1, &(tmpAddr)) == FALSE) {
		return FALSE;
	}
	
	for(;taskCtl.tasks[taskCtl.taskNum].isUsing == TRUE; taskCtl.taskNum = taskCtl.taskNum + 1 % TASK_LIST_MAX);

	(*task) = &taskCtl.tasks[taskCtl.taskNum++];
	(*task)->langMode = (fatCtl.fontFile == NULL) ? 0 : 1;
	(*task)->isUsing = TRUE;
	(*task)->sleepTimeCnt_10ms = 0;
	(*task)->taskId = taskCtl.taskIdBase++;
	(*task)->priority = priority;
	(*task)->status = stopped;
	(*task)->taskName = (int8*)tmpAddr;
	strcpy((*task)->taskName, taskName);
	(*task)->taskName[taskNameLen] = 0;
	tss_init(&((*task)->tss), (*task)->taskId, taskRoutine, stackLocation, eflags);
	set_segmdesc(gdt + (*task)->taskId + TASK_LIST_MAX, 15, (int) (*task)->ldt, AR_LDT);
	
	taskCtl.stoppedTasks[taskCtl.stoppedTasksEnd++] = (*task);
	return TRUE;
}

/** @brief delete task.
 *  This function find the specific task in stopTaskList then free its control block.
 *  @param task the task handle you want to free.
 *  @return whether the function executed successful.
 */
boolean task_free(IN TASK* task)
{
	int32 i, j;
	if(task == NULL)
		return FALSE;
	for(i = 0; i < taskCtl.stoppedTasksEnd; i++) {	//在停止任务队列中查找
		if(taskCtl.stoppedTasks[i] == task) { 		//已找到该任务
			// 从停止的任务队列中将该任务删去
			for(j = i + 1; j < taskCtl.stoppedTasksEnd; j++) {
				taskCtl.stoppedTasks[j - 1] = taskCtl.stoppedTasks[j];
			}
			taskCtl.stoppedTasksEnd--;
			taskCtl.taskNum--;
			//释放taskName的内存
			j = strlen(task->taskName);
			mem_free((uint32)(task->taskName), j);
			// 将该任务放入运行的队列
			task->isUsing = FALSE;
			return TRUE;
		}
	}
	return FALSE;
}

/** @brief start one task.
 *  This function start the specific task.
 *  @param task the task handle you want to use.
 *  @return whether the function executed successful.
 */
boolean task_run(IN TASK* task)
{
	int32 i, j;
	if(task == NULL)
		return FALSE;
	for(i = 0; i < taskCtl.stoppedTasksEnd; i++) {	//在停止任务队列中查找
		if(taskCtl.stoppedTasks[i] == task) { 		//已找到该任务
			io_cli();
			// 从停止的任务队列中将该任务删去
			for(j = i + 1; j < taskCtl.stoppedTasksEnd; j++) {
				taskCtl.stoppedTasks[j - 1] = taskCtl.stoppedTasks[j];
			}
			taskCtl.stoppedTasksEnd--;
			taskCtl.stoppedTasks[0] = (taskCtl.stoppedTasksEnd == 0) ? NULL : taskCtl.stoppedTasks[0];
			
			// 将该任务放入运行的队列
			taskCtl.runningTasks[taskCtl.runningTasksEnd++] = task;
			task->status = running;
			task_sort_running_by_priority();
			io_sti();
			return TRUE;
		}
	}
	for(i = 0; i < taskCtl.sleepTasksEnd; i++) {	//在睡眠任务队列中查找
		if(taskCtl.sleepTasks[i] == task) { 		//已找到该任务
			io_cli();
			// 从停止的任务队列中将该任务删去
			for(j = i + 1; j < taskCtl.sleepTasksEnd; j++) {
				taskCtl.sleepTasks[j - 1] = taskCtl.sleepTasks[j];
			}
			taskCtl.sleepTasksEnd--;
			taskCtl.stoppedTasks[0] = (taskCtl.stoppedTasksEnd == 0) ? NULL : taskCtl.stoppedTasks[0];
			
			// 将该任务放入运行的队列
			taskCtl.runningTasks[taskCtl.runningTasksEnd++] = task;
			task->status = running;
			task_sort_running_by_priority();
			io_sti();
			return TRUE;
		}
	}
	return FALSE;		//未找到
}

/** @brief sleep one task.
 *  This function sleep the specific task with a specific time interval.
 *  @param task the task handle you want to use.
 *  @param sleepTimeCnt_10ms the sleep interval of the task.
 *  @return whether the function executed successful.
 */
boolean task_sleep(IN TASK* task, IN int32 sleepTimeCnt_10ms)
{
	int32 i, j;

	if(task == NULL || task->status != running)
		return FALSE;
	
	if(sleepTimeCnt_10ms == 0) {		//任务的队列无需改变，只需要切换任务即可
		multiTask_switch();
		return TRUE;
	}
	
	for(i = 0; i < taskCtl.runningTasksEnd; i++) {
		if(taskCtl.runningTasks[i] == task) {
			for(j = i + 1; j < taskCtl.runningTasksEnd; j++) {
				taskCtl.runningTasks[j - 1] = taskCtl.runningTasks[j];
			}
			taskCtl.runningTasksEnd--;
			
			taskCtl.sleepTasks[taskCtl.sleepTasksEnd++] = task;
			task->status = sleep;
			task->sleepTimeCnt_10ms = timer_getcount() + sleepTimeCnt_10ms;
			task_sort_sleep_by_sleepTime();
			multiTask_switch();
			return TRUE;
		}
	}
	return FALSE;
}

/** @brief stop one task.
 *  This function stop the specific task.
 *  @param task the task handle you want to use.
 *  @return whether the function executed successful.
 */
boolean task_stop(IN TASK* task)
{
	int32 i, j;
	if(task == NULL)
		return FALSE;
	
	for(i = 0; i < taskCtl.runningTasksEnd; i++) {
		if(taskCtl.runningTasks[i] == task) {
			for(j = i + 1; j < taskCtl.runningTasksEnd; j++) {
				taskCtl.runningTasks[j - 1] = taskCtl.runningTasks[j];
			}
			taskCtl.runningTasksEnd--;
			taskCtl.stoppedTasks[taskCtl.stoppedTasksEnd++] = task;
			task->status = stopped;
			multiTask_switch();
			return TRUE;
		}
	}
	for(i = 0; i < taskCtl.sleepTasksEnd; i++) {
		if(taskCtl.sleepTasks[i] == task) {
			for(j = i + 1; j < taskCtl.sleepTasksEnd; j++) {
				taskCtl.sleepTasks[j - 1] = taskCtl.sleepTasks[j];
			}
			taskCtl.sleepTasksEnd--;
			taskCtl.stoppedTasks[taskCtl.stoppedTasksEnd++] = task;
			task->status = stopped;
			multiTask_switch();
			return TRUE;
		}
	}
	return FALSE;
}

/** @brief stop and delete one task.
 *  This function stop the specific task and delete it.
 *  @param task the task handle you want to use.
 *  @return whether the function executed successful.
 */
boolean task_stop_free(IN TASK* task)
{
	int32 i, j;
	if(task == NULL)
		return FALSE;
	
	for(i = 0; i < taskCtl.runningTasksEnd; i++) {
		if(taskCtl.runningTasks[i] == task) {
			for(j = i + 1; j < taskCtl.runningTasksEnd; j++) {
				taskCtl.runningTasks[j - 1] = taskCtl.runningTasks[j];
			}
			taskCtl.runningTasksEnd--;
			taskCtl.stoppedTasks[taskCtl.stoppedTasksEnd++] = task;
			task->status = stopped;
			task_free(task);
			multiTask_switch();
			return TRUE;
		}
	}
	for(i = 0; i < taskCtl.sleepTasksEnd; i++) {
		if(taskCtl.sleepTasks[i] == task) {
			for(j = i + 1; j < taskCtl.sleepTasksEnd; j++) {
				taskCtl.sleepTasks[j - 1] = taskCtl.sleepTasks[j];
			}
			taskCtl.sleepTasksEnd--;
			taskCtl.stoppedTasks[taskCtl.stoppedTasksEnd++] = task;
			task->status = stopped;
			task_free(task);
			multiTask_switch();
			return TRUE;
		}
	}
	return FALSE;
}

/** @brief sort tasks in taskCtl.runningTask[].
 *  @param NULL
 *  @return NULL
 */
void task_sort_running_by_priority()
{
	int32 i, j;
	TASK* tmpTask;
	
	for(i = 1; i < taskCtl.runningTasksEnd; i++) {
		for(j = 0; j < taskCtl.runningTasksEnd - i; j++) {
			if(taskCtl.runningTasks[j]->priority > taskCtl.runningTasks[j + 1]->priority) {
				tmpTask = taskCtl.runningTasks[j];
				taskCtl.runningTasks[j] = taskCtl.runningTasks[j + 1];
				taskCtl.runningTasks[j + 1] = tmpTask;
			}
		}
	}
}

/** @brief sort tasks in taskCtl.sleepTasks[].
 *  @param NULL
 *  @return NULL
 */
void task_sort_sleep_by_sleepTime()
{
	int32 i, j;
	TASK* tmpTask;
	
	for(i = 1; i < taskCtl.sleepTasksEnd; i++) {
		for(j = 0; j < taskCtl.sleepTasksEnd - i; j++) {
			if(taskCtl.sleepTasks[j]->sleepTimeCnt_10ms > taskCtl.sleepTasks[j + 1]->sleepTimeCnt_10ms) {
				tmpTask = taskCtl.sleepTasks[j];
				taskCtl.sleepTasks[j] = taskCtl.sleepTasks[j + 1];
				taskCtl.sleepTasks[j + 1] = tmpTask;
			}
		}
	}
}

/** @brief set the basic param to be called by x86 internal function.
 *  TSS is a x86 basic function for tasks. It it easy to be used.
 *  @param tss tss pointer get. store in task for future use.
 *  @param taskId Id of this task.
 *  @param eip useful register.
 *  @param esp useful register.
 *  @param eflags useful register.
 *  @return NULL
 */
void tss_init(TSS32* tss, int32 taskId, int32 eip, int32 esp, int32 eflags)
{
	struct SEGMENT_DESCRIPTOR *gdt = (struct SEGMENT_DESCRIPTOR *) ADDR_GDT;
	
	tss->eip = eip;
	tss->esp = esp;
	tss->eflags = eflags;
	tss->eax = 0;
	tss->ecx = 0;
	tss->edx = 0;
	tss->ebx = 0;
	tss->ebp = 0;
	tss->esi = 0;
	tss->edi = 0;
	tss->es = 8;
	tss->cs = 8;
	tss->ss = 8;
	tss->ss0 = 0;		// added 不运行时该值为 0， 运行时非 0
	tss->ds = 8;
	tss->fs = 8;
	tss->gs = 8;
	tss->ldtr = (taskId + TASK_LIST_MAX) * 8;
	tss->iomap = 0x40000000;
	
	set_segmdesc(gdt + taskId, 103, (int32)tss, AR_TSS32);
}

/** @brief initialize multi task environment.
 *  This function initialize multiple tasks environment. The OS's task modol is Round Robin.
 *  @param NULL
 *  @return NULL
 */
void multiTask_init()
{
	int32 i;
	struct SEGMENT_DESCRIPTOR *gdt = (struct SEGMENT_DESCRIPTOR *) ADDR_GDT;

	taskCtl.taskNum = 1;
	taskCtl.taskIdBase = 4;				//当前任务的 Id 为 3
	taskCtl.nowRunningTaskPos = 0;		//当前任务在任务队列的位置
	taskCtl.stoppedTasksEnd = 0;
	taskCtl.sleepTasksEnd = 0;
	taskCtl.runningTasksEnd = 1;
	
	for(i = 0; i < TASK_LIST_MAX; i++) {
		taskCtl.stoppedTasks[i] = NULL;
		taskCtl.sleepTasks[i] = NULL;
		taskCtl.runningTasks[i] = NULL;
		taskCtl.tasks[i].isUsing = FALSE;
	}
	// 初始化当前任务的任务控制块
	taskCtl.tasks[0].isUsing = TRUE;
	taskCtl.tasks[0].taskId = 3;
	taskCtl.tasks[0].priority = 0;
	taskCtl.tasks[0].status = running;
	taskCtl.tasks[0].tss.ldtr = (3 + TASK_LIST_MAX) * 8;
	taskCtl.tasks[0].tss.iomap = 0x40000000;
	mem_alloc(strlen("Os Daemon Task"), (uint32*)&(taskCtl.tasks[0].taskName));
	strcpy(taskCtl.tasks[0].taskName, "Os Daemon Task");
	set_segmdesc(gdt + 3, 103, (int) &(taskCtl.tasks[0].tss), AR_TSS32);
	set_segmdesc(gdt + 3 + TASK_LIST_MAX, 15, (int) taskCtl.tasks[0].ldt, AR_LDT);
	
	load_tr(3 * 8);
	taskCtl.runningTasks[0] = &(taskCtl.tasks[0]);
	
	// 任务切换的时钟
	timer_add(SWITCH_TIME, &task_timer);
	mt_tr = 3;
}

/** @brief switch to next task.
 *  @param NULL
 *  @return NULL
 */
void multiTask_switch()
{
	int32 nextTaskPos;
	if(taskCtl.nowRunningTaskPos >= taskCtl.runningTasksEnd) {
		nextTaskPos = 0;
	}
	else {
		nextTaskPos = (taskCtl.nowRunningTaskPos + 1) % taskCtl.runningTasksEnd;
	}

	// 将 taskId 赋值给 mt_tr
	mt_tr = taskCtl.runningTasks[nextTaskPos]->taskId;
	timer_set_timeout(task_timer, SWITCH_TIME);
	
	if(mt_tr != taskCtl.runningTasks[taskCtl.nowRunningTaskPos]->taskId) {
		taskCtl.nowRunningTaskPos = nextTaskPos;
		far_jmp(0, mt_tr * 8);
	}
	return;
}

/** @brief check running task
 *  @param NULL
 *  @return the handle of the running task
 */
TASK* multiTask_NowRunning()
{
	return taskCtl.runningTasks[taskCtl.nowRunningTaskPos];
}
