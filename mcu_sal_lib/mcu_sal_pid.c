#include "mcu_sal.h"

#ifdef MCU_PID_MODULE_ENABLED

static PID_STRUCT_T *_pid_struct = NULL;

/*
 * ��ΪPID�㷨���л������㣬һֱ�����ۼ�
 * ���Ա���Ҫ���ǻ��ֵĺ�����Ķ���������
 * ���ȶ��Ժ󣬾Ϳ��Խ���صĲ�����Ϣ����
 * �ˣ���Ҫע����������"�ȶ�"����Ҫ����
 * ʵ��Ӳ���������ģ�Ҳ������С���������������1500mv��
 * ���������1mv���ڣ���ô1501mv��1499mv���ǿ��Զ���Ϊ"�ȶ���"
 * �൱�����¿�ʼ����PID����
 */
static void pid_calculate(void)
{
	int32_t  error = 0, error2 = 0;

	error = (int32_t)_pid_struct->para.targetValue - (int32_t)_pid_struct->inputValue;  //notice there must be type conversion

	if(_pid_struct->balance)
	{
		_pid_struct->sumError = _pid_struct->balanceSumError + error;
	}
	else
	{
		_pid_struct->sumError += error;
	}

	if(_pid_struct->prevError != 0x0)  //indicate not the first run here
	{
		error2 = error - _pid_struct->prevError;
	}
	else
	{
		error2 = 0;
	}

	_pid_struct->prevError = error;

	_pid_struct->outputValue = PID_BALANCE_POINT +
    (uint16_t)(_pid_struct->para.kp * (float)error
		     + _pid_struct->para.ti * (float)_pid_struct->sumError
		     + _pid_struct->para.td * (float)error2);
}

static void _pid_deinit()
{
	_pid_struct->balance    	 = 0;
	_pid_struct->inputValue  	 = 0;
	_pid_struct->outputValue 	 = 0;
	_pid_struct->prevError   	 = 0;
	_pid_struct->sumError    	 = 0;
	_pid_struct->balanceSumError = 0;
}

void pid_conf(PID_CONF_PARA *pid_conf)
{
    ASSERT(pid_conf);

	_pid_struct->para.kp 			= pid_conf->kp / PID_CONF_PARA_GAIN;
	_pid_struct->para.ti 			= pid_conf->ti / PID_CONF_PARA_GAIN;
	_pid_struct->para.td 			= pid_conf->td / PID_CONF_PARA_GAIN;
	_pid_struct->para.pid_en 	  	= pid_conf->pid_en;
	_pid_struct->para.targetValue 	= pid_conf->targetValue;
}

void pid_init(PID_STRUCT_T *pid)
{
    ASSERT(pid);

    _pid_struct = pid;

	memset(pid, 0, sizeof(PID_STRUCT_T));

	_pid_struct->para.kp 			= 0.3;
	_pid_struct->para.ti 			= 0.01;
	_pid_struct->para.td 			= 0.1;
	_pid_struct->para.pid_en 	  	= 1;
	_pid_struct->para.targetValue 	= 0;
	_pid_struct->inputValue 		= PID_BALANCE_POINT;
	_pid_struct->balance 			= 0;
}

uint16_t pid_get_cal_val(uint16_t inputValue)
{
    static uint8_t num = 0;

    if(_pid_struct->para.pid_en)
    {
		_pid_struct->inputValue = inputValue;

		if(abs(_pid_struct->para.targetValue - _pid_struct->inputValue) < PID_OUTPUT_ERROR)
		{
			if(_pid_struct->balance == false)  //indicate from no balance state change to balance state
			{
				_pid_struct->balance = true;
				_pid_struct->balanceSumError = _pid_struct->sumError;
				_pid_struct->sumError = 0;
			}
		}
		else
		{
			if(_pid_struct->balance)  //indicate from balance state change to no balance state
			{
				_pid_struct->balance  = false;
				_pid_struct->sumError = _pid_struct->balanceSumError;
			}

			if(abs(_pid_struct->para.targetValue - _pid_struct->inputValue) > PID_OUTPUT_MAX_ERROR)
			{
				num ++;
				if(num > 20)
				{
					_pid_deinit();
					num = 0;
				}
			}
			else
			{
				num = 0;
			}
		}

		pid_calculate();
		return _pid_struct->outputValue;
    }

    return RET_FAILED;
}

uint16_t pid_get_input(void)
{
	return _pid_struct->inputValue;
}

#endif
