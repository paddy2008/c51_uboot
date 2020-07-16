#include "mcu_sal.h"

#ifdef MCU_LOG_MODULE_ENABLED

static char  *_log = NULL;

/*
 * _log[0] 保存log的长度
 * 从_log[1] 开始保存log内容
 * */
void log_init(char *log)
{
	ASSERT(log);
	_log = log;
	memset((void *)_log, 0, LOG_RAM_SIZE);
	_log[0] = 1;
}

/*
 * log_clear这个动作在哪里触发？
 * 1.向_log[0]写1，这种实现方式比较简单，不依赖任何其他的功能模块
 * 2.放入到debug命令中，这种实现方式需要依赖i2c从设备实现了debug功能模块
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
