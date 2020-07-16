#ifndef __MCU_SAL_REG_MAP_H__
#define __MCU_SAL_REG_MAP_H__

#include "mcu_sal_def.h"

#ifdef MCU_REG_MAP_MODULE_ENABLED

typedef uint8_t MCU_BIT_WIDTH_T;

typedef struct
{
	char   			*name;
	uint8_t 		 page;
	MCU_BIT_WIDTH_T  reg_value;
}REG_MAP_STRUCT_T;


typedef struct
{
	uint8_t num;							//registers map to ram's buffer number
	uint8_t max_num;
	REG_MAP_STRUCT_T *map;  				//registers map to ram's buffer space
}REG_MAP_LIST_T;

void reg_map_list_init(REG_MAP_LIST_T *list);
void reg_map_init(REG_MAP_STRUCT_T *reg_map);
void reg_add_show(char *name, MCU_BIT_WIDTH_T reg_value, uint8_t page);
MCU_BIT_WIDTH_T reg_write(char* name, MCU_BIT_WIDTH_T reg_value);
MCU_BIT_WIDTH_T reg_read(char* name);

#endif

#endif
