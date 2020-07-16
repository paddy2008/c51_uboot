#ifndef __MCU_SAL_HW_CONF_H__
#define __MCU_SAL_HW_CONF_H__

#include <stdint.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include "dac_3.h"

//Global Macro define

/*System clock frequency in Hz*/
#define  SYSCLK             		(72000000)

/*SET MCU REGISTER PAGE*/
#define  SET_PAGE(p,q)   			do {q = SFRPAGE;SFRPAGE = (p);} while(0)

/*RESTORE MCU REGISTER PAGE*/
#define  RESTORE_PAGE(q) 			do {SFRPAGE = (q);} while(0)

/*关全局中断*/
#define ENTER_CRITICAL()			{IE_EA = 0;}

/*打开全局中断*/
#define EXIT_CRITICAL()				{IE_EA = 1;}

#define MCU_ALL_MODULES_ENABLED
#define MCU_TASK_MODULE_ENABLED
#define MCU_TERMINAL_MODULE_ENABLED
#define MCU_TIMER_MODULE_ENABLED
#define MCU_ADC_MODULE_ENABLED
#define MCU_I2C_MASTER_MODULE_ENABLED
//#define MCU_I2C_SLAVE_MODULE_ENABLED
#define MCU_FLASH_API_MODULE_ENABLED
#define MCU_FLASH_BUFF_MODULE_ENABLED
#define MCU_REG_MAP_MODULE_ENABLED
#define MCU_LOG_MODULE_ENABLED
#define MCU_APP_PAGE_MODULE_ENABLED
#define MCU_DBG_CMD_MODULE_ENABLED
#define MCU_PID_MODULE_ENABLED
#define MCU_MDIO_MODULE_ENABLED

/************************************************************************************************************************/
#ifdef MCU_TERMINAL_MODULE_ENABLED
#include "Efm8lb12_uart.h"

/*终端一次性允许接收的最长字符数*/
#define  TERMINAL_BUFF_LEN 	    	(100)

/*终端允许可添加命令的最大个数*/
#define  CLI_CMD_NUM				(100)

/*终端保存历史命令的最大个数*/
#define  CLI_HISTORY_CMD_NUM		(5)

/*终端输入命令以空格分开，每个分开的字符串段最大的长度 */
#define  CLI_STR_SECTION_LEN		(20)

/*终端过滤掉以#字符开头的字符串，方便注释*/
#define  SKIP_CHAR  				'#'

/*终端回车后显示的提示符*/
#define  PROMPT   					"\r\n#McuTestBoard:"

/*终端可以允许输入的字符*/
#define  CHECK_CHAR_VALID(ch) 		(((ch) >= '0' && ((ch) <= '9')) || ((ch) >= 'a' && ((ch) <= 'z')) || ((ch) >= 'A' && ((ch) <= 'Z')) || ((ch) == ' ') || ((ch) == '#') ||((ch) == '_'))

#define  lib_putchar 	 	 		uart_putchar
#define  lib_put_string  	 		uart_put_string
#define  lib_terminal_init	 		uart_init
#define  printf	 	 	 	 		uart_printf
#endif
/************************************************************************************************************************/

#ifdef MCU_TIMER_MODULE_ENABLED
#include "Efm8lb12_timer.h"

/*可添加的最大软件定时器个数*/
#define TIMER_NUM							(10)

#define lib_wdt_disable  	 	    		timer_hw_wdt_disable
#define lib_wdt_init     	 	    		timer_hw_wdt_init
#define lib_timer_init   	 	 			timer_hw_init
#define lib_i2c_master_timer_init  			timer0_hw_init    //this timer is used for i2c master clock source
#define lib_uart_timer_init  	     		timer1_hw_init    //this timer is used for uart0 clock source
#define lib_delay_us      					timer5_hw_delay_us
#endif
/************************************************************************************************************************/

#ifdef MCU_ADC_MODULE_ENABLED
#include "Efm8lb12_adc.h"

/*可允许添加的ADC PIN的最大个数*/
#define  ADC_MAX_NUM    					(20)

#define  lib_adc_init						adc0_hw_init
#define  lib_adc_do_measure					adc0_do_measure
#endif
/************************************************************************************************************************/

#ifdef MCU_I2C_MASTER_MODULE_ENABLED
#include "Efm8lb12_i2c_master.h"

/*定义i2c master的SDA,SCL pin*/
SI_SBIT (SDA, SFR_P1, 2);                 // i2c master on P1.2
SI_SBIT (SCL, SFR_P1, 3);                 // i2c master on P1.3

/*定义要使用i2c master的模式, 0x1:使用硬件接口i2c master， 0x0:使用GPIO口模拟实现i2c master*/
#define  I2C_MASTER_MODE 		    	(0x0)

/*定义i2c master的时钟频率*/
#define  I2C_MASTER_DEF_FREQUENCY  		(200)  //uint: khz, i2c master clock

/*i2c master对端从设备的设备地址 */
#define  I2C_DEVICE_ADDR				(0xA0)
#define  I2C_DEVICE_ADDR1				(0xA2)

/*定义i2c master对端从设备的偏移地址宽度大于8bit的设备地址定义*/
#define I2C_SLAVE_DEV1_ADDR				(0x24)
#define I2C_SLAVE_DEV2_ADDR				(0xA8)
#define I2C_SLAVE_DEV3_ADDR				(0xB8)

#define lib_i2c_master_reset     		i2c_master_hw_reset
#define lib_i2c_master_init	     		i2c_master_hw_init
#define lib_i2c_master_set_freq  		i2c_master_hw_set_freq
#define lib_i2c_master_get_freq  		i2c_master_hw_get_freq
#define lib_i2c_master_read  	 		i2c_master_hw_read
#define lib_i2c_master_write     		i2c_master_hw_write
#endif
/************************************************************************************************************************/

#ifdef MCU_I2C_SLAVE_MODULE_ENABLED

/*定义i2c slave的从设备地址*/
#define I2C_SLAVE_A2_ADDR    	  (0x51)	//0xA2
#define I2C_SLAVE_A0_ADDR    	  (0x50)	//0xA0

/*i2c slave一次允许接收的最大字节数*/
#define I2C_WRITE_SIZE_ONCE   	  (8)

/*i2c slave 接收字节寄存器*/
#define RECV_DATA_REG			  I2C0DIN

/*i2c slave 发送字节寄存器*/
#define SEND_DATA_REG			  I2C0DOUT

#define lib_i2c_slave_enable      i2c_slave_hw_enable
#define lib_i2c_slave_init	      i2c_slave_hw_init
#endif
/************************************************************************************************************************/

#if defined(MCU_FLASH_API_MODULE_ENABLED) || defined(MCU_FLASH_BUFF_MODULE_ENABLED)

/*定义flash变量的类型*/
typedef  uint16_t FLADDR;

/*定义硬件flash的页大小，当前是页大小是512字节*/
#define  FLASH_PAGE_SIZE  ((uint16_t)0x200)

/*定义硬件flash的用户可用的最大地址*/
#define  FLASH_MAX_ADDR	  ((uint16_t)0xF9FF)
#endif

#ifdef MCU_FLASH_API_MODULE_ENABLED
#include "Efm8lb12_flash.h"

#define lib_flash_erase_page					flash_hw_erase_page
#define lib_flash_write_byte					flash_hw_write_byte
#define lib_flash_read_byte						flash_hw_read_byte

#endif

#ifdef MCU_FLASH_BUFF_MODULE_ENABLED

/*定义用户自定义flash页大小*/
#define  FLASH_USER_PAGE_SIZE		(128)
#endif
/************************************************************************************************************************/

#ifdef MCU_REG_MAP_MODULE_ENABLED

/*定义可添加reg map的最大个数*/
#define REG_MAP_NUM					(20)
#endif
/************************************************************************************************************************/

#ifdef MCU_LOG_MODULE_ENABLED
/*
 * 0		   : save log into ram
 * 1		   : UART output to terminal
 * other value : empty function and do nothing
 */
#define LOG_WAY	  (0)

/*定义log buff的大小*/
#define LOG_RAM_SIZE 				(128)


#endif
/************************************************************************************************************************/

#ifdef MCU_APP_PAGE_MODULE_ENABLED
/*定义可添加的用户ram page的最大个数*/
#define  APP_PAGE_MAX_NUM 	 		(10)

/*用户自定义page的页大小*/
#define  APP_PAGE_RAM_SIZE   		(128)
#endif
/************************************************************************************************************************/

#ifdef MCU_DBG_CMD_MODULE_ENABLED

/*用户自定义页选择地址*/
#define  PAGE_SELECT_BYTE      			 (127)

/*debug buff一次可以处理的最大字节*/
#define  DEBUG_CMD_BUFF_SIZE			 (16)

/*debug buff在整个page中的偏移地址*/
#define  DEBUG_CMD_START_ADDR 	 		 (0xDA)
#endif

/************************************************************************************************************************/

#ifdef MCU_PID_MODULE_ENABLED

/*PID算法输出值和期望值的最小误差，单位视应用而定，这里使用的是0.1mv*/
#define PID_OUTPUT_ERROR			(10)

/*PID算法输出值和期望值的最大误差，单位视应用而定，这里使用的是0.1mv*/
#define PID_OUTPUT_MAX_ERROR		(6000)

/*PID参数的增益*/
#define PID_CONF_PARA_GAIN			(1000.0)

/*PID平衡点的定义*/
#define PID_BALANCE_POINT   		(12500)
#endif

/************************************************************************************************************************/

#ifdef MCU_TASK_MODULE_ENABLED

/*可允许添加的最大任务个数*/
#define TASK_NUM  					(20)
#endif

/************************************************************************************************************************/

#ifdef MCU_MDIO_MODULE_ENABLED

/*定义MDIO的MDIO,MDC pin*/
SI_SBIT (MDIO, SFR_P2, 1);                 // MDIO on P2.1
SI_SBIT (MDC, SFR_P2, 2);                  // MDC on P2.2
SI_SBIT (DELAY_PIN, SFR_P2, 7);            // delay

#include "Efm8lb12_port.h"
#define lib_mdio_output     	 port_mido_output
#define lib_mdio_input     		 port_mido_input
#endif

/************************************************************************************************************************/

#endif

