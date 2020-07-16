#ifndef __MCU_SAL_TIMER_H__
#define __MCU_SAL_TIMER_H__

#include "mcu_sal_def.h"

#ifdef MCU_TIMER_MODULE_ENABLED

#define JIFFIES_MAX         	(0xFFFFFFFF)  //jiffies max value

typedef  void (*TIMER_FUNC)(void);

typedef struct
{
	volatile uint32_t timeout;   		//��ʱ���ĳ�ʱʱ��,��Ϊ�жϴ��������и��£�����declare by key "volatile"
	volatile uint32_t expires;   		//��ʱ���ڵ�ǰ��ʱ���ϼ��ϳ�ʱʱ��,��Ϊ�жϴ��������и��£�����declare by key "volatile"
    volatile uint8_t  timeout_flag;     //��ʱ����ʱ��־λ,��Ϊ�жϴ��������и��£�����declare by key "volatile"
    uint8_t  		  enable;    		//��ʱ��ʹ�ܿ���
    TIMER_FUNC        function;			//��ʱ���Ĵ�����
}TIMER_STRUCT_T ;


/*
 *
 *������ָ�����齫���еĶ�ʱ�������������������
 *ֱ�ӷ��䶨ʱ���ڴ棬��ô��ռ��10����ʱ�����ڴ�
 *�ռ䣬ʵ���Ͽ���һ����ʱ��Ҳ����ʹ�ã�����Ӳ��
 *��Դ�������޵�MCU��˵��Ȼ������
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

