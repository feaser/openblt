/************************************************************************************//**
* \file         Source\net.c
* \brief        Bootloader TCP/IP network communication interface source file.
* \ingroup      Core
* \internal
*----------------------------------------------------------------------------------------
*                          C O P Y R I G H T
*----------------------------------------------------------------------------------------
*   Copyright (c) 2014  by Feaser    http://www.feaser.com    All rights reserved
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
* You should have received a copy of the GNU General Public License along with OpenBLT.
* If not, see <http://www.gnu.org/licenses/>.
*
* A special exception to the GPL is included to allow you to distribute a combined work 
* that includes OpenBLT without being obliged to provide the source code for any 
* proprietary components. The exception text is included at the bottom of the license
* file <license.html>.
* 
* \endinternal
****************************************************************************************/

/****************************************************************************************
* Include files
****************************************************************************************/
#include "boot.h"                                /* bootloader generic header          */
#if (BOOT_COM_NET_ENABLE > 0)
#include "netdev.h"
#include "uip.h"
#include "uip_arp.h"
#endif


#if (BOOT_COM_NET_ENABLE > 0)
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
* Hook functions
****************************************************************************************/
#if (BOOT_COM_NET_IPADDR_HOOK_ENABLE > 0)
extern void NetIpAddressHook(blt_int8u *ipAddrArray);
#endif
#if (BOOT_COM_NET_NETMASK_HOOK_ENABLE > 0)
extern void NetNetworkMaskHook(blt_int8u *netMaskArray);
#endif
#if (BOOT_COM_NET_GATEWAY_HOOK_ENABLE > 0)
extern void NetGatewayAddressHook(blt_int8u *gatewayAddrArray);
#endif


/****************************************************************************************
* Function prototypes
****************************************************************************************/
static void NetServerTask(void);


/****************************************************************************************
* Local data declarations
****************************************************************************************/
/** \brief Holds the time out value of the uIP periodic timer. */
static blt_int32u periodicTimerTimeOut;
/** \brief Holds the time out value of the uIP ARP timer. */
static blt_int32u ARPTimerTimeOut;


/************************************************************************************//**
** \brief     Initializes the TCP/IP network communication interface.
** \return    none.
**
****************************************************************************************/
void NetInit(void)
{
  uip_ipaddr_t ipaddr;
  #if (BOOT_COM_NET_IPADDR_HOOK_ENABLE > 0)
  blt_int8u ipAddrArray[4];
  #endif
  #if (BOOT_COM_NET_NETMASK_HOOK_ENABLE > 0)
  blt_int8u netMaskArray[4];
  #endif
  #if (BOOT_COM_NET_GATEWAY_HOOK_ENABLE > 0)
  blt_int8u gatewayAddrArray[4];
  #endif

  /* initialize the network device */
  netdev_init();
  /* initialize the timer variables */
  periodicTimerTimeOut = TimerGet() + NET_UIP_PERIODIC_TIMER_MS;
  ARPTimerTimeOut = TimerGet() + NET_UIP_ARP_TIMER_MS;
  /* initialize the uIP TCP/IP stack. */
  uip_init();
  /* set the IP address */
  #if (BOOT_COM_NET_IPADDR_HOOK_ENABLE > 0)
  NetIpAddressHook(ipAddrArray);
  uip_ipaddr(ipaddr, ipAddrArray[0], ipAddrArray[1], ipAddrArray[2], ipAddrArray[3]);
  #else
  uip_ipaddr(ipaddr, BOOT_COM_NET_IPADDR0, BOOT_COM_NET_IPADDR1, BOOT_COM_NET_IPADDR2,
             BOOT_COM_NET_IPADDR3);
  #endif
  uip_sethostaddr(ipaddr);
  /* set the network mask */
  #if (BOOT_COM_NET_NETMASK_HOOK_ENABLE > 0)
  NetNetworkMaskHook(netMaskArray);
  uip_ipaddr(ipaddr, netMaskArray[0], netMaskArray[1], netMaskArray[2], netMaskArray[3]);
  #else
  uip_ipaddr(ipaddr, BOOT_COM_NET_NETMASK0, BOOT_COM_NET_NETMASK1, BOOT_COM_NET_NETMASK2,
             BOOT_COM_NET_NETMASK3);
  #endif
  uip_setnetmask(ipaddr);
  /* set the gateway address */
  #if (BOOT_COM_NET_GATEWAY_HOOK_ENABLE > 0)
  NetGatewayAddressHook(gatewayAddrArray);
  uip_ipaddr(ipaddr, gatewayAddrArray[0], gatewayAddrArray[1], gatewayAddrArray[2], gatewayAddrArray[3]);
  #else
  uip_ipaddr(ipaddr, BOOT_COM_NET_GATEWAY0, BOOT_COM_NET_GATEWAY1, BOOT_COM_NET_GATEWAY2,
             BOOT_COM_NET_GATEWAY3);
  #endif
  uip_setdraddr(ipaddr);
  /* start listening on the configured port for XCP transfers on TCP/IP */
  uip_listen(HTONS(BOOT_COM_NET_PORT));
  /* initialize the MAC and set the MAC address */
  netdev_init_mac();  
} /*** end of NetInit ***/


/************************************************************************************//**
** \brief     Transmits a packet formatted for the communication interface.
** \param     data Pointer to byte array with data that it to be transmitted.
** \param     len  Number of bytes that are to be transmitted.
** \return    none.
**
****************************************************************************************/
void NetTransmitPacket(blt_int8u *data, blt_int8u len)
{
  uip_tcp_appstate_t *s;
  blt_int16u cnt;
  
  /* get pointer to application state */
  s = &(uip_conn->appstate);

  /* add the dto counter first */
  *(blt_int32u*)&(s->dto_data[0]) = s->dto_counter;
  /* copy the actual XCP response */
  for (cnt=0; cnt<len; cnt++)
  {
    s->dto_data[cnt+4] = data[cnt];
  }
  /* set the length of the TCP/IP packet */
  s->dto_len = len + 4;
  /* submit it for transmission */
  uip_send(s->dto_data, s->dto_len);
  /* update dto counter for the next transmission */
  s->dto_counter++;
} /*** end of NetTransmitPacket ***/


/************************************************************************************//**
** \brief     Receives a communication interface packet if one is present.
** \param     data Pointer to byte array where the data is to be stored.
** \return    BLT_TRUE if a packet was received, BLT_FALSE otherwise.
**
****************************************************************************************/
blt_bool NetReceivePacket(blt_int8u *data)
{
  /* run the TCP/IP server task function, which will handle the reception and 
   * transmission of XCP packets
   */
  NetServerTask();
  
  /* packet reception and transmission is completely handled by the NetServerTask so 
   * always return BLT_FALSE here.
   */
  return BLT_FALSE;
} /*** end of NetReceivePacket ***/


/************************************************************************************//**
** \brief     The uIP network application that implements XCP on TCP/IP. Note that this
**            application make use of the fact that XCP is request/response based. So
**            no new request will come in when a response is pending for transmission,
**            if so, the transmission of the pending response is aborted.
** \return    none.
**
****************************************************************************************/
void NetApp(void)
{
  uip_tcp_appstate_t *s;
  blt_int8u *newDataPtr;

  /* get pointer to application state */
  s = &(uip_conn->appstate);
  
  if (uip_connected()) 
  {
    /* init the dto counter and reset the pending dto data length */
    s->dto_counter = 1;
    s->dto_len = 0;
    return;
  }

  if (uip_acked()) 
  {
    /* dto sent so set the pending dto data length to zero */
    s->dto_len = 0;
  }

  if (uip_rexmit()) 
  {
    /* retransmit the currently pending dto response */
    if (s->dto_len > 0)
    {
      /* resend the last pending dto response */
      uip_send(s->dto_data, s->dto_len);
    }
  }  

  if (uip_newdata()) 
  {
    /* XCP is request/response. this means is a new request comes in when a response
     * transmission is still pending, the XCP master either re-initialized or sent
     * the request again because of a response time-out. Either way the pending response
     * should be cancelled before handling the new request.
     */
    s->dto_len = 0;
    /* the first 4 bytes contain a counter value in which we are not really interested */
    newDataPtr = uip_appdata;
    XcpPacketReceived(&newDataPtr[4]);
  }
} /*** end of NetApp ***/


/************************************************************************************//**
** \brief     Runs the TCP/IP server task.
** \return    none.
**
****************************************************************************************/
static void NetServerTask(void)
{
  blt_int32u connection;
  blt_int32u packetLen;
  
  /* check for an RX packet and read it. */
  packetLen = netdev_read();
  if(packetLen > 0)
  {
    /* set uip_len for uIP stack usage */
    uip_len = (blt_int16u)packetLen;

    /* process incoming IP packets here. */
    if(NET_UIP_HEADER_BUF->type == htons(UIP_ETHTYPE_IP))
    {
      uip_arp_ipin();
      uip_input();
      /* if the above function invocation resulted in data that
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
    /* process incoming ARP packets here. */
    else if(NET_UIP_HEADER_BUF->type == htons(UIP_ETHTYPE_ARP))
    {
      uip_arp_arpin();

      /* if the above function invocation resulted in data that
       * should be sent out on the network, the global variable
       * uip_len is set to a value > 0.
       */
      if(uip_len > 0)
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
      if(uip_len > 0)
      {
        uip_arp_out();
        netdev_send();
        uip_len = 0;
      }
    }
  }
  
  /* process ARP Timer here. */
  if (TimerGet() >= ARPTimerTimeOut)
  {
      ARPTimerTimeOut += NET_UIP_ARP_TIMER_MS;
      uip_arp_timer();
  }
} /*** end of NetServerTask ***/
#endif /* BOOT_COM_NET_ENABLE > 0 */


/*********************************** end of net.c **************************************/
