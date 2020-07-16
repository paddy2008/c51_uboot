################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
A51_UPPER_SRCS += \
../src/SILABS_STARTUP.A51 

C_SRCS += \
../src/Efm8lb12_adc.c \
../src/Efm8lb12_cmd.c \
../src/Efm8lb12_flash.c \
../src/Efm8lb12_i2c_master.c \
../src/Efm8lb12_pid.c \
../src/Efm8lb12_port.c \
../src/Efm8lb12_qsfp28.c \
../src/Efm8lb12_timer.c \
../src/Efm8lb12_uart.c \
../src/c51_uboot_main.c 

OBJS += \
./src/Efm8lb12_adc.OBJ \
./src/Efm8lb12_cmd.OBJ \
./src/Efm8lb12_flash.OBJ \
./src/Efm8lb12_i2c_master.OBJ \
./src/Efm8lb12_pid.OBJ \
./src/Efm8lb12_port.OBJ \
./src/Efm8lb12_qsfp28.OBJ \
./src/Efm8lb12_timer.OBJ \
./src/Efm8lb12_uart.OBJ \
./src/SILABS_STARTUP.OBJ \
./src/c51_uboot_main.OBJ 


# Each subdirectory must supply rules for building sources it contributes
src/%.OBJ: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Keil 8051 Compiler'
	C51 "@$(patsubst %.OBJ,%.__i,$@)" || $(RC)
	@echo 'Finished building: $<'
	@echo ' '

src/%.OBJ: ../src/%.A51
	@echo 'Building file: $<'
	@echo 'Invoking: Keil 8051 Assembler'
	AX51 "@$(patsubst %.OBJ,%.__ia,$@)" || $(RC)
	@echo 'Finished building: $<'
	@echo ' '


