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
PROJ_NAME=openblt_demoprog_nucleo_stm32f103rb


#|---------------------------------------------------------------------------------------|
#| Speficy project source files                                                          |
#|---------------------------------------------------------------------------------------|
PROJ_FILES= \
blt_conf.h \
hooks.c \
main.c \
./lib/fatfs/ffconf.h \
./lib/fatfs/mmc.c \
./lib/CMSIS/CM3/CoreSupport/core_cm3.c \
./lib/CMSIS/CM3/CoreSupport/core_cm3.h \
./lib/CMSIS/CM3/DeviceSupport/ST/STM32F10x/stm32f10x.h \
./lib/CMSIS/CM3/DeviceSupport/ST/STM32F10x/system_stm32f10x.c \
./lib/CMSIS/CM3/DeviceSupport/ST/STM32F10x/system_stm32f10x.h \
./lib/STM32F10x_StdPeriph_Driver/inc/stm32f10x_gpio.h \
./lib/STM32F10x_StdPeriph_Driver/inc/stm32f10x_rcc.h \
./lib/STM32F10x_StdPeriph_Driver/inc/stm32f10x_spi.h \
./lib/STM32F10x_StdPeriph_Driver/inc/stm32f10x_usart.h \
./lib/STM32F10x_StdPeriph_Driver/src/stm32f10x_gpio.c \
./lib/STM32F10x_StdPeriph_Driver/src/stm32f10x_rcc.c \
./lib/STM32F10x_StdPeriph_Driver/src/stm32f10x_spi.c \
./lib/STM32F10x_StdPeriph_Driver/src/stm32f10x_usart.c \
./lib/STM32F10x_StdPeriph_Driver/stm32f10x_conf.h \
../../../Source/third_party/fatfs/src/diskio.h \
../../../Source/third_party/fatfs/src/ff.c \
../../../Source/third_party/fatfs/src/ff.h \
../../../Source/third_party/fatfs/src/integer.h \
../../../Source/third_party/fatfs/src/option/unicode.c \
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
../../../Source/assert.c \
../../../Source/assert.h \
../../../Source/plausibility.h \
../../../Source/ARMCM3_STM32/types.h \
../../../Source/ARMCM3_STM32/cpu.c \
../../../Source/ARMCM3_STM32/cpu.h \
../../../Source/ARMCM3_STM32/can.c \
../../../Source/ARMCM3_STM32/can.h \
../../../Source/ARMCM3_STM32/uart.c \
../../../Source/ARMCM3_STM32/uart.h \
../../../Source/ARMCM3_STM32/nvm.c \
../../../Source/ARMCM3_STM32/nvm.h \
../../../Source/ARMCM3_STM32/timer.c \
../../../Source/ARMCM3_STM32/timer.h \
../../../Source/ARMCM3_STM32/flash.c \
../../../Source/ARMCM3_STM32/flash.h \
../../../Source/ARMCM3_STM32/GCC/vectors.c \
../../../Source/ARMCM3_STM32/GCC/cstart.c


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
LIB_PATH  = -L../../../Source/ARMCM3_STM32/GCC/


#|---------------------------------------------------------------------------------------|
#| Options for compiler binaries                                                         |
#|---------------------------------------------------------------------------------------|
CFLAGS   = -g -D inline= -mthumb -mcpu=cortex-m3 -O1 -T memory.x
CFLAGS  += -D PACK_STRUCT_END=__attribute\(\(packed\)\) -Wno-main
CFLAGS  += -D ALIGN_STRUCT_END=__attribute\(\(aligned\(4\)\)\) 
CFLAGS  += -ffunction-sections -fdata-sections $(INC_PATH) -D STM32F10X_MD -D GCC_ARMCM3
CFLAGS  += -D USE_STDPERIPH_DRIVER -D VECT_TAB_FLASH -Wa,-adhlns="$(OBJ_PATH)/$(subst .o,.lst,$@)"
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
	@rm -f $(foreach file,$(AOBJS),$(OBJ_PATH)/$(file))
	@rm -f $(foreach file,$(COBJS),$(OBJ_PATH)/$(file))
	@rm -f $(patsubst %.o,%.lst,$(foreach file,$(COBJS),$(OBJ_PATH)/$(file)))
	@rm -f $(BIN_PATH)/$(PROJ_NAME).elf $(BIN_PATH)/$(PROJ_NAME).map
	@rm -f $(BIN_PATH)/$(PROJ_NAME).srec
	@echo +++ Clean complete

                                                                                                                                                                                                                                                                