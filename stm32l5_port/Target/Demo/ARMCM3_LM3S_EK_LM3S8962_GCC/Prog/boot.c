/************************************************************************************//**
* \file         Demo/ARMCM3_LM3S_EK_LM3S8962_GCC/Prog/boot.c
* \brief        Demo program bootloader interface source file.
* \ingroup      Prog_ARMCM3_LM3S_EK_LM3S8962_GCC
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
* You have received a copy of the GNU General Public License along with OpenBLT. It 
* should be located in ".\Doc\license.html". If not, contact Feaser to obtain a copy.
* 
* \endinternal
****************************************************************************************/

/****************************************************************************************
* Include files
****************************************************************************************/
#include "header.h"                                    /* generic header               */


/****************************************************************************************
* Function prototypes
****************************************************************************************/
#if (BOOT_COM_RS232_ENABLE > 0)
static void BootComRs232Init(void);
static void BootComRs232CheckActivationRequest(void);
#endif
#if (BOOT_COM_CAN_ENABLE > 0)
static void BootComCanInit(void);
static void BootComCanCheckActivationRequest(void);
#endif

/************************************************************************************//**
** \brief     Initializes the communication interface.
** \return    none.
**
****************************************************************************************/
void BootComInit(void)
{
#if (BOOT_COM_RS232_ENABLE > 0)
  BootComRs232Init();
#endif
#if (BOOT_COM_CAN_ENABLE > 0)
  BootComCanInit();
#endif
} /*** end of BootComInit ***/


/************************************************************************************//**
** \brief     Receives the CONNECT request from the host, which indicates that the
**            bootloader should be activated and, if so, activates it.
** \return    none.
**
****************************************************************************************/
void BootComCheckActivationRequest(void)
{
#if (BOOT_COM_RS232_ENABLE > 0)
  BootComRs232CheckActivationRequest();
#endif
#if (BOOT_COM_CAN_ENABLE > 0)
  BootComCanCheckActivationRequest();
#endif
} /*** end of BootComCheckActivationRequest ***/


/************************************************************************************//**
** \brief     Bootloader activation function.
** \return    none.
**
****************************************************************************************/
void BootActivate(void)
{
  /* perform software reset to activate the bootoader again */
  SysCtlReset();
} /*** end of BootActivate ***/


#if (BOOT_COM_RS232_ENABLE > 0)
/****************************************************************************************
*     U N I V E R S A L   A S Y N C H R O N O U S   R X   T X   I N T E R F A C E
****************************************************************************************/

/****************************************************************************************
* Macro definitions
****************************************************************************************/
/** \brief Timeout time for the reception of a CTO packet. The timer is started upon
 *         reception of the first packet byte.
 */
#define RS232_CTO_RX_PACKET_TIMEOUT_MS (100u)


/****************************************************************************************
* Function prototypes
****************************************************************************************/
static unsigned char Rs232ReceiveByte(unsigned char *data);


/************************************************************************************//**
** \brief     Initializes the UART communication interface.
** \return    none.
**
****************************************************************************************/
static void BootComRs232Init(void)
{
  /* enable the UART0 peripheral */
  SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
  /* enable the and configure UART0 related peripherals and pins */
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
  GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
  /* configure the UART0 baudrate and communication parameters */
  UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), BOOT_COM_RS232_BAUDRATE,
                      (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | 
                      UART_CONFIG_PAR_NONE));
} /*** end of BootComRs232Init ***/


/************************************************************************************//**
** \brief     Receives the CONNECT request from the host, which indicates that the
**            bootloader should be activated and, if so, activates it.
** \return    none.
**
****************************************************************************************/
static void BootComRs232CheckActivationRequest(void)
{
  static unsigned char xcpCtoReqPacket[BOOT_COM_RS232_RX_MAX_DATA+1];
  static unsigned char xcpCtoRxLength;
  static unsigned char xcpCtoRxInProgress = 0;
  static unsigned long xcpCtoRxStartTime = 0;

  /* start of cto packet received? */
  if (xcpCtoRxInProgress == 0)
  {
    /* store the message length when received */
    if (Rs232ReceiveByte(&xcpCtoReqPacket[0]) == 1)
    {
      /* check that the length has a valid value. it should not be 0 */
      if ( (xcpCtoReqPacket[0] > 0) &&
           (xcpCtoReqPacket[0] <= BOOT_COM_RS232_RX_MAX_DATA) )
      {
        /* store the start time */
        xcpCtoRxStartTime = TimerGet();
        /* indicate that a cto packet is being received */
        xcpCtoRxInProgress = 1;
        /* reset packet data count */
        xcpCtoRxLength = 0;
      }
    }
  }
  else
  {
    /* store the next packet byte */
    if (Rs232ReceiveByte(&xcpCtoReqPacket[xcpCtoRxLength+1]) == 1)
    {
      /* increment the packet data count */
      xcpCtoRxLength++;

      /* check to see if the entire packet was received */
      if (xcpCtoRxLength == xcpCtoReqPacket[0])
      {
        /* done with cto packet reception */
        xcpCtoRxInProgress = 0;

        /* check if this was an XCP CONNECT command */
        if ((xcpCtoReqPacket[1] == 0xff) && (xcpCtoRxLength == 2))
        {
          /* connection request received so start the bootloader */
          BootActivate();
        }
      }
    }
    else
    {
      /* check packet reception timeout */
      if (TimerGet() > (xcpCtoRxStartTime + RS232_CTO_RX_PACKET_TIMEOUT_MS))
      {
        /* cancel cto packet reception due to timeout. note that this automatically
         * discards the already received packet bytes, allowing the host to retry.
         */
        xcpCtoRxInProgress = 0;
      }
    }
  }
} /*** end of BootComRs232CheckActivationRequest ***/


/************************************************************************************//**
** \brief     Receives a communication interface byte if one is present.
** \param     data Pointer to byte where the data is to be stored.
** \return    1 if a byte was received, 0 otherwise.
**
****************************************************************************************/
static unsigned char Rs232ReceiveByte(unsigned char *data)
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
} /*** end of Rs232ReceiveByte ***/
#endif /* BOOT_COM_RS232_ENABLE > 0 */


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
static void BootComCanInit(void)
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
  /* is it a 29-bit extended CAN identifier? */
  if ((BOOT_COM_CAN_RX_MSG_ID & 0x80000000) != 0)
  {
    /* configure reception acceptance filter for 29-bit CAN identifier. */
    rxMsgObject.ulMsgID &= ~0x80000000;
    rxMsgObject.ulMsgIDMask = 0x1fffffff;
    rxMsgObject.ulFlags |= MSG_OBJ_EXTENDED_ID;
  }
  CANMessageSet(CAN0_BASE, CAN_RX_MSGOBJECT_IDX+1, &rxMsgObject, MSG_OBJ_TYPE_RX);
} /*** end of BootCanComInit ***/


/************************************************************************************//**
** \brief     Receives the CONNECT request from the host, which indicates that the
**            bootloader should be activated and, if so, activates it.
** \return    none.
**
****************************************************************************************/
static void BootComCanCheckActivationRequest(void)
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
    if ((msgData[0] == 0xff) && (msgObject.ulMsgLen == 2))
    {
      /* connection request received so start the bootloader */
      BootActivate();
    }
  }
} /*** end of BootComCanCheckActivationRequest ***/
#endif /* BOOT_COM_CAN_ENABLE > 0 */


/*********************************** end of boot.c *************************************/
