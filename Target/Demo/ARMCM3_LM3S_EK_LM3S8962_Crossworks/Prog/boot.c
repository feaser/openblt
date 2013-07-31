/************************************************************************************//**
* \file         Demo\ARMCM3_LM3S_EK_LM3S8962_Crossworks\Prog\boot.c
* \brief        Demo program bootloader interface source file.
* \ingroup      Prog_ARMCM3_LM3S_EK_LM3S8962_Crossworks
* \internal
*----------------------------------------------------------------------------------------
*                          C O P Y R I G H T
*----------------------------------------------------------------------------------------
*   Copyright (c) 2012  by Feaser    http://www.feaser.com    All rights reserved
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
#include "header.h"                                    /* generic header               */


/************************************************************************************//**
** \brief     Bootloader activation function.
** \return    none.
**
****************************************************************************************/
static void BootActivate(void)
{
  void (*pEntryFromProgFnc)(void);

  /* stop the timer from generating interrupts */
  TimeDeinit();
  /* set pointer to the address of function EntryFromProg in the bootloader. note that
   * 1 is added to this address to enable a switch from Thumb2 to Thumb mode
   */
  pEntryFromProgFnc = (void*)0x000000F0 + 1;
  /* call EntryFromProg to activate the bootloader. */
  pEntryFromProgFnc();
} /*** end of BootActivate ***/


#if (BOOT_COM_UART_ENABLE > 0)
/****************************************************************************************
*     U N I V E R S A L   A S Y N C H R O N O U S   R X   T X   I N T E R F A C E
****************************************************************************************/

/****************************************************************************************
* Function prototypes
****************************************************************************************/
static unsigned char UartReceiveByte(unsigned char *data);


/************************************************************************************//**
** \brief     Initializes the UART communication interface.
** \return    none.
**
****************************************************************************************/
void BootComInit(void)
{
  /* enable the UART0 peripheral */
  SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
  /* enable the and configure UART0 related peripherals and pins */
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
  GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
  /* configure the UART0 baudrate and communication parameters */
  UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), BOOT_COM_UART_BAUDRATE,
                      (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | 
                      UART_CONFIG_PAR_NONE));
} /*** end of BootComInit ***/


/************************************************************************************//**
** \brief     Receives the CONNECT request from the host, which indicates that the
**            bootloader should be activated and, if so, activates it.
** \return    none.
**
****************************************************************************************/
void BootComCheckActivationRequest(void)
{
  static unsigned char xcpCtoReqPacket[BOOT_COM_UART_RX_MAX_DATA+1];
  static unsigned char xcpCtoRxLength;
  static unsigned char xcpCtoRxInProgress = 0;

  /* start of cto packet received? */
  if (xcpCtoRxInProgress == 0)
  {
    /* store the message length when received */
    if (UartReceiveByte(&xcpCtoReqPacket[0]) == 1)
    {
      /* indicate that a cto packet is being received */
      xcpCtoRxInProgress = 1;

      /* reset packet data count */
      xcpCtoRxLength = 0;
    }
  }
  else
  {
    /* store the next packet byte */
    if (UartReceiveByte(&xcpCtoReqPacket[xcpCtoRxLength+1]) == 1)
    {
      /* increment the packet data count */
      xcpCtoRxLength++;

      /* check to see if the entire packet was received */
      if (xcpCtoRxLength == xcpCtoReqPacket[0])
      {
        /* done with cto packet reception */
        xcpCtoRxInProgress = 0;

        /* check if this was an XCP CONNECT command */
        if ((xcpCtoReqPacket[1] == 0xff) && (xcpCtoReqPacket[2] == 0x00))
        {
          /* connection request received so start the bootloader */
          BootActivate();
        }
      }
    }
  }
} /*** end of BootComCheckActivationRequest ***/


/************************************************************************************//**
** \brief     Receives a communication interface byte if one is present.
** \param     data Pointer to byte where the data is to be stored.
** \return    1 if a byte was received, 0 otherwise.
**
****************************************************************************************/
static unsigned char UartReceiveByte(unsigned char *data)
{
  signed long result;

  /* try to read a newly received byte */
  result = UARTCharGetNonBlocking(UART0_BASE);
  /* check if a new byte was received */
  if(result != -1)
  {
    /* store the received byte */
    *data = (unsigned char)result;
    /* inform caller of the newly received byte */
    return 1;
  }
  /* inform caller that no new data was received */
  return 0;
} /*** end of UartReceiveByte ***/
#endif /* BOOT_COM_UART_ENABLE > 0 */


#if (BOOT_COM_CAN_ENABLE > 0)
/****************************************************************************************
*        C O N T R O L L E R   A R E A   N E T W O R K   I N T E R F A C E
****************************************************************************************/

/****************************************************************************************
* Macro definitions
****************************************************************************************/
/** \brief Index of the used reception message object. */
#define CAN_RX_MSGOBJECT_IDX   (0)


/****************************************************************************************
* Local constant declarations
****************************************************************************************/
/** \brief Lookup table to quickly and efficiently convert a bit number to a bit mask. */
static const unsigned short canBitNum2Mask[] =
{
  0x0001, /* bit  0 */
};


/************************************************************************************//**
** \brief     Attempts to match the bittiming parameters to the requested baudrate 
**            for a sample point between 65 and 75%, through a linear search 
**            algorithm. It is based on the equation:
**              baudrate = CAN Clock Freq/((1+PropSeg+Phase1Seg+Phase2Seg)*Prescaler)
** \return    1 if a valid bittiming configuration was found and set. 0 otherwise. 
**
****************************************************************************************/
static unsigned char CanSetBittiming(void)
{
  tCANBitClkParms bitClkParms;
  unsigned char samplepoint;

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
          return 1;
        }
      }
    }
  }
  /* no valid bittiming configuration found */
  return 0;
} /*** end of CanSetBittiming ***/


/************************************************************************************//**
** \brief     Initializes the CAN communication interface.
** \return    none.
**
****************************************************************************************/
void BootComInit(void)
{
  tCANMsgObject rxMsgObject;

  /* configure the CAN pins */
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
  GPIOPinTypeCAN(GPIO_PORTD_BASE, GPIO_PIN_0 | GPIO_PIN_1);
  /* enable the CAN controller */
  SysCtlPeripheralEnable(SYSCTL_PERIPH_CAN0);
  /* reset the state of the CAN controller, including the message objects */
  CANInit(CAN0_BASE);
  /* set the bittiming */
  CanSetBittiming();
  /* take the CAN controller out of the initialization state */
  CANEnable(CAN0_BASE);
  /* setup message object 1 to receive the BOOT_COM_CAN_RX_MSG_ID message*/
  rxMsgObject.ulMsgID = BOOT_COM_CAN_RX_MSG_ID;
  rxMsgObject.ulMsgIDMask = 0x7ff;
  rxMsgObject.ulFlags = MSG_OBJ_USE_ID_FILTER;
  rxMsgObject.ulMsgLen = 8;
  CANMessageSet(CAN0_BASE, CAN_RX_MSGOBJECT_IDX+1, &rxMsgObject, MSG_OBJ_TYPE_RX);
} /*** end of BootComInit ***/


/************************************************************************************//**
** \brief     Receives the CONNECT request from the host, which indicates that the
**            bootloader should be activated and, if so, activates it.
** \return    none.
**
****************************************************************************************/
void BootComCheckActivationRequest(void)
{
  unsigned long status;
  tCANMsgObject msgObject;
  unsigned char msgData[8];

  /* get bitmask of message objects with new data */
  status = CANStatusGet(CAN0_BASE, CAN_STS_NEWDAT);
  /* check if the BOOT_COM_CAN_RX_MSG_ID message was received */
  if ((status & canBitNum2Mask[CAN_RX_MSGOBJECT_IDX]) != 0)
  {
    /* read the message data */
    msgObject.pucMsgData = msgData;
    CANMessageGet(CAN0_BASE, CAN_RX_MSGOBJECT_IDX+1, &msgObject, true);
    /* check if this was an XCP CONNECT command */
    if ((msgData[0] == 0xff) && (msgData[1] == 0x00))
    {
      /* connection request received so start the bootloader */
      BootActivate();
    }
  }
} /*** end of BootComCheckActivationRequest ***/
#endif /* BOOT_COM_CAN_ENABLE > 0 */


/*********************************** end of boot.c *************************************/
