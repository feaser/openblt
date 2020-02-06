/************************************************************************************//**
* \file         Demo/HCS12_Evbplus_Dragon12p_CodeWarrior/Prog/boot.c
* \brief        Demo program bootloader interface source file.
* \ingroup      Prog_HCS12_Evbplus_Dragon12p_CodeWarrior
* \internal
*----------------------------------------------------------------------------------------
*                          C O P Y R I G H T
*----------------------------------------------------------------------------------------
*   Copyright (c) 2013  by Feaser    http://www.feaser.com    All rights reserved
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
  /* perform a software reset by letting the watchdog time out. make sure it is enbled */
  if ( (COPCTL & 0x07) == 0)
  {
    /* enable the watchdog */
    COPCTL = 0x71; 
  }
  /* wait for the watchdog to time out which triggers a reset */
  while (1==1)
  {
    ; 
  }
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
  unsigned short baudrate_sbr0_12;

  /* reset the SCI subsystem's configuration, which automatically configures it for
   * 8,n,1 communication mode.
   */
  SCI0CR2 = 0;   
  SCI0CR1 = 0;   
  SCI0BDH = 0;  
  SCI0BDL = 0;
  /* configure the baudrate from BOOT_COM_RS232_BAUDRATE */
  baudrate_sbr0_12 = (BOOT_CPU_SYSTEM_SPEED_KHZ * 1000ul) / 16 / BOOT_COM_RS232_BAUDRATE; 
  baudrate_sbr0_12 &= SCI0BD_SBR_MASK;
  /* write first MSB then LSB for the baudrate to latch */
  SCI0BDH = (unsigned char)(baudrate_sbr0_12 >> 8);
  SCI0BDL = (unsigned char)baudrate_sbr0_12;
  /* enable the receiver */
  SCI0CR2 |= (SCI0CR2_RE_MASK);
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
  /* check if a new byte was received by means of the RDRF-bit */
  if((SCI0SR1 & SCI0SR1_RDRF_MASK) != 0)
  {
    /* store the received byte */
    data[0] = SCI0DRL;
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
* Type definitions
****************************************************************************************/
/** \brief Structure type with the layout of the CAN bus timing registers. */
typedef struct
{
  unsigned char tseg1;                     /**< CAN time segment 1                     */
  unsigned char tseg2;                     /**< CAN time segment 2                     */
} tCanBusTiming;


/****************************************************************************************
* Macro definitions
****************************************************************************************/
#define CONVERT_STD_ID_TO_REG0(id) ((unsigned char)(((unsigned short)id & 0x07f8) >> 3))
#define CONVERT_STD_ID_TO_REG1(id) ((unsigned char)(id & 0x07) << 5)
#define CONVERT_STD_ID_TO_REG2(id) (0)
#define CONVERT_STD_ID_TO_REG3(id) (0)
#define CONVERT_EXT_ID_TO_REG0(id) ((unsigned char)(id >> 21))
#define CONVERT_EXT_ID_TO_REG1(id) ((((unsigned char)(id >> 15)) & 0x07) |  \
                                   (((unsigned char)(id >> 13)) & 0xe0) | CAN0RXIDR1_IDE_MASK)
#define CONVERT_EXT_ID_TO_REG2(id) ((unsigned char)(((unsigned short)id & 0x7f80) >> 7))
#define CONVERT_EXT_ID_TO_REG3(id) ((unsigned char)(id & 0x7f) << 1)


/****************************************************************************************
* Local constant declarations
****************************************************************************************/
/**
* \brief     Array with possible time quanta configurations.
* \details   According to the CAN protocol 1 bit-time can be made up of between 8..25
*            time quanta (TQ). The total TQ in a bit is SYNC + TSEG1 + TSEG2 with SYNC
*            always being 1. The sample point is (SYNC + TSEG1) / (SYNC + TSEG1 + SEG2)
*            * 100%. This array contains possible and valid time quanta configurations
*            with a sample point between 68..78%.
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
** \brief     Search algorithm to match the desired baudrate to a possible bus timing
**            configuration.
** \param     baud The desired baudrate in kbps. Valid values are 10..1000.
** \param     btr0 Pointer to where the value for register CANxBTR0 will be stored.
** \param     btr1 Pointer to where the value for register CANxBTR1 will be stored.
** \return    1 if the CAN bustiming register values were found, 0 otherwise.
**
****************************************************************************************/
static unsigned char CanGetSpeedConfig(unsigned short baud, unsigned char *btr0, unsigned char *btr1)
{
  unsigned char prescaler;
  unsigned char cnt;

  /* loop through all possible time quanta configurations to find a match */
  for (cnt=0; cnt < sizeof(canTiming)/sizeof(canTiming[0]); cnt++)
  {
    if ((BOOT_CPU_XTAL_SPEED_KHZ % (baud*(canTiming[cnt].tseg1+canTiming[cnt].tseg2+1))) == 0)
    {
      /* compute the prescaler that goes with this TQ configuration */
      prescaler = (unsigned char)(BOOT_CPU_XTAL_SPEED_KHZ/(baud*(canTiming[cnt].tseg1+canTiming[cnt].tseg2+1)));

      /* make sure the prescaler is valid */
      if ( (prescaler > 0) && (prescaler <= 64) )
      {
        /* store the MSCAN bustiming register values */
        *btr0 = prescaler - 1;
        *btr1 = ((canTiming[cnt].tseg2 - 1) << 4) | (canTiming[cnt].tseg1 - 1);
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
  unsigned char btrRegValues[2];
  unsigned long accept_code;
  unsigned long accept_mask;

  /* enter initialization mode. note that this automatically disables CAN interrupts */
  CAN0CTL0 = CAN0CTL0_INITRQ_MASK;
  /* wait for initialization mode entry handshake from the hardware */
  while ((CAN0CTL1 & CAN0CTL1_INITAK_MASK) == 0)
  {
    ;
  }

  /* enable the CAN controller, disable wake up and listen modes and set the
   * crystal oscillator as the clock source.
   */
  CAN0CTL1 = CAN0CTL1_CANE_MASK;

  /* configure baudrate */
  if (CanGetSpeedConfig(BOOT_COM_CAN_BAUDRATE/1000, &btrRegValues[0], &btrRegValues[1]) == 1)
  {
    /* configure the baudrate */
    CAN0BTR0 = btrRegValues[0];
    CAN0BTR1 = btrRegValues[1];
  }

  /* enable 2 32-bit acceptance filters. both will be configured for the same code and
   * mask. the only difference is that filter 0 will be setup to receive extended 29-bit
   * identifiers and filter 0 to receive standard 11-bit identifiers.
   */
  CAN0IDAC_IDAM0 = 0;
  CAN0IDAC_IDAM1 = 0;
  
  /* set the acceptance filter code and mask to receive all messages */
  accept_code = 0x00000000;
  accept_mask = 0x1fffffff;

  /* configure acceptance filter 0 for 29-bit extended identifiers */
  CAN0IDAR0 = CONVERT_EXT_ID_TO_REG0(accept_code);
  CAN0IDAR1 = CONVERT_EXT_ID_TO_REG1(accept_code);
  CAN0IDAR2 = CONVERT_EXT_ID_TO_REG2(accept_code);
  CAN0IDAR3 = CONVERT_EXT_ID_TO_REG3(accept_code);
  CAN0IDMR0 = CONVERT_EXT_ID_TO_REG0(accept_mask);
  CAN0IDMR1 = (CONVERT_EXT_ID_TO_REG1(accept_mask) | 0x10) & (unsigned char)(~0x08);
  CAN0IDMR2 = CONVERT_EXT_ID_TO_REG2(accept_mask);
  CAN0IDMR3 = CONVERT_EXT_ID_TO_REG3(accept_mask);

  /* configure acceptance filter 1 for 11-bit standard identifiers */
  CAN0IDAR4 = CONVERT_STD_ID_TO_REG0(accept_code);
  CAN0IDAR5 = CONVERT_STD_ID_TO_REG1(accept_code);
  CAN0IDAR6 = CONVERT_STD_ID_TO_REG2(accept_code);
  CAN0IDAR7 = CONVERT_STD_ID_TO_REG3(accept_code);
  CAN0IDMR4 = CONVERT_STD_ID_TO_REG0(accept_mask);
  CAN0IDMR5 = CONVERT_STD_ID_TO_REG1(accept_mask) | (0x04 | 0x02 | 0x01);
  CAN0IDMR6 = CONVERT_STD_ID_TO_REG2(accept_mask);
  CAN0IDMR7 = CONVERT_STD_ID_TO_REG3(accept_mask);

  /* leave initialization mode and synchronize to the CAN bus */
  CAN0CTL0_INITRQ = 0;
  /* wait for CAN bus synchronization handshake from the hardware */
  while ((CAN0CTL1 & CAN0CTL1_INITAK_MASK) != 0)
  {
    ;
  }
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

  /* check if a new message was received */
  if ((CAN0RFLG & CAN0RFLG_RXF_MASK) == CAN0RFLG_RXF_MASK)
  {
    /* check IDE-bit to determine if it is a 11-bit or 29-bit identifier */
    if ((CAN0RXIDR1 & CAN0RXIDR1_IDE_MASK) == 0)             
    {
      /* 11-bit id */
      rxMsgId = (*(unsigned short*)(&CAN0RXIDR0)) >> 5;
    }
    else
    {
      /* 29-bit id */
      rxMsgId = (unsigned long)(((*(unsigned long*)(&CAN0RXIDR0)) & 0x0007ffff) >> 1) |
                (unsigned long)(((*(unsigned long*)(&CAN0RXIDR0)) & 0xffe00000) >> 3);
      rxMsgId |= 0x80000000;
    }
    /* is this the packet identifier? */    
    if (rxMsgId == BOOT_COM_CAN_RX_MSG_ID)
    {
      /* check if this was an XCP CONNECT command */ 
      if ( (CAN0RXDSR0 == 0xff) && ((CAN0RXDLR & CAN0RXDLR_DLC_MASK) == 2) )
      {
        /* release the receive object by clearing the rx flag */
        CAN0RFLG &= CAN0RFLG_RXF_MASK;
        /* connection request received so start the bootloader */
        BootActivate();
      }
    }
    /* release the receive object by clearing the rx flag */
    CAN0RFLG &= CAN0RFLG_RXF_MASK;
  }
} /*** end of BootComCanCheckActivationRequest ***/
#endif /* BOOT_COM_CAN_ENABLE > 0 */


/*********************************** end of boot.c *************************************/
