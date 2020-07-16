#include "mcu_sal.h"

void mcu_sal_init()
{
#if defined(MCU_TASK_MODULE_ENABLED)
	static TASK_LIST_T	       task_list;   //create task scheduler manager list
#endif

#if defined(MCU_TERMINAL_MODULE_ENABLED)
	static TERMINAL_STRUCT_T   terminal;    //create terminal handle, include UART, USB, I2CµÈ
#endif

#if defined(MCU_TIMER_MODULE_ENABLED)
	static TIMER_LSIT_T    	   timer_list;   //create software timer list
#endif

#if defined(MCU_ADC_MODULE_ENABLED)
	static ADC_LIST_T  			adc_list;
#endif

#if defined(MCU_FLASH_BUFF_MODULE_ENABLED)
	static FLASH_BUFF_STRUCT_T  flash_buff;
#endif

#if defined(MCU_REG_MAP_MODULE_ENABLED)
	static REG_MAP_LIST_T     	reg_map_list;
#endif

#if defined(MCU_LOG_MODULE_ENABLED) && (LOG_WAY == 0)
	static uint8_t   		   	log_buff[LOG_RAM_SIZE];   //create log buff
#endif

#if defined(MCU_APP_PAGE_MODULE_ENABLED)
	static PAGES_LIST_T     	app_page_list;
#endif

#if defined(MCU_PID_MODULE_ENABLED)
	static PID_STRUCT_T  		pid;
#endif


#if defined(MCU_TASK_MODULE_ENABLED)
	task_list_init(&task_list);
#endif

#if defined(MCU_TERMINAL_MODULE_ENABLED)
	lib_terminal_init();  //hardware UART INIT
	terminal_init(&terminal);
#endif

#if defined(MCU_TIMER_MODULE_ENABLED)
	lib_timer_init();  	  //hardware TIMER INIT
	timer_list_init(&timer_list);
#endif

#if defined(MCU_ADC_MODULE_ENABLED)
	lib_adc_init();  	//hardware ADC INIT
	adc_list_init(&adc_list);
#endif

#if defined(MCU_I2C_MASTER_MODULE_ENABLED)
	i2c_master_init();
#endif

#if defined(MCU_I2C_SLAVE_MODULE_ENABLED)
	i2c_slave_init();
#endif

#if defined(MCU_FLASH_API_MODULE_ENABLED)
#endif

#if defined(MCU_FLASH_BUFF_MODULE_ENABLED)
	flash_init(&flash_buff);
#endif

#if defined(MCU_REG_MAP_MODULE_ENABLED)
	reg_map_list_init(&reg_map_list);
#endif

#if defined(MCU_LOG_MODULE_ENABLED) && (LOG_WAY == 0)
	log_init(&log_buff[0]);
#endif

#if defined(MCU_APP_PAGE_MODULE_ENABLED)
	app_page_list_init(&app_page_list);
#endif

#if defined(MCU_DBG_CMD_MODULE_ENABLED)

#endif

#if defined(MCU_PID_MODULE_ENABLED)
	pid_init(&pid);
#endif

#if defined(MCU_MDIO_MODULE_ENABLED)
	mdio_init();
#endif
}

void mcu_sal_empty(char *fmt,...)
{
	fmt = NULL;
}
