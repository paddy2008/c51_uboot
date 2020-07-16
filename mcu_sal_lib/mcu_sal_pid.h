#ifndef __MCU_SAL_PID_H__
#define __MCU_SAL_PID_H__

#include "mcu_sal_def.h"

#ifdef MCU_PID_MODULE_ENABLED

typedef struct
{
	int16_t 	kp;				// ����ϵ��,Proportional,range from 0.001 ~ 60 and amplify 1000 gains, so save values range from 1 ~ 60000
	int16_t     ti;				// ����ʱ�䳣��,Integral,range from 0.001 ~ 60 and amplify 1000 gains, so save values range from 1 ~ 60000
	int16_t     td;				// ΢��ʱ�䳣��,Derivative,range from 0.001 ~ 60 and amplify 1000 gains, so save values range from 1 ~ 60000
	uint16_t	targetValue; 	// uint 0.1mV
	uint8_t 	pid_en : 1;     // pid�㷨ʹ�ܿ��ء�
} PID_CONF_PARA;


typedef struct
{
	float 		kp;				// ����ϵ��,Proportional,range from 0.001 ~ 60 and amplify 1000 gains, so save values range from 1 ~ 60000
	float     	ti;				// ����ʱ�䳣��,Integral,range from 0.001 ~ 60 and amplify 1000 gains, so save values range from 1 ~ 60000
	float       td;				// ΢��ʱ�䳣��,Derivative,range from 0.001 ~ 60 and amplify 1000 gains, so save values range from 1 ~ 60000
	uint16_t	targetValue; 	// uint 0.1mV
	uint8_t 	pid_en : 1;     // pid�㷨ʹ�ܿ��ء�
} PID_ACTUAL_PARA;


typedef struct
{
	PID_ACTUAL_PARA		para;			// ���������֣�΢���Լ����Ĳ���
	uint8_t				balance : 1;    // ƽ����־λ
	int32_t			    prevError;		// ��һ�����
	int32_t			    sumError;		// ���Ļ��֣���ÿ���������
	int32_t 			balanceSumError;// ����ƽ���״̬�µ�����
	uint16_t		    outputValue;	// uint 0.1mV, pid����õ�����ֵ�������������·
	uint16_t		    inputValue;		// uint 0.1mV, pid���յ�����ֵ
}PID_STRUCT_T;

extern void 		pid_init(PID_STRUCT_T *pid);
extern void 		pid_conf(PID_CONF_PARA *pid_conf);
extern uint16_t 	pid_get_cal_val(uint16_t inputValue);
extern uint16_t     pid_get_input(void);

#endif

#endif

