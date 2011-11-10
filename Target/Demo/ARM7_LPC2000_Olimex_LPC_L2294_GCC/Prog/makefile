#****************************************************************************************
#|  Description: Makefile for NXP LPC2000 using CodeSourcery GNU GCC compiler toolset
#|    File Name: makefile
#|
#|---------------------------------------------------------------------------------------
#|                          C O P Y R I G H T
#|---------------------------------------------------------------------------------------
#|   Copyright (c) 2011  by Feaser LLC    http://www.feaser.com    All rights reserved
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
PROJ_NAME=demoprog_olimex_lpc_l2294_20mhz


#|---------------------------------------------------------------------------------------|
#| Speficy project source files                                                          |
#|---------------------------------------------------------------------------------------|
PROJ_FILES= \
lpc2294.h \
vectors.c \
main.c \
boot.c \
boot.h \
irq.c \
irq.h \
led.c \
led.h \
timer.c \
timer.h \
header.h \
cstart.s


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
VPATH     = $(foreach path,$(sort $(foreach file,$(PROJ_FILES),$(dir $(file)))) $(subst \,/,$(OBJ_PATH)),$(path) :)
OBJ_PATH  = ./obj
BIN_PATH  = ./bin
INC_PATH  = $(patsubst %,-I%,$(sort $(foreach file,$(filter %.h,$(PROJ_FILES)),$(dir $(file)))))
INC_PATH += -I.
LIB_PATH  = -L.


#|---------------------------------------------------------------------------------------|
#| Options for compiler binaries                                                         |
#|---------------------------------------------------------------------------------------|
AFLAGS   = -ahls -mapcs-32 -mcpu=arm7tdmi-s -mfpu=softfpa
CFLAGS   = $(INC_PATH) -O0 -Wall -fmessage-length=0 -mcpu=arm7tdmi-s -g -mlong-calls
CFLAGS  += -Wa,-adhlns="$(OBJ_PATH)/$(subst .o,.lst,$@)"
LFLAGS   = $(LIB_PATH) -T"memory.x" -g -mcpu=arm7tdmi-s -nostartfiles
OCFLAGS  = -O srec
ODFLAGS  = -x
SZFLAGS  = -B -d


#|---------------------------------------------------------------------------------------|
#| Define targets                                                                        |
#|---------------------------------------------------------------------------------------|
AOBJS = $(patsubst %.s,%.o,$(PROJ_ASRCS))
COBJS = $(patsubst %.c,%.o,$(PROJ_CSRCS))


#|---------------------------------------------------------------------------------------|
#| Make ALL                                                                              |
#|---------------------------------------------------------------------------------------|
all : $(BIN_PATH)/$(PROJ_NAME).sx

$(BIN_PATH)/$(PROJ_NAME).sx : $(BIN_PATH)/$(PROJ_NAME).elf
	@$(OC) $(OCFLAGS) $< $@
	@$(OD) $(ODFLAGS) $< > $(BIN_PATH)/$(PROJ_NAME).map
	@echo +++ Summary of memory consumption:
	@$(SZ) $(SZFLAGS) $<
	@echo +++ Build complete [$(notdir $@)]

$(BIN_PATH)/$(PROJ_NAME).elf : $(AOBJS) $(COBJS) 
	@echo +++ Linking [$(notdir $@)]
	@$(LN) $(LFLAGS) -o $@ $(patsubst %.o,$(OBJ_PATH)/%.o,$(^F))


#|---------------------------------------------------------------------------------------|
#| Compile and assemble                                                                  |
#|---------------------------------------------------------------------------------------|
$(AOBJS): %.o: %.s $(PROJ_CHDRS)
	@echo +++ Assembling [$@]
	@$(AS) $(AFLAGS) $< -o $(OBJ_PATH)/$(@F) > $(subst .o,.lst,$(OBJ_PATH)/$(@F))

$(COBJS): %.o: %.c $(PROJ_CHDRS)
	@echo +++ Compiling [$@]
	@$(CC) $(CFLAGS) -c -o $(OBJ_PATH)/$(@F) $<

#|---------------------------------------------------------------------------------------|
#| Make CLEAN                                                                            |
#|---------------------------------------------------------------------------------------|
clean : 
	@echo +++ Cleaning build environment
	@rm -f $(foreach file,$(AOBJS),$(OBJ_PATH)/$(file))
	@rm -f $(foreach file,$(COBJS),$(OBJ_PATH)/$(file))
	@rm -f $(patsubst %.o,%.lst,$(foreach file,$(AOBJS),$(OBJ_PATH)/$(file)))
	@rm -f $(patsubst %.o,%.lst,$(foreach file,$(COBJS),$(OBJ_PATH)/$(file)))
	@rm -f $(BIN_PATH)/$(PROJ_NAME).sx $(BIN_PATH)/$(PROJ_NAME).map $(BIN_PATH)/$(PROJ_NAME).elf
	@echo +++ Clean complete

                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         
