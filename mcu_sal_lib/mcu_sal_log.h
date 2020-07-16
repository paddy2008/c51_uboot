#ifndef __MCU_SAL_LOG_H__
#define __MCU_SAL_LOG_H__

#include "mcu_sal_def.h"

#ifdef MCU_LOG_MODULE_ENABLED

void log_clear();
void log_init(char *log);
void log_save(char *fmt,...);

#endif

#endif
