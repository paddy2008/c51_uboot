#ifndef __EFM8LB1_adc_h__
#define __EFM8LB1_adc_h__

#include <SI_EFM8LB1_Register_Enums.h>
#include "adc_0.h"

#define ADC0_MCU_TEMP		ADC0_POSITIVE_INPUT_TEMP
#define ADC0_VCC			ADC0_POSITIVE_INPUT_P13		// P1.7

#define ADC0_RSSI0			ADC0_POSITIVE_INPUT_P19		// P2.6
#define ADC0_RSSI1			ADC0_POSITIVE_INPUT_NONE
#define ADC0_RSSI2			ADC0_POSITIVE_INPUT_NONE
#define ADC0_RSSI3			ADC0_POSITIVE_INPUT_NONE

#define ADC0_MPD0			ADC0_POSITIVE_INPUT_P18  // P2.5
#define ADC0_MPD1			ADC0_POSITIVE_INPUT_P17  // P2.4
#define ADC0_MPD2			ADC0_POSITIVE_INPUT_P16	 // P2.3
#define ADC0_MPD3			ADC0_POSITIVE_INPUT_P15  // P2.2

#define ADC0_TEC_TEMPOUT	ADC0_POSITIVE_INPUT_P6		//P1.0

#define TEMP_SCALE  (256)

// Number of samples to take per channel,max value is 64
#define ADC_SAMPLE_NUM 				 (8)

//ADC 的参考电压, in 0.1mV
#define ADC_VREF_MV         	 (16500UL)

//读取MCU上的温度传感器
#define ON_CHIP_TEMP_SENSOR      (ADC0_POSITIVE_INPUT_TEMP)

#define TEMP_SENSOR_NUM_CHANNES  (1)

// 12 bit ADC
#define ADC_MAX_RESULT  		 ((1 << 14)-1)

// SYSCLK frequency in Hz
#define ADC0_SYSCLK   			 (49000000/2)

// Set SAR clock to 18MHz
#define ADC0SARCLK    			 (18000000)

// Address in flash where the temp cal low byte value is stored
#define TEMP_CAL_ADDRESS_LOW     (0xFFD4)

// Address in flash where the temp cal high byte value is stored
#define TEMP_CAL_ADDRESS_HIGH    (0xFFD5)

//auto scan buffer start addr at xdata
#define BUFFER_SCAN_TMP_ADDR 	 (0x0F20)


void		adc0_hw_init(void);
uint16_t 	adc0_do_measure(ADC0_PositiveInput_t input_pin);

#endif
