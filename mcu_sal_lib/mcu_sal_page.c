#include "mcu_sal.h"

#ifdef MCU_APP_PAGE_MODULE_ENABLED

static PAGES_LIST_T *page_list = NULL;

void app_page_list_init(PAGES_LIST_T *list)
{
	uint8_t  i = 0;

	ASSERT(list);

	page_list = list;

	for(i = 0; i < APP_PAGE_MAX_NUM; i++)
	{
		page_list->pages[i] = NULL;
	}
	page_list->num = 0;
}

/*
 * 该函数的作用仅仅是将当前分配的page加入到page_list中
 * 并未对当前的page做任何初始化动作，如果要做初始化，必须将对应的
 * 参数传入进来，这样函数的参数就高达3,4个，参数太多，如果结构体增加
 * 成员，那么参数会越变越多，扩展性降低，所以page的初始化在该函数外面
 * 做处理
 */
void app_page_add(APP_PAGE_STRUCT_T *page)
{
	ASSERT(page);
	ASSERT(page_list);

	memset((void *)&page->ram[0], 0 , APP_PAGE_RAM_SIZE);

	if(page_list->num < APP_PAGE_MAX_NUM)
	{
		page_list->pages[page_list->num] = page;
	}
	else
	{
		//lib_log("\r\n app_page_add failed");
		return;
	}

	page_list->num ++;
}

static uint8_t _app_page_get_index(uint8_t page_num)
{
	uint8_t i = 0;

	for(i = 0; i < page_list->num; i ++)
	{
		if(page_list->pages[i]->num == page_num)
		{
			return i;
		}
	}
	//lib_log("\r\n _app_page_get_index failed");
	return RET_FAILED;
}

uint8_t app_page_read_byte(uint8_t page_num, const uint8_t offset)
{
    uint8_t ret = 0, index = 0;

	RE_ASSERT(page_list);

    if(RET_FAILED != (index = _app_page_get_index(page_num)))
    {
        return page_list->pages[index]->ram[OFFSET_INDEX(offset)];
    }

    //lib_log("\r\n app_page_read_byte failed");

    return RET_FAILED;
}

APP_PAGE_STRUCT_T * app_page_get_base_addr(uint8_t page_num)
{
	uint8_t ret = 0, index = 0;

	RE_ASSERT(page_list);

	if(RET_FAILED != (index = _app_page_get_index(page_num)))
	{
		return page_list->pages[index];
	}

	//lib_log("\r\n app_page_get_base_addr failed");

	return NULL;
}

void app_page_write_byte(uint8_t page_num, const uint8_t offset, uint8_t value)
{
    uint8_t ret = 0, index = 0;

	ASSERT(page_list);

	if(RET_FAILED != (index = _app_page_get_index(page_num)))
	{
		page_list->pages[index]->ram[OFFSET_INDEX(offset)] = value;
	}
}

#endif
