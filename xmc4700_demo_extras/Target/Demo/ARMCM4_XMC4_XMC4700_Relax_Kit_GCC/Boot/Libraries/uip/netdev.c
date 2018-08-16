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
#include "uip.h"
#include "uip_arp.h"
#include "boot.h"
#include <string.h>                                  /* for memcpy                     */


/*---------------------------------------------------------------------------*/
#define NETDEV_DEFAULT_MACADDR0           (0x08)
#define NETDEV_DEFAULT_MACADDR1           (0x00)
#define NETDEV_DEFAULT_MACADDR2           (0x27)
#define NETDEV_DEFAULT_MACADDR3           (0x69)
#define NETDEV_DEFAULT_MACADDR4           (0x5B)
#define NETDEV_DEFAULT_MACADDR5           (0x45)


/*---------------------------------------------------------------------------*/
void netdev_init(void)
{
  /* TODO ##Vg Implement netdev_init(). */
}


/*---------------------------------------------------------------------------*/
void netdev_init_mac(void)
{
  struct uip_eth_addr macAddress;

  /* set the default MAC address */
  macAddress.addr[0] = NETDEV_DEFAULT_MACADDR0;
  macAddress.addr[1] = NETDEV_DEFAULT_MACADDR1;
  macAddress.addr[2] = NETDEV_DEFAULT_MACADDR2;
  macAddress.addr[3] = NETDEV_DEFAULT_MACADDR3;
  macAddress.addr[4] = NETDEV_DEFAULT_MACADDR4;
  macAddress.addr[5] = NETDEV_DEFAULT_MACADDR5;

  /* TODO ##Vg Implement netdev_init_mac(). */
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
  unsigned long size = 0;

  /* TODO ##Vg Implement netdev_read(). */

  return size;
}


/*---------------------------------------------------------------------------*/
void netdev_send(void)
{
  /* TODO ##Vg Implement netdev_send(). */

}

