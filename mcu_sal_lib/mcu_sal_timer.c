#include "mcu_sal.h"

#ifdef MCU_TIMER_MODULE_ENABLED

static TIMER_LSIT_T *timer_list = NULL;

/*
 * when timer_list->jiffies is setted 0, there must be init timer_list->timer[i]->expires again
 */
static void _timer_jiffies_zero_by_isr(uint32_t timeout)
{
	uint8_t i = 0;

	ASSERT(timer_list);

	//JIFFIES_MAX - (timeout + timer_list->jiffies) > timeout;
	if(JIFFIES_MAX - timer_list->jiffies < 2 * timeout)
	{
		timer_list->jiffies = 0;

		for(i = 0; i < timer_list->num; i++)
		{
			if(timer_list->timer[i]->function != NULL)
			{
				timer_list->timer[i]->expires = timer_list->timer[i]->timeout;
			}
		}
	}
}

/*
 * when timer_list->jiffies is setted 0, there must be init timer_list->timer[i]->expires again
 */
static void _timer_jiffies_zero(uint32_t timeout)
{
	uint8_t i = 0;

	ASSERT(timer_list);

	//JIFFIES_MAX - (timeout + timer_list->jiffies) > timeout;
	if(JIFFIES_MAX - timer_list->jiffies < 2 * timeout)
	{
		timer_list->jiffies = 0;

		for(i = 0; i < timer_list->num; i++)
		{
			if(timer_list->timer[i]->function != NULL)
				timer_list->timer[i]->expires = timer_list->timer[i]->timeout; //because timer_list->jiffies = 0, timer_list->timer[i]->expires = timer_list->timer[i]->timeout + 0(timer_list->jiffies)
		}
	}
}


/*
 * save new tiner struct, there must be expires = jiffts + varible, because when jiffts = 0, expires is also changed
 */
void timer_add(TIMER_STRUCT_T *timer)
{
	ASSERT(timer);

	if(timer_list->num < TIMER_NUM)
	{
		timer_list->timer[timer_list->num] = timer;
	}
	else
	{
		return;
	}

	timer_list->timer[timer_list->num]->enable	     = 0;     //add new timer default state is disable
	timer_list->timer[timer_list->num]->timeout_flag = false;

	_timer_jiffies_zero(timer->timeout);
	timer_list->timer[timer_list->num]->expires  = timer->timeout + timer_list->jiffies;

	timer_list->num ++;
}

/*
 * modify the software timer timeout time
 */
static void _timer_mod(TIMER_STRUCT_T * timer, uint32_t timeout)
{
	ASSERT(timer);

	_timer_jiffies_zero_by_isr(timeout);

	timer->timeout  = timeout;
	timer->expires  = timeout + timer_list->jiffies;
}

void timer_list_init(TIMER_LSIT_T *list)
{
	uint8_t i = 0;

	ASSERT(list);

	timer_list = list;

	for(i = 0; i < TIMER_NUM; i++)
	{
		timer_list->timer[i] = NULL;
	}

	timer_list->num 	   = 0;
	timer_list->jiffies    = 0;
}

/*
 * start soft timer
 */
void timer_start(TIMER_STRUCT_T * timer)
{
	ASSERT(timer);
	timer->enable = 1;
}

/*
 * stop soft timer
 */
void timer_stop(TIMER_STRUCT_T * timer)
{
	ASSERT(timer);
	timer->enable = 0;
}

/*max delay 65¡£535s*/
void timer_delay_ms(uint16_t ms)
{
	uint32_t current_time = 0;
	uint8_t i = 0, timer_enable_save[TIMER_NUM];

	ASSERT(timer_list);

    _timer_jiffies_zero(ms);

    //disable all timer_list, in order to prevent current_time = JIFFIES_MAX, timer_list->jiffies is setted 0 right now
    for(i = 0; (timer_list->timer[i] != NULL) && (i < timer_list->num); i++)
    {
    	timer_enable_save[i] = timer_list->timer[i]->enable;
    	timer_list->timer[i]->enable = 0;
    }

	current_time = timer_list->jiffies;
    while(timer_list->jiffies <  ms + current_time);

    //restore all timer_list status
    for(i = 0; (timer_list->timer[i] != NULL) && (i < timer_list->num); i++)
    {
		timer_list->timer[i]->enable = timer_enable_save[i];
    }
}

void timer_run(void)
{
	uint8_t i = 0;

	ASSERT(timer_list);

	for(i = 0; (timer_list->timer[i] != NULL) && (i < timer_list->num); i++)
	{
		if(timer_list->timer[i]->timeout_flag)
		{
			timer_list->timer[i]->function();
			timer_list->timer[i]->timeout_flag = false;
		}
	}
}

uint32_t timer_get_tick(void)
{
	RE_ASSERT(timer_list);
	return timer_list->jiffies;
}

void timer_update(void)
{
	uint8_t i = 0;

	ASSERT(timer_list);

	for(i = 0; (timer_list->timer[i] != NULL) && (i < timer_list->num); i++)
	{
		if((timer_list->timer[i]->enable) && (timer_list->timer[i]->expires <= timer_list->jiffies))
		{
			timer_list->timer[i]->timeout_flag = true;
			_timer_mod(timer_list->timer[i], timer_list->timer[i]->timeout); //update timeout time
		}
	}

	timer_list->jiffies ++;
}

#endif



