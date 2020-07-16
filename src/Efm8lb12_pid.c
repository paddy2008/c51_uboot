//#include "Efm8lb12_pid.h"
#include "Efm8lb12_adc.h"
#include "mcu_sal.h"
#include "dac_3.h"

#define DAC_MAX_RESULT  ((1 << 12)-1)

//903 test board pin define
SI_SBIT (TEC_EN, SFR_P0, 3);		//TEC enable or disable
SI_SBIT (TX_DIS, SFR_P3, 2);		//TX close single

static void pid_set_hw_output(uint16_t output)
{
	uint16_t dac_verf_mv = 0, dac_value = 0;

	dac_verf_mv = adc_get_value(ADC0_VCC);
	dac_value = (uint16_t)((float)output/dac_verf_mv * DAC_MAX_RESULT) & DAC_MAX_RESULT;

	DAC3_setOutput(dac_value);
}

void pid_hw_init(PID_CONF_PARA *conf)
{
	flash_read(MSA_FLASH_ADDR_PAGE_00H, (uint8_t *)&conf->targetValue, 2);

	pid_conf(conf);
}

void pid_hw_run()
{
	uint16_t tec_temp = 0;

	if (!(TX_DIS == 0) && (TEC_EN == 1))
	{
		return;
	}

	tec_temp = adc_get_value(ADC0_TEC_TEMPOUT);

	pid_set_hw_output(pid_get_cal_val(tec_temp));
}


