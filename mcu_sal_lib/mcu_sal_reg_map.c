#include "mcu_sal.h"

#ifdef MCU_REG_MAP_MODULE_ENABLED

static REG_MAP_LIST_T *_map_list = NULL;

void reg_map_list_init(REG_MAP_LIST_T *list)
{
	uint8_t i = 0;

	ASSERT(list);

	_map_list = list;

	_map_list->map = NULL;
	_map_list->num = 0;
}

void reg_map_init(REG_MAP_STRUCT_T *reg_map)
{
	uint8_t i = 0;

	_map_list->map = reg_map;

	for(i = 0; i < REG_MAP_NUM; i ++)
	{
		_map_list->map[i].name = NULL;
		_map_list->map[i].page = 0;
		_map_list->map[i].reg_value = 0;
	}
}

static MCU_BIT_WIDTH_T reg_get_value_by_name(const char *name)
{
	uint16_t i = 0;

	RE_ASSERT(_map_list);

	for(i = 0; i < _map_list->num; i++)
	{
		if(!strcmp(name,_map_list->map[i].name))
		{
			return _map_list->map[i].reg_value;
		}
	}

	return RET_FAILED;
}

static void reg_set_value_by_name(const char *name, MCU_BIT_WIDTH_T reg_value)
{
	uint16_t i = 0;

	ASSERT(_map_list);

	for(i = 0; i < _map_list->num; i++)
	{
		if(!strcmp(name,_map_list->map[i].name))
		{
			_map_list->map[i].reg_value = reg_value;
		}
	}
}

static uint8_t reg_get_page_by_name(const char *name)
{
	uint16_t i = 0;

	RE_ASSERT(_map_list);

	for(i = 0; i < _map_list->num; i++)
	{
		if(!strcmp(name,_map_list->map[i].name))
		{
			return _map_list->map[i].page;
		}
	}

	return RET_FAILED;
}

static bool reg_check_map_table_exist(const char *name)
{
	uint16_t i = 0;

	RE_ASSERT(_map_list);

	for(i = 0; i < _map_list->num; i++)
	{
		if(!strcmp(name,_map_list->map[i].name))
		{
			return true;
		}
	}

	return false;
}


MCU_BIT_WIDTH_T reg_read(char* name)
{
	MCU_BIT_WIDTH_T ret = 0, save_page = 0;

	SET_PAGE(reg_get_page_by_name(name), save_page);

	ret = reg_get_value_by_name(name);

	RESTORE_PAGE(save_page);

	return ret;
}

MCU_BIT_WIDTH_T reg_write(char* name, MCU_BIT_WIDTH_T reg_value)
{
	MCU_BIT_WIDTH_T ret = 0, save_page = 0;

	SET_PAGE(reg_get_page_by_name(name), save_page);

	reg_set_value_by_name(name, reg_value);

	RESTORE_PAGE(save_page);

	return ret;
}

static void reg_add_map_table(char *name, MCU_BIT_WIDTH_T reg_value, uint8_t page)
{
	ASSERT(_map_list);

	if(_map_list->num < REG_MAP_NUM)
	{
		if(reg_check_map_table_exist(name) == false)
		{
			_map_list->map[_map_list->num].name 	   = name;
			_map_list->map[_map_list->num].reg_value   = reg_value;
			_map_list->map[_map_list->num].page		   = page;
			_map_list->num ++;
		}
	}
	else
	{
		lib_log("\r\n reg_add_map_table failed");
	}
}

void reg_add_show(char *name, MCU_BIT_WIDTH_T reg_value, uint8_t page)
{
	uint8_t width = sizeof(MCU_BIT_WIDTH_T), i = 0;

	reg_add_map_table(name, reg_value, page);

	if(width == 1)
	{
		lib_log("\r\n%s\t = 0x%bx", name, reg_read(name));
	}
	else
	{
		lib_log("\r\n%s\t = %x", name, reg_read(name));
	}
}

#endif
