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
#include "stm32h7xx.h"                           /* STM32 CPU and HAL        */
#include "lan8742.h"                             /* LAN8742 PHY driver       */


/*---------------------------------------------------------------------------*/
#define NETDEV_DEFAULT_MACADDR0           (0x08)
#define NETDEV_DEFAULT_MACADDR1           (0x00)
#define NETDEV_DEFAULT_MACADDR2           (0x27)
#define NETDEV_DEFAULT_MACADDR3           (0x69)
#define NETDEV_DEFAULT_MACADDR4           (0x5B)
#define NETDEV_DEFAULT_MACADDR5           (0x45)

/* Timeout time for transmitting a packet. */
#define NETDEV_TX_PACKET_TIMEOUT_MS       (250u)

/* Lenght of the reception and transmit buffers. */
#define ETH_RX_BUFFER_SIZE                (ETH_MAX_PACKET_SIZE)
#define ETH_TX_BUFFER_SIZE                (ETH_MAX_PACKET_SIZE)


/*---------------------------------------------------------------------------*/
static int32_t ETH_PHY_IO_Init(void);
static int32_t ETH_PHY_IO_DeInit (void);
static int32_t ETH_PHY_IO_ReadReg(uint32_t DevAddr, uint32_t RegAddr, uint32_t *pRegVal);
static int32_t ETH_PHY_IO_WriteReg(uint32_t DevAddr, uint32_t RegAddr, uint32_t RegVal);
static int32_t ETH_PHY_IO_GetTick(void);


/*---------------------------------------------------------------------------*/
#if defined ( __ICCARM__ ) /*!< IAR Compiler */

#pragma location=0x30040000
ETH_DMADescTypeDef  DMARxDscrTab[ETH_RX_DESC_CNT]; /* Ethernet Rx DMA Descriptors */
#pragma location=0x30040060
ETH_DMADescTypeDef  DMATxDscrTab[ETH_TX_DESC_CNT]; /* Ethernet Tx DMA Descriptors */
#pragma location=0x30040200
uint8_t Rx_Buff[ETH_RX_DESC_CNT][ETH_RX_BUFFER_SIZE]; /* Ethernet Receive Buffers */

#elif defined ( __CC_ARM )  /* MDK ARM Compiler */

__attribute__((at(0x30040000))) ETH_DMADescTypeDef  DMARxDscrTab[ETH_RX_DESC_CNT]; /* Ethernet Rx DMA Descriptors */
__attribute__((at(0x30040060))) ETH_DMADescTypeDef  DMATxDscrTab[ETH_TX_DESC_CNT]; /* Ethernet Tx DMA Descriptors */
__attribute__((at(0x30040200))) uint8_t Rx_Buff[ETH_RX_DESC_CNT][ETH_RX_BUFFER_SIZE]; /* Ethernet Receive Buffer */

#elif defined ( __GNUC__ ) /* GNU Compiler */

ETH_DMADescTypeDef DMARxDscrTab[ETH_RX_DESC_CNT] __attribute__((section(".RxDecripSection"))); /* Ethernet Rx DMA Descriptors */
ETH_DMADescTypeDef DMATxDscrTab[ETH_TX_DESC_CNT] __attribute__((section(".TxDecripSection")));   /* Ethernet Tx DMA Descriptors */
uint8_t Rx_Buff[ETH_RX_DESC_CNT][ETH_RX_BUFFER_SIZE] __attribute__((section(".RxArraySection"))); /* Ethernet Receive Buffers */
uint8_t Tx_Buff[ETH_TX_DESC_CNT][ETH_TX_BUFFER_SIZE] __attribute__((section(".TxArraySection"))); /* Ethernet Transmit Buffers */

#endif


/*---------------------------------------------------------------------------*/
ETH_HandleTypeDef heth;
ETH_TxPacketConfig TxConfig;

lan8742_Object_t LAN8742;
lan8742_IOCtx_t  LAN8742_IOCtx =
{
  ETH_PHY_IO_Init,
  ETH_PHY_IO_DeInit,
  ETH_PHY_IO_WriteReg,
  ETH_PHY_IO_ReadReg,
  ETH_PHY_IO_GetTick
};


/*---------------------------------------------------------------------------*/
static struct uip_eth_addr macAddress;


/*---------------------------------------------------------------------------*/
void HAL_ETH_MspInit(ETH_HandleTypeDef* ethHandle)
{
  GPIO_InitTypeDef GPIO_InitStruct = { 0 };

  if (ethHandle->Instance == ETH)
  {
    /* Enable Peripheral clocks. */
    __HAL_RCC_ETH1MAC_CLK_ENABLE();
    __HAL_RCC_ETH1TX_CLK_ENABLE();
    __HAL_RCC_ETH1RX_CLK_ENABLE();

    /* ETH GPIO Configuration:
     * PC1     ------> ETH_MDC
     * PA1     ------> ETH_REF_CLK
     * PA2     ------> ETH_MDIO
     * PA7     ------> ETH_CRS_DV
     * PC4     ------> ETH_RXD0
     * PC5     ------> ETH_RXD1
     * PB13     ------> ETH_TXD1
     * PG11     ------> ETH_TX_EN
     * PG13     ------> ETH_TXD0
     */
    GPIO_InitStruct.Pin = GPIO_PIN_1|GPIO_PIN_4|GPIO_PIN_5;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF11_ETH;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF11_ETH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_13;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF11_ETH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_11|GPIO_PIN_13;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF11_ETH;
    HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);
  }
}


/*---------------------------------------------------------------------------*/
void HAL_ETH_MspDeInit(ETH_HandleTypeDef* ethHandle)
{
  if (ethHandle->Instance == ETH)
  {
    /* Disable Peripheral clocks. */
    __HAL_RCC_ETH1MAC_CLK_DISABLE();
    __HAL_RCC_ETH1TX_CLK_DISABLE();
    __HAL_RCC_ETH1RX_CLK_DISABLE();

    /* Reset Ethernet GPIO pin configuration. */
    HAL_GPIO_DeInit(GPIOC, GPIO_PIN_1|GPIO_PIN_4|GPIO_PIN_5);
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_7);
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_13);
    HAL_GPIO_DeInit(GPIOG, GPIO_PIN_11|GPIO_PIN_13);
  }
}


/*---------------------------------------------------------------------------*/
void netdev_init(void)
{
  uint32_t PHYLinkState;
  ETH_MACConfigTypeDef MACConf;
  uint32_t speed = 0;
  uint32_t duplex =0;
  uint32_t idx = 0;

  /* Store the default MAC address. */
  macAddress.addr[0] = NETDEV_DEFAULT_MACADDR0;
  macAddress.addr[1] = NETDEV_DEFAULT_MACADDR1;
  macAddress.addr[2] = NETDEV_DEFAULT_MACADDR2;
  macAddress.addr[3] = NETDEV_DEFAULT_MACADDR3;
  macAddress.addr[4] = NETDEV_DEFAULT_MACADDR4;
  macAddress.addr[5] = NETDEV_DEFAULT_MACADDR5;

  /* Initialize Ethernet. */
  heth.Instance = ETH;
  heth.Init.MACAddr = &(macAddress.addr)[0];
  heth.Init.MediaInterface = HAL_ETH_RMII_MODE;
  heth.Init.TxDesc = DMATxDscrTab;
  heth.Init.RxDesc = DMARxDscrTab;
  heth.Init.RxBuffLen = 1524;
  (void)HAL_ETH_Init(&heth);

  /* Configure the transmit packet. */
  memset(&TxConfig, 0 , sizeof(ETH_TxPacketConfig));
  TxConfig.Attributes = ETH_TX_PACKETS_FEATURES_CSUM | ETH_TX_PACKETS_FEATURES_CRCPAD;
  TxConfig.ChecksumCtrl = ETH_CHECKSUM_IPHDR_PAYLOAD_INSERT_PHDR_CALC;
  TxConfig.CRCPadCtrl = ETH_CRC_PAD_INSERT;

  /* TODO ##Vg Port. */
  //LWIP_MEMPOOL_INIT(RX_POOL);

  /* Assign memory buffers to a DMA Rx descriptor. */
  for(idx = 0; idx < ETH_RX_DESC_CNT; idx ++)
  {
    HAL_ETH_DescAssignMemory(&heth, idx, Rx_Buff[idx], NULL);
  }

  /* Set PHY IO functions. */
  LAN8742_RegisterBusIO(&LAN8742, &LAN8742_IOCtx);

  /* Initialize the LAN8742 ETH PHY */
  LAN8742_Init(&LAN8742);

  /* Set the state of the PHY. */
  PHYLinkState = LAN8742_GetLinkState(&LAN8742);

  /* Determine duplex mode and speed. */
  switch (PHYLinkState)
  {
    case LAN8742_STATUS_100MBITS_FULLDUPLEX:
      duplex = ETH_FULLDUPLEX_MODE;
      speed = ETH_SPEED_100M;
      break;
    case LAN8742_STATUS_100MBITS_HALFDUPLEX:
      duplex = ETH_HALFDUPLEX_MODE;
      speed = ETH_SPEED_100M;
      break;
    case LAN8742_STATUS_10MBITS_FULLDUPLEX:
      duplex = ETH_FULLDUPLEX_MODE;
      speed = ETH_SPEED_10M;
      break;
    case LAN8742_STATUS_10MBITS_HALFDUPLEX:
      duplex = ETH_HALFDUPLEX_MODE;
      speed = ETH_SPEED_10M;
      break;
    default:
      duplex = ETH_FULLDUPLEX_MODE;
      speed = ETH_SPEED_10M;
      break;
  }

  /* Update MAC configuration. */
  HAL_ETH_GetMACConfig(&heth, &MACConf);
  MACConf.DuplexMode = duplex;
  MACConf.Speed = speed;
  HAL_ETH_SetMACConfig(&heth, &MACConf);

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

  /* TODO ##Vg Check if a new frame was received and read it. */

  /* Give the result back to the caller. */
  return result;
}


/*---------------------------------------------------------------------------*/
void netdev_send(void)
{
  uint8_t * buffer;
  uint32_t framelength;
  ETH_BufferTypeDef txbuffer = { 0 };

  /* Store the framelength. */
  framelength = uip_len;
  /* Obtain pointer to write the packet data to. */
  /* TODO ##Vg Not sure if this is correct since I added Tx_Buff myself. */
  buffer = (uint8_t *)(&Tx_Buff[0][0]);
  /* Copy the packet data to the buffer. */
  memcpy(buffer, uip_buf, framelength);
  /* Configure the ETH trasnmit buffer. */
  txbuffer.buffer = buffer;
  txbuffer.len = framelength;
  /* Set the transmit configuration. */
  TxConfig.Length = framelength;
  TxConfig.TxBuffer = &txbuffer;
  /* Submit the packet for transmission. */
  HAL_ETH_Transmit(&heth, &TxConfig, NETDEV_TX_PACKET_TIMEOUT_MS);
  /* TODO ##Vg CONTNUE HERE: Test if packet tx works. DHCP should send a packet. */
}


/*---------------------------------------------------------------------------*/
/**
  * @brief  Initializes the MDIO interface GPIO and clocks.
  * @param  None
  * @retval 0 if OK, -1 if ERROR
  */
static int32_t ETH_PHY_IO_Init(void)
{
  /* We assume that MDIO GPIO configuration is already done
     in the ETH_MspInit() else it should be done here
  */

  /* Configure the MDIO Clock */
  HAL_ETH_SetMDIOClockRange(&heth);

  return 0;
}


/*---------------------------------------------------------------------------*/
/**
  * @brief  De-Initializes the MDIO interface .
  * @param  None
  * @retval 0 if OK, -1 if ERROR
  */
static int32_t ETH_PHY_IO_DeInit (void)
{
  return 0;
}


/*---------------------------------------------------------------------------*/
/**
  * @brief  Read a PHY register through the MDIO interface.
  * @param  DevAddr: PHY port address
  * @param  RegAddr: PHY register address
  * @param  pRegVal: pointer to hold the register value
  * @retval 0 if OK -1 if Error
  */
static int32_t ETH_PHY_IO_ReadReg(uint32_t DevAddr, uint32_t RegAddr, uint32_t *pRegVal)
{
  if(HAL_ETH_ReadPHYRegister(&heth, DevAddr, RegAddr, pRegVal) != HAL_OK)
  {
    return -1;
  }

  return 0;
}


/*---------------------------------------------------------------------------*/
/**
  * @brief  Write a value to a PHY register through the MDIO interface.
  * @param  DevAddr: PHY port address
  * @param  RegAddr: PHY register address
  * @param  RegVal: Value to be written
  * @retval 0 if OK -1 if Error
  */
static int32_t ETH_PHY_IO_WriteReg(uint32_t DevAddr, uint32_t RegAddr, uint32_t RegVal)
{
  if(HAL_ETH_WritePHYRegister(&heth, DevAddr, RegAddr, RegVal) != HAL_OK)
  {
    return -1;
  }

  return 0;
}


/*---------------------------------------------------------------------------*/
/**
  * @brief  Get the time in millisecons used for internal PHY driver process.
  * @retval Time value
  */
static int32_t ETH_PHY_IO_GetTick(void)
{
  return TimerGet();
}
