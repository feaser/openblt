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
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_ethernet.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/ethernet.h"
#include "driverlib/flash.h"


/*---------------------------------------------------------------------------*/
#define NETDEV_LINKUP_TIMEOUT_MS          (5000)

#define NETDEV_DEFAULT_MACADDR0           (0x08)
#define NETDEV_DEFAULT_MACADDR1           (0x00)
#define NETDEV_DEFAULT_MACADDR2           (0x27)
#define NETDEV_DEFAULT_MACADDR3           (0x69)
#define NETDEV_DEFAULT_MACADDR4           (0x5B)
#define NETDEV_DEFAULT_MACADDR5           (0x45)


/*---------------------------------------------------------------------------*/
static struct uip_eth_addr macAddress;


/*---------------------------------------------------------------------------*/
void netdev_init(void)
{
  /* enable and reset the ethernet controller. */
  SysCtlPeripheralEnable(SYSCTL_PERIPH_ETH);
  SysCtlPeripheralReset(SYSCTL_PERIPH_ETH);
}


/*---------------------------------------------------------------------------*/
void netdev_init_mac(void)
{
  unsigned long ulUser0, ulUser1;
  unsigned long ulTemp;
  unsigned long ulLinkTimeOut;

  /* enable port F for ethernet LEDs.
   *  LED0        Bit 3   Output
   *  LED1        Bit 2   Output
   */
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
  GPIOPinTypeEthernetLED(GPIO_PORTF_BASE, GPIO_PIN_2 | GPIO_PIN_3);
  /* intialize the ethernet controller and disable all ethernet controller
   * interrupt sources.
   */
  EthernetIntDisable(ETH_BASE, (ETH_INT_PHY | ETH_INT_MDIO | ETH_INT_RXER |
                     ETH_INT_RXOF | ETH_INT_TX | ETH_INT_TXER | ETH_INT_RX));
  ulTemp = EthernetIntStatus(ETH_BASE, false);
  EthernetIntClear(ETH_BASE, ulTemp);
  /* initialize the ethernet controller for operation. */
  EthernetInitExpClk(ETH_BASE, SysCtlClockGet());
  /* configure the ethernet controller for normal operation.
   * - Full Duplex
   * - TX CRC Auto Generation
   * - TX Padding Enabled
   */
  EthernetConfigSet(ETH_BASE, (ETH_CFG_TX_DPLXEN | ETH_CFG_TX_CRCEN |
                               ETH_CFG_TX_PADEN));
  /* enable the ethernet controller. */
  EthernetEnable(ETH_BASE);

  /* wait for the link to become active. */
  ulTemp = EthernetPHYRead(ETH_BASE, PHY_MR1);
  ulLinkTimeOut = TimerGet() + NETDEV_LINKUP_TIMEOUT_MS;
  
  while ((ulTemp & 0x0004) == 0)
  {
    ulTemp = EthernetPHYRead(ETH_BASE, PHY_MR1);
    /* check for timeout so that the software program can still start if the 
     * ethernet cable is not connected.
     */
    if (TimerGet() >= ulLinkTimeOut)
    {
      break;
    }
  }

  /* set the default MAC address */
  macAddress.addr[0] = NETDEV_DEFAULT_MACADDR0;
  macAddress.addr[1] = NETDEV_DEFAULT_MACADDR1;
  macAddress.addr[2] = NETDEV_DEFAULT_MACADDR2;
  macAddress.addr[3] = NETDEV_DEFAULT_MACADDR3;
  macAddress.addr[4] = NETDEV_DEFAULT_MACADDR4;
  macAddress.addr[5] = NETDEV_DEFAULT_MACADDR5;
  /* the LM3S eval kit should have a MAC address pre-propgrammed in flash by the 
   * manufacturer. try to use this one, otherwise use the default values.
   */
  FlashUserGet(&ulUser0, &ulUser1);
  if ( (ulUser0 != 0xffffffff) && (ulUser1 != 0xffffffff) )
  {
    macAddress.addr[0] = ((ulUser0 >>  0) & 0xff);
    macAddress.addr[1] = ((ulUser0 >>  8) & 0xff);
    macAddress.addr[2] = ((ulUser0 >> 16) & 0xff);
    macAddress.addr[3] = ((ulUser1 >>  0) & 0xff);
    macAddress.addr[4] = ((ulUser1 >>  8) & 0xff);
    macAddress.addr[5] = ((ulUser1 >> 16) & 0xff);
  }
  EthernetMACAddrSet(ETH_BASE, &macAddress.addr[0]);
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
  unsigned long ulTemp;

  /* read and Clear the interrupt flag. */
  ulTemp = EthernetIntStatus(ETH_BASE, false);
  EthernetIntClear(ETH_BASE, ulTemp);

  /* check to see if an RX Interrupt has occured. */
  if(ulTemp & ETH_INT_RX)
  {
    return EthernetPacketGetNonBlocking(ETH_BASE, uip_buf, sizeof(uip_buf));
  }
  return 0;
}


/*---------------------------------------------------------------------------*/
void netdev_send(void)
{
  EthernetPacketPut(ETH_BASE, uip_buf, uip_len);
}


