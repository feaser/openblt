/************************************************************************************//**
* \file         Demo/ARMCM7_STM32F7_Nucleo_F767ZI_Keil/Prog/net.c
* \brief        Network application for the uIP TCP/IP stack.
* \ingroup      Prog_ARMCM7_STM32F7_Nucleo_F767ZI_Keil
* \internal
*----------------------------------------------------------------------------------------
*                          C O P Y R I G H T
*----------------------------------------------------------------------------------------
*   Copyright (c) 2019  by Feaser    http://www.feaser.com    All rights reserved
*
*----------------------------------------------------------------------------------------
*                            L I C E N S E
*----------------------------------------------------------------------------------------
* This file is part of OpenBLT. OpenBLT is free software: you can redistribute it and/or
* modify it under the terms of the GNU General Public License as published by the Free
* Software Foundation, either version 3 of the License, or (at your option) any later
* version.
*
* OpenBLT is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
* without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
* PURPOSE. See the GNU General Public License for more details.
*
* You have received a copy of the GNU General Public License along with OpenBLT. It 
* should be located in ".\Doc\license.html". If not, contact Feaser to obtain a copy.
* 
* \endinternal
****************************************************************************************/

/****************************************************************************************
* Include files
****************************************************************************************/
#include "header.h"                                    /* generic header               */
#include "netdev.h"
#include "uip.h"
#include "uip_arp.h"


/****************************************************************************************
* Macro definitions
****************************************************************************************/
/** \brief Delta time for the uIP periodic timer. */
#define NET_UIP_PERIODIC_TIMER_MS   (500)
/** \brief Delta time for the uIP ARP timer. */
#define NET_UIP_ARP_TIMER_MS        (10000)
/** \brief Macro for accessing the Ethernet header information in the buffer */
#define NET_UIP_HEADER_BUF          ((struct uip_eth_hdr *)&uip_buf[0])


/****************************************************************************************
* Local data declarations
****************************************************************************************/
/** \brief Holds the time out value of the uIP periodic timer. */
static unsigned long periodicTimerTimeOut;
/** \brief Holds the time out value of the uIP ARP timer. */
static unsigned long ARPTimerTimeOut;
#if (BOOT_COM_NET_DHCP_ENABLE > 0)
/** \brief Holds the MAC address which is used by the DHCP client. */
static struct uip_eth_addr macAddress;
#endif


/************************************************************************************//**
** \brief     Initializes the TCP/IP network communication interface.
** \return    none.
**
****************************************************************************************/
void NetInit(void)
{
  uip_ipaddr_t ipaddr;

  /* initialize the network device */
  netdev_init();
  /* initialize the timer variables */
  periodicTimerTimeOut = TimerGet() + NET_UIP_PERIODIC_TIMER_MS;
  ARPTimerTimeOut = TimerGet() + NET_UIP_ARP_TIMER_MS;
  /* initialize the uIP TCP/IP stack. */
  uip_init();
#if (BOOT_COM_NET_DHCP_ENABLE == 0)
  /* set the IP address */
  uip_ipaddr(ipaddr, BOOT_COM_NET_IPADDR0, BOOT_COM_NET_IPADDR1, BOOT_COM_NET_IPADDR2,
             BOOT_COM_NET_IPADDR3);
  uip_sethostaddr(ipaddr);
  /* set the network mask */
  uip_ipaddr(ipaddr, BOOT_COM_NET_NETMASK0, BOOT_COM_NET_NETMASK1, BOOT_COM_NET_NETMASK2,
             BOOT_COM_NET_NETMASK3);
  uip_setnetmask(ipaddr);
  /* set the gateway address */
  uip_ipaddr(ipaddr, BOOT_COM_NET_GATEWAY0, BOOT_COM_NET_GATEWAY1, BOOT_COM_NET_GATEWAY2,
             BOOT_COM_NET_GATEWAY3);
  uip_setdraddr(ipaddr);
#else
  /* set the IP address */
  uip_ipaddr(ipaddr, 0, 0, 0, 0);
  uip_sethostaddr(ipaddr);
  /* set the network mask */
  uip_ipaddr(ipaddr, 0, 0, 0, 0);
  uip_setnetmask(ipaddr);
  /* set the gateway address */
  uip_ipaddr(ipaddr, 0, 0, 0, 0);
  uip_setdraddr(ipaddr);
#endif
  /* start listening on the configured port for XCP transfers on TCP/IP */
  uip_listen(HTONS(BOOT_COM_NET_PORT));
  /* initialize the MAC and set the MAC address */
  netdev_init_mac();  

#if (BOOT_COM_NET_DHCP_ENABLE > 0)
    /* initialize the DHCP client application and send the initial request. */
    netdev_get_mac(&macAddress.addr[0]);
    dhcpc_init(&macAddress.addr[0], 6);
    dhcpc_request();
#endif
} /*** end of NetInit ***/


/************************************************************************************//**
** \brief     The uIP network application that detects the XCP connect command on the
**            port used by the bootloader. This indicates that the bootloader should
**            be activated.
** \return    none.
**
****************************************************************************************/
void NetApp(void)
{
  unsigned char *newDataPtr;
  unsigned short newDataLen;

  if (uip_connected()) 
  {
    return;
  }

  if (uip_newdata()) 
  {
    /* a new XCP command was received. check if this is the connect command and in this
     * case activate the bootloader. with XCP on TCP/IP the first 4 bytes contain a 
     * counter value in which we are not really interested.
     */
    newDataPtr = uip_appdata;
    newDataPtr += 4;
    newDataLen = uip_datalen() - 4;
    /* check if this was an XCP CONNECT command */
    if ((newDataPtr[0] == 0xff) && (newDataLen == 2))
    {
      /* the shared parameter at index 0 is used as a boolean flag to indicate if the
       * bootloader should initialize the TCP/IP network stack. we just received a
       * firmware update request from the TCP/IP network and we are about to active
       * the bootloader for a firmware update via TCP/IP. At this point we should
       * set the flag to make sure the bootloader initializes its TCP/IP network stack.
       */
      SharedParamsWriteByIndex(0, 1);
      /* connection request received so start the bootloader */
      BootActivate();
    }
  }
} /*** end of NetApp ***/


/************************************************************************************//**
** \brief     Runs the TCP/IP server task.
** \return    none.
**
****************************************************************************************/
void NetTask(void)
{
  unsigned long connection;
  unsigned long packetLen;
  
  /* check for an RX packet and read it. */
  packetLen = netdev_read();
  if (packetLen > 0)
  {
    /* set uip_len for uIP stack usage */
    uip_len = (unsigned short)packetLen;

    /* process incoming IP packets here. */
    if (NET_UIP_HEADER_BUF->type == htons(UIP_ETHTYPE_IP))
    {
      uip_arp_ipin();
      uip_input();
      /* if the above function invocation resulted in data that
       * should be sent out on the network, the global variable
       * uip_len is set to a value > 0.
       */
      if (uip_len > 0)
      {
        uip_arp_out();
        netdev_send();
        uip_len = 0;
      }
    }
    /* process incoming ARP packets here. */
    else if (NET_UIP_HEADER_BUF->type == htons(UIP_ETHTYPE_ARP))
    {
      uip_arp_arpin();

      /* if the above function invocation resulted in data that
       * should be sent out on the network, the global variable
       * uip_len is set to a value > 0.
       */
      if (uip_len > 0)
      {
        netdev_send();
        uip_len = 0;
      }
    }
  }

  /* process TCP/IP Periodic Timer here. */
  if (TimerGet() >= periodicTimerTimeOut)
  {
    periodicTimerTimeOut += NET_UIP_PERIODIC_TIMER_MS;
    for (connection = 0; connection < UIP_CONNS; connection++)
    {
      uip_periodic(connection);
      /* If the above function invocation resulted in data that
       * should be sent out on the network, the global variable
       * uip_len is set to a value > 0.
       */
      if (uip_len > 0)
      {
        uip_arp_out();
        netdev_send();
        uip_len = 0;
      }
    }

#if UIP_UDP
    for (connection = 0; connection < UIP_UDP_CONNS; connection++)
    {
      uip_udp_periodic(connection);
      /* If the above function invocation resulted in data that
       * should be sent out on the network, the global variable
       * uip_len is set to a value > 0.
       */
      if(uip_len > 0)
      {
        uip_arp_out();
        netdev_send();
        uip_len = 0;
      }
    }
#endif
  }

  /* process ARP Timer here. */
  if (TimerGet() >= ARPTimerTimeOut)
  {
    ARPTimerTimeOut += NET_UIP_ARP_TIMER_MS;
    uip_arp_timer();
  }
} /*** end of NetServerTask ***/


/*********************************** end of net.c **************************************/
