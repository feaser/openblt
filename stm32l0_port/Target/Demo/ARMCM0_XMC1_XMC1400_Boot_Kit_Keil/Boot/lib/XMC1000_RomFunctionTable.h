/*********************************************************************************************************************
 * @file     XMC1000_RomFunctionTable.h
 * @brief    ROM functions prototypes for the XMC1000-Series
 * @version  V1.1
 * @date     06 May 2019
 *
 * @cond
 *********************************************************************************************************************
 * Copyright (c) 2013-2019, Infineon Technologies AG
 * All rights reserved.                        
 *                                             
 * Redistribution and use in source and binary forms, with or without modification,are permitted provided that the 
 * following conditions are met:   
 *                                                                              
 * Redistributions of source code must retain the above copyright notice, this list of conditions and the following 
 * disclaimer.                        
 * 
 * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following 
 * disclaimer in the documentation and/or other materials provided with the distribution.                       
 * 
 * Neither the name of the copyright holders nor the names of its contributors may be used to endorse or promote 
 * products derived from this software without specific prior written permission.                                           
 *                                                                              
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, 
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE  
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE  FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR  
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
 * WHETHER IN CONTRACT, STRICT LIABILITY,OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.                                                  
 *                                                                              
 * To improve the quality of the software, users are encouraged to share modifications, enhancements or bug fixes with 
 * Infineon Technologies AG dave@infineon.com).                                                          
 *********************************************************************************************************************
 *
 **************************** Change history *********************************
 * V1.0, 08 Jan 2013, GD, First version with XMC1000 devices
 * V1.1, 06 May 2019, JFT, Extended ROM functions to incorporate AB step functionality
 *                         _CalcTemperature, _NvmEraseSector, _NvmProgVerifyBlock, _CalcTSEVAR
 *****************************************************************************
 * @endcond 
 */

#ifndef ROM_FUNCTION_TABLE_H
#define ROM_FUNCTION_TABLE_H

#ifdef __cplusplus
extern "C" {
#endif

/* ***************************************************************************
******************************* Constants *******************************
*************************************************************************** */
/* Start address of the ROM function table */
#define ROM_FUNCTION_TABLE_START    (0x00000100U)

/* Pointer to Erase Flash Page routine */
#define _NvmErase                    (ROM_FUNCTION_TABLE_START + 0x00U)

/* Pointer to Erase, Program & Verify Flash Page routine */
#define _NvmProgVerify               (ROM_FUNCTION_TABLE_START + 0x04U)

/* Pointer to Request BMI installation routine */
#define _BmiInstallationReq          (ROM_FUNCTION_TABLE_START + 0x08U)

/* Pointer to Calculate chip temperature routine */
#define _CalcTemperature             (ROM_FUNCTION_TABLE_START + 0x0CU)

/* Pointer to Erase Flash Sector routine */
#define _NvmEraseSector              (ROM_FUNCTION_TABLE_START + 0x10U)

/* Pointer to Program & Verify Flash Block routine */
#define _NvmProgVerifyBlock          (ROM_FUNCTION_TABLE_START + 0x14U)

/* Pointer to Calculate target level for temperature comparison routine */
#define _CalcTSEVAR                  (ROM_FUNCTION_TABLE_START + 0x20U)

/* ***************************************************************************
******************************** Enumerations ********************************
*************************************************************************** */
typedef enum TagNVMStatus
{

/* The function succeeded */
 NVM_PASS               = (int32_t)0x00010000U,
 
/* Generic error code */ 
 NVM_E_FAIL             = (int32_t)0x80010001U,
 
/* Source data not in RAM */ 
 NVM_E_SRC_AREA_EXCCEED = (int32_t)0x80010003U,
 
/* Source data is not 4 byte aligned */ 
 NVM_E_SRC_ALIGNMENT    = (int32_t)0x80010004U,
 
/* NVM module cannot be physically accessed */ 
 NVM_E_NVM_FAIL         = (int32_t)0x80010005U,
 
/* Verification of written page not successful */ 
 NVM_E_VERIFY           = (int32_t)0x80010006U,
 
/* Destination data is not (completely) located in NVM */ 
 NVM_E_DST_AREA_EXCEED  = (int32_t)0x80010009U,
 
/* Destination data is not properly aligned */ 
 NVM_E_DST_ALIGNMENT    = (int32_t)0x80010010U,

} NVM_STATUS;


/****************************************************************************
*********************************** Macros ***********************************
*************************************************************************** */

/****************************************************************************
Description: Erase granularity = 1 Page of  16 blocks of 16 Bytes
                               = Equivalent to 256 Bytes using this routine.

Input parameters:                                                                                                              
- Logical address of the Flash Page to be erased which must be page aligned 
and in NVM address range

Return status:
- OK (NVM_PASS)
- Invalid address (NVM_E_DST_ALIGNMENT or NVM_E_DST_AREA_EXCEED)

Prototype: 
    int32_t XMC1000_NvmErasePage(uint32_t *pageAddr)
****************************************************************************/
#define XMC1000_NvmErasePage (*((int32_t (**) (uint32_t * )) _NvmErase))

/****************************************************************************
Description: This procedure performs erase (skipped if not necessary), program 
and verify of selected Flash page.

Input parameter:                                                                                                              
- Logical address of the target Flash Page, must be page aligned and in NVM 
address range

- Address in SRAM where the data starts, must be 4-byte aligned

Return status:
-  OK (NVM_PASS)
-  Invalid addresses 
                    NVM_E_DST_ALIGNMENT 
                    NVM_E_SRC_ALIGNMENT 
                    NVM_E_DST_AREA_EXCEED 
                    NVM_E_SRC_AREA_EXCCEED
-  Operation failed (Error during low level NVM programming driver): 
                                                                 NVM_E_VERIFY 
                                                                 NVM_E_NVM_FAIL

Prototype: 
  int32_t XMC1000_NvmProgVerify(const uint32_t *srcAddr, uint32_t *dstAddr)
****************************************************************************/
#define XMC1000_NvmProgVerify (*((int32_t (**) (const uint32_t * ,uint32_t * )) _NvmProgVerify))


/****************************************************************************
Description: This procedure initiates installation of a new BMI value. In 
particular, it can be used as well as to restore the state upon delivery for a
device already in User Productive mode. 

Input parameter:                                                                                                              
- BMI value to be installed

Return status:
-  wrong input BMI value (0x01) - only upon error, if OK the procedure triggers
a reset respectively does not return to calling routine !

Prototype: 
  unsigned long XMC1000_BmiInstallationReq(unsigned short requestedBmiValue)
*****************************************************************************/
#define XMC1000_BmiInstallationReq (*((uint32_t (**) (uint16_t)) _BmiInstallationReq))

/****************************************************************************
Description: This procedure calculates the current chip temperature as 
measured by the XMC1000 built-in sensor, based on data from Flash including 
trimming values and pre-calculated constants and data from the actual 
measurement (read from Temperature Sensor Counter2 Monitor Register ANATSEMON).

Input parameter:                                                                                                              
- None

Return status:
- chip temperature in degree Kelvin

Prototype: 
  uint32_t XMC1000_CalcTemperature(void)
*****************************************************************************/
#define XMC1000_CalcTemperature (*((uint32_t (**) (void )) _CalcTemperature))

/****************************************************************************
Description: XMC1000 Flash can be erased with granularity of one sector, i.e. 
 16 pages of (16 blocks of 16 Bytes) = 4K Bytes using this routine.

Input parameter:                                                                                                              
– sectorAddr: logical address of the Flash Sector to be erased, must be in NVM address range

Return status:
– OK (NVM_PASS)
– invalid address (NVM_E_DST_AREA_EXCEEDED, NVM_E_DST_ALIGNMENT)

Prototype: 
  int32_t XMC1000_NvmEraseSector(uint32_t *sectorAddr)
*****************************************************************************/
#define XMC1000_NvmEraseSector (*((int32_t (**) (uint32_t * )) _NvmEraseSector))

/****************************************************************************
Description: XMC1100 Flash can be programmed and verified with granularity of 
one block (4 words of 4 Bytes) = 16 Bytes using this routine.

Input parameter:                                                                                                              
– dstAddr: logical address of the Flash Sector to be erased, must be in NVM address range
- srcAddr: address in SRAM where the data starts 

Return status:
– OK (NVM_PASS)
– invalid addresses (NVM_E_SRC_AREA_EXCEEDED, NVM_E_SRC_ALIGNMENT, 
  NVM_E_DST_AREA_EXCEEDED, NVM_E_DST_ALIGNMENT)
– operation failed (NVM_E_NVM_FAIL, NVM_E_VERIFY)

Prototype: 
  int32_t XMC1000_NvmProgVerifyBlock(const uint32_t *srcAddr, uint32_t *dstAddr)
*****************************************************************************/
#define XMC1000_NvmProgVerifyBlock (*((int32_t (**) (const uint32_t * , uint32_t * )) _NvmProgVerifyBlock))

/****************************************************************************
Description: This procedure, a kind of reverse of Calculate chip temperature, 
calculates the value which must be installed in SCU_ANALOG->ANATSEIH.TSE_IH or 
SCU_ANALOG->ANATSEIL.TSE_IL register to get indication in 
SCU_INTERRUPT->SRRAW.TSE_LOW or SCU_INTERRUPT->SRRAW.TSE_HIGH
when the chip temperature is above/below some target/threshold.

Input parameter:                                                                                                              
– temperature: threshold temperature in degree Kelvin - allowed range 223...423

Return status:
– equivalent sensor threshold value for the temperature provided as input parameter

Prototype: 
  uint32_t XMC1000_CalcTSEVAR(uint32_t temperature)
*****************************************************************************/
#define XMC1000_CalcTSEVAR (*((uint32_t (**) (uint32_t * )) _CalcTSEVAR))

#ifdef __cplusplus
}
#endif

#endif /* ROM_FUNCTION_TABLE_H */
