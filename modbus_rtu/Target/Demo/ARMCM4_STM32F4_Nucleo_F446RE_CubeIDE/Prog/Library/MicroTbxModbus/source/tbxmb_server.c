/************************************************************************************//**
* \file         tbxmb_server.c
* \brief        Modbus server source file.
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
#include "tbxmb_server_private.h"                /* MicroTBX-Modbus server private     */


/****************************************************************************************
* Macro definitions
****************************************************************************************/
/** \brief Unique context type to identify a context as being a server channel. */
#define TBX_MB_SERVER_CONTEXT_TYPE     (37U)


/****************************************************************************************
* Function prototypes
****************************************************************************************/
static void TbxMbServerProcessEvent          (tTbxMbEvent           * event);

static void TbxMbServerFC01ReadCoils         (tTbxMbServerCtx       * context,
                                              tTbxMbTpPacket  const * rxPacket,
                                              tTbxMbTpPacket        * txPacket);

static void TbxMbServerFC02ReadInputs        (tTbxMbServerCtx       * context,
                                              tTbxMbTpPacket  const * rxPacket,
                                              tTbxMbTpPacket        * txPacket);

static void TbxMbServerFC03ReadHoldingRegs   (tTbxMbServerCtx       * context,
                                              tTbxMbTpPacket  const * rxPacket,
                                              tTbxMbTpPacket        * txPacket);

static void TbxMbServerFC04ReadInputRegs     (tTbxMbServerCtx       * context,
                                              tTbxMbTpPacket  const * rxPacket,
                                              tTbxMbTpPacket        * txPacket);

static void TbxMbServerFC05WriteSingleCoil   (tTbxMbServerCtx       * context,
                                              tTbxMbTpPacket  const * rxPacket,
                                              tTbxMbTpPacket        * txPacket);

static void TbxMbServerFC06WriteSingleReg    (tTbxMbServerCtx       * context,
                                              tTbxMbTpPacket  const * rxPacket,
                                              tTbxMbTpPacket        * txPacket);

static void TbxMbServerFC08Diagnostics       (tTbxMbServerCtx       * context,
                                              tTbxMbTpPacket  const * rxPacket,
                                              tTbxMbTpPacket        * txPacket);

static void TbxMbServerFC15WriteMultipleCoils(tTbxMbServerCtx       * context,
                                              tTbxMbTpPacket  const * rxPacket,
                                              tTbxMbTpPacket        * txPacket);

static void TbxMbServerFC16WriteMultipleRegs (tTbxMbServerCtx       * context,
                                              tTbxMbTpPacket  const * rxPacket,
                                              tTbxMbTpPacket        * txPacket);


/************************************************************************************//**
** \brief     Creates a Modbus server channel object and assigns the specified Modbus
**            transport layer to the channel for packet transmission and reception.
** \param     transport Handle to a previously created Modbus transport layer object to
**            assign to the channel.
** \return    Handle to the newly created Modbus server channel object if successful,
**            NULL otherwise.
**
****************************************************************************************/
tTbxMbServer TbxMbServerCreate(tTbxMbTp transport)
{
  tTbxMbServer result = NULL;

  /* Verify parameters. */
  TBX_ASSERT(transport != NULL);

  /* Only continue with valid parameters. */
  if (transport != NULL)
  {
    /* Allocate memory for the new channel context. */
    tTbxMbServerCtx * newServerCtx = TbxMemPoolAllocate(sizeof(tTbxMbServerCtx));
    /* Automatically increase the memory pool, if it was too small. */
    if (newServerCtx == NULL)
    {
      /* No need to check the return value, because if it failed, the following
       * allocation fails too, which is verified later on.
       */
      (void)TbxMemPoolCreate(1U, sizeof(tTbxMbServerCtx));
      newServerCtx = TbxMemPoolAllocate(sizeof(tTbxMbServerCtx));      
    }
    /* Verify memory allocation of the channel context. */
    TBX_ASSERT(newServerCtx != NULL);
    /* Only continue if the memory allocation succeeded. */
    if (newServerCtx != NULL)
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
      newServerCtx->type = TBX_MB_SERVER_CONTEXT_TYPE;
      newServerCtx->instancePtr = NULL;
      newServerCtx->pollFcn = NULL;
      newServerCtx->processFcn = TbxMbServerProcessEvent;
      newServerCtx->readInputFcn = NULL;
      newServerCtx->readCoilFcn = NULL;
      newServerCtx->writeCoilFcn = NULL;
      newServerCtx->readInputRegFcn = NULL;
      newServerCtx->readHoldingRegFcn = NULL;
      newServerCtx->writeHoldingRegFcn = NULL;
      newServerCtx->customFunctionFcn = NULL;
      newServerCtx->tpCtx = tpCtx;
      newServerCtx->tpCtx->channelCtx = newServerCtx;
      newServerCtx->tpCtx->isClient = TBX_FALSE;
      /* Update the result. */
      result = newServerCtx;
    }
  }
  /* Give the result back to the caller. */
  return result;
} /*** end of TbxMbServerCreate ****/


/************************************************************************************//**
** \brief     Releases a Modbus server channel object, previously created with
**            TbxMbServerCreate().
** \param     channel Handle to the Modbus server channel object to release.
**
****************************************************************************************/
void TbxMbServerFree(tTbxMbServer channel)
{
  /* Verify parameters. */
  TBX_ASSERT(channel != NULL);

  /* Only continue with valid parameters. */
  if (channel != NULL)
  {
    /* Convert the server channel pointer to the context structure. */
    tTbxMbServerCtx * serverCtx = (tTbxMbServerCtx *)channel;
    /* Sanity check on the context type. */
    TBX_ASSERT(serverCtx->type == TBX_MB_SERVER_CONTEXT_TYPE);
    /* Remove crosslink between the channel and the transport layer. */
    TbxCriticalSectionEnter();
    serverCtx->tpCtx->channelCtx = NULL;
    serverCtx->tpCtx = NULL;
    /* Invalidate the context to protect it from accidentally being used afterwards. */
    serverCtx->type = 0U;
    serverCtx->pollFcn = NULL;
    serverCtx->processFcn = NULL;
    TbxCriticalSectionExit();
    /* Give the channel context back to the memory pool. */
    TbxMemPoolRelease(serverCtx);
  }
} /*** end of TbxMbServerFree ***/


/************************************************************************************//**
** \brief     Registers the callback function that this server calls, whenever a client
**            requests the reading of a specific discrete input.
** \param     channel Handle to the Modbus server channel object.
** \param     callback Pointer to the callback function.
**
****************************************************************************************/
void TbxMbServerSetCallbackReadInput(tTbxMbServer          channel,
                                     tTbxMbServerReadInput callback)
{
  /* Verify parameters. */
  TBX_ASSERT((channel != NULL) && (callback != NULL));

  /* Only continue with valid parameters. */
  if ((channel != NULL) && (callback != NULL))
  {
    /* Convert the server channel pointer to the context structure. */
    tTbxMbServerCtx * serverCtx = (tTbxMbServerCtx *)channel;
    /* Sanity check on the context type. */
    TBX_ASSERT(serverCtx->type == TBX_MB_SERVER_CONTEXT_TYPE);
    /* Store the callback function pointer. */
    TbxCriticalSectionEnter();
    serverCtx->readInputFcn = callback;
    TbxCriticalSectionExit();
  }
} /*** end of TbxMbServerSetCallbackReadInput ***/


/************************************************************************************//**
** \brief     Registers the callback function that this server calls, whenever a client
**            requests the reading of a specific coil.
** \param     channel Handle to the Modbus server channel object.
** \param     callback Pointer to the callback function.
**
****************************************************************************************/
void TbxMbServerSetCallbackReadCoil(tTbxMbServer         channel,
                                    tTbxMbServerReadCoil callback)
{
  /* Verify parameters. */
  TBX_ASSERT((channel != NULL) && (callback != NULL));

  /* Only continue with valid parameters. */
  if ((channel != NULL) && (callback != NULL))
  {
    /* Convert the server channel pointer to the context structure. */
    tTbxMbServerCtx * serverCtx = (tTbxMbServerCtx *)channel;
    /* Sanity check on the context type. */
    TBX_ASSERT(serverCtx->type == TBX_MB_SERVER_CONTEXT_TYPE);
    /* Store the callback function pointer. */
    TbxCriticalSectionEnter();
    serverCtx->readCoilFcn = callback;
    TbxCriticalSectionExit();
  }
} /*** end of TbxMbServerSetCallbackReadCoil ***/


/************************************************************************************//**
** \brief     Registers the callback function that this server calls, whenever a client
**            requests the writing of a specific coil.
** \param     channel Handle to the Modbus server channel object.
** \param     callback Pointer to the callback function.
**
****************************************************************************************/
void TbxMbServerSetCallbackWriteCoil(tTbxMbServer          channel,
                                     tTbxMbServerWriteCoil callback)
{
  /* Verify parameters. */
  TBX_ASSERT((channel != NULL) && (callback != NULL));

  /* Only continue with valid parameters. */
  if ((channel != NULL) && (callback != NULL))
  {
    /* Convert the server channel pointer to the context structure. */
    tTbxMbServerCtx * serverCtx = (tTbxMbServerCtx *)channel;
    /* Sanity check on the context type. */
    TBX_ASSERT(serverCtx->type == TBX_MB_SERVER_CONTEXT_TYPE);
    /* Store the callback function pointer. */
    TbxCriticalSectionEnter();
    serverCtx->writeCoilFcn = callback;
    TbxCriticalSectionExit();
  }
} /*** end of TbxMbServerSetCallbackWriteCoil ***/


/************************************************************************************//**
** \brief     Registers the callback function that this server calls, whenever a client
**            requests the reading of a specific input register.
** \param     channel Handle to the Modbus server channel object.
** \param     callback Pointer to the callback function.
**
****************************************************************************************/
void TbxMbServerSetCallbackReadInputReg(tTbxMbServer             channel,
                                        tTbxMbServerReadInputReg callback)
{
  /* Verify parameters. */
  TBX_ASSERT((channel != NULL) && (callback != NULL));

  /* Only continue with valid parameters. */
  if ((channel != NULL) && (callback != NULL))
  {
    /* Convert the server channel pointer to the context structure. */
    tTbxMbServerCtx * serverCtx = (tTbxMbServerCtx *)channel;
    /* Sanity check on the context type. */
    TBX_ASSERT(serverCtx->type == TBX_MB_SERVER_CONTEXT_TYPE);
    /* Store the callback function pointer. */
    TbxCriticalSectionEnter();
    serverCtx->readInputRegFcn = callback;
    TbxCriticalSectionExit();
  }
} /*** end of TbxMbServerSetCallbackReadInputReg ***/


/************************************************************************************//**
** \brief     Registers the callback function that this server calls, whenever a client
**            requests the reading of a specific holding register.
** \param     channel Handle to the Modbus server channel object.
** \param     callback Pointer to the callback function.
**
****************************************************************************************/
void TbxMbServerSetCallbackReadHoldingReg(tTbxMbServer               channel,
                                          tTbxMbServerReadHoldingReg callback)
{
  /* Verify parameters. */
  TBX_ASSERT((channel != NULL) && (callback != NULL));

  /* Only continue with valid parameters. */
  if ((channel != NULL) && (callback != NULL))
  {
    /* Convert the server channel pointer to the context structure. */
    tTbxMbServerCtx * serverCtx = (tTbxMbServerCtx *)channel;
    /* Sanity check on the context type. */
    TBX_ASSERT(serverCtx->type == TBX_MB_SERVER_CONTEXT_TYPE);
    /* Store the callback function pointer. */
    TbxCriticalSectionEnter();
    serverCtx->readHoldingRegFcn = callback;
    TbxCriticalSectionExit();
  }
} /*** end of TbxMbServerSetCallbackReadHoldingReg ***/


/************************************************************************************//**
** \brief     Registers the callback function that this server calls, whenever a client
**            requests the writing of a specific holding register.
** \param     channel Handle to the Modbus server channel object.
** \param     callback Pointer to the callback function.
**
****************************************************************************************/
void TbxMbServerSetCallbackWriteHoldingReg(tTbxMbServer                channel,
                                           tTbxMbServerWriteHoldingReg callback)
{
  /* Verify parameters. */
  TBX_ASSERT((channel != NULL) && (callback != NULL));

  /* Only continue with valid parameters. */
  if ((channel != NULL) && (callback != NULL))
  {
    /* Convert the server channel pointer to the context structure. */
    tTbxMbServerCtx * serverCtx = (tTbxMbServerCtx *)channel;
    /* Sanity check on the context type. */
    TBX_ASSERT(serverCtx->type == TBX_MB_SERVER_CONTEXT_TYPE);
    /* Store the callback function pointer. */
    TbxCriticalSectionEnter();
    serverCtx->writeHoldingRegFcn = callback;
    TbxCriticalSectionExit();
  }
} /*** end of TbxMbServerSetCallbackWriteHoldingReg ***/


/************************************************************************************//**
** \brief     Registers the callback function that this server calls, whenever it
**            received a PDU containing a function code not currently supported. With the
**            aid of this callback function the user can implement support for new
**            function codes.
** \param     channel Handle to the Modbus server channel object.
** \param     callback Pointer to the callback function.
**
****************************************************************************************/
void TbxMbServerSetCallbackCustomFunction(tTbxMbServer               channel,
                                          tTbxMbServerCustomFunction callback)
{
  /* Verify parameters. */
  TBX_ASSERT((channel != NULL) && (callback != NULL));

  /* Only continue with valid parameters. */
  if ((channel != NULL) && (callback != NULL))
  {
    /* Convert the server channel pointer to the context structure. */
    tTbxMbServerCtx * serverCtx = (tTbxMbServerCtx *)channel;
    /* Sanity check on the context type. */
    TBX_ASSERT(serverCtx->type == TBX_MB_SERVER_CONTEXT_TYPE);
    /* Store the callback function pointer. */
    TbxCriticalSectionEnter();
    serverCtx->customFunctionFcn = callback;
    TbxCriticalSectionExit();
  }
} /*** end of TbxMbServerSetCallbackCustomFunction ***/


/************************************************************************************//**
** \brief     Event processing function that is automatically called when an event for
**            this server channel object was received in TbxMbEventTask().
** \param     event Pointer to the event to process. Note that the event->context points
**            to the handle of the Modbus server channel object.
**
****************************************************************************************/
static void TbxMbServerProcessEvent(tTbxMbEvent * event)
{
  /* Verify parameters. */
  TBX_ASSERT(event != NULL);

  /* Only continue with valid parameters. */
  if (event != NULL)
  {
    /* Sanity check the context. */
    TBX_ASSERT(event->context != NULL);
    /* Convert the event context to the server channel context structure. */
    tTbxMbServerCtx * serverCtx = (tTbxMbServerCtx *)event->context;
    /* Make sure the context is valid. */
    TBX_ASSERT(serverCtx != NULL);
    /* Only continue with a valid context. */
    if (serverCtx != NULL)
    {
      /* Sanity check on the context type. */
      TBX_ASSERT(serverCtx->type == TBX_MB_SERVER_CONTEXT_TYPE);
      /* Filter on the event identifier. */
      switch (event->id)
      {
        case TBX_MB_EVENT_ID_PDU_RECEIVED:
        {
          uint8_t okayToSendResponse = TBX_FALSE;
          /* Obtain read access to the newly received packet and write access to the
           * response packet. 
           */
          tTbxMbTpPacket * rxPacket = serverCtx->tpCtx->getRxPacketFcn(serverCtx->tpCtx);
          tTbxMbTpPacket * txPacket = serverCtx->tpCtx->getTxPacketFcn(serverCtx->tpCtx);
          /* Since we're requested to process a newly received PDU, these packet accesses
           * should always succeed. Sanity check anyways, just in case.
           */
          TBX_ASSERT((rxPacket != NULL) && (txPacket != NULL));
          /* Only continue with packet access. */
          if ((rxPacket != NULL) && (txPacket != NULL))
          {
            /* Update flag that we can actually send a response, now that we know we 
             * have access to txPacket.
             */
            okayToSendResponse = TBX_TRUE;
            /* Prepare the response packet function code. */
            txPacket->pdu.code = rxPacket->pdu.code;
            /* Filter on the function code. */
            switch (rxPacket->pdu.code)
            {
              /* ---------------- FC01 - Read Coils ---------------------------------- */
              case TBX_MB_FC01_READ_COILS:
              {
                TbxMbServerFC01ReadCoils(serverCtx, rxPacket, txPacket);
              }
              break;

              /* ---------------- FC02 - Read Discrete Inputs ------------------------ */
              case TBX_MB_FC02_READ_DISCRETE_INPUTS:
              {
                TbxMbServerFC02ReadInputs(serverCtx, rxPacket, txPacket);
              }
              break;

              /* ---------------- FC03 - Read Holding Registers ---------------------- */
              case TBX_MB_FC03_READ_HOLDING_REGISTERS:
              {
                TbxMbServerFC03ReadHoldingRegs(serverCtx, rxPacket, txPacket);
              }
              break;

              /* ---------------- FC04 - Read Input Registers ------------------------ */
              case TBX_MB_FC04_READ_INPUT_REGISTERS:
              {
                TbxMbServerFC04ReadInputRegs(serverCtx, rxPacket, txPacket);
              }
              break;

              /* ---------------- FC05 - Write Single Coil --------------------------- */
              case TBX_MB_FC05_WRITE_SINGLE_COIL:
              {
                TbxMbServerFC05WriteSingleCoil(serverCtx, rxPacket, txPacket);
              }
              break;

              /* ---------------- FC06 - Write Single Register ----------------------- */
              case TBX_MB_FC06_WRITE_SINGLE_REGISTER:
              {
                TbxMbServerFC06WriteSingleReg(serverCtx, rxPacket, txPacket);
              }
              break;

              /* ---------------- FC08 - Diagnostics --------------------------------- */
              case TBX_MB_FC08_DIAGNOSTICS:
              {
                TbxMbServerFC08Diagnostics(serverCtx, rxPacket, txPacket);
              }
              break;

              /* ---------------- FC15 - Write Multiple Coils ------------------------ */
              case TBX_MB_FC15_WRITE_MULTIPLE_COILS:
              {
                TbxMbServerFC15WriteMultipleCoils(serverCtx, rxPacket, txPacket);
              }
              break;

              /* ---------------- FC16 - Write Multiple Registers -------------------- */
              case TBX_MB_FC16_WRITE_MULTIPLE_REGISTERS:
              {
                TbxMbServerFC16WriteMultipleRegs(serverCtx, rxPacket, txPacket);
              }
              break;

              /* ---------------- Unsupported function code -------------------------- */
              default:
              {
                uint8_t handled = TBX_FALSE;

                /* Is a custom function code callback configured? */
                if (serverCtx->customFunctionFcn != NULL)
                {
                  /* Prepare callback parameters. */
                  uint8_t const * rxPdu  = &rxPacket->pdu.code;
                  uint8_t       * txPdu  = &txPacket->pdu.code;
                  uint8_t         pduLen = rxPacket->dataLen + 1U;
                  /* Call the custom function code callback. */
                  handled = serverCtx->customFunctionFcn(serverCtx, rxPdu, txPdu, 
                                                         &pduLen);
                  /* Did the callback process the PDU and prepare a response? */
                  if (handled == TBX_TRUE)
                  {
                    /* Set the response data length. */
                    txPacket->dataLen = pduLen - 1U;
                  }
                }
                /* Did the custom function code callback not handle the PDU? */
                if (handled == TBX_FALSE)
                {
                  /* This function code is currently not supported. */
                  txPacket->pdu.code |= TBX_MB_FC_EXCEPTION_MASK;
                  txPacket->pdu.data[0] = TBX_MB_EC01_ILLEGAL_FUNCTION;
                  txPacket->dataLen = 1U;
                }
              }
              break;
            }
          }
          /* Inform the transport layer that were done with the rx packet and no longer
           * need access to it.
           */
          serverCtx->tpCtx->receptionDoneFcn(serverCtx->tpCtx);
          /* Request the transport layer to transmit the response. Note that
           * transmitFcn() should only be called after calling receptionDoneFcn().
           */
          if (okayToSendResponse == TBX_TRUE)
          {
            (void)serverCtx->tpCtx->transmitFcn(serverCtx->tpCtx);
          }
        }
        break;
      
        case TBX_MB_EVENT_ID_PDU_TRANSMITTED:
        {
          /* At this point no additional event handling is needed on this channel upon
           * PDU transmission completion.
           */
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
} /*** end of TbxMbServerProcessEvent ***/


/************************************************************************************//**
** \brief     Handles a newly received PDU for function code 1 - Read Coils.
** \details   Note that this function is called at a time that txPacket->code is already
**            prepared. Also note that txPacket->node should not be touched here.
** \param     context Pointer to the Modbus server channel context.
** \param     rxPacket Received PDU packet with MUX access.
** \param     txPacket Storage for the PDU response packet with MUX access.
**
****************************************************************************************/
static void TbxMbServerFC01ReadCoils(tTbxMbServerCtx       * context,
                                     tTbxMbTpPacket  const * rxPacket,
                                     tTbxMbTpPacket        * txPacket)
{
  /* Verify parameters. */
  TBX_ASSERT((context != NULL) && (rxPacket != NULL) && (txPacket != NULL));

  /* Only continue with valid parameters. */
  if ((context != NULL) && (rxPacket != NULL) && (txPacket != NULL))
  {
    /* Read out request packet parameters. */
    uint16_t startAddr = TbxMbCommonExtractUInt16BE(&rxPacket->pdu.data[0]);
    uint16_t numCoils  = TbxMbCommonExtractUInt16BE(&rxPacket->pdu.data[2]);

    /* Check if a callback function was registered. */
    if (context->readCoilFcn == NULL)
    {
      /* Prepare exception response. */
      txPacket->pdu.code |= TBX_MB_FC_EXCEPTION_MASK;
      txPacket->pdu.data[0] = TBX_MB_EC01_ILLEGAL_FUNCTION;
      txPacket->dataLen = 1U;
    }
    /* Check if the quantity of coils is invalid. */
    else if ((numCoils < 1U) || (numCoils > 2000U))
    {
      /* Prepare exception response. */
      txPacket->pdu.code |= TBX_MB_FC_EXCEPTION_MASK;
      txPacket->pdu.data[0] = TBX_MB_EC03_ILLEGAL_DATA_VALUE;
      txPacket->dataLen = 1U;
    }
    /* All is good for further processing. */
    else
    {
      /* Determine the number of bytes needed to hold all the coil bits. The cast to
       * U8 is okay, because we know that numCoils is <= 2000.
       */
      uint8_t numBytes = (uint8_t)(numCoils / 8U);
      if ((numCoils % 8U) != 0U)
      {
        numBytes++;
      }
      /* Store byte count in the response and prepare the data length. */
      txPacket->pdu.data[0] = numBytes;
      txPacket->dataLen = txPacket->pdu.data[0] + 1U;
      /* Prepare loop indices that aid with storing the coil bits. */
      uint8_t   bitIdx  = 0U;
      uint8_t   byteIdx = 0U;
      /* Initialize byte array pointer for writing the coil bits in the response and
       * already initialize the first byte to all zero (coils OFF) bits.
       */
      uint8_t * coilData = &txPacket->pdu.data[1];
      coilData[0] = 0U;
      /* Loop through all the coils. */
      for (uint16_t idx = 0U; idx < numCoils; idx++)
      {
        uint8_t            coilValue = TBX_OFF;
        tTbxMbServerResult srvResult;
        /* Obtain coil value. */
        srvResult = context->readCoilFcn(context, startAddr + idx, &coilValue);
        /* No exception reported? */
        if (srvResult == TBX_MB_SERVER_OK)
        {
          /* Store the coil value in the response. Note that the coil bits in a byte are
           * initialized to all zeroes, so only update if a coil is in the ON state.
           */
          if (coilValue != TBX_OFF)
          {
            coilData[byteIdx] |= (1U << bitIdx);
          }
          /* Update the bit index. */
          bitIdx++;
          /* Time to move to the next byte? */
          if (bitIdx == 8U)
          {
            /* Reset the bit index, increment the byte index and initialize the byte to
             * all zero (coils OFF) bits.
             */
            bitIdx = 0U;
            byteIdx++;
            coilData[byteIdx] = 0U;
          }
        }
        /* Exception detected. */
        else
        {
          /* Prepare exception response. */
          txPacket->pdu.code |= TBX_MB_FC_EXCEPTION_MASK;
          if (srvResult == TBX_MB_SERVER_ERR_ILLEGAL_DATA_ADDR)
          {
            txPacket->pdu.data[0] = TBX_MB_EC02_ILLEGAL_DATA_ADDRESS;
          }
          else
          {
            txPacket->pdu.data[0] = TBX_MB_EC04_SERVER_DEVICE_FAILURE;
          }
          txPacket->dataLen = 1U;
          /* Stop looping. */
          break;
        }
      }
    }
  }
} /*** end of TbxMbServerFC01ReadCoils ***/


/************************************************************************************//**
** \brief     Handles a newly received PDU for function code 2 - Read Discrete Inputs.
** \details   Note that this function is called at a time that txPacket->code is already
**            prepared. Also note that txPacket->node should not be touched here.
** \param     context Pointer to the Modbus server channel context.
** \param     rxPacket Received PDU packet with MUX access.
** \param     txPacket Storage for the PDU response packet with MUX access.
**
****************************************************************************************/
static void TbxMbServerFC02ReadInputs(tTbxMbServerCtx       * context,
                                      tTbxMbTpPacket  const * rxPacket,
                                      tTbxMbTpPacket        * txPacket)
{
  /* Verify parameters. */
  TBX_ASSERT((context != NULL) && (rxPacket != NULL) && (txPacket != NULL));

  /* Only continue with valid parameters. */
  if ((context != NULL) && (rxPacket != NULL) && (txPacket != NULL))
  {
    /* Read out request packet parameters. */
    uint16_t startAddr = TbxMbCommonExtractUInt16BE(&rxPacket->pdu.data[0]);
    uint16_t numInputs = TbxMbCommonExtractUInt16BE(&rxPacket->pdu.data[2]);

    /* Check if a callback function was registered. */
    if (context->readInputFcn == NULL)
    {
      /* Prepare exception response. */
      txPacket->pdu.code |= TBX_MB_FC_EXCEPTION_MASK;
      txPacket->pdu.data[0] = TBX_MB_EC01_ILLEGAL_FUNCTION;
      txPacket->dataLen = 1U;
    }
    /* Check if the quantity of inputs is invalid. */
    else if ((numInputs < 1U) || (numInputs > 2000U))
    {
      /* Prepare exception response. */
      txPacket->pdu.code |= TBX_MB_FC_EXCEPTION_MASK;
      txPacket->pdu.data[0] = TBX_MB_EC03_ILLEGAL_DATA_VALUE;
      txPacket->dataLen = 1U;
    }
    /* All is good for further processing. */
    else
    {
      /* Determine the number of bytes needed to hold all the input bits. The cast to
       * U8 is okay, because we know that numInputs is <= 2000.
       */
      uint8_t numBytes = (uint8_t)(numInputs / 8U);
      if ((numInputs % 8U) != 0U)
      {
        numBytes++;
      }
      /* Store byte count in the response and prepare the data length. */
      txPacket->pdu.data[0] = numBytes;
      txPacket->dataLen = txPacket->pdu.data[0] + 1U;
      /* Prepare loop indices that aid with storing the input bits. */
      uint8_t   bitIdx  = 0U;
      uint8_t   byteIdx = 0U;
      /* Initialize byte array pointer for writing the input bits in the response and
       * already initialize the first byte to all zero (input OFF) bits.
       */
      uint8_t * inputData = &txPacket->pdu.data[1];
      inputData[0] = 0U;
      /* Loop through all the inputs. */
      for (uint16_t idx = 0U; idx < numInputs; idx++)
      {
        uint8_t            inputValue = TBX_OFF;
        tTbxMbServerResult srvResult;
        /* Obtain input value. */
        srvResult = context->readInputFcn(context, startAddr + idx, &inputValue);
        /* No exception reported? */
        if (srvResult == TBX_MB_SERVER_OK)
        {
          /* Store the input value in the response. Note that the input bits in a byte
           * are initialized to all zeroes, so only update if an input is in the ON
           * state.
           */
          if (inputValue != TBX_OFF)
          {
            inputData[byteIdx] |= (1U << bitIdx);
          }
          /* Update the bit index. */
          bitIdx++;
          /* Time to move to the next byte? */
          if (bitIdx == 8U)
          {
            /* Reset the bit index, increment the byte index and initialize the byte to
             * all zero (input OFF) bits.
             */
            bitIdx = 0U;
            byteIdx++;
            inputData[byteIdx] = 0U;
          }
        }
        /* Exception detected. */
        else
        {
          /* Prepare exception response. */
          txPacket->pdu.code |= TBX_MB_FC_EXCEPTION_MASK;
          if (srvResult == TBX_MB_SERVER_ERR_ILLEGAL_DATA_ADDR)
          {
            txPacket->pdu.data[0] = TBX_MB_EC02_ILLEGAL_DATA_ADDRESS;
          }
          else
          {
            txPacket->pdu.data[0] = TBX_MB_EC04_SERVER_DEVICE_FAILURE;
          }
          txPacket->dataLen = 1U;
          /* Stop looping. */
          break;
        }
      }
    }
  }
} /*** end of TbxMbServerFC02ReadInputs ***/


/************************************************************************************//**
** \brief     Handles a newly received PDU for function code 3 - Read Holding Registers.
** \details   Note that this function is called at a time that txPacket->code is already
**            prepared. Also note that txPacket->node should not be touched here.
** \param     context Pointer to the Modbus server channel context.
** \param     rxPacket Received PDU packet with MUX access.
** \param     txPacket Storage for the PDU response packet with MUX access.
**
****************************************************************************************/
static void TbxMbServerFC03ReadHoldingRegs(tTbxMbServerCtx       * context,
                                           tTbxMbTpPacket  const * rxPacket,
                                           tTbxMbTpPacket        * txPacket)
{
  /* Verify parameters. */
  TBX_ASSERT((context != NULL) && (rxPacket != NULL) && (txPacket != NULL));

  /* Only continue with valid parameters. */
  if ((context != NULL) && (rxPacket != NULL) && (txPacket != NULL))
  {
    /* Read out request packet parameters. */
    uint16_t startAddr = TbxMbCommonExtractUInt16BE(&rxPacket->pdu.data[0]);
    uint16_t numRegs   = TbxMbCommonExtractUInt16BE(&rxPacket->pdu.data[2]);

    /* Check if a callback function was registered. */
    if (context->readHoldingRegFcn == NULL)
    {
      /* Prepare exception response. */
      txPacket->pdu.code |= TBX_MB_FC_EXCEPTION_MASK;
      txPacket->pdu.data[0] = TBX_MB_EC01_ILLEGAL_FUNCTION;
      txPacket->dataLen = 1U;
    }
    /* Check if the quantity of registers is invalid. */
    else if ((numRegs < 1U) || (numRegs > 125U))
    {
      /* Prepare exception response. */
      txPacket->pdu.code |= TBX_MB_FC_EXCEPTION_MASK;
      txPacket->pdu.data[0] = TBX_MB_EC03_ILLEGAL_DATA_VALUE;
      txPacket->dataLen = 1U;
    }
    /* All is good for further processing. */
    else
    {
      /* Store byte count in the response and prepare the data length. */
      txPacket->pdu.data[0] = 2U * numRegs;
      txPacket->dataLen = txPacket->pdu.data[0] + 1U;
      /* Loop through all the registers. */
      for (uint8_t idx = 0U; idx < numRegs; idx++)
      {
        uint16_t           regValue = 0U;
        tTbxMbServerResult srvResult;
        /* Obtain register value. */
        srvResult = context->readHoldingRegFcn(context, startAddr + idx, &regValue);
        /* No exception reported? */
        if (srvResult == TBX_MB_SERVER_OK)
        {
          /* Store the register value in the response. */
          TbxMbCommonStoreUInt16BE(regValue, &txPacket->pdu.data[1U + (idx * 2U)]);
        }
        /* Exception detected. */
        else
        {
          /* Prepare exception response. */
          txPacket->pdu.code |= TBX_MB_FC_EXCEPTION_MASK;
          if (srvResult == TBX_MB_SERVER_ERR_ILLEGAL_DATA_ADDR)
          {
            txPacket->pdu.data[0] = TBX_MB_EC02_ILLEGAL_DATA_ADDRESS;
          }
          else
          {
            txPacket->pdu.data[0] = TBX_MB_EC04_SERVER_DEVICE_FAILURE;
          }
          txPacket->dataLen = 1U;
          /* Stop looping. */
          break;
        }
      }
    }
  }
} /*** end of TbxMbServerFC03ReadHoldingRegs ***/


/************************************************************************************//**
** \brief     Handles a newly received PDU for function code 4 - Read Input Registers.
** \details   Note that this function is called at a time that txPacket->code is already
**            prepared. Also note that txPacket->node should not be touched here.
** \param     context Pointer to the Modbus server channel context.
** \param     rxPacket Received PDU packet with MUX access.
** \param     txPacket Storage for the PDU response packet with MUX access.
**
****************************************************************************************/
static void TbxMbServerFC04ReadInputRegs(tTbxMbServerCtx       * context,
                                        tTbxMbTpPacket  const * rxPacket,
                                        tTbxMbTpPacket        * txPacket)
{
  /* Verify parameters. */
  TBX_ASSERT((context != NULL) && (rxPacket != NULL) && (txPacket != NULL));

  /* Only continue with valid parameters. */
  if ((context != NULL) && (rxPacket != NULL) && (txPacket != NULL))
  {
    /* Read out request packet parameters. */
    uint16_t startAddr = TbxMbCommonExtractUInt16BE(&rxPacket->pdu.data[0]);
    uint16_t numRegs   = TbxMbCommonExtractUInt16BE(&rxPacket->pdu.data[2]);

    /* Check if a callback function was registered. */
    if (context->readInputRegFcn == NULL)
    {
      /* Prepare exception response. */
      txPacket->pdu.code |= TBX_MB_FC_EXCEPTION_MASK;
      txPacket->pdu.data[0] = TBX_MB_EC01_ILLEGAL_FUNCTION;
      txPacket->dataLen = 1U;
    }
    /* Check if the quantity of registers is invalid. */
    else if ((numRegs < 1U) || (numRegs > 125U))
    {
      /* Prepare exception response. */
      txPacket->pdu.code |= TBX_MB_FC_EXCEPTION_MASK;
      txPacket->pdu.data[0] = TBX_MB_EC03_ILLEGAL_DATA_VALUE;
      txPacket->dataLen = 1U;
    }
    /* All is good for further processing. */
    else
    {
      /* Store byte count in the response and prepare the data length. */
      txPacket->pdu.data[0] = 2U * numRegs;
      txPacket->dataLen = txPacket->pdu.data[0] + 1U;
      /* Loop through all the registers. */
      for (uint8_t idx = 0U; idx < numRegs; idx++)
      {
        uint16_t           regValue = 0U;
        tTbxMbServerResult srvResult;
        /* Obtain register value. */
        srvResult = context->readInputRegFcn(context, startAddr + idx, &regValue);
        /* No exception reported? */
        if (srvResult == TBX_MB_SERVER_OK)
        {
          /* Store the register value in the response. */
          TbxMbCommonStoreUInt16BE(regValue, &txPacket->pdu.data[1U + (idx * 2U)]);
        }
        /* Exception detected. */
        else
        {
          /* Prepare exception response. */
          txPacket->pdu.code |= TBX_MB_FC_EXCEPTION_MASK;
          if (srvResult == TBX_MB_SERVER_ERR_ILLEGAL_DATA_ADDR)
          {
            txPacket->pdu.data[0] = TBX_MB_EC02_ILLEGAL_DATA_ADDRESS;
          }
          else
          {
            txPacket->pdu.data[0] = TBX_MB_EC04_SERVER_DEVICE_FAILURE;
          }
          txPacket->dataLen = 1U;
          /* Stop looping. */
          break;
        }
      }
    }
  }
} /*** end of TbxMbServerFC04ReadInputRegs ***/


/************************************************************************************//**
** \brief     Handles a newly received PDU for function code 5 - Write Single Coil.
** \details   Note that this function is called at a time that txPacket->code is already
**            prepared. Also note that txPacket->node should not be touched here.
** \param     context Pointer to the Modbus server channel context.
** \param     rxPacket Received PDU packet with MUX access.
** \param     txPacket Storage for the PDU response packet with MUX access.
**
****************************************************************************************/
static void TbxMbServerFC05WriteSingleCoil(tTbxMbServerCtx       * context,
                                           tTbxMbTpPacket  const * rxPacket,
                                           tTbxMbTpPacket        * txPacket)
{
  /* Verify parameters. */
  TBX_ASSERT((context != NULL) && (rxPacket != NULL) && (txPacket != NULL));

  /* Only continue with valid parameters. */
  if ((context != NULL) && (rxPacket != NULL) && (txPacket != NULL))
  {
    /* Read out request packet parameters. */
    uint16_t startAddr   = TbxMbCommonExtractUInt16BE(&rxPacket->pdu.data[0]);
    uint16_t outputValue = TbxMbCommonExtractUInt16BE(&rxPacket->pdu.data[2]);

    /* Check if a callback function was registered. */
    if (context->writeCoilFcn == NULL)
    {
      /* Prepare exception response. */
      txPacket->pdu.code |= TBX_MB_FC_EXCEPTION_MASK;
      txPacket->pdu.data[0] = TBX_MB_EC01_ILLEGAL_FUNCTION;
      txPacket->dataLen = 1U;
    }
    /* Check if the output value is invalid. */
    else if ((outputValue != 0x0000U) && (outputValue != 0xFF00U))
    {
      /* Prepare exception response. */
      txPacket->pdu.code |= TBX_MB_FC_EXCEPTION_MASK;
      txPacket->pdu.data[0] = TBX_MB_EC03_ILLEGAL_DATA_VALUE;
      txPacket->dataLen = 1U;
    }
    /* All is good for further processing. */
    else
    {
      /* Prepare the response and its data length. It's the same as the request. */
      txPacket->pdu.data[0U] = rxPacket->pdu.data[0U];
      txPacket->pdu.data[1U] = rxPacket->pdu.data[1U];
      txPacket->pdu.data[2U] = rxPacket->pdu.data[2U];
      txPacket->pdu.data[3U] = rxPacket->pdu.data[3U];
      txPacket->dataLen = 4U;
      /* Write the coil value. */
      tTbxMbServerResult srvResult;
      uint8_t            coilValue = (outputValue == 0x0000U) ? TBX_OFF : TBX_ON;
      srvResult = context->writeCoilFcn(context, startAddr, coilValue);
      /* Exception reported? */
      if (srvResult != TBX_MB_SERVER_OK)
      {
        /* Prepare exception response. */
        txPacket->pdu.code |= TBX_MB_FC_EXCEPTION_MASK;
        if (srvResult == TBX_MB_SERVER_ERR_ILLEGAL_DATA_ADDR)
        {
          txPacket->pdu.data[0] = TBX_MB_EC02_ILLEGAL_DATA_ADDRESS;
        }
        else
        {
          txPacket->pdu.data[0] = TBX_MB_EC04_SERVER_DEVICE_FAILURE;
        }
        txPacket->dataLen = 1U;
      }
    }
  }
} /*** end of TbxMbServerFC05WriteSingleCoil ***/


/************************************************************************************//**
** \brief     Handles a newly received PDU for function code 6 - Write Single Register.
** \details   Note that this function is called at a time that txPacket->code is already
**            prepared. Also note that txPacket->node should not be touched here.
** \param     context Pointer to the Modbus server channel context.
** \param     rxPacket Received PDU packet with MUX access.
** \param     txPacket Storage for the PDU response packet with MUX access.
**
****************************************************************************************/
static void TbxMbServerFC06WriteSingleReg(tTbxMbServerCtx       * context,
                                          tTbxMbTpPacket  const * rxPacket,
                                          tTbxMbTpPacket        * txPacket)
{
  /* Verify parameters. */
  TBX_ASSERT((context != NULL) && (rxPacket != NULL) && (txPacket != NULL));

  /* Only continue with valid parameters. */
  if ((context != NULL) && (rxPacket != NULL) && (txPacket != NULL))
  {
    /* Read out request packet parameters. */
    uint16_t regAddr  = TbxMbCommonExtractUInt16BE(&rxPacket->pdu.data[0]);
    uint16_t regValue = TbxMbCommonExtractUInt16BE(&rxPacket->pdu.data[2]);

    /* Check if a callback function was registered. */
    if (context->writeHoldingRegFcn == NULL)
    {
      /* Prepare exception response. */
      txPacket->pdu.code |= TBX_MB_FC_EXCEPTION_MASK;
      txPacket->pdu.data[0] = TBX_MB_EC01_ILLEGAL_FUNCTION;
      txPacket->dataLen = 1U;
    }
    /* All is good for further processing. */
    else
    {
      /* Prepare the response and its data length. It's the same as the request. */
      txPacket->pdu.data[0U] = rxPacket->pdu.data[0U];
      txPacket->pdu.data[1U] = rxPacket->pdu.data[1U];
      txPacket->pdu.data[2U] = rxPacket->pdu.data[2U];
      txPacket->pdu.data[3U] = rxPacket->pdu.data[3U];
      txPacket->dataLen = 4U;
      /* Write the register value. */
      tTbxMbServerResult srvResult;
      srvResult = context->writeHoldingRegFcn(context, regAddr, regValue);
      /* Exception reported? */
      if (srvResult != TBX_MB_SERVER_OK)
      {
        /* Prepare exception response. */
        txPacket->pdu.code |= TBX_MB_FC_EXCEPTION_MASK;
        if (srvResult == TBX_MB_SERVER_ERR_ILLEGAL_DATA_ADDR)
        {
          txPacket->pdu.data[0] = TBX_MB_EC02_ILLEGAL_DATA_ADDRESS;
        }
        else
        {
          txPacket->pdu.data[0] = TBX_MB_EC04_SERVER_DEVICE_FAILURE;
        }
        txPacket->dataLen = 1U;
      }
    }
  }
} /*** end of TbxMbServerFC06WriteSingleReg ***/


/************************************************************************************//**
** \brief     Handles a newly received PDU for function code 8 - Diagnostics.
** \details   Note that this function is called at a time that txPacket->code is already
**            prepared. Also note that txPacket->node should not be touched here.
** \param     context Pointer to the Modbus server channel context.
** \param     rxPacket Received PDU packet with MUX access.
** \param     txPacket Storage for the PDU response packet with MUX access.
**
****************************************************************************************/
static void TbxMbServerFC08Diagnostics(tTbxMbServerCtx       * context,
                                       tTbxMbTpPacket  const * rxPacket,
                                       tTbxMbTpPacket        * txPacket)
{
  /* Verify parameters. */
  TBX_ASSERT((context != NULL) && (rxPacket != NULL) && (txPacket != NULL));

  /* Only continue with valid parameters. */
  if ((context != NULL) && (rxPacket != NULL) && (txPacket != NULL))
  {
    /* Read out request packet parameters. */
    uint16_t subCode   = TbxMbCommonExtractUInt16BE(&rxPacket->pdu.data[0]);
    uint16_t dataField = TbxMbCommonExtractUInt16BE(&rxPacket->pdu.data[2]);
    /* Prepare the most common response. It's typically the sub-function code echoed,
     * together with a 16-bit unsigned value.
     */
    txPacket->pdu.data[0U] = rxPacket->pdu.data[0U];
    txPacket->pdu.data[1U] = rxPacket->pdu.data[1U];
    txPacket->dataLen = 4U;

    /* Filter on the received sub-function code. */
    switch (subCode)
    {
      case TBX_MB_DIAG_SC_QUERY_DATA:
      {
        /* Echo the received data back. */
        for (uint8_t idx = 0U; idx < rxPacket->dataLen; idx++)
        {
         txPacket->pdu.data[idx] = rxPacket->pdu.data[idx];
        }
        txPacket->dataLen = rxPacket->dataLen;
      }
      break;

      case TBX_MB_DIAG_SC_CLEAR_COUNTERS:
      {
        /* Data field not valid? */
        if (dataField != 0x0000U)
        {
          /* Prepare exception response. */
          txPacket->pdu.code |= TBX_MB_FC_EXCEPTION_MASK;
          txPacket->pdu.data[0] = TBX_MB_EC03_ILLEGAL_DATA_VALUE;
          txPacket->dataLen = 1U;
        }
        /* All is good for further processing. */        
        else
        {
          /* Clear the diagnostic counters. */
          context->tpCtx->diagInfo.busMsgCnt     = 0U;
          context->tpCtx->diagInfo.busCommErrCnt = 0U;
          context->tpCtx->diagInfo.busExcpErrCnt = 0U;
          context->tpCtx->diagInfo.srvMsgCnt     = 0U;
          context->tpCtx->diagInfo.srvNoRespCnt  = 0U;
          /* Echo the request data field. */
          TbxMbCommonStoreUInt16BE(dataField, &txPacket->pdu.data[2U]);
        }
      }
      break;

      case TBX_MB_DIAG_SC_BUS_MESSAGE_COUNT:
      {
        /* Data field not valid? */
        if (dataField != 0x0000U)
        {
          /* Prepare exception response. */
          txPacket->pdu.code |= TBX_MB_FC_EXCEPTION_MASK;
          txPacket->pdu.data[0] = TBX_MB_EC03_ILLEGAL_DATA_VALUE;
          txPacket->dataLen = 1U;
        }
        /* All is good for further processing. */        
        else
        {
          /* Store he bus message count. */
          TbxMbCommonStoreUInt16BE(context->tpCtx->diagInfo.busMsgCnt, 
                                   &txPacket->pdu.data[2U]);
        }
      }
      break;

      case TBX_MB_DIAG_SC_BUS_COMM_ERROR_COUNT:
      {
        /* Data field not valid? */
        if (dataField != 0x0000U)
        {
          /* Prepare exception response. */
          txPacket->pdu.code |= TBX_MB_FC_EXCEPTION_MASK;
          txPacket->pdu.data[0] = TBX_MB_EC03_ILLEGAL_DATA_VALUE;
          txPacket->dataLen = 1U;
        }
        /* All is good for further processing. */        
        else
        {
          /* Store he bus message count. */
          TbxMbCommonStoreUInt16BE(context->tpCtx->diagInfo.busCommErrCnt, 
                                   &txPacket->pdu.data[2U]);
        }
      }
      break;

      case TBX_MB_DIAG_SC_BUS_EXCEPTION_ERROR_COUNT:
      {
        /* Data field not valid? */
        if (dataField != 0x0000U)
        {
          /* Prepare exception response. */
          txPacket->pdu.code |= TBX_MB_FC_EXCEPTION_MASK;
          txPacket->pdu.data[0] = TBX_MB_EC03_ILLEGAL_DATA_VALUE;
          txPacket->dataLen = 1U;
        }
        /* All is good for further processing. */        
        else
        {
          /* Store he bus message count. */
          TbxMbCommonStoreUInt16BE(context->tpCtx->diagInfo.busExcpErrCnt, 
                                   &txPacket->pdu.data[2U]);
        }
      }
      break;

      case TBX_MB_DIAG_SC_SERVER_MESSAGE_COUNT:
      {
        /* Data field not valid? */
        if (dataField != 0x0000U)
        {
          /* Prepare exception response. */
          txPacket->pdu.code |= TBX_MB_FC_EXCEPTION_MASK;
          txPacket->pdu.data[0] = TBX_MB_EC03_ILLEGAL_DATA_VALUE;
          txPacket->dataLen = 1U;
        }
        /* All is good for further processing. */        
        else
        {
          /* Store he bus message count. */
          TbxMbCommonStoreUInt16BE(context->tpCtx->diagInfo.srvMsgCnt, 
                                   &txPacket->pdu.data[2U]);
        }
      }
      break;

      case TBX_MB_DIAG_SC_SERVER_NO_RESPONSE_COUNT:
      {
        /* Data field not valid? */
        if (dataField != 0x0000U)
        {
          /* Prepare exception response. */
          txPacket->pdu.code |= TBX_MB_FC_EXCEPTION_MASK;
          txPacket->pdu.data[0] = TBX_MB_EC03_ILLEGAL_DATA_VALUE;
          txPacket->dataLen = 1U;
        }
        /* All is good for further processing. */        
        else
        {
          /* Store he bus message count. */
          TbxMbCommonStoreUInt16BE(context->tpCtx->diagInfo.srvNoRespCnt, 
                                   &txPacket->pdu.data[2U]);
        }
      }
      break;

      default:
      {
        /* Unsupported sub-function code. Prepare exception response. */
        txPacket->pdu.code |= TBX_MB_FC_EXCEPTION_MASK;
        txPacket->pdu.data[0] = TBX_MB_EC01_ILLEGAL_FUNCTION;
        txPacket->dataLen = 1U;
      }
      break;
    }
  }
} /*** end of TbxMbServerFC08Diagnostics ***/


/************************************************************************************//**
** \brief     Handles a newly received PDU for function code 15 - Write Multiple Coils.
** \details   Note that this function is called at a time that txPacket->code is already
**            prepared. Also note that txPacket->node should not be touched here.
** \param     context Pointer to the Modbus server channel context.
** \param     rxPacket Received PDU packet with MUX access.
** \param     txPacket Storage for the PDU response packet with MUX access.
**
****************************************************************************************/
static void TbxMbServerFC15WriteMultipleCoils(tTbxMbServerCtx       * context,
                                              tTbxMbTpPacket  const * rxPacket,
                                              tTbxMbTpPacket        * txPacket)
{
  /* Verify parameters. */
  TBX_ASSERT((context != NULL) && (rxPacket != NULL) && (txPacket != NULL));

  /* Only continue with valid parameters. */
  if ((context != NULL) && (rxPacket != NULL) && (txPacket != NULL))
  {
    /* Read out request packet parameters. */
    uint16_t startAddr = TbxMbCommonExtractUInt16BE(&rxPacket->pdu.data[0]);
    uint16_t numCoils  = TbxMbCommonExtractUInt16BE(&rxPacket->pdu.data[2]);
    uint8_t  byteCnt   = rxPacket->pdu.data[4];
    /* Determine the number of bytes needed to hold all the coil bits. Make it U16 
     * because the range validity of numCoils is not yet checked.
     */
    uint16_t numBytes = (uint16_t)(numCoils / 8U);
    if ((numCoils % 8U) != 0U)
    {
      numBytes++;
    }
    /* Check if a callback function was registered. */
    if (context->writeCoilFcn == NULL)
    {
      /* Prepare exception response. */
      txPacket->pdu.code |= TBX_MB_FC_EXCEPTION_MASK;
      txPacket->pdu.data[0] = TBX_MB_EC01_ILLEGAL_FUNCTION;
      txPacket->dataLen = 1U;
    }
    /* Check if the quantity of coils is invalid. */
    else if (((numCoils < 1U) || (numCoils > 1968U)))
    {
      /* Prepare exception response. */
      txPacket->pdu.code |= TBX_MB_FC_EXCEPTION_MASK;
      txPacket->pdu.data[0] = TBX_MB_EC03_ILLEGAL_DATA_VALUE;
      txPacket->dataLen = 1U;
    }
    /* Check if the quantity of bytes is invalid. */
    else if (numBytes != byteCnt)
    {
      /* Prepare exception response. */
      txPacket->pdu.code |= TBX_MB_FC_EXCEPTION_MASK;
      txPacket->pdu.data[0] = TBX_MB_EC03_ILLEGAL_DATA_VALUE;
      txPacket->dataLen = 1U;
    }
    /* All is good for further processing. */
    else
    {
      /* Prepare the response and its data length. It's mostly the same as the request.*/
      txPacket->pdu.data[0U] = rxPacket->pdu.data[0U];
      txPacket->pdu.data[1U] = rxPacket->pdu.data[1U];
      txPacket->pdu.data[2U] = rxPacket->pdu.data[2U];
      txPacket->pdu.data[3U] = rxPacket->pdu.data[3U];
      txPacket->dataLen = 4U;
      /* Prepare loop indices that aid with writing the coil bits. */
      uint8_t         bitIdx  = 0U;
      uint8_t         byteIdx = 0U;
      /* Initialize byte array pointer for reading the coil bits from the request. */
      uint8_t const * coilData = &rxPacket->pdu.data[5];
      /* Loop through all the coils. */
      for (uint16_t idx = 0U; idx < numCoils; idx++)
      {
        uint8_t            coilValue = TBX_OFF;
        tTbxMbServerResult srvResult;
        /* Extract the requested coil value. */
        if ((coilData[byteIdx] & (1U << bitIdx)) != 0U)
        {
          coilValue = TBX_ON;
        }
        /* Write the coil value. */
        srvResult = context->writeCoilFcn(context, startAddr + idx, coilValue);
        /* Exception reported? */
        if (srvResult != TBX_MB_SERVER_OK)
        {
          /* Prepare exception response. */
          txPacket->pdu.code |= TBX_MB_FC_EXCEPTION_MASK;
          if (srvResult == TBX_MB_SERVER_ERR_ILLEGAL_DATA_ADDR)
          {
            txPacket->pdu.data[0] = TBX_MB_EC02_ILLEGAL_DATA_ADDRESS;
          }
          else
          {
            txPacket->pdu.data[0] = TBX_MB_EC04_SERVER_DEVICE_FAILURE;
          }
          txPacket->dataLen = 1U;
          /* Stop looping. */
          break;
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
} /*** end of TbxMbServerFC15WriteMultipleCoils ***/


/************************************************************************************//**
** \brief     Handles a newly received PDU for function code 16 - Write Multiple
**            Registers.
** \details   Note that this function is called at a time that txPacket->code is already
**            prepared. Also note that txPacket->node should not be touched here.
** \param     context Pointer to the Modbus server channel context.
** \param     rxPacket Received PDU packet with MUX access.
** \param     txPacket Storage for the PDU response packet with MUX access.
**
****************************************************************************************/
static void TbxMbServerFC16WriteMultipleRegs(tTbxMbServerCtx       * context,
                                             tTbxMbTpPacket  const * rxPacket,
                                             tTbxMbTpPacket        * txPacket)
{
  /* Verify parameters. */
  TBX_ASSERT((context != NULL) && (rxPacket != NULL) && (txPacket != NULL));

  /* Only continue with valid parameters. */
  if ((context != NULL) && (rxPacket != NULL) && (txPacket != NULL))
  {
    /* Read out request packet parameters. */
    uint16_t startAddr = TbxMbCommonExtractUInt16BE(&rxPacket->pdu.data[0]);
    uint16_t numRegs   = TbxMbCommonExtractUInt16BE(&rxPacket->pdu.data[2]);
    uint8_t  byteCnt   = rxPacket->pdu.data[4];

    /* Check if a callback function was registered. */
    if (context->writeHoldingRegFcn == NULL)
    {
      /* Prepare exception response. */
      txPacket->pdu.code |= TBX_MB_FC_EXCEPTION_MASK;
      txPacket->pdu.data[0] = TBX_MB_EC01_ILLEGAL_FUNCTION;
      txPacket->dataLen = 1U;
    }
    /* Check if the quantity of registers is invalid. */
    else if (((numRegs < 1U) || (numRegs > 123U)) || (byteCnt != (numRegs * 2U)))
    {
      /* Prepare exception response. */
      txPacket->pdu.code |= TBX_MB_FC_EXCEPTION_MASK;
      txPacket->pdu.data[0] = TBX_MB_EC03_ILLEGAL_DATA_VALUE;
      txPacket->dataLen = 1U;
    }
    /* All is good for further processing. */
    else
    {
      /* Prepare the response and its data length. It's mostly the same as the request.*/
      txPacket->pdu.data[0U] = rxPacket->pdu.data[0U];
      txPacket->pdu.data[1U] = rxPacket->pdu.data[1U];
      txPacket->pdu.data[2U] = rxPacket->pdu.data[2U];
      txPacket->pdu.data[3U] = rxPacket->pdu.data[3U];
      txPacket->dataLen = 4U;
      /* Loop through all the registers. */
      for (uint8_t idx = 0U; idx < numRegs; idx++)
      {
        uint16_t           regValue;
        tTbxMbServerResult srvResult;
        /* Extract the requested register value. */
        regValue = TbxMbCommonExtractUInt16BE(&rxPacket->pdu.data[5U + (idx * 2U)]);
        /* Write the register value. */
        srvResult = context->writeHoldingRegFcn(context, startAddr + idx, regValue);
        /* Exception reported? */
        if (srvResult != TBX_MB_SERVER_OK)
        {
          /* Prepare exception response. */
          txPacket->pdu.code |= TBX_MB_FC_EXCEPTION_MASK;
          if (srvResult == TBX_MB_SERVER_ERR_ILLEGAL_DATA_ADDR)
          {
            txPacket->pdu.data[0] = TBX_MB_EC02_ILLEGAL_DATA_ADDRESS;
          }
          else
          {
            txPacket->pdu.data[0] = TBX_MB_EC04_SERVER_DEVICE_FAILURE;
          }
          txPacket->dataLen = 1U;
          /* Stop looping. */
          break;
        }
      }
    }
  }
} /*** end of TbxMbServerFC16WriteMultipleRegs ***/


/*********************************** end of tbxmb_server.c *****************************/
