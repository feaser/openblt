/************************************************************************************//**
* \file         port/linux/netaccess.c
* \brief        TCP/IP network access source file.
* \ingroup      NetAccess
* \internal
*----------------------------------------------------------------------------------------
*                          C O P Y R I G H T
*----------------------------------------------------------------------------------------
*   Copyright (c) 2017  by Feaser    http://www.feaser.com    All rights reserved
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
#include <assert.h>                         /* for assertions                          */
#include <stdint.h>                         /* for standard integer types              */
#include <stddef.h>                         /* for NULL declaration                    */
#include <stdbool.h>                        /* for boolean type                        */
#include <string.h>                         /* for string utilities                    */
#include <unistd.h>                         /* for close                               */
#include <sys/socket.h>                     /* for socket                              */
#include <arpa/inet.h>                      /* for inet_addr                           */
#include <netdb.h>                          /* for hostent                             */
#include "netaccess.h"                      /* TCP/IP network access module            */


/****************************************************************************************
* Macro definitions
****************************************************************************************/
/** \brief Constant value that indicates that the network socket is invalid. */
#define NETACCESS_INVALID_SOCKET       (-1)


/****************************************************************************************
* Local data declarations
****************************************************************************************/
/** \brief The socket that is used as an endpoint for the TCP/IP network
 * communication.
 */
static int netAccessSocket;


/************************************************************************************//**
** \brief     Initializes the network access module.
**
****************************************************************************************/
void NetAccessInit(void)
{
  /* Invalidate the socket. */
  netAccessSocket = NETACCESS_INVALID_SOCKET;
} /*** end of NetAccessInit ***/


/************************************************************************************//**
** \brief     Terminates the network access module.
**
****************************************************************************************/
void NetAccessTerminate(void)
{
  /* Make sure to disconnect form the server. */
  NetAccessDisconnect();
} /*** end of NetAccessTerminate ***/


/************************************************************************************//**
** \brief     Connects to the TCP/IP server at the specified address and the given port.
** \param     address The address of the server. This can be a hostname (such as
**            mydomain.com) or an IP address (such as 127.0.0.1).
** \param     port The port number on the server to connect to.
** \return    True if successful, false otherwise.
**
****************************************************************************************/
bool NetAccessConnect(char const * address, uint16_t port)
{
  bool result = false;
  struct addrinfo hints = { 0 };
  struct addrinfo * serverinfo = NULL;
  struct sockaddr_in serverIPv4 = { 0 };
  struct sockaddr_in6 serverIPv6 = { 0 };
  bool serverIPv4found = false;
  bool serverIPv6found = false;
  
  /* Check parameters. */
  assert(address != NULL);
  assert(port > 0);

  /* Start by invalidating the socket. */
  netAccessSocket = NETACCESS_INVALID_SOCKET;

  /* Only continue with valid parameters. */
  if ( (address != NULL) && (port > 0) ) /*lint !e774 */
  {
    /* Set result to true and only reset it to false upon detection of a problem. */
    result = true;

    /* Initialize hints structure to aid in hostname resolving. Note that AF_UNSPEC is
     * used to support both IPv4 and IPv6.
     */
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = (int)SOCK_STREAM;
    hints.ai_flags |= AI_CANONNAME;

    /* Attempt to resolve the hostname. This converts the hostname to an IP address, if
     * it wasn't already an IP address.
     */
    if (getaddrinfo(address, NULL, &hints, &serverinfo) != 0)
    {
      /* Could not resolve the hostname. */
      result = false;
    }
    /* Sanity check on the pointer that should now be initialized and contain data. */
    else
    {
      if (serverinfo == NULL)
      {
        result = false;
      }
    }
    /* The serverinfo pointer now points to an array with results of the hostname
     * resolving. We only need one, so grab the first valid one. Prefer IPv4 over IPv6.
     */
    if (result)
    {
      /* Point to the first entry. */
      struct addrinfo * entry = serverinfo;
      /* Loop over the entries until a valid one was found. */
      while (entry != NULL)
      {
        /* Does this entry contain an IPv4 address? */
        if (entry->ai_family == AF_INET)
        {
          /* Copy this one for later usage. */
          memcpy (&serverIPv4, entry->ai_addr, entry->ai_addrlen);
          serverIPv4.sin_family = AF_INET;
          serverIPv4.sin_port = htons(port);
          /* Set flag so we know which socket address variable to use later on. */
          serverIPv4found = true;
          /* No need to go over the other entries, since we found a valid one. */
          break;
        }
        /* Does this entry contain an IPv6 address? */
        if (entry->ai_family == AF_INET6)
        {
          /* Copy this one for later usage. */
          memcpy (&serverIPv6, entry->ai_addr, entry->ai_addrlen);
          serverIPv6.sin6_family = AF_INET6;
          serverIPv6.sin6_port = htons(port);
          /* Set flag so we know which socket address variable to use later on. */
          serverIPv6found = true;
          /* No need to go over the other entries, since we found a valid one. */
          break;
        }
        /* Move on to the next one. */
        entry = entry->ai_next;
      }
    }
    /* Check that a valid entry was found. */
    if (result)
    {
      if ( (!serverIPv4found) && (!serverIPv6found) )
      {
        result = false;
      }
    }
    /* Create the socket. */
    if (result)
    {
      /* Create the socket based on the family type. */
      if (serverIPv4found)
      {
        netAccessSocket = socket(serverIPv4.sin_family, (int)SOCK_STREAM, IPPROTO_TCP);
      }
      else
      {
        netAccessSocket = socket(serverIPv6.sin6_family, (int)SOCK_STREAM, IPPROTO_TCP);
      }
      /* Check the socket. */
      if (netAccessSocket < 0)
      {
        /* Could not create the socket. */
        netAccessSocket = NETACCESS_INVALID_SOCKET;
        result = false;
      }
    }
    /* Connect the socket. */
    if (result)
    {
      /* Get the socket address pointer based on the family type. */
      struct sockaddr * server_address;
      if (serverIPv4found)
      {
        server_address = (struct sockaddr *)&serverIPv4; /*lint !e740 */
      }
      else
      {
        server_address = (struct sockaddr *)&serverIPv6; /*lint !e740 */
      }
      /* Attempt to connect. */
      if (connect(netAccessSocket, server_address, sizeof(struct sockaddr)) < 0)
      {
        /* Could not connect. Close the socket and negate result value. */
        close(netAccessSocket);
        netAccessSocket = NETACCESS_INVALID_SOCKET;
        result = false;
      }
    }
  }
  /* Give the result back to the caller. */
  return result;
} /*** end of NetAccessConnect ***/


/************************************************************************************//**
** \brief     Disconnects from the TCP/IP server.
**
****************************************************************************************/
void NetAccessDisconnect(void)
{
  /* Close the socket if it is open. */
  if (netAccessSocket >= 0)
  {
    close(netAccessSocket);
    netAccessSocket = NETACCESS_INVALID_SOCKET;
  }
} /*** end of NetAccessDisconnect ***/


/************************************************************************************//**
** \brief     Sends data to the TCP/IP server.
** \param     data Pointer to byte array with data to send.
** \param     length Number of bytes to send.
** \return    True if successful, false otherwise.
**
****************************************************************************************/
bool NetAccessSend(uint8_t const * data, uint32_t length)
{
  bool result = false;

  /* Check parameters. */
  assert(data != NULL);
  assert(length > 0);

  /* Only continue with valid parameters. */
  if ( (data != NULL) && (length > 0) ) /*lint !e774 */
  {
    /* Only continue with a valid socket. */
    if (netAccessSocket >= 0)
    {
      /* Attempt to send the data. */
      if (send(netAccessSocket, data, length, 0) >= 0)
      {
        /* Successfully send the data. */
        result = true;
      }
    }
  }
  /* Give the result back to the caller. */
  return result;
} /*** end of NetAccessSend ***/


/************************************************************************************//**
** \brief     Receives data from the TCP/IP server in a blocking manner.
** \param     data Pointer to byte array to store the received data.
** \param     length Holds the max number of bytes that can be stored into the byte
**            array. This function also overwrites this value with the number of bytes
**            that were actually received.
** \param     timeout Timeout in milliseconds for the data reception.
** \return    True if successful, false otherwise.
**
****************************************************************************************/
bool NetAccessReceive(uint8_t * data, uint32_t * length, uint32_t timeout)
{
  bool result = false;
  struct timeval tv;
  ssize_t receivedLen;

  /* Check parameters. */
  assert(data != NULL);
  assert(length != NULL);
  assert(timeout > 0);

  /* Only continue with valid parameters. */
  if ( (data != NULL) && (length != NULL) && (timeout > 0) ) /*lint !e774 */
  {
    /* Only continue with a valid socket. */
    if (netAccessSocket >= 0)
    {
      /* Configure the timeout for the receive operation. */
      tv.tv_sec = (time_t)(timeout / 1000u);
      tv.tv_usec = (timeout % 1000u) * 1000u;
      if (setsockopt(netAccessSocket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(struct timeval)) >= 0)
      {
        /* Attempt to receive data. */
        receivedLen = recv(netAccessSocket, data, *length, 0);
        /* Process the result. Everything < 0 indicate that an error occured. A value of
         * zero is also treated as an error, since data was expected.
         */
        if (receivedLen > 0)
        {
          /* Store the number of received bytes. */
          *length = (uint32_t)receivedLen;
          /* Successfully received data. */
          result = true;
        }
      }
    }
  }
  /* Give the result back to the caller. */
  return result;
} /*** end of NetAccessReceive ***/


/*********************************** end of netaccess.c ********************************/

