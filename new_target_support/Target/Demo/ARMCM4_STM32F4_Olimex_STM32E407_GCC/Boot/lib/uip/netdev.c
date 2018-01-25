/*
 * Copyright (c) 2001, Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * Author: Adam Dunkels <adam@sics.se>
 *
 * $Id: netdev.c,v 1.8 2006/06/07 08:39:58 adam Exp $
 */


/*---------------------------------------------------------------------------*/
#include "uip.h"
#include "uip_arp.h"
#include "boot.h"
#include "stm32f4xx.h"                               /* STM32 registers                */
#include "stm32f4xx_conf.h"                          /* STM32 peripheral drivers       */
#include "stm32_eth.h"                               /* STM32 ethernet library         */
#include <string.h>                                  /* for memcpy                     */


/*---------------------------------------------------------------------------*/
#define NETDEV_DEFAULT_MACADDR0           (0x08)
#define NETDEV_DEFAULT_MACADDR1           (0x00)
#define NETDEV_DEFAULT_MACADDR2           (0x27)
#define NETDEV_DEFAULT_MACADDR3           (0x69)
#define NETDEV_DEFAULT_MACADDR4           (0x5B)
#define NETDEV_DEFAULT_MACADDR5           (0x45)


/*---------------------------------------------------------------------------*/
static void netdev_TxDscrInit(void);
static void netdev_RxDscrInit(void);

/*---------------------------------------------------------------------------*/
typedef union _TranDesc0_t
{
  uint32_t Data;
  struct {
    uint32_t  DB            : 1;
    uint32_t  UF            : 1;
    uint32_t  ED            : 1;
    uint32_t  CC            : 4;
    uint32_t  VF            : 1;
    uint32_t  EC            : 1;
    uint32_t  LC            : 1;
    uint32_t  NC            : 1;
    uint32_t  LSC           : 1;
    uint32_t  IPE           : 1;
    uint32_t  FF            : 1;
    uint32_t  JT            : 1;
    uint32_t  ES            : 1;
    uint32_t  IHE           : 1;
    uint32_t                : 3;
    uint32_t  TCH           : 1;
    uint32_t  TER           : 1;
    uint32_t  CIC           : 2;
    uint32_t                : 2;
    uint32_t  DP            : 1;
    uint32_t  DC            : 1;
    uint32_t  FS            : 1;
    uint32_t  LSEG          : 1;
    uint32_t  IC            : 1;
    uint32_t  OWN           : 1;
  };
} TranDesc0_t, * pTranDesc0_t;

typedef union _TranDesc1_t
{
  uint32_t Data;
  struct {
    uint32_t  TBS1          :13;
    uint32_t                : 3;
    uint32_t  TBS2          :12;
    uint32_t                : 3;
  };
} TranDesc1_t, * pTranDesc1_t;

typedef union _RecDesc0_t
{
  uint32_t Data;
  struct {
    uint32_t  RMAM_PCE      : 1;
    uint32_t  CE            : 1;
    uint32_t  DE            : 1;
    uint32_t  RE            : 1;
    uint32_t  RWT           : 1;
    uint32_t  FT            : 1;
    uint32_t  LC            : 1;
    uint32_t  IPHCE         : 1;
    uint32_t  LS            : 1;
    uint32_t  FS            : 1;
    uint32_t  VLAN          : 1;
    uint32_t  OE            : 1;
    uint32_t  LE            : 1;
    uint32_t  SAF           : 1;
    uint32_t  DERR          : 1;
    uint32_t  ES            : 1;
    uint32_t  FL            :14;
    uint32_t  AFM           : 1;
    uint32_t  OWN           : 1;
  };
} RecDesc0_t, * pRecDesc0_t;

typedef union _recDesc1_t
{
  uint32_t Data;
  struct {
    uint32_t  RBS1          :13;
    uint32_t                : 1;
    uint32_t  RCH           : 1;
    uint32_t  RER           : 1;
    uint32_t  RBS2          :14;
    uint32_t  DIC           : 1;
  };
} RecDesc1_t, * pRecDesc1_t;

typedef union _EnetDmaDesc_t
{
  uint32_t Data[4];
  // Rx DMA descriptor
  struct
  {
    RecDesc0_t                RxDesc0;
    RecDesc1_t                RxDesc1;
    uint32_t *                   pBuffer;
    union
    {
      uint32_t *                 pBuffer2;
      union _EnetDmaDesc_t *  pEnetDmaNextDesc;
    };
  } Rx;
  // Tx DMA descriptor
  struct
  {
    TranDesc0_t               TxDesc0;
    TranDesc1_t               TxDesc1;
    uint32_t *                   pBuffer1;
    union
    {
      uint32_t *                 pBuffer2;
      union _EnetDmaDesc_t *  pEnetDmaNextDesc;
    };
  } Tx;
} EnetDmaDesc_t, * pEnetDmaDesc_t;


/*---------------------------------------------------------------------------*/
uint8_t RxBuff[UIP_CONF_BUFFER_SIZE] __attribute__ ((aligned (4)));
uint8_t TxBuff[UIP_CONF_BUFFER_SIZE] __attribute__ ((aligned (4)));

EnetDmaDesc_t EnetDmaRx __attribute__((aligned (128)));
EnetDmaDesc_t EnetDmaTx __attribute__ ((aligned (128)));


/*---------------------------------------------------------------------------*/
void netdev_init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  ETH_InitTypeDef ETH_InitStructure;

  /* Enable ETHERNET clocks  */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_ETH_MAC | RCC_AHB1Periph_ETH_MAC_Tx |
                         RCC_AHB1Periph_ETH_MAC_Rx | RCC_AHB1Periph_ETH_MAC_PTP, ENABLE);

  
  /* Enable GPIOs clocks */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA |	RCC_AHB1Periph_GPIOB | 
                         RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOG, ENABLE);

  /* Enable SYSCFG clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
  /*Select RMII Interface*/
  SYSCFG_ETH_MediaInterfaceConfig(SYSCFG_ETH_MediaInterface_RMII);
  
  /* ETHERNET pins configuration */
  /* PA
    ETH_RMII_REF_CLK: PA1
    ETH_RMII_MDIO: PA2
    ETH_RMII_MDINT: PA3
    ETH_RMII_CRS_DV: PA7
   */

  /* Configure PA1, PA2, PA3 and PA7*/
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_7;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  /* Connect PA1, PA2, PA3 and PA7 to ethernet module*/
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource1, GPIO_AF_ETH);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_ETH);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_ETH);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_ETH);

  /* PB
    ETH_RMII_TX_EN: PG11
  */

  /* Configure PG11*/
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOG, &GPIO_InitStructure);

  /* Connect PG11 to ethernet module*/
  GPIO_PinAFConfig(GPIOG, GPIO_PinSource11, GPIO_AF_ETH);

  /* PC
    ETH_RMII_MDC: PC1
    ETH_RMII_RXD0: PC4
    ETH_RMII_RXD1: PC5
  */

  /* Configure PC1, PC4 and PC5*/
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_4 | GPIO_Pin_5;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOC, &GPIO_InitStructure);

  /* Connect PC1, PC4 and PC5 to ethernet module*/
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource1, GPIO_AF_ETH);
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource4, GPIO_AF_ETH);
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource5, GPIO_AF_ETH);

  /* PG
    ETH_RMII_TXD0: PG13
    ETH_RMII_TXD1: PG14
  */

  /* Configure PG13 and PG14*/
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOG, &GPIO_InitStructure);

  /* Connect PG13 and PG14 to ethernet module*/
  GPIO_PinAFConfig(GPIOG, GPIO_PinSource13, GPIO_AF_ETH);
  GPIO_PinAFConfig(GPIOG, GPIO_PinSource14, GPIO_AF_ETH);

  /* Reset ETHERNET on AHB Bus */
  ETH_DeInit();
  
  /* Software reset */ 
  ETH_SoftwareReset();
  
  /* Wait for software reset */
  while(ETH_GetSoftwareResetStatus()==SET);

  /* ETHERNET Configuration ------------------------------------------------------*/
  /* Call ETH_StructInit if you don't like to configure all ETH_InitStructure parameter */
  ETH_StructInit(&ETH_InitStructure);
  
  /* Fill ETH_InitStructure parametrs */
  /*------------------------   MAC   -----------------------------------*/
  ETH_InitStructure.ETH_AutoNegotiation = ETH_AutoNegotiation_Disable  ;  
  ETH_InitStructure.ETH_LoopbackMode = ETH_LoopbackMode_Disable;              
  ETH_InitStructure.ETH_RetryTransmission = ETH_RetryTransmission_Disable;                                                                                  
  ETH_InitStructure.ETH_AutomaticPadCRCStrip = ETH_AutomaticPadCRCStrip_Disable;                                                                                                                                                                        
  ETH_InitStructure.ETH_ReceiveAll = ETH_ReceiveAll_Enable;                                                                                                       
  ETH_InitStructure.ETH_BroadcastFramesReception = ETH_BroadcastFramesReception_Disable;      
  ETH_InitStructure.ETH_PromiscuousMode = ETH_PromiscuousMode_Disable;                                                             
  ETH_InitStructure.ETH_MulticastFramesFilter = ETH_MulticastFramesFilter_Perfect;      
  ETH_InitStructure.ETH_UnicastFramesFilter = ETH_UnicastFramesFilter_Perfect;                        
  ETH_InitStructure.ETH_Mode = ETH_Mode_FullDuplex;                        
  ETH_InitStructure.ETH_Speed = ETH_Speed_100M;                        

  unsigned int PhyAddr;
    union {
      uint32_t    HI_LO;
      struct
      {
        uint16_t  LO;
        uint16_t  HI;
      };
    } PHYID;
  for(PhyAddr = 0; 32 > PhyAddr; PhyAddr++)
  { 
    // datasheet for the ks8721bl ethernet controller (http://www.micrel.com/_PDF/Ethernet/datasheets/ks8721bl-sl.pdf)
    // page 20 --> PHY Identifier 1 and 2
    PHYID.HI = ETH_ReadPHYRegister(PhyAddr,2);  // 0x0022
    PHYID.LO = ETH_ReadPHYRegister(PhyAddr,3);  // 0x1619
    if ((0x00221619 == PHYID.HI_LO) || (0x0007C0F1 == PHYID.HI_LO))
      break;
  }
  if (32 < PhyAddr)
  {
    ASSERT_RT(BLT_FALSE);
  }
  /* Configure Ethernet */
  if(0 == ETH_Init(&ETH_InitStructure, PhyAddr))
  {
    ASSERT_RT(BLT_FALSE);
  }

  netdev_TxDscrInit();
  netdev_RxDscrInit();
  ETH_Start();
}


/*---------------------------------------------------------------------------*/
void netdev_init_mac(void)
{
  struct uip_eth_addr macAddress;

  /* set the default MAC address */
  macAddress.addr[0] = NETDEV_DEFAULT_MACADDR0;
  macAddress.addr[1] = NETDEV_DEFAULT_MACADDR1;
  macAddress.addr[2] = NETDEV_DEFAULT_MACADDR2;
  macAddress.addr[3] = NETDEV_DEFAULT_MACADDR3;
  macAddress.addr[4] = NETDEV_DEFAULT_MACADDR4;
  macAddress.addr[5] = NETDEV_DEFAULT_MACADDR5;
  uip_setethaddr(macAddress);
}


/*---------------------------------------------------------------------------*/
unsigned int netdev_read(void)
{
  uint32_t size;
  /*check for validity*/
  if(0 == EnetDmaRx.Rx.RxDesc0.OWN)
  {
    /*Get the size of the packet*/
    size = EnetDmaRx.Rx.RxDesc0.FL; // CRC
    memcpy(uip_buf, RxBuff, size);   //string.h library*/
  }
  else
  {
    return 0;
  }
  /* Give the buffer back to ENET */
  EnetDmaRx.Rx.RxDesc0.OWN = 1;
  /* Start the receive operation */
  ETH->DMARPDR = 1;
  /* Return no error */
  return size;
}


/*---------------------------------------------------------------------------*/
void netdev_send(void)
{
  while(EnetDmaTx.Tx.TxDesc0.OWN);

  /* Copy the  application buffer to the driver buffer
     Using this MEMCOPY_L2L_BY4 makes the copy routine faster
     than memcpy */
  memcpy(TxBuff, uip_buf, uip_len);

  /* Assign ENET address to Temp Tx Array */
  EnetDmaTx.Tx.pBuffer1 = (uint32_t *)TxBuff;

  /* Setting the Frame Length*/
  EnetDmaTx.Tx.TxDesc0.Data = 0;
  EnetDmaTx.Tx.TxDesc0.TCH  = 1;
  EnetDmaTx.Tx.TxDesc0.LSEG = 1;
  EnetDmaTx.Tx.TxDesc0.FS   = 1;
  EnetDmaTx.Tx.TxDesc0.DC   = 0;
  EnetDmaTx.Tx.TxDesc0.DP   = 0;

  EnetDmaTx.Tx.TxDesc1.Data = 0;
  EnetDmaTx.Tx.TxDesc1.TBS1 = (uip_len&0xFFF);

  /* Start the ENET by setting the VALID bit in dmaPackStatus of current descr*/
  EnetDmaTx.Tx.TxDesc0.OWN = 1;

  /* Start the transmit operation */
  ETH->DMATPDR = 1;
}


/*---------------------------------------------------------------------------*/
static void netdev_RxDscrInit(void)
{
  /* Initialization */
  /* Assign temp Rx array to the ENET buffer */
  EnetDmaRx.Rx.pBuffer = (uint32_t *)RxBuff;

  /* Initialize RX ENET Status and control */
  EnetDmaRx.Rx.RxDesc0.Data = 0;

  /* Initialize the next descriptor- In our case its single descriptor */
  EnetDmaRx.Rx.pEnetDmaNextDesc = &EnetDmaRx;

  EnetDmaRx.Rx.RxDesc1.Data = 0;
  EnetDmaRx.Rx.RxDesc1.RER  = 0; // end of ring
  EnetDmaRx.Rx.RxDesc1.RCH  = 1; // end of ring

  /* Set the max packet size  */
  EnetDmaRx.Rx.RxDesc1.RBS1 = UIP_CONF_BUFFER_SIZE;

  /* Setting the VALID bit */
  EnetDmaRx.Rx.RxDesc0.OWN = 1;
  /* Setting the RX NEXT Descriptor Register inside the ENET */
  ETH->DMARDLAR = (uint32_t)&EnetDmaRx;
}


/*---------------------------------------------------------------------------*/
static void netdev_TxDscrInit(void)
{
  /* ENET Start Address */
  EnetDmaTx.Tx.pBuffer1 = (uint32_t *)TxBuff;

  /* Next Descriptor Address */
  EnetDmaTx.Tx.pEnetDmaNextDesc = &EnetDmaTx;

  /* Initialize ENET status and control */
  EnetDmaTx.Tx.TxDesc0.TCH  = 1;
  EnetDmaTx.Tx.TxDesc0.Data = 0;
  EnetDmaTx.Tx.TxDesc1.Data = 0;
  /* Tx next set to Tx descriptor base */
  ETH->DMATDLAR = (uint32_t)&EnetDmaTx;

}
