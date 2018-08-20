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
#include "header.h"                                    /* generic header               */
#include "uip.h"
#include "uip_arp.h"
#include "xmc_gpio.h"
#include "xmc_eth_mac.h"
#include "xmc_eth_phy.h"
#include <string.h>


/*---------------------------------------------------------------------------*/
#define NETDEV_LINKUP_TIMEOUT_MS       (5000)

#define NETDEV_DEFAULT_MACADDR0        (0x08)
#define NETDEV_DEFAULT_MACADDR1        (0x00)
#define NETDEV_DEFAULT_MACADDR2        (0x27)
#define NETDEV_DEFAULT_MACADDR3        (0x69)
#define NETDEV_DEFAULT_MACADDR4        (0x5B)
#define NETDEV_DEFAULT_MACADDR5        (0x45)

#define NETDEV_DEFAULT_MACADDR         ((uint64_t)NETDEV_DEFAULT_MACADDR0 | \
                                       ((uint64_t)NETDEV_DEFAULT_MACADDR1 << 8) | \
                                       ((uint64_t)NETDEV_DEFAULT_MACADDR2 << 16) | \
                                       ((uint64_t)NETDEV_DEFAULT_MACADDR3 << 24) | \
                                       ((uint64_t)NETDEV_DEFAULT_MACADDR4 << 32) | \
                                       ((uint64_t)NETDEV_DEFAULT_MACADDR5 << 40))

#define ETH_UIP_0_NUM_RX_BUF           (4U)
#define ETH_UIP_0_NUM_TX_BUF           (4U)

#define ETH_UIP_0_CRS_DV               XMC_GPIO_PORT15, 9U
#define ETH_UIP_0_RXER                 XMC_GPIO_PORT2, 4U
#define ETH_UIP_0_RXD0                 XMC_GPIO_PORT2, 2U
#define ETH_UIP_0_RXD1                 XMC_GPIO_PORT2, 3U
#define ETH_UIP_0_TXEN                 XMC_GPIO_PORT2, 5U
#define ETH_UIP_0_TXD0                 XMC_GPIO_PORT2, 8U
#define ETH_UIP_0_TXD1                 XMC_GPIO_PORT2, 9U
#define ETH_UIP_0_RMII_CLK             XMC_GPIO_PORT15, 8U
#define ETH_UIP_0_MDC                  XMC_GPIO_PORT2, 7U
#define ETH_UIP_0_MDIO                 XMC_GPIO_PORT2, 0U
#define ETH_UIP_0_PIN_LIST_SIZE        (10U)

#define ETH_UIP_0_PHY_ADDR             (0)


/*---------------------------------------------------------------------------*/
#if defined(__ICCARM__)
#pragma data_alignment=4
static XMC_ETH_MAC_DMA_DESC_t ETH_UIP_0_rx_desc[ETH_UIP_0_NUM_RX_BUF] @ ".iram2";
#pragma data_alignment=4
static XMC_ETH_MAC_DMA_DESC_t ETH_UIP_0_tx_desc[ETH_UIP_0_NUM_TX_BUF] @ ".iram2";
#pragma data_alignment=4
static uint8_t ETH_UIP_0_rx_buf[ETH_UIP_0_NUM_RX_BUF][XMC_ETH_MAC_BUF_SIZE] @ ".iram2";
#pragma data_alignment=4
static uint8_t ETH_UIP_0_tx_buf[ETH_UIP_0_NUM_TX_BUF][XMC_ETH_MAC_BUF_SIZE] @ ".iram2";
#elif defined(__CC_ARM)
static __attribute__((aligned(4))) XMC_ETH_MAC_DMA_DESC_t ETH_UIP_0_rx_desc[ETH_UIP_0_NUM_RX_BUF] __attribute__((section ("RW_IRAM1")));
static __attribute__((aligned(4))) XMC_ETH_MAC_DMA_DESC_t ETH_UIP_0_tx_desc[ETH_UIP_0_NUM_TX_BUF] __attribute__((section ("RW_IRAM1")));
static __attribute__((aligned(4))) uint8_t ETH_UIP_0_rx_buf[ETH_UIP_0_NUM_RX_BUF][XMC_ETH_MAC_BUF_SIZE] __attribute__((section ("RW_IRAM1")));
static __attribute__((aligned(4))) uint8_t ETH_UIP_0_tx_buf[ETH_UIP_0_NUM_TX_BUF][XMC_ETH_MAC_BUF_SIZE] __attribute__((section ("RW_IRAM1")));
#elif defined(__GNUC__)
static __attribute__((aligned(4))) XMC_ETH_MAC_DMA_DESC_t ETH_UIP_0_rx_desc[ETH_UIP_0_NUM_RX_BUF] __attribute__((section ("ETH_RAM")));
static __attribute__((aligned(4))) XMC_ETH_MAC_DMA_DESC_t ETH_UIP_0_tx_desc[ETH_UIP_0_NUM_TX_BUF] __attribute__((section ("ETH_RAM")));
static __attribute__((aligned(4))) uint8_t ETH_UIP_0_rx_buf[ETH_UIP_0_NUM_RX_BUF][XMC_ETH_MAC_BUF_SIZE] __attribute__((section ("ETH_RAM")));
static __attribute__((aligned(4))) uint8_t ETH_UIP_0_tx_buf[ETH_UIP_0_NUM_TX_BUF][XMC_ETH_MAC_BUF_SIZE] __attribute__((section ("ETH_RAM")));
#else
static __attribute__((aligned(4))) XMC_ETH_MAC_DMA_DESC_t ETH_UIP_0_rx_desc[ETH_UIP_0_NUM_RX_BUF];
static __attribute__((aligned(4))) XMC_ETH_MAC_DMA_DESC_t ETH_UIP_0_tx_desc[ETH_UIP_0_NUM_TX_BUF];
static __attribute__((aligned(4))) uint8_t ETH_UIP_0_rx_buf[ETH_UIP_0_NUM_RX_BUF][XMC_ETH_MAC_BUF_SIZE];
static __attribute__((aligned(4))) uint8_t ETH_UIP_0_tx_buf[ETH_UIP_0_NUM_TX_BUF][XMC_ETH_MAC_BUF_SIZE];
#endif

const XMC_ETH_PHY_CONFIG_t eth_phy_config =
{
  .interface = XMC_ETH_LINK_INTERFACE_RMII,
  .enable_auto_negotiate = false,
  .duplex = XMC_ETH_LINK_DUPLEX_FULL,
  .speed = XMC_ETH_LINK_SPEED_100M
};

XMC_ETH_MAC_t eth_mac =
{
  .regs = ETH0,
  .address = NETDEV_DEFAULT_MACADDR,
  .rx_desc = ETH_UIP_0_rx_desc,
  .tx_desc = ETH_UIP_0_tx_desc,
  .rx_buf = &ETH_UIP_0_rx_buf[0][0],
  .tx_buf = &ETH_UIP_0_tx_buf[0][0],
  .num_rx_buf = ETH_UIP_0_NUM_RX_BUF,
  .num_tx_buf = ETH_UIP_0_NUM_TX_BUF
};


/*---------------------------------------------------------------------------*/
void netdev_init(void)
{
  XMC_ETH_MAC_PORT_CTRL_t port_control;
  XMC_GPIO_CONFIG_t gpio_config;
  gpio_config.output_level = XMC_GPIO_OUTPUT_LEVEL_LOW;
  gpio_config.mode = XMC_GPIO_MODE_INPUT_TRISTATE;

  /* Initialize GPIO pins. */
  XMC_GPIO_Init(ETH_UIP_0_CRS_DV, &gpio_config);

  gpio_config.mode = XMC_GPIO_MODE_INPUT_TRISTATE;
  XMC_GPIO_Init(ETH_UIP_0_RXER, &gpio_config);

  gpio_config.mode = XMC_GPIO_MODE_INPUT_TRISTATE;
  XMC_GPIO_Init(ETH_UIP_0_RXD0, &gpio_config);

  gpio_config.mode = XMC_GPIO_MODE_INPUT_TRISTATE;
  XMC_GPIO_Init(ETH_UIP_0_RXD1, &gpio_config);

  gpio_config.output_strength = XMC_GPIO_OUTPUT_STRENGTH_STRONG_SHARP_EDGE;
  gpio_config.mode = XMC_GPIO_MODE_OUTPUT_PUSH_PULL_ALT1;
  XMC_GPIO_Init(ETH_UIP_0_TXEN, &gpio_config);

  gpio_config.output_strength = XMC_GPIO_OUTPUT_STRENGTH_STRONG_SHARP_EDGE;
  gpio_config.mode = XMC_GPIO_MODE_OUTPUT_PUSH_PULL_ALT1;
  XMC_GPIO_Init(ETH_UIP_0_TXD0, &gpio_config);

  gpio_config.output_strength = XMC_GPIO_OUTPUT_STRENGTH_STRONG_SHARP_EDGE;
  gpio_config.mode = XMC_GPIO_MODE_OUTPUT_PUSH_PULL_ALT1;
  XMC_GPIO_Init(ETH_UIP_0_TXD1, &gpio_config);

  gpio_config.mode = XMC_GPIO_MODE_INPUT_TRISTATE;
  XMC_GPIO_Init(ETH_UIP_0_RMII_CLK, &gpio_config);

  gpio_config.output_strength = XMC_GPIO_OUTPUT_STRENGTH_STRONG_SHARP_EDGE;
  gpio_config.mode = XMC_GPIO_MODE_OUTPUT_PUSH_PULL_ALT1;
  XMC_GPIO_Init(ETH_UIP_0_MDC, &gpio_config);

  gpio_config.mode = XMC_GPIO_MODE_INPUT_TRISTATE;
  XMC_GPIO_Init(ETH_UIP_0_MDIO, &gpio_config);

  XMC_GPIO_SetHardwareControl(ETH_UIP_0_MDIO, XMC_GPIO_HWCTRL_PERIPHERAL1);

  port_control.mode = XMC_ETH_MAC_PORT_CTRL_MODE_RMII;
  port_control.rxd0 = (XMC_ETH_MAC_PORT_CTRL_RXD0_t)0U;
  port_control.rxd1 = (XMC_ETH_MAC_PORT_CTRL_RXD1_t)0U;
  port_control.clk_rmii = (XMC_ETH_MAC_PORT_CTRL_CLK_RMII_t)2U;
  port_control.crs_dv = (XMC_ETH_MAC_PORT_CTRL_CRS_DV_t)2U;
  port_control.rxer = (XMC_ETH_MAC_PORT_CTRL_RXER_t)0U;
  port_control.mdio = (XMC_ETH_MAC_PORT_CTRL_MDIO_t)1U;
  XMC_ETH_MAC_SetPortControl(&eth_mac, port_control);
}


/*---------------------------------------------------------------------------*/
void netdev_init_mac(void)
{
  struct uip_eth_addr macAddress;
  uint32_t ulLinkTimeOut;

  (void)XMC_ETH_MAC_Init(&eth_mac);

  XMC_ETH_MAC_DisableJumboFrame(&eth_mac);

  /* Wait for link to become active. */
  ulLinkTimeOut = TimerGet() + NETDEV_LINKUP_TIMEOUT_MS;
  while (XMC_ETH_PHY_GetLinkStatus(&eth_mac, ETH_UIP_0_PHY_ADDR) == XMC_ETH_LINK_STATUS_DOWN)
  {
    /* Check for timeout so that the software program can still start if the ethernet
     * cable is not connected.
     */
    if (TimerGet() >= ulLinkTimeOut)
    {
      break;
    }
  }

  XMC_ETH_PHY_Init(&eth_mac, ETH_UIP_0_PHY_ADDR, &eth_phy_config);
  XMC_ETH_MAC_SetLink(&eth_mac, XMC_ETH_LINK_SPEED_100M, XMC_ETH_LINK_DUPLEX_FULL);
  XMC_ETH_MAC_EnableTx(&eth_mac);
  XMC_ETH_MAC_EnableRx(&eth_mac);

  /* set the default MAC address */
  macAddress.addr[0] = NETDEV_DEFAULT_MACADDR0;
  macAddress.addr[1] = NETDEV_DEFAULT_MACADDR1;
  macAddress.addr[2] = NETDEV_DEFAULT_MACADDR2;
  macAddress.addr[3] = NETDEV_DEFAULT_MACADDR3;
  macAddress.addr[4] = NETDEV_DEFAULT_MACADDR4;
  macAddress.addr[5] = NETDEV_DEFAULT_MACADDR5;
  uip_setethaddr(macAddress);
}


/*---------------------------------------------------------------------------*/
void netdev_get_mac(unsigned char * mac_addr)
{
  mac_addr[0] = NETDEV_DEFAULT_MACADDR0;
  mac_addr[1] = NETDEV_DEFAULT_MACADDR1;
  mac_addr[2] = NETDEV_DEFAULT_MACADDR2;
  mac_addr[3] = NETDEV_DEFAULT_MACADDR3;
  mac_addr[4] = NETDEV_DEFAULT_MACADDR4;
  mac_addr[5] = NETDEV_DEFAULT_MACADDR5;
}


/*---------------------------------------------------------------------------*/
unsigned int netdev_read(void)
{
  uint32_t result = 0;
  uint32_t framelen;

  /* Check if a new frame was received. */
  framelen = XMC_ETH_MAC_GetRxFrameSize(&eth_mac);
  /* Only process the frame if the length is valid. */
  if ((framelen > 0U) && (framelen <= (uint32_t)XMC_ETH_MAC_BUF_SIZE))
  {
    XMC_ETH_MAC_ReadFrame(&eth_mac, uip_buf, framelen);
    result = framelen;
  }

  /* Give the result back to the caller. */
  return result;
}


/*---------------------------------------------------------------------------*/
void netdev_send(void)
{
  uint8_t *buffer;
  uint32_t idx;

  /* Only send the frame if the length is valid. */
  if ((uip_len > 0U) && (uip_len <= (uint32_t)XMC_ETH_MAC_BUF_SIZE))
  {
    if (XMC_ETH_MAC_IsTxDescriptorOwnedByDma(&eth_mac))
    {
      XMC_ETH_MAC_ResumeTx(&eth_mac);
    }
    else
    {
      buffer = XMC_ETH_MAC_GetTxBuffer(&eth_mac);
      for (idx = 0; idx < uip_len; idx++)
      {
        buffer[idx] = uip_buf[idx];
      }
      XMC_ETH_MAC_SetTxBufferSize(&eth_mac, uip_len);
      /* Return the Tx descriptor but do not use the XMC_ETH_MAC_ReturnTxDescriptor().
       * This function enables the 'checksum insertion control' feature by setting the
       * ETH_MAC_DMA_TDES0_CIC in the eth_mac.tx_desc[eth_mac.tx_index].status. When
       * this feature is enabled, an incorrect checksum of 0x0000 is always added to
       * ICMP reponse messages such as the PING reply.
       */
      eth_mac.tx_ts_index = eth_mac.tx_index;
      eth_mac.tx_desc[eth_mac.tx_index].status |= ETH_MAC_DMA_TDES0_OWN;
      eth_mac.tx_index++;
      if (eth_mac.tx_index == eth_mac.num_tx_buf)
      {
        eth_mac.tx_index = 0U;
      }
      eth_mac.frame_end = NULL;
      XMC_ETH_MAC_ResumeTx(&eth_mac);
    }
  }
}

