/* Based on CPU DB MC9S12DG256_112, version 2.87.346 (RegistersPrg V2.28) */
/* DataSheet : 9S12DT256DGV3/D V03.04 */

#include <mc9s12dg256.h>

/*lint -save -esym(765, *) */


/* * * * *  8-BIT REGISTERS  * * * * * * * * * * * * * * * */
volatile PORTESTR _PORTE;                                  /* Port E Register; 0x00000008 */
volatile DDRESTR _DDRE;                                    /* Port E Data Direction Register; 0x00000009 */
volatile PEARSTR _PEAR;                                    /* Port E Assignment Register; 0x0000000A */
volatile MODESTR _MODE;                                    /* Mode Register; 0x0000000B */
volatile PUCRSTR _PUCR;                                    /* Pull-Up Control Register; 0x0000000C */
volatile RDRIVSTR _RDRIV;                                  /* Reduced Drive of I/O Lines; 0x0000000D */
volatile EBICTLSTR _EBICTL;                                /* External Bus Interface Control; 0x0000000E */
volatile INITRMSTR _INITRM;                                /* Initialization of Internal RAM Position Register; 0x00000010 */
volatile INITRGSTR _INITRG;                                /* Initialization of Internal Registers Position Register; 0x00000011 */
volatile INITEESTR _INITEE;                                /* Initialization of Internal EEPROM Position Register; 0x00000012 */
volatile MISCSTR _MISC;                                    /* Miscellaneous System Control Register; 0x00000013 */
volatile ITCRSTR _ITCR;                                    /* Interrupt Test Control Register; 0x00000015 */
volatile ITESTSTR _ITEST;                                  /* Interrupt Test Register; 0x00000016 */
volatile MEMSIZ0STR _MEMSIZ0;                              /* Memory Size Register Zero; 0x0000001C */
volatile MEMSIZ1STR _MEMSIZ1;                              /* Memory Size Register One; 0x0000001D */
volatile INTCRSTR _INTCR;                                  /* Interrupt Control Register; 0x0000001E */
volatile HPRIOSTR _HPRIO;                                  /* Highest Priority I Interrupt; 0x0000001F */
volatile BKPCT0STR _BKPCT0;                                /* Breakpoint Control Register 0; 0x00000028 */
volatile BKPCT1STR _BKPCT1;                                /* Breakpoint Control Register 1; 0x00000029 */
volatile BKP0XSTR _BKP0X;                                  /* First Address Memory Expansion Breakpoint Register; 0x0000002A */
volatile BKP0HSTR _BKP0H;                                  /* First Address High Byte Breakpoint Register; 0x0000002B */
volatile BKP0LSTR _BKP0L;                                  /* First Address Low Byte Breakpoint Register; 0x0000002C */
volatile BKP1XSTR _BKP1X;                                  /* Second Address Memory Expansion Breakpoint Register; 0x0000002D */
volatile BKP1HSTR _BKP1H;                                  /* Data (Second Address) High Byte Breakpoint Register; 0x0000002E */
volatile BKP1LSTR _BKP1L;                                  /* Data (Second Address) Low Byte Breakpoint Register; 0x0000002F */
volatile PPAGESTR _PPAGE;                                  /* Page Index Register; 0x00000030 */
volatile PORTKSTR _PORTK;                                  /* Port K Data Register; 0x00000032 */
volatile DDRKSTR _DDRK;                                    /* Port K Data Direction Register; 0x00000033 */
volatile SYNRSTR _SYNR;                                    /* CRG Synthesizer Register; 0x00000034 */
volatile REFDVSTR _REFDV;                                  /* CRG Reference Divider Register; 0x00000035 */
volatile CRGFLGSTR _CRGFLG;                                /* CRG Flags Register; 0x00000037 */
volatile CRGINTSTR _CRGINT;                                /* CRG Interrupt Enable Register; 0x00000038 */
volatile CLKSELSTR _CLKSEL;                                /* CRG Clock Select Register; 0x00000039 */
volatile PLLCTLSTR _PLLCTL;                                /* CRG PLL Control Register; 0x0000003A */
volatile RTICTLSTR _RTICTL;                                /* CRG RTI Control Register; 0x0000003B */
volatile COPCTLSTR _COPCTL;                                /* CRG COP Control Register; 0x0000003C */
volatile ARMCOPSTR _ARMCOP;                                /* CRG COP Timer Arm/Reset Register; 0x0000003F */
volatile TIOSSTR _TIOS;                                    /* Timer Input Capture/Output Compare Select; 0x00000040 */
volatile CFORCSTR _CFORC;                                  /* Timer Compare Force Register; 0x00000041 */
volatile OC7MSTR _OC7M;                                    /* Output Compare 7 Mask Register; 0x00000042 */
volatile OC7DSTR _OC7D;                                    /* Output Compare 7 Data Register; 0x00000043 */
volatile TSCR1STR _TSCR1;                                  /* Timer System Control Register1; 0x00000046 */
volatile TTOVSTR _TTOV;                                    /* Timer Toggle On Overflow Register; 0x00000047 */
volatile TCTL1STR _TCTL1;                                  /* Timer Control Register 1; 0x00000048 */
volatile TCTL2STR _TCTL2;                                  /* Timer Control Register 2; 0x00000049 */
volatile TCTL3STR _TCTL3;                                  /* Timer Control Register 3; 0x0000004A */
volatile TCTL4STR _TCTL4;                                  /* Timer Control Register 4; 0x0000004B */
volatile TIESTR _TIE;                                      /* Timer Interrupt Enable Register; 0x0000004C */
volatile TSCR2STR _TSCR2;                                  /* Timer System Control Register 2; 0x0000004D */
volatile TFLG1STR _TFLG1;                                  /* Main Timer Interrupt Flag 1; 0x0000004E */
volatile TFLG2STR _TFLG2;                                  /* Main Timer Interrupt Flag 2; 0x0000004F */
volatile PACTLSTR _PACTL;                                  /* 16-Bit Pulse Accumulator A Control Register; 0x00000060 */
volatile PAFLGSTR _PAFLG;                                  /* Pulse Accumulator A Flag Register; 0x00000061 */
volatile MCCTLSTR _MCCTL;                                  /* Modulus Down Counter underflow; 0x00000066 */
volatile MCFLGSTR _MCFLG;                                  /* 16-Bit Modulus Down Counter Flag Register; 0x00000067 */
volatile ICPARSTR _ICPAR;                                  /* Input Control Pulse Accumulator Register; 0x00000068 */
volatile DLYCTSTR _DLYCT;                                  /* Delay Counter Control Register; 0x00000069 */
volatile ICOVWSTR _ICOVW;                                  /* Input Control Overwrite Register; 0x0000006A */
volatile ICSYSSTR _ICSYS;                                  /* Input Control System Control Register; 0x0000006B */
volatile PBCTLSTR _PBCTL;                                  /* 16-Bit Pulse Accumulator B Control Register; 0x00000070 */
volatile PBFLGSTR _PBFLG;                                  /* Pulse Accumulator B Flag Register; 0x00000071 */
volatile ATD0STAT0STR _ATD0STAT0;                          /* ATD 0 Status Register 0; 0x00000086 */
volatile ATD0TEST1STR _ATD0TEST1;                          /* ATD0 Test Register; 0x00000089 */
volatile ATD0STAT1STR _ATD0STAT1;                          /* ATD 0 Status Register 1; 0x0000008B */
volatile ATD0DIENSTR _ATD0DIEN;                            /* ATD 0 Input Enable Register; 0x0000008D */
volatile PORTAD0STR _PORTAD0;                              /* Port AD0 Register; 0x0000008F */
volatile PWMESTR _PWME;                                    /* PWM Enable Register; 0x000000A0 */
volatile PWMPOLSTR _PWMPOL;                                /* PWM Polarity Register; 0x000000A1 */
volatile PWMCLKSTR _PWMCLK;                                /* PWM Clock Select Register; 0x000000A2 */
volatile PWMPRCLKSTR _PWMPRCLK;                            /* PWM Prescale Clock Select Register; 0x000000A3 */
volatile PWMCAESTR _PWMCAE;                                /* PWM Center Align Enable Register; 0x000000A4 */
volatile PWMCTLSTR _PWMCTL;                                /* PWM Control Register; 0x000000A5 */
volatile PWMSCLASTR _PWMSCLA;                              /* PWM Scale A Register; 0x000000A8 */
volatile PWMSCLBSTR _PWMSCLB;                              /* PWM Scale B Register; 0x000000A9 */
volatile PWMSDNSTR _PWMSDN;                                /* PWM Shutdown Register; 0x000000C4 */
volatile SCI0CR1STR _SCI0CR1;                              /* SCI 0 Control Register 1; 0x000000CA */
volatile SCI0CR2STR _SCI0CR2;                              /* SCI 0 Control Register 2; 0x000000CB */
volatile SCI0SR1STR _SCI0SR1;                              /* SCI 0 Status Register 1; 0x000000CC */
volatile SCI0SR2STR _SCI0SR2;                              /* SCI 0 Status Register 2; 0x000000CD */
volatile SCI0DRHSTR _SCI0DRH;                              /* SCI 0 Data Register High; 0x000000CE */
volatile SCI0DRLSTR _SCI0DRL;                              /* SCI 0 Data Register Low; 0x000000CF */
volatile SCI1CR1STR _SCI1CR1;                              /* SCI 1 Control Register 1; 0x000000D2 */
volatile SCI1CR2STR _SCI1CR2;                              /* SCI 1 Control Register 2; 0x000000D3 */
volatile SCI1SR1STR _SCI1SR1;                              /* SCI 1 Status Register 1; 0x000000D4 */
volatile SCI1SR2STR _SCI1SR2;                              /* SCI 1 Status Register 2; 0x000000D5 */
volatile SCI1DRHSTR _SCI1DRH;                              /* SCI 1 Data Register High; 0x000000D6 */
volatile SCI1DRLSTR _SCI1DRL;                              /* SCI 1 Data Register Low; 0x000000D7 */
volatile SPI0CR1STR _SPI0CR1;                              /* SPI 0 Control Register; 0x000000D8 */
volatile SPI0CR2STR _SPI0CR2;                              /* SPI 0 Control Register 2; 0x000000D9 */
volatile SPI0BRSTR _SPI0BR;                                /* SPI 0 Baud Rate Register; 0x000000DA */
volatile SPI0SRSTR _SPI0SR;                                /* SPI 0 Status Register; 0x000000DB */
volatile SPI0DRSTR _SPI0DR;                                /* SPI 0 Data Register; 0x000000DD */
volatile IBADSTR _IBAD;                                    /* IIC Address Register; 0x000000E0 */
volatile IBFDSTR _IBFD;                                    /* IIC Frequency Divider Register; 0x000000E1 */
volatile IBCRSTR _IBCR;                                    /* IIC Control Register; 0x000000E2 */
volatile IBSRSTR _IBSR;                                    /* IIC Status Register; 0x000000E3 */
volatile IBDRSTR _IBDR;                                    /* IIC Data I/O Register; 0x000000E4 */
volatile SPI1CR1STR _SPI1CR1;                              /* SPI 1 Control Register; 0x000000F0 */
volatile SPI1CR2STR _SPI1CR2;                              /* SPI 1 Control Register 2; 0x000000F1 */
volatile SPI1BRSTR _SPI1BR;                                /* SPI 1 Baud Rate Register; 0x000000F2 */
volatile SPI1SRSTR _SPI1SR;                                /* SPI 1 Status Register; 0x000000F3 */
volatile SPI1DRSTR _SPI1DR;                                /* SPI 1 Data Register; 0x000000F5 */
volatile SPI2CR1STR _SPI2CR1;                              /* SPI 2 Control Register; 0x000000F8 */
volatile SPI2CR2STR _SPI2CR2;                              /* SPI 2 Control Register 2; 0x000000F9 */
volatile SPI2BRSTR _SPI2BR;                                /* SPI 2 Baud Rate Register; 0x000000FA */
volatile SPI2SRSTR _SPI2SR;                                /* SPI 2 Status Register; 0x000000FB */
volatile SPI2DRSTR _SPI2DR;                                /* SPI 2 Data Register; 0x000000FD */
volatile FCLKDIVSTR _FCLKDIV;                              /* Flash Clock Divider Register; 0x00000100 */
volatile FSECSTR _FSEC;                                    /* Flash Security Register; 0x00000101 */
volatile FCNFGSTR _FCNFG;                                  /* Flash Configuration Register; 0x00000103 */
volatile FPROTSTR _FPROT;                                  /* Flash Protection Register; 0x00000104 */
volatile FSTATSTR _FSTAT;                                  /* Flash Status Register; 0x00000105 */
volatile FCMDSTR _FCMD;                                    /* Flash Command Buffer and Register; 0x00000106 */
volatile ECLKDIVSTR _ECLKDIV;                              /* EEPROM Clock Divider Register; 0x00000110 */
volatile ECNFGSTR _ECNFG;                                  /* EEPROM Configuration Register; 0x00000113 */
volatile EPROTSTR _EPROT;                                  /* EEPROM Protection Register; 0x00000114 */
volatile ESTATSTR _ESTAT;                                  /* EEPROM Status Register; 0x00000115 */
volatile ECMDSTR _ECMD;                                    /* EEPROM Command Buffer and Register; 0x00000116 */
volatile ATD1STAT0STR _ATD1STAT0;                          /* ATD 1 Status Register 0; 0x00000126 */
volatile ATD1TEST1STR _ATD1TEST1;                          /* ATD1 Test Register; 0x00000129 */
volatile ATD1STAT1STR _ATD1STAT1;                          /* ATD 1 Status Register 1; 0x0000012B */
volatile ATD1DIENSTR _ATD1DIEN;                            /* ATD 1 Input Enable Register; 0x0000012D */
volatile PORTAD1STR _PORTAD1;                              /* Port AD1 Register; 0x0000012F */
volatile CAN0CTL0STR _CAN0CTL0;                            /* MSCAN 0 Control 0 Register; 0x00000140 */
volatile CAN0CTL1STR _CAN0CTL1;                            /* MSCAN 0 Control 1 Register; 0x00000141 */
volatile CAN0BTR0STR _CAN0BTR0;                            /* MSCAN 0 Bus Timing Register 0; 0x00000142 */
volatile CAN0BTR1STR _CAN0BTR1;                            /* MSCAN 0 Bus Timing Register 1; 0x00000143 */
volatile CAN0RFLGSTR _CAN0RFLG;                            /* MSCAN 0 Receiver Flag Register; 0x00000144 */
volatile CAN0RIERSTR _CAN0RIER;                            /* MSCAN 0 Receiver Interrupt Enable Register; 0x00000145 */
volatile CAN0TFLGSTR _CAN0TFLG;                            /* MSCAN 0 Transmitter Flag Register; 0x00000146 */
volatile CAN0TIERSTR _CAN0TIER;                            /* MSCAN 0 Transmitter Interrupt Enable Register; 0x00000147 */
volatile CAN0TARQSTR _CAN0TARQ;                            /* MSCAN 0 Transmitter Message Abort Request; 0x00000148 */
volatile CAN0TAAKSTR _CAN0TAAK;                            /* MSCAN 0 Transmitter Message Abort Control; 0x00000149 */
volatile CAN0TBSELSTR _CAN0TBSEL;                          /* MSCAN 0 Transmit Buffer Selection; 0x0000014A */
volatile CAN0IDACSTR _CAN0IDAC;                            /* MSCAN 0 Identifier Acceptance Control Register; 0x0000014B */
volatile CAN0RXERRSTR _CAN0RXERR;                          /* MSCAN 0 Receive Error Counter Register; 0x0000014E */
volatile CAN0TXERRSTR _CAN0TXERR;                          /* MSCAN 0 Transmit Error Counter Register; 0x0000014F */
volatile CAN0IDAR0STR _CAN0IDAR0;                          /* MSCAN 0 Identifier Acceptance Register 0; 0x00000150 */
volatile CAN0IDAR1STR _CAN0IDAR1;                          /* MSCAN 0 Identifier Acceptance Register 1; 0x00000151 */
volatile CAN0IDAR2STR _CAN0IDAR2;                          /* MSCAN 0 Identifier Acceptance Register 2; 0x00000152 */
volatile CAN0IDAR3STR _CAN0IDAR3;                          /* MSCAN 0 Identifier Acceptance Register 3; 0x00000153 */
volatile CAN0IDMR0STR _CAN0IDMR0;                          /* MSCAN 0 Identifier Mask Register 0; 0x00000154 */
volatile CAN0IDMR1STR _CAN0IDMR1;                          /* MSCAN 0 Identifier Mask Register 1; 0x00000155 */
volatile CAN0IDMR2STR _CAN0IDMR2;                          /* MSCAN 0 Identifier Mask Register 2; 0x00000156 */
volatile CAN0IDMR3STR _CAN0IDMR3;                          /* MSCAN 0 Identifier Mask Register 3; 0x00000157 */
volatile CAN0IDAR4STR _CAN0IDAR4;                          /* MSCAN 0 Identifier Acceptance Register 4; 0x00000158 */
volatile CAN0IDAR5STR _CAN0IDAR5;                          /* MSCAN 0 Identifier Acceptance Register 5; 0x00000159 */
volatile CAN0IDAR6STR _CAN0IDAR6;                          /* MSCAN 0 Identifier Acceptance Register 6; 0x0000015A */
volatile CAN0IDAR7STR _CAN0IDAR7;                          /* MSCAN 0 Identifier Acceptance Register 7; 0x0000015B */
volatile CAN0IDMR4STR _CAN0IDMR4;                          /* MSCAN 0 Identifier Mask Register 4; 0x0000015C */
volatile CAN0IDMR5STR _CAN0IDMR5;                          /* MSCAN 0 Identifier Mask Register 5; 0x0000015D */
volatile CAN0IDMR6STR _CAN0IDMR6;                          /* MSCAN 0 Identifier Mask Register 6; 0x0000015E */
volatile CAN0IDMR7STR _CAN0IDMR7;                          /* MSCAN 0 Identifier Mask Register 7; 0x0000015F */
volatile CAN0RXIDR0STR _CAN0RXIDR0;                        /* MSCAN 0 Receive Identifier Register 0; 0x00000160 */
volatile CAN0RXIDR1STR _CAN0RXIDR1;                        /* MSCAN 0 Receive Identifier Register 1; 0x00000161 */
volatile CAN0RXIDR2STR _CAN0RXIDR2;                        /* MSCAN 0 Receive Identifier Register 2; 0x00000162 */
volatile CAN0RXIDR3STR _CAN0RXIDR3;                        /* MSCAN 0 Receive Identifier Register 3; 0x00000163 */
volatile CAN0RXDSR0STR _CAN0RXDSR0;                        /* MSCAN 0 Receive Data Segment Register 0; 0x00000164 */
volatile CAN0RXDSR1STR _CAN0RXDSR1;                        /* MSCAN 0 Receive Data Segment Register 1; 0x00000165 */
volatile CAN0RXDSR2STR _CAN0RXDSR2;                        /* MSCAN 0 Receive Data Segment Register 2; 0x00000166 */
volatile CAN0RXDSR3STR _CAN0RXDSR3;                        /* MSCAN 0 Receive Data Segment Register 3; 0x00000167 */
volatile CAN0RXDSR4STR _CAN0RXDSR4;                        /* MSCAN 0 Receive Data Segment Register 4; 0x00000168 */
volatile CAN0RXDSR5STR _CAN0RXDSR5;                        /* MSCAN 0 Receive Data Segment Register 5; 0x00000169 */
volatile CAN0RXDSR6STR _CAN0RXDSR6;                        /* MSCAN 0 Receive Data Segment Register 6; 0x0000016A */
volatile CAN0RXDSR7STR _CAN0RXDSR7;                        /* MSCAN 0 Receive Data Segment Register 7; 0x0000016B */
volatile CAN0RXDLRSTR _CAN0RXDLR;                          /* MSCAN 0 Receive Data Length Register; 0x0000016C */
volatile CAN0TXIDR0STR _CAN0TXIDR0;                        /* MSCAN 0 Transmit Identifier Register 0; 0x00000170 */
volatile CAN0TXIDR1STR _CAN0TXIDR1;                        /* MSCAN 0 Transmit Identifier Register 1; 0x00000171 */
volatile CAN0TXIDR2STR _CAN0TXIDR2;                        /* MSCAN 0 Transmit Identifier Register 2; 0x00000172 */
volatile CAN0TXIDR3STR _CAN0TXIDR3;                        /* MSCAN 0 Transmit Identifier Register 3; 0x00000173 */
volatile CAN0TXDSR0STR _CAN0TXDSR0;                        /* MSCAN 0 Transmit Data Segment Register 0; 0x00000174 */
volatile CAN0TXDSR1STR _CAN0TXDSR1;                        /* MSCAN 0 Transmit Data Segment Register 1; 0x00000175 */
volatile CAN0TXDSR2STR _CAN0TXDSR2;                        /* MSCAN 0 Transmit Data Segment Register 2; 0x00000176 */
volatile CAN0TXDSR3STR _CAN0TXDSR3;                        /* MSCAN 0 Transmit Data Segment Register 3; 0x00000177 */
volatile CAN0TXDSR4STR _CAN0TXDSR4;                        /* MSCAN 0 Transmit Data Segment Register 4; 0x00000178 */
volatile CAN0TXDSR5STR _CAN0TXDSR5;                        /* MSCAN 0 Transmit Data Segment Register 5; 0x00000179 */
volatile CAN0TXDSR6STR _CAN0TXDSR6;                        /* MSCAN 0 Transmit Data Segment Register 6; 0x0000017A */
volatile CAN0TXDSR7STR _CAN0TXDSR7;                        /* MSCAN 0 Transmit Data Segment Register 7; 0x0000017B */
volatile CAN0TXDLRSTR _CAN0TXDLR;                          /* MSCAN 0 Transmit Data Length Register; 0x0000017C */
volatile CAN0TXTBPRSTR _CAN0TXTBPR;                        /* MSCAN 0 Transmit Buffer Priority; 0x0000017D */
volatile PTTSTR _PTT;                                      /* Port T I/O Register; 0x00000240 */
volatile PTITSTR _PTIT;                                    /* Port T Input Register; 0x00000241 */
volatile DDRTSTR _DDRT;                                    /* Port T Data Direction Register; 0x00000242 */
volatile RDRTSTR _RDRT;                                    /* Port T Reduced Drive Register; 0x00000243 */
volatile PERTSTR _PERT;                                    /* Port T Pull Device Enable Register; 0x00000244 */
volatile PPSTSTR _PPST;                                    /* Port T Polarity Select Register; 0x00000245 */
volatile PTSSTR _PTS;                                      /* Port S I/O Register; 0x00000248 */
volatile PTISSTR _PTIS;                                    /* Port S Input Register; 0x00000249 */
volatile DDRSSTR _DDRS;                                    /* Port S Data Direction Register; 0x0000024A */
volatile RDRSSTR _RDRS;                                    /* Port S Reduced Drive Register; 0x0000024B */
volatile PERSSTR _PERS;                                    /* Port S Pull Device Enable Register; 0x0000024C */
volatile PPSSSTR _PPSS;                                    /* Port S Polarity Select Register; 0x0000024D */
volatile WOMSSTR _WOMS;                                    /* Port S Wired-Or Mode Register; 0x0000024E */
volatile PTMSTR _PTM;                                      /* Port M I/O Register; 0x00000250 */
volatile PTIMSTR _PTIM;                                    /* Port M Input Register; 0x00000251 */
volatile DDRMSTR _DDRM;                                    /* Port M Data Direction Register; 0x00000252 */
volatile RDRMSTR _RDRM;                                    /* Port M Reduced Drive Register; 0x00000253 */
volatile PERMSTR _PERM;                                    /* Port M Pull Device Enable Register; 0x00000254 */
volatile PPSMSTR _PPSM;                                    /* Port M Polarity Select Register; 0x00000255 */
volatile WOMMSTR _WOMM;                                    /* Port M Wired-Or Mode Register; 0x00000256 */
volatile MODRRSTR _MODRR;                                  /* Module Routing Register; 0x00000257 */
volatile PTPSTR _PTP;                                      /* Port P I/O Register; 0x00000258 */
volatile PTIPSTR _PTIP;                                    /* Port P Input Register; 0x00000259 */
volatile DDRPSTR _DDRP;                                    /* Port P Data Direction Register; 0x0000025A */
volatile RDRPSTR _RDRP;                                    /* Port P Reduced Drive Register; 0x0000025B */
volatile PERPSTR _PERP;                                    /* Port P Pull Device Enable Register; 0x0000025C */
volatile PPSPSTR _PPSP;                                    /* Port P Polarity Select Register; 0x0000025D */
volatile PIEPSTR _PIEP;                                    /* Port P Interrupt Enable Register; 0x0000025E */
volatile PIFPSTR _PIFP;                                    /* Port P Interrupt Flag Register; 0x0000025F */
volatile PTHSTR _PTH;                                      /* Port H I/O Register; 0x00000260 */
volatile PTIHSTR _PTIH;                                    /* Port H Input Register; 0x00000261 */
volatile DDRHSTR _DDRH;                                    /* Port H Data Direction Register; 0x00000262 */
volatile RDRHSTR _RDRH;                                    /* Port H Reduced Drive Register; 0x00000263 */
volatile PERHSTR _PERH;                                    /* Port H Pull Device Enable Register; 0x00000264 */
volatile PPSHSTR _PPSH;                                    /* Port H Polarity Select Register; 0x00000265 */
volatile PIEHSTR _PIEH;                                    /* Port H Interrupt Enable Register; 0x00000266 */
volatile PIFHSTR _PIFH;                                    /* Port H Interrupt Flag Register; 0x00000267 */
volatile PTJSTR _PTJ;                                      /* Port J I/O Register; 0x00000268 */
volatile PTIJSTR _PTIJ;                                    /* Port J Input Register; 0x00000269 */
volatile DDRJSTR _DDRJ;                                    /* Port J Data Direction Register; 0x0000026A */
volatile RDRJSTR _RDRJ;                                    /* Port J Reduced Drive Register; 0x0000026B */
volatile PERJSTR _PERJ;                                    /* Port J Pull Device Enable Register; 0x0000026C */
volatile PPSJSTR _PPSJ;                                    /* Port J Polarity Select Register; 0x0000026D */
volatile PIEJSTR _PIEJ;                                    /* Port J Interrupt Enable Register; 0x0000026E */
volatile PIFJSTR _PIFJ;                                    /* Port J Interrupt Flag Register; 0x0000026F */
volatile CAN4CTL0STR _CAN4CTL0;                            /* MSCAN4 Control 0 Register; 0x00000280 */
volatile CAN4CTL1STR _CAN4CTL1;                            /* MSCAN4 Control 1 Register; 0x00000281 */
volatile CAN4BTR0STR _CAN4BTR0;                            /* MSCAN4 Bus Timing Register 0; 0x00000282 */
volatile CAN4BTR1STR _CAN4BTR1;                            /* MSCAN4 Bus Timing Register 1; 0x00000283 */
volatile CAN4RFLGSTR _CAN4RFLG;                            /* MSCAN4 Receiver Flag Register; 0x00000284 */
volatile CAN4RIERSTR _CAN4RIER;                            /* MSCAN4 Receiver Interrupt Enable Register; 0x00000285 */
volatile CAN4TFLGSTR _CAN4TFLG;                            /* MSCAN4 Transmitter Flag Register; 0x00000286 */
volatile CAN4TIERSTR _CAN4TIER;                            /* MSCAN4 Transmitter Interrupt Enable Register; 0x00000287 */
volatile CAN4TARQSTR _CAN4TARQ;                            /* MSCAN 4 Transmitter Message Abort Request; 0x00000288 */
volatile CAN4TAAKSTR _CAN4TAAK;                            /* MSCAN4 Transmitter Message Abort Control; 0x00000289 */
volatile CAN4TBSELSTR _CAN4TBSEL;                          /* MSCAN4 Transmit Buffer Selection; 0x0000028A */
volatile CAN4IDACSTR _CAN4IDAC;                            /* MSCAN4 Identifier Acceptance Control Register; 0x0000028B */
volatile CAN4RXERRSTR _CAN4RXERR;                          /* MSCAN4 Receive Error Counter Register; 0x0000028E */
volatile CAN4TXERRSTR _CAN4TXERR;                          /* MSCAN4 Transmit Error Counter Register; 0x0000028F */
volatile CAN4IDAR0STR _CAN4IDAR0;                          /* MSCAN4 Identifier Acceptance Register 0; 0x00000290 */
volatile CAN4IDAR1STR _CAN4IDAR1;                          /* MSCAN4 Identifier Acceptance Register 1; 0x00000291 */
volatile CAN4IDAR2STR _CAN4IDAR2;                          /* MSCAN4 Identifier Acceptance Register 2; 0x00000292 */
volatile CAN4IDAR3STR _CAN4IDAR3;                          /* MSCAN4 Identifier Acceptance Register 3; 0x00000293 */
volatile CAN4IDMR0STR _CAN4IDMR0;                          /* MSCAN4 Identifier Mask Register 0; 0x00000294 */
volatile CAN4IDMR1STR _CAN4IDMR1;                          /* MSCAN4 Identifier Mask Register 1; 0x00000295 */
volatile CAN4IDMR2STR _CAN4IDMR2;                          /* MSCAN4 Identifier Mask Register 2; 0x00000296 */
volatile CAN4IDMR3STR _CAN4IDMR3;                          /* MSCAN4 Identifier Mask Register 3; 0x00000297 */
volatile CAN4IDAR4STR _CAN4IDAR4;                          /* MSCAN4 Identifier Acceptance Register 4; 0x00000298 */
volatile CAN4IDAR5STR _CAN4IDAR5;                          /* MSCAN4 Identifier Acceptance Register 5; 0x00000299 */
volatile CAN4IDAR6STR _CAN4IDAR6;                          /* MSCAN4 Identifier Acceptance Register 6; 0x0000029A */
volatile CAN4IDAR7STR _CAN4IDAR7;                          /* MSCAN4 Identifier Acceptance Register 7; 0x0000029B */
volatile CAN4IDMR4STR _CAN4IDMR4;                          /* MSCAN4 Identifier Mask Register 4; 0x0000029C */
volatile CAN4IDMR5STR _CAN4IDMR5;                          /* MSCAN4 Identifier Mask Register 5; 0x0000029D */
volatile CAN4IDMR6STR _CAN4IDMR6;                          /* MSCAN4 Identifier Mask Register 6; 0x0000029E */
volatile CAN4IDMR7STR _CAN4IDMR7;                          /* MSCAN4 Identifier Mask Register 7; 0x0000029F */
volatile CAN4RXIDR0STR _CAN4RXIDR0;                        /* MSCAN4 Receive Identifier Register 0; 0x000002A0 */
volatile CAN4RXIDR1STR _CAN4RXIDR1;                        /* MSCAN4 Receive Identifier Register 1; 0x000002A1 */
volatile CAN4RXIDR2STR _CAN4RXIDR2;                        /* MSCAN4 Receive Identifier Register 2; 0x000002A2 */
volatile CAN4RXIDR3STR _CAN4RXIDR3;                        /* MSCAN4 Receive Identifier Register 3; 0x000002A3 */
volatile CAN4RXDSR0STR _CAN4RXDSR0;                        /* MSCAN4 Receive Data Segment Register 0; 0x000002A4 */
volatile CAN4RXDSR1STR _CAN4RXDSR1;                        /* MSCAN4 Receive Data Segment Register 1; 0x000002A5 */
volatile CAN4RXDSR2STR _CAN4RXDSR2;                        /* MSCAN4 Receive Data Segment Register 2; 0x000002A6 */
volatile CAN4RXDSR3STR _CAN4RXDSR3;                        /* MSCAN4 Receive Data Segment Register 3; 0x000002A7 */
volatile CAN4RXDSR4STR _CAN4RXDSR4;                        /* MSCAN4 Receive Data Segment Register 4; 0x000002A8 */
volatile CAN4RXDSR5STR _CAN4RXDSR5;                        /* MSCAN4 Receive Data Segment Register 5; 0x000002A9 */
volatile CAN4RXDSR6STR _CAN4RXDSR6;                        /* MSCAN4 Receive Data Segment Register 6; 0x000002AA */
volatile CAN4RXDSR7STR _CAN4RXDSR7;                        /* MSCAN4 Receive Data Segment Register 7; 0x000002AB */
volatile CAN4RXDLRSTR _CAN4RXDLR;                          /* MSCAN4 Receive Data Length Register; 0x000002AC */
volatile CAN4TXIDR0STR _CAN4TXIDR0;                        /* MSCAN4 Transmit Identifier Register 0; 0x000002B0 */
volatile CAN4TXIDR1STR _CAN4TXIDR1;                        /* MSCAN4 Transmit Identifier Register 1; 0x000002B1 */
volatile CAN4TXIDR2STR _CAN4TXIDR2;                        /* MSCAN4 Transmit Identifier Register 2; 0x000002B2 */
volatile CAN4TXIDR3STR _CAN4TXIDR3;                        /* MSCAN4 Transmit Identifier Register 3; 0x000002B3 */
volatile CAN4TXDSR0STR _CAN4TXDSR0;                        /* MSCAN4 Transmit Data Segment Register 0; 0x000002B4 */
volatile CAN4TXDSR1STR _CAN4TXDSR1;                        /* MSCAN4 Transmit Data Segment Register 1; 0x000002B5 */
volatile CAN4TXDSR2STR _CAN4TXDSR2;                        /* MSCAN4 Transmit Data Segment Register 2; 0x000002B6 */
volatile CAN4TXDSR3STR _CAN4TXDSR3;                        /* MSCAN4 Transmit Data Segment Register 3; 0x000002B7 */
volatile CAN4TXDSR4STR _CAN4TXDSR4;                        /* MSCAN4 Transmit Data Segment Register 4; 0x000002B8 */
volatile CAN4TXDSR5STR _CAN4TXDSR5;                        /* MSCAN4 Transmit Data Segment Register 5; 0x000002B9 */
volatile CAN4TXDSR6STR _CAN4TXDSR6;                        /* MSCAN4 Transmit Data Segment Register 6; 0x000002BA */
volatile CAN4TXDSR7STR _CAN4TXDSR7;                        /* MSCAN4 Transmit Data Segment Register 7; 0x000002BB */
volatile CAN4TXDLRSTR _CAN4TXDLR;                          /* MSCAN4 Transmit Data Length Register; 0x000002BC */
volatile CAN4TXTBPRSTR _CAN4TXTBPR;                        /* MSCAN4 Transmit Transmit Buffer Priority; 0x000002BD */
/* NVFPROT3 - macro for reading non volatile register      Non volatile Block 3 Flash Protection Register; 0x0000FF0A */
/* NVFPROT2 - macro for reading non volatile register      Non volatile Block 2 Flash Protection Register; 0x0000FF0B */
/* NVFPROT1 - macro for reading non volatile register      Non volatile Block 1 Flash Protection Register; 0x0000FF0C */
/* NVFPROT0 - macro for reading non volatile register      Non volatile Block 0 Flash Protection Register; 0x0000FF0D */
/* NVFSEC - macro for reading non volatile register        Non volatile Flash Security Register; 0x0000FF0F */


/* * * * *  16-BIT REGISTERS  * * * * * * * * * * * * * * * */
volatile PORTABSTR _PORTAB;                                /* Port AB Register; 0x00000000 */
volatile DDRABSTR _DDRAB;                                  /* Port AB Data Direction Register; 0x00000002 */
volatile PARTIDSTR _PARTID;                                /* Part ID Register; 0x0000001A */
volatile TCNTSTR _TCNT;                                    /* Timer Count Register; 0x00000044 */
volatile TC0STR _TC0;                                      /* Timer Input Capture/Output Compare Register 0; 0x00000050 */
volatile TC1STR _TC1;                                      /* Timer Input Capture/Output Compare Register 1; 0x00000052 */
volatile TC2STR _TC2;                                      /* Timer Input Capture/Output Compare Register 2; 0x00000054 */
volatile TC3STR _TC3;                                      /* Timer Input Capture/Output Compare Register 3; 0x00000056 */
volatile TC4STR _TC4;                                      /* Timer Input Capture/Output Compare Register 4; 0x00000058 */
volatile TC5STR _TC5;                                      /* Timer Input Capture/Output Compare Register 5; 0x0000005A */
volatile TC6STR _TC6;                                      /* Timer Input Capture/Output Compare Register 6; 0x0000005C */
volatile TC7STR _TC7;                                      /* Timer Input Capture/Output Compare Register 7; 0x0000005E */
volatile PACN32STR _PACN32;                                /* Pulse Accumulators Count 32 Register; 0x00000062 */
volatile PACN10STR _PACN10;                                /* Pulse Accumulators Count 10 Register; 0x00000064 */
volatile PA32HSTR _PA32H;                                  /* 8-Bit Pulse Accumulators Holding 32 Register; 0x00000072 */
volatile PA10HSTR _PA10H;                                  /* 8-Bit Pulse Accumulators Holding 10 Register; 0x00000074 */
volatile MCCNTSTR _MCCNT;                                  /* Modulus Down-Counter Count Register; 0x00000076 */
volatile TC0HSTR _TC0H;                                    /* Timer Input Capture Holding Registers 0; 0x00000078 */
volatile TC1HSTR _TC1H;                                    /* Timer Input Capture Holding Registers 1; 0x0000007A */
volatile TC2HSTR _TC2H;                                    /* Timer Input Capture Holding Registers 2; 0x0000007C */
volatile TC3HSTR _TC3H;                                    /* Timer Input Capture Holding Registers 3; 0x0000007E */
volatile ATD0CTL23STR _ATD0CTL23;                          /* ATD 0 Control Register 23; 0x00000082 */
volatile ATD0CTL45STR _ATD0CTL45;                          /* ATD 0 Control Register 45; 0x00000084 */
volatile ATD0DR0STR _ATD0DR0;                              /* ATD 0 Conversion Result Register 0; 0x00000090 */
volatile ATD0DR1STR _ATD0DR1;                              /* ATD 0 Conversion Result Register 1; 0x00000092 */
volatile ATD0DR2STR _ATD0DR2;                              /* ATD 0 Conversion Result Register 2; 0x00000094 */
volatile ATD0DR3STR _ATD0DR3;                              /* ATD 0 Conversion Result Register 3; 0x00000096 */
volatile ATD0DR4STR _ATD0DR4;                              /* ATD 0 Conversion Result Register 4; 0x00000098 */
volatile ATD0DR5STR _ATD0DR5;                              /* ATD 0 Conversion Result Register 5; 0x0000009A */
volatile ATD0DR6STR _ATD0DR6;                              /* ATD 0 Conversion Result Register 6; 0x0000009C */
volatile ATD0DR7STR _ATD0DR7;                              /* ATD 0 Conversion Result Register 7; 0x0000009E */
volatile PWMCNT01STR _PWMCNT01;                            /* PWM Channel Counter 01 Register; 0x000000AC */
volatile PWMCNT23STR _PWMCNT23;                            /* PWM Channel Counter 23 Register; 0x000000AE */
volatile PWMCNT45STR _PWMCNT45;                            /* PWM Channel Counter 45 Register; 0x000000B0 */
volatile PWMCNT67STR _PWMCNT67;                            /* PWM Channel Counter 67 Register; 0x000000B2 */
volatile PWMPER01STR _PWMPER01;                            /* PWM Channel Period 01 Register; 0x000000B4 */
volatile PWMPER23STR _PWMPER23;                            /* PWM Channel Period 23 Register; 0x000000B6 */
volatile PWMPER45STR _PWMPER45;                            /* PWM Channel Period 45 Register; 0x000000B8 */
volatile PWMPER67STR _PWMPER67;                            /* PWM Channel Period 67 Register; 0x000000BA */
volatile PWMDTY01STR _PWMDTY01;                            /* PWM Channel Duty 01 Register; 0x000000BC */
volatile PWMDTY23STR _PWMDTY23;                            /* PWM Channel Duty 23 Register; 0x000000BE */
volatile PWMDTY45STR _PWMDTY45;                            /* PWM Channel Duty 45 Register; 0x000000C0 */
volatile PWMDTY67STR _PWMDTY67;                            /* PWM Channel Duty 67 Register; 0x000000C2 */
volatile SCI0BDSTR _SCI0BD;                                /* SCI 0 Baud Rate Register; 0x000000C8 */
volatile SCI1BDSTR _SCI1BD;                                /* SCI 1 Baud Rate Register; 0x000000D0 */
volatile ATD1CTL23STR _ATD1CTL23;                          /* ATD 1 Control Register 23; 0x00000122 */
volatile ATD1CTL45STR _ATD1CTL45;                          /* ATD 1 Control Register 45; 0x00000124 */
volatile ATD1DR0STR _ATD1DR0;                              /* ATD 1 Conversion Result Register 0; 0x00000130 */
volatile ATD1DR1STR _ATD1DR1;                              /* ATD 1 Conversion Result Register 1; 0x00000132 */
volatile ATD1DR2STR _ATD1DR2;                              /* ATD 1 Conversion Result Register 2; 0x00000134 */
volatile ATD1DR3STR _ATD1DR3;                              /* ATD 1 Conversion Result Register 3; 0x00000136 */
volatile ATD1DR4STR _ATD1DR4;                              /* ATD 1 Conversion Result Register 4; 0x00000138 */
volatile ATD1DR5STR _ATD1DR5;                              /* ATD 1 Conversion Result Register 5; 0x0000013A */
volatile ATD1DR6STR _ATD1DR6;                              /* ATD 1 Conversion Result Register 6; 0x0000013C */
volatile ATD1DR7STR _ATD1DR7;                              /* ATD 1 Conversion Result Register 7; 0x0000013E */
volatile CAN0RXTSRSTR _CAN0RXTSR;                          /* MSCAN 0 Receive Time Stamp Register; 0x0000016E */
volatile CAN0TXTSRSTR _CAN0TXTSR;                          /* MSCAN 0 Transmit Time Stamp Register; 0x0000017E */
volatile CAN4RXTSRSTR _CAN4RXTSR;                          /* MSCAN 4 Receive Time Stamp Register; 0x000002AE */
volatile CAN4TXTSRSTR _CAN4TXTSR;                          /* MSCAN 4 Transmit Time Stamp Register; 0x000002BE */
/* BAKEY0 - macro for reading non volatile register        Backdoor Access Key 0; 0x0000FF00 */
/* BAKEY1 - macro for reading non volatile register        Backdoor Access Key 1; 0x0000FF02 */
/* BAKEY2 - macro for reading non volatile register        Backdoor Access Key 2; 0x0000FF04 */
/* BAKEY3 - macro for reading non volatile register        Backdoor Access Key 3; 0x0000FF06 */

/*lint -restore */

/* EOF */
