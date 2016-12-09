#****************************************************************************************
#|  Description: Makefile for STM32 using CodeSourcery GNU GCC compiler toolset
#|    File Name: makefile
#|
#|---------------------------------------------------------------------------------------
#|                          C O P Y R I G H T
#|---------------------------------------------------------------------------------------
#|   Copyright (c) 2011  by Feaser    http://www.feaser.com    All rights reserved
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

#|---------------------------------------------------------------------------------------|
#| Configure project name                                                                |
#|---------------------------------------------------------------------------------------|
PROJ_NAME=demoprog_nucleo_stm32f103rb


#|---------------------------------------------------------------------------------------|
#| Speficy project source files                                                          |
#|---------------------------------------------------------------------------------------|
PROJ_FILES= \
boot.c \
boot.h \
cstart.c \
header.h \
led.c \
led.h \
main.c \
timer.c \
timer.h \
vectors.c \
lib/stdperiphlib/stm32f10x_conf.h \
lib/stdperiphlib/CMSIS/CM3/CoreSupport/core_cm3.c \
lib/stdperiphlib/CMSIS/CM3/CoreSupport/core_cm3.h \
lib/stdperiphlib/CMSIS/CM3/DeviceSupport/ST/STM32F10x/stm32f10x.h \
lib/stdperiphlib/CMSIS/CM3/DeviceSupport/ST/STM32F10x/system_stm32f10x.c \
lib/stdperiphlib/CMSIS/CM3/DeviceSupport/ST/STM32F10x/system_stm32f10x.h \
lib/stdperiphlib/STM32F10x_StdPeriph_Driver/inc/misc.h \
lib/stdperiphlib/STM32F10x_StdPeriph_Driver/inc/stm32f10x_adc.h \
lib/stdperiphlib/STM32F10x_StdPeriph_Driver/inc/stm32f10x_bkp.h \
lib/stdperiphlib/STM32F10x_StdPeriph_Driver/inc/stm32f10x_can.h \
lib/stdperiphlib/STM32F10x_StdPeriph_Driver/inc/stm32f10x_cec.h \
lib/stdperiphlib/STM32F10x_StdPeriph_Driver/inc/stm32f10x_crc.h \
lib/stdperiphlib/STM32F10x_StdPeriph_Driver/inc/stm32f10x_dac.h \
lib/stdperiphlib/STM32F10x_StdPeriph_Driver/inc/stm32f10x_dbgmcu.h \
lib/stdperiphlib/STM32F10x_StdPeriph_Driver/inc/stm32f10x_dma.h \
lib/stdperiphlib/STM32F10x_StdPeriph_Driver/inc/stm32f10x_exti.h \
lib/stdperiphlib/STM32F10x_StdPeriph_Driver/inc/stm32f10x_flash.h \
lib/stdperiphlib/STM32F10x_StdPeriph_Driver/inc/stm32f10x_fsmc.h \
lib/stdperiphlib/STM32F10x_StdPeriph_Driver/inc/stm32f10x_gpio.h \
lib/stdperiphlib/STM32F10x_StdPeriph_Driver/inc/stm32f10x_i2c.h \
lib/stdperiphlib/STM32F10x_StdPeriph_Driver/inc/stm32f10x_iwdg.h \
lib/stdperiphlib/STM32F10x_StdPeriph_Driver/inc/stm32f10x_pwr.h \
lib/stdperiphlib/STM32F10x_StdPeriph_Driver/inc/stm32f10x_rcc.h \
lib/stdperiphlib/STM32F10x_StdPeriph_Driver/inc/stm32f10x_rtc.h \
lib/stdperiphlib/STM32F10x_StdPeriph_Driver/inc/stm32f10x_sdio.h \
lib/stdperiphlib/STM32F10x_StdPeriph_Driver/inc/stm32f10x_spi.h \
lib/stdperiphlib/STM32F10x_StdPeriph_Driver/inc/stm32f10x_tim.h \
lib/stdperiphlib/STM32F10x_StdPeriph_Driver/inc/stm32f10x_usart.h \
lib/stdperiphlib/STM32F10x_StdPeriph_Driver/inc/stm32f10x_wwdg.h \
lib/stdperiphlib/STM32F10x_StdPeriph_Driver/src/misc.c \
lib/stdperiphlib/STM32F10x_StdPeriph_Driver/src/stm32f10x_adc.c \
lib/stdperiphlib/STM32F10x_StdPeriph_Driver/src/stm32f10x_bkp.c \
lib/stdperiphlib/STM32F10x_StdPeriph_Driver/src/stm32f10x_can.c \
lib/stdperiphlib/STM32F10x_StdPeriph_Driver/src/stm32f10x_cec.c \
lib/stdperiphlib/STM32F10x_StdPeriph_Driver/src/stm32f10x_crc.c \
lib/stdperiphlib/STM32F10x_StdPeriph_Driver/src/stm32f10x_dac.c \
lib/stdperiphlib/STM32F10x_StdPeriph_Driver/src/stm32f10x_dbgmcu.c \
lib/stdperiphlib/STM32F10x_StdPeriph_Driver/src/stm32f10x_dma.c \
lib/stdperiphlib/STM32F10x_StdPeriph_Driver/src/stm32f10x_exti.c \
lib/stdperiphlib/STM32F10x_StdPeriph_Driver/src/stm32f10x_flash.c \
lib/stdperiphlib/STM32F10x_StdPeriph_Driver/src/stm32f10x_fsmc.c \
lib/stdperiphlib/STM32F10x_StdPeriph_Driver/src/stm32f10x_gpio.c \
lib/stdperiphlib/STM32F10x_StdPeriph_Driver/src/stm32f10x_i2c.c \
lib/stdperiphlib/STM32F10x_StdPeriph_Driver/src/stm32f10x_iwdg.c \
lib/stdperiphlib/STM32F10x_StdPeriph_Driver/src/stm32f10x_pwr.c \
lib/stdperiphlib/STM32F10x_StdPeriph_Driver/src/stm32f10x_rcc.c \
lib/stdperiphlib/STM32F10x_StdPeriph_Driver/src/stm32f10x_rtc.c \
lib/stdperiphlib/STM32F10x_StdPeriph_Driver/src/stm32f10x_sdio.c \
lib/stdperiphlib/STM32F10x_StdPeriph_Driver/src/stm32f10x_spi.c \
lib/stdperiphlib/STM32F10x_StdPeriph_Driver/src/stm32f10x_tim.c \
lib/stdperiphlib/STM32F10x_StdPeriph_Driver/src/stm32f10x_usart.c \
lib/stdperiphlib/STM32F10x_StdPeriph_Driver/src/stm32f10x_wwdg.c


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
INC_PATH += -I.
LIB_PATH  = 


#|---------------------------------------------------------------------------------------|
#| Options for compiler binaries                                                         |
#|---------------------------------------------------------------------------------------|
CFLAGS   = -g -mthumb -mcpu=cortex-m3 -mlong-calls -O1 -T memory.x
CFLAGS  += -D PACK_STRUCT_END=__attribute\(\(packed\)\) -D sprintf=usprintf -Wno-main
CFLAGS  += -D ALIGN_STRUCT_END=__attribute\(\(aligned\(4\)\)\) -D snprintf=usnprintf
CFLAGS  += -D printf=uipprintf -ffunction-sections -fdata-sections $(INC_PATH)
CFLAGS  += -D STM32F10X_MD -D USE_STDPERIPH_DRIVER -D VECT_TAB_FLASH -D GCC_ARMCM3
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

                                                                                                                                                                                                                                                                