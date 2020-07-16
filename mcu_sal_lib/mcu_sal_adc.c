#include "mcu_sal.h"

#ifdef MCU_ADC_MODULE_ENABLED

static ADC_LIST_T  *_adc_list = NULL;

void adc_list_init(ADC_LIST_T *list)
{
	uint8_t i = 0;

	ASSERT(list);

	_adc_list = list;

	for(i = 0; i < ADC_MAX_NUM; i++)
	{
		_adc_list->adc[i] = NULL;
	}

	_adc_list->num = 0;
}

void adc_add(ADC_STRUCT_T *adc, uint8_t input_pin)
{
	ASSERT(adc);
	ASSERT(_adc_list);

	if(_adc_list->num < ADC_MAX_NUM)
	{
		_adc_list->adc[_adc_list->num] = adc;
	}
	else
	{
		lib_log("\r\n adc_add failed");
		return;
	}

	_adc_list->adc[_adc_list->num]->autoscan   = 1;
	_adc_list->adc[_adc_list->num]->enable	   = 1;        //add new adc pin and default state is enable
	_adc_list->adc[_adc_list->num]->pin        = input_pin;
	_adc_list->num ++;
}

void adc_run(void)
{
	uint8_t i = 0;

	ASSERT(_adc_list);

	for(i = 0; (_adc_list->adc[i] != NULL) && (i < _adc_list->num); i++)
	{
		if (_adc_list->adc[i]->autoscan && _adc_list->adc[i]->enable)
		{
			_adc_list->adc[i]->val = lib_adc_do_measure(_adc_list->adc[i]->pin);
		}
	}
}

uint16_t adc_get_value(uint8_t input_pin)
{
	uint8_t i = 0;

	RE_ASSERT(_adc_list);

	for(i = 0; (_adc_list->adc[i] != NULL) && (i < _adc_list->num); i++)
	{
		if(input_pin == _adc_list->adc[i]->pin)
		{
			return _adc_list->adc[i]->val;
		}
	}
	lib_log("\r\n adc_get_value failed");
	return 0;
}


uint16_t adc_manual_poll(uint8_t input_pin)
{
	return lib_adc_do_measure(input_pin);
}

#endif


