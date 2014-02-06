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
#| You should have received a copy of the GNU General Public License along with OpenBTL.
#| If not, see <http://www.gnu.org/licenses/>.
#|
#****************************************************************************************
SHELL = sh

#|---------------------------------------------------------------------------------------|
#| Configure project name                                                                |
#|---------------------------------------------------------------------------------------|
PROJ_NAME=demoprog_ek_lm3s6965


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
time.c \
time.h \
vectors.c \
lib/inc/asmdefs.h \
lib/inc/hw_adc.h \
lib/inc/hw_comp.h \
lib/inc/hw_epi.h \
lib/inc/hw_ethernet.h \
lib/inc/hw_flash.h \
lib/inc/hw_gpio.h \
lib/inc/hw_hibernate.h \
lib/inc/hw_i2c.h \
lib/inc/hw_i2s.h \
lib/inc/hw_ints.h \
lib/inc/hw_memmap.h \
lib/inc/hw_nvic.h \
lib/inc/hw_pwm.h \
lib/inc/hw_qei.h \
lib/inc/hw_ssi.h \
lib/inc/hw_sysctl.h \
lib/inc/hw_timer.h \
lib/inc/hw_types.h \
lib/inc/hw_uart.h \
lib/inc/hw_udma.h \
lib/inc/hw_usb.h \
lib/inc/hw_watchdog.h \
lib/inc/lm3s6965.h \
lib/driverlib/adc.c \
lib/driverlib/adc.h \
lib/driverlib/comp.c \
lib/driverlib/comp.h \
lib/driverlib/cpu.c \
lib/driverlib/cpu.h \
lib/driverlib/debug.h \
lib/driverlib/epi.c \
lib/driverlib/epi.h \
lib/driverlib/ethernet.c \
lib/driverlib/ethernet.h \
lib/driverlib/flash.c \
lib/driverlib/flash.h \
lib/driverlib/gpio.c \
lib/driverlib/gpio.h \
lib/driverlib/hibernate.c \
lib/driverlib/hibernate.h \
lib/driverlib/i2c.c \
lib/driverlib/i2c.h \
lib/driverlib/i2s.c \
lib/driverlib/i2s.h \
lib/driverlib/interrupt.c \
lib/driverlib/interrupt.h \
lib/driverlib/mpu.c \
lib/driverlib/mpu.h \
lib/driverlib/pin_map.h \
lib/driverlib/pwm.c \
lib/driverlib/pwm.h \
lib/driverlib/qei.c \
lib/driverlib/qei.h \
lib/driverlib/rom.h \
lib/driverlib/rom_map.h \
lib/driverlib/ssi.c \
lib/driverlib/ssi.h \
lib/driverlib/sysctl.c \
lib/driverlib/sysctl.h \
lib/driverlib/systick.c \
lib/driverlib/systick.h \
lib/driverlib/timer.c \
lib/driverlib/timer.h \
lib/driverlib/uart.c \
lib/driverlib/uart.h \
lib/driverlib/udma.c \
lib/driverlib/udma.h \
lib/driverlib/usb.c \
lib/driverlib/usb.h \
lib/driverlib/watchdog.c \
lib/driverlib/watchdog.h


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
CFLAGS  += -D DEBUG -D gcc
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

                                                                                                                                                                                                                                                                