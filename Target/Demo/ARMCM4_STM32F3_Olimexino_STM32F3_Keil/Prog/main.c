/************************************************************************************//**
* \file         Demo/ARMCM4_STM32F3_Olimexino_STM32F3_Keil/Prog/main.c
* \brief        Demo program application source file.
* \ingroup      Prog_ARMCM4_STM32F3_Olimexino_STM32F3_Keil
* \internal
*----------------------------------------------------------------------------------------
*                          C O P Y R I G H T
*----------------------------------------------------------------------------------------
*   Copyright (c) 2016  by Feaser    http://www.feaser.com    All rights reserved
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
#include "header.h"                                    /* generic header               */


/****************************************************************************************
* Type definitions
****************************************************************************************/
/** \brief Layout of the firmware info table type. Note that this is what you need to
 *         typecast the pointers to in the bootloader's InfoTableCheckHook() function.
 *         Feel free to add more or less information to this info table as you see fit.
 */
typedef struct t_firmware_info_table
{
  unsigned long tableId;         /**< fixed value for identification as an info table. */
  unsigned long productId;       /**< product identification. E.g. 1234 = Airpump.     */
  unsigned long firmwareVersion; /**< firmware version. E.g. 10429 = v1.4.29           */
} tFirmwareInfoTable;


/****************************************************************************************
* Function prototypes
****************************************************************************************/
static void Init(void);
static void SystemClock_Config(void);
static void VectorBase_Config(void);


/****************************************************************************************
* Global constant declarations
****************************************************************************************/
/** \brief     The firmware info table.
 *  \details   Note that this constant table is linked to the ".ARM.__at_0x0800A200". 
 *             section. This section is automatically created by the linker and its start
 *             address is extracted from the hexadecimal address value, embedded in the
 *             section name ".ARM.__at_0x0800A200", so 0x0800A200. This is 0x200 bytes
 *             after the start address of our firmware. This is also right after the
 *             "boot block" as defined by the bootloader's flash driver.
 *             This start address was configured such that this info table appears at the
 *             same location as configured with the BOOT_INFO_TABLE_ADDR configuration
 *             macro and has the same length as configured with the BOOT_INFO_TABLE_LEN
 *             configuration macro.
 *
 *             Instead of adding this table directly in the code of the firmware, an
 *             alternative approach would be to patch it into the firmware file during a
 *             postbuild operation. A free application such as "srec_cat" can be
 *             used. It's also possible to build your own patch tool based on LibOpenBLT,
 *             since this library includes firmware file reading and editing
 *             functionality.
 *  \attention When using the bootloader's default internal signature checksum
 *             verification mechanism, this info table should not be located in the area
 *             that the bootloader's flash driver calls the "boot block", unless the
 *             info table itself is located before the actual location configured for the
 *             signature checksum value. The "boot block" is defined as the first
 *             FLASH_WRITE_BLOCK_SIZE bytes of the user program's flash.
 *
 *             If the info table were to be located in the "boot block" and after the
 *             actual signature checksum location, a miniscule chance is present that a
 *             power loss during the write of this "boot block" results in a valid
 *             signature checksum value, yet only partially written (and therefore
 *             invalid) info table.
 *
 *             Note that this restriction does not apply when using the Improved Checksum
 *             add-on module, instead of the default internal signature checksum. More
 *             info regarding the Improved Checksum add-on module can be found here:
 *             - https://www.feaser.com/en/addons.php#checksum
 */
const tFirmwareInfoTable firmwareInfoTable __attribute__((section(".ARM.__at_0x0800A200"))) =
{
  .tableId = 0x9A4B8107UL,
  .productId = 1234UL,
  .firmwareVersion = 10429UL
};


/************************************************************************************//**
** \brief     This is the entry point for the bootloader application and is called 
**            by the reset interrupt vector after the C-startup routines executed.
** \return    Program exit code.
**
****************************************************************************************/
int main(void)
{
  /* Initialize the microcontroller */
  Init();
  /* Initialize the bootloader interface */
  BootComInit();

  /* start the infinite program loop */
  while (1)
  {
    /* Toggle LED with a fixed frequency. */
    LedToggle();
    /* Check for bootloader activation request. */
    BootComCheckActivationRequest();
  }
  /* Set program exit code. note that the program should never get here. */
  return 0;
} /*** end of main ***/


/************************************************************************************//**
** \brief     Initializes the microcontroller.
** \return    none.
**
****************************************************************************************/
static void Init(void)
{
  /* Configure the vector table base address. */
  VectorBase_Config();
  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();
  /* Configure the system clock. */
  SystemClock_Config();
  /* Initialize the timer driver. */
  TimerInit();
  /* Initialize the led driver. */
  LedInit();
} /*** end of Init ***/


/************************************************************************************//**
** \brief     Vector base address configuration. It should no longer be at the start of
**            flash memory but moved forward because the first part of flash is
**            reserved for the bootloader. Note that this is already done by the
**            bootloader before starting this program. Unfortunately, function
**            SystemInit() overwrites this change again. 
** \return    none.
**
****************************************************************************************/
static void VectorBase_Config(void)
{
  /* The constant array with vectors of the vector table is declared externally in the
   * c-startup code.
   */
  extern const unsigned long __Vectors[];

  /* Remap the vector table to where the vector table is located for this program. */
  SCB->VTOR = (unsigned long)&__Vectors[0];
} /*** end of VectorBase_Config ***/


/************************************************************************************//**
** \brief     System Clock Configuration. This code was created by CubeMX and configures
**            the system clock.
** \return    none.
**
****************************************************************************************/
static void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};
  
  /* Initializes the RCC Oscillators according to the specified parameters. */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    /* Clock configuration incorrect or hardware failure. Hang the system to prevent
     * damage.
     */
    while(1);
  }
  
  /* Initializes the CPU, AHB and APB buses clocks. */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK |
                                RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    /* Clock configuration incorrect or hardware failure. Hang the system to prevent
     * damage.
     */
    while(1);
  }

  /* Select peripheral source clocks. */  
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART3;
  PeriphClkInit.Usart3ClockSelection = RCC_USART3CLKSOURCE_PCLK1;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    /* Clock selection incorrect or hardware failure. Hang the system to prevent
     * damage.
     */
    while(1);
  }
} /*** end of SystemClock_Config ***/


/************************************************************************************//**
** \brief     Initializes the Global MSP. This function is called from HAL_Init()
**            function to perform system level initialization (GPIOs, clock, DMA,
**            interrupt).
** \return    none.
**
****************************************************************************************/
void HAL_MspInit(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* SYSCFG and PWR clock enable. */
  __HAL_RCC_SYSCFG_CLK_ENABLE();
  __HAL_RCC_PWR_CLK_ENABLE();
  
  /* GPIO ports clock enable. */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();

#if (BOOT_COM_RS232_ENABLE > 0)
  /* Peripheral clock enable. */
    __HAL_RCC_USART3_CLK_ENABLE();
#endif /* BOOT_COM_RS232_ENABLE > 0 */
#if (BOOT_COM_CAN_ENABLE > 0)
  /* Peripheral clock enable. */
    __HAL_RCC_CAN1_CLK_ENABLE();
#endif /* BOOT_COM_CAN_ENABLE > 0 */
  
  /* Configure the LED GPIO pin. */
  GPIO_InitStruct.Pin = GPIO_PIN_5;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
#if (BOOT_COM_RS232_ENABLE > 0)
  /* UART TX and RX GPIO pin configuration. */
  GPIO_InitStruct.Pin = GPIO_PIN_10 | GPIO_PIN_11;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF7_USART3;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
#endif /* BOOT_COM_RS232_ENABLE > 0 */
#if (BOOT_COM_CAN_ENABLE > 0)
  /* CAN TX and RX GPIO pin configuration. */
  GPIO_InitStruct.Pin = GPIO_PIN_8 | GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF9_CAN;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
#endif /* BOOT_COM_CAN_ENABLE > 0 */
} /*** end of HAL_MspInit ***/


/************************************************************************************//**
** \brief     Deinitializes the Global MSP. This function is called from HAL_DeInit()
**            function to perform system level Deinitialization (GPIOs, clock, DMA,
**            interrupt).
** \return    none.
**
****************************************************************************************/
void HAL_MspDeInit(void)
{
#if (BOOT_COM_CAN_ENABLE > 0)
  /* Reset CAN GPIO pin configuration. */
  HAL_GPIO_DeInit(GPIOB, GPIO_PIN_8 | GPIO_PIN_9);
#endif /* BOOT_COM_CAN_ENABLE > 0 */
#if (BOOT_COM_RS232_ENABLE > 0)
  /* Reset UART GPIO pin configuration. */
  HAL_GPIO_DeInit(GPIOB, GPIO_PIN_10 | GPIO_PIN_11);
#endif /* BOOT_COM_RS232_ENABLE > 0 */
  /* Deconfigure GPIO pin for the LED. */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
  HAL_GPIO_DeInit(GPIOA, GPIO_PIN_5);

#if (BOOT_COM_CAN_ENABLE > 0)
  /* Peripheral clock enable. */
  __HAL_RCC_CAN1_CLK_DISABLE();
#endif /* BOOT_COM_CAN_ENABLE > 0 */
#if (BOOT_COM_RS232_ENABLE > 0)
  /* Peripheral clock disable. */
  __HAL_RCC_USART3_CLK_DISABLE();
#endif /* BOOT_COM_RS232_ENABLE > 0 */
  /* GPIO ports clock disable. */
  __HAL_RCC_GPIOB_CLK_DISABLE();
  __HAL_RCC_GPIOA_CLK_DISABLE();
  /* Power and SYSCFG clock disable. */
  __HAL_RCC_PWR_CLK_DISABLE();
  __HAL_RCC_SYSCFG_CLK_DISABLE();
} /*** end of HAL_MspDeInit ***/


/*********************************** end of main.c *************************************/
