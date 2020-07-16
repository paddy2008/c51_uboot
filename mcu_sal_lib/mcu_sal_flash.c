#include "mcu_sal.h"

#ifdef MCU_FLASH_API_MODULE_ENABLED

void flash_read (FLADDR addr, uint8_t *value, uint16_t len)
{
   uint16_t i = 0;

   for (i = 0; i < len; i++)
   {
      *value ++ = lib_flash_read_byte (addr + i);
   }
}

/*
 *the second parameter len valid range is 0 to FLASH_PAGE_SIZE
 *the  <from> + <len> must be less than the maximum flash address.
 */
static int8_t flash_erase_range(FLADDR from, uint16_t len)
{
	FLADDR	erase_start_addr = GET_FLASH_PAGE_START_ADDR(from);
	FLADDR 	erase_end_addr   = (FLADDR)(from + len) ,cur_addr = 0;
	uint16_t i = 0;
	uint8_t save_buf[FLASH_PAGE_SIZE] = {0};

	if((len > FLASH_PAGE_SIZE) || (erase_end_addr >  FLASH_MAX_ADDR))
	{
		lib_log("\r\n flash_erase_range failed");
		return -1;
	}

	flash_read(erase_start_addr, save_buf, FLASH_PAGE_SIZE);

	lib_flash_erase_page(erase_start_addr);

	for(i = 0; i < FLASH_PAGE_SIZE; i++)
	{
		cur_addr = erase_start_addr + i;

		if (cur_addr < from || cur_addr >= erase_end_addr)
		{
			lib_flash_write_byte(cur_addr, save_buf[i]);
		}
	}

	return 0;
}

int8_t flash_write(FLADDR addr, uint8_t *value, uint16_t len)
{
	uint16_t i = 0;

	if (flash_erase_range (addr, len) < 0)
	{
		lib_log("\r\n flash_write failed");
		return -1;
	}

	for (i = addr; i < addr + len; i++)
	{
		lib_flash_write_byte (i, *value ++);
	}

	return 0;
}

int8_t flash_update(FLADDR addr, uint8_t *value, uint16_t len)
{
	return flash_write(addr, value, len);
}

void flash_write_byte(FLADDR addr, uint8_t value)
{
	uint16_t page_start_addr = 0 , i = 0;
	uint8_t  save_buf[FLASH_PAGE_SIZE] = {0};
	uint8_t  cur_value = lib_flash_read_byte(addr);

	if(cur_value == value)  //if want to write value is equal to value info flash,there is not need to write it info flash
	{
		return;
	}
	else
	{
		if(cur_value != 0xFF)   //indicate this addr was writen value,so it must be erase page at first
		{
			page_start_addr = GET_FLASH_PAGE_START_ADDR(addr);

			flash_read(page_start_addr, save_buf, FLASH_PAGE_SIZE);

			lib_flash_erase_page(page_start_addr);

			if(addr - page_start_addr >= 0 && addr - page_start_addr < FLASH_PAGE_SIZE)
			{
				save_buf[addr - page_start_addr]  = value;
			}

			for(i = 0; i < FLASH_PAGE_SIZE; i++)
			{
				lib_flash_write_byte(page_start_addr + i, save_buf[i]);
			}
		}
		else  //indicate this addr was not writen value, it can be writen directly
		{
			lib_flash_write_byte(addr, value);
		}
	}
}

/*
void flash_write(FLADDR addr, uint8_t *value, uint16_t len)
{
	uint16_t page_start_addr = 0 , i = 0;
	uint8_t  save_buf[512] = {0}, cur_value = 0;
	uint8_t *ptr = value;

	bool isErasePage = false;

	for (i = 0; i < len; i++)
	{
		cur_value = lib_flash_read_byte(addr + i);

		if((cur_value != 0xFF) && (cur_value != *ptr++))
		{
			isErasePage = true;
			break;
		}
	}

	ptr = value;

	if(isErasePage)
	{
		page_start_addr = GET_FLASH_PAGE_START_ADDR(addr);

		flash_read(page_start_addr, save_buf, FLASH_PAGE_SIZE);

		lib_flash_erase_page(page_start_addr);

		for (i = addr; i < addr + len; i++)
		{
			if(((i - page_start_addr) >= 0) && ((i - page_start_addr) < FLASH_PAGE_SIZE))
			{
				save_buf[i - page_start_addr] = *ptr++;
			}
		}

		for(i = 0; i < FLASH_PAGE_SIZE; i++)
		{
			lib_flash_write_byte(page_start_addr + i, save_buf[i]);
		}
	}
	else
	{
		for (i = addr; i < addr + len; i++)
		{
			lib_flash_write_byte(i, *ptr++);
		}
	}
}
*/
#endif

#ifdef MCU_FLASH_BUFF_MODULE_ENABLED

/*
 * ���µ���������flash_init,flash_recv_byte��flash_write_run
 * ��ҪӦ����i2c master����i2c slave��д���ݵ�����£�����Ҫ��д�������
 * ���浽flash��,����flash_init�������ram�ռ䲢����ʼ����
 * flash_recv_byte����ͣ�Ľ������ݣ�flash_write_run����ͣ�Ľ�����
 * д�뵽flash�У�����flash_write_run������������ִ�У�
 */
static FLASH_BUFF_STRUCT_T *_flash_buff = NULL;

void flash_init(FLASH_BUFF_STRUCT_T *flash)
{
	ASSERT(flash);

	_flash_buff = flash;

	memset((void *)_flash_buff, 0 , sizeof(FLASH_BUFF_STRUCT_T));
}

/*
 * ����һ��ѭ��buff,��Ȼbuff�Ĵ�Сֻ��128�ֽڣ�����ʵ���Ͽ����������մ���128�ֽڵ�����
 * ������������:
 * 1.����������10���ֽ��Ժ�ǰ10��λ�ñ�ռ��
 * 2.�����յ�11���ֽڵ�ʱ�򣬲����Ƿŵ���11��λ�ã������ȼ��ǰ10��λ�����Ƿ��пռ䱻�ͷ�
 * ����У���д����Ӧ�Ŀռ䣬���û�У���д����11��λ��
 */
void flash_recv_byte(FLADDR addr, uint8_t offset, const uint8_t value)  //this function is called by i2c slave isr
{
	uint8_t index = 0, page = 0;

	ASSERT(_flash_buff);

	for(index = 0; index < FLASH_USER_PAGE_SIZE; index ++)
	{
		if(MY_GET_BIT(_flash_buff->buff[index].offset, 7) == 0x0) //This addr is empty
		{
			_flash_buff->buff[index].addr       = addr;
			_flash_buff->buff[index].value      = value;
			_flash_buff->buff[index].offset     = (offset | (1 << 7));
			_flash_buff->num ++;
			break;
		}
	}
}

void flash_write_run(void)
{
	FLADDR  addr = 0;
	uint8_t i = 0, page = 0;

	ASSERT(_flash_buff);

	if(_flash_buff->num == 0)
	{
		return;
	}

	for (i = 0; i < FLASH_USER_PAGE_SIZE, _flash_buff->num; i++)
	{
		if(MY_GET_BIT(_flash_buff->buff[i].offset, 7))  //if the buff is not empty
		{
			flash_write_byte((FLADDR)(_flash_buff->buff[i].addr  + OFFSET_INDEX(_flash_buff->buff[i].offset)), _flash_buff->buff[i].value);
			_flash_buff->num -- ;
			_flash_buff->buff[i].offset = MY_CLEAR_BIT(_flash_buff->buff[i].offset, 7);
		}
	}
}

#endif

