/****************************************************************************************
|  Description: demo program bootloader interface source file
|    File Name: boot.c
|
|----------------------------------------------------------------------------------------
|                          C O P Y R I G H T
|----------------------------------------------------------------------------------------
|   Copyright (c) 2013  by Feaser    http://www.feaser.com    All rights reserved
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

  /* stop the timer interrupt */
  TimerDeinit();
  /* set pointer to the address of function EntryFromProg in the bootloader. note that
   * 1 is added to this address to enable a switch from Thumb2 to Thumb mode
   */
  pEntryFromProgFnc = (void(*)(void))(0x08000188 + 1);
  /* call EntryFromProg to activate the bootloader. */
  pEntryFromProgFnc();
} /*** end of BootActivate ***/


#if (BOOT_COM_UART_ENABLE > 0)
/****************************************************************************************
*     U N I V E R S A L   A S Y N C H R O N O U S   R X   T X   I N T E R F A C E
****************************************************************************************/

/****************************************************************************************
* Macro definitions
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
  GPIO_InitTypeDef  GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure;

  /* enable UART peripheral clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6, ENABLE);
  /* enable GPIO peripheral clock for transmitter and receiver pins */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
  /* connect the pin to the peripherals alternate function */
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource6, GPIO_AF_USART6);
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource7, GPIO_AF_USART6);
  /* configure USART Tx as alternate function  */
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  /* configure USART Rx as alternate function */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  /* initialize the uart for the specified communication speed */
  USART_InitStructure.USART_BaudRate = BOOT_COM_UART_BAUDRATE;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_Init(USART6, &USART_InitStructure);
  /* enable UART */
  USART_Cmd(USART6, ENABLE);
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
  /* check flag to see if a byte was received */
  if (USART_GetFlagStatus(USART6, USART_FLAG_RXNE) == SET)
  {
    /* retrieve and store the newly received byte */
    *data = (unsigned char)USART_ReceiveData(USART6);
    /* all done */
    return 1;
  }
  /* still here to no new byte received */
  return 0;
} /*** end of UartReceiveByte ***/
#endif /* BOOT_COM_UART_ENABLE > 0 */


/*********************************** end of boot.c *************************************/
