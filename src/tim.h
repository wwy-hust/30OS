
#ifndef _TIM_H_
#define _TIM_H_

#include "typedef.h"
#include "interrupt.h"
#include "funcUtils.h"
#include "multiTask.h"

#define PIT_CTRL	0x0043
#define PIT_CNT0	0x0040

#define MAX_TIMER 256

typedef struct TIM{
	uint32 	timeout;
	boolean	isUsing;
	boolean	isTimeout;
}TIM;

typedef struct TIM_CTL{
	uint32	timecount;				//时间基准，中断中累加
	int16 	sortedEnd;
	int16		timeoutEnd;
	int16		timerPos;
	TIM*		sortedTimer[MAX_TIMER];
	TIM*		timeoutTimer[MAX_TIMER];
	TIM			timers[MAX_TIMER];
}TIM_CTL;

// Timer interrupt handler
void interrupt_handler_0x20(int32 *esp);

void tim_init();
boolean timer_add(IN uint32 timeout, OUT TIM** timer);
boolean timer_set_timeout(IN TIM* timer, IN uint32 timeout);
boolean timer_del(IN TIM* timer);
boolean timer_is_valid(IN TIM* timer);
uint32	timer_getcount();

//used only in tim model
void tim_revise();
void tim_sort();

#endif

