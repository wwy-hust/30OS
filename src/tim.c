/** @file tim.c
 *  @brief basic functions related to timer.
 *
 *  The timer including create timer, start timer, stop timer, delete timer. etc.
 */

#include "tim.h"

TIM_CTL timerCtl;	///< The struct used to hold all timers.
extern TIM* task_timer;		///< the timer used to switch tasks.
extern TASK_CTL taskCtl;	///< taskes control block used to control switch tasks.

/** @brief timer interrupt.
 *  Add global timecounter. check sortedTimer[] in timerCtl to see if there are some timers which are timeout.
 *  Put the timeout timer to timerCtl.timeoutTimer[]. check sleep tasks list in taskCtl, if the task has sleep 
 *  for enough time, wake up them and call scheduler.
 *  @param esp stack pointer
 *  @return NULL
 */
void interrupt_handler_0x20(int32 *esp)
{
	uint8 i, j;
	io_out8(PIC0_OCW2, 0x60);		///< 把IRQ-00信号接收结束的信息通知给PIC

	timerCtl.timecount++;

	if(timerCtl.sortedEnd != 0 && timerCtl.timecount >= timerCtl.sortedTimer[0]->timeout) {	///< 有定时器超时
		for(i = 0; i < timerCtl.sortedEnd && timerCtl.timecount >= timerCtl.sortedTimer[i]->timeout; i++);
		for(j = 0; j < i; j++) {
			timerCtl.sortedTimer[j]->isTimeout = TRUE;
			timerCtl.timeoutTimer[timerCtl.timeoutEnd++] = timerCtl.sortedTimer[j];
		}
		for(j = i; j < timerCtl.sortedEnd; j++) {
			timerCtl.sortedTimer[j - i] = timerCtl.sortedTimer[j];
		}
		timerCtl.sortedEnd -= i;
		timerCtl.sortedTimer[0] = (timerCtl.sortedEnd == 0) ? NULL : timerCtl.sortedTimer[0];

		if(task_timer != NULL && task_timer->isUsing == TRUE && task_timer->isTimeout == TRUE) {
			multiTask_switch();
		}
	}
	
	if(taskCtl.sleepTasksEnd > 0) {
		/// 找到超时的 sleep 队列中的位置
		for(i = 0; i < taskCtl.sleepTasksEnd && taskCtl.sleepTasks[i]->sleepTimeCnt_10ms <= timerCtl.timecount; i++);
		/// 将所有超时的任务放到运行队列中
		for(j = 0; j < i; j++) {
			taskCtl.sleepTasks[j]->status = running;
			taskCtl.runningTasks[taskCtl.runningTasksEnd++] = taskCtl.sleepTasks[j];
		}
		/// 从 sleep 队列中删掉这些任务
		for(j = i; j < taskCtl.sleepTasksEnd; j++) {
			taskCtl.sleepTasks[j - i] = taskCtl.sleepTasks[j];
		}
		/// 进行调度
		multiTask_switch();
	}
	
}

/** @brief init timerCtl.
 *  Init timerCtl structure. Set hardware interrupt interval.
 *  @param NULL
 *  @return NULL
 */
void tim_init()
{
	int i = 0;
	timerCtl.sortedEnd = 0;
	timerCtl.timeoutEnd = 0;
	timerCtl.timecount = 0;
	timerCtl.timerPos = 0;
	for(; i < MAX_TIMER; i++) {
		timerCtl.sortedTimer[i] = NULL;
		timerCtl.timeoutTimer[i] = NULL;
		timerCtl.timers[i].isUsing = FALSE;
	}
	
	io_out8(PIT_CTRL, 0x34);
	// 设置为 0x2e9c, 一秒约中断100次
	io_out8(PIT_CNT0, 0x9c);
	io_out8(PIT_CNT0, 0x2e);
}

/** @brief create a timer, add the timer into sortedTimer list.
 *  input timeout value. unit of timeout is 10ms.
 *  @param timeout the timeout value in 10ms.
 *  @param timer the timer handle you can used to reset timeout, stop timer, del timer.
 *  @return whether add operation is successful. if yes, return TRUE els FALSE.
 */
boolean timer_add(IN uint32 timeout, OUT TIM** timer)
{
	if(timerCtl.sortedEnd >= MAX_TIMER) {
		timer = NULL;
		return FALSE;
	}
	for(;timerCtl.timers[timerCtl.timerPos].isUsing == TRUE;timerCtl.timerPos = (timerCtl.timerPos + 1) % MAX_TIMER);

	(*timer) = &(timerCtl.timers[timerCtl.timerPos]);
	(*timer)->timeout = timeout + timerCtl.timecount;
	if((*timer)->timeout < timerCtl.timecount) {
		tim_revise();
	}
	(*timer)->timeout = timeout + timerCtl.timecount;
	(*timer)->isUsing = TRUE;
	(*timer)->isTimeout = FALSE;
	timerCtl.sortedTimer[timerCtl.sortedEnd] = (*timer);
	timerCtl.sortedEnd++;
	timerCtl.timerPos = (timerCtl.timerPos + 1) % MAX_TIMER;
	tim_sort();
	return TRUE;
}

/** @brief reset timeout value of timer.
 *  input timer handle and timeout value. unit of timeout is 10ms.
 *  @param timer timer handle of the timer you want to set.
 *  @param timeout the timeout value in 10ms.
 *  @return whether operation is successful. if yes, return TRUE els FALSE.
 */
boolean timer_set_timeout(IN TIM* timer, IN uint32 timeout)
{
	int32 i, j;
	if(timer == NULL || timer->isUsing == FALSE)
		return FALSE;
	
	for(i = 0; i < timerCtl.timeoutEnd && timer != timerCtl.timeoutTimer[i]; i++);
	if(i >= timerCtl.timeoutEnd) {
		for(i = 0; i < timerCtl.sortedEnd && timer != timerCtl.sortedTimer[i]; i++);
		if(i >= timerCtl.sortedEnd)		//未找到该 定时器
			return FALSE;
		//调整定时器参数即可
		timer->timeout = timeout + timerCtl.timecount;
		if(timer->timeout < timerCtl.timecount) {
			tim_revise();
		}
		tim_sort();
	}
	else {		//在超时的队列中找到了该 定时器(位置为 i)，将其从超时队列移动到排序队列
		for(j = i + 1; j < timerCtl.timeoutEnd; j++) {
			timerCtl.timeoutTimer[j - 1] = timerCtl.timeoutTimer[j];
		}
		timerCtl.timeoutEnd--;

		timer->timeout = timeout + timerCtl.timecount;
		if(timer->timeout < timerCtl.timecount) {
			tim_revise();
		}
		timer->isTimeout = FALSE;
		timerCtl.sortedTimer[timerCtl.sortedEnd++] = timer;
		tim_sort();
	}
	return TRUE;
}

/** @brief delete one timer.
 *  input timer handle to delete the timer.
 *  @param timer timer handle of the timer you want to delete.
 *  @return whether operation is successful. if yes, return TRUE els FALSE.
 */
boolean timer_del(IN TIM* timer)
{
	int i;
	for(i = 0; i < timerCtl.timeoutEnd && timerCtl.timeoutTimer[i] != timer; i++);
	if(i == timerCtl.timeoutEnd) {
		return FALSE;
	}
	for(; i < timerCtl.timeoutEnd - 1; i++) {
		timerCtl.timeoutTimer[i] = timerCtl.timeoutTimer[i + 1];
	}
	timer->isUsing = FALSE;
	return TRUE;
}

/** @brief test whether the timer is valid.
 *  input timer handle of the timer.
 *  @param timer timer handle of the timer you want to test.
 *  @return whether the timer is valid.
 */
boolean timer_is_valid(IN TIM* timer)
{
	int32 i;
	for(i = 0; i < MAX_TIMER && timer != &(timerCtl.timers[i]); i++);
	if(i < MAX_TIMER)
		return TRUE;
	else 
		return FALSE;
}

/** @brief get current time counter.
 *  @param NULL
 *  @return the time counter value.
 */
uint32	timer_getcount()
{
	return timerCtl.timecount;
}

/** @brief internal called function. 
 *  if the counter is overflow. then set time counter to 0 and revise every timer existed.
 *  @param NULL
 *  @return NULL
 */
void tim_revise()
{
	uint32 t, i;
	io_cli();
	t = timerCtl.timecount;
	for(i = 0; i < timerCtl.sortedEnd; i++) {
		timerCtl.sortedTimer[i]->timeout -= t;
	}
	timerCtl.timecount = 0;
	io_sti();
}

/** @brief sort the timer in sortedTimer[] list. 
 *  @param NULL
 *  @return NULL
 */
void tim_sort()
{
	int i, j;
	TIM* tmpTimer;
	for(i = timerCtl.sortedEnd; i > 0; i--) {
		for(j = 0; j < i - 1; j++) {
			if(timerCtl.sortedTimer[j]->timeout > timerCtl.sortedTimer[j + 1]->timeout) {
				tmpTimer = timerCtl.sortedTimer[j];
				timerCtl.sortedTimer[j] = timerCtl.sortedTimer[j + 1];
				timerCtl.sortedTimer[j + 1] = tmpTimer;
			}
		}
	}
}
