/****************************************************************************************
|  Description: bootloader CAN communication interface source file
|    File Name: can.c
|
|----------------------------------------------------------------------------------------
|                          C O P Y R I G H T
|----------------------------------------------------------------------------------------
|   Copyright (c) 2012  by Feaser    http://www.feaser.com    All rights reserved
|
|----------------------------------------------------------------------------------------
|                            L I C E N S E
|----------------------------------------------------------------------------------------
| This file is part of OpenBLT. OpenBLT is free software: you can redistribute it and/or
| modify it under the terms of the GNU General Public License as published by the Free
| Software Foundation, either version 3 of the License, or (at your option) any later
| version.
|
| OpenBLT is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
| without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
| PURPOSE. See the GNU General Public License for more details.
|
| You should have received a copy of the GNU General Public License along with OpenBLT.
| If not, see <http://www.gnu.org/licenses/>.
|
| A special exception to the GPL is included to allow you to distribute a combined work 
| that includes OpenBLT without being obliged to provide the source code for any 
| proprietary components. The exception text is included at the bottom of the license
| file <license.html>.
| 
****************************************************************************************/


/****************************************************************************************
* Include files
****************************************************************************************/
#include "boot.h"                                /* bootloader generic header          */
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#if (BOOT_COM_CAN_ENABLE > 0)
#include "driverlib/sysctl.h"
#include "driverlib/canlib.h"
#endif


#if (BOOT_COM_CAN_ENABLE > 0)
/****************************************************************************************
* Macro definitions
****************************************************************************************/
/* index of the used transmission and reception message objects */
#define CAN_RX_MSGOBJECT_IDX   (0)
#define CAN_TX_MSGOBJECT_IDX   (1)


/****************************************************************************************
* Local constant declarations
****************************************************************************************/
/* lookup table to quickly and efficiently convert a bit number to a bit mask */
static const blt_int16u canBitNum2Mask[] =
{
  0x0001, /* bit  0 */
  0x0002  /* bit  1 */
};


/****************************************************************************************
** NAME:           CanSetBittiming
** PARAMETER:      none
** RETURN VALUE:   BLT_TRUE if a valid bittiming configuration was found and set. 
**                 BLT_FALSE otherwise.
** DESCRIPTION:    Attempts to match the bittiming parameters to the requested baudrate 
**                 for a sample point between 65 and 75%, through a linear search 
**                 algorithm. It is based on the equation:
**                  baudrate = CAN Clock Freq/((1+PropSeg+Phase1Seg+Phase2Seg)*Prescaler)
**
****************************************************************************************/
static blt_int8u CanSetBittiming(void)
{
  tCANBitClkParms bitClkParms;
  blt_int8u       samplepoint;

  /* init SJW to maximum value */
  bitClkParms.uSJW = 4;

  /* use a double loop to iterate through all possible settings of uSyncPropPhase1Seg
   * and uPhase2Seg.
   */
  for (bitClkParms.uSyncPropPhase1Seg = 16; bitClkParms.uSyncPropPhase1Seg >= 1; bitClkParms.uSyncPropPhase1Seg--)
  {
    for (bitClkParms.uPhase2Seg = 8; bitClkParms.uPhase2Seg >= 1; bitClkParms.uPhase2Seg--)
    {
      samplepoint = ((1+bitClkParms.uSyncPropPhase1Seg) * 100) / (1+bitClkParms.uSyncPropPhase1Seg+bitClkParms.uPhase2Seg);
      /* check that sample points is within the preferred range */
      if ( (samplepoint >= 65) && (samplepoint <= 75) )
      {
        /* does a prescaler exist to get the exact baudrate with these bittiming
         * settings?
         */
        if ((((BOOT_CPU_XTAL_SPEED_KHZ*1000)/BOOT_COM_CAN_BAUDRATE) % (1+bitClkParms.uSyncPropPhase1Seg+bitClkParms.uPhase2Seg)) == 0)
        {
          /* bittiming configuration found. now update SJW to that it is never greater
           * than one of the phase segments. Giving the fact that the sample point is
           * rather high, only phase seg 2 need to be considered for this.
           */
          if (bitClkParms.uPhase2Seg < 4)
          {
           bitClkParms.uSJW = bitClkParms.uPhase2Seg;
          }
          /* calculate the actual prescaler value */
          bitClkParms.uQuantumPrescaler = ((BOOT_CPU_XTAL_SPEED_KHZ*1000)/BOOT_COM_CAN_BAUDRATE)/(1+bitClkParms.uSyncPropPhase1Seg+bitClkParms.uPhase2Seg);
          /* apply this bittiming configuration */
          CANSetBitTiming(CAN0_BASE, &bitClkParms);
          /* break loop and return from function */
          return BLT_TRUE;
        }
      }
    }
  }
  /* no valid bittiming configuration found */
  return BLT_FALSE;
} /*** end of CanSetBittiming ***/


/****************************************************************************************
** NAME:           CanInit
** PARAMETER:      none
** RETURN VALUE:   none
** DESCRIPTION:    Initializes the CAN controller and synchronizes it to the CAN bus.
**
****************************************************************************************/
void CanInit(void)
{
  blt_bool      result;
  tCANMsgObject rxMsgObject;

  /* the current implementation supports CAN0. throw an assertion error in case a 
   * different CAN channel is configured.  
   */
  ASSERT_CT(BOOT_COM_CAN_CHANNEL_INDEX == 0); 
  /* enable the CAN controller */
  SysCtlPeripheralEnable(SYSCTL_PERIPH_CAN0);
  /* reset the state of the CAN controller, including the message objects */
  CANInit(CAN0_BASE);
  /* set the bittiming */
  result = CanSetBittiming();
  ASSERT_RT(result == BLT_TRUE);
  /* take the CAN controller out of the initialization state */
  CANEnable(CAN0_BASE);
  /* setup message object 1 to receive the BOOT_COM_CAN_RX_MSG_ID message*/
  rxMsgObject.ulMsgID = BOOT_COM_CAN_RX_MSG_ID;
  rxMsgObject.ulMsgIDMask = 0x7ff;
  rxMsgObject.ulFlags = MSG_OBJ_USE_ID_FILTER;
  rxMsgObject.ulMsgLen = 8;
  CANMessageSet(CAN0_BASE, CAN_RX_MSGOBJECT_IDX+1, &rxMsgObject, MSG_OBJ_TYPE_RX);
} /*** end of CanInit ***/


/****************************************************************************************
** NAME:           CanTransmitPacket
** PARAMETER:      data pointer to byte array with data that it to be transmitted.
**                 len  number of bytes that are to be transmitted.
** RETURN VALUE:   none
** DESCRIPTION:    Transmits a packet formatted for the communication interface.
**
****************************************************************************************/
void CanTransmitPacket(blt_int8u *data, blt_int8u len)
{
  blt_int32u    status;
  tCANMsgObject msgObject;

  /* get bitmask of message objects that are busy transmitting messages */
  status = CANStatusGet(CAN0_BASE, CAN_STS_TXREQUEST);
  /* make sure that the transmit message object is ready to accept a new message */
  ASSERT_RT((status & canBitNum2Mask[CAN_TX_MSGOBJECT_IDX]) == 0);
  /* prepare the message and submit it for transmission */
  msgObject.ulMsgID = BOOT_COM_CAN_TX_MSG_ID;
  msgObject.ulFlags = 0;
  msgObject.ulMsgLen = len;
  msgObject.pucMsgData = data;
  CANMessageSet(CAN0_BASE, CAN_TX_MSGOBJECT_IDX+1, &msgObject, MSG_OBJ_TYPE_TX);
  /* now wait for the transmission to complete */
  do
  {
    status = CANStatusGet(CAN0_BASE, CAN_STS_TXREQUEST);
    /* service the watchdog */
    CopService();
  }
  while ((status & canBitNum2Mask[CAN_TX_MSGOBJECT_IDX]) != 0);
} /*** end of CanTransmitPacket ***/


/****************************************************************************************
** NAME:           CanReceivePacket
** PARAMETER:      data pointer to byte array where the data is to be stored.
** RETURN VALUE:   BLT_TRUE is a packet was received, BLT_FALSE otherwise.
** DESCRIPTION:    Receives a communication interface packet if one is present.
**
****************************************************************************************/
blt_bool CanReceivePacket(blt_int8u *data)
{
  blt_int32u    status;
  tCANMsgObject msgObject;

  /* get bitmask of message objects with new data */
  status = CANStatusGet(CAN0_BASE, CAN_STS_NEWDAT);
  /* check if the BOOT_COM_CAN_RX_MSG_ID message was received */
  if ((status & canBitNum2Mask[CAN_RX_MSGOBJECT_IDX]) == 0)
  {
    /* message not received */
    return BLT_FALSE;
  }
  /* read the message data */
  msgObject.pucMsgData = data;
  CANMessageGet(CAN0_BASE, CAN_RX_MSGOBJECT_IDX+1, &msgObject, true);
  /* message was successfully received */
  return BLT_TRUE;
} /*** end of CanReceivePacket ***/
#endif /* BOOT_COM_CAN_ENABLE > 0 */


/*********************************** end of can.c **************************************/
