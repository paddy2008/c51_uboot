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

/*��ȫ���ж�*/
#define ENTER_CRITICAL()			{IE_EA = 0;}

/*��ȫ���ж�*/
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

/*�ն�һ����������յ���ַ���*/
#define  TERMINAL_BUFF_LEN 	    	(100)

/*�ն��������������������*/
#define  CLI_CMD_NUM				(100)

/*�ն˱�����ʷ�����������*/
#define  CLI_HISTORY_CMD_NUM		(5)

/*�ն����������Կո�ֿ���ÿ���ֿ����ַ��������ĳ��� */
#define  CLI_STR_SECTION_LEN		(20)

/*�ն˹��˵���#�ַ���ͷ���ַ���������ע��*/
#define  SKIP_CHAR  				'#'

/*�ն˻س�����ʾ����ʾ��*/
#define  PROMPT   					"\r\n#McuTestBoard:"

/*�ն˿�������������ַ�*/
#define  CHECK_CHAR_VALID(ch) 		(((ch) >= '0' && ((ch) <= '9')) || ((ch) >= 'a' && ((ch) <= 'z')) || ((ch) >= 'A' && ((ch) <= 'Z')) || ((ch) == ' ') || ((ch) == '#') ||((ch) == '_'))

#define  lib_putchar 	 	 		uart_putchar
#define  lib_put_string  	 		uart_put_string
#define  lib_terminal_init	 		uart_init
#define  printf	 	 	 	 		uart_printf
#endif
/************************************************************************************************************************/

#ifdef MCU_TIMER_MODULE_ENABLED
#include "Efm8lb12_timer.h"

/*����ӵ���������ʱ������*/
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

/*��������ӵ�ADC PIN��������*/
#define  ADC_MAX_NUM    					(20)

#define  lib_adc_init						adc0_hw_init
#define  lib_adc_do_measure					adc0_do_measure
#endif
/************************************************************************************************************************/

#ifdef MCU_I2C_MASTER_MODULE_ENABLED
#include "Efm8lb12_i2c_master.h"

/*����i2c master��SDA,SCL pin*/
SI_SBIT (SDA, SFR_P1, 2);                 // i2c master on P1.2
SI_SBIT (SCL, SFR_P1, 3);                 // i2c master on P1.3

/*����Ҫʹ��i2c master��ģʽ, 0x1:ʹ��Ӳ���ӿ�i2c master�� 0x0:ʹ��GPIO��ģ��ʵ��i2c master*/
#define  I2C_MASTER_MODE 		    	(0x0)

/*����i2c master��ʱ��Ƶ��*/
#define  I2C_MASTER_DEF_FREQUENCY  		(200)  //uint: khz, i2c master clock

/*i2c master�Զ˴��豸���豸��ַ */
#define  I2C_DEVICE_ADDR				(0xA0)
#define  I2C_DEVICE_ADDR1				(0xA2)

/*����i2c master�Զ˴��豸��ƫ�Ƶ�ַ��ȴ���8bit���豸��ַ����*/
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

/*����i2c slave�Ĵ��豸��ַ*/
#define I2C_SLAVE_A2_ADDR    	  (0x51)	//0xA2
#define I2C_SLAVE_A0_ADDR    	  (0x50)	//0xA0

/*i2c slaveһ��������յ�����ֽ���*/
#define I2C_WRITE_SIZE_ONCE   	  (8)

/*i2c slave �����ֽڼĴ���*/
#define RECV_DATA_REG			  I2C0DIN

/*i2c slave �����ֽڼĴ���*/
#define SEND_DATA_REG			  I2C0DOUT

#define lib_i2c_slave_enable      i2c_slave_hw_enable
#define lib_i2c_slave_init	      i2c_slave_hw_init
#endif
/************************************************************************************************************************/

#if defined(MCU_FLASH_API_MODULE_ENABLED) || defined(MCU_FLASH_BUFF_MODULE_ENABLED)

/*����flash����������*/
typedef  uint16_t FLADDR;

/*����Ӳ��flash��ҳ��С����ǰ��ҳ��С��512�ֽ�*/
#define  FLASH_PAGE_SIZE  ((uint16_t)0x200)

/*����Ӳ��flash���û����õ�����ַ*/
#define  FLASH_MAX_ADDR	  ((uint16_t)0xF9FF)
#endif

#ifdef MCU_FLASH_API_MODULE_ENABLED
#include "Efm8lb12_flash.h"

#define lib_flash_erase_page					flash_hw_erase_page
#define lib_flash_write_byte					flash_hw_write_byte
#define lib_flash_read_byte						flash_hw_read_byte

#endif

#ifdef MCU_FLASH_BUFF_MODULE_ENABLED

/*�����û��Զ���flashҳ��С*/
#define  FLASH_USER_PAGE_SIZE		(128)
#endif
/************************************************************************************************************************/

#ifdef MCU_REG_MAP_MODULE_ENABLED

/*��������reg map��������*/
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

/*����log buff�Ĵ�С*/
#define LOG_RAM_SIZE 				(128)


#endif
/************************************************************************************************************************/

#ifdef MCU_APP_PAGE_MODULE_ENABLED
/*�������ӵ��û�ram page��������*/
#define  APP_PAGE_MAX_NUM 	 		(10)

/*�û��Զ���page��ҳ��С*/
#define  APP_PAGE_RAM_SIZE   		(128)
#endif
/************************************************************************************************************************/

#ifdef MCU_DBG_CMD_MODULE_ENABLED

/*�û��Զ���ҳѡ���ַ*/
#define  PAGE_SELECT_BYTE      			 (127)

/*debug buffһ�ο��Դ��������ֽ�*/
#define  DEBUG_CMD_BUFF_SIZE			 (16)

/*debug buff������page�е�ƫ�Ƶ�ַ*/
#define  DEBUG_CMD_START_ADDR 	 		 (0xDA)
#endif

/************************************************************************************************************************/

#ifdef MCU_PID_MODULE_ENABLED

/*PID�㷨���ֵ������ֵ����С����λ��Ӧ�ö���������ʹ�õ���0.1mv*/
#define PID_OUTPUT_ERROR			(10)

/*PID�㷨���ֵ������ֵ���������λ��Ӧ�ö���������ʹ�õ���0.1mv*/
#define PID_OUTPUT_MAX_ERROR		(6000)

/*PID����������*/
#define PID_CONF_PARA_GAIN			(1000.0)

/*PIDƽ���Ķ���*/
#define PID_BALANCE_POINT   		(12500)
#endif

/************************************************************************************************************************/

#ifdef MCU_TASK_MODULE_ENABLED

/*��������ӵ�����������*/
#define TASK_NUM  					(20)
#endif

/************************************************************************************************************************/

#ifdef MCU_MDIO_MODULE_ENABLED

/*����MDIO��MDIO,MDC pin*/
SI_SBIT (MDIO, SFR_P2, 1);                 // MDIO on P2.1
SI_SBIT (MDC, SFR_P2, 2);                  // MDC on P2.2
SI_SBIT (DELAY_PIN, SFR_P2, 7);            // delay

#include "Efm8lb12_port.h"
#define lib_mdio_output     	 port_mido_output
#define lib_mdio_input     		 port_mido_input
#endif

/************************************************************************************************************************/

#endif

