################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../my_lib/lib_adc.c \
../my_lib/lib_i2c_master.c \
../my_lib/lib_terminal.c \
../my_lib/lib_timer.c 

OBJS += \
./my_lib/lib_adc.OBJ \
./my_lib/lib_i2c_master.OBJ \
./my_lib/lib_terminal.OBJ \
./my_lib/lib_timer.OBJ 


# Each subdirectory must supply rules for building sources it contributes
my_lib/%.OBJ: ../my_lib/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Keil 8051 Compiler'
	C51 "@$(patsubst %.OBJ,%.__i,$@)" || $(RC)
	@echo 'Finished building: $<'
	@echo ' '

my_lib/lib_adc.OBJ: D:/Work\ Project/SingleChipWorkSpaveNew/C51_UBOOT/my_lib/lib_adc.h C:/SiliconLabs/SimplicityStudio/v3/developer/toolchains/keil_8051/9.53/INC/STRING.H C:/SiliconLabs/SimplicityStudio/v3/developer/sdks/si8051/v3/Device/shared/si8051Base/stdint.h D:/Work\ Project/SingleChipWorkSpaveNew/C51_UBOOT/my_lib/lib_peripheral.h D:/Work\ Project/SingleChipWorkSpaveNew/C51_UBOOT/inc/Efm8lb12_uart.h D:/Work\ Project/SingleChipWorkSpaveNew/C51_UBOOT/inc/Efm8lb12_i2c_master.h D:/Work\ Project/SingleChipWorkSpaveNew/C51_UBOOT/inc/Efm8lb12_timer.h D:/Work\ Project/SingleChipWorkSpaveNew/C51_UBOOT/inc/Efm8lb12_adc.h C:/SiliconLabs/SimplicityStudio/v3/developer/sdks/si8051/v3/Device/EFM8LB1/inc/SI_EFM8LB1_Register_Enums.h C:/SiliconLabs/SimplicityStudio/v3/developer/sdks/si8051/v3/Device/EFM8LB1/peripheral_driver/inc/adc_0.h C:/SiliconLabs/SimplicityStudio/v3/developer/sdks/si8051/v3/Device/EFM8LB1/inc/SI_EFM8LB1_Defs.h D:/Work\ Project/SingleChipWorkSpaveNew/C51_UBOOT/inc/efm8_config.h C:/SiliconLabs/SimplicityStudio/v3/developer/sdks/si8051/v3/Device/shared/si8051Base/si_toolchain.h C:/SiliconLabs/SimplicityStudio/v3/developer/sdks/si8051/v3/Device/shared/si8051Base/stdbool.h

my_lib/lib_i2c_master.OBJ: C:/SiliconLabs/SimplicityStudio/v3/developer/toolchains/keil_8051/9.53/INC/STRING.H D:/Work\ Project/SingleChipWorkSpaveNew/C51_UBOOT/my_lib/lib_timer.h D:/Work\ Project/SingleChipWorkSpaveNew/C51_UBOOT/my_lib/lib_macro.h D:/Work\ Project/SingleChipWorkSpaveNew/C51_UBOOT/my_lib/lib_i2c_master.h C:/SiliconLabs/SimplicityStudio/v3/developer/sdks/si8051/v3/Device/shared/si8051Base/stdint.h C:/SiliconLabs/SimplicityStudio/v3/developer/sdks/si8051/v3/Device/shared/si8051Base/stdbool.h D:/Work\ Project/SingleChipWorkSpaveNew/C51_UBOOT/my_lib/lib_peripheral.h C:/SiliconLabs/SimplicityStudio/v3/developer/sdks/si8051/v3/Device/EFM8LB1/inc/SI_EFM8LB1_Register_Enums.h D:/Work\ Project/SingleChipWorkSpaveNew/C51_UBOOT/inc/Efm8lb12_uart.h D:/Work\ Project/SingleChipWorkSpaveNew/C51_UBOOT/inc/Efm8lb12_i2c_master.h D:/Work\ Project/SingleChipWorkSpaveNew/C51_UBOOT/inc/Efm8lb12_timer.h D:/Work\ Project/SingleChipWorkSpaveNew/C51_UBOOT/inc/Efm8lb12_adc.h C:/SiliconLabs/SimplicityStudio/v3/developer/sdks/si8051/v3/Device/EFM8LB1/inc/SI_EFM8LB1_Defs.h C:/SiliconLabs/SimplicityStudio/v3/developer/sdks/si8051/v3/Device/EFM8LB1/peripheral_driver/inc/adc_0.h C:/SiliconLabs/SimplicityStudio/v3/developer/sdks/si8051/v3/Device/shared/si8051Base/si_toolchain.h D:/Work\ Project/SingleChipWorkSpaveNew/C51_UBOOT/inc/efm8_config.h

my_lib/lib_terminal.OBJ: C:/SiliconLabs/SimplicityStudio/v3/developer/toolchains/keil_8051/9.53/INC/STDIO.H C:/SiliconLabs/SimplicityStudio/v3/developer/toolchains/keil_8051/9.53/INC/STRING.H C:/SiliconLabs/SimplicityStudio/v3/developer/toolchains/keil_8051/9.53/INC/STDARG.H D:/Work\ Project/SingleChipWorkSpaveNew/C51_UBOOT/my_lib/lib_terminal.h D:/Work\ Project/SingleChipWorkSpaveNew/C51_UBOOT/my_lib/lib_macro.h C:/SiliconLabs/SimplicityStudio/v3/developer/sdks/si8051/v3/Device/EFM8LB1/inc/SI_EFM8LB1_Register_Enums.h D:/Work\ Project/SingleChipWorkSpaveNew/C51_UBOOT/my_lib/lib_peripheral.h C:/SiliconLabs/SimplicityStudio/v3/developer/sdks/si8051/v3/Device/EFM8LB1/inc/SI_EFM8LB1_Defs.h D:/Work\ Project/SingleChipWorkSpaveNew/C51_UBOOT/inc/Efm8lb12_uart.h D:/Work\ Project/SingleChipWorkSpaveNew/C51_UBOOT/inc/Efm8lb12_i2c_master.h D:/Work\ Project/SingleChipWorkSpaveNew/C51_UBOOT/inc/Efm8lb12_timer.h D:/Work\ Project/SingleChipWorkSpaveNew/C51_UBOOT/inc/Efm8lb12_adc.h C:/SiliconLabs/SimplicityStudio/v3/developer/sdks/si8051/v3/Device/shared/si8051Base/si_toolchain.h C:/SiliconLabs/SimplicityStudio/v3/developer/sdks/si8051/v3/Device/shared/si8051Base/stdint.h C:/SiliconLabs/SimplicityStudio/v3/developer/sdks/si8051/v3/Device/EFM8LB1/peripheral_driver/inc/adc_0.h C:/SiliconLabs/SimplicityStudio/v3/developer/sdks/si8051/v3/Device/shared/si8051Base/stdbool.h D:/Work\ Project/SingleChipWorkSpaveNew/C51_UBOOT/inc/efm8_config.h

my_lib/lib_timer.OBJ: D:/Work\ Project/SingleChipWorkSpaveNew/C51_UBOOT/my_lib/lib_timer.h C:/SiliconLabs/SimplicityStudio/v3/developer/sdks/si8051/v3/Device/shared/si8051Base/stdint.h C:/SiliconLabs/SimplicityStudio/v3/developer/toolchains/keil_8051/9.53/INC/STRING.H C:/SiliconLabs/SimplicityStudio/v3/developer/sdks/si8051/v3/Device/shared/si8051Base/stdbool.h D:/Work\ Project/SingleChipWorkSpaveNew/C51_UBOOT/my_lib/lib_peripheral.h D:/Work\ Project/SingleChipWorkSpaveNew/C51_UBOOT/inc/Efm8lb12_uart.h D:/Work\ Project/SingleChipWorkSpaveNew/C51_UBOOT/inc/Efm8lb12_i2c_master.h D:/Work\ Project/SingleChipWorkSpaveNew/C51_UBOOT/inc/Efm8lb12_timer.h D:/Work\ Project/SingleChipWorkSpaveNew/C51_UBOOT/inc/Efm8lb12_adc.h C:/SiliconLabs/SimplicityStudio/v3/developer/sdks/si8051/v3/Device/EFM8LB1/inc/SI_EFM8LB1_Register_Enums.h C:/SiliconLabs/SimplicityStudio/v3/developer/sdks/si8051/v3/Device/EFM8LB1/peripheral_driver/inc/adc_0.h C:/SiliconLabs/SimplicityStudio/v3/developer/sdks/si8051/v3/Device/EFM8LB1/inc/SI_EFM8LB1_Defs.h D:/Work\ Project/SingleChipWorkSpaveNew/C51_UBOOT/inc/efm8_config.h C:/SiliconLabs/SimplicityStudio/v3/developer/sdks/si8051/v3/Device/shared/si8051Base/si_toolchain.h


