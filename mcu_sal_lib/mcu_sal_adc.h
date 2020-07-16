#ifndef __MCU_SAL_ADC_H__
#define __MCU_SAL_ADC_H__

#include "mcu_sal_def.h"

#ifdef MCU_ADC_MODULE_ENABLED

typedef struct {
	uint8_t						pin;		// ADC pin
	uint8_t						autoscan;   // enable single channel scan function
	uint8_t						enable;		// enabled or disabled
	uint16_t					val; 		// save ADC standard value and uint is 0.1 mV
}ADC_STRUCT_T;

typedef struct {
	uint8_t  	  num;					//the number of add ADC PIN
	ADC_STRUCT_T *adc[ADC_MAX_NUM];		//ADC manager list
}ADC_LIST_T;

extern void adc_run(void);

extern void adc_list_init(ADC_LIST_T *list);

extern void adc_add(ADC_STRUCT_T *adc,uint8_t input_pin);

extern uint16_t adc_manual_poll(uint8_t input_pin);

extern uint16_t adc_get_value(uint8_t input_pin);


#endif

#endif

