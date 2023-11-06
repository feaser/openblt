/*
 * Copyright (c) 2001, Swedish Institute of Computer Science.
 * All rights reserved.
 *
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
 * $Id: netdev.c,v 1.8 2006/06/07 08:39:58 adam Exp $
 */


/*---------------------------------------------------------------------------*/
#include <string.h>
#include "uip.h"
#include "uip_arp.h"
#include "boot.h"
#include "stm32f7xx.h"                           /* STM32 CPU and HAL header           */


/*---------------------------------------------------------------------------*/
#define NETDEV_DEFAULT_MACADDR0           (0x08)
#define NETDEV_DEFAULT_MACADDR1           (0x00)
#define NETDEV_DEFAULT_MACADDR2           (0x27)
#define NETDEV_DEFAULT_MACADDR3           (0x69)
#define NETDEV_DEFAULT_MACADDR4           (0x5B)
#define NETDEV_DEFAULT_MACADDR5           (0x45)

/* Timeout time for transmitting a packet. */
#define NETDEV_TX_PACKET_TIMEOUT_MS       (250u)


/*---------------------------------------------------------------------------*/
ETH_HandleTypeDef heth;


#if defined ( __ICCARM__ ) /*!< IAR Compiler */
  #pragma data_alignment=4
#endif
__ALIGN_BEGIN ETH_DMADescTypeDef  DMARxDscrTab[ETH_RXBUFNB] __ALIGN_END;/* Ethernet Rx MA Descriptor */

#if defined ( __ICCARM__ ) /*!< IAR Compiler */
  #pragma data_alignment=4
#endif
__ALIGN_BEGIN ETH_DMADescTypeDef  DMATxDscrTab[ETH_TXBUFNB] __ALIGN_END;/* Ethernet Tx DMA Descriptor */

#if defined ( __ICCARM__ ) /*!< IAR Compiler */
  #pragma data_alignment=4
#endif
__ALIGN_BEGIN uint8_t Rx_Buff[ETH_RXBUFNB][ETH_RX_BUF_SIZE] __ALIGN_END; /* Ethernet Receive Buffer */

#if defined ( __ICCARM__ ) /*!< IAR Compiler */
  #pragma data_alignment=4
#endif
__ALIGN_BEGIN uint8_t Tx_Buff[ETH_TXBUFNB][ETH_TX_BUF_SIZE] __ALIGN_END; /* Ethernet Transmit Buffer */


/*---------------------------------------------------------------------------*/
static struct uip_eth_addr macAddress;


/*---------------------------------------------------------------------------*/
void HAL_ETH_MspInit(ETH_HandleTypeDef* heth)
{
  GPIO_InitTypeDef GPIO_InitStruct;

  if (heth->Instance == ETH)
  {
    /* Ethernet clock enable */
    __HAL_RCC_ETH_CLK_ENABLE();

    /* ETH GPIO Configuration:
     * PC1      ------> ETH_MDC
     * PA1      ------> ETH_REF_CLK
     * PA2      ------> ETH_MDIO
     * PA7      ------> ETH_CRS_DV
     * PC4      ------> ETH_RXD0
     * PC5      ------> ETH_RXD1
     * PB13     ------> ETH_TXD1
     * PG11     ------> ETH_TX_EN
     * PG13     ------> ETH_TXD0
     */
    GPIO_InitStruct.Pin = GPIO_PIN_1 | GPIO_PIN_4 | GPIO_PIN_5;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF11_ETH;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF11_ETH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_13;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF11_ETH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_11 | GPIO_PIN_13;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF11_ETH;
    HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);
  }
}


/*---------------------------------------------------------------------------*/
void HAL_ETH_MspDeInit(ETH_HandleTypeDef* heth)
{
  if (heth->Instance == ETH)
  {
    /* Ethernet clock disable */
    __HAL_RCC_ETH_CLK_DISABLE();

    /* Reset Ethernet GPIO pin configuration. */
    HAL_GPIO_DeInit(GPIOC, GPIO_PIN_1 | GPIO_PIN_4 | GPIO_PIN_5);
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_7);
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_13);
    HAL_GPIO_DeInit(GPIOG, GPIO_PIN_11 | GPIO_PIN_13);
  }
}


/*---------------------------------------------------------------------------*/
void netdev_init(void)
{
  /* Store the default MAC address. */
  macAddress.addr[0] = NETDEV_DEFAULT_MACADDR0;
  macAddress.addr[1] = NETDEV_DEFAULT_MACADDR1;
  macAddress.addr[2] = NETDEV_DEFAULT_MACADDR2;
  macAddress.addr[3] = NETDEV_DEFAULT_MACADDR3;
  macAddress.addr[4] = NETDEV_DEFAULT_MACADDR4;
  macAddress.addr[5] = NETDEV_DEFAULT_MACADDR5;

  /* Initialize Ethernet. */
  heth.Instance = ETH;
  heth.Init.AutoNegotiation = ETH_AUTONEGOTIATION_ENABLE;
  heth.Init.PhyAddress = LAN8742A_PHY_ADDRESS;
  heth.Init.MACAddr = &(macAddress.addr)[0];
  heth.Init.RxMode = ETH_RXPOLLING_MODE;
  heth.Init.ChecksumMode = ETH_CHECKSUM_BY_HARDWARE;
  heth.Init.MediaInterface = ETH_MEDIA_INTERFACE_RMII;
  (void)HAL_ETH_Init(&heth);

  /* Initialize Tx Descriptors list: Chain Mode */
  HAL_ETH_DMATxDescListInit(&heth, DMATxDscrTab, &Tx_Buff[0][0], ETH_TXBUFNB);
  /* Initialize Rx Descriptors list: Chain Mode  */
  HAL_ETH_DMARxDescListInit(&heth, DMARxDscrTab, &Rx_Buff[0][0], ETH_RXBUFNB);
  /* Enable MAC and DMA transmission and reception */
  HAL_ETH_Start(&heth);
}


/*---------------------------------------------------------------------------*/
void netdev_init_mac(void)
{
  /* Configure the MAC address */
  uip_setethaddr(macAddress);
}


/*---------------------------------------------------------------------------*/
void netdev_get_mac(unsigned char * mac_addr)
{
  mac_addr[0] = macAddress.addr[0];
  mac_addr[1] = macAddress.addr[1];
  mac_addr[2] = macAddress.addr[2];
  mac_addr[3] = macAddress.addr[3];
  mac_addr[4] = macAddress.addr[4];
  mac_addr[5] = macAddress.addr[5];
}


/*---------------------------------------------------------------------------*/
unsigned int netdev_read(void)
{
  unsigned int result = 0;
  uint16_t  len = 0;
  uint8_t * buffer;
  __IO ETH_DMADescTypeDef *dmarxdesc;

  /* Check if a new frame was received. */
  if (HAL_ETH_GetReceivedFrame(&heth) == HAL_OK)
  {
    /* Obtain the size of the packet and a pointer to the buffer with packet data. */
    len = heth.RxFrameInfos.length;
    buffer = (uint8_t *)heth.RxFrameInfos.buffer;
    /* Copy the received packet data into the uip buffer. */
    memcpy(uip_buf, buffer, len);
    /* Obtain pointer to the reception DMA descriptor. */
    dmarxdesc = heth.RxFrameInfos.FSRxDesc;
    /* Give the buffer back to DMA by setting the Own-bit in the reception descriptor. */
    dmarxdesc->Status |= ETH_DMARXDESC_OWN;
    /* Clear the segment count. */
    heth.RxFrameInfos.SegCount =0;
    /* When reception buffer unavailable flag is set, clear it and resume reception. */
    if ((heth.Instance->DMASR & ETH_DMASR_RBUS) != (uint32_t)RESET)
    {
      /* Clear RBUS ETHERNET DMA flag */
      heth.Instance->DMASR = ETH_DMASR_RBUS;
      /* Resume DMA reception */
      heth.Instance->DMARPDR = 0;
    }
    /* Update the result. */
    result = len;
  }
  /* Give the result back to the caller. */
  return result;
}


/*---------------------------------------------------------------------------*/
void netdev_send(void)
{
  uint8_t * buffer;
  __IO ETH_DMADescTypeDef *DmaTxDesc;
  uint32_t framelength;
  uint32_t timeout;
  ErrorStatus errorStatus = SUCCESS;

  /* Obtain pointer to the transmission DMA descriptor. */
  DmaTxDesc = heth.TxDesc;
  /* Set timeout time to wait for the DMA buffer to become available. */
  timeout = TimerGet() + NETDEV_TX_PACKET_TIMEOUT_MS;
  /* Only continue with packet transmission of the buffer is available. */
  while ((DmaTxDesc->Status & ETH_DMATXDESC_OWN) != (uint32_t)RESET)
  {
    CopService();
    /* Break loop upon timeout. This would indicate a hardware failure. */
    if (TimerGet() > timeout)
    {
      /* Update the error status. */
      errorStatus = ERROR;
      break;
    }
  }
  /* Only continue with transmission if not error was detected. */
  if (errorStatus == SUCCESS)
  {
    /* Store the framelength. */
    framelength = uip_len;
    /* Obtain pointer to write the packet data to. */
    buffer = (uint8_t *)(heth.TxDesc->Buffer1Addr);
    /* Copy the packet data to the buffer. */
    memcpy(buffer, uip_buf, framelength);
    /* Prepare transmit descriptors to give to DMA. */
    HAL_ETH_TransmitFrame(&heth, framelength);
  }

  /* When Transmit Underflow flag is set, clear it and issue a Transmit Poll Demand to
   * resume transmission.
   */
  if ((heth.Instance->DMASR & ETH_DMASR_TUS) != (uint32_t)RESET)
  {
    /* Clear TUS ETHERNET DMA flag. */
    heth.Instance->DMASR = ETH_DMASR_TUS;
    /* Resume DMA transmission. */
    heth.Instance->DMATPDR = 0;
  }
}


