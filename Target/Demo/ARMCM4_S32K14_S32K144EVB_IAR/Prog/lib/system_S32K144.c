/*
 * Copyright (c) 2015 Freescale Semiconductor, Inc.
 * Copyright 2016-2023 NXP
 *
 * NXP Confidential and Proprietary. This software is owned or controlled by NXP
 * and may only be used strictly in accordance with the applicable license terms.
 * By expressly accepting such terms or by downloading, installing, activating
 * and/or otherwise using the software, you are agreeing that you have read, and
 * that you agree to comply with and are bound by, such license terms. If you do
 * not agree to be bound by the applicable license terms, then you may not retain,
 * install, activate or otherwise use the software.
 */

/**
 * @page misra_violations MISRA-C:2012 violations
 *
 * @section [global]
 * Violates MISRA 2012 Advisory Rule 8.9, An object should be defined at block
 * scope if its identifier only appears in a single function.
 * An object with static storage duration declared at block scope cannot be
 * accessed directly from outside the block.
 *
 * @section [global]
 * Violates MISRA 2012 Advisory Rule 11.4, A conversion should not be performed
 * between a pointer to object and an integer type.
 * The cast is required to initialize a pointer with an unsigned int define,
 * representing an address.
 *
 * @section [global]
 * Violates MISRA 2012 Required Rule 11.6, A cast shall not be performed
 * between pointer to void and an arithmetic type.
 * The cast is required to initialize a pointer with an unsigned int define,
 * representing an address.
 *
 * @section [global]
 * Violates MISRA 2012 Advisory Rule 8.7, External could be made static.
 * Function is defined for usage by application code.
 *
 */

#include "device_registers.h"
#include "system_S32K144.h"
#include "stdbool.h"

/* ----------------------------------------------------------------------------
   -- Core clock
   ---------------------------------------------------------------------------- */

uint32_t SystemCoreClock = DEFAULT_SYSTEM_CLOCK;

/*FUNCTION**********************************************************************
 *
 * Function Name : SystemInit
 * Description   : This function disables the watchdog, enables FPU
 * and the power mode protection if the corresponding feature macro
 * is enabled. SystemInit is called from startup_device file.
 *
 * Implements    : SystemInit_Activity
 *END**************************************************************************/
void SystemInit(void)
{
/**************************************************************************/
                      /* FPU ENABLE*/
/**************************************************************************/
#ifdef ENABLE_FPU
  /* Enable CP10 and CP11 coprocessors */
  S32_SCB->CPACR |= (S32_SCB_CPACR_CP10_MASK | S32_SCB_CPACR_CP11_MASK);
#ifdef  ERRATA_E6940
  /* Disable lazy context save of floating point state by clearing LSPEN bit
   * Workaround for errata e6940 */
  S32_SCB->FPCCR &= ~(S32_SCB_FPCCR_LSPEN_MASK);
#endif
#endif /* ENABLE_FPU */
/**************************************************************************/
                      /* WDOG DISABLE*/
/**************************************************************************/

#if (DISABLE_WDOG)
  /* Write of the WDOG unlock key to CNT register, must be done in order to allow any modifications*/
  IP_WDOG->CNT = (uint32_t ) FEATURE_WDOG_UNLOCK_VALUE;
  /* The dummy read is used in order to make sure that the WDOG registers will be configured only
   * after the write of the unlock value was completed. */
  (void)IP_WDOG->CNT;

  /* Initial write of WDOG configuration register:
   * enables support for 32-bit refresh/unlock command write words,
   * clock select from LPO, update enable, watchdog disabled */
  IP_WDOG->CS  = (uint32_t ) ( (1UL << WDOG_CS_CMD32EN_SHIFT)                       |
                            (FEATURE_WDOG_CLK_FROM_LPO << WDOG_CS_CLK_SHIFT)     |
                            (0U << WDOG_CS_EN_SHIFT)                             |
                            (1U << WDOG_CS_UPDATE_SHIFT)                         );

  /* Configure timeout */
  IP_WDOG->TOVAL = (uint32_t )0xFFFF;
#endif /* (DISABLE_WDOG) */

/**************************************************************************/
            /* ENABLE CACHE */
/**************************************************************************/
#if defined(I_CACHE) && (ICACHE_ENABLE == 1)
  /* Invalidate and enable code cache */
  LMEM->PCCCR = LMEM_PCCCR_INVW0(1) | LMEM_PCCCR_INVW1(1) | LMEM_PCCCR_GO(1) | LMEM_PCCCR_ENCACHE(1);
#endif /* defined(I_CACHE) && (ICACHE_ENABLE == 1) */
}

/*FUNCTION**********************************************************************
 *
 * Function Name : SystemCoreClockUpdate
 * Description   : This function must be called whenever the core clock is changed
 * during program execution. It evaluates the clock register settings and calculates
 * the current core clock.
 *
 * Implements    : SystemCoreClockUpdate_Activity
 *END**************************************************************************/
void SystemCoreClockUpdate(void)
{
  uint32_t SCGOUTClock = 0U;      /* Variable to store output clock frequency of the SCG module */
  uint32_t regValue;              /* Temporary variable */
  uint32_t divider, prediv, multi;
  bool validSystemClockSource = true;
  static const uint32_t fircFreq[] = {
      FEATURE_SCG_FIRC_FREQ0,
  };

  divider = ((IP_SCG->CSR & SCG_CSR_DIVCORE_MASK) >> SCG_CSR_DIVCORE_SHIFT) + 1U;

  switch ((IP_SCG->CSR & SCG_CSR_SCS_MASK) >> SCG_CSR_SCS_SHIFT) {
    case 0x1:
      /* System OSC */
      SCGOUTClock = CPU_XTAL_CLK_HZ;
      break;
    case 0x2:
      /* Slow IRC */
      regValue = (IP_SCG->SIRCCFG & SCG_SIRCCFG_RANGE_MASK) >> SCG_SIRCCFG_RANGE_SHIFT;

      if (regValue != 0U)
      {
        SCGOUTClock = FEATURE_SCG_SIRC_HIGH_RANGE_FREQ;
      }

      break;
    case 0x3:
      /* Fast IRC */
      regValue = (IP_SCG->FIRCCFG & SCG_FIRCCFG_RANGE_MASK) >> SCG_FIRCCFG_RANGE_SHIFT;
      SCGOUTClock= fircFreq[regValue];
      break;
    case 0x6:
      /* System PLL */
      SCGOUTClock = CPU_XTAL_CLK_HZ;
      prediv = ((IP_SCG->SPLLCFG & SCG_SPLLCFG_PREDIV_MASK) >> SCG_SPLLCFG_PREDIV_SHIFT) + 1U;
      multi = ((IP_SCG->SPLLCFG & SCG_SPLLCFG_MULT_MASK) >> SCG_SPLLCFG_MULT_SHIFT) + 16U;
      SCGOUTClock = SCGOUTClock * multi / (prediv * 2U);
      break;
    default:
      validSystemClockSource = false;
      break;
  }

  if (validSystemClockSource == true) {
     SystemCoreClock = (SCGOUTClock / divider);
  }
}

/*FUNCTION**********************************************************************
 *
 * Function Name : SystemSoftwareReset
 * Description   : This function is used to initiate a system reset
 *
 * Implements    : SystemSoftwareReset_Activity
 *END**************************************************************************/
void SystemSoftwareReset(void)
{
    uint32_t regValue;

    /* Read Application Interrupt and Reset Control Register */
    regValue = S32_SCB->AIRCR;

    /* Clear register key */
    regValue &= ~( S32_SCB_AIRCR_VECTKEY_MASK);

    /* Configure System reset request bit and Register Key */
    regValue |= S32_SCB_AIRCR_VECTKEY(FEATURE_SCB_VECTKEY);
    regValue |= S32_SCB_AIRCR_SYSRESETREQ(0x1u);

    /* Write computed register value */
    S32_SCB->AIRCR = regValue;
}

/*******************************************************************************
 * EOF
 ******************************************************************************/
