#****************************************************************************************
#|  Description: Makefile for STM32 using CodeSourcery GNU GCC compiler toolset
#|    File Name: makefile
#|
#|---------------------------------------------------------------------------------------
#|                          C O P Y R I G H T
#|---------------------------------------------------------------------------------------
#|   Copyright (c) 2013  by Feaser    http://www.feaser.com    All rights reserved
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
PROJ_NAME=openbtl_olimex_stm32e407


#|---------------------------------------------------------------------------------------|
#| Speficy project source files                                                          |
#|---------------------------------------------------------------------------------------|
PROJ_FILES= \
blt_conf.h \
hooks.c \
main.c \
usbd_bulk.c \
usbd_bulk.h \
usbd_conf.h \
usbd_desc.c \
usbd_desc.h \
usbd_usr.c \
usb_bsp.c \
usb_conf.h \
./lib/usbdevicelib/Core/inc/usbd_core.h \
./lib/usbdevicelib/Core/inc/usbd_def.h \
./lib/usbdevicelib/Core/inc/usbd_ioreq.h \
./lib/usbdevicelib/Core/inc/usbd_req.h \
./lib/usbdevicelib/Core/inc/usbd_usr.h \
./lib/usbdevicelib/Core/src/usbd_core.c \
./lib/usbdevicelib/Core/src/usbd_ioreq.c \
./lib/usbdevicelib/Core/src/usbd_req.c \
./lib/usbotgdriver/inc/usb_bsp.h \
./lib/usbotgdriver/inc/usb_core.h \
./lib/usbotgdriver/inc/usb_dcd.h \
./lib/usbotgdriver/inc/usb_dcd_int.h \
./lib/usbotgdriver/inc/usb_defines.h \
./lib/usbotgdriver/inc/usb_regs.h \
./lib/usbotgdriver/src/usb_core.c \
./lib/usbotgdriver/src/usb_dcd.c \
./lib/usbotgdriver/src/usb_dcd_int.c \
./lib/fatfs/ffconf.h \
./lib/fatfs/mmc.c \
./lib/ethernetlib/inc/stm32_eth.h \
./lib/ethernetlib/src/stm32_eth.c \
./lib/uip/clock-arch.c \
./lib/uip/clock-arch.h \
./lib/uip/netdev.c \
./lib/uip/netdev.h \
./lib/uip/uip-conf.h \
./lib/stdperiphlib/stm32f4xx_conf.h \
./lib/stdperiphlib/STM32F4xx_StdPeriph_Driver/inc/misc.h \
./lib/stdperiphlib/STM32F4xx_StdPeriph_Driver/inc/stm32f4xx_adc.h \
./lib/stdperiphlib/STM32F4xx_StdPeriph_Driver/inc/stm32f4xx_can.h \
./lib/stdperiphlib/STM32F4xx_StdPeriph_Driver/inc/stm32f4xx_crc.h \
./lib/stdperiphlib/STM32F4xx_StdPeriph_Driver/inc/stm32f4xx_cryp.h \
./lib/stdperiphlib/STM32F4xx_StdPeriph_Driver/inc/stm32f4xx_dac.h \
./lib/stdperiphlib/STM32F4xx_StdPeriph_Driver/inc/stm32f4xx_dbgmcu.h \
./lib/stdperiphlib/STM32F4xx_StdPeriph_Driver/inc/stm32f4xx_dcmi.h \
./lib/stdperiphlib/STM32F4xx_StdPeriph_Driver/inc/stm32f4xx_dma.h \
./lib/stdperiphlib/STM32F4xx_StdPeriph_Driver/inc/stm32f4xx_exti.h \
./lib/stdperiphlib/STM32F4xx_StdPeriph_Driver/inc/stm32f4xx_flash.h \
./lib/stdperiphlib/STM32F4xx_StdPeriph_Driver/inc/stm32f4xx_fsmc.h \
./lib/stdperiphlib/STM32F4xx_StdPeriph_Driver/inc/stm32f4xx_gpio.h \
./lib/stdperiphlib/STM32F4xx_StdPeriph_Driver/inc/stm32f4xx_hash.h \
./lib/stdperiphlib/STM32F4xx_StdPeriph_Driver/inc/stm32f4xx_i2c.h \
./lib/stdperiphlib/STM32F4xx_StdPeriph_Driver/inc/stm32f4xx_iwdg.h \
./lib/stdperiphlib/STM32F4xx_StdPeriph_Driver/inc/stm32f4xx_pwr.h \
./lib/stdperiphlib/STM32F4xx_StdPeriph_Driver/inc/stm32f4xx_rcc.h \
./lib/stdperiphlib/STM32F4xx_StdPeriph_Driver/inc/stm32f4xx_rng.h \
./lib/stdperiphlib/STM32F4xx_StdPeriph_Driver/inc/stm32f4xx_rtc.h \
./lib/stdperiphlib/STM32F4xx_StdPeriph_Driver/inc/stm32f4xx_sdio.h \
./lib/stdperiphlib/STM32F4xx_StdPeriph_Driver/inc/stm32f4xx_spi.h \
./lib/stdperiphlib/STM32F4xx_StdPeriph_Driver/inc/stm32f4xx_syscfg.h \
./lib/stdperiphlib/STM32F4xx_StdPeriph_Driver/inc/stm32f4xx_tim.h \
./lib/stdperiphlib/STM32F4xx_StdPeriph_Driver/inc/stm32f4xx_usart.h \
./lib/stdperiphlib/STM32F4xx_StdPeriph_Driver/inc/stm32f4xx_wwdg.h \
./lib/stdperiphlib/STM32F4xx_StdPeriph_Driver/src/misc.c \
./lib/stdperiphlib/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_adc.c \
./lib/stdperiphlib/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_can.c \
./lib/stdperiphlib/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_crc.c \
./lib/stdperiphlib/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_cryp.c \
./lib/stdperiphlib/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_cryp_aes.c \
./lib/stdperiphlib/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_cryp_des.c \
./lib/stdperiphlib/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_cryp_tdes.c \
./lib/stdperiphlib/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_dac.c \
./lib/stdperiphlib/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_dbgmcu.c \
./lib/stdperiphlib/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_dcmi.c \
./lib/stdperiphlib/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_dma.c \
./lib/stdperiphlib/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_exti.c \
./lib/stdperiphlib/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_flash.c \
./lib/stdperiphlib/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_fsmc.c \
./lib/stdperiphlib/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_gpio.c \
./lib/stdperiphlib/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_hash.c \
./lib/stdperiphlib/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_hash_md5.c \
./lib/stdperiphlib/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_hash_sha1.c \
./lib/stdperiphlib/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_i2c.c \
./lib/stdperiphlib/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_iwdg.c \
./lib/stdperiphlib/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_pwr.c \
./lib/stdperiphlib/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_rcc.c \
./lib/stdperiphlib/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_rng.c \
./lib/stdperiphlib/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_rtc.c \
./lib/stdperiphlib/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_sdio.c \
./lib/stdperiphlib/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_spi.c \
./lib/stdperiphlib/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_syscfg.c \
./lib/stdperiphlib/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_tim.c \
./lib/stdperiphlib/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_usart.c \
./lib/stdperiphlib/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_wwdg.c \
./lib/stdperiphlib/CMSIS/Include/core_cm4.h \
./lib/stdperiphlib/CMSIS/Include/core_cm4_simd.h \
./lib/stdperiphlib/CMSIS/Include/core_cmFunc.h \
./lib/stdperiphlib/CMSIS/Include/core_cmInstr.h \
./lib/stdperiphlib/CMSIS/Device/ST/STM32F4xx/Source/system_stm32f4xx.c \
./lib/stdperiphlib/CMSIS/Device/ST/STM32F4xx/Include/stm32f4xx.h \
./lib/stdperiphlib/CMSIS/Device/ST/STM32F4xx/Include/system_stm32f4xx.h \
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
../../../Source/boot.c \
../../../Source/boot.h \
../../../Source/com.c \
../../../Source/com.h \
../../../Source/net.c \
../../../Source/net.h \
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
../../../Source/can.h \
../../../Source/cpu.h \
../../../Source/uart.h \
../../../Source/usb.h \
../../../Source/nvm.h \
../../../Source/timer.h \
../../../Source/plausibility.h \
../../../Source/ARMCM4_STM32/types.h \
../../../Source/ARMCM4_STM32/can.c \
../../../Source/ARMCM4_STM32/cpu.c \
../../../Source/ARMCM4_STM32/flash.c \
../../../Source/ARMCM4_STM32/flash.h \
../../../Source/ARMCM4_STM32/uart.c \
../../../Source/ARMCM4_STM32/usb.c \
../../../Source/ARMCM4_STM32/nvm.c \
../../../Source/ARMCM4_STM32/timer.c \
../../../Source/ARMCM4_STM32/GCC/vectors.c \
../../../Source/ARMCM4_STM32/GCC/cstart.c


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
LIB_PATH  = -L../../../Source/ARMCM4_STM32/GCC/


#|---------------------------------------------------------------------------------------|
#| Options for compiler binaries                                                         |
#|---------------------------------------------------------------------------------------|
CFLAGS   = -g -D inline= -mthumb -mcpu=cortex-m4 -O1 -T memory.x
CFLAGS  += -D PACK_STRUCT_END=__attribute\(\(packed\)\) -Wno-main
CFLAGS  += -D ALIGN_STRUCT_END=__attribute\(\(aligned\(4\)\)\) 
CFLAGS  += -ffunction-sections -fdata-sections $(INC_PATH) -D STM32F4XX -D GCC_ARMCM3
CFLAGS  += -D USE_STDPERIPH_DRIVER -D HSE_VALUE=12000000 -D USE_USB_OTG_FS
CFLAGS  += -Wa,-adhlns="$(OBJ_PATH)/$(subst .o,.lst,$@)" -Wno-attributes
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

                                                                                                                                                                                                                                                                