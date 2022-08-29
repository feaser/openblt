/**
 * \file IfxGeth.c
 * \brief GETH  basic functionality
 *
 * \version iLLD_1_0_1_12_0_1
 * \copyright Copyright (c) 2020 Infineon Technologies AG. All rights reserved.
 *
 *
 *                                 IMPORTANT NOTICE
 *
 *
 * Infineon Technologies AG (Infineon) is supplying this file for use
 * exclusively with Infineon's microcontroller products. This file can be freely
 * distributed within development tools that are supporting such microcontroller
 * products.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 * OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 * INFINEON SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 * OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 *
 */

/******************************************************************************/
/*----------------------------------Includes----------------------------------*/
/******************************************************************************/

#include "IfxGeth.h"

/******************************************************************************/
/*-------------------------Function Implementations---------------------------*/
/******************************************************************************/

void IfxGeth_disableModule(Ifx_GETH *gethSFR)
{
    uint16 psw = IfxScuWdt_getCpuWatchdogPassword();
    IfxScuWdt_clearCpuEndinit(psw); /* clears the endinit protection*/
    gethSFR->CLC.B.DISR = 1;        /* set the enable request */
    IfxScuWdt_setCpuEndinit(psw);   /* sets the endinit protection back on*/
}


void IfxGeth_enableModule(Ifx_GETH *gethSFR)
{
    uint16 psw = IfxScuWdt_getCpuWatchdogPassword();

    if (IfxGeth_isModuleEnabled(gethSFR) != 1) /* if module is not enabled already */
    {
        IfxScuWdt_clearCpuEndinit(psw);        /* clears the endinit protection*/
        gethSFR->CLC.B.DISR = 0;               /* set the enable request */
        IfxScuWdt_setCpuEndinit(psw);          /* sets the endinit protection back on*/
    }

    IfxGeth_isModuleEnabled(gethSFR);          /* read back to ensure proper enabling */
}


volatile Ifx_SRC_SRCR *IfxGeth_getSrcPointer(Ifx_GETH *gethSFR, IfxGeth_ServiceRequest serviceRequest)
{
    IFX_UNUSED_PARAMETER(gethSFR);
    return &MODULE_SRC.GETH.GETH[0].SR[serviceRequest];
}


void IfxGeth_mac_setLineSpeed(Ifx_GETH *gethSFR, IfxGeth_LineSpeed speed)
{
    switch (speed)
    {
    case IfxGeth_LineSpeed_10Mbps:
        gethSFR->MAC_CONFIGURATION.B.PS  = 1;
        gethSFR->MAC_CONFIGURATION.B.FES = 0;
        break;
    case IfxGeth_LineSpeed_100Mbps:
        gethSFR->MAC_CONFIGURATION.B.PS  = 1;
        gethSFR->MAC_CONFIGURATION.B.FES = 1;
        break;
    case IfxGeth_LineSpeed_1000Mbps:
        gethSFR->MAC_CONFIGURATION.B.PS  = 0;
        gethSFR->MAC_CONFIGURATION.B.FES = 0;
        break;
    case IfxGeth_LineSpeed_2500Mbps:
        gethSFR->MAC_CONFIGURATION.B.PS  = 0;
        gethSFR->MAC_CONFIGURATION.B.FES = 1;
        break;
    default:
        break;
    }
}


void IfxGeth_mac_setMacAddress(Ifx_GETH *gethSFR, uint8 *macAddress)
{
    gethSFR->MAC_ADDRESS_HIGH0.U = 0
                                   | ((uint32)macAddress[4] << 0U)
                                   | ((uint32)macAddress[5] << 8U)
                                   | 0x80000000U;

    gethSFR->MAC_ADDRESS_LOW0.U = 0
                                  | ((uint32)macAddress[0] << 0U)
                                  | ((uint32)macAddress[1] << 8U)
                                  | ((uint32)macAddress[2] << 16U)
                                  | ((uint32)macAddress[3] << 24U);
}


void IfxGeth_mtl_clearAllInterruptFlags(Ifx_GETH *gethSFR, IfxGeth_MtlQueue queueId)
{
    uint32 value = (1 << IfxGeth_MtlInterruptFlag_txQueueUnderflow) | (1 << IfxGeth_MtlInterruptFlag_averageBitsPerSlot) | (1 << IfxGeth_MtlInterruptFlag_rxQueueOverflow);

    switch (queueId)
    {
    case IfxGeth_MtlQueue_0:
        gethSFR->MTL_Q0.INTERRUPT_CONTROL_STATUS.U |= value;
        break;
    case IfxGeth_MtlQueue_1:
        gethSFR->MTL_Q1.INTERRUPT_CONTROL_STATUS.U |= value;
        break;
    case IfxGeth_MtlQueue_2:
        gethSFR->MTL_Q2.INTERRUPT_CONTROL_STATUS.U |= value;
        break;
    case IfxGeth_MtlQueue_3:
        gethSFR->MTL_Q3.INTERRUPT_CONTROL_STATUS.U |= value;
        break;
    }
}


void IfxGeth_mtl_clearInterruptFlag(Ifx_GETH *gethSFR, IfxGeth_MtlQueue queueId, IfxGeth_MtlInterruptFlag flag)
{
    uint32 mask  = ((1 << IfxGeth_MtlInterruptFlag_txQueueUnderflow) | (1 << IfxGeth_MtlInterruptFlag_averageBitsPerSlot) | (1 << IfxGeth_MtlInterruptFlag_rxQueueOverflow)) << 8;
    uint32 value = (1 << flag);

    switch (queueId)
    {
    case IfxGeth_MtlQueue_0:
        gethSFR->MTL_Q0.INTERRUPT_CONTROL_STATUS.U = (gethSFR->MTL_Q0.INTERRUPT_CONTROL_STATUS.U & mask) | value;
        break;
    case IfxGeth_MtlQueue_1:
        gethSFR->MTL_Q1.INTERRUPT_CONTROL_STATUS.U = (gethSFR->MTL_Q1.INTERRUPT_CONTROL_STATUS.U & mask) | value;
        break;
    case IfxGeth_MtlQueue_2:
        gethSFR->MTL_Q2.INTERRUPT_CONTROL_STATUS.U = (gethSFR->MTL_Q2.INTERRUPT_CONTROL_STATUS.U & mask) | value;
        break;
    case IfxGeth_MtlQueue_3:
        gethSFR->MTL_Q3.INTERRUPT_CONTROL_STATUS.U = (gethSFR->MTL_Q3.INTERRUPT_CONTROL_STATUS.U & mask) | value;
        break;
    }
}


void IfxGeth_mtl_disableInterrupt(Ifx_GETH *gethSFR, IfxGeth_MtlQueue queueId, IfxGeth_MtlInterruptFlag flag)
{
    uint32 mask  = (1 << IfxGeth_MtlInterruptFlag_txQueueUnderflow) | (1 << IfxGeth_MtlInterruptFlag_averageBitsPerSlot) | (1 << IfxGeth_MtlInterruptFlag_rxQueueOverflow);

    uint32 value = ~(1 << (flag + 8));

    switch (queueId)
    {
    case IfxGeth_MtlQueue_0:
        gethSFR->MTL_Q0.INTERRUPT_CONTROL_STATUS.U = (gethSFR->MTL_Q0.INTERRUPT_CONTROL_STATUS.U & ~mask) & value;
        break;
    case IfxGeth_MtlQueue_1:
        gethSFR->MTL_Q1.INTERRUPT_CONTROL_STATUS.U = (gethSFR->MTL_Q1.INTERRUPT_CONTROL_STATUS.U & ~mask) & value;
        break;
    case IfxGeth_MtlQueue_2:
        gethSFR->MTL_Q2.INTERRUPT_CONTROL_STATUS.U = (gethSFR->MTL_Q2.INTERRUPT_CONTROL_STATUS.U & ~mask) & value;
        break;
    case IfxGeth_MtlQueue_3:
        gethSFR->MTL_Q3.INTERRUPT_CONTROL_STATUS.U = (gethSFR->MTL_Q3.INTERRUPT_CONTROL_STATUS.U & ~mask) & value;
        break;
    }
}


void IfxGeth_mtl_enableInterrupt(Ifx_GETH *gethSFR, IfxGeth_MtlQueue queueId, IfxGeth_MtlInterruptFlag flag)
{
    uint32 mask  = (1 << IfxGeth_MtlInterruptFlag_txQueueUnderflow) | (1 << IfxGeth_MtlInterruptFlag_averageBitsPerSlot) | (1 << IfxGeth_MtlInterruptFlag_rxQueueOverflow);
    uint32 value = (1 << (flag + 8));

    switch (queueId)
    {
    case IfxGeth_MtlQueue_0:
        gethSFR->MTL_Q0.INTERRUPT_CONTROL_STATUS.U = (gethSFR->MTL_Q0.INTERRUPT_CONTROL_STATUS.U & ~mask) | value;
        break;
    case IfxGeth_MtlQueue_1:
        gethSFR->MTL_Q1.INTERRUPT_CONTROL_STATUS.U = (gethSFR->MTL_Q1.INTERRUPT_CONTROL_STATUS.U & ~mask) | value;
        break;
    case IfxGeth_MtlQueue_2:
        gethSFR->MTL_Q2.INTERRUPT_CONTROL_STATUS.U = (gethSFR->MTL_Q2.INTERRUPT_CONTROL_STATUS.U & ~mask) | value;
        break;
    case IfxGeth_MtlQueue_3:
        gethSFR->MTL_Q3.INTERRUPT_CONTROL_STATUS.U = (gethSFR->MTL_Q3.INTERRUPT_CONTROL_STATUS.U & ~mask) | value;
        break;
    }
}


void IfxGeth_mtl_enableTxQueue(Ifx_GETH *gethSFR, IfxGeth_TxMtlQueue queueId)
{
    switch (queueId)
    {
    case IfxGeth_TxMtlQueue_0:
        gethSFR->MTL_TXQ0.OPERATION_MODE.B.TXQEN = 2;
        break;
    case IfxGeth_TxMtlQueue_1:
        gethSFR->MTL_TXQ1.OPERATION_MODE.B.TXQEN = 2;
        break;
    case IfxGeth_TxMtlQueue_2:
        gethSFR->MTL_TXQ2.OPERATION_MODE.B.TXQEN = 2;
        break;
    case IfxGeth_TxMtlQueue_3:
        gethSFR->MTL_TXQ3.OPERATION_MODE.B.TXQEN = 2;
        break;
    }
}


boolean IfxGeth_mtl_isInterruptFlagSet(Ifx_GETH *gethSFR, IfxGeth_MtlQueue queueId, IfxGeth_MtlInterruptFlag flag)
{
    uint32  value  = (1 << flag);
    boolean status = 0;

    switch (queueId)
    {
    case IfxGeth_MtlQueue_0:
        status = (gethSFR->MTL_Q0.INTERRUPT_CONTROL_STATUS.U & (Ifx_UReg_32Bit)value);
        break;
    case IfxGeth_MtlQueue_1:
        status = (gethSFR->MTL_Q1.INTERRUPT_CONTROL_STATUS.U & (Ifx_UReg_32Bit)value);
        break;
    case IfxGeth_MtlQueue_2:
        status = (gethSFR->MTL_Q2.INTERRUPT_CONTROL_STATUS.U & (Ifx_UReg_32Bit)value);
        break;
    case IfxGeth_MtlQueue_3:
        status = (gethSFR->MTL_Q3.INTERRUPT_CONTROL_STATUS.U & (Ifx_UReg_32Bit)value);
        break;
    }

    return status;
}


void IfxGeth_mtl_setRxForwardErrorPacket(Ifx_GETH *gethSFR, IfxGeth_RxMtlQueue queueId, boolean enabled)
{
    switch (queueId)
    {
    case IfxGeth_RxMtlQueue_0:
        gethSFR->MTL_RXQ0.OPERATION_MODE.B.FEP = enabled;
        break;
    case IfxGeth_RxMtlQueue_1:
        gethSFR->MTL_RXQ1.OPERATION_MODE.B.FEP = enabled;
        break;
    case IfxGeth_RxMtlQueue_2:
        gethSFR->MTL_RXQ2.OPERATION_MODE.B.FEP = enabled;
        break;
    case IfxGeth_RxMtlQueue_3:
        gethSFR->MTL_RXQ3.OPERATION_MODE.B.FEP = enabled;
        break;
    }
}


void IfxGeth_mtl_setRxForwardUndersizedGoodPacket(Ifx_GETH *gethSFR, IfxGeth_RxMtlQueue queueId, boolean enabled)
{
    switch (queueId)
    {
    case IfxGeth_RxMtlQueue_0:
        gethSFR->MTL_RXQ0.OPERATION_MODE.B.FUP = enabled;
        break;
    case IfxGeth_RxMtlQueue_1:
        gethSFR->MTL_RXQ1.OPERATION_MODE.B.FUP = enabled;
        break;
    case IfxGeth_RxMtlQueue_2:
        gethSFR->MTL_RXQ2.OPERATION_MODE.B.FUP = enabled;
        break;
    case IfxGeth_RxMtlQueue_3:
        gethSFR->MTL_RXQ3.OPERATION_MODE.B.FUP = enabled;
        break;
    }
}


void IfxGeth_mtl_setRxQueueSize(Ifx_GETH *gethSFR, IfxGeth_RxMtlQueue queueId, IfxGeth_QueueSize queueSize)
{
    switch (queueId)
    {
    case IfxGeth_RxMtlQueue_0:
        gethSFR->MTL_RXQ0.OPERATION_MODE.B.RQS = queueSize;
        break;
    case IfxGeth_RxMtlQueue_1:
        gethSFR->MTL_RXQ1.OPERATION_MODE.B.RQS = queueSize;
        break;
    case IfxGeth_RxMtlQueue_2:
        gethSFR->MTL_RXQ2.OPERATION_MODE.B.RQS = queueSize;
        break;
    case IfxGeth_RxMtlQueue_3:
        gethSFR->MTL_RXQ3.OPERATION_MODE.B.RQS = queueSize;
        break;
    }
}


void IfxGeth_mtl_setRxStoreAndForward(Ifx_GETH *gethSFR, IfxGeth_RxMtlQueue queueId, boolean enabled)
{
    switch (queueId)
    {
    case IfxGeth_RxMtlQueue_0:
        gethSFR->MTL_RXQ0.OPERATION_MODE.B.RSF = enabled;
        break;
    case IfxGeth_RxMtlQueue_1:
        gethSFR->MTL_RXQ1.OPERATION_MODE.B.RSF = enabled;
        break;
    case IfxGeth_RxMtlQueue_2:
        gethSFR->MTL_RXQ2.OPERATION_MODE.B.RSF = enabled;
        break;
    case IfxGeth_RxMtlQueue_3:
        gethSFR->MTL_RXQ3.OPERATION_MODE.B.RSF = enabled;
        break;
    }
}


void IfxGeth_mtl_setTxQueueSize(Ifx_GETH *gethSFR, IfxGeth_TxMtlQueue queueId, IfxGeth_QueueSize queueSize)
{
    switch (queueId)
    {
    case IfxGeth_TxMtlQueue_0:
        gethSFR->MTL_TXQ0.OPERATION_MODE.B.TQS = queueSize;
        break;
    case IfxGeth_TxMtlQueue_1:
        gethSFR->MTL_TXQ1.OPERATION_MODE.B.TQS = queueSize;
        break;
    case IfxGeth_TxMtlQueue_2:
        gethSFR->MTL_TXQ2.OPERATION_MODE.B.TQS = queueSize;
        break;
    case IfxGeth_TxMtlQueue_3:
        gethSFR->MTL_TXQ3.OPERATION_MODE.B.TQS = queueSize;
        break;
    }
}


void IfxGeth_mtl_setTxStoreAndForward(Ifx_GETH *gethSFR, IfxGeth_TxMtlQueue queueId, boolean enabled)
{
    switch (queueId)
    {
    case IfxGeth_TxMtlQueue_0:
        gethSFR->MTL_TXQ0.OPERATION_MODE.B.TSF = enabled;
        break;
    case IfxGeth_TxMtlQueue_1:
        gethSFR->MTL_TXQ1.OPERATION_MODE.B.TSF = enabled;
        break;
    case IfxGeth_TxMtlQueue_2:
        gethSFR->MTL_TXQ2.OPERATION_MODE.B.TSF = enabled;
        break;
    case IfxGeth_TxMtlQueue_3:
        gethSFR->MTL_TXQ3.OPERATION_MODE.B.TSF = enabled;
        break;
    }
}


void IfxGeth_resetModule(Ifx_GETH *gethSFR)
{
    uint16 passwd = IfxScuWdt_getCpuWatchdogPassword();
    IfxScuWdt_clearCpuEndinit(passwd);

    gethSFR->KRST0.B.RST = 1;           /* Only if both Kernel reset bits are set a reset is executed */
    gethSFR->KRST1.B.RST = 1;
    IfxScuWdt_setCpuEndinit(passwd);

    while (0 == gethSFR->KRST0.B.RSTSTAT)   /* Wait until reset is executed */
    {}

    IfxScuWdt_clearCpuEndinit(passwd);
    gethSFR->KRSTCLR.B.CLR = 1;         /* Clear Kernel reset status bit */

    IfxScuWdt_setCpuEndinit(passwd);
}


void IfxGeth_phy_Clause22_readMDIORegister(uint32 layerAddr, uint32 regAddr, uint32 *pData)
{
    // 5bit Physical Layer Adddress, 5bit GMII Regnr, 4bit csrclock divider, Read, Busy
    GETH_MAC_MDIO_ADDRESS.U = (layerAddr << 21) | (regAddr << 16) | (0 << 8) | (3 << 2) | (1 << 0);

    IFXGETH_PHY_WAIT_GMII_READY();

    // get data
    *pData = GETH_MAC_MDIO_DATA.U;
}


void IfxGeth_Phy_Clause22_writeMDIORegister(uint32 layerAddr, uint32 regAddr, uint32 data)
{
    // put data
    GETH_MAC_MDIO_DATA.U = data;

    // 5bit Physical Layer Adddress, 5bit GMII Regnr, 4bit csrclock divider, Write, Busy
    GETH_MAC_MDIO_ADDRESS.U = (layerAddr << 21) | (regAddr << 16) | (0 << 8) | (1 << 2) | (1 << 0);

    IFXGETH_PHY_WAIT_GMII_READY();
}


void IfxGeth_phy_Clause45_readMDIORegister(uint32 layerAddr, uint32 deviceAddr, uint32 regAddr, uint32 *pData)
{
    // put data
    GETH_MAC_MDIO_DATA.U = (regAddr << 16);

    // 5bit Physical Layer Adddress, 5bit GMII Regnr, 4bit csrclock divider, Read, Clause 45, Busy
    GETH_MAC_MDIO_ADDRESS.U = (layerAddr << 21) | (deviceAddr << 16) | (0 << 8) | (3 << 2) | (1 << 1) | (1 << 0);

    IFXGETH_PHY_WAIT_GMII_READY();

    // get data
    *pData = GETH_MAC_MDIO_DATA.U;
}


void IfxGeth_Phy_Clause45_writeMDIORegister(uint32 layerAddr, uint32 deviceAddr, uint32 regAddr, uint32 data)
{
    // put data
    GETH_MAC_MDIO_DATA.U = (regAddr << 16) | data;

    // 5bit Physical Layer Adddress, 5bit GMII Regnr, 4bit csrclock divider, Write, Clause 45, Busy
    GETH_MAC_MDIO_ADDRESS.U = (layerAddr << 21) | (deviceAddr << 16) | (0 << 8) | (1 << 2) | (1 << 1) | (1 << 0);

    IFXGETH_PHY_WAIT_GMII_READY();
}


void IfxGeth_mac_setMaxPacketSize(Ifx_GETH *gethSFR, uint16 maxPacketSize)
{
    uint16                     maxSize;
    Ifx_GETH_MAC_CONFIGURATION macConfig;

    maxSize     = maxPacketSize;
    macConfig.U = gethSFR->MAC_CONFIGURATION.U;

    if (maxSize == 1518)
    {
        macConfig.B.JE     = 0;
        macConfig.B.S2KP   = 0;
        macConfig.B.GPSLCE = 0;
        macConfig.B.JD     = 0;
    }
    else if (maxSize == 2000)
    {
        macConfig.B.JE     = 0;
        macConfig.B.S2KP   = 1;
        macConfig.B.GPSLCE = 0;
        macConfig.B.JD     = 0;
    }
    else if (maxSize == 9018)
    {
        macConfig.B.JE     = 1;
        macConfig.B.GPSLCE = 0;
        macConfig.B.JD     = 0;
    }
    else
    {
        macConfig.B.JE                        = 0;
        macConfig.B.S2KP                      = 0;
        macConfig.B.GPSLCE                    = 1;
        macConfig.B.JD                        = 1;
        gethSFR->MAC_EXT_CONFIGURATION.B.GPSL = maxSize;
        gethSFR->MAC_WATCHDOG_TIMEOUT.B.PWE   = 1;
        gethSFR->MAC_WATCHDOG_TIMEOUT.B.WTO   = 0x0e;
    }

    gethSFR->MAC_CONFIGURATION.U = macConfig.U;
}


IfxGeth_Index IfxGeth_getIndex(Ifx_GETH *geth)
{
    uint32        index;
    IfxGeth_Index result;

    result = IfxGeth_Index_none;

    for (index = 0; index < IFXGETH_NUM_MODULES; index++)
    {
        if (IfxGeth_cfg_indexMap[index].module == geth)
        {
            result = (IfxGeth_Index)IfxGeth_cfg_indexMap[index].index;
            break;
        }
    }

    return result;
}


Ifx_GETH *IfxGeth_getAddress(IfxGeth_Index geth)
{
    Ifx_GETH *module;

    if (geth < IFXGETH_NUM_MODULES)
    {
        module = (Ifx_GETH *)IfxGeth_cfg_indexMap[geth].module;
    }
    else
    {
        module = NULL_PTR;
    }

    return module;
}


boolean IfxGeth_mac_writeQueueVlanTag(Ifx_GETH *gethSFR, IfxGeth_MtlQueue queueId, uint16 vLanTag)
{
    Ifx_GETH_MAC_VLAN_INCL vlan_incl;

    if (gethSFR->MAC_VLAN_INCL.B.BUSY)
    {
        return FALSE;
    }

    vlan_incl.B.CBTI         = 1;
    vlan_incl.B.CSVL         = 0;
    vlan_incl.B.VLT          = vLanTag;
    vlan_incl.B.RDWR         = 1;
    vlan_incl.B.ADDR         = queueId;

    gethSFR->MAC_VLAN_INCL.U = vlan_incl.U;

    // Wait until operation is finished
    while (gethSFR->MAC_VLAN_INCL.B.BUSY)
    {}

    return TRUE;
}


boolean IfxGeth_mac_readQueueVlanTag(Ifx_GETH *gethSFR, IfxGeth_MtlQueue queueId, uint16 *const vLanTag)
{
    Ifx_GETH_MAC_VLAN_INCL vlan_incl;

    if (gethSFR->MAC_VLAN_INCL.B.BUSY)
    {
        return FALSE;
    }

    vlan_incl.B.CBTI         = 1;
    vlan_incl.B.RDWR         = 0;
    vlan_incl.B.ADDR         = queueId;

    gethSFR->MAC_VLAN_INCL.U = vlan_incl.U;

    // Wait until operation is finished
    while (gethSFR->MAC_VLAN_INCL.B.BUSY)
    {}

    *vLanTag = gethSFR->MAC_VLAN_INCL.B.VLT;

    return TRUE;
}
