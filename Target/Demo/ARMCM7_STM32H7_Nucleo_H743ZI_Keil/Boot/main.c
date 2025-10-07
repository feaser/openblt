/************************************************************************************//**
* \file         Demo/ARMCM7_STM32H7_Nucleo_H743ZI_Keil/Boot/main.c
* \brief        Bootloader application source file.
* \ingroup      Boot_ARMCM7_STM32H7_Nucleo_H743ZI_Keil
* \internal
*----------------------------------------------------------------------------------------
*                          C O P Y R I G H T
*----------------------------------------------------------------------------------------
*   Copyright (c) 2020  by Feaser    http://www.feaser.com    All rights reserved
*
*----------------------------------------------------------------------------------------
*                            L I C E N S E
*----------------------------------------------------------------------------------------
* This file is part of OpenBLT. OpenBLT is free software: you can redistribute it and/or
* modify it under the terms of the GNU General Public License as published by the Free
* Software Foundation, either version 3 of the License, or (at your option) any later
* version.
*
* OpenBLT is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
* without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
* PURPOSE. See the GNU General Public License for more details.
*
* You have received a copy of the GNU General Public License along with OpenBLT. It
* should be located in ".\Doc\license.html". If not, contact Feaser to obtain a copy.
*
* \endinternal
****************************************************************************************/

/****************************************************************************************
* Include files
****************************************************************************************/
#include "boot.h"                                /* bootloader generic header          */
#include "stm32h7xx.h"                           /* STM32 CPU and HAL header           */
#include "stm32h7xx_ll_pwr.h"                    /* STM32 LL PWR header                */
#include "stm32h7xx_ll_cortex.h"                 /* STM32 LL Cortex header             */
#include "stm32h7xx_ll_rcc.h"                    /* STM32 LL RCC header                */
#include "stm32h7xx_ll_utils.h"                  /* STM32 LL UTILS header              */
#include "stm32h7xx_ll_gpio.h"                   /* STM32 LL GPIO header               */
#include "shared_params.h"                       /* Shared parameters header           */


/****************************************************************************************
* Function prototypes
****************************************************************************************/
static void Init(void);
static void SystemClock_Config(void);
static void MPU_Config(void);


/************************************************************************************//**
** \brief     This is the entry point for the bootloader application and is called
**            by the reset interrupt vector after the C-startup routines executed.
** \return    Program return code.
**
****************************************************************************************/
int main(void)
{
  blt_int8u deferredInitRequestFlag = 0;

  /* initialize the microcontroller */
  Init();
  /* initialize the shared parameters module */
  SharedParamsInit();
  /* initialize the bootloader */
  BootInit();
#if (BOOT_COM_DEFERRED_INIT_ENABLE == 1)
  /* the bootloader is configured to NOT initialize the TCP/IP network stack by default
   * to bypass unnecessary delay times before starting the user program. the TCP/IP net-
   * work tack is now only initialized when: (a) no valid user program is detected, or
   * (b) a forced backdoor entry occurred (CpuUserProgramStartHook() returned BLT_FALSE).
   *
   * these demo bootloader and user programs have one extra feature implemented for
   * demonstration purposes. the demo user program can detect firmware update requests
   * from the TCP/IP network in which case it activates the bootloader. But...the
   * TCP/IP network stack will not be initialized in this situation. for this reason
   * the shared parameter module was integrated in both the bootloader and user program.
   * more information about the shared parameter module can be found here:
   *   https://www.feaser.com/en/blog/?p=216
   *
   * the shared parameter at the first index (0) contains a flag. this flag is set to
   * 1, right before the user program activates this bootloader, to explicitly request
   * the bootloader to initialize the TCP/IP network stack. this makes it possible for
   * a firmware update to proceed. the code here reads out this flag and performs the
   * TCP/IP network stack initialization when requested.
   */
  SharedParamsReadByIndex(0, &deferredInitRequestFlag);
  if (deferredInitRequestFlag == 1)
  {
    /* explicitly initialize all communication interface for which the deferred
     * initialization feature was enabled.
     */
    ComDeferredInit();
  }
#endif
  
  /* start the infinite program loop */
  while (1)
  {
    /* run the bootloader task */
    BootTask();
  }

  /* program should never get here */
  return 0;
} /*** end of main ***/


/************************************************************************************//**
** \brief     Initializes the microcontroller.
** \return    none.
**
****************************************************************************************/
static void Init(void)
{
  /* configure MPU */
  MPU_Config();
  /* HAL library initialization */
  HAL_Init();
  /* configure system clock */
  SystemClock_Config();
} /*** end of Init ***/


/************************************************************************************//**
** \brief     System Clock Configuration. This code was created by CubeMX and configures
**            the system clock to match the configuration in the bootloader's
**            configuration (blt_conf.h), specifically the macros:
**            BOOT_CPU_SYSTEM_SPEED_KHZ and BOOT_CPU_XTAL_SPEED_KHZ.
**            Note that the Lower Layer drivers were selected in CubeMX for the RCC
**            subsystem.
** \return    none.
**
****************************************************************************************/
static void SystemClock_Config(void)
{
  /* Set flash latency. */
  LL_FLASH_SetLatency(LL_FLASH_LATENCY_4);
  if (LL_FLASH_GetLatency() != LL_FLASH_LATENCY_4)
  {
    /* Error setting flash latency. */
    ASSERT_RT(BLT_FALSE);
  }

  /* Configure the main internal regulator output voltage. */
  LL_PWR_ConfigSupply(LL_PWR_LDO_SUPPLY);
  LL_PWR_SetRegulVoltageScaling(LL_PWR_REGU_VOLTAGE_SCALE0);

  /* Enable the HSE clock. */
  LL_RCC_HSE_EnableBypass();
  LL_RCC_HSE_Enable();

  /* Wait till HSE is ready. */
  while (LL_RCC_HSE_IsReady() != 1)
  {
    ;
  }

  /* Configure and enable the PLL. */
  LL_RCC_PLL_SetSource(LL_RCC_PLLSOURCE_HSE);
  LL_RCC_PLL1P_Enable();
  LL_RCC_PLL1Q_Enable();
  LL_RCC_PLL1_SetVCOInputRange(LL_RCC_PLLINPUTRANGE_2_4);
  LL_RCC_PLL1_SetVCOOutputRange(LL_RCC_PLLVCORANGE_WIDE);
  LL_RCC_PLL1_SetM(4);
  LL_RCC_PLL1_SetN(480);
  LL_RCC_PLL1_SetP(2);
  LL_RCC_PLL1_SetQ(20);
  LL_RCC_PLL1_SetR(2);
  LL_RCC_PLL1_Enable();
  /* Wait till PLL is ready. */
  while (LL_RCC_PLL1_IsReady() != 1)
  {
    ;
  }
  
  /* Configure and enable the PLL2. */
  LL_RCC_PLL2Q_Enable();
  LL_RCC_PLL2_SetVCOInputRange(LL_RCC_PLLINPUTRANGE_8_16);
  LL_RCC_PLL2_SetVCOOutputRange(LL_RCC_PLLVCORANGE_MEDIUM);
  LL_RCC_PLL2_SetM(1);
  LL_RCC_PLL2_SetN(20);
  LL_RCC_PLL2_SetP(2);
  LL_RCC_PLL2_SetQ(2);
  LL_RCC_PLL2_SetR(2);
  LL_RCC_PLL2_Enable();

   /* Wait till PLL is ready */
  while(LL_RCC_PLL2_IsReady() != 1)
  {
    ;
  }

  /* Intermediate AHB prescaler 2 when target frequency clock is higher than 80 MHz */
  LL_RCC_SetAHBPrescaler(LL_RCC_AHB_DIV_2);

  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL1);
  LL_RCC_SetSysPrescaler(LL_RCC_SYSCLK_DIV_1);
  LL_RCC_SetAHBPrescaler(LL_RCC_AHB_DIV_2);
  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_2);
  LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_2);
  LL_RCC_SetAPB3Prescaler(LL_RCC_APB3_DIV_2);
  LL_RCC_SetAPB4Prescaler(LL_RCC_APB4_DIV_2);

  /* Configure peripheral clock sources. */
  LL_RCC_SetUSARTClockSource(LL_RCC_USART234578_CLKSOURCE_PCLK1);
  LL_RCC_SetFDCANClockSource(LL_RCC_FDCAN_CLKSOURCE_PLL2Q);
  LL_RCC_SetUSBClockSource(LL_RCC_USB_CLKSOURCE_PLL1Q);

  /* Update the system clock speed setting. */
  LL_SetSystemCoreClock(BOOT_CPU_SYSTEM_SPEED_KHZ * 1000u);
} /*** end of SystemClock_Config ***/


/************************************************************************************//**
** \brief     Memory Protection Unit Configuration. This code was created by CubeMX and 
**            configures the MPU. Note that the Lower Layer drivers were selected in
**            CubeMX for the MPU subsystem.
** \return    none.
**
****************************************************************************************/
static void MPU_Config(void)
{
  /* Disables the MPU. */
  LL_MPU_Disable();
  
  /* Disable speculative access to unused memory. Overall a good idea and start for the
   * MPU configuration.
   */
  LL_MPU_ConfigRegion(LL_MPU_REGION_NUMBER0, 0x87, 0x0, LL_MPU_REGION_SIZE_4GB|LL_MPU_TEX_LEVEL0|LL_MPU_REGION_NO_ACCESS|LL_MPU_INSTRUCTION_ACCESS_DISABLE|LL_MPU_ACCESS_SHAREABLE|LL_MPU_ACCESS_NOT_CACHEABLE|LL_MPU_ACCESS_NOT_BUFFERABLE);
  LL_MPU_EnableRegion(LL_MPU_REGION_NUMBER0);

  /* Initializes and configures the region with the Ethernet Tx and Rx descriptors and
   * buffers as "device" memory type that can be shared. This is needed because the DMA
   * needs to access it.
   */
  LL_MPU_ConfigRegion(LL_MPU_REGION_NUMBER1, 0x0, 0x30040000, LL_MPU_REGION_SIZE_32KB|LL_MPU_TEX_LEVEL0|LL_MPU_REGION_FULL_ACCESS|LL_MPU_INSTRUCTION_ACCESS_DISABLE|LL_MPU_ACCESS_SHAREABLE|LL_MPU_ACCESS_NOT_CACHEABLE|LL_MPU_ACCESS_BUFFERABLE);
  LL_MPU_EnableRegion(LL_MPU_REGION_NUMBER1);

  /* Initializes and configures the region with the shared parameters RAM buffer as
   * "normal" memory type that is "non-cacheable" and "shareable". Otherwise the shared
   * parameters contents at not always committed to RAM when D-cache is enabled. Not
   * absolutely necessary for the bootloader as it disabled the D-cache, yet kept here
   * for future references.
   */
  LL_MPU_ConfigRegion(LL_MPU_REGION_NUMBER2, 0x0, 0x24000000, LL_MPU_REGION_SIZE_64B|LL_MPU_TEX_LEVEL1|LL_MPU_REGION_FULL_ACCESS|LL_MPU_INSTRUCTION_ACCESS_DISABLE|LL_MPU_ACCESS_SHAREABLE|LL_MPU_ACCESS_NOT_CACHEABLE|LL_MPU_ACCESS_NOT_BUFFERABLE);
  LL_MPU_EnableRegion(LL_MPU_REGION_NUMBER2);

  /* Enables the MPU. */
  LL_MPU_Enable(LL_MPU_CTRL_PRIVILEGED_DEFAULT);
} /*** end of MPU_Config ***/


/************************************************************************************//**
** \brief     Initializes the Global MSP. This function is called from HAL_Init()
**            function to perform system level initialization (GPIOs, clock, DMA,
**            interrupt).
** \return    none.
**
****************************************************************************************/
void HAL_MspInit(void)
{
  LL_GPIO_InitTypeDef GPIO_InitStruct;

  /* SYSCFG clock enable. */
  LL_APB4_GRP1_EnableClock(LL_APB4_GRP1_PERIPH_SYSCFG);

  /* GPIO ports clock enable. */
  LL_AHB4_GRP1_EnableClock(LL_AHB4_GRP1_PERIPH_GPIOA);
  LL_AHB4_GRP1_EnableClock(LL_AHB4_GRP1_PERIPH_GPIOB);
  LL_AHB4_GRP1_EnableClock(LL_AHB4_GRP1_PERIPH_GPIOC);
  LL_AHB4_GRP1_EnableClock(LL_AHB4_GRP1_PERIPH_GPIOD);

#if (BOOT_COM_RS232_ENABLE > 0)
  /* UART clock enable. */
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_USART3);
#endif

#if (BOOT_COM_CAN_ENABLE > 0)
  /* CAN clock enable. */
  LL_APB1_GRP2_EnableClock(LL_APB1_GRP2_PERIPH_FDCAN);
#endif

  /* Configure GPIO pin for the LED. */
  GPIO_InitStruct.Pin = LL_GPIO_PIN_7;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* Configure GPIO pin for (optional) backdoor entry input. */
  GPIO_InitStruct.Pin = LL_GPIO_PIN_13;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(GPIOC, &GPIO_InitStruct);

#if (BOOT_COM_RS232_ENABLE > 0)
  /* UART TX and RX GPIO pin configuration. */
  GPIO_InitStruct.Pin = LL_GPIO_PIN_8 | LL_GPIO_PIN_9;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_7;
  LL_GPIO_Init(GPIOD, &GPIO_InitStruct);
#endif

#if (BOOT_COM_CAN_ENABLE > 0)
  /* CAN TX and RX GPIO pin configuration. */
  GPIO_InitStruct.Pin = LL_GPIO_PIN_8 | LL_GPIO_PIN_9;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_9;
  LL_GPIO_Init(GPIOB, &GPIO_InitStruct);
#endif

#if (BOOT_COM_USB_ENABLE > 0)
  /* USB pin configuration. */
  GPIO_InitStruct.Pin = LL_GPIO_PIN_11 | LL_GPIO_PIN_12;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_10;
  LL_GPIO_Init(GPIOA, &GPIO_InitStruct);
#endif

#if (BOOT_COM_USB_ENABLE > 0)
  /* USB clock enable. */
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_USB2OTGHS);
#endif
} /*** end of HAL_MspInit ***/


/************************************************************************************//**
** \brief     DeInitializes the Global MSP. This function is called from HAL_DeInit()
**            function to perform system level de-initialization (GPIOs, clock, DMA,
**            interrupt).
** \return    none.
**
****************************************************************************************/
void HAL_MspDeInit(void)
{
  /* Reset the RCC clock configuration to the default reset state. */
  LL_RCC_DeInit();
  
  /* Reset GPIO pin for the LED to turn it off. */
  LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_7);

  /* Deinit used GPIOs. */
  LL_GPIO_DeInit(GPIOD);
  LL_GPIO_DeInit(GPIOC);
  LL_GPIO_DeInit(GPIOB);
  LL_GPIO_DeInit(GPIOA);

#if (BOOT_COM_USB_ENABLE > 0)
  /* USB clock disable. */
  LL_AHB1_GRP1_DisableClock(LL_AHB1_GRP1_PERIPH_USB2OTGHS);
#endif

#if (BOOT_COM_CAN_ENABLE > 0)
  /* CAN clock disable. */
  LL_APB1_GRP2_DisableClock(LL_APB1_GRP2_PERIPH_FDCAN);
#endif

#if (BOOT_COM_RS232_ENABLE > 0)
  /* UART clock disable. */
  LL_APB1_GRP1_DisableClock(LL_APB1_GRP1_PERIPH_USART3);
#endif

  /* GPIO ports clock disable. */
  LL_AHB4_GRP1_DisableClock(LL_AHB4_GRP1_PERIPH_GPIOD);
  LL_AHB4_GRP1_DisableClock(LL_AHB4_GRP1_PERIPH_GPIOC);
  LL_AHB4_GRP1_DisableClock(LL_AHB4_GRP1_PERIPH_GPIOB);
  LL_AHB4_GRP1_DisableClock(LL_AHB4_GRP1_PERIPH_GPIOA);

  /* SYSCFG clock disable. */
  LL_APB4_GRP1_DisableClock(LL_APB4_GRP1_PERIPH_SYSCFG);
  /* Disable the MPU and reset the region configuration. */
  LL_MPU_Disable();
  LL_MPU_DisableRegion(LL_MPU_REGION_NUMBER0);
  LL_MPU_DisableRegion(LL_MPU_REGION_NUMBER1);
  LL_MPU_DisableRegion(LL_MPU_REGION_NUMBER2);
} /*** end of HAL_MspDeInit ***/


/************************************************************************************//**
** \brief     This function handles the SysTick interrupt. The HAL driver is initialized
**            before the bootloader disables the global interrupts and reconfigures the
**            SysTick. It is theoretically possible that the SysTick interrupt still
**            fires before the timer driver disables it. Therefore the handler is
**            implemented here. If not, then the default handler from the C startup 
**            code is used, which hangs the system.
** \return    none.
**
****************************************************************************************/
void SysTick_Handler(void)
{
  /* Nothing to do here. */
} /*** end of SysTick_Handler ***/


/*********************************** end of main.c *************************************/
