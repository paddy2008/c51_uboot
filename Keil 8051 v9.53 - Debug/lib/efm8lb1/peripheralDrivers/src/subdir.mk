################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../lib/efm8lb1/peripheralDrivers/src/adc_0.c \
../lib/efm8lb1/peripheralDrivers/src/dac_0.c \
../lib/efm8lb1/peripheralDrivers/src/dac_1.c \
../lib/efm8lb1/peripheralDrivers/src/dac_2.c \
../lib/efm8lb1/peripheralDrivers/src/dac_3.c \
../lib/efm8lb1/peripheralDrivers/src/wdt_0.c 

OBJS += \
./lib/efm8lb1/peripheralDrivers/src/adc_0.OBJ \
./lib/efm8lb1/peripheralDrivers/src/dac_0.OBJ \
./lib/efm8lb1/peripheralDrivers/src/dac_1.OBJ \
./lib/efm8lb1/peripheralDrivers/src/dac_2.OBJ \
./lib/efm8lb1/peripheralDrivers/src/dac_3.OBJ \
./lib/efm8lb1/peripheralDrivers/src/wdt_0.OBJ 


# Each subdirectory must supply rules for building sources it contributes
lib/efm8lb1/peripheralDrivers/src/%.OBJ: ../lib/efm8lb1/peripheralDrivers/src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Keil 8051 Compiler'
	C51 "@$(patsubst %.OBJ,%.__i,$@)" || $(RC)
	@echo 'Finished building: $<'
	@echo ' '


