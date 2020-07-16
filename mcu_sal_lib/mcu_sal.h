#ifndef __MCU_SAL_H__
#define __MCU_SAL_H__

#include "mcu_sal_terminal.h"
#include "mcu_sal_adc.h"
#include "mcu_sal_dbg_cmd.h"
#include "mcu_sal_flash.h"
#include "mcu_sal_hw_conf.h"
#include "mcu_sal_i2c_master.h"
#include "mcu_sal_i2c_slave.h"
#include "mcu_sal_log.h"
#include "mcu_sal_mdio.h"
#include "mcu_sal_page.h"
#include "mcu_sal_pid.h"
#include "mcu_sal_reg_map.h"
#include "mcu_sal_task.h"
#include "mcu_sal_terminal.h"
#include "mcu_sal_timer.h"
#include "mcu_sal_ver.h"

extern void mcu_sal_init();

extern void mcu_sal_empty(char *fmt,...);

#if LOG_WAY == 0
	#ifdef MCU_LOG_MODULE_ENABLED
		#define lib_log  log_save
	#else
		#define lib_log  mcu_sal_empty
	#endif
#elif  LOG_WAY == 1
	#ifdef MCU_TERMINAL_MODULE_ENABLED
		#define lib_log  printf
	#else
		#define lib_log  mcu_sal_empty
	#endif
#endif

#endif
