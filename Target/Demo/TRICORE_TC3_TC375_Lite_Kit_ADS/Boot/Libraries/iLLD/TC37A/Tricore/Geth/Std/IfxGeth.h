/**
 * \file IfxGeth.h
 * \brief GETH  basic functionality
 * \ingroup IfxLld_Geth
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
 * Standard layer
 *
 * \defgroup IfxLld_Geth_Std_Enumerations Enumerations
 * \ingroup IfxLld_Geth_Std
 * \defgroup IfxLld_Geth_Std_MAC_Functions MAC Functions
 * \ingroup IfxLld_Geth_Std
 * \defgroup IfxLld_Geth_Std_Module_Functions Module Functions
 * \ingroup IfxLld_Geth_Std
 * \defgroup IfxLld_Geth_Std_MTL_Functions MTL Functions
 * \ingroup IfxLld_Geth_Std
 * \defgroup IfxLld_Geth_Std_DMA_Functions DMA Functions
 * \ingroup IfxLld_Geth_Std
 * \defgroup IfxLld_Geth_Std_DataStructures DataStructures
 * \ingroup IfxLld_Geth_Std
 * \defgroup IfxLld_Geth_Std_Unions Unions
 * \ingroup IfxLld_Geth_Std
 */

#ifndef IFXGET_H
#define IFXGET_H 1

/******************************************************************************/
/*----------------------------------Includes----------------------------------*/
/******************************************************************************/

#include "_Impl/IfxGeth_cfg.h"
#include "IfxGeth_reg.h"
#include "Scu/Std/IfxScuWdt.h"
#include "Src/Std/IfxSrc.h"
#include "IfxGeth_bf.h"

/******************************************************************************/
/*-----------------------------------Macros-----------------------------------*/
/******************************************************************************/

/** \brief Max number of TX descriptors per list
 */
#ifndef IFXGETH_MAX_TX_DESCRIPTORS
#define IFXGETH_MAX_TX_DESCRIPTORS (8)
#endif

/** \brief Max number of RX descriptors per list
 */
#ifndef IFXGETH_MAX_RX_DESCRIPTORS
#define IFXGETH_MAX_RX_DESCRIPTORS (8)
#endif

/** \brief Waits until GMII is busy
 */
#define IFXGETH_PHY_WAIT_GMII_READY() while (GETH_MAC_MDIO_ADDRESS.B.GB) {}

/******************************************************************************/
/*--------------------------------Enumerations--------------------------------*/
/******************************************************************************/

/** \addtogroup IfxLld_Geth_Std_Enumerations
 * \{ */
/** \brief Programmable burst length of DMA channels\n
 * Definition in DMA_CHi_TX_CONTROL.B.PBL and DMA_CHi_RX_CONTROL.B.PBL
 */
typedef enum
{
    IfxGeth_DmaBurstLength_0  = 0,   /**< \brief maximum burst length 0 */
    IfxGeth_DmaBurstLength_1  = 1,   /**< \brief maximum burst length 1 */
    IfxGeth_DmaBurstLength_2  = 2,   /**< \brief maximum burst length 2 */
    IfxGeth_DmaBurstLength_4  = 4,   /**< \brief maximum burst length 4 */
    IfxGeth_DmaBurstLength_8  = 8,   /**< \brief maximum burst length 8 */
    IfxGeth_DmaBurstLength_16 = 16,  /**< \brief maximum burst length 16 */
    IfxGeth_DmaBurstLength_32 = 32   /**< \brief maximum burst length 32 */
} IfxGeth_DmaBurstLength;

/** \brief DMA channel Index
 */
typedef enum
{
    IfxGeth_DmaChannel_0,     /**< \brief Dma Channel 0  */
    IfxGeth_DmaChannel_1,     /**< \brief Dma Channel 1  */
    IfxGeth_DmaChannel_2,     /**< \brief Dma Channel 2  */
    IfxGeth_DmaChannel_3      /**< \brief Dma Channel 3  */
} IfxGeth_DmaChannel;

/** \brief DMA interrupt flags
 */
typedef enum
{
    IfxGeth_DmaInterruptFlag_transmitInterrupt         = 0,  /**< \brief Transmit Interrupt */
    IfxGeth_DmaInterruptFlag_transmitStopped           = 1,  /**< \brief Transmit Stopped */
    IfxGeth_DmaInterruptFlag_transmitBufferUnavailable = 2,  /**< \brief Trasmit Buffer Unavailable */
    IfxGeth_DmaInterruptFlag_receiveInterrupt          = 6,  /**< \brief Receive Interrupt */
    IfxGeth_DmaInterruptFlag_receiveBufferUnavailable  = 7,  /**< \brief Receive Buffer Unavailable */
    IfxGeth_DmaInterruptFlag_receiveStopped            = 8,  /**< \brief Receive Stopped */
    IfxGeth_DmaInterruptFlag_receiveWatchdogTimeout    = 9,  /**< \brief Receive Watchdog Timeout */
    IfxGeth_DmaInterruptFlag_earlyTransmitInterrupt    = 10, /**< \brief Early Transmit Interrupt */
    IfxGeth_DmaInterruptFlag_earlyReceiveInterrupt     = 11, /**< \brief Early Receive Interrupt */
    IfxGeth_DmaInterruptFlag_fatalBusError             = 12, /**< \brief Fatal Bus Error */
    IfxGeth_DmaInterruptFlag_contextDescriptorError    = 13, /**< \brief Context Descriptor Error */
    IfxGeth_DmaInterruptFlag_abnormalInterruptSummary  = 14, /**< \brief Abnormal Interrupt Summary */
    IfxGeth_DmaInterruptFlag_normalInterruptSummary    = 15  /**< \brief Normal Interrupt Summary */
} IfxGeth_DmaInterruptFlag;

/** \brief Ethernet line speed
 */
typedef enum
{
    IfxGeth_LineSpeed_10Mbps,    /**< \brief 10 Mbps */
    IfxGeth_LineSpeed_100Mbps,   /**< \brief 100 Mbps */
    IfxGeth_LineSpeed_1000Mbps,  /**< \brief 1000 Mbps */
    IfxGeth_LineSpeed_2500Mbps   /**< \brief 2500 Mbps */
} IfxGeth_LineSpeed;

/** \brief MTL interrupt flags
 */
typedef enum
{
    IfxGeth_MtlInterruptFlag_txQueueUnderflow   = 0,  /**< \brief Transmit Queue Underflow */
    IfxGeth_MtlInterruptFlag_averageBitsPerSlot = 1,  /**< \brief Average Bits Per Slot */
    IfxGeth_MtlInterruptFlag_rxQueueOverflow    = 16  /**< \brief Receive Queue Overflow */
} IfxGeth_MtlInterruptFlag;

/** \brief MTL Queue ID
 */
typedef enum
{
    IfxGeth_MtlQueue_0,     /**< \brief Mtl Queue 0  */
    IfxGeth_MtlQueue_1,     /**< \brief Mtl Queue 1  */
    IfxGeth_MtlQueue_2,     /**< \brief Mtl Queue 2  */
    IfxGeth_MtlQueue_3      /**< \brief Mtl Queue 3  */
} IfxGeth_MtlQueue;

/** \brief External Phy Interface RMII Mode
 */
typedef enum
{
    IfxGeth_PhyInterfaceMode_mii   = 0,  /**< \brief MII mode */
    IfxGeth_PhyInterfaceMode_rgmii = 1,  /**< \brief RGMII mode */
    IfxGeth_PhyInterfaceMode_rmii  = 4   /**< \brief RMII mode */
} IfxGeth_PhyInterfaceMode;

/** \brief Preamble Length for Transmit packets\n
 * Definition in MAC_CONFIGURATION.B.PRELEN
 */
typedef enum
{
    IfxGeth_PreambleLength_7Bytes,  /**< \brief 7 bytes of preamble */
    IfxGeth_PreambleLength_5Bytes,  /**< \brief 5 bytes of preamble */
    IfxGeth_PreambleLength_3Bytes   /**< \brief 3 bytes of preamble */
} IfxGeth_PreambleLength;

/** \brief Tx/RX Queue size in blocks of 256 bytes\n
 * Definition in MTL_TXQi_OPERATION_MODE.B.TQS and MTL_RXQi_OPERATION_MODE.B.RQS
 */
typedef enum
{
    IfxGeth_QueueSize_256Bytes,     /**< \brief 256Bytes  */
    IfxGeth_QueueSize_512Bytes,     /**< \brief 512Bytes  */
    IfxGeth_QueueSize_768Bytes,     /**< \brief 768Bytes  */
    IfxGeth_QueueSize_1024Bytes,    /**< \brief 1024Bytes  */
    IfxGeth_QueueSize_1280Bytes,    /**< \brief 1280Bytes  */
    IfxGeth_QueueSize_1536Bytes,    /**< \brief 1536Bytes  */
    IfxGeth_QueueSize_1792Bytes,    /**< \brief 1792Bytes  */
    IfxGeth_QueueSize_2048Bytes,    /**< \brief 2048Bytes  */
    IfxGeth_QueueSize_2304Bytes,    /**< \brief 2304Bytes  */
    IfxGeth_QueueSize_2560Bytes,    /**< \brief 2560Bytes  */
    IfxGeth_QueueSize_2816Bytes,    /**< \brief 2816Bytes  */
    IfxGeth_QueueSize_3072Bytes,    /**< \brief 3072Bytes  */
    IfxGeth_QueueSize_3328Bytes,    /**< \brief 3328Bytes  */
    IfxGeth_QueueSize_3584Bytes,    /**< \brief 3584Bytes  */
    IfxGeth_QueueSize_3840Bytes,    /**< \brief 3840Bytes  */
    IfxGeth_QueueSize_4096Bytes,    /**< \brief 4096Bytes  */
    IfxGeth_QueueSize_4352Bytes,    /**< \brief 4352Bytes  */
    IfxGeth_QueueSize_4608Bytes,    /**< \brief 4608Bytes  */
    IfxGeth_QueueSize_4864Bytes,    /**< \brief 4864Bytes  */
    IfxGeth_QueueSize_5120Bytes,    /**< \brief 5120Bytes  */
    IfxGeth_QueueSize_5376Bytes,    /**< \brief 5376Bytes  */
    IfxGeth_QueueSize_5632Bytes,    /**< \brief 5632Bytes  */
    IfxGeth_QueueSize_5888Bytes,    /**< \brief 5888Bytes  */
    IfxGeth_QueueSize_6144Bytes,    /**< \brief 6144Bytes  */
    IfxGeth_QueueSize_6400Bytes,    /**< \brief 6400Bytes  */
    IfxGeth_QueueSize_6656Bytes,    /**< \brief 6656Bytes  */
    IfxGeth_QueueSize_6912Bytes,    /**< \brief 6912Bytes  */
    IfxGeth_QueueSize_7168Bytes,    /**< \brief 7168Bytes  */
    IfxGeth_QueueSize_7424Bytes,    /**< \brief 7424Bytes  */
    IfxGeth_QueueSize_7680Bytes,    /**< \brief 7680Bytes  */
    IfxGeth_QueueSize_7936Bytes,    /**< \brief 7936Bytes  */
    IfxGeth_QueueSize_8192Bytes     /**< \brief 8192Bytes  */
} IfxGeth_QueueSize;

/** \brief Receive Arbitration Algorithm for Rx Queues\n
 * Definition in MTL_OPERATION_MODE.B.RAA
 */
typedef enum
{
    IfxGeth_RxArbitrationAlgorithm_sp  = 0, /**< \brief Strict Priority */
    IfxGeth_RxArbitrationAlgorithm_wsp = 1  /**< \brief Weighted Strict Priority */
} IfxGeth_RxArbitrationAlgorithm;

/** \brief Rx DMA channel
 */
typedef enum
{
    IfxGeth_RxDmaChannel_0,     /**< \brief Rx Dma Channel 0  */
    IfxGeth_RxDmaChannel_1,     /**< \brief Rx Dma Channel 1  */
    IfxGeth_RxDmaChannel_2,     /**< \brief Rx Dma Channel 2  */
    IfxGeth_RxDmaChannel_3      /**< \brief Rx Dma Channel 3  */
} IfxGeth_RxDmaChannel;

/** \brief Rx MTL Queue ID
 */
typedef enum
{
    IfxGeth_RxMtlQueue_0,     /**< \brief Rx Queue 0  */
    IfxGeth_RxMtlQueue_1,     /**< \brief Rx Queue 1  */
    IfxGeth_RxMtlQueue_2,     /**< \brief Rx Queue 2  */
    IfxGeth_RxMtlQueue_3      /**< \brief Rx Queue 3  */
} IfxGeth_RxMtlQueue;

/** \brief Geth service request index
 */
typedef enum
{
    IfxGeth_ServiceRequest_0,     /**< \brief Service Request SR0  */
    IfxGeth_ServiceRequest_1,     /**< \brief Service Request SR1  */
    IfxGeth_ServiceRequest_2,     /**< \brief Service Request SR2  */
    IfxGeth_ServiceRequest_3,     /**< \brief Service Request SR3  */
    IfxGeth_ServiceRequest_4,     /**< \brief Service Request SR4  */
    IfxGeth_ServiceRequest_5,     /**< \brief Service Request SR5  */
    IfxGeth_ServiceRequest_6,     /**< \brief Service Request SR6  */
    IfxGeth_ServiceRequest_7,     /**< \brief Service Request SR7  */
    IfxGeth_ServiceRequest_8,     /**< \brief Service Request SR8  */
    IfxGeth_ServiceRequest_9      /**< \brief Service Request SR9  */
} IfxGeth_ServiceRequest;

/** \brief Tx DMA channel
 */
typedef enum
{
    IfxGeth_TxDmaChannel_0,     /**< \brief Tx Dma Channel 0  */
    IfxGeth_TxDmaChannel_1,     /**< \brief Tx Dma Channel 1  */
    IfxGeth_TxDmaChannel_2,     /**< \brief Tx Dma Channel 2  */
    IfxGeth_TxDmaChannel_3      /**< \brief Tx Dma Channel 3  */
} IfxGeth_TxDmaChannel;

/** \brief Tx MTL Queue ID
 */
typedef enum
{
    IfxGeth_TxMtlQueue_0,     /**< \brief Tx Queue 0  */
    IfxGeth_TxMtlQueue_1,     /**< \brief Tx Queue 1  */
    IfxGeth_TxMtlQueue_2,     /**< \brief Tx Queue 2  */
    IfxGeth_TxMtlQueue_3      /**< \brief Tx Queue 3  */
} IfxGeth_TxMtlQueue;

/** \brief Tx Scheduling Algorithm for Tx Queues\n
 * Definition in MTL_OPERATION_MODE.B.SCHALG
 */
typedef enum
{
    IfxGeth_TxSchedulingAlgorithm_wrr = 0,  /**< \brief WRR Algorithm */
    IfxGeth_TxSchedulingAlgorithm_sp  = 3   /**< \brief Strict Priority Algorithm */
} IfxGeth_TxSchedulingAlgorithm;

/** \} */

/** \brief Duplex Mode\n
 * Definintion in MAC_CONFIGURATION.B.DM
 */
typedef enum
{
    IfxGeth_DuplexMode_halfDuplex,  /**< \brief Half Duplex Mode */
    IfxGeth_DuplexMode_fullDuplex   /**< \brief Full Duplex Mode */
} IfxGeth_DuplexMode;

/** \brief Loopback Mode\n
 * Definition in MAC_CONFIGURATION.B.LM
 */
typedef enum
{
    IfxGeth_LoopbackMode_disable,  /**< \brief Disable loopback */
    IfxGeth_LoopbackMode_enable    /**< \brief Enable loopback */
} IfxGeth_LoopbackMode;

/******************************************************************************/
/*-----------------------------Data Structures--------------------------------*/
/******************************************************************************/

/** \addtogroup IfxLld_Geth_Std_DataStructures
 * \{ */
/** \brief Bit Fields of RDES0 Context Descriptor
 */
typedef struct
{
    uint32 RTSL : 32;     /**< \brief Receive Packet Timestamp Low */
} IfxGeth_RxContextDescr0_Bits;

/** \brief Bit Fields of RDES1 Context Descriptor
 */
typedef struct
{
    uint32 RTSH : 32;     /**< \brief Receive Packet Timestamp High */
} IfxGeth_RxContextDescr1_Bits;

/** \brief Bit Fields of RDES2 Context Descriptor
 */
typedef struct
{
    uint32 reserved_0 : 32;     /**< \brief Reserved */
} IfxGeth_RxContextDescr2_Bits;

/** \brief Bit Fields of RDES3 Context Descriptor
 */
typedef struct
{
    uint32 reserved_0 : 29;     /**< \brief Reserved */
    uint32 DE : 1;              /**< \brief Descriptor Error */
    uint32 CTXT : 1;            /**< \brief Receive Context Descriptor */
    uint32 OWN : 1;             /**< \brief Own Bit */
} IfxGeth_RxContextDescr3_Bits;

/** \brief Bit Fields of RDES0 in Read Format
 */
typedef struct
{
    uint32 BUF1AP : 32;     /**< \brief Header or Buffer 1 Address Pointer */
} IfxGeth_RxDescr0_RF_Bits;

/** \brief Bit Fields of RDES0 in Write back Format
 */
typedef struct
{
    uint32 OVT : 16;     /**< \brief Outer VLAN Tag */
    uint32 IVT : 16;     /**< \brief Inner VLAN Tag */
} IfxGeth_RxDescr0_WF_Bits;

/** \brief Bit Fields of RDES1 in Read Format
 */
typedef struct
{
    uint32 reserved_0 : 32;     /**< \brief Reserved */
} IfxGeth_RxDescr1_RF_Bits;

/** \brief Bit Fields of RDES1 in Write back Format
 */
typedef struct
{
    uint32 PT : 3;       /**< \brief Payload Type */
    uint32 IPHE : 1;     /**< \brief IP Header Error */
    uint32 IP4 : 1;      /**< \brief IPV4 Header Present */
    uint32 IP6 : 1;      /**< \brief IPv6 header Present */
    uint32 IPCB : 1;     /**< \brief IP Checksum Bypassed */
    uint32 IPCE : 1;     /**< \brief IP Payload Error */
    uint32 PMT : 4;      /**< \brief PTP Message Type */
    uint32 PFT : 1;      /**< \brief PTP Packet Type */
    uint32 PV : 1;       /**< \brief PTP Version */
    uint32 TSA : 1;      /**< \brief Timestamp Available */
    uint32 TD : 1;       /**< \brief Timestamp Dropped */
    uint32 OPC : 16;     /**< \brief OAM Sub-Type Code, or MAC Control Packet opcode */
} IfxGeth_RxDescr1_WF_Bits;

/** \brief Bit Fields of RDES2 in Read Format
 */
typedef struct
{
    uint32 BUF2AP : 32;     /**< \brief Buffer 2 Address Pointer */
} IfxGeth_RxDescr2_RF_Bits;

/** \brief Bit Fields of RDES2 in Write back Format
 */
typedef struct
{
    uint32 HL : 10;             /**< \brief L3/L4 Header Length */
    uint32 ARPNR : 1;           /**< \brief ARP Reply Not Generated */
    uint32 reserved_11 : 3;     /**< \brief Reserved */
    uint32 ITS : 1;             /**< \brief Inner VLAN Tag Filter Status (ITS) */
    uint32 OTS : 1;             /**< \brief VLAN Filter Status */
    uint32 SAF : 1;             /**< \brief SA Address Filter Fail */
    uint32 DAF : 1;             /**< \brief Destination Address Filter Fail */
    uint32 HF : 1;              /**< \brief Hash Filter Status */
    uint32 MADRM : 8;           /**< \brief MAC Address Match or Hash Value */
    uint32 L3FM : 1;            /**< \brief Layer 3 Filter Match */
    uint32 L4FM : 1;            /**< \brief Layer 4 Filter Match */
    uint32 L3L4FM : 3;          /**< \brief Layer 3 and Layer 4 Filter Number Matched */
} IfxGeth_RxDescr2_WF_Bits;

/** \brief Bit Fields of RDES3 in Read Format
 */
typedef struct
{
    uint32 reserved_0 : 24;     /**< \brief Reserved */
    uint32 BUF1V : 1;           /**< \brief Buffer 1 Address Valid */
    uint32 BUF2V : 1;           /**< \brief Buffer 2 Address Valid */
    uint32 reserved_27 : 4;     /**< \brief Reserved */
    uint32 IOC : 1;             /**< \brief Interrupt on Completion */
    uint32 OWN : 1;             /**< \brief Own bit */
} IfxGeth_RxDescr3_RF_Bits;

/** \brief Bit Fields of RDES3 in Write back Format
 */
typedef struct
{
    uint32 PL : 15;      /**< \brief Packet Length */
    uint32 ES : 1;       /**< \brief Error Summary */
    uint32 LT : 3;       /**< \brief Length/Type Field */
    uint32 DE : 1;       /**< \brief Dribble Bit Error */
    uint32 RE : 1;       /**< \brief Receive Error */
    uint32 OE : 1;       /**< \brief Overflow Error */
    uint32 RWT : 1;      /**< \brief Receive Watchdog Timeout */
    uint32 GP : 1;       /**< \brief Giant Packet */
    uint32 CE : 1;       /**< \brief CRC Error */
    uint32 RS0V : 1;     /**< \brief Receive Status RDES0 Valid */
    uint32 RS1V : 1;     /**< \brief Receive Status RDES1 Valid */
    uint32 RS2V : 1;     /**< \brief Receive Status RDES2 Valid */
    uint32 LD : 1;       /**< \brief Last Descriptor */
    uint32 FD : 1;       /**< \brief First Descriptor */
    uint32 CTXT : 1;     /**< \brief Receive Context Descriptor */
    uint32 OWN : 1;      /**< \brief Own Bit */
} IfxGeth_RxDescr3_WF_Bits;

/** \brief Bit Fields of TDES0 Context Descriptor
 */
typedef struct
{
    uint32 TTSL : 32;     /**< \brief Transmit Packet Timestamp Low */
} IfxGeth_TxContextDescr0_Bits;

/** \brief Bit Fields of TDES1 Context Descriptor
 */
typedef struct
{
    uint32 TTSH : 32;     /**< \brief Transmit Packet Timestamp High */
} IfxGeth_TxContextDescr1_Bits;

/** \brief Bit Fields of TDES2 Context Descriptor
 */
typedef struct
{
    uint32 MSS : 14;            /**< \brief Maximum Segment Size */
    uint32 reserved_14 : 2;     /**< \brief Reserved */
    uint32 IVT : 16;            /**< \brief Inner VLAN Tag */
} IfxGeth_TxContextDescr2_Bits;

/** \brief Bit Fields of TDES3 Context Descriptor
 */
typedef struct
{
    uint32 VT : 16;             /**< \brief VLAN Tag */
    uint32 VLTV : 1;            /**< \brief VLAN Tag Valid */
    uint32 IVLTV : 1;           /**< \brief Inner VLAN Tag Valid */
    uint32 IVTIR : 2;           /**< \brief Inner VLAN Tag Insert or Replace */
    uint32 reserved_20 : 3;     /**< \brief Reserved */
    uint32 CDE : 1;             /**< \brief Context Descriptor Error */
    uint32 reserved_24 : 2;     /**< \brief Reserved */
    uint32 TCMSSV : 1;          /**< \brief One-Step Timestamp Correction Input or MSS Valid */
    uint32 OSTC : 1;            /**< \brief One-Step Timestamp Correction Enable */
    uint32 reserved_28 : 2;     /**< \brief Reserved */
    uint32 CTXT : 1;            /**< \brief Context Type */
    uint32 OWN : 1;             /**< \brief Own Bit */
} IfxGeth_TxContextDescr3_Bits;

/** \brief Bit Fields of TDES0 in Read Format
 */
typedef struct
{
    uint32 BUF1AP : 32;     /**< \brief Buffer 1 Address Pointer */
} IfxGeth_TxDescr0_RF_Bits;

/** \brief Bit Fields of TDES0 in Write-back Format
 */
typedef struct
{
    uint32 TTSL : 32;     /**< \brief Transmit Packet Timestamp Low */
} IfxGeth_TxDescr0_WF_Bits;

/** \brief Bit Fields of TDES1 in Read Format
 */
typedef struct
{
    uint32 BUF2AP : 32;     /**< \brief Buffer 2 or Buffer 1 Address Pointer */
} IfxGeth_TxDescr1_RF_Bits;

/** \brief Bit Fields of TDES1 in Write-back Format
 */
typedef struct
{
    uint32 TTSH : 32;     /**< \brief Transmit Packet Timestamp High */
} IfxGeth_TxDescr1_WF_Bits;

/** \brief Bit Fields of TDES2 in Read Format
 */
typedef struct
{
    uint32 B1L : 14;          /**< \brief Header Length or Buffer 1 Length */
    uint32 VTIR : 2;          /**< \brief VLAN Tag Insertion or Replacement */
    uint32 B2L : 14;          /**< \brief Buffer 2 Length */
    uint32 TTSE_TMWD : 1;     /**< \brief Transmit Timestamp Enable or External TSO Memory Write Enable */
    uint32 IOC : 1;           /**< \brief Interrupt on Completion */
} IfxGeth_TxDescr2_RF_Bits;

/** \brief Bit Fields of TDES2 in Write-back Format
 */
typedef struct
{
    uint32 reserved_0 : 32;     /**< \brief Reserved */
} IfxGeth_TxDescr2_WF_Bits;

/** \brief Bit Fields of TDES3 in Read Format
 */
typedef struct
{
    uint32 FL_TPL : 15;         /**< \brief Packet Length or TCP Payload Length */
    uint32 TPL : 1;             /**< \brief Reserved or TCP Payload Length */
    uint32 CIC_TPL : 2;         /**< \brief Checksum Insertion Control or TCP Payload LengthThese bits control the checksum */
    uint32 TSE : 1;             /**< \brief TCP Segmentation Enable */
    uint32 SLOTNUM_THL : 4;     /**< \brief SLOTNUM: Slot Number Control Bits in AV Mode */
    uint32 SAIC : 3;            /**< \brief SA Insertion Control */
    uint32 CPC : 2;             /**< \brief CRC Pad Control */
    uint32 LD : 1;              /**< \brief Last Descriptor */
    uint32 FD : 1;              /**< \brief First Descriptor */
    uint32 CTXT : 1;            /**< \brief Context TypeThis bit should be set to 1'b0 for normal descriptor */
    uint32 OWN : 1;             /**< \brief Own Bit */
} IfxGeth_TxDescr3_RF_Bits;

/** \brief Bit Fields of TDES3 in Write-back Format
 */
typedef struct
{
    uint32 IHE : 1;              /**< \brief IP Header Error */
    uint32 DB : 1;               /**< \brief Deferred Bit */
    uint32 UF : 1;               /**< \brief Underflow Error */
    uint32 ED : 1;               /**< \brief Excessive Deferral */
    uint32 CC : 4;               /**< \brief Collision Count */
    uint32 EC : 1;               /**< \brief Excessive Collision */
    uint32 LC : 1;               /**< \brief Late Collision */
    uint32 NC : 1;               /**< \brief No Carrier */
    uint32 LOC : 1;              /**< \brief Loss of Carrier */
    uint32 PCE : 1;              /**< \brief Payload Checksum */
    uint32 FF : 1;               /**< \brief Packet Flushed */
    uint32 JT : 1;               /**< \brief Jabber Timeout */
    uint32 ES : 1;               /**< \brief Error Summary */
    uint32 reserved_16 : 1;      /**< \brief Reserved */
    uint32 TTSS : 1;             /**< \brief Tx Timestamp Status */
    uint32 reserved_18 : 10;     /**< \brief Reserved */
    uint32 LD : 1;               /**< \brief Last Descriptor */
    uint32 FD : 1;               /**< \brief First Descriptor */
    uint32 CTXT : 1;             /**< \brief Context Type */
    uint32 OWN : 1;              /**< \brief Own bit */
} IfxGeth_TxDescr3_WF_Bits;

/** \} */

/** \addtogroup IfxLld_Geth_Std_Unions
 * \{ */
/** \brief RDES0
 */
typedef union
{
    IfxGeth_RxDescr0_RF_Bits     R;       /**< \brief Read Format Bitfiled Access */
    IfxGeth_RxDescr0_WF_Bits     W;       /**< \brief Write back Format Bitfiled Access */
    IfxGeth_RxContextDescr0_Bits C;       /**< \brief Context Descriptor Format Bitfiled Access */
    uint32                       U;       /**< \brief Unsigned access */
} IfxGeth_RxDescr0;

/** \brief RDES1
 */
typedef union
{
    IfxGeth_RxDescr1_RF_Bits     R;       /**< \brief Read Format Bitfiled Access */
    IfxGeth_RxDescr1_WF_Bits     W;       /**< \brief Write back Format Bitfiled Access */
    IfxGeth_RxContextDescr1_Bits C;       /**< \brief Context Descriptor Format Bitfiled Access */
    uint32                       U;       /**< \brief Unsigned access */
} IfxGeth_RxDescr1;

/** \brief RDES2
 */
typedef union
{
    IfxGeth_RxDescr2_RF_Bits     R;       /**< \brief Read Format Bitfiled Access */
    IfxGeth_RxDescr2_WF_Bits     W;       /**< \brief Write back Format Bitfiled Access */
    IfxGeth_RxContextDescr2_Bits C;       /**< \brief Context Descriptor Format Bitfiled Access */
    uint32                       U;       /**< \brief Unsigned access */
} IfxGeth_RxDescr2;

/** \brief RDES3
 */
typedef union
{
    IfxGeth_RxDescr3_RF_Bits     R;       /**< \brief Read Format Bitfiled Access */
    IfxGeth_RxDescr3_WF_Bits     W;       /**< \brief Write back Format Bitfiled Access */
    IfxGeth_RxContextDescr3_Bits C;       /**< \brief Context Descriptor Format Bitfiled Access */
    uint32                       U;       /**< \brief Unsigned access */
} IfxGeth_RxDescr3;

/** \brief TDES0
 */
typedef union
{
    IfxGeth_TxDescr0_RF_Bits     R;       /**< \brief Read Format Bitfiled Access */
    IfxGeth_TxDescr0_WF_Bits     W;       /**< \brief Write-back Format Bitfiled Access */
    IfxGeth_TxContextDescr0_Bits C;       /**< \brief Context Descriptor Format Bitfiled Access */
    uint32                       U;       /**< \brief Unsigned access */
} IfxGeth_TxDescr0;

/** \brief TDES1
 */
typedef union
{
    IfxGeth_TxDescr1_RF_Bits     R;       /**< \brief Read Format Bitfiled Access */
    IfxGeth_TxDescr1_WF_Bits     W;       /**< \brief Write-back Format Bitfiled Access */
    IfxGeth_TxContextDescr1_Bits C;       /**< \brief Context Descriptor Format Bitfiled Access */
    uint32                       U;       /**< \brief Unsigned access */
} IfxGeth_TxDescr1;

/** \brief TDES2
 */
typedef union
{
    IfxGeth_TxDescr2_RF_Bits     R;       /**< \brief Read Format Bitfiled Access */
    IfxGeth_TxDescr2_WF_Bits     W;       /**< \brief Write-back Format Bitfiled Access */
    IfxGeth_TxContextDescr2_Bits C;       /**< \brief COntext Descriptor Format Bitfiled Access */
    uint32                       U;       /**< \brief Unsigned access */
} IfxGeth_TxDescr2;

/** \brief TDES3
 */
typedef union
{
    IfxGeth_TxDescr3_RF_Bits     R;       /**< \brief Read Format Bitfiled Access */
    IfxGeth_TxDescr3_WF_Bits     W;       /**< \brief Write-back Format Bitfiled Access */
    IfxGeth_TxContextDescr3_Bits C;       /**< \brief Context Descriptor Format Bitfiled Access */
    uint32                       U;       /**< \brief Unsigned access */
} IfxGeth_TxDescr3;

/** \} */

/** \addtogroup IfxLld_Geth_Std_DataStructures
 * \{ */
/** \brief Rx Descriptor
 */
typedef struct
{
    IfxGeth_RxDescr0 RDES0;       /**< \brief Rx Descriptor DWORD 0 */
    IfxGeth_RxDescr1 RDES1;       /**< \brief Rx Descriptor DWORD 1 */
    IfxGeth_RxDescr2 RDES2;       /**< \brief Rx Descriptor DWORD 2 */
    IfxGeth_RxDescr3 RDES3;       /**< \brief Rx Descriptor DWORD 3 */
} IfxGeth_RxDescr;

/** \brief Tx Descriptor
 */
typedef struct
{
    IfxGeth_TxDescr0 TDES0;       /**< \brief Tx Descriptor DWORD 0 */
    IfxGeth_TxDescr1 TDES1;       /**< \brief Tx Descriptor DWORD 1 */
    IfxGeth_TxDescr2 TDES2;       /**< \brief Tx Descriptor DWORD 2 */
    IfxGeth_TxDescr3 TDES3;       /**< \brief Tx Descriptor DWORD 3 */
} IfxGeth_TxDescr;

/** \} */

/** \addtogroup IfxLld_Geth_Std_Unions
 * \{ */
/** \brief Rx Descriptor List
 */
typedef union
{
    volatile IfxGeth_RxDescr descr[IFXGETH_MAX_RX_DESCRIPTORS];       /**< \brief list of RX descriptors */
} IfxGeth_RxDescrList;

/** \brief Tx Descriptor List
 */
typedef union
{
    volatile IfxGeth_TxDescr descr[IFXGETH_MAX_TX_DESCRIPTORS];       /**< \brief list of TX descriptors */
} IfxGeth_TxDescrList;

/** \} */

/** \addtogroup IfxLld_Geth_Std_MAC_Functions
 * \{ */

/******************************************************************************/
/*-------------------------Inline Function Prototypes-------------------------*/
/******************************************************************************/

/** \brief Disables the Receiver of MAC Core
 * \param gethSFR Pointer to GETH register base address
 * \return None
 */
IFX_INLINE void IfxGeth_mac_disableReceiver(Ifx_GETH *gethSFR);

/** \brief Disables the Trasnmitter of MAC Core
 * \param gethSFR Pointer to GETH register base address
 * \return None
 */
IFX_INLINE void IfxGeth_mac_disableTransmitter(Ifx_GETH *gethSFR);

/** \brief Enables the Receiver of MAC Core
 * \param gethSFR Pointer to GETH register base address
 * \return None
 */
IFX_INLINE void IfxGeth_mac_enableReceiver(Ifx_GETH *gethSFR);

/** \brief Enables the Trasnmitter of MAC Core
 * \param gethSFR Pointer to GETH register base address
 * \return None
 */
IFX_INLINE void IfxGeth_mac_enableTransmitter(Ifx_GETH *gethSFR);

/** \brief Enables / Disables Passing of All Multicast packets
 * \param gethSFR Pointer to GETH register base address
 * \param enabled Pass All Multicast enable / disable
 * \return None
 */
IFX_INLINE void IfxGeth_mac_setAllMulticastPassing(Ifx_GETH *gethSFR, boolean enabled);

/** \brief Enables / Disables CRC Checking for Received Packets
 * \param gethSFR Pointer to GETH register base address
 * \param enabled CRC Checking for Received Packets enable / disable
 * \return None
 */
IFX_INLINE void IfxGeth_mac_setCrcChecking(Ifx_GETH *gethSFR, boolean enabled);

/** \brief Enables/Disables the Automatic Pad or CRC Stripping for frames less than 1536 bytes and \n
 *     CRC stripping for Type packets
 * \param gethSFR Pointer to GETH register base address
 * \param acsEnabled Automatic Pad or CRC Stripping enable/disable
 * \param cstEnabled CRC Stripping for Type packets enable/disable
 * \return None
 */
IFX_INLINE void IfxGeth_mac_setCrcStripping(Ifx_GETH *gethSFR, boolean acsEnabled, boolean cstEnabled);

/** \brief Sets the Duplex Mode
 * \param gethSFR Pointer to GETH register base address
 * \param mode Duplex Mode
 * \return None
 */
IFX_INLINE void IfxGeth_mac_setDuplexMode(Ifx_GETH *gethSFR, IfxGeth_DuplexMode mode);

/** \brief Sets the Preamble Length for Transmit packets
 * \param gethSFR Pointer to GETH register base address
 * \param length Preamble Length for Transmit packets
 * \return None
 */
IFX_INLINE void IfxGeth_mac_setPreambleLength(Ifx_GETH *gethSFR, IfxGeth_PreambleLength length);

/** \brief Enables / Disables Promiscuous Mode
 * \param gethSFR Pointer to GETH register base address
 * \param enabled Promiscuous Mode enable / disable
 * \return None
 */
IFX_INLINE void IfxGeth_mac_setPromiscuousMode(Ifx_GETH *gethSFR, boolean enabled);

/** \brief Set the priorities for Rx queues for mapping tagged packets
 * \param gethSFR Pointer to GETH register base address
 * \param channel Rx DMA channel
 * \param priorities Rx queue priority
 * \return None
 */
IFX_INLINE void IfxGeth_mac_setVlanPriorityQueueRouting(Ifx_GETH *gethSFR, IfxGeth_RxDmaChannel channel, uint8 priorities);

/** \brief Enable / Disable Queue Insertion
 * \param gethSFR Pointer to GETH register base address
 * \param enable config parameter TRUE: enabled/ FALSE: disabled
 * \return None
 */
IFX_INLINE void IfxGeth_mac_setQueueVlanInsertion(Ifx_GETH *gethSFR, boolean enable);

/******************************************************************************/
/*-------------------------Global Function Prototypes-------------------------*/
/******************************************************************************/

/** \brief Sets the Ethernet Line Speed
 * \param gethSFR Pointer to GETH register base address
 * \param speed Ethernet Line Speed
 * \return None
 */
IFX_EXTERN void IfxGeth_mac_setLineSpeed(Ifx_GETH *gethSFR, IfxGeth_LineSpeed speed);

/** \brief Sets the MAC Address
 * \param gethSFR Pointer to GETH register base address
 * \param macAddress MAC Address
 * \return None
 */
IFX_EXTERN void IfxGeth_mac_setMacAddress(Ifx_GETH *gethSFR, uint8 *macAddress);

/** \brief Writes to Queue Vlan tag
 * \param gethSFR Pointer to GETH register base address
 * \param queueId Queue Index
 * \param vLanTag VLAN Tag
 * \return TRUE if write was successfull
 */
IFX_EXTERN boolean IfxGeth_mac_writeQueueVlanTag(Ifx_GETH *gethSFR, IfxGeth_MtlQueue queueId, uint16 vLanTag);

/** \brief Reads from Queue Vlan tag
 * \param gethSFR Pointer to GETH register base address
 * \param queueId Queue Index
 * \param const vLanTag VLAN Tag
 * \return TRUE if read was successfull
 */
IFX_EXTERN boolean IfxGeth_mac_readQueueVlanTag(Ifx_GETH *gethSFR, IfxGeth_MtlQueue queueId, uint16 *const vLanTag);

/** \} */

/** \addtogroup IfxLld_Geth_Std_Module_Functions
 * \{ */

/******************************************************************************/
/*-------------------------Inline Function Prototypes-------------------------*/
/******************************************************************************/

/** \brief Returns the status of whether clock for GETH module is enabled or diabled
 * \param gethSFR Pointer to GETH register base address
 * \return Status:\n
 * TRUE : if module is enabled\n
 * FALSE : if module is disabled
 */
IFX_INLINE boolean IfxGeth_isModuleEnabled(Ifx_GETH *gethSFR);

/** \brief Sets the External Phy Interface mode
 * \param gethSFR Pointer to GETH register base address
 * \param mode PHY Interface Mode (MII/RGMII/RMII)
 * \return None
 */
IFX_INLINE void IfxGeth_setPhyInterfaceMode(Ifx_GETH *gethSFR, IfxGeth_PhyInterfaceMode mode);

/******************************************************************************/
/*-------------------------Global Function Prototypes-------------------------*/
/******************************************************************************/

/** \brief Disables the clock for GETH module
 * \param gethSFR Pointer to GETH register base address
 * \return None
 */
IFX_EXTERN void IfxGeth_disableModule(Ifx_GETH *gethSFR);

/** \brief Enables the clock for GETH module
 * \param gethSFR Pointer to GETH register base address
 * \return None
 */
IFX_EXTERN void IfxGeth_enableModule(Ifx_GETH *gethSFR);

/** \brief Returns the Src Pointer of the selected GETH service request node
 * \param gethSFR Pointer to GETH register base address
 * \param serviceRequest Service Request number
 * \return pointer to Src register
 */
IFX_EXTERN volatile Ifx_SRC_SRCR *IfxGeth_getSrcPointer(Ifx_GETH *gethSFR, IfxGeth_ServiceRequest serviceRequest);

/** \brief resets Ethernet kernel
 * \param gethSFR Pointer to GETH register base address
 * \return None
 */
IFX_EXTERN void IfxGeth_resetModule(Ifx_GETH *gethSFR);

/** \brief API to get the resource index of the GETH specified.
 * \param geth Pointer to Geth register
 * \return geth resource index
 */
IFX_EXTERN IfxGeth_Index IfxGeth_getIndex(Ifx_GETH *geth);

/** \brief Get the address of the specified GETH resource index
 * \param geth Module index of the GETH
 * \return GETH module register address
 */
IFX_EXTERN Ifx_GETH *IfxGeth_getAddress(IfxGeth_Index geth);

/** \} */

/** \addtogroup IfxLld_Geth_Std_MTL_Functions
 * \{ */

/******************************************************************************/
/*-------------------------Inline Function Prototypes-------------------------*/
/******************************************************************************/

/** \brief Enables the selected Rx Queue
 * \param gethSFR Pointer to GETH register base address
 * \param queueId Rx Queue Index
 * \return None
 */
IFX_INLINE void IfxGeth_mtl_enableRxQueue(Ifx_GETH *gethSFR, IfxGeth_RxMtlQueue queueId);

/** \brief sets the Rx Arbitration algorithm
 * \param gethSFR Pointer to GETH register base address
 * \param arbitrationAlgorithm Rx Arbitration Algorithm
 * \return None
 */
IFX_INLINE void IfxGeth_mtl_setRxArbitrationAlgorithm(Ifx_GETH *gethSFR, IfxGeth_RxArbitrationAlgorithm arbitrationAlgorithm);

/** \brief Sets the selected DMA Channel mapping for the selected Rx Queue
 * \param gethSFR Pointer to GETH register base address
 * \param queueId Rx Queue Index
 * \param dmaChannel DMA channel for selected Rx Queue
 * \return None
 */
IFX_INLINE void IfxGeth_mtl_setRxQueueDmaChannelMapping(Ifx_GETH *gethSFR, IfxGeth_RxMtlQueue queueId, IfxGeth_RxDmaChannel dmaChannel);

/** \brief Sets the selected Rx Queue for DA based DMA channel
 * \param gethSFR Pointer to GETH register base address
 * \param queueId Rx Queue Index
 * \param enabled Enable/Disable Receive Store and Forward
 * \return None
 */
IFX_INLINE void IfxGeth_mtl_setRxQueueForDaBasedDmaChannel(Ifx_GETH *gethSFR, IfxGeth_RxMtlQueue queueId, boolean enabled);

/** \brief sets the Tx Scheduling algorithm
 * \param gethSFR Pointer to GETH register base address
 * \param schedulingAlgorithm Tx Scheduling Algorithm
 * \return None
 */
IFX_INLINE void IfxGeth_mtl_setTxSchedulingAlgorithm(Ifx_GETH *gethSFR, IfxGeth_TxSchedulingAlgorithm schedulingAlgorithm);

/** \brief Enable and set queue ID to route packets failing unicast filter
 * \param gethSFR Pointer to GETH register base address
 * \param queueId Rx MTL Queue ID
 * \return None
 */
IFX_INLINE void IfxGeth_mtl_enableUnicastFilterFailQueuing(Ifx_GETH *gethSFR, IfxGeth_RxMtlQueue queueId);

/** \brief Disable queuing of packets failing unicast filtering
 * \param gethSFR Pointer to GETH register base address
 * \return None
 */
IFX_INLINE void IfxGeth_mtl_disableUnicastFilterFailQueuing(Ifx_GETH *gethSFR);

/** \brief Enable and set queue ID to route packets failing multicast filter
 * \param gethSFR Pointer to GETH register base address
 * \param queueId Rx MTL Queue ID
 * \return None
 */
IFX_INLINE void IfxGeth_mtl_enableMulticastFilterFailQueuing(Ifx_GETH *gethSFR, IfxGeth_RxMtlQueue queueId);

/** \brief Disable queuing of packets failing multicast filtering
 * \param gethSFR Pointer to GETH register base address
 * \return None
 */
IFX_INLINE void IfxGeth_mtl_disableMulticastFilterFailQueuing(Ifx_GETH *gethSFR);

/** \brief Enable and set queue ID to route packets failing VLAN tag filter
 * \param gethSFR Pointer to GETH register base address
 * \param queueId Rx MTL Queue ID
 * \return None
 */
IFX_INLINE void IfxGeth_mtl_enableVlanFilterFailQueuing(Ifx_GETH *gethSFR, IfxGeth_RxMtlQueue queueId);

/** \brief Disable queuing of packets failing VLAN tag filtering
 * \param gethSFR Pointer to GETH register base address
 * \return None
 */
IFX_INLINE void IfxGeth_mtl_disableVlanFilterFailQueuing(Ifx_GETH *gethSFR);

/******************************************************************************/
/*-------------------------Global Function Prototypes-------------------------*/
/******************************************************************************/

/** \brief Clears all the Mtl interrupt flags
 * \param gethSFR Pointer to GETH register base address
 * \param queueId Queue Index
 * \return None
 */
IFX_EXTERN void IfxGeth_mtl_clearAllInterruptFlags(Ifx_GETH *gethSFR, IfxGeth_MtlQueue queueId);

/** \brief Clears the selected Mtl interrupt flag
 * \param gethSFR Pointer to GETH register base address
 * \param queueId Queue Index
 * \param flag MTL interrupt flag
 * \return None
 */
IFX_EXTERN void IfxGeth_mtl_clearInterruptFlag(Ifx_GETH *gethSFR, IfxGeth_MtlQueue queueId, IfxGeth_MtlInterruptFlag flag);

/** \brief Disables the selected Mtl interrupt flag
 * \param gethSFR Pointer to GETH register base address
 * \param queueId Queue Index
 * \param flag MTL interrupt flag
 * \return None
 */
IFX_EXTERN void IfxGeth_mtl_disableInterrupt(Ifx_GETH *gethSFR, IfxGeth_MtlQueue queueId, IfxGeth_MtlInterruptFlag flag);

/** \brief Enables the selected Mtl interrupt flag
 * \param gethSFR Pointer to GETH register base address
 * \param queueId Queue Index
 * \param flag MTL interrupt flag
 * \return None
 */
IFX_EXTERN void IfxGeth_mtl_enableInterrupt(Ifx_GETH *gethSFR, IfxGeth_MtlQueue queueId, IfxGeth_MtlInterruptFlag flag);

/** \brief Enables the selected TX Queue
 * \param gethSFR Pointer to GETH register base address
 * \param queueId Tx Queue Index
 * \return None
 */
IFX_EXTERN void IfxGeth_mtl_enableTxQueue(Ifx_GETH *gethSFR, IfxGeth_TxMtlQueue queueId);

/** \brief Returns the status of selected Mtl interrupt flag
 * \param gethSFR Pointer to GETH register base address
 * \param queueId Queue Index
 * \param flag MTL interrupt flag
 * \return TRUE: if set\n
 * FLASE : if not set
 */
IFX_EXTERN boolean IfxGeth_mtl_isInterruptFlagSet(Ifx_GETH *gethSFR, IfxGeth_MtlQueue queueId, IfxGeth_MtlInterruptFlag flag);

/** \brief Sets the Receive Forward Error Packets for the selected RX Queue
 * \param gethSFR Pointer to GETH register base address
 * \param queueId Rx Queue Index
 * \param enabled Enable/Disable Receive Forward Error Packets
 * \return None
 */
IFX_EXTERN void IfxGeth_mtl_setRxForwardErrorPacket(Ifx_GETH *gethSFR, IfxGeth_RxMtlQueue queueId, boolean enabled);

/** \brief Sets the Receive Forward Undersized Good Packets for the selected RX Queue
 * \param gethSFR Pointer to GETH register base address
 * \param queueId Rx Queue Index
 * \param enabled Enable/Disable Receive Forward Undersized Good Packets
 * \return None
 */
IFX_EXTERN void IfxGeth_mtl_setRxForwardUndersizedGoodPacket(Ifx_GETH *gethSFR, IfxGeth_RxMtlQueue queueId, boolean enabled);

/** \brief Sets the size of the selected RX Queue
 * \param gethSFR Pointer to GETH register base address
 * \param queueId Rx Queue Index
 * \param queueSize Rx Queue Size
 * \return None
 */
IFX_EXTERN void IfxGeth_mtl_setRxQueueSize(Ifx_GETH *gethSFR, IfxGeth_RxMtlQueue queueId, IfxGeth_QueueSize queueSize);

/** \brief Sets the Receive Store And Forward for the selected RX Queue
 * \param gethSFR Pointer to GETH register base address
 * \param queueId Rx Queue Index
 * \param enabled Enable/Disable Receive Store and Forward
 * \return None
 */
IFX_EXTERN void IfxGeth_mtl_setRxStoreAndForward(Ifx_GETH *gethSFR, IfxGeth_RxMtlQueue queueId, boolean enabled);

/** \brief Sets the size of the selected TX Queue
 * \param gethSFR Pointer to GETH register base address
 * \param queueId Tx Queue Index
 * \param queueSize Tx Queue Size
 * \return None
 */
IFX_EXTERN void IfxGeth_mtl_setTxQueueSize(Ifx_GETH *gethSFR, IfxGeth_TxMtlQueue queueId, IfxGeth_QueueSize queueSize);

/** \brief Sets the Transmit Store And Forward for teh selected TX Queue
 * \param gethSFR Pointer to GETH register base address
 * \param queueId Tx Queue Index
 * \param enabled Enable/Disable Transmit Store and Forward
 * \return None
 */
IFX_EXTERN void IfxGeth_mtl_setTxStoreAndForward(Ifx_GETH *gethSFR, IfxGeth_TxMtlQueue queueId, boolean enabled);

/** \} */

/** \addtogroup IfxLld_Geth_Std_DMA_Functions
 * \{ */

/******************************************************************************/
/*-------------------------Inline Function Prototypes-------------------------*/
/******************************************************************************/

/** \brief Applies a software reset of MAC and DMA controller
 * \param gethSFR Pointer to GETH register base address
 * \return None
 */
IFX_INLINE void IfxGeth_dma_applySoftwareReset(Ifx_GETH *gethSFR);

/** \brief Returns the status of selected DMA interrupt flag
 * \param gethSFR Pointer to GETH register base address
 * \param channelId DMA channel Id
 * \return None
 */
IFX_INLINE void IfxGeth_dma_clearAllInterruptFlags(Ifx_GETH *gethSFR, IfxGeth_DmaChannel channelId);

/** \brief Returns the status of selected DMA interrupt flag
 * \param gethSFR Pointer to GETH register base address
 * \param channelId DMA channel Id
 * \param flag DMA interrupt flag
 * \return None
 */
IFX_INLINE void IfxGeth_dma_clearInterruptFlag(Ifx_GETH *gethSFR, IfxGeth_DmaChannel channelId, IfxGeth_DmaInterruptFlag flag);

/** \brief Disables the selected DMA interrupt
 * \param gethSFR Pointer to GETH register base address
 * \param channelId DMA channel Id
 * \param flag DMA interrupt flag
 * \return None
 */
IFX_INLINE void IfxGeth_dma_disableInterrupt(Ifx_GETH *gethSFR, IfxGeth_DmaChannel channelId, IfxGeth_DmaInterruptFlag flag);

/** \brief Enables the selected DMA interrupt
 * \param gethSFR Pointer to GETH register base address
 * \param channelId DMA channel Id
 * \param flag DMA interrupt flag
 * \return None
 */
IFX_INLINE void IfxGeth_dma_enableInterrupt(Ifx_GETH *gethSFR, IfxGeth_DmaChannel channelId, IfxGeth_DmaInterruptFlag flag);

/** \brief Returns the status of selected DMA interrupt flag
 * \param gethSFR Pointer to GETH register base address
 * \param channelId DMA channel Id
 * \param flag DMA interrupt flag
 * \return TRUE: if set\n
 * FLASE : if not set
 */
IFX_INLINE boolean IfxGeth_dma_isInterruptFlagSet(Ifx_GETH *gethSFR, IfxGeth_DmaChannel channelId, IfxGeth_DmaInterruptFlag flag);

/** \brief Wiats until software reset of MAC and DMA controller is done or until timout.
 * \param gethSFR Pointer to GETH register base address
 */
IFX_INLINE boolean IfxGeth_dma_isSoftwareResetDone(Ifx_GETH *gethSFR);

/** \brief Enables/Disables Address aligned beats
 * \param gethSFR Pointer to GETH register base address
 * \param enabled Enable/Disable Address Aligned Beats
 * \return None
 */
IFX_INLINE void IfxGeth_dma_setAddressAlignedBeats(Ifx_GETH *gethSFR, boolean enabled);

/** \brief Enables/Disables Fixed Burst Length
 * \param gethSFR Pointer to GETH register base address
 * \param enabled Enable/Disable Fixed Burst
 * \return None
 */
IFX_INLINE void IfxGeth_dma_setFixedBurst(Ifx_GETH *gethSFR, boolean enabled);

/** \brief Enables/Disables Mixed Burst Length
 * \param gethSFR Pointer to GETH register base address
 * \param enabled Enable/Disable Mixed Burst
 * \return None
 */
IFX_INLINE void IfxGeth_dma_setMixedBurst(Ifx_GETH *gethSFR, boolean enabled);

/** \brief sets the size of the rx buffers in descriptors for selected Rx channel of DMA
 * \param gethSFR Pointer to GETH register base address
 * \param channel Rx channel Id
 * \param size Rx Buffers size (applies for both buffers)
 * \return None
 */
IFX_INLINE void IfxGeth_dma_setRxBufferSize(Ifx_GETH *gethSFR, IfxGeth_RxDmaChannel channel, uint16 size);

/** \brief sets the base address of the first descriptor in the Receive descriptor list
 * \param gethSFR Pointer to GETH register base address
 * \param channel Rx channel Id
 * \param address base address of the first descriptor in the Receive descriptor list
 * \return None
 */
IFX_INLINE void IfxGeth_dma_setRxDescriptorListAddress(Ifx_GETH *gethSFR, IfxGeth_RxDmaChannel channel, uint32 address);

/** \brief sets the Length of the Rx descriptors ring
 * \param gethSFR Pointer to GETH register base address
 * \param channel Rx channel Id
 * \param length Length of the ring (no of total Rx descriptors)
 * \return None
 */
IFX_INLINE void IfxGeth_dma_setRxDescriptorRingLength(Ifx_GETH *gethSFR, IfxGeth_RxDmaChannel channel, uint32 length);

/** \brief sets the address of the last valid descriptor in the Receive descriptor list
 * \param gethSFR Pointer to GETH register base address
 * \param channel Rx channel Id
 * \param address address of the last valid descriptor in the Receive descriptor list
 * \return None
 */
IFX_INLINE void IfxGeth_dma_setRxDescriptorTailPointer(Ifx_GETH *gethSFR, IfxGeth_RxDmaChannel channel, uint32 address);

/** \brief sets the programmable burst length of selected Rx channel of DMA
 * \param gethSFR Pointer to GETH register base address
 * \param channel Rx channel Id
 * \param length Programmable burst length
 * \return None
 */
IFX_INLINE void IfxGeth_dma_setRxMaxBurstLength(Ifx_GETH *gethSFR, IfxGeth_RxDmaChannel channel, IfxGeth_DmaBurstLength length);

/** \brief sets the base address of the first descriptor in the Transmit descriptor list
 * \param gethSFR Pointer to GETH register base address
 * \param channel Tx channel Id
 * \param address base address of the first descriptor in the Transmit descriptor list
 * \return None
 */
IFX_INLINE void IfxGeth_dma_setTxDescriptorListAddress(Ifx_GETH *gethSFR, IfxGeth_TxDmaChannel channel, uint32 address);

/** \brief sets the Length of the Tx descriptors ring
 * \param gethSFR Pointer to GETH register base address
 * \param channel Tx channel Id
 * \param length Length of the ring (no of total Tx descriptors)
 * \return None
 */
IFX_INLINE void IfxGeth_dma_setTxDescriptorRingLength(Ifx_GETH *gethSFR, IfxGeth_TxDmaChannel channel, uint32 length);

/** \brief sets the address of the last valid descriptor in the Transmit descriptor list
 * \param gethSFR Pointer to GETH register base address
 * \param channel Tx channel Id
 * \param address address of the last valid descriptor in the Transmit descriptor list
 * \return None
 */
IFX_INLINE void IfxGeth_dma_setTxDescriptorTailPointer(Ifx_GETH *gethSFR, IfxGeth_TxDmaChannel channel, uint32 address);

/** \brief sets the programmable burst length of selected Tx channel of DMA
 * \param gethSFR Pointer to GETH register base address
 * \param channel Tx channel Id
 * \param length Programmable burst length
 * \return None
 */
IFX_INLINE void IfxGeth_dma_setTxMaxBurstLength(Ifx_GETH *gethSFR, IfxGeth_TxDmaChannel channel, IfxGeth_DmaBurstLength length);

/** \brief starts the receiver of selected Rx channel of DMA
 * \param gethSFR Pointer to GETH register base address
 * \param channel Rx channel Id
 * \return None
 */
IFX_INLINE void IfxGeth_dma_startReceiver(Ifx_GETH *gethSFR, IfxGeth_RxDmaChannel channel);

/** \brief starts the tramsimitter of selected Tx channel of DMA
 * \param gethSFR Pointer to GETH register base address
 * \param channel Tx channel Id
 * \return None
 */
IFX_INLINE void IfxGeth_dma_startTransmitter(Ifx_GETH *gethSFR, IfxGeth_TxDmaChannel channel);

/** \brief Stops the tramsimitter of selected Tx channel of DMA
 * \param gethSFR Pointer to GETH register base address
 * \param channel Tx channel Id
 * \return None
 */
IFX_INLINE void IfxGeth_dma_stopTransmitter(Ifx_GETH *gethSFR, IfxGeth_TxDmaChannel channel);

/** \} */

/******************************************************************************/
/*-------------------------Inline Function Prototypes-------------------------*/
/******************************************************************************/

/** \brief Enables/Disables the MAC operation in the loopback mode at GMII or MII
 * \param gethSFR Pointer to GETH register base address
 * \param mode Loopback Mode Enable / Disable
 * \return None
 */
IFX_INLINE void IfxGeth_mac_setLoopbackMode(Ifx_GETH *gethSFR, IfxGeth_LoopbackMode mode);

/******************************************************************************/
/*-------------------------Global Function Prototypes-------------------------*/
/******************************************************************************/

/** \brief Reads a MDIO register of Clause 22 PHY
 * \param layerAddr Layer Address
 * \param regAddr Register Address
 * \param pData Pointer to Data
 * \return None
 */
IFX_EXTERN void IfxGeth_phy_Clause22_readMDIORegister(uint32 layerAddr, uint32 regAddr, uint32 *pData);

/** \brief Writes to a MDIO register of Clause 22 PHY
 * \param layerAddr Layer Address
 * \param regAddr Register Address
 * \param data Data
 * \return None
 */
IFX_EXTERN void IfxGeth_Phy_Clause22_writeMDIORegister(uint32 layerAddr, uint32 regAddr, uint32 data);

/** \brief Reads a MDIO register of Clause 45 PHY
 * \param layerAddr Layer Address
 * \param deviceAddr Device Address
 * \param regAddr Register Address
 * \param pData Pointer to Data
 * \return None
 */
IFX_EXTERN void IfxGeth_phy_Clause45_readMDIORegister(uint32 layerAddr, uint32 deviceAddr, uint32 regAddr, uint32 *pData);

/** \brief Writes to a MDIO register for Clause 45 PHY
 * \param layerAddr Layer Address
 * \param deviceAddr Device Address
 * \param regAddr Register Address
 * \param data Data
 * \return None
 */
IFX_EXTERN void IfxGeth_Phy_Clause45_writeMDIORegister(uint32 layerAddr, uint32 deviceAddr, uint32 regAddr, uint32 data);

/** \brief Sets the maximum size of the packet
 * \param gethSFR Pointer to GETH register base address
 * \param maxPacketSize Minimum size of the frame beyond which giant packet status is set.
 * \return None
 */
IFX_EXTERN void IfxGeth_mac_setMaxPacketSize(Ifx_GETH *gethSFR, uint16 maxPacketSize);

/******************************************************************************/
/*---------------------Inline Function Implementations------------------------*/
/******************************************************************************/

IFX_INLINE void IfxGeth_dma_applySoftwareReset(Ifx_GETH *gethSFR)
{
    gethSFR->DMA_MODE.B.SWR = 1;
}


IFX_INLINE void IfxGeth_dma_clearAllInterruptFlags(Ifx_GETH *gethSFR, IfxGeth_DmaChannel channelId)
{
    gethSFR->DMA_CH[channelId].STATUS.U = 0;
}


IFX_INLINE void IfxGeth_dma_clearInterruptFlag(Ifx_GETH *gethSFR, IfxGeth_DmaChannel channelId, IfxGeth_DmaInterruptFlag flag)
{
    uint32 value = (1 << flag);
    gethSFR->DMA_CH[channelId].STATUS.U = value;
}


IFX_INLINE void IfxGeth_dma_disableInterrupt(Ifx_GETH *gethSFR, IfxGeth_DmaChannel channelId, IfxGeth_DmaInterruptFlag flag)
{
    uint32 value = ~(1 << flag);
    gethSFR->DMA_CH[channelId].INTERRUPT_ENABLE.U &= value;
}


IFX_INLINE void IfxGeth_dma_enableInterrupt(Ifx_GETH *gethSFR, IfxGeth_DmaChannel channelId, IfxGeth_DmaInterruptFlag flag)
{
    uint32 value = (1 << flag);
    gethSFR->DMA_CH[channelId].INTERRUPT_ENABLE.U |= value;
}


IFX_INLINE boolean IfxGeth_dma_isInterruptFlagSet(Ifx_GETH *gethSFR, IfxGeth_DmaChannel channelId, IfxGeth_DmaInterruptFlag flag)
{
    uint32 value = (1 << flag);
    return gethSFR->DMA_CH[channelId].STATUS.U & (Ifx_UReg_32Bit)value;
}


IFX_INLINE boolean IfxGeth_dma_isSoftwareResetDone(Ifx_GETH *gethSFR)
{
    return gethSFR->DMA_MODE.B.SWR == 0;
}


IFX_INLINE void IfxGeth_dma_setAddressAlignedBeats(Ifx_GETH *gethSFR, boolean enabled)
{
    gethSFR->DMA_SYSBUS_MODE.B.AAL = enabled;
}


IFX_INLINE void IfxGeth_dma_setFixedBurst(Ifx_GETH *gethSFR, boolean enabled)
{
    gethSFR->DMA_SYSBUS_MODE.B.FB = enabled;
}


IFX_INLINE void IfxGeth_dma_setMixedBurst(Ifx_GETH *gethSFR, boolean enabled)
{
    gethSFR->DMA_SYSBUS_MODE.B.MB = enabled;
}


IFX_INLINE void IfxGeth_dma_setRxBufferSize(Ifx_GETH *gethSFR, IfxGeth_RxDmaChannel channel, uint16 size)
{
    /* [RBSZ_13_Y:RBSZ_X_0] represents the size of the Rx buffer*/
    gethSFR->DMA_CH[channel].RX_CONTROL.B.RBSZ_13_Y = (uint32)((size >> 2) & IFX_GETH_DMA_CH_RX_CONTROL_RBSZ_13_Y_MSK);
}


IFX_INLINE void IfxGeth_dma_setRxDescriptorListAddress(Ifx_GETH *gethSFR, IfxGeth_RxDmaChannel channel, uint32 address)
{
    gethSFR->DMA_CH[channel].RXDESC_LIST_ADDRESS.U = (uint32)address;
}


IFX_INLINE void IfxGeth_dma_setRxDescriptorRingLength(Ifx_GETH *gethSFR, IfxGeth_RxDmaChannel channel, uint32 length)
{
    gethSFR->DMA_CH[channel].RXDESC_RING_LENGTH.U = length;
}


IFX_INLINE void IfxGeth_dma_setRxDescriptorTailPointer(Ifx_GETH *gethSFR, IfxGeth_RxDmaChannel channel, uint32 address)
{
    gethSFR->DMA_CH[channel].RXDESC_TAIL_POINTER.U = address;
}


IFX_INLINE void IfxGeth_dma_setRxMaxBurstLength(Ifx_GETH *gethSFR, IfxGeth_RxDmaChannel channel, IfxGeth_DmaBurstLength length)
{
    gethSFR->DMA_CH[channel].RX_CONTROL.B.RXPBL = length;
}


IFX_INLINE void IfxGeth_dma_setTxDescriptorListAddress(Ifx_GETH *gethSFR, IfxGeth_TxDmaChannel channel, uint32 address)
{
    gethSFR->DMA_CH[channel].TXDESC_LIST_ADDRESS.U = address;
}


IFX_INLINE void IfxGeth_dma_setTxDescriptorRingLength(Ifx_GETH *gethSFR, IfxGeth_TxDmaChannel channel, uint32 length)
{
    gethSFR->DMA_CH[channel].TXDESC_RING_LENGTH.U = length;
}


IFX_INLINE void IfxGeth_dma_setTxDescriptorTailPointer(Ifx_GETH *gethSFR, IfxGeth_TxDmaChannel channel, uint32 address)
{
    gethSFR->DMA_CH[channel].TXDESC_TAIL_POINTER.U = (uint32)address;
}


IFX_INLINE void IfxGeth_dma_setTxMaxBurstLength(Ifx_GETH *gethSFR, IfxGeth_TxDmaChannel channel, IfxGeth_DmaBurstLength length)
{
    gethSFR->DMA_CH[channel].TX_CONTROL.B.TXPBL = length;
}


IFX_INLINE void IfxGeth_dma_startReceiver(Ifx_GETH *gethSFR, IfxGeth_RxDmaChannel channel)
{
    gethSFR->DMA_CH[channel].RX_CONTROL.B.SR = TRUE;
}


IFX_INLINE void IfxGeth_dma_startTransmitter(Ifx_GETH *gethSFR, IfxGeth_TxDmaChannel channel)
{
    gethSFR->DMA_CH[channel].TX_CONTROL.B.ST = TRUE;
}


IFX_INLINE void IfxGeth_dma_stopTransmitter(Ifx_GETH *gethSFR, IfxGeth_TxDmaChannel channel)
{
    gethSFR->DMA_CH[channel].TX_CONTROL.B.ST = FALSE;
}


IFX_INLINE boolean IfxGeth_isModuleEnabled(Ifx_GETH *gethSFR)
{
    return (gethSFR->CLC.B.DISS == 0) ? 1 : 0;
}


IFX_INLINE void IfxGeth_mac_disableReceiver(Ifx_GETH *gethSFR)
{
    gethSFR->MAC_CONFIGURATION.B.RE = 0;
}


IFX_INLINE void IfxGeth_mac_disableTransmitter(Ifx_GETH *gethSFR)
{
    gethSFR->MAC_CONFIGURATION.B.TE = 0;
}


IFX_INLINE void IfxGeth_mac_enableReceiver(Ifx_GETH *gethSFR)
{
    gethSFR->MAC_CONFIGURATION.B.RE = 1;
}


IFX_INLINE void IfxGeth_mac_enableTransmitter(Ifx_GETH *gethSFR)
{
    gethSFR->MAC_CONFIGURATION.B.TE = 1;
}


IFX_INLINE void IfxGeth_mac_setAllMulticastPassing(Ifx_GETH *gethSFR, boolean enabled)
{
    gethSFR->MAC_PACKET_FILTER.B.PM = ((enabled == 1) ? 1 : 0);
}


IFX_INLINE void IfxGeth_mac_setCrcChecking(Ifx_GETH *gethSFR, boolean enabled)
{
    gethSFR->MAC_EXT_CONFIGURATION.B.DCRCC = ((enabled == 1) ? 0 : 1);
}


IFX_INLINE void IfxGeth_mac_setCrcStripping(Ifx_GETH *gethSFR, boolean acsEnabled, boolean cstEnabled)
{
    gethSFR->MAC_CONFIGURATION.B.ACS = ((acsEnabled == 1) ? 1 : 0);
    gethSFR->MAC_CONFIGURATION.B.CST = ((cstEnabled == 1) ? 1 : 0);
}


IFX_INLINE void IfxGeth_mac_setDuplexMode(Ifx_GETH *gethSFR, IfxGeth_DuplexMode mode)
{
    gethSFR->MAC_CONFIGURATION.B.DM = mode;
}


IFX_INLINE void IfxGeth_mac_setLoopbackMode(Ifx_GETH *gethSFR, IfxGeth_LoopbackMode mode)
{
    gethSFR->MAC_CONFIGURATION.B.LM = mode;
}


IFX_INLINE void IfxGeth_mac_setPreambleLength(Ifx_GETH *gethSFR, IfxGeth_PreambleLength length)
{
    gethSFR->MAC_CONFIGURATION.B.PRELEN = length;
}


IFX_INLINE void IfxGeth_mac_setPromiscuousMode(Ifx_GETH *gethSFR, boolean enabled)
{
    gethSFR->MAC_PACKET_FILTER.B.PR = ((enabled == 1) ? 1 : 0);
}


IFX_INLINE void IfxGeth_mtl_enableRxQueue(Ifx_GETH *gethSFR, IfxGeth_RxMtlQueue queueId)
{
    gethSFR->MAC_RXQ_CTRL0.U |= (2 << (queueId * 2));
}


IFX_INLINE void IfxGeth_mtl_setRxArbitrationAlgorithm(Ifx_GETH *gethSFR, IfxGeth_RxArbitrationAlgorithm arbitrationAlgorithm)
{
    gethSFR->MTL_OPERATION_MODE.B.RAA = arbitrationAlgorithm;
}


IFX_INLINE void IfxGeth_mtl_setRxQueueDmaChannelMapping(Ifx_GETH *gethSFR, IfxGeth_RxMtlQueue queueId, IfxGeth_RxDmaChannel dmaChannel)
{
    gethSFR->MTL_RXQ_DMA_MAP0.U |= (dmaChannel << (queueId * 8));
}


IFX_INLINE void IfxGeth_mtl_setRxQueueForDaBasedDmaChannel(Ifx_GETH *gethSFR, IfxGeth_RxMtlQueue queueId, boolean enabled)
{
    gethSFR->MTL_RXQ_DMA_MAP0.U |= (enabled << ((queueId * 8) + 4));
}


IFX_INLINE void IfxGeth_mtl_setTxSchedulingAlgorithm(Ifx_GETH *gethSFR, IfxGeth_TxSchedulingAlgorithm schedulingAlgorithm)
{
    gethSFR->MTL_OPERATION_MODE.B.SCHALG = schedulingAlgorithm;
}


IFX_INLINE void IfxGeth_setPhyInterfaceMode(Ifx_GETH *gethSFR, IfxGeth_PhyInterfaceMode mode)
{
    gethSFR->GPCTL.B.EPR = mode;
}


IFX_INLINE void IfxGeth_mac_setVlanPriorityQueueRouting(Ifx_GETH *gethSFR, IfxGeth_RxDmaChannel channel, uint8 priorities)
{
    gethSFR->MAC_RXQ_CTRL2.U = (gethSFR->MAC_RXQ_CTRL2.U & ((uint32)~(0xFFU << (channel * 8)))) | (priorities << (channel * 8));
}


IFX_INLINE void IfxGeth_mtl_enableUnicastFilterFailQueuing(Ifx_GETH *gethSFR, IfxGeth_RxMtlQueue queueId)
{
    gethSFR->MAC_RXQ_CTRL4.B.UFFQ  = queueId;
    gethSFR->MAC_RXQ_CTRL4.B.UFFQE = TRUE;
}


IFX_INLINE void IfxGeth_mtl_disableUnicastFilterFailQueuing(Ifx_GETH *gethSFR)
{
    gethSFR->MAC_RXQ_CTRL4.B.UFFQE = FALSE;
}


IFX_INLINE void IfxGeth_mtl_enableMulticastFilterFailQueuing(Ifx_GETH *gethSFR, IfxGeth_RxMtlQueue queueId)
{
    gethSFR->MAC_RXQ_CTRL4.B.MFFQ  = queueId;
    gethSFR->MAC_RXQ_CTRL4.B.MFFQE = TRUE;
}


IFX_INLINE void IfxGeth_mtl_disableMulticastFilterFailQueuing(Ifx_GETH *gethSFR)
{
    gethSFR->MAC_RXQ_CTRL4.B.MFFQE = FALSE;
}


IFX_INLINE void IfxGeth_mtl_enableVlanFilterFailQueuing(Ifx_GETH *gethSFR, IfxGeth_RxMtlQueue queueId)
{
    gethSFR->MAC_RXQ_CTRL4.B.VFFQ  = queueId;
    gethSFR->MAC_RXQ_CTRL4.B.VFFQE = TRUE;
}


IFX_INLINE void IfxGeth_mtl_disableVlanFilterFailQueuing(Ifx_GETH *gethSFR)
{
    gethSFR->MAC_RXQ_CTRL4.B.VFFQE = FALSE;
}


IFX_INLINE void IfxGeth_mac_setQueueVlanInsertion(Ifx_GETH *gethSFR, boolean enable)
{
    gethSFR->MAC_VLAN_INCL.B.CBTI = enable;
}


#endif /* IFXGET_H */
