#****************************************************************************************
#|  Description: Makefile for EFM32 using CodeSourcery GNU GCC compiler toolset
#|    File Name: makefile
#|
#|---------------------------------------------------------------------------------------
#|                          C O P Y R I G H T
#|---------------------------------------------------------------------------------------
#|   Copyright (c) 2012  by Feaser    http://www.feaser.com    All rights reserved
#|
#|---------------------------------------------------------------------------------------
#|                            L I C E N S E
#|---------------------------------------------------------------------------------------
#| This file is part of OpenBTL. OpenBTL is free software: you can redistribute it and/or
#| modify it under the terms of the GNU General Public License as published by the Free
#| Software Foundation, either version 3 of the License, or (at your option) any later
#| version.
#|
#| OpenBTL is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
#| without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
#| PURPOSE. See the GNU General Public License for more details.
#|
#| You have received a copy of the GNU General Public License along with OpenBLT. It 
#| should be located in ".\Doc\license.html". If not, contact Feaser to obtain a copy.
#|
#****************************************************************************************
SHELL = sh

#|---------------------------------------------------------------------------------------|
#| Configure project name                                                                |
#|---------------------------------------------------------------------------------------|
PROJ_NAME=demoprog_olimex_efm32g880


#|---------------------------------------------------------------------------------------|
#| Speficy project source files                                                          |
#|---------------------------------------------------------------------------------------|
PROJ_FILES= \
boot.c \
boot.h \
cstart.c \
header.h \
irq.c \
irq.h \
led.c \
led.h \
main.c \
timer.c \
timer.h \
vectors.c \
./lib/CMSIS/CM3/CoreSupport/core_cm3.c \
./lib/CMSIS/CM3/CoreSupport/core_cm3.h \
./lib/CMSIS/CM3/CoreSupport/core_cmFunc.h \
./lib/CMSIS/CM3/CoreSupport/core_cmInstr.h \
./lib/CMSIS/CM3/DeviceSupport/EnergyMicro/EFM32/efm32.h \
./lib/CMSIS/CM3/DeviceSupport/EnergyMicro/EFM32/efm32g880f128.h \
./lib/CMSIS/CM3/DeviceSupport/EnergyMicro/EFM32/system_efm32.c \
./lib/CMSIS/CM3/DeviceSupport/EnergyMicro/EFM32/system_efm32.h \
./lib/lcd/lcdcontroller.c \
./lib/lcd/lcdcontroller.h \
./lib/lcd/lcddisplay.h \
./lib/efm32lib/inc/efm32_acmp.h \
./lib/efm32lib/inc/efm32_adc.h \
./lib/efm32lib/inc/efm32_aes.h \
./lib/efm32lib/inc/efm32_assert.h \
./lib/efm32lib/inc/efm32_bitband.h \
./lib/efm32lib/inc/efm32_chip.h \
./lib/efm32lib/inc/efm32_cmu.h \
./lib/efm32lib/inc/efm32_common.h \
./lib/efm32lib/inc/efm32_dac.h \
./lib/efm32lib/inc/efm32_dbg.h \
./lib/efm32lib/inc/efm32_dma.h \
./lib/efm32lib/inc/efm32_ebi.h \
./lib/efm32lib/inc/efm32_emu.h \
./lib/efm32lib/inc/efm32_gpio.h \
./lib/efm32lib/inc/efm32_i2c.h \
./lib/efm32lib/inc/efm32_int.h \
./lib/efm32lib/inc/efm32_lcd.h \
./lib/efm32lib/inc/efm32_lesense.h \
./lib/efm32lib/inc/efm32_letimer.h \
./lib/efm32lib/inc/efm32_leuart.h \
./lib/efm32lib/inc/efm32_mpu.h \
./lib/efm32lib/inc/efm32_msc.h \
./lib/efm32lib/inc/efm32_opamp.h \
./lib/efm32lib/inc/efm32_pcnt.h \
./lib/efm32lib/inc/efm32_prs.h \
./lib/efm32lib/inc/efm32_rmu.h \
./lib/efm32lib/inc/efm32_rtc.h \
./lib/efm32lib/inc/efm32_system.h \
./lib/efm32lib/inc/efm32_timer.h \
./lib/efm32lib/inc/efm32_usart.h \
./lib/efm32lib/inc/efm32_vcmp.h \
./lib/efm32lib/inc/efm32_wdog.h \
./lib/efm32lib/src/efm32_acmp.c \
./lib/efm32lib/src/efm32_adc.c \
./lib/efm32lib/src/efm32_aes.c \
./lib/efm32lib/src/efm32_assert.c \
./lib/efm32lib/src/efm32_cmu.c \
./lib/efm32lib/src/efm32_dac.c \
./lib/efm32lib/src/efm32_dbg.c \
./lib/efm32lib/src/efm32_dma.c \
./lib/efm32lib/src/efm32_ebi.c \
./lib/efm32lib/src/efm32_emu.c \
./lib/efm32lib/src/efm32_gpio.c \
./lib/efm32lib/src/efm32_i2c.c \
./lib/efm32lib/src/efm32_int.c \
./lib/efm32lib/src/efm32_lcd.c \
./lib/efm32lib/src/efm32_lesense.c \
./lib/efm32lib/src/efm32_letimer.c \
./lib/efm32lib/src/efm32_leuart.c \
./lib/efm32lib/src/efm32_mpu.c \
./lib/efm32lib/src/efm32_msc.c \
./lib/efm32lib/src/efm32_opamp.c \
./lib/efm32lib/src/efm32_pcnt.c \
./lib/efm32lib/src/efm32_prs.c \
./lib/efm32lib/src/efm32_rmu.c \
./lib/efm32lib/src/efm32_rtc.c \
./lib/efm32lib/src/efm32_system.c \
./lib/efm32lib/src/efm32_timer.c \
./lib/efm32lib/src/efm32_usart.c \
./lib/efm32lib/src/efm32_vcmp.c \
./lib/efm32lib/src/efm32_wdog.c


#|---------------------------------------------------------------------------------------|
#| Compiler binaries                                                                     |
#|---------------------------------------------------------------------------------------|
CC = arm-none-eabi-gcc
LN = arm-none-eabi-gcc
OC = arm-none-eabi-objcopy
OD = arm-none-eabi-objdump
AS = arm-none-eabi-as
SZ = arm-none-eabi-size


#|---------------------------------------------------------------------------------------|
#| Extract file names                                                                    |
#|---------------------------------------------------------------------------------------|
PROJ_ASRCS  = $(filter %.s,$(foreach file,$(PROJ_FILES),$(notdir $(file))))
PROJ_CSRCS  = $(filter %.c,$(foreach file,$(PROJ_FILES),$(notdir $(file))))
PROJ_CHDRS  = $(filter %.h,$(foreach file,$(PROJ_FILES),$(notdir $(file))))
PROJ_CCMPL  = $(patsubst %.c,%.cpl,$(PROJ_CSRCS))
PROJ_ACMPL  = $(patsubst %.s,%.cpl,$(PROJ_ASRCS))


#|---------------------------------------------------------------------------------------|
#| Set important path variables                                                          |
#|---------------------------------------------------------------------------------------|
VPATH    = $(foreach path,$(sort $(foreach file,$(PROJ_FILES),$(dir $(file)))) $(subst \,/,$(OBJ_PATH)),$(path) :)
OBJ_PATH = obj
BIN_PATH = bin
INC_PATH = $(patsubst %,-I%,$(sort $(foreach file,$(filter %.h,$(PROJ_FILES)),$(dir $(file)))))
INC_PATH += -I. -I./lib
LIB_PATH  = 


#|---------------------------------------------------------------------------------------|
#| Options for compiler binaries                                                         |
#|---------------------------------------------------------------------------------------|
CFLAGS   = -g -D inline= -mthumb -mcpu=cortex-m3 -mlong-calls -O1 -T memory.x
CFLAGS  += -D PACK_STRUCT_END=__attribute\(\(packed\)\) -D sprintf=usprintf -Wno-main
CFLAGS  += -D ALIGN_STRUCT_END=__attribute\(\(aligned\(4\)\)\) -D snprintf=usnprintf
CFLAGS  += -D printf=uipprintf -ffunction-sections -fdata-sections $(INC_PATH)
CFLAGS  += -D EFM32G880F128  -Wno-attributes
LFLAGS   = -nostartfiles -Xlinker -M -Xlinker -Map=$(BIN_PATH)/$(PROJ_NAME).map
LFLAGS  += $(LIB_PATH) -Xlinker --gc-sections
OFLAGS   = -O srec
ODFLAGS  = -x
SZFLAGS  = -B -d


#|---------------------------------------------------------------------------------------|
#| Specify library files                                                                 |
#|---------------------------------------------------------------------------------------|
LIBS = 


#|---------------------------------------------------------------------------------------|
#| Define targets                                                                        |
#|---------------------------------------------------------------------------------------|
AOBJS = $(patsubst %.s,%.o,$(PROJ_ASRCS))
COBJS = $(patsubst %.c,%.o,$(PROJ_CSRCS))


#|---------------------------------------------------------------------------------------|
#| Make ALL                                                                              |
#|---------------------------------------------------------------------------------------|
all : $(BIN_PATH)/$(PROJ_NAME).srec


$(BIN_PATH)/$(PROJ_NAME).srec : $(BIN_PATH)/$(PROJ_NAME).elf
	@$(OC) $< $(OFLAGS) $@
	@$(OD) $(ODFLAGS) $< > $(BIN_PATH)/$(PROJ_NAME).map
	@echo +++ Summary of memory consumption:
	@$(SZ) $(SZFLAGS) $<
	@echo +++ Build complete [$(notdir $@)]

$(BIN_PATH)/$(PROJ_NAME).elf : $(AOBJS) $(COBJS) 
	@echo +++ Linking [$(notdir $@)]
	@$(LN) $(CFLAGS) -o $@ $(patsubst %.o,$(OBJ_PATH)/%.o,$(^F)) $(LIBS) $(LFLAGS)


#|---------------------------------------------------------------------------------------|
#| Compile and assemble                                                                  |
#|---------------------------------------------------------------------------------------|
$(AOBJS): %.o: %.s $(PROJ_CHDRS)
	@echo +++ Assembling [$(notdir $<)]
	@$(AS) $(AFLAGS) $< -o $(OBJ_PATH)/$(@F)

$(COBJS): %.o: %.c $(PROJ_CHDRS)
	@echo +++ Compiling [$(notdir $<)]
	@$(CC) $(CFLAGS) -c $< -o $(OBJ_PATH)/$(@F)


#|---------------------------------------------------------------------------------------|
#| Make CLEAN                                                                            |
#|---------------------------------------------------------------------------------------|
clean : 
	@echo +++ Cleaning build environment
	@cs-rm -f $(foreach file,$(AOBJS),$(OBJ_PATH)/$(file))
	@cs-rm -f $(foreach file,$(COBJS),$(OBJ_PATH)/$(file))
	@cs-rm -f $(patsubst %.o,%.lst,$(foreach file,$(COBJS),$(OBJ_PATH)/$(file)))
	@cs-rm -f $(BIN_PATH)/$(PROJ_NAME).elf $(BIN_PATH)/$(PROJ_NAME).map
	@cs-rm -f $(BIN_PATH)/$(PROJ_NAME).srec
	@echo +++ Clean complete

                                                                                                                                                                                                                                                                