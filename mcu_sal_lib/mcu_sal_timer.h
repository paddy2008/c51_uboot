#ifndef __MCU_SAL_TIMER_H__
#define __MCU_SAL_TIMER_H__

#include "mcu_sal_def.h"

#ifdef MCU_TIMER_MODULE_ENABLED

#define JIFFIES_MAX         	(0xFFFFFFFF)  //jiffies max value

typedef  void (*TIMER_FUNC)(void);

typedef struct
{
	volatile uint32_t timeout;   		//定时器的超时时间,因为中断处理函数中有更新，所以declare by key "volatile"
	volatile uint32_t expires;   		//定时器在当前的时间上加上超时时间,因为中断处理函数中有更新，所以declare by key "volatile"
    volatile uint8_t  timeout_flag;     //定时器超时标志位,因为中断处理函数中有更新，所以declare by key "volatile"
    uint8_t  		  enable;    		//定时器使能开关
    TIMER_FUNC        function;			//定时器的处理函数
}TIMER_STRUCT_T ;


/*
 *
 *这里用指针数组将所有的定时器关联起来，如果这里
 *直接分配定时器内存，那么会占用10个定时器的内存
 *空间，实际上可能一个定时器也不会使用，对于硬件
 *资源本身有限的MCU来说显然不合理
 */
typedef struct
{
	uint8_t 			num;
	volatile uint32_t   jiffies;
	TIMER_STRUCT_T 	    *timer[TIMER_NUM];
}TIMER_LSIT_T;


extern void 	timer_run(void);
extern void 	timer_update(void);
extern void 	timer_delay_ms(uint16_t ms);
extern void 	timer_stop(TIMER_STRUCT_T * timer);
extern void 	timer_start(TIMER_STRUCT_T *timer);
extern void 	timer_add(TIMER_STRUCT_T *timer);
extern void 	timer_list_init(TIMER_LSIT_T *list);
extern uint32_t timer_get_tick(void);

#endif


#endif

