#include "mcu_sal.h"

#ifdef MCU_LOG_MODULE_ENABLED

static char  *_log = NULL;

/*
 * _log[0] ����log�ĳ���
 * ��_log[1] ��ʼ����log����
 * */
void log_init(char *log)
{
	ASSERT(log);
	_log = log;
	memset((void *)_log, 0, LOG_RAM_SIZE);
	_log[0] = 1;
}

/*
 * log_clear������������ﴥ����
 * 1.��_log[0]д1������ʵ�ַ�ʽ�Ƚϼ򵥣��������κ������Ĺ���ģ��
 * 2.���뵽debug�����У�����ʵ�ַ�ʽ��Ҫ����i2c���豸ʵ����debug����ģ��
 */
void log_clear()
{
	ASSERT(_log);
	memset((void *)_log, 0, LOG_RAM_SIZE);
	_log[0] = 1;
}

void log_save(char *fmt,...)
{
	char str[LOG_RAM_SIZE] = {0};
	uint8_t len = 0;
	va_list ap;

	ASSERT(_log);

	va_start(ap,fmt);
	vsprintf(str,fmt,ap);
	va_end(ap);
	len = strlen(str);

	if ((_log[0] + len) >= LOG_RAM_SIZE)
		return;

	strcpy(&_log[_log[0]], str);
	_log[0] += len;
}


#endif
