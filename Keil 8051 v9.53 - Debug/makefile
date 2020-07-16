################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Keil tools exit code 1 is warnings only, so don't treat as an error in make
RC := test $$? -lt 2

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include src/subdir.mk
-include mcu_sal_lib/subdir.mk
-include lib/efm8lb1/peripheralDrivers/src/subdir.mk
-include lib/efm8_assert/subdir.mk
-include subdir.mk
-include objects.mk

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: C51_UBOOT.omf

# Tool invocations
C51_UBOOT.omf: echo_path $(OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: Keil 8051 Linker'
	LX51 "@C51_UBOOT.lnp" || $(RC)
	@echo 'Finished building target: $@'

# Change from all upper case to the expected case
	@-mv "C51_UBOOT.OMF.CRBUILD" "$@"

# Generate hex file: C51_UBOOT.hex
	@Ohx51 "C51_UBOOT.omf" "HEXFILE (C51_UBOOT.hex)" "H386" 2>&1 >/dev/null

# Other Targets
clean:
	-$(RM) $(OBJS)$(EXECUTABLES) C51_UBOOT.omf
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
# echo the path
echo_path:
	@echo PATH=$$PATH
	@echo ' '

