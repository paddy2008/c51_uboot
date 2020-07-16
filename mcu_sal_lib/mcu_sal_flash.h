#ifndef __MCU_SAL_FLASH_H__
#define __MCU_SAL_FLASH_H__

#include "mcu_sal_def.h"

#if defined(MCU_FLASH_API_MODULE_ENABLED) || defined(MCU_FLASH_BUFF_MODULE_ENABLED)

#define  GET_FLASH_PAGE_START_ADDR(addr)  ((addr) & (~((FLASH_PAGE_SIZE) - 1)))
/*
 * 按照如下定义:
 * typedef struct
 *{
 *	uint16_t page_start_addr;
 *	uint8_t  offset;
 *	uint8_t  value;
 *	uint8_t  empty : 1;
 *}WRTTE_FLASH_STRUCT_T;
 * 处理起来方便很多，但是这样一样，WRTTE_FLASH_STRUCT_T结构体的大小就是5个bytes
 * 如果定义为page_index,
 * flash的基地址等于       = 0 + FLASH_PAGE_SIZE * page_index
 * 可以将empty和page_start_addr组合到一起，这样WRTTE_FLASH_STRUCT_T结构体
 * 的大小就是3个bytes,如果缓冲区定义为128字节，那么就节约了256byte的空间，这对硬件
 * 资源相当紧张的MCU而言很重要
 * */
typedef struct
{
	FLADDR    addr  ;
	uint8_t   offset;  //bit7:1---buff has value,0--- buff do't have value,bit0~bit6:replace the flash page index
	uint8_t   value;
}FLASH_DATA_STRUCT_T;

typedef struct
{
	uint8_t num;		//实际填充数据的个数
	FLASH_DATA_STRUCT_T buff[FLASH_USER_PAGE_SIZE];
}FLASH_BUFF_STRUCT_T;

void flash_init(FLASH_BUFF_STRUCT_T *flash);
void flash_write_run(void);

void flash_write_byte(FLADDR addr, uint8_t value);
void flash_read (FLADDR addr, uint8_t *value, uint16_t len);
int8_t flash_write(FLADDR addr, uint8_t *value, uint16_t len);
int8_t flash_update(FLADDR addr, uint8_t *value, uint16_t len);
void flash_recv_byte(FLADDR addr, uint8_t offset, const uint8_t value);

#endif

#endif
