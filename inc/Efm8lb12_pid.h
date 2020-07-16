#ifndef __Efm8lb12_pid_h__
#define __Efm8lb12_pid_h__

#include <SI_EFM8LB1_Register_Enums.h>

#include "mcu_sal.h"

void pid_hw_init(PID_STRUCT_T *pid, PID_CONF_PARA *pid_conf);
void pid_hw_run();

#endif

