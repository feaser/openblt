/****************************************************************************************
|  Description: demo program bootloader interface source file
|    File Name: boot.c
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
#include "header.h"                                    /* generic header               */


/****************************************************************************************
** NAME:           BootActivate
** PARAMETER:      none
** RETURN VALUE:   none
** DESCRIPTION:    Bootloader activation function.
**
****************************************************************************************/
static void BootActivate(void)
{
  void (*pEntryFromProgFnc)(void);

  /* set pointer to the address of function EntryFromProg in the bootloader. note that
   * 1 is added to this address to enable a switch from Thumb2 to Thumb mode
   */
  pEntryFromProgFnc = (void*)0x000000B8 + 1;
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


/****************************************************************************************
** NAME:           BootComInit
** PARAMETER:      none
** RETURN VALUE:   none
** DESCRIPTION:    Initializes the UART communication interface
**
****************************************************************************************/
void BootComInit(void)
{
  LEUART_Init_TypeDef init = LEUART_INIT_DEFAULT;

  /* configure GPIO pins */
  CMU_ClockEnable(cmuClock_GPIO, true);
  /* to avoid false start, configure output as high */
  GPIO_PinModeSet(gpioPortC, 6, gpioModePushPull, 1);
  GPIO_PinModeSet(gpioPortC, 7, gpioModeInput, 0);
  /* enable CORE LE clock in order to access LE modules */
  CMU_ClockEnable(cmuClock_CORELE, true);
  /* select LFXO for LEUARTs (and wait for it to stabilize) */
  CMU_ClockSelectSet(cmuClock_LFB, cmuSelect_LFXO);
  /* do not prescale clock */
  CMU_ClockDivSet(cmuClock_LEUART1, cmuClkDiv_1);
  /* enable LEUART1 clock */
  CMU_ClockEnable(cmuClock_LEUART1, true);
  /* configure LEUART */
  init.enable = leuartDisable;
  LEUART_Init(LEUART1, &init);
  LEUART_BaudrateSet(LEUART1, 0, BOOT_COM_UART_BAUDRATE);
  /* enable pins at default location */
  LEUART1->ROUTE = LEUART_ROUTE_RXPEN | LEUART_ROUTE_TXPEN;
  /* clear previous RX interrupts */
  LEUART_IntClear(LEUART1, LEUART_IF_RXDATAV);
  /* finally enable it */
  LEUART_Enable(LEUART1, leuartEnable);
} /*** end of BootComInit ***/


/****************************************************************************************
** NAME:           BootComCheckActivationRequest
** PARAMETER:      none
** RETURN VALUE:   none
** DESCRIPTION:    Receives the CONNECT request from the host, which indicates that the
**                 bootloader should be activated and, if so, activates it.
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


/****************************************************************************************
** NAME:           UartReceiveByte
** PARAMETER:      data pointer to byte where the data is to be stored.
** RETURN VALUE:   1 if a byte was received, 0 otherwise.
** DESCRIPTION:    Receives a communication interface byte if one is present.
**
****************************************************************************************/
static unsigned char UartReceiveByte(unsigned char *data)
{
  /* check to see if a new bytes was received */
  if ((LEUART1->IF & LEUART_IF_RXDATAV) != 0)
  {
    /* store the received data byte and set return value to positive */
    *data = LEUART_Rx(LEUART1);
    return 1;
  }
  /* still here to no new byte received */
  return 0;
} /*** end of UartReceiveByte ***/
#endif /* BOOT_COM_UART_ENABLE > 0 */


/*********************************** end of boot.c *************************************/
