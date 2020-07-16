#ifndef __EFM8LB1_flash_h__
#define __EFM8LB1_flash_h__

#include <SI_EFM8LB1_Register_Enums.h>

typedef enum {
	MSA_FLASH_ADDR_PAGE_LOWER	= 0xF480, //0xEF40, page2 is reserved space for user
	MSA_FLASH_ADDR_PAGE_00H		= 0xEE00, //0xEE12, after 59k
	MSA_FLASH_ADDR_PAGE_01H		= 0xEE80, //0xEE92,
	MSA_FLASH_ADDR_PAGE_02H		= 0xEF00, //0xEF12,
	MSA_FLASH_ADDR_PAGE_03H		= 0xEF80, //0xEF92,
	MSA_FLASH_ADDR_PAGE_05H		= 0xF200,
	MSA_FLASH_ADDR_PAGE_06H		= 0xF280, //0xF000,
	MSA_FLASH_ADDR_PAGE_07H		= 0xF300, //0xF080,
	MSA_FLASH_ADDR_PAGE_08H 	= 0xF380, //0xF080,
	MSA_FLASH_ADDR_PAGE_09H 	= 0xF400, //0xF080,
}MSA_FLASH_ADDR_PAGE;

void    flash_hw_erase_page (uint16_t addr);
void    flash_hw_write_byte (uint16_t addr, uint8_t byte);
uint8_t flash_hw_read_byte (uint16_t addr);

#endif
