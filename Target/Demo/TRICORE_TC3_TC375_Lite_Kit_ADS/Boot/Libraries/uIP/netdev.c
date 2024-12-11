/************************************************************************************//**
* \file         Demo/TRICORE_TC3_TC375_Lite_Kit_Master_ADS/Boot/Libraries/uIP/netdev.c
* \brief        uIP network device port source file.
* \ingroup      Boot_TRICORE_TC3_TC375_Lite_Kit_Master_ADS
* \internal
*----------------------------------------------------------------------------------------
*                          C O P Y R I G H T
*----------------------------------------------------------------------------------------
*   Copyright (c) 2001, Swedish Institute of Computer Science. All rights reserved.
*
*----------------------------------------------------------------------------------------
*                            L I C E N S E
*----------------------------------------------------------------------------------------
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions
* are met:
*
* 1. Redistributions of source code must retain the above copyright
*    notice, this list of conditions and the following disclaimer.
*
* 2. Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in the
*    documentation and/or other materials provided with the distribution.
*
* 3. Neither the name of the Institute nor the names of its contributors
*    may be used to endorse or promote products derived from this software
*    without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
* ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
* FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
* OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
* LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
* OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
* SUCH DAMAGE.
*
* Author: Adam Dunkels <adam@sics.se>
*
* This file is part of the uIP TCP/IP stack.
*
* \endinternal
****************************************************************************************/

/****************************************************************************************
* Include files
****************************************************************************************/
#include <string.h>                              /* String utils (memcpy)              */
#include "uip.h"                                 /* uIP TCP/IP stack                   */
#include "uip_arp.h"                             /* uIP address resolution protocol    */
#include "boot.h"                                /* Bootloader generic header          */
#include "IfxGeth_Eth.h"                         /* iLLD ethernet driver               */
#include "IfxGeth_Phy_Dp83825i.h"                /* Ethernet PHY DP83825I driver       */


/****************************************************************************************
* Macro definitions
****************************************************************************************/
/* The MAC address for the network interface. */
#define NETDEV_DEFAULT_MACADDR0           (0x08)
#define NETDEV_DEFAULT_MACADDR1           (0x00)
#define NETDEV_DEFAULT_MACADDR2           (0x27)
#define NETDEV_DEFAULT_MACADDR3           (0x69)
#define NETDEV_DEFAULT_MACADDR4           (0x5B)
#define NETDEV_DEFAULT_MACADDR5           (0x45)

/* Timeout time for transmitting a packet. */
#define NETDEV_TX_PACKET_TIMEOUT_MS       (250U)

/* Timeout time for allocating a new tranmsit buffer. */
#define NETDEV_TX_BUFF_ALLOC_TIMEOUT_MS   (10U)

/* GETH Tx/Rx buffer size definitions. */
#define IFXGETH_HEADER_LENGTH             (14)                                /* words */
#define IFXGETH_MAX_TX_BUFFER_SIZE        (2560+IFXGETH_HEADER_LENGTH+2)      /* bytes */
#define IFXGETH_MAX_RX_BUFFER_SIZE        (2560+IFXGETH_HEADER_LENGTH+2)      /* bytes */


/****************************************************************************************
* Local data declarations
****************************************************************************************/
/* MAC adress buffer. */
static struct uip_eth_addr macAddress;

/* GETH driver handle. */
static IfxGeth_Eth g_IfxGeth;

/* Ethernet Tx/Rx data buffers. */
static uint8 channel0TxBuffer1[IFXGETH_MAX_TX_DESCRIPTORS][IFXGETH_MAX_TX_BUFFER_SIZE];
static uint8 channel0RxBuffer1[IFXGETH_MAX_RX_DESCRIPTORS][IFXGETH_MAX_RX_BUFFER_SIZE];

/* Boolean flag to keep track of the link status. */
static uint8 linkUpFlag;


/****************************************************************************************
* Local constant declarations
****************************************************************************************/
/* Pin configuration of the DP83825I PHY. */
static const IfxGeth_Eth_RmiiPins rmii_pins = {
   .crsDiv = &IfxGeth_CRSDVA_P11_11_IN,                                      /* CRSDIV */
   .refClk = &IfxGeth_REFCLKA_P11_12_IN,                                     /* REFCLK */
   .rxd0   = &IfxGeth_RXD0A_P11_10_IN,                                       /* RXD0   */
   .rxd1   = &IfxGeth_RXD1A_P11_9_IN,                                        /* RXD1   */
   .mdc    = &IfxGeth_MDC_P21_2_OUT,                                         /* MDC    */
   .mdio   = &IfxGeth_MDIO_P21_3_INOUT,                                      /* MDIO   */
   .txd0   = &IfxGeth_TXD0_P11_3_OUT,                                        /* TXD0   */
   .txd1   = &IfxGeth_TXD1_P11_2_OUT,                                        /* TXD1   */
   .txEn   = &IfxGeth_TXEN_P11_6_OUT                                         /* TXEN   */
};


/****************************************************************************************
* Function prototypes
****************************************************************************************/
static void netdev_link_refresh(void);


/****************************************************************************************
*            N E T W O R K   D E V I C E   R O U T I N E S
****************************************************************************************/
/************************************************************************************//**
** \brief     Initializes the network device.
**
****************************************************************************************/
void netdev_init(void)
{
  IfxGeth_Eth_Config GethConfig;

  /* Reset the link status flag. */
  linkUpFlag = 0;

  /* Store the default MAC address. */
  macAddress.addr[0] = NETDEV_DEFAULT_MACADDR0;
  macAddress.addr[1] = NETDEV_DEFAULT_MACADDR1;
  macAddress.addr[2] = NETDEV_DEFAULT_MACADDR2;
  macAddress.addr[3] = NETDEV_DEFAULT_MACADDR3;
  macAddress.addr[4] = NETDEV_DEFAULT_MACADDR4;
  macAddress.addr[5] = NETDEV_DEFAULT_MACADDR5;
  
  /* Initialize the config structure with default values. */
  IfxGeth_Eth_initModuleConfig(&GethConfig, &MODULE_GETH);
  /* Store information about the DP83825I PHY. */
  GethConfig.phyInterfaceMode = IfxGeth_PhyInterfaceMode_rmii;
  GethConfig.pins.rmiiPins = &rmii_pins;
  GethConfig.mac.lineSpeed = IfxGeth_LineSpeed_100Mbps;
  /* MAC core configuration. */
  GethConfig.mac.loopbackMode = IfxGeth_LoopbackMode_disable;
  GethConfig.mac.macAddress[0] = macAddress.addr[0];
  GethConfig.mac.macAddress[1] = macAddress.addr[1];
  GethConfig.mac.macAddress[2] = macAddress.addr[2];
  GethConfig.mac.macAddress[3] = macAddress.addr[3];
  GethConfig.mac.macAddress[4] = macAddress.addr[4];
  GethConfig.mac.macAddress[5] = macAddress.addr[5];
  /* MTL configuration. */
  GethConfig.mtl.numOfTxQueues = 1;
  GethConfig.mtl.numOfRxQueues = 1;
  GethConfig.mtl.txQueue[0].txQueueSize = IfxGeth_QueueSize_2560Bytes;
  GethConfig.mtl.txQueue[0].storeAndForward = TRUE;
  GethConfig.mtl.rxQueue[0].rxQueueSize = IfxGeth_QueueSize_2560Bytes;
  GethConfig.mtl.rxQueue[0].rxDmaChannelMap = IfxGeth_RxDmaChannel_0;
  GethConfig.mtl.rxQueue[0].storeAndForward = TRUE;
  /* DMA configuration. */
  GethConfig.dma.numOfTxChannels = 1;
  GethConfig.dma.numOfRxChannels = 1;
  /* DMA Tx configuration. */
  GethConfig.dma.txChannel[0].channelId = IfxGeth_TxDmaChannel_0;
  GethConfig.dma.txChannel[0].txDescrList = (IfxGeth_TxDescrList *)&IfxGeth_Eth_txDescrList[0];
  GethConfig.dma.txChannel[0].txBuffer1StartAddress = (uint32 *)&channel0TxBuffer1[0][0];
  GethConfig.dma.txChannel[0].txBuffer1Size = IFXGETH_MAX_TX_BUFFER_SIZE;
  /* DMA Rx configuration. */
  GethConfig.dma.rxChannel[0].channelId = IfxGeth_RxDmaChannel_0;
  GethConfig.dma.rxChannel[0].rxDescrList = (IfxGeth_RxDescrList *)&IfxGeth_Eth_rxDescrList[0];
  GethConfig.dma.rxChannel[0].rxBuffer1StartAddress = (uint32 *)&channel0RxBuffer1[0][0];
  GethConfig.dma.rxChannel[0].rxBuffer1Size = IFXGETH_MAX_RX_BUFFER_SIZE;
  /* DMA event signaling. Note that the priority is set to 0, which bypassed the
   * interrupt from actually being enabled. This network interface works in polling
   * mode.
   */
  GethConfig.dma.txInterrupt[0].channelId = IfxGeth_DmaChannel_0;
  GethConfig.dma.txInterrupt[0].priority = 0;
  GethConfig.dma.txInterrupt[0].provider = IfxSrc_Tos_cpu0;
  GethConfig.dma.rxInterrupt[0].channelId = IfxGeth_DmaChannel_0;
  GethConfig.dma.rxInterrupt[0].priority = 0;
  GethConfig.dma.rxInterrupt[0].provider = IfxSrc_Tos_cpu0;
  /* Initialize the module. */
  IfxGeth_Eth_initModule(&g_IfxGeth, &GethConfig);

  /* Get the ID of Ethernet PHY do determine the board version, also needed for SCR. */
  IfxPort_setPinModeOutput(IfxGeth_MDC_P21_2_OUT.pin.port,
                           IfxGeth_MDC_P21_2_OUT.pin.pinIndex,
                           IfxPort_OutputMode_pushPull, IfxGeth_MDC_P21_2_OUT.select);
  GETH_GPCTL.B.ALTI0  = IfxGeth_MDIO_P21_3_INOUT.inSelect;

  /* Initialize the PHY. */
  IfxGeth_Eth_Phy_Dp83825i_init();

  /* Enable Ethernet transmitter and receiver. */
  IfxGeth_Eth_startTransmitters(&g_IfxGeth, 1);
  IfxGeth_Eth_startReceivers(&g_IfxGeth, 1);
  
  /* Refresh the link status now that the Ethernet module is up and running. */
  netdev_link_refresh();
} /*** end of netdev_init ***/


/************************************************************************************//**
** \brief     Initializes the MAC address of the network device.
**
****************************************************************************************/
void netdev_init_mac(void)
{
  /* Configure the MAC address */
  uip_setethaddr(macAddress);
} /*** end of netdev_init_mac ***/


/************************************************************************************//**
** \brief     Obtains the MAC address of the network device.
** \param     mac_addr MAC address storage buffer as a byte array.
**
****************************************************************************************/
void netdev_get_mac(unsigned char * mac_addr)
{
  mac_addr[0] = macAddress.addr[0];
  mac_addr[1] = macAddress.addr[1];
  mac_addr[2] = macAddress.addr[2];
  mac_addr[3] = macAddress.addr[3];
  mac_addr[4] = macAddress.addr[4];
  mac_addr[5] = macAddress.addr[5];
} /*** end of netdev_get_mac ***/


/************************************************************************************//**
** \brief     Read newly received data from the network device.
** \details   Copy the newly received data to the uip_buf byte buffer. Keep in mind its
**            maximum size (UIP_CONF_BUFFER_SIZE).
** \return    Number of newly received bytes that were copied to uip_buf or 0 if no new
**            data was received.
**
****************************************************************************************/
unsigned int netdev_read(void)
{
  unsigned int               result = 0;
  IfxGeth_RxDescr volatile * rxDescriptor;
  uint16                     frameLen;
  uint8                    * rxData;

  /* Note this this function is called continuously to poll for new data. A good place
   * to also refresh the link status.
   */
  netdev_link_refresh();

  /* Only continue if the link is actually up. */
  if (linkUpFlag != 0)
  {
    /* Only continue if new data was received. */
    if (IfxGeth_Eth_isRxDataAvailable(&g_IfxGeth, IfxGeth_RxDmaChannel_0) != FALSE)
    {
      /* Get pointer to its descriptor. */
      rxDescriptor = IfxGeth_Eth_getActualRxDescriptor(&g_IfxGeth, IfxGeth_RxDmaChannel_0);
      /* Only continue with a valid descriptor. */
      if (rxDescriptor != NULL)
      {
        /* Determine the size of the newly received frame. */
        uint32 rdes3 = rxDescriptor->RDES3.U;
        uint32 rdes1 = rxDescriptor->RDES1.U;

        if (((rdes3 & (1UL << 15)) != 0U) ||
            ((rdes1 & (1UL << 7)) != 0U) ||
            ((rdes3 & (1UL << 28)) == 0U))
        {
          /* Error, this block is invalid. */
          frameLen = 0;
        }
        else
        {
          /* Subtract CRC. */
          frameLen = (uint16)((rdes3 & 0x7FFF) - 4U);
        }
        /* Only continue with a valid frame length. */
        if (frameLen > 0)
        {
          /* Obtain pointer to the received data. */
          rxData = IfxGeth_Eth_getReceiveBuffer(&g_IfxGeth, IfxGeth_RxDmaChannel_0);
          /* Only continue with a valid data pointer. */
          if (rxData != NULL)
          {
            /* Copy the received data ato the uIP data buffer. */
            memcpy(uip_buf, rxData, frameLen);
            /* Update the result. */
            result = frameLen;
          }
        }
      }
      /* Free the receive buffer, enabling it for the further reception. */
      IfxGeth_Eth_freeReceiveBuffer(&g_IfxGeth, IfxGeth_RxDmaChannel_0);
    }
  }

  /* Give the result back to the caller. */
  return result;
} /*** end of netdev_read ***/


/************************************************************************************//**
** \brief     Send data on the network device.
** \details   The data to send is available in the uip_buf byte buffer and the number of
**            bytes to send is stored in uip_len.
**
****************************************************************************************/
void netdev_send(void)
{
  uint8                    * txData = NULL;
  uint32                     timeout;
  IfxGeth_TxDescr volatile * txDescriptor;

  /* Only continue if the link is actually up. */
  if (linkUpFlag != 0)
  {
    /* Set timeout time to wait for a free transmit buffer to become available. */
    timeout = TimerGet() + NETDEV_TX_BUFF_ALLOC_TIMEOUT_MS;
    /* Wait for a free transmit buffer to become available. */
    while (txData == NULL)
    {
      /* Attempt to get a free transmit buffer. */
      txData = IfxGeth_Eth_getTransmitBuffer(&g_IfxGeth, IfxGeth_TxDmaChannel_0);
      /* Keep the watchdog happy */
      CopService();
      /* Break loop upon timeout. This would indicate a hardware failure. */
      if (TimerGet() > timeout)
      {
        break;
      }
    }

    /* Only continue with a valid transmit buffer. */
    if (txData != NULL)
    {
      /* Copy the packet data to the tx data buffer. */
      memcpy(txData, uip_buf, uip_len);
      /* Make sure the buffer length is set to the max. available, just in case a
       * previously packet overwrote this.
       */
      txDescriptor = IfxGeth_Eth_getActualTxDescriptor(&g_IfxGeth, IfxGeth_TxDmaChannel_0);
      txDescriptor->TDES2.R.B1L = IFXGETH_MAX_TX_BUFFER_SIZE;
      /* Clear the TX interrupt status flag. */
      IfxGeth_dma_clearInterruptFlag(g_IfxGeth.gethSFR, IfxGeth_DmaChannel_0,
                                     IfxGeth_DmaInterruptFlag_transmitInterrupt) ;
      /* Submit the buffer transmit request. */
      IfxGeth_Eth_sendTransmitBuffer(&g_IfxGeth, uip_len, IfxGeth_TxDmaChannel_0);
      /* Set timeout time to wait for transmit completion. */
      timeout = TimerGet() + NETDEV_TX_PACKET_TIMEOUT_MS;
      /* Wait for the transmit operation to complete. */
      while (IfxGeth_dma_isInterruptFlagSet(g_IfxGeth.gethSFR, IfxGeth_DmaChannel_0,
                                      IfxGeth_DmaInterruptFlag_transmitInterrupt) == FALSE)
      {
        /* Keep the watchdog happy */
        CopService();
        /* Break loop upon timeout. This would indicate a hardware failure. */
        if (TimerGet() > timeout)
        {
          break;
        }
      }
    }
  }
} /*** end of netdev_send ***/


/************************************************************************************//**
** \brief     Utility function that should be called continuously to monitor the link
**            status.
**
****************************************************************************************/
static void netdev_link_refresh(void)
{
  static unsigned long              nextRefreshEvent = 0;
  unsigned long                     currentTime;
  IfxGeth_LineSpeed                 LineSpeed;
  IfxGeth_DuplexMode                DuplexMode;
  Ifx_GETH_MAC_PHYIF_CONTROL_STATUS CtrlStatus;
  unsigned char                     linkUpFlagPrevious;

  /* Get the current time. */
  currentTime = TimerGet();
  /* Check for refresh event. */
  if (currentTime >= nextRefreshEvent)
  {
    /* Schedule the next refresh event 100 milliseconds from now. */
    nextRefreshEvent = currentTime + 100;
    /* Store last known link status. */
    linkUpFlagPrevious = linkUpFlag;
    /* Obtain the current link state from the PHY. */
    CtrlStatus.U = IfxGeth_Eth_Phy_Dp83825i_link_status();
    /* Update the link status flag. */
    linkUpFlag = (CtrlStatus.B.LNKSTS == 1) ? 1 : 0;
    /* Did the link change from down to up? */
    if ( (linkUpFlag == 1) && (linkUpFlag != linkUpFlagPrevious) )
    {
      /* Set safe default for the line speed and duplex mode. */
      DuplexMode = IfxGeth_DuplexMode_halfDuplex;
      LineSpeed = IfxGeth_LineSpeed_10Mbps;

      /* Update duplex mode. */
      if (CtrlStatus.B.LNKMOD == 1)
      {
        DuplexMode = IfxGeth_DuplexMode_fullDuplex;
      }
      IfxGeth_mac_setDuplexMode(g_IfxGeth.gethSFR, DuplexMode);

      /* Update line speed. */
      if (CtrlStatus.B.LNKSPEED == 1)
      {
        LineSpeed = IfxGeth_LineSpeed_100Mbps;
      }
      else if (CtrlStatus.B.LNKSPEED > 1)
      {
        LineSpeed = IfxGeth_LineSpeed_1000Mbps;
      }
      IfxGeth_mac_setLineSpeed(g_IfxGeth.gethSFR, LineSpeed);
    }
  }
} /*** end of netdev_link_refresh ***/


/*********************************** end of netdev.c ***********************************/
