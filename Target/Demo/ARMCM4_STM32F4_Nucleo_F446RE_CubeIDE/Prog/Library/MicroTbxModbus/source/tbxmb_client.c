/************************************************************************************//**
* \file         tbxmb_client.c
* \brief        Modbus client source file.
* \internal
*----------------------------------------------------------------------------------------
*                          C O P Y R I G H T
*----------------------------------------------------------------------------------------
*   Copyright (c) 2023 by Feaser     www.feaser.com     All rights reserved
*
*----------------------------------------------------------------------------------------
*                            L I C E N S E
*----------------------------------------------------------------------------------------
*
* SPDX-License-Identifier: GPL-3.0-or-later
*
* This file is part of MicroTBX-Modbus. MicroTBX-Modbus is free software: you can
* redistribute it and/or modify it under the terms of the GNU General Public License as
* published by the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* MicroTBX-Modbus is distributed in the hope that it will be useful, but WITHOUT ANY
* WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
* PARTICULAR PURPOSE. See the GNU General Public License for more details.
*
* You have received a copy of the GNU General Public License along with MicroTBX-Modbus.
* If not, see www.gnu.org/licenses/.
*
* \endinternal
****************************************************************************************/

/****************************************************************************************
* Include files
****************************************************************************************/
#include "microtbx.h"                            /* MicroTBX module                    */
#include "microtbxmodbus.h"                      /* MicroTBX-Modbus module             */
#include "tbxmb_event_private.h"                 /* MicroTBX-Modbus event private      */
#include "tbxmb_osal_private.h"                  /* MicroTBX-Modbus OSAL private       */
#include "tbxmb_tp_private.h"                    /* MicroTBX-Modbus TP private         */
#include "tbxmb_client_private.h"                /* MicroTBX-Modbus client private     */


/****************************************************************************************
* Macro definitions
****************************************************************************************/
/** \brief Unique context type to identify a context as being a client channel. */
#define TBX_MB_CLIENT_CONTEXT_TYPE     (23U)


/****************************************************************************************
* Function prototypes
****************************************************************************************/
static void TbxMbClientProcessEvent(tTbxMbEvent * event);


/************************************************************************************//**
** \brief     Creates a Modbus client channel object and assigns the specified Modbus
**            transport layer to the channel for packet transmission and reception.
** \param     transport Handle to a previously created Modbus transport layer object to
**            assign to the channel.
** \param     responseTimeout Maximum time in milliseconds to wait for a response from
**            the Modbus server, after sending a PDU.
** \param     turnaroundDelay Delay time in milliseconds after sending a broadcast PDU
**            to give all recipients sufficient time to process the PDU.
** \return    Handle to the newly created Modbus client channel object if successful,
**            NULL otherwise.
**
****************************************************************************************/
tTbxMbClient TbxMbClientCreate(tTbxMbTp transport,
                               uint16_t responseTimeout,
                               uint16_t turnaroundDelay)
{
  tTbxMbClient result = NULL;

  /* Verify parameters. */
  TBX_ASSERT(transport != NULL);

  /* Only continue with valid parameters. */
  if (transport != NULL)
  {
    /* Allocate memory for the new channel context. */
    tTbxMbClientCtx * newClientCtx = TbxMemPoolAllocate(sizeof(tTbxMbClientCtx));
    /* Automatically increase the memory pool, if it was too small. */
    if (newClientCtx == NULL)
    {
      /* No need to check the return value, because if it failed, the following
       * allocation fails too, which is verified later on.
       */
      (void)TbxMemPoolCreate(1U, sizeof(tTbxMbClientCtx));
      newClientCtx = TbxMemPoolAllocate(sizeof(tTbxMbClientCtx));      
    }
    /* Verify memory allocation of the channel context. */
    TBX_ASSERT(newClientCtx != NULL);
    /* Only continue if the memory allocation succeeded. */
    if (newClientCtx != NULL)
    {
      /* Convert the TP channel pointer to the context structure. */
      tTbxMbTpCtx * tpCtx = (tTbxMbTpCtx *)transport;
      /* Sanity check on the transport layer's interface function. That way there is 
       * no need to do it later on, making it more run-time efficient. Also check that
       * it's not already linked to another channel.
       */
      TBX_ASSERT((tpCtx->transmitFcn != NULL) && (tpCtx->receptionDoneFcn != NULL) &&
                 (tpCtx->getRxPacketFcn != NULL) && (tpCtx->getTxPacketFcn != NULL) &&
                 (tpCtx->channelCtx == NULL));
      /* Initialize the channel context. Start by crosslinking the transport layer. */
      newClientCtx->type = TBX_MB_CLIENT_CONTEXT_TYPE;
      newClientCtx->instancePtr = NULL;
      newClientCtx->pollFcn = NULL;
      newClientCtx->processFcn = TbxMbClientProcessEvent;
      newClientCtx->responseTimeout = responseTimeout;
      newClientCtx->turnaroundDelay = turnaroundDelay;
      newClientCtx->transceiveSem = TbxMbOsalSemCreate();
      newClientCtx->tpCtx = tpCtx;
      newClientCtx->tpCtx->channelCtx = newClientCtx;
      newClientCtx->tpCtx->isClient = TBX_TRUE;
      /* Update the result. */
      result = newClientCtx;
    }
  }
  /* Give the result back to the caller. */
  return result;
} /*** end of TbxMbClientCreate ****/


/************************************************************************************//**
** \brief     Releases a Modbus client channel object, previously created with
**            TbxMbClientCreate().
** \param     channel Handle to the Modbus client channel object to release.
**
****************************************************************************************/
void TbxMbClientFree(tTbxMbClient channel)
{
  /* Verify parameters. */
  TBX_ASSERT(channel != NULL);

  /* Only continue with valid parameters. */
  if (channel != NULL)
  {
    /* Convert the client channel pointer to the context structure. */
    tTbxMbClientCtx * clientCtx = (tTbxMbClientCtx *)channel;
    /* Sanity check on the context type. */
    TBX_ASSERT(clientCtx->type == TBX_MB_CLIENT_CONTEXT_TYPE);
    /* Release the semaphore used for syncing to PDU transmit and reception events. */
    TbxMbOsalSemFree(clientCtx->transceiveSem);
    /* Remove crosslink between the channel and the transport layer. */
    TbxCriticalSectionEnter();
    clientCtx->tpCtx->channelCtx = NULL;
    clientCtx->tpCtx = NULL;
    /* Invalidate the context to protect it from accidentally being used afterwards. */
    clientCtx->type = 0U;
    clientCtx->pollFcn = NULL;
    clientCtx->processFcn = NULL;
    clientCtx->transceiveSem = NULL;
    TbxCriticalSectionExit();
    /* Give the channel context back to the memory pool. */
    TbxMemPoolRelease(clientCtx);
  }
} /*** end of TbxMbClientFree ***/


/************************************************************************************//**
** \brief     Event processing function that is automatically called when an event for
**            this client channel object was received in TbxMbEventTask().
** \param     event Pointer to the event to process. Note that the event->context points
**            to the handle of the Modbus client channel object.
**
****************************************************************************************/
static void TbxMbClientProcessEvent(tTbxMbEvent * event)
{
  /* Verify parameters. */
  TBX_ASSERT(event != NULL);

  /* Only continue with valid parameters. */
  if (event != NULL)
  {
    /* Sanity check the context. */
    TBX_ASSERT(event->context != NULL);
    /* Convert the event context  to the client channel context structure. */
    tTbxMbClientCtx * clientCtx = (tTbxMbClientCtx *)event->context;
    /* Make sure the context is valid. */
    TBX_ASSERT(clientCtx != NULL);
    /* Only continue with a valid context. */
    if (clientCtx != NULL)
    {
      /* Sanity check on the context type. */
      TBX_ASSERT(clientCtx->type == TBX_MB_CLIENT_CONTEXT_TYPE);
      /* Filter on the event identifier. */
      switch (event->id)
      {
        case TBX_MB_EVENT_ID_PDU_RECEIVED:
        {
          /* Give the PDU received semaphore to synchronize whatever task is waiting
           * for this event.
           */
          TbxMbOsalSemGive(clientCtx->transceiveSem, TBX_FALSE);
        }
        break;

        case TBX_MB_EVENT_ID_PDU_TRANSMITTED:
        {
          /* Give the PDU transmitted semaphore to synchronize whatever task is waiting
           * for this event.
           */
          TbxMbOsalSemGive(clientCtx->transceiveSem, TBX_FALSE);
        }
        break;

        default:
        {
          /* An unsupported event was dispatched to us. Should not happen. */
          TBX_ASSERT(TBX_FALSE);
        }
        break;
      }
    }
  }
} /*** end of TbxMbClientProcessEvent ***/


/************************************************************************************//**
** \brief     Helper function to both transmit a request packet and receive the reponse
**            packet, if applicable (unicast).
** \param     clientCtx Pointer to the Modbus client channel for the requested operation.
** \param     isBroadcast TBX_TRUE for sending a broadcast request, TBX_FALSE for
**            unicast.
** \return    TBX_OK if the request packet could be transmitted and (a) a response for
**            the unicast request was received or (b) the turnaround timeout passed after
**            sending the broadcast request. TBX_ERROR otherwise.
**
****************************************************************************************/
static uint8_t TbxMbClientTransceive(tTbxMbClientCtx * clientCtx,
                                     uint8_t           isBroadcast)
{
  uint8_t  result      = TBX_ERROR;
  uint16_t waitTimeout = clientCtx->responseTimeout;

  /* Update the wait time in case it is a broadcast request. */
  if (isBroadcast == TBX_TRUE)
  {
    waitTimeout = clientCtx->turnaroundDelay;
  }

  /* Request the transport layer to transmit the request packet and update the
   * result accordingly.
   */
  result = clientCtx->tpCtx->transmitFcn(clientCtx->tpCtx);
  /* Only continue if the request was successfully submitted for transmission. */
  if (result == TBX_OK)
  {
    /* Wait for the request packet transmit completion. The packet response reception
     * timeout can be re-used for this because a packet transmission won't take longer
     * than a packet reception, since it uses the same communication interface.
     */
    if (TbxMbOsalSemTake(clientCtx->transceiveSem, clientCtx->responseTimeout)
        == TBX_FALSE)
    {
      /* For some reason the packet transmission did not complete within the expected
       * time.
       * Flag the error.
       */
      result = TBX_ERROR;
    }
    /* Only continue if the request successfully completed transmission. */
    if (result == TBX_OK)
    {
      /* Wait for the reception of the response from the server, with a timeout. */
      if (TbxMbOsalSemTake(clientCtx->transceiveSem, waitTimeout) == TBX_FALSE)
      {
        /* Semaphore timeout occured. Either because no response was received, which
         * is an error. Or because the turnaround time after the broadcast request
         * passed, which is okay.
         */
        if (isBroadcast == TBX_FALSE)
        {
          /* Flag the error. */
          result = TBX_ERROR;
        }
      }
    }
  }
  /* Give the result back to the caller. */
  return result;
} /*** end of TbxMbClientTransceive ***/


/************************************************************************************//**
** \brief     Reads the coil(s) from the server with the specified node address.
** \param     channel Handle to the Modbus client channel for the requested operation.
** \param     node The address of the server. This parameter is transport layer
**            dependent. It is needed on RTU/ASCII, yet don't care for TCP unless it is
**            a gateway to an RTU network. If it's don't care, set it to a value of 1.
** \param     addr Starting element address (0..65535) in the Modbus data table for the
**            coil read operation.
** \param     num Number of elements to read from the coils data table. Range can be
**            1..2000.
** \param     coils Pointer to array with TBX_ON / TBX_OFF values where the coil state
**            will be written to.
** \return    TBX_OK if successful, TBX_ERROR otherwise.
**
****************************************************************************************/
uint8_t TbxMbClientReadCoils(tTbxMbClient   channel,
                             uint8_t        node,
                             uint16_t       addr,
                             uint16_t       num,
                             uint8_t      * coils)
{
  uint8_t result = TBX_ERROR;

  /* Verify the parameters. */
  TBX_ASSERT((channel != NULL) && (node <= TBX_MB_TP_NODE_ADDR_MAX) && (num >= 1U) &&
             (num <= 2000U) && (coils != NULL));

  /* Only continue with valid parameters. */
  if ((channel != NULL) && (node <= TBX_MB_TP_NODE_ADDR_MAX) && (num >= 1U) &&
      (num <= 2000U) && (coils != NULL))
  {
    /* Convert the client channel pointer to the context structure. */
    tTbxMbClientCtx * clientCtx = (tTbxMbClientCtx *)channel;
    /* Sanity check on the context type. */
    TBX_ASSERT(clientCtx->type == TBX_MB_CLIENT_CONTEXT_TYPE);

    /* Obtain write access to the request packet. */
    tTbxMbTpPacket * txPacket = clientCtx->tpCtx->getTxPacketFcn(clientCtx->tpCtx);
    /* Should always work, unless this function is being called recursively. Only
     * continue with access for preparing the request packet.
     */
    if (txPacket != NULL)
    {
      /* Prepare the request packet. */
      txPacket->node = node;
      txPacket->pdu.code = TBX_MB_FC01_READ_COILS;
      txPacket->dataLen = 4U;
      /* Starting address. */
      TbxMbCommonStoreUInt16BE(addr, &txPacket->pdu.data[0]);
      /* Number of coils. */
      TbxMbCommonStoreUInt16BE(num, &txPacket->pdu.data[2]);

      /* Determine the request type (broadcast / unicast). */
      uint8_t isBroadcast = TBX_FALSE;
      if (node == TBX_MB_TP_NODE_ADDR_BROADCAST)
      {
        isBroadcast = TBX_TRUE;
      }
      /* Transmit the request and wait for the response to a unicast request to come in
       * or the turnaround time to pass for a broadcast request.
       */
      result = TbxMbClientTransceive(clientCtx, isBroadcast);

      /* Only continue with processing the response if all is okay so far and the request
       * was unicast.
       */
      if ((result == TBX_OK) && (isBroadcast == TBX_FALSE))
      {
        /* Obtain read access to the response packet. */
        tTbxMbTpPacket * rxPacket = clientCtx->tpCtx->getRxPacketFcn(clientCtx->tpCtx);
        /* Since we just received a response packet, the packet access should always 
         * succeed. Sanity check anyways, just in case.
         */
        TBX_ASSERT(rxPacket != NULL);
        /* Only continue with packet access. */
        if (rxPacket != NULL)
        {
          /* Determine the number of bytes needed to hold all the coil bits. The cast to
           * U8 is okay, because we know that num is <= 2000.
           */
          uint8_t numBytes = (uint8_t)(num / 8U);
          if ((num % 8U) != 0U)
          {
            numBytes++;
          }
          /* Check that the response came from the expected node, that it's a response
           * with the same function code (not an exception response) and that the data
           * length and the byte count are as expected.
           */
          uint8_t byteCount = rxPacket->pdu.data[0];
          if ((rxPacket->node != node) ||
              (rxPacket->pdu.code != TBX_MB_FC01_READ_COILS) ||
              (byteCount != numBytes) ||
              (rxPacket->dataLen != (byteCount + 1U)) )
          {
            result = TBX_ERROR;
          }
          /* Response content valid. Process its data. */
          else
          {
            /* Prepare loop indices that aid with reading the coils. */
            uint8_t   bitIdx  = 0U;
            uint8_t   byteIdx = 0U;
            /* Initialize byte array pointer for reading the coils. */
            uint8_t const * coilData = &rxPacket->pdu.data[1];
            /* Loop through all the coils. */
            for (uint16_t idx = 0U; idx < num; idx++)
            {
              /* Extract and store the state of the coil. */
              if ((coilData[byteIdx] & (1U << bitIdx)) != 0U)
              {
                coils[idx] = TBX_ON;
              }
              else
              {
                coils[idx] = TBX_OFF;
              }
              /* Update the bit index. */
              bitIdx++;
              /* Time to move to the next byte? */
              if (bitIdx == 8U)
              {
                /* Reset the bit index and increment the byte index. */
                bitIdx = 0U;
                byteIdx++;
              }
            }
          }
        }
        /* Could not access the response packet. */
        else
        {
          result = TBX_ERROR;
        }
        /* Inform the transport layer that were done with the rx packet and no longer
         * need access to it.
         */
        clientCtx->tpCtx->receptionDoneFcn(clientCtx->tpCtx);
      }
    }
  }      
  /* Give the result back to the caller. */
  return result;
} /*** end of TbxMbClientReadCoils ***/


/************************************************************************************//**
** \brief     Reads the discrete input(s) from the server with the specified node
**            address.
** \param     channel Handle to the Modbus client channel for the requested operation.
** \param     node The address of the server. This parameter is transport layer
**            dependent. It is needed on RTU/ASCII, yet don't care for TCP unless it is
**            a gateway to an RTU network. If it's don't care, set it to a value of 1.
** \param     addr Starting element address (0..65535) in the Modbus data table for the
**            discrete input read operation.
** \param     num Number of elements to read from the discrete inputs data table. Range
**            can be 1..2000
** \param     inputs Pointer to array with TBX_ON / TBX_OFF values where the discrete
**            input state will be written to.
** \return    TBX_OK if successful, TBX_ERROR otherwise.
**
****************************************************************************************/
uint8_t TbxMbClientReadInputs(tTbxMbClient   channel,
                              uint8_t        node,
                              uint16_t       addr,
                              uint16_t       num,
                              uint8_t      * inputs)
{
  uint8_t result = TBX_ERROR;

  /* Verify the parameters. */
  TBX_ASSERT((channel != NULL) && (node <= TBX_MB_TP_NODE_ADDR_MAX) && (num >= 1U) &&
             (num <= 2000U) && (inputs != NULL));

  /* Only continue with valid parameters. */
  if ((channel != NULL) && (node <= TBX_MB_TP_NODE_ADDR_MAX) && (num >= 1U) &&
      (num <= 2000U) && (inputs != NULL))
  {
    /* Convert the client channel pointer to the context structure. */
    tTbxMbClientCtx * clientCtx = (tTbxMbClientCtx *)channel;
    /* Sanity check on the context type. */
    TBX_ASSERT(clientCtx->type == TBX_MB_CLIENT_CONTEXT_TYPE);

    /* Obtain write access to the request packet. */
    tTbxMbTpPacket * txPacket = clientCtx->tpCtx->getTxPacketFcn(clientCtx->tpCtx);
    /* Should always work, unless this function is being called recursively. Only
     * continue with access for preparing the request packet.
     */
    if (txPacket != NULL)
    {
      /* Prepare the request packet. */
      txPacket->node = node;
      txPacket->pdu.code = TBX_MB_FC02_READ_DISCRETE_INPUTS;
      txPacket->dataLen = 4U;
      /* Starting address. */
      TbxMbCommonStoreUInt16BE(addr, &txPacket->pdu.data[0]);
      /* Number of discrete inputs. */
      TbxMbCommonStoreUInt16BE(num, &txPacket->pdu.data[2]);

      /* Determine the request type (broadcast / unicast). */
      uint8_t isBroadcast = TBX_FALSE;
      if (node == TBX_MB_TP_NODE_ADDR_BROADCAST)
      {
        isBroadcast = TBX_TRUE;
      }
      /* Transmit the request and wait for the response to a unicast request to come in
       * or the turnaround time to pass for a broadcast request.
       */
      result = TbxMbClientTransceive(clientCtx, isBroadcast);

      /* Only continue with processing the response if all is okay so far and the request
       * was unicast.
       */
      if ((result == TBX_OK) && (isBroadcast == TBX_FALSE))
      {
        /* Obtain read access to the response packet. */
        tTbxMbTpPacket * rxPacket = clientCtx->tpCtx->getRxPacketFcn(clientCtx->tpCtx);
        /* Since we just received a response packet, the packet access should always 
         * succeed. Sanity check anyways, just in case.
         */
        TBX_ASSERT(rxPacket != NULL);
        /* Only continue with packet access. */
        if (rxPacket != NULL)
        {
          /* Determine the number of bytes needed to hold all the input bits. The cast to
           * U8 is okay, because we know that num is <= 2000.
           */
          uint8_t numBytes = (uint8_t)(num / 8U);
          if ((num % 8U) != 0U)
          {
            numBytes++;
          }
          /* Check that the response came from the expected node, that it's a response
           * with the same function code (not an exception response) and that the data
           * length and the byte count are as expected.
           */
          uint8_t byteCount = rxPacket->pdu.data[0];
          if ((rxPacket->node != node) ||
              (rxPacket->pdu.code != TBX_MB_FC02_READ_DISCRETE_INPUTS) ||
              (byteCount != numBytes) ||
              (rxPacket->dataLen != (byteCount + 1U)) )
          {
            result = TBX_ERROR;
          }
          /* Response content valid. Process its data. */
          else
          {
            /* Prepare loop indices that aid with reading the input bits. */
            uint8_t   bitIdx  = 0U;
            uint8_t   byteIdx = 0U;
            /* Initialize byte array pointer for reading the input bits. */
            uint8_t const * inputData = &rxPacket->pdu.data[1];
            /* Loop through all the inputs. */
            for (uint16_t idx = 0U; idx < num; idx++)
            {
              /* Extract and store the state of the discrete input. */
              if ((inputData[byteIdx] & (1U << bitIdx)) != 0U)
              {
                inputs[idx] = TBX_ON;
              }
              else
              {
                inputs[idx] = TBX_OFF;
              }
              /* Update the bit index. */
              bitIdx++;
              /* Time to move to the next byte? */
              if (bitIdx == 8U)
              {
                /* Reset the bit index and increment the byte index. */
                bitIdx = 0U;
                byteIdx++;
              }
            }
          }
        }
        /* Could not access the response packet. */
        else
        {
          result = TBX_ERROR;
        }
        /* Inform the transport layer that were done with the rx packet and no longer
         * need access to it.
         */
        clientCtx->tpCtx->receptionDoneFcn(clientCtx->tpCtx);
      }
    }
  }      
  /* Give the result back to the caller. */
  return result;
} /*** end of TbxMbClientReadInputs ***/


/************************************************************************************//**
** \brief     Reads the input register(s) from the server with the specified node
**            address.
** \param     channel Handle to the Modbus client channel for the requested operation.
** \param     node The address of the server. This parameter is transport layer
**            dependent. It is needed on RTU/ASCII, yet don't care for TCP unless it is
**            a gateway to an RTU network. If it's don't care, set it to a value of 1.
** \param     addr Starting element address (0..65535) in the Modbus data table for the
**            input register read operation.
** \param     num Number of elements to read from the input registers data table. Range
**            can be 1..125
** \param     inputRegs Pointer to array where the input register values will be written
**            to.
** \return    TBX_OK if successful, TBX_ERROR otherwise.
**
****************************************************************************************/
uint8_t TbxMbClientReadInputRegs(tTbxMbClient   channel,
                                 uint8_t        node,
                                 uint16_t       addr,
                                 uint8_t        num,
                                 uint16_t     * inputRegs)
{
  uint8_t result = TBX_ERROR;

  /* Verify the parameters. */
  TBX_ASSERT((channel != NULL) && (node <= TBX_MB_TP_NODE_ADDR_MAX) && (num >= 1U) &&
             (num <= 125U) && (inputRegs != NULL));

  /* Only continue with valid parameters. */
  if ((channel != NULL) && (node <= TBX_MB_TP_NODE_ADDR_MAX) && (num >= 1U) &&
      (num <= 125U) && (inputRegs != NULL))
  {
    /* Convert the client channel pointer to the context structure. */
    tTbxMbClientCtx * clientCtx = (tTbxMbClientCtx *)channel;
    /* Sanity check on the context type. */
    TBX_ASSERT(clientCtx->type == TBX_MB_CLIENT_CONTEXT_TYPE);

    /* Obtain write access to the request packet. */
    tTbxMbTpPacket * txPacket = clientCtx->tpCtx->getTxPacketFcn(clientCtx->tpCtx);
    /* Should always work, unless this function is being called recursively. Only
     * continue with access for preparing the request packet.
     */
    if (txPacket != NULL)
    {
      /* Prepare the request packet. */
      txPacket->node = node;
      txPacket->pdu.code = TBX_MB_FC04_READ_INPUT_REGISTERS;
      txPacket->dataLen = 4U;
      /* Starting address. */
      TbxMbCommonStoreUInt16BE(addr, &txPacket->pdu.data[0]);
      /* Number of registers. */
      TbxMbCommonStoreUInt16BE(num, &txPacket->pdu.data[2]);

      /* Determine the request type (broadcast / unicast). */
      uint8_t isBroadcast = TBX_FALSE;
      if (node == TBX_MB_TP_NODE_ADDR_BROADCAST)
      {
        isBroadcast = TBX_TRUE;
      }
      /* Transmit the request and wait for the response to a unicast request to come in
       * or the turnaround time to pass for a broadcast request.
       */
      result = TbxMbClientTransceive(clientCtx, isBroadcast);

      /* Only continue with processing the response if all is okay so far and the request
       * was unicast.
       */
      if ((result == TBX_OK) && (isBroadcast == TBX_FALSE))
      {
        /* Obtain read access to the response packet. */
        tTbxMbTpPacket * rxPacket = clientCtx->tpCtx->getRxPacketFcn(clientCtx->tpCtx);
        /* Since we just received a response packet, the packet access should always 
         * succeed. Sanity check anyways, just in case.
         */
        TBX_ASSERT(rxPacket != NULL);
        /* Only continue with packet access. */
        if (rxPacket != NULL)
        {
          /* Check that the response came from the expected node, that it's a response
           * with the same function code (not an exception response) and that the data
           * length and the byte count are as expected.
           */
          uint8_t byteCount = rxPacket->pdu.data[0];
          if ((rxPacket->node != node) ||
              (rxPacket->pdu.code != TBX_MB_FC04_READ_INPUT_REGISTERS) ||
              (byteCount != (num * 2U)) ||
              (rxPacket->dataLen != (byteCount + 1U)) )
          {
            result = TBX_ERROR;
          }
          /* Response content valid. Process its data. */
          else
          {
            /* Set pointer to where the input registers start in the response. */
            uint8_t const * regValPtr = &rxPacket->pdu.data[1];
            /* Read out and store the input register values. */
            for (uint8_t idx = 0U; idx < num; idx++)
            {
              inputRegs[idx] = TbxMbCommonExtractUInt16BE(&regValPtr[idx * 2U]);
            }
          }
        }
        /* Could not access the response packet. */
        else
        {
          result = TBX_ERROR;
        }
        /* Inform the transport layer that were done with the rx packet and no longer
         * need access to it.
         */
        clientCtx->tpCtx->receptionDoneFcn(clientCtx->tpCtx);
      }
    }
  }
  /* Give the result back to the caller. */
  return result;
} /*** end of TbxMbClientReadInputRegs ***/


/************************************************************************************//**
** \brief     Reads the holding register(s) from the server with the specified node
**            address.
** \param     channel Handle to the Modbus client channel for the requested operation.
** \param     node The address of the server. This parameter is transport layer
**            dependent. It is needed on RTU/ASCII, yet don't care for TCP unless it is
**            a gateway to an RTU network. If it's don't care, set it to a value of 1.
** \param     addr Starting element address (0..65535) in the Modbus data table for the
**            holding register read operation.
** \param     num Number of elements to read from the holding registers data table. Range
**            can be 1..125
** \param     holdingRegs Pointer to array where the holding register values will be
**            written to.
** \return    TBX_OK if successful, TBX_ERROR otherwise.
**
****************************************************************************************/
uint8_t TbxMbClientReadHoldingRegs(tTbxMbClient   channel,
                                   uint8_t        node,
                                   uint16_t       addr,
                                   uint8_t        num,
                                   uint16_t     * holdingRegs)
{
  uint8_t result = TBX_ERROR;

  /* Verify the parameters. */
  TBX_ASSERT((channel != NULL) && (node <= TBX_MB_TP_NODE_ADDR_MAX) && (num >= 1U) &&
             (num <= 125U) && (holdingRegs != NULL));

  /* Only continue with valid parameters. */
  if ((channel != NULL) && (node <= TBX_MB_TP_NODE_ADDR_MAX) && (num >= 1U) &&
      (num <= 125U) && (holdingRegs != NULL))
  {
    /* Convert the client channel pointer to the context structure. */
    tTbxMbClientCtx * clientCtx = (tTbxMbClientCtx *)channel;
    /* Sanity check on the context type. */
    TBX_ASSERT(clientCtx->type == TBX_MB_CLIENT_CONTEXT_TYPE);

    /* Obtain write access to the request packet. */
    tTbxMbTpPacket * txPacket = clientCtx->tpCtx->getTxPacketFcn(clientCtx->tpCtx);
    /* Should always work, unless this function is being called recursively. Only
     * continue with access for preparing the request packet.
     */
    if (txPacket != NULL)
    {
      /* Prepare the request packet. */
      txPacket->node = node;
      txPacket->pdu.code = TBX_MB_FC03_READ_HOLDING_REGISTERS;
      txPacket->dataLen = 4U;
      /* Starting address. */
      TbxMbCommonStoreUInt16BE(addr, &txPacket->pdu.data[0]);
      /* Number of registers. */
      TbxMbCommonStoreUInt16BE(num, &txPacket->pdu.data[2]);

      /* Determine the request type (broadcast / unicast). */
      uint8_t isBroadcast = TBX_FALSE;
      if (node == TBX_MB_TP_NODE_ADDR_BROADCAST)
      {
        isBroadcast = TBX_TRUE;
      }
      /* Transmit the request and wait for the response to a unicast request to come in
       * or the turnaround time to pass for a broadcast request.
       */
      result = TbxMbClientTransceive(clientCtx, isBroadcast);

      /* Only continue with processing the response if all is okay so far and the request
       * was unicast.
       */
      if ((result == TBX_OK) && (isBroadcast == TBX_FALSE))
      {
        /* Obtain read access to the response packet. */
        tTbxMbTpPacket * rxPacket = clientCtx->tpCtx->getRxPacketFcn(clientCtx->tpCtx);
        /* Since we just received a response packet, the packet access should always 
         * succeed. Sanity check anyways, just in case.
         */
        TBX_ASSERT(rxPacket != NULL);
        /* Only continue with packet access. */
        if (rxPacket != NULL)
        {
          /* Check that the response came from the expected node, that it's a response
           * with the same function code (not an exception response) and that the data
           * length and the byte count are as expected.
           */
          uint8_t byteCount = rxPacket->pdu.data[0];
          if ((rxPacket->node != node) ||
              (rxPacket->pdu.code != TBX_MB_FC03_READ_HOLDING_REGISTERS) ||
              (byteCount != (num * 2U)) ||
              (rxPacket->dataLen != (byteCount + 1U)) )
          {
            result = TBX_ERROR;
          }
          /* Response content valid. Process its data. */
          else
          {
            /* Set pointer to where the holding registers start in the response. */
            uint8_t const * regValPtr = &rxPacket->pdu.data[1];
            /* Read out and store the holding register values. */
            for (uint8_t idx = 0U; idx < num; idx++)
            {
              holdingRegs[idx] = TbxMbCommonExtractUInt16BE(&regValPtr[idx * 2U]);
            }
          }
        }
        /* Could not access the response packet. */
        else
        {
          result = TBX_ERROR;
        }
        /* Inform the transport layer that were done with the rx packet and no longer
         * need access to it.
         */
        clientCtx->tpCtx->receptionDoneFcn(clientCtx->tpCtx);
      }
    }
  }
  /* Give the result back to the caller. */
  return result;
} /*** end of TbxMbClientReadHoldingRegs ***/


/************************************************************************************//**
** \brief     Writes the coil(s) to the server with the specified node address.
** \param     channel Handle to the Modbus client channel for the requested operation.
** \param     node The address of the server. This parameter is transport layer
**            dependent. It is needed on RTU/ASCII, yet don't care for TCP unless it is
**            a gateway to an RTU network. If it's don't care, set it to a value of 1.
** \param     addr Starting element address (0..65535) in the Modbus data table for the
**            coil write operation.
** \param     num Number of elements to write to the coils data table. Range can be
**            1..1968
** \param     coils Pointer to array with the desired TBX_ON / TBX_OFF coil values.
** \return    TBX_OK if successful, TBX_ERROR otherwise.
**
****************************************************************************************/
uint8_t TbxMbClientWriteCoils(tTbxMbClient         channel,
                              uint8_t              node,
                              uint16_t             addr,
                              uint16_t             num,
                              uint8_t      const * coils)
{
  uint8_t result = TBX_ERROR;

  /* Verify the parameters. */
  TBX_ASSERT((channel != NULL) && (node <= TBX_MB_TP_NODE_ADDR_MAX) && (num >= 1U) &&
             (num <= 1968U) && (coils != NULL));

  /* Only continue with valid parameters. */
  if ((channel != NULL) && (node <= TBX_MB_TP_NODE_ADDR_MAX) && (num >= 1U) &&
      (num <= 1968U) && (coils != NULL))
  {
    /* Convert the client channel pointer to the context structure. */
    tTbxMbClientCtx * clientCtx = (tTbxMbClientCtx *)channel;
    /* Sanity check on the context type. */
    TBX_ASSERT(clientCtx->type == TBX_MB_CLIENT_CONTEXT_TYPE);

    /* Obtain write access to the request packet. */
    tTbxMbTpPacket * txPacket = clientCtx->tpCtx->getTxPacketFcn(clientCtx->tpCtx);
    /* Should always work, unless this function is being called recursively. Only
     * continue with access for preparing the request packet.
     */
    if (txPacket != NULL)
    {
      /* Writing just a single coil? */
      if (num == 1U)
      {
        /* Prepare the request packet. */
        txPacket->node = node;
        txPacket->pdu.code = TBX_MB_FC05_WRITE_SINGLE_COIL;
        txPacket->dataLen = 4U;
        /* Coil address. */
        TbxMbCommonStoreUInt16BE(addr, &txPacket->pdu.data[0]);
        /* Coil value. */
        uint16_t coilValue = (coils[0] == TBX_OFF) ? 0x0000U : 0xFF00U;
        TbxMbCommonStoreUInt16BE(coilValue, &txPacket->pdu.data[2]);
      }
      /* Writing multiple coils. */
      else
      {
        /* Determine the number of bytes needed to hold all the coil bits. The cast to
         * U8 is okay, because we know that num is <= 1968.
         */
        uint8_t numBytes = (uint8_t)(num / 8U);
        if ((num % 8U) != 0U)
        {
          numBytes++;
        }
        /* Prepare the request packet. */
        txPacket->node = node;
        txPacket->pdu.code = TBX_MB_FC15_WRITE_MULTIPLE_COILS;
        txPacket->dataLen = numBytes + 5U;
        /* Start address. */
        TbxMbCommonStoreUInt16BE(addr, &txPacket->pdu.data[0]);
        /* Number of holding registers. */
        TbxMbCommonStoreUInt16BE(num, &txPacket->pdu.data[2]);
        /* Byte count. */
        txPacket->pdu.data[4] = numBytes;
        /* Prepare loop indices that aid with reading the input bits. */
        uint8_t   bitIdx  = 0U;
        uint8_t   byteIdx = 0U;
        /* Set pointer to where the coils start in the request and already initialize the
         * first byte to all zero (coil OFF) bits.
        */
        uint8_t * coilData = &txPacket->pdu.data[5];
        coilData[0] = 0U;
        /* Store the coil values. */
        for (uint16_t idx = 0U; idx < num; idx++)
        {
          /* Should the coil be ON? */
          if (coils[idx] != TBX_OFF)
          {
            coilData[byteIdx] |= (1U << bitIdx);
          }
          /* Update the bit index. */
          bitIdx++;
          /* Time to move to the next byte? */
          if (bitIdx == 8U)
          {
            /* Reset the bit index, increment the byte index and initialize the byte to
             * all zero (coil OFF) bits.
             */
            bitIdx = 0U;
            byteIdx++;
            coilData[byteIdx] = 0U;
          }
        }
      }
      /* Determine the request type (broadcast / unicast). */
      uint8_t isBroadcast = TBX_FALSE;
      if (node == TBX_MB_TP_NODE_ADDR_BROADCAST)
      {
        isBroadcast = TBX_TRUE;
      }
      /* Transmit the request and wait for the response to a unicast request to come in
       * or the turnaround time to pass for a broadcast request.
       */
      result = TbxMbClientTransceive(clientCtx, isBroadcast);

      /* Only continue with processing the response if all is okay so far and the request
       * was unicast.
       */
      if ((result == TBX_OK) && (isBroadcast == TBX_FALSE))
      {
        /* Obtain read access to the response packet. */
        tTbxMbTpPacket * rxPacket = clientCtx->tpCtx->getRxPacketFcn(clientCtx->tpCtx);
        /* Since we just received a response packet, the packet access should always 
         * succeed. Sanity check anyways, just in case.
         */
        TBX_ASSERT(rxPacket != NULL);
        /* Only continue with packet access. */
        if (rxPacket != NULL)
        {
          /* Wrote just a single coil? */
          if (num == 1U)
          {
            /* Coil value. */
            uint16_t coilValue = (coils[0] == TBX_OFF) ? 0x0000U : 0xFF00U;
            /* Check that the response came from the expected node, that it's a response
             * with the same function code (not an exception response), that the coil
             * address and value are as expected and that the data length is as
             * expected.
             */
            if ((rxPacket->node != node) ||
                (rxPacket->pdu.code != TBX_MB_FC05_WRITE_SINGLE_COIL) ||
                (TbxMbCommonExtractUInt16BE(&rxPacket->pdu.data[0]) != addr) ||
                (TbxMbCommonExtractUInt16BE(&rxPacket->pdu.data[2]) != coilValue) ||
                (rxPacket->dataLen != 4U))
            {
              result = TBX_ERROR;
            }
          }
          /* Wrote multiple holding registers. */
          else
          {
            /* Check that the response came from the expected node, that it's a response
             * with the same function code (not an exception response), that the coil
             * start address and quantity are as expected and that the data length is as
             * expected.
             */
            if ((rxPacket->node != node) ||
                (rxPacket->pdu.code != TBX_MB_FC15_WRITE_MULTIPLE_COILS) ||
                (TbxMbCommonExtractUInt16BE(&rxPacket->pdu.data[0]) != addr) ||
                (TbxMbCommonExtractUInt16BE(&rxPacket->pdu.data[2]) != num) ||
                (rxPacket->dataLen != 4U))
            {
              result = TBX_ERROR;
            }
          }
        }
        /* Could not access the response packet. */
        else
        {
          result = TBX_ERROR;
        }
        /* Inform the transport layer that were done with the rx packet and no longer
         * need access to it.
         */
        clientCtx->tpCtx->receptionDoneFcn(clientCtx->tpCtx);
      }
    }
  }  
  /* Give the result back to the caller. */
  return result;
} /*** end of TbxMbClientWriteCoils ***/


/************************************************************************************//**
** \brief     Writes the holding register(s) to the server with the specified node
**            address.
** \param     channel Handle to the Modbus client channel for the requested operation.
** \param     node The address of the server. This parameter is transport layer
**            dependent. It is needed on RTU/ASCII, yet don't care for TCP unless it is
**            a gateway to an RTU network. If it's don't care, set it to a value of 1.
** \param     addr Starting element address (0..65535) in the Modbus data table for the
**            holding register write operation.
** \param     num Number of elements to write to the holding registers data table. Range
**            can be 1..123
** \param     holdingRegs Pointer to array with the desired holding register values.
** \return    TBX_OK if successful, TBX_ERROR otherwise.
**
****************************************************************************************/
uint8_t TbxMbClientWriteHoldingRegs(tTbxMbClient         channel,
                                    uint8_t              node,
                                    uint16_t             addr,
                                    uint8_t              num,
                                    uint16_t     const * holdingRegs)
{
  uint8_t result = TBX_ERROR;

  /* Verify the parameters. */
  TBX_ASSERT((channel != NULL) && (node <= TBX_MB_TP_NODE_ADDR_MAX) && (num >= 1U) &&
             (num <= 123U) && (holdingRegs != NULL));

  /* Only continue with valid parameters. */
  if ((channel != NULL) && (node <= TBX_MB_TP_NODE_ADDR_MAX) && (num >= 1U) &&
      (num <= 123U) && (holdingRegs != NULL))
  {
    /* Convert the client channel pointer to the context structure. */
    tTbxMbClientCtx * clientCtx = (tTbxMbClientCtx *)channel;
    /* Sanity check on the context type. */
    TBX_ASSERT(clientCtx->type == TBX_MB_CLIENT_CONTEXT_TYPE);

    /* Obtain write access to the request packet. */
    tTbxMbTpPacket * txPacket = clientCtx->tpCtx->getTxPacketFcn(clientCtx->tpCtx);
    /* Should always work, unless this function is being called recursively. Only
     * continue with access for preparing the request packet.
     */
    if (txPacket != NULL)
    {
      /* Writing just a single holding register? */
      if (num == 1U)
      {
        /* Prepare the request packet. */
        txPacket->node = node;
        txPacket->pdu.code = TBX_MB_FC06_WRITE_SINGLE_REGISTER;
        txPacket->dataLen = 4U;
        /* Holding register address. */
        TbxMbCommonStoreUInt16BE(addr, &txPacket->pdu.data[0]);
        /* Holding register value. */
        TbxMbCommonStoreUInt16BE(holdingRegs[0], &txPacket->pdu.data[2]);
      }
      /* Writing multiple holding registers. */
      else
      {
        /* Determine byte count needed for storing the holding register values. */
        uint8_t byteCount = num * 2U;
        /* Prepare the request packet. */
        txPacket->node = node;
        txPacket->pdu.code = TBX_MB_FC16_WRITE_MULTIPLE_REGISTERS;
        txPacket->dataLen = byteCount + 5U;
        /* Start address. */
        TbxMbCommonStoreUInt16BE(addr, &txPacket->pdu.data[0]);
        /* Number of holding registers. */
        TbxMbCommonStoreUInt16BE(num, &txPacket->pdu.data[2]);
        /* Byte count. */
        txPacket->pdu.data[4] = byteCount;
        /* Set pointer to where the holding registers start in the request. */
        uint8_t * regValPtr = &txPacket->pdu.data[5];
        /* Store the holding register values. */
        for (uint8_t idx = 0U; idx < num; idx++)
        {
          TbxMbCommonStoreUInt16BE(holdingRegs[idx], &regValPtr[idx * 2U]);
        }
      }
      /* Determine the request type (broadcast / unicast). */
      uint8_t isBroadcast = TBX_FALSE;
      if (node == TBX_MB_TP_NODE_ADDR_BROADCAST)
      {
        isBroadcast = TBX_TRUE;
      }
      /* Transmit the request and wait for the response to a unicast request to come in
       * or the turnaround time to pass for a broadcast request.
       */
      result = TbxMbClientTransceive(clientCtx, isBroadcast);

      /* Only continue with processing the response if all is okay so far and the request
       * was unicast.
       */
      if ((result == TBX_OK) && (isBroadcast == TBX_FALSE))
      {
        /* Obtain read access to the response packet. */
        tTbxMbTpPacket * rxPacket = clientCtx->tpCtx->getRxPacketFcn(clientCtx->tpCtx);
        /* Since we just received a response packet, the packet access should always 
         * succeed. Sanity check anyways, just in case.
         */
        TBX_ASSERT(rxPacket != NULL);
        /* Only continue with packet access. */
        if (rxPacket != NULL)
        {
          /* Wrote just a single holding register? */
          if (num == 1U)
          {
            /* Check that the response came from the expected node, that it's a response
             * with the same function code (not an exception response), that the register
             * address and values are as expected and that the data length is as
             * expected.
             */
            if ((rxPacket->node != node) ||
                (rxPacket->pdu.code != TBX_MB_FC06_WRITE_SINGLE_REGISTER) ||
                (TbxMbCommonExtractUInt16BE(&rxPacket->pdu.data[0]) != addr) ||
                (TbxMbCommonExtractUInt16BE(&rxPacket->pdu.data[2]) != holdingRegs[0]) ||
                (rxPacket->dataLen != 4U))
            {
              result = TBX_ERROR;
            }
          }
          /* Wrote multiple holding registers. */
          else
          {
            /* Check that the response came from the expected node, that it's a response
             * with the same function code (not an exception response), that the register
             * start address and quantity are as expected and that the data length is as
             * expected.
             */
            if ((rxPacket->node != node) ||
                (rxPacket->pdu.code != TBX_MB_FC16_WRITE_MULTIPLE_REGISTERS) ||
                (TbxMbCommonExtractUInt16BE(&rxPacket->pdu.data[0]) != addr) ||
                (TbxMbCommonExtractUInt16BE(&rxPacket->pdu.data[2]) != num) ||
                (rxPacket->dataLen != 4U))
            {
              result = TBX_ERROR;
            }
          }
        }
        /* Could not access the response packet. */
        else
        {
          result = TBX_ERROR;
        }
        /* Inform the transport layer that were done with the rx packet and no longer
         * need access to it.
         */
        clientCtx->tpCtx->receptionDoneFcn(clientCtx->tpCtx);
      }
    }

  }      
  /* Give the result back to the caller. */
  return result;
} /*** end of TbxMbClientWriteHoldingRegs ***/


/************************************************************************************//**
** \brief     Perform diagnostic operation on the server for checking the communication
**            system.
** \param     channel Handle to the Modbus client channel for the requested operation.
** \param     node The address of the server. This parameter is transport layer
**            dependent. It is needed on RTU/ASCII, yet don't care for TCP unless it is
**            a gateway to an RTU network. If it's don't care, set it to a value of 1.
** \param     subcode Sub-function code for specifying the diagnostic operation to
**            perform. Currently supported values:
**              - TBX_MB_DIAG_SC_QUERY_DATA
**              - TBX_MB_DIAG_SC_CLEAR_COUNTERS
**              - TBX_MB_DIAG_SC_BUS_MESSAGE_COUNT
**              - TBX_MB_DIAG_SC_BUS_COMM_ERROR_COUNT
**              - TBX_MB_DIAG_SC_BUS_EXCEPTION_ERROR_COUNT
**              - TBX_MB_DIAG_SC_SERVER_MESSAGE_COUNT
**              - TBX_MB_DIAG_SC_SERVER_NO_RESPONSE_COUNT
** \param     count Location where the retrieved count value will be written to. Only
**            applicable for the subcodes that end with _COUNT.
** \return    TBX_OK if successful, TBX_ERROR otherwise.
**
****************************************************************************************/
uint8_t TbxMbClientDiagnostics(tTbxMbClient   channel,
                               uint8_t        node,
                               uint16_t       subcode,
                               uint16_t     * count)
{
  uint8_t        result      = TBX_ERROR;
  uint16_t const queryData[] = { 0xFFFFU, 0x0000U, 0xAA55U, 0x55AAU, 0x3723U };

  /* Verify the parameters. */
  TBX_ASSERT((channel != NULL) && (node <= TBX_MB_TP_NODE_ADDR_MAX) && 
             ((subcode == TBX_MB_DIAG_SC_QUERY_DATA) || 
              (subcode == TBX_MB_DIAG_SC_CLEAR_COUNTERS) ||
              (subcode == TBX_MB_DIAG_SC_BUS_MESSAGE_COUNT) ||
              (subcode == TBX_MB_DIAG_SC_BUS_COMM_ERROR_COUNT) ||
              (subcode == TBX_MB_DIAG_SC_BUS_EXCEPTION_ERROR_COUNT) ||
              (subcode == TBX_MB_DIAG_SC_SERVER_MESSAGE_COUNT) ||
              (subcode == TBX_MB_DIAG_SC_SERVER_NO_RESPONSE_COUNT)));

  /* Only continue with valid parameters. */
  if ((channel != NULL) && (node <= TBX_MB_TP_NODE_ADDR_MAX) && 
      ((subcode == TBX_MB_DIAG_SC_QUERY_DATA) || 
       (subcode == TBX_MB_DIAG_SC_CLEAR_COUNTERS) ||
       (subcode == TBX_MB_DIAG_SC_BUS_MESSAGE_COUNT) ||
       (subcode == TBX_MB_DIAG_SC_BUS_COMM_ERROR_COUNT) ||
       (subcode == TBX_MB_DIAG_SC_BUS_EXCEPTION_ERROR_COUNT) ||
       (subcode == TBX_MB_DIAG_SC_SERVER_MESSAGE_COUNT) ||
       (subcode == TBX_MB_DIAG_SC_SERVER_NO_RESPONSE_COUNT)))
  {
    /* Convert the client channel pointer to the context structure. */
    tTbxMbClientCtx * clientCtx = (tTbxMbClientCtx *)channel;
    /* Sanity check on the context type. */
    TBX_ASSERT(clientCtx->type == TBX_MB_CLIENT_CONTEXT_TYPE);

    /* Obtain write access to the request packet. */
    tTbxMbTpPacket * txPacket = clientCtx->tpCtx->getTxPacketFcn(clientCtx->tpCtx);
    /* Should always work, unless this function is being called recursively. Only
     * continue with access for preparing the request packet.
     */
    if (txPacket != NULL)
    {
      /* Prepare the request packet. */
      txPacket->node = node;
      txPacket->pdu.code = TBX_MB_FC08_DIAGNOSTICS;
      TbxMbCommonStoreUInt16BE(subcode, &txPacket->pdu.data[0]);
      /* Requested to perform a query data diagnostic operation? */
      if (subcode == TBX_MB_DIAG_SC_QUERY_DATA)
      {
        /* Write the query data for loopback testing. */
        for (uint8_t idx = 0U; idx < (sizeof(queryData)/sizeof(queryData[0])); idx++)
        {
          TbxMbCommonStoreUInt16BE(queryData[idx], &txPacket->pdu.data[2U + (idx * 2U)]);
        }
        txPacket->dataLen = ((sizeof(queryData) / sizeof(queryData[0])) * 2U) + 2U;
      }
      /* All other supported subcodes require a 16-bit zero value data field. */
      else
      {
        /* Store the data field as per the protocol. */
        TbxMbCommonStoreUInt16BE(0x0000U, &txPacket->pdu.data[2U]);
        txPacket->dataLen = 4U;
      }

      /* Determine the request type (broadcast / unicast). */
      uint8_t isBroadcast = TBX_FALSE;
      if (node == TBX_MB_TP_NODE_ADDR_BROADCAST)
      {
        isBroadcast = TBX_TRUE;
      }
      /* Transmit the request and wait for the response to a unicast request to come in
       * or the turnaround time to pass for a broadcast request.
       */
      result = TbxMbClientTransceive(clientCtx, isBroadcast);

      /* Only continue with processing the response if all is okay so far and the request
       * was unicast.
       */
      if ((result == TBX_OK) && (isBroadcast == TBX_FALSE))
      {
        /* Obtain read access to the response packet. */
        tTbxMbTpPacket * rxPacket = clientCtx->tpCtx->getRxPacketFcn(clientCtx->tpCtx);
        /* Since we just received a response packet, the packet access should always 
         * succeed. Sanity check anyways, just in case.
         */
        TBX_ASSERT(rxPacket != NULL);
        /* Only continue with packet access. */
        if (rxPacket != NULL)
        {
          /* Check that the response came from the expected node, that it's a response
           * with the same function code (not an exception response) and that it's a
           * response with the same function sub-code.
           */
          if ((rxPacket->node != node) ||
              (rxPacket->pdu.code != TBX_MB_FC08_DIAGNOSTICS) ||
              (TbxMbCommonExtractUInt16BE(&rxPacket->pdu.data[0]) != subcode))
          {
            result = TBX_ERROR;
          }
          /* Response looks valid so far. Continue with processing its data.*/
          else
          {
            if (subcode == TBX_MB_DIAG_SC_QUERY_DATA)
            {
              const uint8_t queryDataLen = sizeof(queryData) / sizeof(queryData[0]);
              /* Check the data length. */
              if (rxPacket->dataLen != ((queryDataLen * 2U) + 2U))
              {
                result = TBX_ERROR;
              }
              /* Data length okay, continue with checking its content. */
              else
              {
                /* Loop through the received query data. */
                for (uint8_t idx = 0U; idx < queryDataLen; idx++)
                {
                  uint8_t const * entryPtr = &rxPacket->pdu.data[2];
                  uint16_t entry = TbxMbCommonExtractUInt16BE(&entryPtr[idx * 2U]);
                  /* Check that its value is the same as what was sent in the request. */
                  if (entry != queryData[idx])
                  {
                    /* Flag the error and stop the loop. */
                    result = TBX_ERROR;
                    break;
                  }
                }
              }
            }
            else if (subcode == TBX_MB_DIAG_SC_CLEAR_COUNTERS)
            {
              /* Check the data length. */
              if (rxPacket->dataLen != 4U)
              {
                result = TBX_ERROR;
              }
              /* Data length okay, continue with checking its content. */
              else
              {
                uint8_t const * dataValPtr = &rxPacket->pdu.data[2];
                uint16_t dataVal = TbxMbCommonExtractUInt16BE(dataValPtr);
                /* Check that the value is as expected. */
                if (dataVal != 0x0000U)
                {
                  result = TBX_ERROR;
                }
              }
            }
            /* subcode for reading a count. */
            else
            {
              /* Check the data length. */
              if (rxPacket->dataLen != 4U)
              {
                result = TBX_ERROR;
              }
              /* Data length okay, continue with extracting the count value. */
              else
              {
                uint8_t const * countValPtr = &rxPacket->pdu.data[2];
                uint16_t countVal = TbxMbCommonExtractUInt16BE(countValPtr);
                /* Verify that count parameter. */
                TBX_ASSERT(count != NULL);
                /* Only continue with a valid count parameter. */
                if (count != NULL)
                {
                  /* Store the count value. */
                  *count = countVal;
                }
                else
                {
                  /* The an error due to an invalid count parameter. */
                  result = TBX_ERROR;
                }
              }
            }
          }
        }
        /* Could not access the response packet. */
        else
        {
          result = TBX_ERROR;
        }
        /* Inform the transport layer that were done with the rx packet and no longer
         * need access to it.
         */
        clientCtx->tpCtx->receptionDoneFcn(clientCtx->tpCtx);
      }
    }
  }
  /* Give the result back to the caller. */
  return result;
} /*** end of TbxMbClientDiagnostics ***/


/************************************************************************************//**
** \brief     Send a custom function code PDU to the server and receive its response PDU.
**            Thanks to this functionality, the user can support Modbus function codes
**            that are either currently not supported or user defined extensions.
** \details   The "txPdu" and "rxPdu" parameters are pointers to the byte array of the
**            PDU. The first byte (i.e. txPdu[0]) contains the function code, followed by
**            its data bytes. When calling this function, set the "len" parameter to the
**            length of the "txPdu". This function updates the "len" parameter with the
**            length of the received PDU, which it stores in "rxPdu".
** \example   Example of manually sending the "Write Single Register 0x06" function code
**            to a node with address 10 (0x0A) for setting the holding register at
**            address 40000 to a value of 127:
**
**              uint8_t  requestPdu[TBX_MB_TP_PDU_MAX_LEN];
**              uint8_t  responsePdu[TBX_MB_TP_PDU_MAX_LEN];
**              uint8_t  pduLen;
**              uint16_t holdingRegAddr = 40000U;
**              uint16_t holdingRegValue = 127U;
**
**              requestPdu[0] = TBX_MB_FC06_WRITE_SINGLE_REGISTER;
**              requestPdu[1] = (uint8_t)(holdingRegAddr >> 8U);
**              requestPdu[2] = (uint8_t) holdingRegAddr;
**              requestPdu[3] = (uint8_t)(holdingRegValue >> 8U);
**              requestPdu[4] = (uint8_t) holdingRegValue;
**              pduLen = 5U;
**
**              TbxMbClientCustomFunction(modbusClient, 0x0A, requestPdu, responsePdu, 
**                                        &pduLen);
**
** \param     channel Handle to the Modbus client channel for the requested operation.
** \param     node The address of the server. This parameter is transport layer
**            dependent. It is needed on RTU/ASCII, yet don't care for TCP unless it is
**            a gateway to an RTU network. If it's don't care, set it to a value of 1.
** \param     txPdu Pointer to a byte array with the PDU to transmit.
** \param     rxPdu Pointer to a byte array with the received response PDU.
** \param     len Pointer to the PDU length, including the function code.
** \return    TBX_OK if successful, TBX_ERROR otherwise.
**
****************************************************************************************/
uint8_t TbxMbClientCustomFunction (tTbxMbClient         channel,
                                   uint8_t              node,
                                   uint8_t      const * txPdu,
                                   uint8_t            * rxPdu,
                                   uint8_t            * len)
{
  uint8_t result = TBX_ERROR;

  /* Verify the parameters. */
  TBX_ASSERT((channel != NULL) && (node <= TBX_MB_TP_NODE_ADDR_MAX) && (txPdu != NULL) &&
             (rxPdu != NULL) && (len != NULL));

  /* Only continue with valid parameters. */
  if ((channel != NULL) && (node <= TBX_MB_TP_NODE_ADDR_MAX) && (txPdu != NULL) &&
      (rxPdu != NULL) && (len != NULL))
  {
    /* Convert the client channel pointer to the context structure. */
    tTbxMbClientCtx * clientCtx = (tTbxMbClientCtx *)channel;
    /* Sanity check on the context type. */
    TBX_ASSERT(clientCtx->type == TBX_MB_CLIENT_CONTEXT_TYPE);

    /* Obtain write access to the request packet. */
    tTbxMbTpPacket * txPacket = clientCtx->tpCtx->getTxPacketFcn(clientCtx->tpCtx);
    /* Should always work, unless this function is being called recursively. Only
     * continue with access for preparing the request packet and with a valid packet
     * length. It should at least have a PDU function code.
     */
    if ((txPacket != NULL) && (*len > 0U))
    {
      /* Prepare the request packet. */
      txPacket->node = node;
      txPacket->pdu.code = txPdu[0];
      txPacket->dataLen = *len - 1U;
      for (uint8_t idx = 0U; idx < txPacket->dataLen; idx++)
      {
        txPacket->pdu.data[idx] = txPdu[idx + 1U];
      }

      /* Determine the request type (broadcast / unicast). */
      uint8_t isBroadcast = TBX_FALSE;
      if (node == TBX_MB_TP_NODE_ADDR_BROADCAST)
      {
        isBroadcast = TBX_TRUE;
      }
      
      /* Transmit the request and wait for the response to a unicast request to come in
       * or the turnaround time to pass for a broadcast request.
       */
      result = TbxMbClientTransceive(clientCtx, isBroadcast);

      /* Initialize the length of the response PDU to zero. This default indicates that
       * no response was received. This is the case in the request was a broadcast one or
       * it the response was not valid. If will be updated later on, if a valid response
       * was received.
       */
      *len = 0U;

      /* Only continue with processing the response if all is okay so far and the request
       * was unicast.
       */
      if ((result == TBX_OK) && (isBroadcast == TBX_FALSE))
      {
        /* Obtain read access to the response packet. */
        tTbxMbTpPacket * rxPacket = clientCtx->tpCtx->getRxPacketFcn(clientCtx->tpCtx);
        /* Since we just received a response packet, the packet access should always 
         * succeed. Sanity check anyways, just in case.
         */
        TBX_ASSERT(rxPacket != NULL);
        /* Only continue with packet access. */
        if (rxPacket != NULL)
        {
          /* Check that the response came from the expected node. */
          if (rxPacket->node != node)
          {
            result = TBX_ERROR;
          }
          /* Response content valid. Copy its data. */
          else
          {
            /* Set the length, including the function code. */
            *len = rxPacket->dataLen + 1U;
            /* Set the function code. */
            rxPdu[0] = rxPacket->pdu.code;
            /* Copy the packet data. */
            for (uint8_t idx = 0U; idx < rxPacket->dataLen; idx++)
            {
              rxPdu[idx + 1U] = rxPacket->pdu.data[idx];
            }
          }
        }
        /* Could not access the response packet. */
        else
        {
          result = TBX_ERROR;
        }
        /* Inform the transport layer that were done with the rx packet and no longer
         * need access to it.
         */
        clientCtx->tpCtx->receptionDoneFcn(clientCtx->tpCtx);
      }
    }
  }
  /* Give the result back to the caller. */
  return result;
} /*** end of TbxMbClientCustomFunction ***/


/*********************************** end of tbxmb_client.c *****************************/
