#ifndef __MCU_SAL_DBG_CMD_H__
#define __MCU_SAL_DBG_CMD_H__

#include "mcu_sal_def.h"

#ifdef MCU_DBG_CMD_MODULE_ENABLED

typedef struct
{
	uint8_t   cmd;
	uint8_t   dev_addr;
	uint16_t  offset;
	uint8_t   len;
	uint8_t   page;
}CONFIG_STRUCT_T;

typedef struct
{
	CONFIG_STRUCT_T  config;
	uint8_t  		 buf[DEBUG_CMD_BUFF_SIZE];
}DBG_CMD_STRUCT_T;

typedef enum
{
    CMD_TYPE_READ				= 0x01,
    CMD_TYPE_WRITE				= 0x02,
    CMD_TYPE_RESET				= 0x05,
    CMD_TYPE_TEC_ENABLE			= 0x06,
	CMD_TYPE_IDEL				= 0xF2,
} DEBUG_CMD_TYPE;


void 	dbg_cmd_handle(DBG_CMD_STRUCT_T *debug_cmd);
uint8_t i2c_pass_through_read(uint8_t dev_addr, uint8_t page, uint16_t offset_addr, uint8_t *value, uint8_t len);
uint8_t i2c_pass_through_write(uint8_t dev_addr,uint8_t page, uint16_t offset_addr, uint8_t *value, uint8_t len);

#endif

#endif
