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

lib/efm8lb1/peripheralDrivers/src/adc_0.OBJ: C:/SiliconLabs/SimplicityStudio/v3/developer/sdks/si8051/v3/Device/EFM8LB1/peripheral_driver/inc/adc_0.h C:/SiliconLabs/SimplicityStudio/v3/developer/toolchains/keil_8051/9.53/INC/ASSERT.H D:/Work\ Project/SingleChipWorkSpaveNew/C51_UBOOT/inc/efm8_config.h C:/SiliconLabs/SimplicityStudio/v3/developer/sdks/si8051/v3/Device/EFM8LB1/inc/SI_EFM8LB1_Register_Enums.h C:/SiliconLabs/SimplicityStudio/v3/developer/toolchains/keil_8051/9.53/INC/STDIO.H C:/SiliconLabs/SimplicityStudio/v3/developer/sdks/si8051/v3/Device/EFM8LB1/inc/SI_EFM8LB1_Defs.h C:/SiliconLabs/SimplicityStudio/v3/developer/sdks/si8051/v3/Device/shared/si8051Base/si_toolchain.h C:/SiliconLabs/SimplicityStudio/v3/developer/sdks/si8051/v3/Device/shared/si8051Base/stdint.h C:/SiliconLabs/SimplicityStudio/v3/developer/sdks/si8051/v3/Device/shared/si8051Base/stdbool.h

lib/efm8lb1/peripheralDrivers/src/dac_0.OBJ: C:/SiliconLabs/SimplicityStudio/v3/developer/sdks/si8051/v3/Device/EFM8LB1/peripheral_driver/inc/dac_0.h D:/Work\ Project/SingleChipWorkSpaveNew/C51_UBOOT/inc/efm8_config.h C:/SiliconLabs/SimplicityStudio/v3/developer/sdks/si8051/v3/Device/EFM8LB1/inc/SI_EFM8LB1_Register_Enums.h C:/SiliconLabs/SimplicityStudio/v3/developer/sdks/si8051/v3/Device/EFM8LB1/inc/SI_EFM8LB1_Defs.h C:/SiliconLabs/SimplicityStudio/v3/developer/sdks/si8051/v3/Device/shared/si8051Base/si_toolchain.h C:/SiliconLabs/SimplicityStudio/v3/developer/sdks/si8051/v3/Device/shared/si8051Base/stdint.h C:/SiliconLabs/SimplicityStudio/v3/developer/sdks/si8051/v3/Device/shared/si8051Base/stdbool.h

lib/efm8lb1/peripheralDrivers/src/dac_1.OBJ: C:/SiliconLabs/SimplicityStudio/v3/developer/sdks/si8051/v3/Device/EFM8LB1/peripheral_driver/inc/dac_1.h D:/Work\ Project/SingleChipWorkSpaveNew/C51_UBOOT/inc/efm8_config.h C:/SiliconLabs/SimplicityStudio/v3/developer/sdks/si8051/v3/Device/EFM8LB1/inc/SI_EFM8LB1_Register_Enums.h C:/SiliconLabs/SimplicityStudio/v3/developer/sdks/si8051/v3/Device/EFM8LB1/inc/SI_EFM8LB1_Defs.h C:/SiliconLabs/SimplicityStudio/v3/developer/sdks/si8051/v3/Device/shared/si8051Base/si_toolchain.h C:/SiliconLabs/SimplicityStudio/v3/developer/sdks/si8051/v3/Device/shared/si8051Base/stdint.h C:/SiliconLabs/SimplicityStudio/v3/developer/sdks/si8051/v3/Device/shared/si8051Base/stdbool.h

lib/efm8lb1/peripheralDrivers/src/dac_2.OBJ: C:/SiliconLabs/SimplicityStudio/v3/developer/sdks/si8051/v3/Device/EFM8LB1/peripheral_driver/inc/dac_2.h D:/Work\ Project/SingleChipWorkSpaveNew/C51_UBOOT/inc/efm8_config.h C:/SiliconLabs/SimplicityStudio/v3/developer/sdks/si8051/v3/Device/EFM8LB1/inc/SI_EFM8LB1_Register_Enums.h C:/SiliconLabs/SimplicityStudio/v3/developer/sdks/si8051/v3/Device/EFM8LB1/inc/SI_EFM8LB1_Defs.h C:/SiliconLabs/SimplicityStudio/v3/developer/sdks/si8051/v3/Device/shared/si8051Base/si_toolchain.h C:/SiliconLabs/SimplicityStudio/v3/developer/sdks/si8051/v3/Device/shared/si8051Base/stdint.h C:/SiliconLabs/SimplicityStudio/v3/developer/sdks/si8051/v3/Device/shared/si8051Base/stdbool.h

lib/efm8lb1/peripheralDrivers/src/dac_3.OBJ: C:/SiliconLabs/SimplicityStudio/v3/developer/sdks/si8051/v3/Device/EFM8LB1/peripheral_driver/inc/dac_3.h D:/Work\ Project/SingleChipWorkSpaveNew/C51_UBOOT/inc/efm8_config.h C:/SiliconLabs/SimplicityStudio/v3/developer/sdks/si8051/v3/Device/EFM8LB1/inc/SI_EFM8LB1_Register_Enums.h C:/SiliconLabs/SimplicityStudio/v3/developer/sdks/si8051/v3/Device/EFM8LB1/inc/SI_EFM8LB1_Defs.h C:/SiliconLabs/SimplicityStudio/v3/developer/sdks/si8051/v3/Device/shared/si8051Base/si_toolchain.h C:/SiliconLabs/SimplicityStudio/v3/developer/sdks/si8051/v3/Device/shared/si8051Base/stdint.h C:/SiliconLabs/SimplicityStudio/v3/developer/sdks/si8051/v3/Device/shared/si8051Base/stdbool.h

lib/efm8lb1/peripheralDrivers/src/wdt_0.OBJ: C:/SiliconLabs/SimplicityStudio/v3/developer/sdks/si8051/v3/Device/EFM8LB1/peripheral_driver/inc/wdt_0.h C:/SiliconLabs/SimplicityStudio/v3/developer/toolchains/keil_8051/9.53/INC/ASSERT.H D:/Work\ Project/SingleChipWorkSpaveNew/C51_UBOOT/inc/efm8_config.h C:/SiliconLabs/SimplicityStudio/v3/developer/sdks/si8051/v3/Device/EFM8LB1/inc/SI_EFM8LB1_Register_Enums.h C:/SiliconLabs/SimplicityStudio/v3/developer/toolchains/keil_8051/9.53/INC/STDIO.H C:/SiliconLabs/SimplicityStudio/v3/developer/sdks/si8051/v3/Device/EFM8LB1/inc/SI_EFM8LB1_Defs.h C:/SiliconLabs/SimplicityStudio/v3/developer/sdks/si8051/v3/Device/shared/si8051Base/si_toolchain.h C:/SiliconLabs/SimplicityStudio/v3/developer/sdks/si8051/v3/Device/shared/si8051Base/stdint.h C:/SiliconLabs/SimplicityStudio/v3/developer/sdks/si8051/v3/Device/shared/si8051Base/stdbool.h


