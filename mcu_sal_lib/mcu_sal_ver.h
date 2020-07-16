#ifndef __MCU_SAL_VER_H__
#define __MCU_SAL_VER_H__

#include "mcu_sal_def.h"

#define VERSION_MAJOR	(1)
#define VERSION_MINOR	(1)

typedef struct
{
	uint8_t sec;
	uint8_t min;
	uint8_t hour;
	uint8_t day;
	uint8_t mon;
	uint8_t year;
}BUILD_TIME;

uint8_t  get_year(const char *date);
uint8_t  get_month(const char *date);
uint8_t  get_day(const char *date);
uint8_t  get_hour(const char *time);
uint8_t  get_minute(const char *time);
uint8_t  get_second(const char *time);

#endif
