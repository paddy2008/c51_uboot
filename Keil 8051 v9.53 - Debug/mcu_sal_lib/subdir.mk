################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../mcu_sal_lib/mcu_sal.c \
../mcu_sal_lib/mcu_sal_adc.c \
../mcu_sal_lib/mcu_sal_dbg_cmd.c \
../mcu_sal_lib/mcu_sal_flash.c \
../mcu_sal_lib/mcu_sal_i2c_master.c \
../mcu_sal_lib/mcu_sal_i2c_slave.c \
../mcu_sal_lib/mcu_sal_log.c \
../mcu_sal_lib/mcu_sal_mdio.c \
../mcu_sal_lib/mcu_sal_page.c \
../mcu_sal_lib/mcu_sal_pid.c \
../mcu_sal_lib/mcu_sal_reg_map.c \
../mcu_sal_lib/mcu_sal_task.c \
../mcu_sal_lib/mcu_sal_terminal.c \
../mcu_sal_lib/mcu_sal_timer.c \
../mcu_sal_lib/mcu_sal_ver.c 

OBJS += \
./mcu_sal_lib/mcu_sal.OBJ \
./mcu_sal_lib/mcu_sal_adc.OBJ \
./mcu_sal_lib/mcu_sal_dbg_cmd.OBJ \
./mcu_sal_lib/mcu_sal_flash.OBJ \
./mcu_sal_lib/mcu_sal_i2c_master.OBJ \
./mcu_sal_lib/mcu_sal_i2c_slave.OBJ \
./mcu_sal_lib/mcu_sal_log.OBJ \
./mcu_sal_lib/mcu_sal_mdio.OBJ \
./mcu_sal_lib/mcu_sal_page.OBJ \
./mcu_sal_lib/mcu_sal_pid.OBJ \
./mcu_sal_lib/mcu_sal_reg_map.OBJ \
./mcu_sal_lib/mcu_sal_task.OBJ \
./mcu_sal_lib/mcu_sal_terminal.OBJ \
./mcu_sal_lib/mcu_sal_timer.OBJ \
./mcu_sal_lib/mcu_sal_ver.OBJ 


# Each subdirectory must supply rules for building sources it contributes
mcu_sal_lib/%.OBJ: ../mcu_sal_lib/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Keil 8051 Compiler'
	C51 "@$(patsubst %.OBJ,%.__i,$@)" || $(RC)
	@echo 'Finished building: $<'
	@echo ' '


