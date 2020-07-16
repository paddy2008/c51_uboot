#include "mcu_sal.h"
#include "assert.h"
#include "Efm8lb12_adc.h"

ADC0_NEW_AUTOSCAN_BUFFER_ARRAY(scan_buf,ADC_SAMPLE_NUM,BUFFER_SCAN_TMP_ADDR);

// Calibration value for the temperature sensor at 0 degrees C, stored in CODE space
SI_LOCATED_VARIABLE_NO_INIT(TEMPSENSOR_0C_LOW,  uint8_t const, SI_SEG_CODE, TEMP_CAL_ADDRESS_LOW);
SI_LOCATED_VARIABLE_NO_INIT(TEMPSENSOR_0C_HIGH, uint8_t const, SI_SEG_CODE, TEMP_CAL_ADDRESS_HIGH);


/* return voltage in 0.1mV */
static uint16_t adc0_normalization(const uint16_t * buffer, uint8_t size)
{
	uint32_t i, sum = 0;
	
	for (i = 0; i < size; i++)
	  sum += (buffer[i] & ADC_MAX_RESULT);

	return (uint16_t)(((float)sum / (float)size * ADC_VREF_MV * 2) / ADC_MAX_RESULT);
}

/*
 * get standard voltage value and return voltage in 0.1mV
 */
uint16_t adc0_do_measure(ADC0_PositiveInput_t input_pin)
{
	ADC0_setAutoscanInputs(input_pin, 1);
	ADC0_enableAutoscan((ADC0_AutoscanBuffer_t *)scan_buf, ADC_SAMPLE_NUM, ADC0_AUTOSCAN_MODE_SINGLE);
	ADC0_startConversion();
	while(!ADC0_isConversionComplete());  // Wait for scan to complete
	ADC0_clearIntFlags(ADC0_CONVERSION_COMPLETE_IF);
	return adc0_normalization(&scan_buf[0], ADC_SAMPLE_NUM);
}

void adc0_hw_init(void)
{
	//PAC_DISABLED
	//ADC0 conversion initiated on write of 1 to ADBUSY.
	ADC0CN2 = ADC0CN2_ADCM__ADBUSY | ADC0CN2_PACEN__PAC_DISABLED;

	//ADC0 operates in 12-bit mode.
	//Right justified. No shifting applied.
	//Perform and Accumulate 1 conversions.
	ADC0CN1 = ADC0CN1_ADBITS__14_BIT | ADC0CN1_ADSJST__RIGHT_NO_SHIFT | ADC0CN1_ADRPT__ACC_1;


	//The ADC0 voltage reference is the internal voltage reference,
	//The reference is nominally 2.4 V.if MCU needs to uses the temperature sensor£¬so internal voltage reference must be selected
	//because the result of calculating is lower deviation(Îó²î)
	//Power Up Delay Time = 1.46us
	ADC0CF2 = ADC0CF2_GNDSL__GND_PIN | ADC0CF2_REFSL__INTERNAL_VREF | (0x04 << ADC0CF2_ADPWR__SHIFT);

	//ADC0CF0 = ADC0CF0_ADCLKSEL__SYSCLK | (0x01 << ADC0CF0_ADSC__SHIFT);
	//ADC0CF0 = ((ADC0_SYSCLK/ADC0SARCLK)-1)<<3;  // Set SAR clock to 12MHz
	 ADC0CF0 = ADC0CF0_ADCLKSEL__SYSCLK | (0x05 << ADC0CF0_ADSC__SHIFT);

	//Enable low power mode.
	//Conversion Tracking Time = 5us
	ADC0CF1 = ADC0CF1_ADLPM__LP_ENABLED | (0x3D << ADC0CF1_ADTK__SHIFT);

	//Autoscan Single Trigger Enable
	SFRPAGE = 0x30;
	ADC0ASCF |= ADC0ASCF_STEN__SINGLE_TRIGGER;

	//Disable ADC0 (low-power shutdown).
	//Power down when ADC is idle (not converting).
	//clear Conversion Complete Interrupt Flag
	//clear ADC Busy
	//clear Window Compare Interrupt Flag
	//The on-chip PGA gain is 1.0.
	//enable the Temperature Sensor
	SFRPAGE = 0x00;
	ADC0CN0 &= ~ADC0CN0_ADGN__FMASK;
	ADC0CN0 |= ADC0CN0_ADEN__ENABLED | ADC0CN0_ADGN__GAIN_0P5 | ADC0CN0_IPOEN__POWER_DOWN | ADC0CN0_TEMPE__TEMP_ENABLED;

	//EIE1 |= 0x08;  //ADC0 Conversion Complete Interrupt Disable
}

/* return in 1/256 C */
int16_t adc0_get_mcu_temp()
{
	uint16_t tempsensor_0c = 0;   // Stores the temp sensor cal value at 0C as [high byte][low byte]
	float code_temp = 0;
	tempsensor_0c = (TEMPSENSOR_0C_HIGH << 8) | TEMPSENSOR_0C_LOW;
	code_temp=(float)adc_get_value((uint8_t)ADC0_MCU_TEMP) * ADC_MAX_RESULT/ADC_VREF_MV/2;
	return (int16_t)((code_temp* 2.0 - (float)tempsensor_0c) / (float)28 * TEMP_SCALE);
}

static void _adc0_clearIntFlags_by_isr(uint8_t flags)
{
  uint8_t savedPage = SFRPAGE;

  SFRPAGE = 0x30;
  // Must pass one of the valid flags
  SLAB_ASSERT(flags & (ADC0_CONVERSION_COMPLETE_IF | ADC0_WINDOW_COMPARE_IF));

  ADC0CN0 &= ~(flags & (ADC0_CONVERSION_COMPLETE_IF | ADC0_WINDOW_COMPARE_IF));

  SFRPAGE = savedPage;
}

SI_INTERRUPT (ADC0EOC_ISR, ADC0EOC_IRQn)
{
	_adc0_clearIntFlags_by_isr(ADC0_CONVERSION_COMPLETE_IF);
}

