/************************************************************************************//**
* \file         tbxmb_rtu.c
* \brief        Modbus RTU transport layer source file.
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
#include "tbxmb_uart_private.h"                  /* MicroTBX-Modbus UART private       */


/****************************************************************************************
* Macro definitions
****************************************************************************************/
#ifndef TBX_MB_RTU_T1_5_TIMEOUT_ENABLE
/** \brief The Modbus RTU protocol requires that there is no more than a 1.5 character
 *         time between newly received packet bytes. Otherwise a newly received packet
 *         should be ignored. This RTU transport layer supports this monitoring if this
 *         configuration macro is > 0, which is recommended.
 * 
 *         However, not all RTU hardware on the market fullfills this requirement. To
 *         make it easier to get communication with other Modbus RTU devices working,
 *         the monitoring of the max. 1.5 character timeout is disabled by default. 
 *             
 *         In case you know that all devices on your Modbus RTU network adhere to the
 *         max. 1.5 character time requirement or you need to pass Modbus compliance
 *         testing, you can enable the 1.5 character timeout detection. To override this
 *         default configuration, you can add a macro with the same name, but with a
 *         value of 1 (enable), to "tbx_conf.h".
 */
#define TBX_MB_RTU_T1_5_TIMEOUT_ENABLE      (0U)
#endif

/** \brief Unique context type to identify a context as being an RTU transport layer. */
#define TBX_MB_RTU_CONTEXT_TYPE             (84U)

/** \brief Initial state. */
#define TBX_MB_RTU_STATE_INIT               (0U)

/** \brief Idle state. Ready to receive or transmit. */
#define TBX_MB_RTU_STATE_IDLE               (1U)

/** \brief Transmitting a PDU state. */
#define TBX_MB_RTU_STATE_TRANSMISSION       (2U)

/** \brief Receiving a PDU state. */
#define TBX_MB_RTU_STATE_RECEPTION          (3U)

/** \brief Validating a newly received PDU state. */
#define TBX_MB_RTU_STATE_VALIDATION         (4U)


/****************************************************************************************
* Function prototypes
****************************************************************************************/
static void             TbxMbRtuPoll            (tTbxMbTp               transport);

static uint8_t          TbxMbRtuTransmit        (tTbxMbTp               transport);

static void             TbxMbRtuReceptionDone   (tTbxMbTp               transport);

static tTbxMbTpPacket * TbxMbRtuGetRxPacket     (tTbxMbTp               transport);

static tTbxMbTpPacket * TbxMbRtuGetTxPacket     (tTbxMbTp               transport);

static uint8_t          TbxMbRtuValidate        (tTbxMbTp               transport);

static void             TbxMbRtuTransmitComplete(tTbxMbUartPort         port);

static void             TbxMbRtuDataReceived    (tTbxMbUartPort         port, 
                                                 uint8_t        const * data, 
                                                 uint8_t                len);
                                                 
static uint16_t         TbxMbRtuCalculatCrc     (uint8_t        const * data, 
                                                 uint16_t               len);


/****************************************************************************************
* Local data declarations
****************************************************************************************/
/** \brief RTU transport layer handle lookup table by UART port. Uses for finding the
 *         transport layer handle that uses a specific serial port, in a run-time
 *         efficient way.
 */
static volatile tTbxMbTpCtx * tbxMbRtuCtx[TBX_MB_UART_NUM_PORT] = { 0 };


/************************************************************************************//**
** \brief     Creates a Modbus RTU transport layer object.
** \param     nodeAddr The address of the node. Can be in the range 1..247 for a server
**            node. Set it to 0 for the client.
** \param     port The serial port to use. The actual meaning of the serial port is
**            hardware dependent. It typically maps to the UART peripheral number. E.g. 
**            TBX_MB_UART_PORT1 = USART1 on an STM32.
** \param     baudrate The desired communication speed.
** \param     stopbits Number of stop bits at the end of a character.
** \param     parity Parity bit type to use.
** \return    Handle to the newly created RTU transport layer object if successful, NULL
**            otherwise.
**
****************************************************************************************/
tTbxMbTp TbxMbRtuCreate(uint8_t            nodeAddr, 
                        tTbxMbUartPort     port, 
                        tTbxMbUartBaudrate baudrate,
                        tTbxMbUartStopbits stopbits,
                        tTbxMbUartParity   parity)
{
  tTbxMbTp result = NULL;

  /* Make sure the OSAL event module is initialized. The application will always first
   * create a transport layer object before a channel object. Consequently, this is the
   * best place to do the OSAL module initialization.
   */
  TbxMbOsalEventInit();

  /* Verify parameters. */
  TBX_ASSERT((nodeAddr <= TBX_MB_TP_NODE_ADDR_MAX) &&
             (port < TBX_MB_UART_NUM_PORT) && 
             (baudrate < TBX_MB_UART_NUM_BAUDRATE) &&
             (stopbits < TBX_MB_UART_NUM_STOPBITS) &&
             (parity < TBX_MB_UART_NUM_PARITY));

  /* Only continue with valid parameters. */
  if ((nodeAddr <= TBX_MB_TP_NODE_ADDR_MAX) &&
      (port < TBX_MB_UART_NUM_PORT) && 
      (baudrate < TBX_MB_UART_NUM_BAUDRATE) &&
      (stopbits < TBX_MB_UART_NUM_STOPBITS) &&
      (parity < TBX_MB_UART_NUM_PARITY))
  {
    /* Allocate memory for the new transport context. */
    tTbxMbTpCtx * newTpCtx = TbxMemPoolAllocate(sizeof(tTbxMbTpCtx));
    /* Automatically increase the memory pool, if it was too small. */
    if (newTpCtx == NULL)
    {
      /* No need to check the return value, because if it failed, the following
       * allocation fails too, which is verified later on.
       */
      (void)TbxMemPoolCreate(1U, sizeof(tTbxMbTpCtx));
      newTpCtx = TbxMemPoolAllocate(sizeof(tTbxMbTpCtx));      
    }
    /* Verify memory allocation of the transport context. */
    TBX_ASSERT(newTpCtx != NULL);
    /* Only continue if the memory allocation succeeded. */
    if (newTpCtx != NULL)
    {
      /* Initialize the transport context. */
      newTpCtx->type = TBX_MB_RTU_CONTEXT_TYPE;
      newTpCtx->instancePtr = NULL;
      newTpCtx->pollFcn = TbxMbRtuPoll;
      newTpCtx->processFcn = NULL;
      newTpCtx->transmitFcn = TbxMbRtuTransmit;
      newTpCtx->receptionDoneFcn = TbxMbRtuReceptionDone;
      newTpCtx->getRxPacketFcn = TbxMbRtuGetRxPacket;
      newTpCtx->getTxPacketFcn = TbxMbRtuGetTxPacket;
      newTpCtx->nodeAddr = nodeAddr;
      newTpCtx->port = port;
      newTpCtx->state = TBX_MB_RTU_STATE_INIT;
      newTpCtx->rxTime = TbxMbPortTimerCount();
      newTpCtx->initStateExitSem = TbxMbOsalSemCreate();
      newTpCtx->diagInfo.busMsgCnt = 0U;
      newTpCtx->diagInfo.busCommErrCnt = 0U;
      newTpCtx->diagInfo.busExcpErrCnt = 0U;
      newTpCtx->diagInfo.srvMsgCnt = 0U;
      newTpCtx->diagInfo.srvNoRespCnt = 0U;
      /* Store the transport context in the lookup table. */
      tbxMbRtuCtx[port] = newTpCtx;
      /* Initialize the port. Note the RTU always uses 8 databits. */
      TbxMbUartInit(port, baudrate, TBX_MB_UART_8_DATABITS, stopbits, parity,
                    TbxMbRtuTransmitComplete, TbxMbRtuDataReceived);
      /* Determine the 1.5 and 3.5 character times in units of 50us ticks. If the
       * baudrate is greater than 19200, then these are fixed to 750us and 1750us,
       * respectively. Make sure to add one extra to adjust for timer resolution
       * inaccuracy.
       */
      if (baudrate > TBX_MB_UART_19200BPS)
      {
        newTpCtx->t1_5Ticks = 16U;                         /* 750us / 50us ticks.      */
        newTpCtx->t3_5Ticks = 36U;                         /* 1750us / 50us ticks      */
      }
      /* Need to calculate the 1.5 and 3.5 character times. */
      else
      {
        /* On RTU, one character equals 11 bits: start-bit, 8 data-bits, parity-bit and
         * stop-bit. In case no parity is used, 2 stop-bits are required by the protocol.
         * This means that the number of characters per seconds equals the baudrate
         * divided by 11. The character time in seconds is the reciprocal of that.
         * Multiply by 10^6 to get the charater time in microseconds:
         *
         * tCharMicros = 11 * 1000000 / baudrate.
         *
         * The 1.5 and 3.5 character times in microseconds:
         *
         * t1_5CharMicros = 11 * 1000000 * 1.5 / baudrate = 16500000 / baudrate
         * t3_5CharMicros = 11 * 1000000 * 3.5 / baudrate = 38500000 / baudrate
         * 
         * This module uses ticks of a 20 kHz timer as a time unit. Each tick is 50us:
         * 
         * t1_5CharTicks = (16500000 / 50) / baudrate = 330000 / baudrate
         * t3_5CharTicks = (38500000 / 50) / baudrate = 770000 / baudrate
         * 
         */
        const uint16_t baudrateLookup[] =
        {
          1200,                                            /* TBX_MB_UART_1200BPS      */
          2400,                                            /* TBX_MB_UART_2400BPS      */
          4800,                                            /* TBX_MB_UART_4800BPS      */
          9600,                                            /* TBX_MB_UART_9600BPS      */
          19200                                            /* TBX_MB_UART_19200BPS     */
        };
        /* The following calculation does integer roundup (A + (B-1)) / B and adds one
         * extra to adjust for timer resolution inaccuracy.
         */
        uint16_t baudBps = baudrateLookup[baudrate];
        newTpCtx->t1_5Ticks = (uint16_t)(((330000UL + (baudBps - 1UL)) / baudBps) + 1U);
        newTpCtx->t3_5Ticks = (uint16_t)(((770000UL + (baudBps - 1UL)) / baudBps) + 1U);
      }
      /* Instruct the event task to call our polling function to be able to determine
       * when it's time to transition from INIT to IDLE.
       */
      tTbxMbEvent newEvent = {.context = newTpCtx, .id = TBX_MB_EVENT_ID_START_POLLING};
      TbxMbOsalEventPost(&newEvent, TBX_FALSE);
      /* Update the result. */
      result = newTpCtx;
    }
  }
  /* Give the result back to the caller. */
  return result;
} /*** end of TbxMbRtuCreate ***/  


/************************************************************************************//**
** \brief     Releases a Modbus RTU transport layer object, previously created with 
**            TbxMbRtuCreate().
** \param     transport Handle to RTU transport layer object to release.
**
****************************************************************************************/
void TbxMbRtuFree(tTbxMbTp transport)
{
  /* Verify parameters. */
  TBX_ASSERT(transport != NULL);

  /* Only continue with valid parameters. */
  if (transport != NULL)
  {
    /* Convert the TP channel pointer to the context structure. */
    tTbxMbTpCtx * tpCtx = (tTbxMbTpCtx *)transport;
    /* Sanity check on the context type. */
    TBX_ASSERT(tpCtx->type == TBX_MB_RTU_CONTEXT_TYPE);
    /* Release the semaphore used for syncing to the INIT to IDLE state transition. */
    TbxMbOsalSemFree(tpCtx->initStateExitSem);
    TbxCriticalSectionEnter();
    /* Remove the channel from the lookup table. */
    tbxMbRtuCtx[tpCtx->port] = NULL;
    /* Invalidate the context to protect it from accidentally being used afterwards. */
    tpCtx->type = 0U;
    tpCtx->pollFcn = NULL;
    tpCtx->processFcn = NULL;
    TbxCriticalSectionExit();
    /* Give the transport layer context back to the memory pool. */
    TbxMemPoolRelease(tpCtx);
  }
} /*** end of TbxMbRtuFree ***/


/************************************************************************************//**
** \brief     Event polling function that is automatically called during each call of
**            TbxMbEventTask(), if activated. Use the TBX_MB_EVENT_ID_START_POLLING and
**            TBX_MB_EVENT_ID_STOP_POLLING events to activate and deactivate.
** \param     transport Handle to RTU transport layer object.
**
****************************************************************************************/
static void TbxMbRtuPoll(tTbxMbTp transport)
{
  /* Verify parameters. */
  TBX_ASSERT(transport != NULL);

  /* Only continue with valid parameters. */
  if (transport != NULL)
  {
    /* Convert the TP channel pointer to the context structure. */
    tTbxMbTpCtx * tpCtx = (tTbxMbTpCtx *)transport;
    /* Sanity check on the context type. */
    TBX_ASSERT(tpCtx->type == TBX_MB_RTU_CONTEXT_TYPE);
    /* Filter on the current state. */
    TbxCriticalSectionEnter();
    uint8_t currentState = tpCtx->state;
    TbxCriticalSectionExit();
    switch (currentState)
    {
      case TBX_MB_RTU_STATE_RECEPTION:
      {
        TbxCriticalSectionEnter();
        uint16_t rxTimeCopy = tpCtx->rxTime;
        TbxCriticalSectionExit();
        /* Calculate the number of time ticks that elapsed since the reception of the
         * last byte. Note that this calculation works, even if the timer counter
         * overflowed.
         */
        uint16_t deltaTicks = TbxMbPortTimerCount() - rxTimeCopy;
        /* Did 3.5 character times elapse since the last byte reception? */
        if (deltaTicks >= tpCtx->t3_5Ticks)
        {
          /* Instruct the event task to stop calling our polling function. */
          tTbxMbEvent newEvent;
          newEvent.context = tpCtx;
          newEvent.id = TBX_MB_EVENT_ID_STOP_POLLING;
          TbxMbOsalEventPost(&newEvent, TBX_FALSE);
          /* Is the newly received frame still in the OK state? */
          TbxCriticalSectionEnter();
          uint8_t rxAduOkayCpy = tpCtx->rxAduOkay;
          TbxCriticalSectionExit();
          if (rxAduOkayCpy == TBX_TRUE)
          {
            /* Transition to the VALIDATION state. This prevents newly received bytes
             * from being added to the packet. No bytes should be received anyways at
             * this point, but you never know. Better safe than sorry.
             */
            TbxCriticalSectionEnter();
            tpCtx->state = TBX_MB_RTU_STATE_VALIDATION;
            TbxCriticalSectionExit();
            /* Packet reception complete. Set the PDU data length field. At this point 
             * rxAduWrIdx holds to total received bytes in the ADU. The PDU data length
             * is that one, minus:
             * - Node address (1 byte)
             * - Function code (1 byte)
             * - CRC16 (2 bytes)
             *
             * Note that in the VALIDATION state, the data reception path is locked until
             * a transition back to IDLE state is made. Consequenty, there is no need for
             * critical sections when accessing the .rxXyz elements of the TP context. 
             */
            tpCtx->rxPacket.dataLen = tpCtx->rxAduWrIdx - 4U;
            /* Also store the node address in the packet's node element. That's were 
             * channels expect it. It's in the first byte of the ADU and the ADU starts
             * at one byte before the PDU, which is the last byte of head[].
             */
            tpCtx->rxPacket.node = tpCtx->rxPacket.head[TBX_MB_TP_ADU_HEAD_LEN_MAX-1U];
            /* Validate the newly received packet. */
            if (TbxMbRtuValidate(tpCtx) != TBX_OK)
            {
              /* Discard the newly received frame by transitioning back to IDLE. */
              TbxCriticalSectionEnter();
              tpCtx->state = TBX_MB_RTU_STATE_IDLE;
              TbxCriticalSectionExit();
            }
            /* Newly received packet is valid. */
            else
            {
              /* Post an event to the linked channel for further processing of the PDU.*/
              tTbxMbEvent pduRxEvent;
              pduRxEvent.context = tpCtx->channelCtx;
              pduRxEvent.id = TBX_MB_EVENT_ID_PDU_RECEIVED;
              TbxMbOsalEventPost(&pduRxEvent, TBX_FALSE);
            }
          }
          /* Frame was marked as not okay (NOK) during its reception. Most likely a
           * 1.5 character timeout. 
           */
          else
          {
            /* Discard the newly received frame by transitioning back to IDLE. */
            TbxCriticalSectionEnter();
            tpCtx->state = TBX_MB_RTU_STATE_IDLE;
            TbxCriticalSectionExit();
          }
        }
      }
      break;

      case TBX_MB_RTU_STATE_TRANSMISSION:
      {
        TbxCriticalSectionEnter();
        uint16_t txDoneTimeCopy = tpCtx->txDoneTime;
        TbxCriticalSectionExit();
        /* Calculate the number of time ticks that elapsed since completing the packet
         * transmission. Note that this calculation works, even if the timer counter
         * overflowed.
         */
        uint16_t deltaTicks = TbxMbPortTimerCount() - txDoneTimeCopy;
        /* After t3_5 it's time to transition to the IDLE state. */
        if (deltaTicks >= tpCtx->t3_5Ticks)
        {
          /* Transition back to the IDLE state. */
          TbxCriticalSectionEnter();
          tpCtx->state = TBX_MB_RTU_STATE_IDLE;
          TbxCriticalSectionExit();
          /* Instruct the event task to stop calling our polling function. */
          tTbxMbEvent newEvent;
          newEvent.context = tpCtx;
          newEvent.id = TBX_MB_EVENT_ID_STOP_POLLING;
          TbxMbOsalEventPost(&newEvent, TBX_FALSE);
          /* Post an event to the linked channel for inform them that the PDU
           * transmission completed. Note that it's okay to reuse the event local.
           */
          newEvent.context = tpCtx->channelCtx;
          newEvent.id = TBX_MB_EVENT_ID_PDU_TRANSMITTED;
          TbxMbOsalEventPost(&newEvent, TBX_FALSE);
        }
      }
      break;

      case TBX_MB_RTU_STATE_INIT:
      {
        TbxCriticalSectionEnter();
        uint16_t rxTimeCopy = tpCtx->rxTime;
        TbxCriticalSectionExit();
        /* Calculate the number of time ticks that elapsed since entering the INIT state
         * or the reception of the last byte, whichever one comes last. Note that this
         * calculation works, even if the timer counter overflowed.
         */
        uint16_t deltaTicks = TbxMbPortTimerCount() - rxTimeCopy;
        /* After t3_5 it's time to transition to the IDLE state. */
        if (deltaTicks >= tpCtx->t3_5Ticks)
        {
          /* Transition to the IDLE state. */
          TbxCriticalSectionEnter();
          tpCtx->state = TBX_MB_RTU_STATE_IDLE;
          TbxCriticalSectionExit();
          /* Instruct the event task to stop calling our polling function. */
          tTbxMbEvent newEvent;
          newEvent.context = tpCtx;
          newEvent.id = TBX_MB_EVENT_ID_STOP_POLLING;
          TbxMbOsalEventPost(&newEvent, TBX_FALSE);
          /* Give the semaphore to sync the transmit function to this event. This is 
           * needed for an RTU client, when transmit it called before being in the INIt
           * state.
           */
          TbxMbOsalSemGive(tpCtx->initStateExitSem, TBX_FALSE);
        }
      }
      break;

      default:
      {
        /* In the current state, nothing needs to be done. */
      }
      break;
    }
  }
} /*** end of TbxMbRtuPoll ***/


/************************************************************************************//**
** \brief     Starts the transmission of a communication packet, stored in the transport
**            layer object.
** \param     transport Handle to RTU transport layer object.
** \return    TBX_OK if successful, TBX_ERROR otherwise. 
**
****************************************************************************************/
static uint8_t TbxMbRtuTransmit(tTbxMbTp transport)
{
  uint8_t result = TBX_ERROR;

  /* Verify parameters. */
  TBX_ASSERT(transport != NULL);

  /* Only continue with valid parameters. */
  if (transport != NULL)
  {
    /* Convert the TP channel pointer to the context structure. */
    tTbxMbTpCtx * tpCtx = (tTbxMbTpCtx *)transport;
    /* Sanity check on the context type. */
    TBX_ASSERT(tpCtx->type == TBX_MB_RTU_CONTEXT_TYPE);
    /* Are we requested to transmit an exception response? */
    TbxCriticalSectionEnter();
    uint8_t codeCopy = tpCtx->txPacket.pdu.code;
    TbxCriticalSectionExit();
    if ((codeCopy & TBX_MB_FC_EXCEPTION_MASK) == TBX_MB_FC_EXCEPTION_MASK)
    {
      /* Increment the total number of exception responses. */
      tpCtx->diagInfo.busExcpErrCnt++;
    }
    TbxCriticalSectionEnter();
    /* Still in the INIT state and configured as a client? */
    if ( (tpCtx->state == TBX_MB_RTU_STATE_INIT) && (tpCtx->isClient == TBX_TRUE) )
    {
      /* A client could start transmitting right after system initialization, when
       * this instance is still in the INIT state. It's not user friendly to then make
       * the transmit() fail right way, meaning that the user would always have to
       * build in a wait loop hoping that this instance reached the IDLE state.
       *
       * For this reason, perform this waiting for the IDLE state right here. But with
       * a timeout mechanism. The IDLE state is reached after the reception of the last
       * byte of an ADU that a server might be transmitting, plus the t3_5 time to
       * signal the end-of-frame.
       *
       * The largest ADU to receive is 256 bytes. Adding the t3_5 end-of-frame time, 
       * means the longest time to wait for a transition to the IDLE state is:
       * 256 + 3.5 = 259.5 characters. This is ceil(259.5/3.5) = 75 times the t3_5
       * timer interval. Use this to calculate the timeout in ticks of the RTU timer.
       */
      uint16_t waitTimeoutTicks = tpCtx->t3_5Ticks * 75U;
      /* Convert it to milliseconds. Knowing that the RTU timer always runs at 20 kHz,
       * divide by 20. Just make sure to do integer roundup (A + (B-1)) / B.
       */
      uint16_t waitTimeoutMs = (waitTimeoutTicks + 19U) / 20U;
      /* Wait for the transition from INIT to IDLE with the calculated timeout. Note
       * that there is no need to check the return value. This would just mean that
       * no transition to IDLE took place before the timeout. The IDLE state check is
       * done later on in this function, so that error situation is already handled.
       * Make sure to briefly leave the critical section for calling TbxMbOsalSemTake().
       * With an RTOS this could lead to a context switch for which interrupts need to
       * be enabled.
       */
      TbxCriticalSectionExit();
      (void)TbxMbOsalSemTake(tpCtx->initStateExitSem, waitTimeoutMs);
      TbxCriticalSectionEnter();
    }
    /* New transmissions are only possible from the IDLE state. */
    uint8_t okayToTransmit = TBX_FALSE;
    if (tpCtx->state == TBX_MB_RTU_STATE_IDLE)
    {
      /* Should a response actually be transmitted? If we are a server, then upon
       * reception packet validation, txPacket.node was already set to 
       * TBX_MB_TP_NODE_ADDR_BROADCAST for us, in case of a broadcast request, which
       * does not require a response.
       */
      if ( (tpCtx->isClient == TBX_FALSE) && 
           (tpCtx->txPacket.node == TBX_MB_TP_NODE_ADDR_BROADCAST) )
      {
        /* To bypass the actual response transmission, simply update the result to
         * indicate success and keep the okayToTransmit set to its default TBX_FALSE.
         */
        result = TBX_OK;
      }
      /* Okay to transmit the response. */
      else
      {
        okayToTransmit = TBX_TRUE;
        /* Transition to the TRANSMISSION state to lock access to the txPacket for the
         * duration of the transmission. Note that the unlock happens once the state 
         * transitions back to IDLE. This happens 3.5 character times after the 
         * completion of the transmission.
         */
        tpCtx->state = TBX_MB_RTU_STATE_TRANSMISSION;
      }
    }
    TbxCriticalSectionExit();
    /* Only continue if no other packet transmission is already in progress. */
    if (okayToTransmit == TBX_TRUE)
    {
      /* Determine ADU specific properties. The ADU starts at one byte before the PDU, 
       * which is the last byte of head[]. The ADU's length is:
       * - Node address (1 byte)
       * - Function code (1 byte)
       * - Packet data (dataLen bytes)
       * - CRC16 (2 bytes)
       */
      uint8_t * aduPtr = &tpCtx->txPacket.head[TBX_MB_TP_ADU_HEAD_LEN_MAX-1U];
      uint16_t  aduLen = tpCtx->txPacket.dataLen + 4U;
      /* Populate the ADU head. For RTU it is the address field right in front of the
       * PDU. For client->server transfers the address field is the servers's node
       * address (unicast) or 0 (broadcast) and the client channel will have stored it in
       * the txPacket.node element. For server-client transfers it always the servers's
       * node address as stored when creating the RTU transport layer context.
       */
      aduPtr[0] = (tpCtx->isClient == TBX_TRUE) ? tpCtx->txPacket.node : tpCtx->nodeAddr;
      /* Populate the ADU tail. For RTU it is the CRC16 right after the PDU's data. */
      uint16_t adu_crc = TbxMbRtuCalculatCrc(aduPtr, aduLen - 2U);
      aduPtr[aduLen - 2U] = (uint8_t)adu_crc;                         /* CRC16 low.  */
      aduPtr[aduLen - 1U] = (uint8_t)(adu_crc >> 8U);                 /* CRC16 high. */
      /* Pass ADU transmit request on to the UART module. */
      result = TbxMbUartTransmit(tpCtx->port, aduPtr, aduLen);
      /* Transition back to the IDLE state, because the transmission could not be
       * started. The unlocks access to txPacket for a possible future transmission.
       */
      if (result != TBX_OK)
      {
        TbxCriticalSectionEnter();
        tpCtx->state = TBX_MB_RTU_STATE_IDLE;
        TbxCriticalSectionExit();
      }
    }
    /* Problem detected that prevented the response from being sent? */
    if (result == TBX_ERROR)
    {
      /* Increment the total number of not sent responses. */
      tpCtx->diagInfo.srvNoRespCnt++;
    }
  }
  /* Give the result back to the caller. */
  return result;
} /*** end of TbxMbRtuTransmit ***/


/************************************************************************************//**
** \brief     Signals that the caller is done with processing a reception PDU. Should be
**            called by a channel after receiving the TBX_MB_EVENT_ID_PDU_RECEIVED event
**            and no longer needing access to the PDU stored in the transport layer
**            context.
** \param     transport Handle to RTU transport layer object.
**
****************************************************************************************/
static void TbxMbRtuReceptionDone(tTbxMbTp transport)
{
  /* Verify parameters. */
  TBX_ASSERT(transport != NULL);

  /* Only continue with valid parameters. */
  if (transport != NULL)
  {
    /* Convert the TP channel pointer to the context structure. */
    tTbxMbTpCtx * tpCtx = (tTbxMbTpCtx *)transport;
    /* Sanity check on the context type. */
    TBX_ASSERT(tpCtx->type == TBX_MB_RTU_CONTEXT_TYPE);
    /* This function should only be called in the VALIDATION state. Verify this. */
    TbxCriticalSectionEnter();
    uint8_t currentState = tpCtx->state;
    TbxCriticalSectionExit();
    TBX_ASSERT(currentState == TBX_MB_RTU_STATE_VALIDATION);
    /* Only continue in the VALIDATION state. Note that in the VALIDATION state, the data
     * reception path is locked until a transition back to IDLE state is made, which is
     * handled by this function.
     */
    if (currentState == TBX_MB_RTU_STATE_VALIDATION)
    {
      /* Transistion back to the IDLE state to unlock the data reception path, allowing
       * the reception of new packets.
       */
      TbxCriticalSectionEnter();
      tpCtx->state = TBX_MB_RTU_STATE_IDLE;
      TbxCriticalSectionExit();
    }
  }
} /*** end of TbxMbRtuReceptionDone ****/


/************************************************************************************//**
** \brief     Interface function to be called by a channel to obtain read access to the 
**            reception packet. Returns NULL is the packet is currently not accessible.
**            Can be called when processing the TBX_MB_EVENT_ID_PDU_RECEIVED event.
** \param     transport Handle to RTU transport layer object.
** \return    Pointer to the packet or NULL if currently not accessible.
**
****************************************************************************************/
static tTbxMbTpPacket * TbxMbRtuGetRxPacket(tTbxMbTp transport)
{
  tTbxMbTpPacket * result = NULL;

  /* Verify parameters. */
  TBX_ASSERT(transport != NULL);

  /* Only continue with valid parameters. */
  if (transport != NULL)
  {
    /* Convert the TP channel pointer to the context structure. */
    tTbxMbTpCtx * tpCtx = (tTbxMbTpCtx *)transport;
    /* Sanity check on the context type. */
    TBX_ASSERT(tpCtx->type == TBX_MB_RTU_CONTEXT_TYPE);
    /* Access to the reception packet by a channel is only allowed in the VALIDATION
     * state. In this state the reception path is locked until a transition back to IDLE
     * state is made. This happens once the channel called receptionDoneFcn().
     */
    TbxCriticalSectionEnter();
    uint8_t currentState = tpCtx->state;
    TbxCriticalSectionExit();
    if (currentState == TBX_MB_RTU_STATE_VALIDATION)
    {
      /* Update the result. */
      result = &tpCtx->rxPacket;
    }
  }
  /* Give the result back to the caller. */
  return result;
} /*** end of TbxMbRtuGetRxPacket ***/


/************************************************************************************//**
** \brief     Interface function to be called by a channel to obtain write access to the
**            transmission packet. Returns NULL is the packet is currently not
**            accessible. Can by called to prepare the transmit packet before calling the
**            transport layer's transmitFcn().
** \param     transport Handle to RTU transport layer object.
** \return    Pointer to the packet or NULL if currently not accessible.
**
****************************************************************************************/
static tTbxMbTpPacket * TbxMbRtuGetTxPacket(tTbxMbTp transport)
{
  tTbxMbTpPacket * result = NULL;

  /* Verify parameters. */
  TBX_ASSERT(transport != NULL);

  /* Only continue with valid parameters. */
  if (transport != NULL)
  {
    /* Convert the TP channel pointer to the context structure. */
    tTbxMbTpCtx * tpCtx = (tTbxMbTpCtx *)transport;
    /* Sanity check on the context type. */
    TBX_ASSERT(tpCtx->type == TBX_MB_RTU_CONTEXT_TYPE);
    /* Access to the transmission packet by a channel is only allowed outside the 
     * TRANSMISSION state. In this state the transmission path is locked until a
     * transition back to IDLE state is made. This happens once the transport layer
     * completed the packet transmission.
     */
    TbxCriticalSectionEnter();
    uint8_t currentState = tpCtx->state;
    TbxCriticalSectionExit();
    if (currentState != TBX_MB_RTU_STATE_TRANSMISSION)
    {
      /* Update the result. */
      result = &tpCtx->txPacket;
    }
  }
  /* Give the result back to the caller. */
  return result;
} /*** end of TbxMbRtuGetTxPacket ***/


/************************************************************************************//**
** \brief     Validates a newly received communication packet, stored in the transport
**            layer object.
** \param     transport Handle to RTU transport layer object.
** \return    TBX_OK if successful, TBX_ERROR otherwise. 
**
****************************************************************************************/
static uint8_t TbxMbRtuValidate(tTbxMbTp transport)
{
  uint8_t result = TBX_ERROR;

  /* Verify parameters. */
  TBX_ASSERT(transport != NULL);

  /* Only continue with valid parameters. */
  if (transport != NULL)
  {
    /* Convert the TP channel pointer to the context structure. */
    tTbxMbTpCtx * tpCtx = (tTbxMbTpCtx *)transport;
    /* Sanity check on the context type. */
    TBX_ASSERT(tpCtx->type == TBX_MB_RTU_CONTEXT_TYPE);
    /* This function should only be called in the VALIDATION state. Verify this. */
    TbxCriticalSectionEnter();
    uint8_t currentState = tpCtx->state;
    TbxCriticalSectionExit();
    TBX_ASSERT(currentState == TBX_MB_RTU_STATE_VALIDATION);
    /* Only continue in the VALIDATION state. Note that in the VALIDATION state, the data
     * reception path is locked until a transition back to IDLE state is made.
     * Consequenty, there is no need for critical sections when accessing the .rxXyz
     * elements of the TP context. 
     */
    if (currentState == TBX_MB_RTU_STATE_VALIDATION)
    {
      /* Increment the total number of received packets, regardless of addressing or
       * CRC.
       */
      tpCtx->diagInfo.busMsgCnt++;
      /* The ADU for an RTU packet starts at one byte before the PDU, which is the last
       * byte of head[]. Get the pointer of where the ADU starts in the rxPacket.
       */
      uint8_t * aduPtr = &tpCtx->rxPacket.head[TBX_MB_TP_ADU_HEAD_LEN_MAX-1U];
      /* The CRC16 is stored in the 2 bytes after the PDU data bytes:
       * - Node address (1 byte)
       * - Function code (1 byte)
       * - Packet data (dataLen bytes)
       * - CRC16 (2 bytes)
       */
      uint8_t const * crcPtr = &aduPtr[2U + tpCtx->rxPacket.dataLen];
      /* Read out the CRC16 stored in the ADU packet. */
      uint16_t packetCrc = crcPtr[0] | (uint16_t)(crcPtr[1] << 8U);
      /* Calculate the CRC16 based on the packet contents. It's calculated over the
       * entire ADU data, just excluding the last two byte with the CRC16.
       */
      uint16_t calcCrc = TbxMbRtuCalculatCrc(aduPtr, tpCtx->rxPacket.dataLen + 2U);
      /* Are the two CRC16s a mismatch? */
      if (packetCrc != calcCrc)
      {
        /* Increment the total number of received packets with an incorrect CRC. */
        tpCtx->diagInfo.busCommErrCnt++;
      }
      /* CRC16 check passed. */
      else
      {
        /* Continue checking if the ADU is addressed to us. This check is different for a
         * server and a client. Start with the server case.
         */
        if (tpCtx->isClient == TBX_FALSE)
        {
          /* Only process frames that are addressed to us (unicast or broadcast). */
          if ((tpCtx->rxPacket.node == tpCtx->nodeAddr) ||
              (tpCtx->rxPacket.node == TBX_MB_TP_NODE_ADDR_BROADCAST))
          {
            /* Increment the total number of received packets with a correct CRC, that
             * were addressed to us. Either via unicast of broadcast.
             */
            tpCtx->diagInfo.srvMsgCnt++;
            /* Set the node address in the txPacket node element. It is used during
             * transmission to decide if the actual sending of the response should be
             * suppressed, which is the case for TBX_MB_TP_NODE_ADDR_BROADCAST. No need
             * for a critical section, because we are guaranteed not in the IDLE or
             * TRANSMISSION states.
             */
            tpCtx->txPacket.node = tpCtx->rxPacket.node;
            /* Packet is valid. Update the result accordingly. */
            result = TBX_OK;
          }
        }
        /* Linked to a client channel. */
        else
        {
          /* Only process frames that are send from a valid server. */
          if ( (tpCtx->rxPacket.node >= TBX_MB_TP_NODE_ADDR_MIN) ||
               (tpCtx->rxPacket.node <= TBX_MB_TP_NODE_ADDR_MAX) )
          {
            /* Packet is valid. Update the result accordingly. */
            result = TBX_OK;
          }
        }
      }
    }
  }
  /* Give the result back to the caller. */
  return result;
} /*** end of TbxMbRtuValidate ***/


/************************************************************************************//**
** \brief     Event function to signal to this module that the entire transfer completed.
** \attention This function should be called by the UART module.
** \details   This function accesses the transport layer context, which is a shared
**            resource. Even though this function is called at UART Tx interrupt level,
**            it is still necessary to access the transport layer context through a
**            critical section. On a multicore target, the event thread might run on
**            one core, while this interrupt runs on another core. A critical section
**            for such a target manages a spin lock, needed to have mutual exclusive
**            access to the shared resource.
** \param     port The serial port that the transfer completed on.
**
****************************************************************************************/
static void TbxMbRtuTransmitComplete(tTbxMbUartPort port)
{
  /* Verify parameters. */
  TBX_ASSERT(port < TBX_MB_UART_NUM_PORT);

  /* Only continue with valid parameters. */
  if (port < TBX_MB_UART_NUM_PORT)
  {
    /* Obtain transport layer context linked to UART port of this event. */
    tTbxMbTpCtx volatile * tpCtx = tbxMbRtuCtx[port];
    /* Verify transport layer context. */
    TBX_ASSERT(tpCtx != NULL)
    /* Only continue with a valid transport layer context. Note that there is no need
     * to also check the transport layer type, because only RTU types are stored in the
     * tbxMbRtuCtx[] array.
     */
    if (tpCtx != NULL)
    {
      TbxCriticalSectionEnter();
      uint8_t stateCopy = tpCtx->state;
      TbxCriticalSectionExit();
      /* This function should only be called when in the TRANSMISSION state. Verify
       * this. 
       */
      TBX_ASSERT(stateCopy == TBX_MB_RTU_STATE_TRANSMISSION);
      /* Only continue in the TRANSMISSION state. */
      if (stateCopy == TBX_MB_RTU_STATE_TRANSMISSION)
      {
        /* Store the time that the transmission completed. */
        TbxCriticalSectionEnter();
        tpCtx->txDoneTime = TbxMbPortTimerCount();
        TbxCriticalSectionExit();
        /* Instruct the event task to start calling our polling function. Needed to
         * detect the 3.5 character timeout, after which we can transition back to the
         * IDLE state.
         */
        tTbxMbEvent newEvent;
        newEvent.context = (void *)tpCtx;
        newEvent.id = TBX_MB_EVENT_ID_START_POLLING;
        TbxMbOsalEventPost(&newEvent, TBX_TRUE);
      }
    }
  }
} /*** end of TbxMbRtuTransmitComplete ***/


/************************************************************************************//**
** \brief     Event function to signal the reception of new data to this module.
** \attention This function should be called by the UART module. 
** \details   This function accesses the transport layer context, which is a shared
**            resource. Even though this function is called at UART Tx interrupt level,
**            it is still necessary to access the transport layer context through a
**            critical section. On a multicore target, the event thread might run on
**            one core, while this interrupt runs on another core. A critical section
**            for such a target manages a spin lock, needed to have mutual exclusive
**            access to the shared resource.
** \param     port The serial port that the transfer completed on.
** \param     data Byte array with newly received data.
** \param     len Number of newly received bytes.
**
****************************************************************************************/
static void TbxMbRtuDataReceived(tTbxMbUartPort         port, 
                                 uint8_t        const * data, 
                                 uint8_t                len)
{
  /* Verify parameters. */
  TBX_ASSERT((port < TBX_MB_UART_NUM_PORT) && 
             (data != NULL) &&
             (len > 0U));

  /* Only continue with valid parameters. */
  if ((port < TBX_MB_UART_NUM_PORT) && 
      (data != NULL) &&
      (len > 0U))
  {
    /* Obtain transport layer context linked to UART port of this event. */
    tTbxMbTpCtx volatile * tpCtx = tbxMbRtuCtx[port];
    /* Verify transport layer context. */
    TBX_ASSERT(tpCtx != NULL)
    /* Only continue with a valid transport layer context. Note that there is no need
     * to also check the transport layer type, because only RTU types are stored in the
     * tbxMbRtuCtx[] array.
     */
    if (tpCtx != NULL)
    {
      /* Get current time in RTU timer ticks. */
      uint16_t currentTime = TbxMbPortTimerCount();
      TbxCriticalSectionEnter();
      /* Store the reception timestamp but first make a backup of the old timestamp, 
       * which is needed later on to do the 1.5 character timeout detection.
       */
      #if (TBX_MB_RTU_T1_5_TIMEOUT_ENABLE > 0U)        
      uint16_t oldRxTime = tpCtx->rxTime;
      #endif
      tpCtx->rxTime = currentTime;
      /* The ADU for an RTU packet starts at one byte before the PDU, which is the last
       * byte of head[]. Get the pointer of where the ADU starts in the rxPacket.
       */
      uint8_t volatile * aduPtr = &tpCtx->rxPacket.head[TBX_MB_TP_ADU_HEAD_LEN_MAX-1U];
      /* Get copy of the state so the we can exit the critical section. */
      uint8_t stateCopy = tpCtx->state;
      TbxCriticalSectionExit();
      /* Are we in the RECEPTION state? Make sure to check this one first, as it will 
       * happen the most.
       */
      if (stateCopy == TBX_MB_RTU_STATE_RECEPTION)
      {
        TbxCriticalSectionEnter();
        #if (TBX_MB_RTU_T1_5_TIMEOUT_ENABLE > 0U)        
        /* Check if a 1.5 character timeout occurred since the last reception. Note that
         * this calculation works, even if the RTU timer counter overflowed.
         */
        uint16_t deltaTicks = tpCtx->rxTime - oldRxTime;
        if (deltaTicks >= tpCtx->t1_5Ticks)
        {
          /* Flag frame as not okay (NOK). */
          tpCtx->rxAduOkay = TBX_FALSE;
        }
        #endif
        /* Check if the newly received data would still fit. Note that an ADU on RTU can
         * have max 256 bytes:
         * - Node address (1 byte)
         * - Function code (1 byte)
         * - Packet data (max 252 bytes)
         * - CRC16 (2 bytes)
         */
        if ((tpCtx->rxAduWrIdx + len) > 256U)
        {
          /* Flag frame as not okay (NOK). */
          tpCtx->rxAduOkay = TBX_FALSE;
        }
        /* Only process the newly received data if the ADU reception frame is still
         * flagged as OK. If not, then eventually a 3.5 character idle time will be
         * detected to mark the end of the packet/frame. At which point its data will be
         * discarded.
         */
        if (tpCtx->rxAduOkay == TBX_TRUE)
        {
          /* Append the received data to the ADU. */
          for (uint8_t idx = 0U; idx < len; idx++)
          {
            aduPtr[tpCtx->rxAduWrIdx + idx] = data[idx];
          }
          /* Update the write indexer into the ADU reception packet. */
          tpCtx->rxAduWrIdx += len;
        }
        TbxCriticalSectionExit();
      }
      /* Are we in the IDLE state? */
      else if (stateCopy == TBX_MB_RTU_STATE_IDLE)
      {
        TbxCriticalSectionEnter();
        /* Transition to the RECEIVING state. */
        tpCtx->state = TBX_MB_RTU_STATE_RECEPTION;
        /* Copy the received data at the start of the ADU. Note that there is no need
         * to do a check to see if it fits in the ADU buffer. The ADU can have up to
         * 256 bytes and the len parameter is an unsigned 8-bit so that always fits.
         */
        for (uint8_t idx = 0U; idx < len; idx++)
        {
          aduPtr[idx] = data[idx];
        }
        /* Initialize the write indexer into the ADU reception packet, while taking into
         * account the bytes that were just written.
         */
        tpCtx->rxAduWrIdx = len;
        /* Initialize frame OK/NOK flag to okay so far. */
        tpCtx->rxAduOkay = TBX_TRUE;
        TbxCriticalSectionExit();
        /* Instruct the event task to call our polling function to be able to determine
         * when the 3.5 character idle time occurred, which marks the end of the packet.
         */
        tTbxMbEvent newEvent;
        newEvent.context = (void *)tpCtx;
        newEvent.id = TBX_MB_EVENT_ID_START_POLLING;
        TbxMbOsalEventPost(&newEvent, TBX_TRUE);
      }
      else
      {
        /* Nothing left to do, but MISRA requires this terminating else statement. */
      }
    }
  }
} /*** end of TbxMbRtuDataReceived ***/


/************************************************************************************//**
** \brief     Calculates the Modbus RTU defined CRC16 checksum over the bytes in the
**            specified data array.
** \param     data Pointer to the byte array with data.
** \param     len Number of data bytes to include in the CRC16 calculation.
** \return    The calculated CRC16 checksum value.
**
****************************************************************************************/
static uint16_t TbxMbRtuCalculatCrc(uint8_t  const * data, 
                                    uint16_t         len)
{
  /* Lookup table for fast CRC16 calculation. Made static to lower the stack load. */
  static const uint16_t tbxMbRtuCrcTable[] =
  {
    0x0000, 0xC0C1, 0xC181, 0x0140, 0xC301, 0x03C0, 0x0280, 0xC241,
    0xC601, 0x06C0, 0x0780, 0xC741, 0x0500, 0xC5C1, 0xC481, 0x0440,
    0xCC01, 0x0CC0, 0x0D80, 0xCD41, 0x0F00, 0xCFC1, 0xCE81, 0x0E40,
    0x0A00, 0xCAC1, 0xCB81, 0x0B40, 0xC901, 0x09C0, 0x0880, 0xC841,
    0xD801, 0x18C0, 0x1980, 0xD941, 0x1B00, 0xDBC1, 0xDA81, 0x1A40,
    0x1E00, 0xDEC1, 0xDF81, 0x1F40, 0xDD01, 0x1DC0, 0x1C80, 0xDC41,
    0x1400, 0xD4C1, 0xD581, 0x1540, 0xD701, 0x17C0, 0x1680, 0xD641,
    0xD201, 0x12C0, 0x1380, 0xD341, 0x1100, 0xD1C1, 0xD081, 0x1040,
    0xF001, 0x30C0, 0x3180, 0xF141, 0x3300, 0xF3C1, 0xF281, 0x3240,
    0x3600, 0xF6C1, 0xF781, 0x3740, 0xF501, 0x35C0, 0x3480, 0xF441,
    0x3C00, 0xFCC1, 0xFD81, 0x3D40, 0xFF01, 0x3FC0, 0x3E80, 0xFE41,
    0xFA01, 0x3AC0, 0x3B80, 0xFB41, 0x3900, 0xF9C1, 0xF881, 0x3840,
    0x2800, 0xE8C1, 0xE981, 0x2940, 0xEB01, 0x2BC0, 0x2A80, 0xEA41,
    0xEE01, 0x2EC0, 0x2F80, 0xEF41, 0x2D00, 0xEDC1, 0xEC81, 0x2C40,
    0xE401, 0x24C0, 0x2580, 0xE541, 0x2700, 0xE7C1, 0xE681, 0x2640,
    0x2200, 0xE2C1, 0xE381, 0x2340, 0xE101, 0x21C0, 0x2080, 0xE041,
    0xA001, 0x60C0, 0x6180, 0xA141, 0x6300, 0xA3C1, 0xA281, 0x6240,
    0x6600, 0xA6C1, 0xA781, 0x6740, 0xA501, 0x65C0, 0x6480, 0xA441,
    0x6C00, 0xACC1, 0xAD81, 0x6D40, 0xAF01, 0x6FC0, 0x6E80, 0xAE41,
    0xAA01, 0x6AC0, 0x6B80, 0xAB41, 0x6900, 0xA9C1, 0xA881, 0x6840,
    0x7800, 0xB8C1, 0xB981, 0x7940, 0xBB01, 0x7BC0, 0x7A80, 0xBA41,
    0xBE01, 0x7EC0, 0x7F80, 0xBF41, 0x7D00, 0xBDC1, 0xBC81, 0x7C40,
    0xB401, 0x74C0, 0x7580, 0xB541, 0x7700, 0xB7C1, 0xB681, 0x7640,
    0x7200, 0xB2C1, 0xB381, 0x7340, 0xB101, 0x71C0, 0x7080, 0xB041,
    0x5000, 0x90C1, 0x9181, 0x5140, 0x9301, 0x53C0, 0x5280, 0x9241,
    0x9601, 0x56C0, 0x5780, 0x9741, 0x5500, 0x95C1, 0x9481, 0x5440,
    0x9C01, 0x5CC0, 0x5D80, 0x9D41, 0x5F00, 0x9FC1, 0x9E81, 0x5E40,
    0x5A00, 0x9AC1, 0x9B81, 0x5B40, 0x9901, 0x59C0, 0x5880, 0x9841,
    0x8801, 0x48C0, 0x4980, 0x8941, 0x4B00, 0x8BC1, 0x8A81, 0x4A40,
    0x4E00, 0x8EC1, 0x8F81, 0x4F40, 0x8D01, 0x4DC0, 0x4C80, 0x8C41,
    0x4400, 0x84C1, 0x8581, 0x4540, 0x8701, 0x47C0, 0x4680, 0x8641,
    0x8201, 0x42C0, 0x4380, 0x8341, 0x4100, 0x81C1, 0x8081, 0x4040
  };
  uint16_t result = 0U;
  uint16_t crc = 0xFFFFU;

  /* Loop over all the data bytes. */
  for (uint16_t byteIdx = 0; byteIdx < len; byteIdx++)
  {
    /* Calculate the index into the CRC lookup table. */
    uint8_t tableIdx = data[byteIdx] ^ crc;
    crc >>= 8;
    crc ^= tbxMbRtuCrcTable[tableIdx];
  }
  /* Update the result. */
  result = crc;
  /* Give the result back to the caller. */
  return result;
} /*** end of TbxMbRtuCalculatCrc ***/


/*********************************** end of tbxmb_rtu.c ********************************/
