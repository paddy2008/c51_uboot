#ifndef __MCU_SAL_PAGE_H__
#define __MCU_SAL_PAGE_H__

#include "mcu_sal_def.h"

#ifdef MCU_APP_PAGE_MODULE_ENABLED

typedef struct
{
	uint8_t                   ram[APP_PAGE_RAM_SIZE];
	uint8_t	code             *page_flash_base;	// MSA_FLASH_ADDR_PAGE
	uint8_t 				  num;				//Ò³Âë

	// indicates read-only or read-write byte
	// 0: read-only
	// 1: read-write
	uint8_t                    *writable_mask;
}APP_PAGE_STRUCT_T;

typedef struct
{
	uint8_t 		    num;
	APP_PAGE_STRUCT_T  *pages[APP_PAGE_MAX_NUM];
}PAGES_LIST_T;

void 	app_page_list_init(PAGES_LIST_T *list);

void 	app_page_add(APP_PAGE_STRUCT_T *page);

void 	app_page_write_byte(uint8_t page_num, const uint8_t offset, uint8_t value);

uint8_t app_page_read_byte(uint8_t page_num, const uint8_t offset);

APP_PAGE_STRUCT_T *app_page_get_base_addr(uint8_t page_num);

#endif

#endif
