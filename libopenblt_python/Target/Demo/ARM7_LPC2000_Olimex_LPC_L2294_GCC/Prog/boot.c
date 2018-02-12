/************************************************************************************//**
* \file         Demo\ARM7_LPC2000_Olimex_LPC_L2294_GCC\Prog\boot.c
* \brief        Demo program bootloader interface source file.
* \ingroup      Prog_ARM7_LPC2000_Olimex_LPC_L2294_GCC
* \internal
*----------------------------------------------------------------------------------------
*                          C O P Y R I G H T
*----------------------------------------------------------------------------------------
*   Copyright (c) 2011  by Feaser    http://www.feaser.com    All rights reserved
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
#if (BOOT_COM_UART_ENABLE > 0)
static void BootComUartInit(void);
static void BootComUartCheckActivationRequest(void);
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
#if (BOOT_COM_UART_ENABLE > 0)
  BootComUartInit();
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
#if (BOOT_COM_UART_ENABLE > 0)
  BootComUartCheckActivationRequest();
#endif
#if (BOOT_COM_CAN_ENABLE > 0)
  BootComCanCheckActivationRequest();
#endif
} /*** end of BootComCheckActivationRequest ***/


/************************************************************************************//**
** \brief     Bootloader activation function. Performs a software reset by configuring
**            and triggering the watchdog.
** \return    none.
**
****************************************************************************************/
void BootActivate(void)
{
  #define WDEN_BIT     (0x01) /* watchdog enable bit (set only) */
  #define WDRESET_BIT  (0x02) /* watchdog reset enable bit      */

  /* configure a short timeout. not really interesting as we won't be using it */  
  WDTC = 1024;  
  /* enable the watchdog and configure it such that a watchdog timeout causes a reset */
  WDMOD = WDEN_BIT | WDRESET_BIT; 
  /* start the watchdog */
  WDFEED = 0xAA;
  WDFEED = 0x55;
  /* write invalid feed sequence to cause an instant reset */
  WDFEED = 0xAA;
  WDFEED = 0x00;
} /*** end of BootActivate ***/


#if (BOOT_COM_UART_ENABLE > 0)
/****************************************************************************************
*     U N I V E R S A L   A S Y N C H R O N O U S   R X   T X   I N T E R F A C E
****************************************************************************************/

/****************************************************************************************
* Macro definitions
****************************************************************************************/
/** \brief Divisor latch access bit. */
#define UART_DLAB      (0x80)
/** \brief 8 data and 1 stop bit, no parity. */
#define UART_MODE_8N1  (0x03)
/** \brief FIFO reset and RX FIFO 1 deep. */
#define UART_FIFO_RX1  (0x07)
/** \brief Receiver data ready. */
#define UART_RDR       (0x01)
/** \brief Timeout time for the reception of a CTO packet. The timer is started upon
 *         reception of the first packet byte.
 */
#define UART_CTO_RX_PACKET_TIMEOUT_MS (100u)


/****************************************************************************************
* Function prototypes
****************************************************************************************/
static unsigned char UartReceiveByte(unsigned char *data);


/************************************************************************************//**
** \brief     Initializes the UART communication interface.
** \return    none.
**
****************************************************************************************/
static void BootComUartInit(void)
{
  unsigned long baud_reg_value;                           /* baudrate register value      */
  
  /* configure P0.0 for UART0 Tx and P0.1 for UART0 Rx functionality */  
  PINSEL0 |= 0x05;
  /* disable UART related interrupt generation. this driver works in polling mode */
  U0IER = 0;
  /* clear interrupt id register */
  U0IIR = 0;
  /* clear line status register */
  U0LSR = 0;
  /* set divisor latch DLAB = 1 so buadrate can be configured */
  U0LCR = UART_DLAB;
  /* Baudrate calculation: 
   *   y = BOOT_CPU_SYSTEM_SPEED_KHZ * 1000 / 16 / BOOT_COM_UART_BAUDRATE and add 
   *   smartness to automatically round the value up/down using the following trick:
   *     y = x/n can round with y = (x + (n + 1)/2 ) / n
   */
  baud_reg_value = (((BOOT_CPU_SYSTEM_SPEED_KHZ*1000/16)+ \
                    ((BOOT_COM_UART_BAUDRATE+1)/2))/BOOT_COM_UART_BAUDRATE);
  /* write the calculated baudrate selector value to the registers */
  U0DLL = (unsigned char)baud_reg_value;
  U0DLM = (unsigned char)(baud_reg_value >> 8);
  /* configure 8 data bits, no parity and 1 stop bit and set DLAB = 0 */
  U0LCR = UART_MODE_8N1;
  /* enable and reset transmit and receive FIFO. necessary for UART operation */
  U0FCR = UART_FIFO_RX1;
} /*** end of BootComUartInit ***/


/************************************************************************************//**
** \brief     Receives the CONNECT request from the host, which indicates that the
**            bootloader should be activated and, if so, activates it.
** \return    none.
**
****************************************************************************************/
static void BootComUartCheckActivationRequest(void)
{
  static unsigned char xcpCtoReqPacket[BOOT_COM_UART_RX_MAX_DATA+1];
  static unsigned char xcpCtoRxLength;
  static unsigned char xcpCtoRxInProgress = 0;
  static unsigned long xcpCtoRxStartTime = 0;

  /* start of cto packet received? */
  if (xcpCtoRxInProgress == 0)
  {
    /* store the message length when received */
    if (UartReceiveByte(&xcpCtoReqPacket[0]) == 1)
    {
      /* check that the length has a valid value. it should not be 0 */
      if ( (xcpCtoReqPacket[0] > 0) &&
           (xcpCtoReqPacket[0] <= BOOT_COM_UART_RX_MAX_DATA) )
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
    else
    {
      /* check packet reception timeout */
      if (TimerGet() > (xcpCtoRxStartTime + UART_CTO_RX_PACKET_TIMEOUT_MS))
      {
        /* cancel cto packet reception due to timeout. note that this automatically
         * discards the already received packet bytes, allowing the host to retry.
         */
        xcpCtoRxInProgress = 0;
      }
    }
  }
} /*** end of BootComUartCheckActivationRequest ***/


/************************************************************************************//**
** \brief     Receives a communication interface byte if one is present.
** \param     data Pointer to byte where the data is to be stored.
** \return    1 if a byte was received, 0 otherwise.
**
****************************************************************************************/
static unsigned char UartReceiveByte(unsigned char *data)
{
  /* check if a new byte was received by means of the RDR-bit */
  if((U0LSR & UART_RDR) != 0)
  {
    /* store the received byte */
    data[0] = U0RBR;
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
/** \brief Transmit buffer 1 idle. */
#define CAN_TBS1        (0x00000004)
/** \brief Transmit buffer 1 complete. */
#define CAN_TCS1        (0x00000008)
/** \brief Receive buffer release. */
#define CAN_RRB         (0x04)
/** \brief Receive buffer status. */
#define CAN_RBS         (0x01)
/** \brief Transmission request. */
#define CAN_TR          (0x01)
/** \brief Select tx buffer 1 for transmit. */
#define CAN_STB1        (0x20)
/** \brief Frame format bit. 0 for 11-bit and 1 for 29-bit CAN identifiers. */
#define CAN_FF          (0x80000000)


/****************************************************************************************
* Type definitions
****************************************************************************************/
/** \brief Structure type for grouping CAN bus timing related information. */
typedef struct t_can_bus_timing
{
  unsigned char tseg1;                                /**< CAN time segment 1          */
  unsigned char tseg2;                                /**< CAN time segment 2          */
} tCanBusTiming;


/****************************************************************************************
* Local constant declarations
****************************************************************************************/
/** \brief CAN bittiming table for dynamically calculating the bittiming settings.
 *  \details According to the CAN protocol 1 bit-time can be made up of between 8..25 
 *           time quanta (TQ). The total TQ in a bit is SYNC + TSEG1 + TSEG2 with SYNC 
 *           always being 1. The sample point is (SYNC + TSEG1) / (SYNC + TSEG1 + SEG2) * 
 *           100%. This array contains possible and valid time quanta configurations with
 *           a sample point between 68..78%.
 */
static const tCanBusTiming canTiming[] =
{                       /*  TQ | TSEG1 | TSEG2 | SP  */
                        /* ------------------------- */
    {  5, 2 },          /*   8 |   5   |   2   | 75% */
    {  6, 2 },          /*   9 |   6   |   2   | 78% */
    {  6, 3 },          /*  10 |   6   |   3   | 70% */
    {  7, 3 },          /*  11 |   7   |   3   | 73% */
    {  8, 3 },          /*  12 |   8   |   3   | 75% */
    {  9, 3 },          /*  13 |   9   |   3   | 77% */
    {  9, 4 },          /*  14 |   9   |   4   | 71% */
    { 10, 4 },          /*  15 |  10   |   4   | 73% */
    { 11, 4 },          /*  16 |  11   |   4   | 75% */
    { 12, 4 },          /*  17 |  12   |   4   | 76% */
    { 12, 5 },          /*  18 |  12   |   5   | 72% */
    { 13, 5 },          /*  19 |  13   |   5   | 74% */
    { 14, 5 },          /*  20 |  14   |   5   | 75% */
    { 15, 5 },          /*  21 |  15   |   5   | 76% */
    { 15, 6 },          /*  22 |  15   |   6   | 73% */
    { 16, 6 },          /*  23 |  16   |   6   | 74% */
    { 16, 7 },          /*  24 |  16   |   7   | 71% */
    { 16, 8 }           /*  25 |  16   |   8   | 68% */
};


/************************************************************************************//**
** \brief     Search algorithm to match the desired baudrate to a possible bus 
**            timing configuration.
** \param     baud The desired baudrate in kbps. Valid values are 10..1000.
** \param     btr  Pointer to where the value for register CANxBTR will be stored.
** \return    1 if the CAN bustiming register values were found, 0 otherwise.
**
****************************************************************************************/
static unsigned char CanGetSpeedConfig(unsigned short baud, unsigned long *btr)
{
  unsigned short prescaler;
  unsigned char  cnt;

  /* loop through all possible time quanta configurations to find a match */
  for (cnt=0; cnt < sizeof(canTiming)/sizeof(canTiming[0]); cnt++)
  {
    if ((BOOT_CPU_SYSTEM_SPEED_KHZ % (baud*(canTiming[cnt].tseg1+canTiming[cnt].tseg2+1))) == 0)
    {
      /* compute the prescaler that goes with this TQ configuration */
      prescaler = BOOT_CPU_SYSTEM_SPEED_KHZ/(baud*(canTiming[cnt].tseg1+canTiming[cnt].tseg2+1));

      /* make sure the prescaler is valid */
      if ( (prescaler > 0) && (prescaler <= 1024) )
      {
        /* store the prescaler and bustiming register value */
        *btr  = prescaler - 1;
        *btr |= ((canTiming[cnt].tseg2 - 1) << 20) | ((canTiming[cnt].tseg1 - 1) << 16);
        /* found a good bus timing configuration */
        return 1;
      }
    }
  }
  /* could not find a good bus timing configuration */
  return 0;
} /*** end of CanGetSpeedConfig ***/


/************************************************************************************//**
** \brief     Initializes the CAN communication interface.
** \return    none.
**
****************************************************************************************/
static void BootComCanInit(void)
{
  unsigned long btr_reg_value;
  
  /* configure acceptance filter for bypass mode so it receives all messages */
  CANAFMR = 0x00000002L;
  /* take CAN controller offline and go into reset mode */
  CAN1MOD = 1; 
  /* disable all interrupts. driver only needs to work in polling mode */
  CAN1IER = 0;
  /* reset CAN controller status */
  CAN1GSR = 0; 
  /* configure the bittiming */
  if (CanGetSpeedConfig(BOOT_COM_CAN_BAUDRATE/1000, &btr_reg_value) == 1)
  {
    /* write the bittiming configuration to the register */
    CAN1BTR = btr_reg_value;  
  }  
  /* enter normal operating mode and synchronize to the CAN bus */
  CAN1MOD = 0;
} /*** end of BootComCanInit ***/


/************************************************************************************//**
** \brief     Receives the CONNECT request from the host, which indicates that the
**            bootloader should be activated and, if so, activates it.
** \return    none.
**
****************************************************************************************/
static void BootComCanCheckActivationRequest(void)
{
  unsigned long rxMsgId;
  unsigned char data[2];
  unsigned char idMatchFound = 0;
  
  /* check if a new message was received */
  if ((CAN1SR & CAN_RBS) == 0)
  {
    return;
  }
  /* read out the CAN message identifier */
  rxMsgId = CAN1RID;
  /* was is a 29-bit extended CAN identifier? */
  if ((CAN1RFS & CAN_FF) != 0)
  {
    /* set mask bit. */
    rxMsgId |= 0x80000000;
  }
  /* see if this is the message identifier that we are interested in */
  if (rxMsgId == BOOT_COM_CAN_RX_MSG_ID)
  {
    /* store the message data */
    data[0] = (unsigned char)CAN1RDA; 
    data[1] = (unsigned char)(CAN1RDA >> 8); 
    /* set matched flag. */
    idMatchFound = 1;
  }
  /* release the receive buffer */
  CAN1CMR = CAN_RRB;
  /* check if a match was found. */
  if (idMatchFound == 1)
  {
    /* check if this was an XCP CONNECT command */
    if ((data[0] == 0xff) && (data[1] == 0x00))
    {
      /* connection request received so start the bootloader */
      BootActivate();
    }
  }  
} /*** end of BootComCanCheckActivationRequest ***/
#endif /* BOOT_COM_CAN_ENABLE > 0 */


/*********************************** end of boot.c *************************************/
