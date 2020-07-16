#ifndef __MCU_SAL_FLASH_H__
#define __MCU_SAL_FLASH_H__

#include "mcu_sal_def.h"

#if defined(MCU_FLASH_API_MODULE_ENABLED) || defined(MCU_FLASH_BUFF_MODULE_ENABLED)

#define  GET_FLASH_PAGE_START_ADDR(addr)  ((addr) & (~((FLASH_PAGE_SIZE) - 1)))
/*
 * �������¶���:
 * typedef struct
 *{
 *	uint16_t page_start_addr;
 *	uint8_t  offset;
 *	uint8_t  value;
 *	uint8_t  empty : 1;
 *}WRTTE_FLASH_STRUCT_T;
 * ������������ܶ࣬��������һ����WRTTE_FLASH_STRUCT_T�ṹ��Ĵ�С����5��bytes
 * �������Ϊpage_index,
 * flash�Ļ���ַ����       = 0 + FLASH_PAGE_SIZE * page_index
 * ���Խ�empty��page_start_addr��ϵ�һ������WRTTE_FLASH_STRUCT_T�ṹ��
 * �Ĵ�С����3��bytes,�������������Ϊ128�ֽڣ���ô�ͽ�Լ��256byte�Ŀռ䣬���Ӳ��
 * ��Դ�൱���ŵ�MCU���Ժ���Ҫ
 * */
typedef struct
{
	FLADDR    addr  ;
	uint8_t   offset;  //bit7:1---buff has value,0--- buff do't have value,bit0~bit6:replace the flash page index
	uint8_t   value;
}FLASH_DATA_STRUCT_T;

typedef struct
{
	uint8_t num;		//ʵ��������ݵĸ���
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
