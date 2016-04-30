#****************************************************************************************
#|  Description: Makefile for LM3S using CodeSourcery GNU GCC compiler toolset
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
PROJ_NAME=openbtl_ek_lm3s6965


#|---------------------------------------------------------------------------------------|
#| Speficy project source files                                                          |
#|---------------------------------------------------------------------------------------|
PROJ_FILES= \
blt_conf.h \
hooks.c \
main.c \
./lib/inc/hw_ethernet.h \
./lib/inc/hw_flash.h \
./lib/inc/hw_gpio.h \
./lib/inc/hw_ints.h \
./lib/inc/hw_memmap.h \
./lib/inc/hw_nvic.h \
./lib/inc/hw_sysctl.h \
./lib/inc/hw_types.h \
./lib/inc/hw_uart.h \
./lib/inc/hw_ssi.h \
./lib/driverlib/ethernet.c \
./lib/driverlib/ethernet.h \
./lib/driverlib/cpulib.c \
./lib/driverlib/flashlib.c \
./lib/driverlib/gpio.h \
./lib/driverlib/sysctl.c \
./lib/driverlib/uartlib.h \
./lib/driverlib/cpulib.h \
./lib/driverlib/flashlib.h \
./lib/driverlib/interrupt.c \
./lib/driverlib/sysctl.h \
./lib/driverlib/debug.h \
./lib/driverlib/gpio.c \
./lib/driverlib/interrupt.h \
./lib/driverlib/uartlib.c \
./lib/driverlib/pin_map.h \
./lib/driverlib/ssi.c \
./lib/driverlib/ssi.h \
./lib/fatfs/ffconf.h \
./lib/fatfs/mmc.c \
./lib/uip/clock-arch.c \
./lib/uip/clock-arch.h \
./lib/uip/netdev.c \
./lib/uip/netdev.h \
./lib/uip/uip-conf.h \
../../../Source/boot.c \
../../../Source/boot.h \
../../../Source/com.c \
../../../Source/com.h \
../../../Source/xcp.c \
../../../Source/xcp.h \
../../../Source/backdoor.c \
../../../Source/backdoor.h \
../../../Source/cop.c \
../../../Source/cop.h \
../../../Source/file.c \
../../../Source/file.h \
../../../Source/net.c \
../../../Source/net.h \
../../../Source/assert.c \
../../../Source/assert.h \
../../../Source/cpu.h \
../../../Source/uart.h \
../../../Source/nvm.h \
../../../Source/timer.h \
../../../Source/plausibility.h \
../../../Source/third_party/fatfs/src/diskio.h \
../../../Source/third_party/fatfs/src/ff.c \
../../../Source/third_party/fatfs/src/ff.h \
../../../Source/third_party/fatfs/src/integer.h \
../../../Source/third_party/fatfs/src/option/unicode.c \
../../../Source/third_party/uip/uip/clock.h \
../../../Source/third_party/uip/uip/lc-addrlabels.h \
../../../Source/third_party/uip/uip/lc-switch.h \
../../../Source/third_party/uip/uip/lc.h \
../../../Source/third_party/uip/uip/pt.h \
../../../Source/third_party/uip/uip/uip-fw.h \
../../../Source/third_party/uip/uip/uip-neighbor.h \
../../../Source/third_party/uip/uip/uip-split.h \
../../../Source/third_party/uip/uip/uip.c \
../../../Source/third_party/uip/uip/uip.h \
../../../Source/third_party/uip/uip/uip_arch.h \
../../../Source/third_party/uip/uip/uip_arp.c \
../../../Source/third_party/uip/uip/uip_arp.h \
../../../Source/third_party/uip/uip/uip_timer.c \
../../../Source/third_party/uip/uip/uip_timer.h \
../../../Source/third_party/uip/uip/uiplib.c \
../../../Source/third_party/uip/uip/uiplib.h \
../../../Source/third_party/uip/uip/uipopt.h \
../../../Source/ARMCM3_LM3S/types.h \
../../../Source/ARMCM3_LM3S/cpu.c \
../../../Source/ARMCM3_LM3S/uart.c \
../../../Source/ARMCM3_LM3S/nvm.c \
../../../Source/ARMCM3_LM3S/timer.c \
../../../Source/ARMCM3_LM3S/flash.c \
../../../Source/ARMCM3_LM3S/flash.h \
../../../Source/ARMCM3_LM3S/GCC/vectors.c \
../../../Source/ARMCM3_LM3S/GCC/cstart.c


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
LIB_PATH  = -L../../../Source/ARMCM3_LM3S/GCC/


#|---------------------------------------------------------------------------------------|
#| Options for compiler binaries                                                         |
#|---------------------------------------------------------------------------------------|
CFLAGS   = -g -D inline= -mthumb -mcpu=cortex-m3 -Os -T memory.x
CFLAGS  += -D PACK_STRUCT_END=__attribute\(\(packed\)\) -Wno-main
CFLAGS  += -D ALIGN_STRUCT_END=__attribute\(\(aligned\(4\)\)\) 
CFLAGS  += -ffunction-sections -fdata-sections $(INC_PATH) -D DEBUG -D gcc
CFLAGS  += -Wa,-adhlns="$(OBJ_PATH)/$(subst .o,.lst,$@)"
LFLAGS   = -nostartfiles -Xlinker -M -Xlinker -Map=$(BIN_PATH)/$(PROJ_NAME).map
LFLAGS  += $(LIB_PATH) -Xlinker --gc-sections
OFLAGS   = -O binary
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
all : $(BIN_PATH)/$(PROJ_NAME).bin


$(BIN_PATH)/$(PROJ_NAME).bin : $(BIN_PATH)/$(PROJ_NAME).elf
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
	@cs-rm -f $(BIN_PATH)/$(PROJ_NAME).bin
	@echo +++ Clean complete

                                                                                                                                                                                                                                                                