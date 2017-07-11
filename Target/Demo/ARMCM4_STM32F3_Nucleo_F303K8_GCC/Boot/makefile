#****************************************************************************************
#|  Description: Makefile for GNU ARM Embedded toolchain.
#|    File Name: makefile
#|
#|---------------------------------------------------------------------------------------
#|                          C O P Y R I G H T
#|---------------------------------------------------------------------------------------
#|   Copyright (c) 2017  by Feaser    http://www.feaser.com    All rights reserved
#|
#|---------------------------------------------------------------------------------------
#|                            L I C E N S E
#|---------------------------------------------------------------------------------------
#| This file is part of OpenBLT. OpenBLT is free software: you can redistribute it and/or
#| modify it under the terms of the GNU General Public License as published by the Free
#| Software Foundation, either version 3 of the License, or (at your option) any later
#| version.
#|
#| OpenBLT is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
#| without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
#| PURPOSE. See the GNU General Public License for more details.
#|
#| You have received a copy of the GNU General Public License along with OpenBLT. It 
#| should be located in ".\Doc\license.html". If not, contact Feaser to obtain a copy.
#|
#****************************************************************************************
SHELL = sh

#|--------------------------------------------------------------------------------------|
#| Configure project name                                                               |
#|--------------------------------------------------------------------------------------|
PROJ_NAME=openblt_stm32f303


#|--------------------------------------------------------------------------------------|
#| Configure tool path                                                                  |
#|--------------------------------------------------------------------------------------|
TOOL_PATH=/opt/gcc-arm-none-eabi-5_4-2016q3/bin/


#|--------------------------------------------------------------------------------------|
#| Speficy project source files                                                         |
#|--------------------------------------------------------------------------------------|
PROJ_FILES= \
blt_conf.h \
main.c \
hooks.c \
led.c \
led.h \
startup_stm32f30x.S \
lib/stm32f30x_conf.h \
lib/stm32f30x.h \
lib/system_stm32f30x.h \
lib/system_stm32f30x.c \
lib/newlib/_exit.c \
lib/cmsis/core_cmSimd.h \
lib/cmsis/core_cmFunc.h \
lib/cmsis/core_cmInstr.h \
lib/cmsis/arm_const_structs.h \
lib/cmsis/core_cm4.h \
lib/cmsis/arm_common_tables.h \
lib/cmsis/arm_math.h \
lib/SPL/inc/stm32f30x_opamp.h \
lib/SPL/inc/stm32f30x_dma.h \
lib/SPL/inc/stm32f30x_pwr.h \
lib/SPL/inc/stm32f30x_iwdg.h \
lib/SPL/inc/stm32f30x_comp.h \
lib/SPL/inc/stm32f30x_syscfg.h \
lib/SPL/inc/stm32f30x_spi.h \
lib/SPL/inc/stm32f30x_rcc.h \
lib/SPL/inc/stm32f30x_usart.h \
lib/SPL/inc/stm32f30x_adc.h \
lib/SPL/inc/stm32f30x_rtc.h \
lib/SPL/inc/stm32f30x_wwdg.h \
lib/SPL/inc/stm32f30x_fmc.h \
lib/SPL/inc/stm32f30x_dac.h \
lib/SPL/inc/stm32f30x_exti.h \
lib/SPL/inc/stm32f30x_dbgmcu.h \
lib/SPL/inc/stm32f30x_i2c.h \
lib/SPL/inc/stm32f30x_misc.h \
lib/SPL/inc/stm32f30x_gpio.h \
lib/SPL/inc/stm32f30x_can.h \
lib/SPL/inc/stm32f30x_flash.h \
lib/SPL/inc/stm32f30x_crc.h \
lib/SPL/inc/stm32f30x_hrtim.h \
lib/SPL/inc/stm32f30x_tim.h \
lib/SPL/src/stm32f30x_dbgmcu.c \
lib/SPL/src/stm32f30x_opamp.c \
lib/SPL/src/stm32f30x_rtc.c \
lib/SPL/src/stm32f30x_misc.c \
lib/SPL/src/stm32f30x_wwdg.c \
lib/SPL/src/stm32f30x_iwdg.c \
lib/SPL/src/stm32f30x_flash.c \
lib/SPL/src/stm32f30x_pwr.c \
lib/SPL/src/stm32f30x_crc.c \
lib/SPL/src/stm32f30x_tim.c \
lib/SPL/src/stm32f30x_adc.c \
lib/SPL/src/stm32f30x_hrtim.c \
lib/SPL/src/stm32f30x_usart.c \
lib/SPL/src/stm32f30x_spi.c \
lib/SPL/src/stm32f30x_dac.c \
lib/SPL/src/stm32f30x_gpio.c \
lib/SPL/src/stm32f30x_can.c \
lib/SPL/src/stm32f30x_exti.c \
lib/SPL/src/stm32f30x_fmc.c \
lib/SPL/src/stm32f30x_rcc.c \
lib/SPL/src/stm32f30x_syscfg.c \
lib/SPL/src/stm32f30x_i2c.c \
lib/SPL/src/stm32f30x_comp.c \
lib/SPL/src/stm32f30x_dma.c \
../../../Source/assert.h \
../../../Source/assert.c \
../../../Source/backdoor.h \
../../../Source/backdoor.c \
../../../Source/boot.h \
../../../Source/boot.c \
../../../Source/com.h \
../../../Source/com.c \
../../../Source/cop.h \
../../../Source/cop.c \
../../../Source/file.h \
../../../Source/file.c \
../../../Source/net.h \
../../../Source/net.c \
../../../Source/xcp.h \
../../../Source/xcp.c \
../../../Source/plausibility.h \
../../../Source/usb.h \
../../../Source/can.h \
../../../Source/timer.h \
../../../Source/uart.h \
../../../Source/nvm.h \
../../../Source/cpu.h \
../../../Source/ARMCM4_STM32F3/types.h \
../../../Source/ARMCM4_STM32F3/flash.h \
../../../Source/ARMCM4_STM32F3/can.c \
../../../Source/ARMCM4_STM32F3/uart.c \
../../../Source/ARMCM4_STM32F3/flash.c \
../../../Source/ARMCM4_STM32F3/timer.c \
../../../Source/ARMCM4_STM32F3/cpu.c \
../../../Source/ARMCM4_STM32F3/nvm.c \
../../../Source/ARMCM4_STM32F3/GCC/cpu_comp.c


#|--------------------------------------------------------------------------------------|
#| Toolchain binaries                                                                   |
#|--------------------------------------------------------------------------------------|
RM = rm
CC = $(TOOL_PATH)arm-none-eabi-gcc
LN = $(TOOL_PATH)arm-none-eabi-gcc
OC = $(TOOL_PATH)arm-none-eabi-objcopy
OD = $(TOOL_PATH)arm-none-eabi-objdump
AS = $(TOOL_PATH)arm-none-eabi-gcc
SZ = $(TOOL_PATH)arm-none-eabi-size


#|--------------------------------------------------------------------------------------|
#| Extract file names                                                                   |
#|--------------------------------------------------------------------------------------|
PROJ_ASRCS  = $(filter %.S,$(foreach file,$(PROJ_FILES),$(notdir $(file))))
PROJ_CSRCS  = $(filter %.c,$(foreach file,$(PROJ_FILES),$(notdir $(file))))
PROJ_CHDRS  = $(filter %.h,$(foreach file,$(PROJ_FILES),$(notdir $(file))))


#|--------------------------------------------------------------------------------------|
#| Set important path variables                                                         |
#|--------------------------------------------------------------------------------------|
VPATH    = $(foreach path,$(sort $(foreach file,$(PROJ_FILES),$(dir $(file)))) $(subst \,/,$(OBJ_PATH)),$(path) :)
OBJ_PATH = obj
BIN_PATH = bin
INC_PATH = $(patsubst %,-I%,$(sort $(foreach file,$(filter %.h,$(PROJ_FILES)),$(dir $(file)))))
LIB_PATH  = 


#|--------------------------------------------------------------------------------------|
#| Options for toolchain binaries                                                       |
#|--------------------------------------------------------------------------------------|
HEAP_SIZE   = 0x0000
STACK_SIZE  = 0x0100
STDFLAGS    = -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -fno-strict-aliasing
STDFLAGS   += -fdata-sections -ffunction-sections -Wall -g3
OPTFLAGS    = -Og
CFLAGS      = $(STDFLAGS) $(OPTFLAGS)
CFLAGS     += -DSTM32F303K8 -DSTM32F334x8 -DUSE_STDPERIPH_DRIVER
CFLAGS     += -D__HEAP_SIZE=$(HEAP_SIZE) -D__STACK_SIZE=$(STACK_SIZE)
CFLAGS     += $(INC_PATH)
AFLAGS      = $(CFLAGS)
LFLAGS      = $(STDFLAGS) $(OPTFLAGS)
LFLAGS     += -Wl,--defsym=__HEAP_SIZE=$(HEAP_SIZE) -Wl,--defsym=__STACK_SIZE=$(STACK_SIZE)
LFLAGS     += -Wl,-script="stm32f303k8_flash.ld" -Wl,-Map=$(BIN_PATH)/$(PROJ_NAME).map
LFLAGS     += -specs=nano.specs -Wl,--gc-sections $(LIB_PATH)
OFLAGS      = -O srec
ODFLAGS     = -x
SZFLAGS     = -B -d
RMFLAGS     = -f


#|--------------------------------------------------------------------------------------|
#| Specify library files                                                                |
#|--------------------------------------------------------------------------------------|
LIBS = 


#|--------------------------------------------------------------------------------------|
#| Define targets                                                                       |
#|--------------------------------------------------------------------------------------|
AOBJS = $(patsubst %.S,%.o,$(PROJ_ASRCS))
COBJS = $(patsubst %.c,%.o,$(PROJ_CSRCS))


#|--------------------------------------------------------------------------------------|
#| Make ALL                                                                             |
#|--------------------------------------------------------------------------------------|
.PHONY: all
all: $(BIN_PATH)/$(PROJ_NAME).srec


$(BIN_PATH)/$(PROJ_NAME).srec : $(BIN_PATH)/$(PROJ_NAME).elf
	@$(OC) $< $(OFLAGS) $@
	@$(OD) $(ODFLAGS) $< > $(BIN_PATH)/$(PROJ_NAME).map
	@echo +++ Summary of memory consumption:
	@$(SZ) $(SZFLAGS) $<
	@echo +++ Build complete [$(notdir $@)]

$(BIN_PATH)/$(PROJ_NAME).elf : $(AOBJS) $(COBJS) 
	@echo +++ Linking [$(notdir $@)]
	@$(LN) $(LFLAGS) -o $@ $(patsubst %.o,$(OBJ_PATH)/%.o,$(^F)) $(LIBS)


#|--------------------------------------------------------------------------------------|
#| Compile and assemble                                                                 |
#|--------------------------------------------------------------------------------------|
$(AOBJS): %.o: %.S $(PROJ_CHDRS)
	@echo +++ Assembling [$(notdir $<)]
	@$(AS) $(AFLAGS) -c $< -o $(OBJ_PATH)/$(@F)

$(COBJS): %.o: %.c $(PROJ_CHDRS)
	@echo +++ Compiling [$(notdir $<)]
	@$(CC) $(CFLAGS) -c $< -o $(OBJ_PATH)/$(@F)


#|--------------------------------------------------------------------------------------|
#| Make CLEAN                                                                           |
#|--------------------------------------------------------------------------------------|
.PHONY: clean
clean: 
	@echo +++ Cleaning build environment
	@$(RM) $(RMFLAGS) $(foreach file,$(AOBJS),$(OBJ_PATH)/$(file))
	@$(RM) $(RMFLAGS) $(foreach file,$(COBJS),$(OBJ_PATH)/$(file))
	@$(RM) $(RMFLAGS) $(patsubst %.o,%.lst,$(foreach file,$(COBJS),$(OBJ_PATH)/$(file)))
	@$(RM) $(RMFLAGS) $(BIN_PATH)/$(PROJ_NAME).elf $(BIN_PATH)/$(PROJ_NAME).map
	@$(RM) $(RMFLAGS) $(BIN_PATH)/$(PROJ_NAME).srec
	@echo +++ Clean complete

                                                                                                                                                                                                                                                                
