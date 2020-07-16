#ifndef __MCU_SAL_TASK_H__
#define __MCU_SAL_TASK_H__

#include "mcu_sal_def.h"

#ifdef MCU_TASK_MODULE_ENABLED

typedef  void (*TASK_FUNC)(void);


/*
 * 需要特别注意的是由于keil编译器的特性，所有栈空间共用，
 * 它是通过调用关系来确保栈参数正确的传递的，如果使用keil
 * 编译器，只要使用到函数指针，都必须通过OVERLAY指令手动
 * 调整调用关系为正确预期的调用关系。另外在测试这个特性的时候
 * 如果函数指针执行的函数内容比较少，例如只打印一句输出信息，
 * 它并不会有问题，因为使用到的堆栈空间较少，不会覆盖上级调用
 * 的堆栈空间，就不会有问题
 */
typedef struct
{
	uint8_t 	id;     //task id and it is used for fast scheduling task match
	uint8_t     pri;    //task priority
	TASK_FUNC   func;   //task body pointer
}TASK_STRUCT_T;

typedef struct
{
	uint8_t num;           //the number of the fast scheduling tasks
	uint8_t id[TASK_NUM];  //the task id of the fast scheduling tasks
}FAST_TASK_T;

typedef struct
{
	uint8_t 	    		task_num;   		//the number of the all tasks
	volatile FAST_TASK_T	fast_task;  		//because fast_task is updated by multiple interrupt handle function
	TASK_STRUCT_T  			*tasks[TASK_NUM];
}TASK_LIST_T;

/*
 * task id must be unique
 */
typedef enum
{
	TASK_ID_TERMINAL_HANDLE				= 0x0,
	TASK_ID_TESE1						= 0x1,
	TASK_ID_TESE2						= 0x2,
	TASK_ID_TESE3						= 0x3,
	TASK_ID_TESE4						= 0x4,
	TASK_ID_DEBUG_CMD_TASK				= 0x5,
	TASK_ID_ADC_RUN_TASK				= 0x6,
	TASK_ID_MSA_RUN_TASK				= 0x8,
	TASK_ID_TIMER_TASK					= 0x10,
	TASK_ID_INVALID,
}TASK_ID;

/*
 * task priority must be unique
 */
typedef enum
{
	PRI0		= 0x0,
	PRI1		= 0x1,
	PRI2		= 0x2,
	PRI3		= 0x3,
	PRI4		= 0x4,
	PRI5		= 0x5,
	PRI6		= 0x6,
	PRI7		= 0x7,
	PRI8		= 0x8,
	PRI9		= 0x9,
	PRI10		= 0xA,
	PRI11		= 0xB,
	PRI12		= 0xC,
	PRI13		= 0xD,
	PRI14		= 0xE,
	PRI15		= 0xF,
	PRI16		= 0x10,
	PRI17		= 0x11,
	PRI18		= 0x12,
	PRI19		= 0x13,
	PRI20		= 0x14,
}TASK_PRI;

void task_list_init(TASK_LIST_T *list);
void task_run(void);
void task_fast_handle(uint8_t id);
void task_add(TASK_STRUCT_T *task);

#endif

#endif
