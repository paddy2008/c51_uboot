#ifndef __MCU_SAL_PID_H__
#define __MCU_SAL_PID_H__

#include "mcu_sal_def.h"

#ifdef MCU_PID_MODULE_ENABLED

typedef struct
{
	int16_t 	kp;				// 比例系数,Proportional,range from 0.001 ~ 60 and amplify 1000 gains, so save values range from 1 ~ 60000
	int16_t     ti;				// 积分时间常数,Integral,range from 0.001 ~ 60 and amplify 1000 gains, so save values range from 1 ~ 60000
	int16_t     td;				// 微分时间常数,Derivative,range from 0.001 ~ 60 and amplify 1000 gains, so save values range from 1 ~ 60000
	uint16_t	targetValue; 	// uint 0.1mV
	uint8_t 	pid_en : 1;     // pid算法使能开关、
} PID_CONF_PARA;


typedef struct
{
	float 		kp;				// 比例系数,Proportional,range from 0.001 ~ 60 and amplify 1000 gains, so save values range from 1 ~ 60000
	float     	ti;				// 积分时间常数,Integral,range from 0.001 ~ 60 and amplify 1000 gains, so save values range from 1 ~ 60000
	float       td;				// 微分时间常数,Derivative,range from 0.001 ~ 60 and amplify 1000 gains, so save values range from 1 ~ 60000
	uint16_t	targetValue; 	// uint 0.1mV
	uint8_t 	pid_en : 1;     // pid算法使能开关、
} PID_ACTUAL_PARA;


typedef struct
{
	PID_ACTUAL_PARA		para;			// 比例，积分，微分以及误差的参数
	uint8_t				balance : 1;    // 平衡点标志位
	int32_t			    prevError;		// 上一个误差
	int32_t			    sumError;		// 误差的积分，即每次误差的求和
	int32_t 			balanceSumError;// 保存平衡点状态下的误差和
	uint16_t		    outputValue;	// uint 0.1mV, pid计算得到的数值，输出到反馈电路
	uint16_t		    inputValue;		// uint 0.1mV, pid接收到的数值
}PID_STRUCT_T;

extern void 		pid_init(PID_STRUCT_T *pid);
extern void 		pid_conf(PID_CONF_PARA *pid_conf);
extern uint16_t 	pid_get_cal_val(uint16_t inputValue);
extern uint16_t     pid_get_input(void);

#endif

#endif

