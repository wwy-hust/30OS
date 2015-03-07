/** @file tim.h
 *  @brief basic functions related to timer.
 *
 *  The timer including create timer, start timer, stop timer, delete timer. etc.
 */

#ifndef _TIM_H_
#define _TIM_H_

#include "typedef.h"
#include "interrupt.h"
#include "funcUtils.h"
#include "multiTask.h"

/** @addtogroup hal
 *  @{
 */
#define PIT_CTRL	0x0043	///< BIOS used, related to timer
#define PIT_CNT0	0x0040	///< BIOS used, related to timer
/// @}

/** @addtogroup tim
 *  @brief this module is in charge of the time recording and timer management.
 *  The time is recorded in timerCtl.timecount. all timers restore in timerCtl.timers[]. 
 *  You could refer to TIM_CTL for more information.
 *  @{
 */
#define MAX_TIMER 256		///< max number of timer can be created.

/** @brief structure of one timer. */
typedef struct TIM{
	uint32 	timeout;		///< the timecount value then the timer is timeout.
	boolean	isUsing;		///< Whether the timer is under used.
	boolean	isTimeout;		///< whether the timer is timeout.
}TIM;

/** @brief structure of global timerCtl. */
typedef struct TIM_CTL{
	uint32	timecount;		///< global timecount value. when time interrupt happens, this value will increase one.
	int16 	sortedEnd;		///< End pos of sortedTimer[]. also the length of sortedTimer[].
	int16		timeoutEnd;	///< End pos of timeoutTimer[]. also the length of timeoutTimer[].
	int16		timerPos;	///< End pos of timers[]. also the count of created timer.
	TIM*		sortedTimer[MAX_TIMER];		///< sortedTimer[] list, order by timeout in TIM.
	TIM*		timeoutTimer[MAX_TIMER];	///< timeoutTimer[] list
	TIM			timers[MAX_TIMER];		///< store all timers in this list, no matter it was used or not.
}TIM_CTL;
///@}


// Timer interrupt handler
void interrupt_handler_0x20(int32 *esp);


/** @addtogroup tim
 *  @{
 */
void tim_init();
boolean timer_add(IN uint32 timeout, OUT TIM** timer);
boolean timer_set_timeout(IN TIM* timer, IN uint32 timeout);
boolean timer_del(IN TIM* timer);
boolean timer_is_valid(IN TIM* timer);
uint32	timer_getcount();

//used only inside tim model
void tim_revise();
void tim_sort();
///@}

#endif

