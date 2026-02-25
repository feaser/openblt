/*
 * Copyright (c) 2015 Freescale Semiconductor, Inc.
 * Copyright 2016-2021, 2023 NXP
 *                                                                                                                                                                                                                                                     
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef DEVASSERT_H
#define DEVASSERT_H

#include <stdbool.h>

/**
 * @page misra_violations MISRA-C:2012 violations
 *
 * @section [global]
 * Violates MISRA 2012 Advisory Rule 2.5, global macro not referenced.
 * The macro is defined to be used by drivers to validate input parameters and can be disabled.
 *
 * @section [global]
 * Violates MISRA 2012 Advisory Directive 4.9, Function-like macro defined.
 * The macros are used to validate input parameters to driver functions.
 *
 */

/**
\page Error_detection_and_reporting Error detection and reporting

S32 SDK drivers can use a mechanism to validate data coming from upper software layers (application code) by performing
a number of checks on input parameters' range or other invariants that can be statically checked (not dependent on
runtime conditions). A failed validation is indicative of a software bug in application code, therefore it is important
to use this mechanism during development.

The validation is performed by using DEV_ASSERT macro.
A default implementation of this macro is provided in this file. However, application developers can provide their own
implementation in a custom file. This requires defining the CUSTOM_DEVASSERT symbol with the specific file name in the
project configuration (for example: -DCUSTOM_DEVASSERT="custom_devassert.h")

The default implementation accommodates two behaviors, based on DEV_ERROR_DETECT symbol:
 - When DEV_ERROR_DETECT symbol is defined in the project configuration (for example: -DDEV_ERROR_DETECT), the validation
   performed by the DEV_ASSERT macro is enabled, and a failed validation triggers a software breakpoint and further execution is
   prevented (application spins in an infinite loop)
   This configuration is recommended for development environments, as it prevents further execution and allows investigating
   potential problems from the point of error detection.
 - When DEV_ERROR_DETECT symbol is not defined, the DEV_ASSERT macro is implemented as no-op, therefore disabling all validations.
   This configuration can be used to eliminate the overhead of development-time checks.

It is the application developer's responsibility to decide the error detection strategy for production code: one can opt to
disable development-time checking altogether (by not defining DEV_ERROR_DETECT symbol), or one can opt to keep the checks
in place and implement a recovery mechanism in case of a failed validation, by defining CUSTOM_DEVASSERT to point
to the file containing the custom implementation.
*/

#if defined (CUSTOM_DEVASSERT)
    /* If the CUSTOM_DEVASSERT symbol is defined, then add the custom implementation */
    #include CUSTOM_DEVASSERT
#elif defined (DEV_ERROR_DETECT)
    /* Implement default assert macro */
static inline void DevAssert(volatile bool x)
{
    if(x) { } else { BKPT_ASM; for(;;) {} }
}
    #define DEV_ASSERT(x) DevAssert(x)
#else
    /* Assert macro does nothing */
    #define DEV_ASSERT(x) ((void)0)
#endif

#endif /* DEVASSERT_H */

/*******************************************************************************
 * EOF
 ******************************************************************************/
