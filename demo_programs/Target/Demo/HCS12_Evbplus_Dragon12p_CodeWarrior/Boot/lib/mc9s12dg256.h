/* Based on CPU DB MC9S12DG256_112, version 2.87.347 (RegistersPrg V2.32) */
/*
** ###################################################################
**     Filename  : mc9s12dg256.h
**     Processor : MC9S12DG256CPV
**     FileFormat: V2.32
**     DataSheet : 9S12DT256DGV3/D V03.04
**     Compiler  : CodeWarrior compiler
**     Date/Time : 8.6.2010, 9:00
**     Abstract  :
**         This header implements the mapping of I/O devices.
**
**     Copyright : 1997 - 2010 Freescale Semiconductor, Inc. All Rights Reserved.
**     
**     http      : www.freescale.com
**     mail      : support@freescale.com
**
**     CPU Registers Revisions:
**      - 24.05.2006, V2.87.285:
**              - Removed bits MCCNTlo_BIT0..MCCNTlo_BIT7 and MCCNThi_BIT8.. MCCNThi_BIT15. REASON: Bug-fix (#3166 in Issue Manager)
**
**     File-Format-Revisions:
**      - 14.11.2005, V2.00 :
**               - Deprecated symbols added for backward compatibility (section at the end of this file)
**      - 15.11.2005, V2.01 :
**               - Fixed invalid instruction in macro __RESET_WATCHDOG for HCS12 family.
**      - 17.12.2005, V2.02 :
**               - Arrays (symbols xx_ARR) are defined as pointer to volatile, see issue #2778
**      - 16.01.2006, V2.03 :
**               - Fixed declaration of non volatile registers. Now it does not require (but allows) their initialization, see issue #2920.
**               - "volatile" modifier removed from declaration of non volatile registers (that contain modifier "const")
**      - 08.03.2006, V2.04 :
**               - Support for bit(s) names duplicated with any register name in .h header files
**      - 24.03.2006, V2.05 :
**               - Fixed macro __RESET_WATCHDOG for HCS12 family - address and correct write order.
**      - 26.04.2006, V2.06 :
**               - Changes have not affected this file (because they are related to another family)
**      - 27.04.2006, V2.07 :
**               - Fixed macro __RESET_WATCHDOG for HCS12, HCS12X ,HCS08 DZ and HCS08 EN derivatives (write 0x55,0xAA).
**      - 07.06.2006, V2.08 :
**               - Changes have not affected this file (because they are related to another family)
**      - 03.07.2006, V2.09 :
**               - Changes have not affected this file (because they are related to another family)
**      - 27.10.2006, V2.10 :
**               - __RESET_WATCHDOG improved formating and re-definition
**      - 23.11.2006, V2.11 :
**               - Changes have not affected this file (because they are related to another family)
**      - 22.01.2007, V2.12 :
**               - Fixed declaration of non volatile registers. Now it does not require (but allows) their initialization, see issue #4086.
**      - 01.03.2007, V2.13 :
**               - Flash commands constants values converted to HEX format
**      - 02.03.2007, V2.14 :
**               - Interrupt vector numbers added into .H, see VectorNumber_*
**      - 26.03.2007, V2.15 :
**               - Changes have not affected this file (because they are related to another family)
**      - 10.05.2007, V2.16 :
**               - Changes have not affected this file (because they are related to another family)
**      - 05.06.2007, V2.17 :
**               - Changes have not affected this file (because they are related to another family)
**      - 19.07.2007, V2.18 :
**               - Improved number of blanked lines inside register structures
**      - 06.08.2007, V2.19 :
**               - CPUDB revisions generated ahead of the file-format revisions.
**      - 11.09.2007, V2.20 :
**               - Added comment about initialization of unbonded pins.
**      - 02.01.2008, V2.21 :
**               - Changes have not affected this file (because they are related to another family)
**      - 13.02.2008, V2.22 :
**               - Changes have not affected this file (because they are related to another family)
**      - 20.02.2008, V2.23 :
**               - Termination of pragma V30toV31Compatible added, #5708
**      - 03.07.2008, V2.24 :
**               - Added support for bits with name starting with number (like "1HZ")
**      - 28.11.2008, V2.25 :
**               - StandBy RAM array declaration for ANSI-C added
**      - 1.12.2008, V2.26 :
**               - Duplication of bit (or bit-group) name with register name is not marked as a problem, is register is internal only and it is not displayed in I/O map.
**      - 17.3.2009, V2.27 :
**               - Merged bit-group is not generated, if the name matches with another bit name in the register
**      - 6.4.2009, V2.28 :
**               - Fixed generation of merged bits for bit-groups with a digit at the end, if group-name is defined in CPUDB
**      - 3.8.2009, V2.29 :
**               - If there is just one bits group matching register name, single bits are not generated
**      - 10.9.2009, V2.30 :
**               - Fixed generation of registers arrays.
**      - 15.10.2009, V2.31 :
**               - Changes have not affected this file (because they are related to another family)
**      - 18.05.2010, V2.32 :
**               - MISRA compliance: U/UL suffixes added to all numbers (_MASK,_BITNUM and addresses)
**
**     Not all general-purpose I/O pins are available on all packages or on all mask sets of a specific
**     derivative device. To avoid extra current drain from floating input pins, the user’s reset
**     initialization routine in the application program must either enable on-chip pull-up devices
**     or change the direction of unconnected pins to outputs so the pins do not float.
** ###################################################################
*/

#ifndef _MC9S12DG256_H
#define _MC9S12DG256_H

/*lint -save  -e950 -esym(960,18.4) -e46 -esym(961,19.7) Disable MISRA rule (1.1,18.4,6.4,19.7) checking. */
/* Types definition */
typedef unsigned char byte;
typedef unsigned int word;
typedef unsigned long dword;
typedef unsigned long dlong[2];

#define REG_BASE 0x0000                /* Base address for the I/O register block */


#pragma MESSAGE DISABLE C1106 /* WARNING C1106: Non-standard bitfield type */
#pragma OPTION ADD V30toV31Compatible "-BfaGapLimitBits4294967295" /*this guarantee correct bitfield positions*/

/**************** interrupt vector numbers ****************/
#define VectorNumber_VReserved63        63U
#define VectorNumber_VReserved62        62U
#define VectorNumber_VReserved61        61U
#define VectorNumber_VReserved60        60U
#define VectorNumber_VReserved59        59U
#define VectorNumber_VReserved58        58U
#define VectorNumber_Vpwmesdn           57U
#define VectorNumber_Vportp             56U
#define VectorNumber_Vcan4tx            55U
#define VectorNumber_Vcan4rx            54U
#define VectorNumber_Vcan4err           53U
#define VectorNumber_Vcan4wkup          52U
#define VectorNumber_VReserved51        51U
#define VectorNumber_VReserved50        50U
#define VectorNumber_VReserved49        49U
#define VectorNumber_VReserved48        48U
#define VectorNumber_VReserved47        47U
#define VectorNumber_VReserved46        46U
#define VectorNumber_VReserved45        45U
#define VectorNumber_VReserved44        44U
#define VectorNumber_VReserved43        43U
#define VectorNumber_VReserved42        42U
#define VectorNumber_VReserved41        41U
#define VectorNumber_VReserved40        40U
#define VectorNumber_Vcan0tx            39U
#define VectorNumber_Vcan0rx            38U
#define VectorNumber_Vcan0err           37U
#define VectorNumber_Vcan0wkup          36U
#define VectorNumber_Vflash             35U
#define VectorNumber_Veeprom            34U
#define VectorNumber_Vspi2              33U
#define VectorNumber_Vspi1              32U
#define VectorNumber_Viic               31U
#define VectorNumber_VReserved30        30U
#define VectorNumber_Vcrgscm            29U
#define VectorNumber_Vcrgplllck         28U
#define VectorNumber_Vtimpabovf         27U
#define VectorNumber_Vtimmdcu           26U
#define VectorNumber_Vporth             25U
#define VectorNumber_Vportj             24U
#define VectorNumber_Vatd1              23U
#define VectorNumber_Vatd0              22U
#define VectorNumber_Vsci1              21U
#define VectorNumber_Vsci0              20U
#define VectorNumber_Vspi0              19U
#define VectorNumber_Vtimpaie           18U
#define VectorNumber_Vtimpaaovf         17U
#define VectorNumber_Vtimovf            16U
#define VectorNumber_Vtimch7            15U
#define VectorNumber_Vtimch6            14U
#define VectorNumber_Vtimch5            13U
#define VectorNumber_Vtimch4            12U
#define VectorNumber_Vtimch3            11U
#define VectorNumber_Vtimch2            10U
#define VectorNumber_Vtimch1            9U
#define VectorNumber_Vtimch0            8U
#define VectorNumber_Vrti               7U
#define VectorNumber_Virq               6U
#define VectorNumber_Vxirq              5U
#define VectorNumber_Vswi               4U
#define VectorNumber_Vtrap              3U
#define VectorNumber_Vcop               2U
#define VectorNumber_Vclkmon            1U
#define VectorNumber_Vreset             0U

/**************** interrupt vector table ****************/
#define VReserved63                     0xFF80U
#define VReserved62                     0xFF82U
#define VReserved61                     0xFF84U
#define VReserved60                     0xFF86U
#define VReserved59                     0xFF88U
#define VReserved58                     0xFF8AU
#define Vpwmesdn                        0xFF8CU
#define Vportp                          0xFF8EU
#define Vcan4tx                         0xFF90U
#define Vcan4rx                         0xFF92U
#define Vcan4err                        0xFF94U
#define Vcan4wkup                       0xFF96U
#define VReserved51                     0xFF98U
#define VReserved50                     0xFF9AU
#define VReserved49                     0xFF9CU
#define VReserved48                     0xFF9EU
#define VReserved47                     0xFFA0U
#define VReserved46                     0xFFA2U
#define VReserved45                     0xFFA4U
#define VReserved44                     0xFFA6U
#define VReserved43                     0xFFA8U
#define VReserved42                     0xFFAAU
#define VReserved41                     0xFFACU
#define VReserved40                     0xFFAEU
#define Vcan0tx                         0xFFB0U
#define Vcan0rx                         0xFFB2U
#define Vcan0err                        0xFFB4U
#define Vcan0wkup                       0xFFB6U
#define Vflash                          0xFFB8U
#define Veeprom                         0xFFBAU
#define Vspi2                           0xFFBCU
#define Vspi1                           0xFFBEU
#define Viic                            0xFFC0U
#define VReserved30                     0xFFC2U
#define Vcrgscm                         0xFFC4U
#define Vcrgplllck                      0xFFC6U
#define Vtimpabovf                      0xFFC8U
#define Vtimmdcu                        0xFFCAU
#define Vporth                          0xFFCCU
#define Vportj                          0xFFCEU
#define Vatd1                           0xFFD0U
#define Vatd0                           0xFFD2U
#define Vsci1                           0xFFD4U
#define Vsci0                           0xFFD6U
#define Vspi0                           0xFFD8U
#define Vtimpaie                        0xFFDAU
#define Vtimpaaovf                      0xFFDCU
#define Vtimovf                         0xFFDEU
#define Vtimch7                         0xFFE0U
#define Vtimch6                         0xFFE2U
#define Vtimch5                         0xFFE4U
#define Vtimch4                         0xFFE6U
#define Vtimch3                         0xFFE8U
#define Vtimch2                         0xFFEAU
#define Vtimch1                         0xFFECU
#define Vtimch0                         0xFFEEU
#define Vrti                            0xFFF0U
#define Virq                            0xFFF2U
#define Vxirq                           0xFFF4U
#define Vswi                            0xFFF6U
#define Vtrap                           0xFFF8U
#define Vcop                            0xFFFAU
#define Vclkmon                         0xFFFCU
#define Vreset                          0xFFFEU

/**************** registers I/O map ****************/

/*** PORTAB - Port AB Register; 0x00000000 ***/
typedef union {
  word Word;
   /* Overlapped registers: */
  struct {
    /*** PORTA - Port A Register; 0x00000000 ***/
    union {
      byte Byte;
      struct {
        byte BIT0        :1;                                       /* Port A Bit 0 */
        byte BIT1        :1;                                       /* Port A Bit 1 */
        byte BIT2        :1;                                       /* Port A Bit 2 */
        byte BIT3        :1;                                       /* Port A Bit 3 */
        byte BIT4        :1;                                       /* Port A Bit 4 */
        byte BIT5        :1;                                       /* Port A Bit 5 */
        byte BIT6        :1;                                       /* Port A Bit 6 */
        byte BIT7        :1;                                       /* Port A Bit 7 */
      } Bits;
    } PORTASTR;
    #define PORTA                       _PORTAB.Overlap_STR.PORTASTR.Byte
    #define PORTA_BIT0                  _PORTAB.Overlap_STR.PORTASTR.Bits.BIT0
    #define PORTA_BIT1                  _PORTAB.Overlap_STR.PORTASTR.Bits.BIT1
    #define PORTA_BIT2                  _PORTAB.Overlap_STR.PORTASTR.Bits.BIT2
    #define PORTA_BIT3                  _PORTAB.Overlap_STR.PORTASTR.Bits.BIT3
    #define PORTA_BIT4                  _PORTAB.Overlap_STR.PORTASTR.Bits.BIT4
    #define PORTA_BIT5                  _PORTAB.Overlap_STR.PORTASTR.Bits.BIT5
    #define PORTA_BIT6                  _PORTAB.Overlap_STR.PORTASTR.Bits.BIT6
    #define PORTA_BIT7                  _PORTAB.Overlap_STR.PORTASTR.Bits.BIT7
    
    #define PORTA_BIT0_MASK             1U
    #define PORTA_BIT1_MASK             2U
    #define PORTA_BIT2_MASK             4U
    #define PORTA_BIT3_MASK             8U
    #define PORTA_BIT4_MASK             16U
    #define PORTA_BIT5_MASK             32U
    #define PORTA_BIT6_MASK             64U
    #define PORTA_BIT7_MASK             128U
    

    /*** PORTB - Port B Register; 0x00000001 ***/
    union {
      byte Byte;
      struct {
        byte BIT0        :1;                                       /* Port B Bit 0 */
        byte BIT1        :1;                                       /* Port B Bit 1 */
        byte BIT2        :1;                                       /* Port B Bit 2 */
        byte BIT3        :1;                                       /* Port B Bit 3 */
        byte BIT4        :1;                                       /* Port B Bit 4 */
        byte BIT5        :1;                                       /* Port B Bit 5 */
        byte BIT6        :1;                                       /* Port B Bit 6 */
        byte BIT7        :1;                                       /* Port B Bit 7 */
      } Bits;
    } PORTBSTR;
    #define PORTB                       _PORTAB.Overlap_STR.PORTBSTR.Byte
    #define PORTB_BIT0                  _PORTAB.Overlap_STR.PORTBSTR.Bits.BIT0
    #define PORTB_BIT1                  _PORTAB.Overlap_STR.PORTBSTR.Bits.BIT1
    #define PORTB_BIT2                  _PORTAB.Overlap_STR.PORTBSTR.Bits.BIT2
    #define PORTB_BIT3                  _PORTAB.Overlap_STR.PORTBSTR.Bits.BIT3
    #define PORTB_BIT4                  _PORTAB.Overlap_STR.PORTBSTR.Bits.BIT4
    #define PORTB_BIT5                  _PORTAB.Overlap_STR.PORTBSTR.Bits.BIT5
    #define PORTB_BIT6                  _PORTAB.Overlap_STR.PORTBSTR.Bits.BIT6
    #define PORTB_BIT7                  _PORTAB.Overlap_STR.PORTBSTR.Bits.BIT7
    
    #define PORTB_BIT0_MASK             1U
    #define PORTB_BIT1_MASK             2U
    #define PORTB_BIT2_MASK             4U
    #define PORTB_BIT3_MASK             8U
    #define PORTB_BIT4_MASK             16U
    #define PORTB_BIT5_MASK             32U
    #define PORTB_BIT6_MASK             64U
    #define PORTB_BIT7_MASK             128U
    
  } Overlap_STR;

  struct {
    word BIT0        :1;                                       /* Port AB Bit 0 */
    word BIT1        :1;                                       /* Port AB Bit 1 */
    word BIT2        :1;                                       /* Port AB Bit 2 */
    word BIT3        :1;                                       /* Port AB Bit 3 */
    word BIT4        :1;                                       /* Port AB Bit 4 */
    word BIT5        :1;                                       /* Port AB Bit 5 */
    word BIT6        :1;                                       /* Port AB Bit 6 */
    word BIT7        :1;                                       /* Port AB Bit 7 */
    word BIT8        :1;                                       /* Port AB Bit 8 */
    word BIT9        :1;                                       /* Port AB Bit 9 */
    word BIT10       :1;                                       /* Port AB Bit 10 */
    word BIT11       :1;                                       /* Port AB Bit 11 */
    word BIT12       :1;                                       /* Port AB Bit 12 */
    word BIT13       :1;                                       /* Port AB Bit 13 */
    word BIT14       :1;                                       /* Port AB Bit 14 */
    word BIT15       :1;                                       /* Port AB Bit 15 */
  } Bits;
} PORTABSTR;
extern volatile PORTABSTR _PORTAB @(REG_BASE + 0x00000000UL);
#define PORTAB                          _PORTAB.Word
#define PORTAB_BIT0                     _PORTAB.Bits.BIT0
#define PORTAB_BIT1                     _PORTAB.Bits.BIT1
#define PORTAB_BIT2                     _PORTAB.Bits.BIT2
#define PORTAB_BIT3                     _PORTAB.Bits.BIT3
#define PORTAB_BIT4                     _PORTAB.Bits.BIT4
#define PORTAB_BIT5                     _PORTAB.Bits.BIT5
#define PORTAB_BIT6                     _PORTAB.Bits.BIT6
#define PORTAB_BIT7                     _PORTAB.Bits.BIT7
#define PORTAB_BIT8                     _PORTAB.Bits.BIT8
#define PORTAB_BIT9                     _PORTAB.Bits.BIT9
#define PORTAB_BIT10                    _PORTAB.Bits.BIT10
#define PORTAB_BIT11                    _PORTAB.Bits.BIT11
#define PORTAB_BIT12                    _PORTAB.Bits.BIT12
#define PORTAB_BIT13                    _PORTAB.Bits.BIT13
#define PORTAB_BIT14                    _PORTAB.Bits.BIT14
#define PORTAB_BIT15                    _PORTAB.Bits.BIT15

#define PORTAB_BIT0_MASK                1U
#define PORTAB_BIT1_MASK                2U
#define PORTAB_BIT2_MASK                4U
#define PORTAB_BIT3_MASK                8U
#define PORTAB_BIT4_MASK                16U
#define PORTAB_BIT5_MASK                32U
#define PORTAB_BIT6_MASK                64U
#define PORTAB_BIT7_MASK                128U
#define PORTAB_BIT8_MASK                256U
#define PORTAB_BIT9_MASK                512U
#define PORTAB_BIT10_MASK               1024U
#define PORTAB_BIT11_MASK               2048U
#define PORTAB_BIT12_MASK               4096U
#define PORTAB_BIT13_MASK               8192U
#define PORTAB_BIT14_MASK               16384U
#define PORTAB_BIT15_MASK               32768U


/*** DDRAB - Port AB Data Direction Register; 0x00000002 ***/
typedef union {
  word Word;
   /* Overlapped registers: */
  struct {
    /*** DDRA - Port A Data Direction Register; 0x00000002 ***/
    union {
      byte Byte;
      struct {
        byte BIT0        :1;                                       /* Data Direction Port A Bit 0 */
        byte BIT1        :1;                                       /* Data Direction Port A Bit 1 */
        byte BIT2        :1;                                       /* Data Direction Port A Bit 2 */
        byte BIT3        :1;                                       /* Data Direction Port A Bit 3 */
        byte BIT4        :1;                                       /* Data Direction Port A Bit 4 */
        byte BIT5        :1;                                       /* Data Direction Port A Bit 5 */
        byte BIT6        :1;                                       /* Data Direction Port A Bit 6 */
        byte BIT7        :1;                                       /* Data Direction Port A Bit 7 */
      } Bits;
    } DDRASTR;
    #define DDRA                        _DDRAB.Overlap_STR.DDRASTR.Byte
    #define DDRA_BIT0                   _DDRAB.Overlap_STR.DDRASTR.Bits.BIT0
    #define DDRA_BIT1                   _DDRAB.Overlap_STR.DDRASTR.Bits.BIT1
    #define DDRA_BIT2                   _DDRAB.Overlap_STR.DDRASTR.Bits.BIT2
    #define DDRA_BIT3                   _DDRAB.Overlap_STR.DDRASTR.Bits.BIT3
    #define DDRA_BIT4                   _DDRAB.Overlap_STR.DDRASTR.Bits.BIT4
    #define DDRA_BIT5                   _DDRAB.Overlap_STR.DDRASTR.Bits.BIT5
    #define DDRA_BIT6                   _DDRAB.Overlap_STR.DDRASTR.Bits.BIT6
    #define DDRA_BIT7                   _DDRAB.Overlap_STR.DDRASTR.Bits.BIT7
    
    #define DDRA_BIT0_MASK              1U
    #define DDRA_BIT1_MASK              2U
    #define DDRA_BIT2_MASK              4U
    #define DDRA_BIT3_MASK              8U
    #define DDRA_BIT4_MASK              16U
    #define DDRA_BIT5_MASK              32U
    #define DDRA_BIT6_MASK              64U
    #define DDRA_BIT7_MASK              128U
    

    /*** DDRB - Port B Data Direction Register; 0x00000003 ***/
    union {
      byte Byte;
      struct {
        byte BIT0        :1;                                       /* Data Direction Port B Bit 0 */
        byte BIT1        :1;                                       /* Data Direction Port B Bit 1 */
        byte BIT2        :1;                                       /* Data Direction Port B Bit 2 */
        byte BIT3        :1;                                       /* Data Direction Port B Bit 3 */
        byte BIT4        :1;                                       /* Data Direction Port B Bit 4 */
        byte BIT5        :1;                                       /* Data Direction Port B Bit 5 */
        byte BIT6        :1;                                       /* Data Direction Port B Bit 6 */
        byte BIT7        :1;                                       /* Data Direction Port B Bit 7 */
      } Bits;
    } DDRBSTR;
    #define DDRB                        _DDRAB.Overlap_STR.DDRBSTR.Byte
    #define DDRB_BIT0                   _DDRAB.Overlap_STR.DDRBSTR.Bits.BIT0
    #define DDRB_BIT1                   _DDRAB.Overlap_STR.DDRBSTR.Bits.BIT1
    #define DDRB_BIT2                   _DDRAB.Overlap_STR.DDRBSTR.Bits.BIT2
    #define DDRB_BIT3                   _DDRAB.Overlap_STR.DDRBSTR.Bits.BIT3
    #define DDRB_BIT4                   _DDRAB.Overlap_STR.DDRBSTR.Bits.BIT4
    #define DDRB_BIT5                   _DDRAB.Overlap_STR.DDRBSTR.Bits.BIT5
    #define DDRB_BIT6                   _DDRAB.Overlap_STR.DDRBSTR.Bits.BIT6
    #define DDRB_BIT7                   _DDRAB.Overlap_STR.DDRBSTR.Bits.BIT7
    
    #define DDRB_BIT0_MASK              1U
    #define DDRB_BIT1_MASK              2U
    #define DDRB_BIT2_MASK              4U
    #define DDRB_BIT3_MASK              8U
    #define DDRB_BIT4_MASK              16U
    #define DDRB_BIT5_MASK              32U
    #define DDRB_BIT6_MASK              64U
    #define DDRB_BIT7_MASK              128U
    
  } Overlap_STR;

  struct {
    word BIT0        :1;                                       /* Data Direction Port AB Bit 0 */
    word BIT1        :1;                                       /* Data Direction Port AB Bit 1 */
    word BIT2        :1;                                       /* Data Direction Port AB Bit 2 */
    word BIT3        :1;                                       /* Data Direction Port AB Bit 3 */
    word BIT4        :1;                                       /* Data Direction Port AB Bit 4 */
    word BIT5        :1;                                       /* Data Direction Port AB Bit 5 */
    word BIT6        :1;                                       /* Data Direction Port AB Bit 6 */
    word BIT7        :1;                                       /* Data Direction Port AB Bit 7 */
    word BIT8        :1;                                       /* Data Direction Port AB Bit 8 */
    word BIT9        :1;                                       /* Data Direction Port AB Bit 9 */
    word BIT10       :1;                                       /* Data Direction Port AB Bit 10 */
    word BIT11       :1;                                       /* Data Direction Port AB Bit 11 */
    word BIT12       :1;                                       /* Data Direction Port AB Bit 12 */
    word BIT13       :1;                                       /* Data Direction Port AB Bit 13 */
    word BIT14       :1;                                       /* Data Direction Port AB Bit 14 */
    word BIT15       :1;                                       /* Data Direction Port AB Bit 15 */
  } Bits;
} DDRABSTR;
extern volatile DDRABSTR _DDRAB @(REG_BASE + 0x00000002UL);
#define DDRAB                           _DDRAB.Word
#define DDRAB_BIT0                      _DDRAB.Bits.BIT0
#define DDRAB_BIT1                      _DDRAB.Bits.BIT1
#define DDRAB_BIT2                      _DDRAB.Bits.BIT2
#define DDRAB_BIT3                      _DDRAB.Bits.BIT3
#define DDRAB_BIT4                      _DDRAB.Bits.BIT4
#define DDRAB_BIT5                      _DDRAB.Bits.BIT5
#define DDRAB_BIT6                      _DDRAB.Bits.BIT6
#define DDRAB_BIT7                      _DDRAB.Bits.BIT7
#define DDRAB_BIT8                      _DDRAB.Bits.BIT8
#define DDRAB_BIT9                      _DDRAB.Bits.BIT9
#define DDRAB_BIT10                     _DDRAB.Bits.BIT10
#define DDRAB_BIT11                     _DDRAB.Bits.BIT11
#define DDRAB_BIT12                     _DDRAB.Bits.BIT12
#define DDRAB_BIT13                     _DDRAB.Bits.BIT13
#define DDRAB_BIT14                     _DDRAB.Bits.BIT14
#define DDRAB_BIT15                     _DDRAB.Bits.BIT15

#define DDRAB_BIT0_MASK                 1U
#define DDRAB_BIT1_MASK                 2U
#define DDRAB_BIT2_MASK                 4U
#define DDRAB_BIT3_MASK                 8U
#define DDRAB_BIT4_MASK                 16U
#define DDRAB_BIT5_MASK                 32U
#define DDRAB_BIT6_MASK                 64U
#define DDRAB_BIT7_MASK                 128U
#define DDRAB_BIT8_MASK                 256U
#define DDRAB_BIT9_MASK                 512U
#define DDRAB_BIT10_MASK                1024U
#define DDRAB_BIT11_MASK                2048U
#define DDRAB_BIT12_MASK                4096U
#define DDRAB_BIT13_MASK                8192U
#define DDRAB_BIT14_MASK                16384U
#define DDRAB_BIT15_MASK                32768U


/*** PORTE - Port E Register; 0x00000008 ***/
typedef union {
  byte Byte;
  struct {
    byte BIT0        :1;                                       /* Port E Bit 0 */
    byte BIT1        :1;                                       /* Port E Bit 1 */
    byte BIT2        :1;                                       /* Port E Bit 2 */
    byte BIT3        :1;                                       /* Port E Bit 3 */
    byte BIT4        :1;                                       /* Port E Bit 4 */
    byte BIT5        :1;                                       /* Port E Bit 5 */
    byte BIT6        :1;                                       /* Port E Bit 6 */
    byte BIT7        :1;                                       /* Port E Bit 7 */
  } Bits;
} PORTESTR;
extern volatile PORTESTR _PORTE @(REG_BASE + 0x00000008UL);
#define PORTE                           _PORTE.Byte
#define PORTE_BIT0                      _PORTE.Bits.BIT0
#define PORTE_BIT1                      _PORTE.Bits.BIT1
#define PORTE_BIT2                      _PORTE.Bits.BIT2
#define PORTE_BIT3                      _PORTE.Bits.BIT3
#define PORTE_BIT4                      _PORTE.Bits.BIT4
#define PORTE_BIT5                      _PORTE.Bits.BIT5
#define PORTE_BIT6                      _PORTE.Bits.BIT6
#define PORTE_BIT7                      _PORTE.Bits.BIT7

#define PORTE_BIT0_MASK                 1U
#define PORTE_BIT1_MASK                 2U
#define PORTE_BIT2_MASK                 4U
#define PORTE_BIT3_MASK                 8U
#define PORTE_BIT4_MASK                 16U
#define PORTE_BIT5_MASK                 32U
#define PORTE_BIT6_MASK                 64U
#define PORTE_BIT7_MASK                 128U


/*** DDRE - Port E Data Direction Register; 0x00000009 ***/
typedef union {
  byte Byte;
  struct {
    byte             :1; 
    byte             :1; 
    byte BIT2        :1;                                       /* Data Direction Port E Bit 2 */
    byte BIT3        :1;                                       /* Data Direction Port E Bit 3 */
    byte BIT4        :1;                                       /* Data Direction Port E Bit 4 */
    byte BIT5        :1;                                       /* Data Direction Port E Bit 5 */
    byte BIT6        :1;                                       /* Data Direction Port E Bit 6 */
    byte BIT7        :1;                                       /* Data Direction Port E Bit 7 */
  } Bits;
  struct {
    byte         :1;
    byte         :1;
    byte grpBIT_2 :6;
  } MergedBits;
} DDRESTR;
extern volatile DDRESTR _DDRE @(REG_BASE + 0x00000009UL);
#define DDRE                            _DDRE.Byte
#define DDRE_BIT2                       _DDRE.Bits.BIT2
#define DDRE_BIT3                       _DDRE.Bits.BIT3
#define DDRE_BIT4                       _DDRE.Bits.BIT4
#define DDRE_BIT5                       _DDRE.Bits.BIT5
#define DDRE_BIT6                       _DDRE.Bits.BIT6
#define DDRE_BIT7                       _DDRE.Bits.BIT7
#define DDRE_BIT_2                      _DDRE.MergedBits.grpBIT_2
#define DDRE_BIT                        DDRE_BIT_2

#define DDRE_BIT2_MASK                  4U
#define DDRE_BIT3_MASK                  8U
#define DDRE_BIT4_MASK                  16U
#define DDRE_BIT5_MASK                  32U
#define DDRE_BIT6_MASK                  64U
#define DDRE_BIT7_MASK                  128U
#define DDRE_BIT_2_MASK                 252U
#define DDRE_BIT_2_BITNUM               2U


/*** PEAR - Port E Assignment Register; 0x0000000A ***/
typedef union {
  byte Byte;
  struct {
    byte             :1; 
    byte             :1; 
    byte RDWE        :1;                                       /* Read / Write Enable */
    byte LSTRE       :1;                                       /* Low Strobe (LSTRB) Enable */
    byte NECLK       :1;                                       /* No External E Clock */
    byte PIPOE       :1;                                       /* Pipe Status Signal Output Enable */
    byte             :1; 
    byte NOACCE      :1;                                       /* CPU No Access Output Enable */
  } Bits;
} PEARSTR;
extern volatile PEARSTR _PEAR @(REG_BASE + 0x0000000AUL);
#define PEAR                            _PEAR.Byte
#define PEAR_RDWE                       _PEAR.Bits.RDWE
#define PEAR_LSTRE                      _PEAR.Bits.LSTRE
#define PEAR_NECLK                      _PEAR.Bits.NECLK
#define PEAR_PIPOE                      _PEAR.Bits.PIPOE
#define PEAR_NOACCE                     _PEAR.Bits.NOACCE

#define PEAR_RDWE_MASK                  4U
#define PEAR_LSTRE_MASK                 8U
#define PEAR_NECLK_MASK                 16U
#define PEAR_PIPOE_MASK                 32U
#define PEAR_NOACCE_MASK                128U


/*** MODE - Mode Register; 0x0000000B ***/
typedef union {
  byte Byte;
  struct {
    byte EME         :1;                                       /* Emulate Port E */
    byte EMK         :1;                                       /* Emulate Port K */
    byte             :1; 
    byte IVIS        :1;                                       /* Internal Visibility */
    byte             :1; 
    byte MODA        :1;                                       /* Mode Select Bit A */
    byte MODB        :1;                                       /* Mode Select Bit B */
    byte MODC        :1;                                       /* Mode Select Bit C */
  } Bits;
  struct {
    byte         :1;
    byte         :1;
    byte         :1;
    byte         :1;
    byte         :1;
    byte grpMODx :3;
  } MergedBits;
} MODESTR;
extern volatile MODESTR _MODE @(REG_BASE + 0x0000000BUL);
#define MODE                            _MODE.Byte
#define MODE_EME                        _MODE.Bits.EME
#define MODE_EMK                        _MODE.Bits.EMK
#define MODE_IVIS                       _MODE.Bits.IVIS
#define MODE_MODA                       _MODE.Bits.MODA
#define MODE_MODB                       _MODE.Bits.MODB
#define MODE_MODC                       _MODE.Bits.MODC
#define MODE_MODx                       _MODE.MergedBits.grpMODx

#define MODE_EME_MASK                   1U
#define MODE_EMK_MASK                   2U
#define MODE_IVIS_MASK                  8U
#define MODE_MODA_MASK                  32U
#define MODE_MODB_MASK                  64U
#define MODE_MODC_MASK                  128U
#define MODE_MODx_MASK                  224U
#define MODE_MODx_BITNUM                5U


/*** PUCR - Pull-Up Control Register; 0x0000000C ***/
typedef union {
  byte Byte;
  struct {
    byte PUPAE       :1;                                       /* Pull-Up Port A Enable */
    byte PUPBE       :1;                                       /* Pull-Up Port B Enable */
    byte             :1; 
    byte             :1; 
    byte PUPEE       :1;                                       /* Pull-Up Port E Enable */
    byte             :1; 
    byte             :1; 
    byte PUPKE       :1;                                       /* Pull-Up Port K Enable */
  } Bits;
} PUCRSTR;
extern volatile PUCRSTR _PUCR @(REG_BASE + 0x0000000CUL);
#define PUCR                            _PUCR.Byte
#define PUCR_PUPAE                      _PUCR.Bits.PUPAE
#define PUCR_PUPBE                      _PUCR.Bits.PUPBE
#define PUCR_PUPEE                      _PUCR.Bits.PUPEE
#define PUCR_PUPKE                      _PUCR.Bits.PUPKE

#define PUCR_PUPAE_MASK                 1U
#define PUCR_PUPBE_MASK                 2U
#define PUCR_PUPEE_MASK                 16U
#define PUCR_PUPKE_MASK                 128U


/*** RDRIV - Reduced Drive of I/O Lines; 0x0000000D ***/
typedef union {
  byte Byte;
  struct {
    byte RDPA        :1;                                       /* Reduced Drive of Port A */
    byte RDPB        :1;                                       /* Reduced Drive of Port B */
    byte             :1; 
    byte             :1; 
    byte RDPE        :1;                                       /* Reduced Drive of Port E */
    byte             :1; 
    byte             :1; 
    byte RDPK        :1;                                       /* Reduced Drive of Port K */
  } Bits;
  struct {
    byte grpRDPx :2;
    byte         :1;
    byte         :1;
    byte         :1;
    byte         :1;
    byte         :1;
    byte         :1;
  } MergedBits;
} RDRIVSTR;
extern volatile RDRIVSTR _RDRIV @(REG_BASE + 0x0000000DUL);
#define RDRIV                           _RDRIV.Byte
#define RDRIV_RDPA                      _RDRIV.Bits.RDPA
#define RDRIV_RDPB                      _RDRIV.Bits.RDPB
#define RDRIV_RDPE                      _RDRIV.Bits.RDPE
#define RDRIV_RDPK                      _RDRIV.Bits.RDPK
#define RDRIV_RDPx                      _RDRIV.MergedBits.grpRDPx

#define RDRIV_RDPA_MASK                 1U
#define RDRIV_RDPB_MASK                 2U
#define RDRIV_RDPE_MASK                 16U
#define RDRIV_RDPK_MASK                 128U
#define RDRIV_RDPx_MASK                 3U
#define RDRIV_RDPx_BITNUM               0U


/*** EBICTL - External Bus Interface Control; 0x0000000E ***/
typedef union {
  byte Byte;
  struct {
    byte ESTR        :1;                                       /* E Stretches */
    byte             :1; 
    byte             :1; 
    byte             :1; 
    byte             :1; 
    byte             :1; 
    byte             :1; 
    byte             :1; 
  } Bits;
} EBICTLSTR;
extern volatile EBICTLSTR _EBICTL @(REG_BASE + 0x0000000EUL);
#define EBICTL                          _EBICTL.Byte
#define EBICTL_ESTR                     _EBICTL.Bits.ESTR

#define EBICTL_ESTR_MASK                1U


/*** INITRM - Initialization of Internal RAM Position Register; 0x00000010 ***/
typedef union {
  byte Byte;
  struct {
    byte RAMHAL      :1;                                       /* Internal RAM map alignment */
    byte             :1; 
    byte             :1; 
    byte RAM11       :1;                                       /* Internal RAM map position Bit 11 */
    byte RAM12       :1;                                       /* Internal RAM map position Bit 12 */
    byte RAM13       :1;                                       /* Internal RAM map position Bit 13 */
    byte RAM14       :1;                                       /* Internal RAM map position Bit 14 */
    byte RAM15       :1;                                       /* Internal RAM map position Bit 15 */
  } Bits;
  struct {
    byte         :1;
    byte         :1;
    byte         :1;
    byte grpRAM_11 :5;
  } MergedBits;
} INITRMSTR;
extern volatile INITRMSTR _INITRM @(REG_BASE + 0x00000010UL);
#define INITRM                          _INITRM.Byte
#define INITRM_RAMHAL                   _INITRM.Bits.RAMHAL
#define INITRM_RAM11                    _INITRM.Bits.RAM11
#define INITRM_RAM12                    _INITRM.Bits.RAM12
#define INITRM_RAM13                    _INITRM.Bits.RAM13
#define INITRM_RAM14                    _INITRM.Bits.RAM14
#define INITRM_RAM15                    _INITRM.Bits.RAM15
#define INITRM_RAM_11                   _INITRM.MergedBits.grpRAM_11
#define INITRM_RAM                      INITRM_RAM_11

#define INITRM_RAMHAL_MASK              1U
#define INITRM_RAM11_MASK               8U
#define INITRM_RAM12_MASK               16U
#define INITRM_RAM13_MASK               32U
#define INITRM_RAM14_MASK               64U
#define INITRM_RAM15_MASK               128U
#define INITRM_RAM_11_MASK              248U
#define INITRM_RAM_11_BITNUM            3U


/*** INITRG - Initialization of Internal Registers Position Register; 0x00000011 ***/
typedef union {
  byte Byte;
  struct {
    byte             :1; 
    byte             :1; 
    byte             :1; 
    byte REG11       :1;                                       /* Internal Registers Map Position Bit 11 */
    byte REG12       :1;                                       /* Internal Registers Map Position Bit 12 */
    byte REG13       :1;                                       /* Internal Registers Map Position Bit 13 */
    byte REG14       :1;                                       /* Internal Registers Map Position Bit 14 */
    byte             :1; 
  } Bits;
  struct {
    byte         :1;
    byte         :1;
    byte         :1;
    byte grpREG_11 :4;
    byte         :1;
  } MergedBits;
} INITRGSTR;
extern volatile INITRGSTR _INITRG @(REG_BASE + 0x00000011UL);
#define INITRG                          _INITRG.Byte
#define INITRG_REG11                    _INITRG.Bits.REG11
#define INITRG_REG12                    _INITRG.Bits.REG12
#define INITRG_REG13                    _INITRG.Bits.REG13
#define INITRG_REG14                    _INITRG.Bits.REG14
#define INITRG_REG_11                   _INITRG.MergedBits.grpREG_11
#define INITRG_REG                      INITRG_REG_11

#define INITRG_REG11_MASK               8U
#define INITRG_REG12_MASK               16U
#define INITRG_REG13_MASK               32U
#define INITRG_REG14_MASK               64U
#define INITRG_REG_11_MASK              120U
#define INITRG_REG_11_BITNUM            3U


/*** INITEE - Initialization of Internal EEPROM Position Register; 0x00000012 ***/
typedef union {
  byte Byte;
  struct {
    byte EEON        :1;                                       /* Internal EEPROM On */
    byte             :1; 
    byte             :1; 
    byte             :1; 
    byte EE12        :1;                                       /* Internal EEPROM map position Bit 12 */
    byte EE13        :1;                                       /* Internal EEPROM map position Bit 13 */
    byte EE14        :1;                                       /* Internal EEPROM map position Bit 14 */
    byte EE15        :1;                                       /* Internal EEPROM map position Bit 15 */
  } Bits;
  struct {
    byte         :1;
    byte         :1;
    byte         :1;
    byte         :1;
    byte grpEE_12 :4;
  } MergedBits;
} INITEESTR;
extern volatile INITEESTR _INITEE @(REG_BASE + 0x00000012UL);
#define INITEE                          _INITEE.Byte
#define INITEE_EEON                     _INITEE.Bits.EEON
#define INITEE_EE12                     _INITEE.Bits.EE12
#define INITEE_EE13                     _INITEE.Bits.EE13
#define INITEE_EE14                     _INITEE.Bits.EE14
#define INITEE_EE15                     _INITEE.Bits.EE15
#define INITEE_EE_12                    _INITEE.MergedBits.grpEE_12
#define INITEE_EE                       INITEE_EE_12

#define INITEE_EEON_MASK                1U
#define INITEE_EE12_MASK                16U
#define INITEE_EE13_MASK                32U
#define INITEE_EE14_MASK                64U
#define INITEE_EE15_MASK                128U
#define INITEE_EE_12_MASK               240U
#define INITEE_EE_12_BITNUM             4U


/*** MISC - Miscellaneous System Control Register; 0x00000013 ***/
typedef union {
  byte Byte;
  struct {
    byte ROMON       :1;                                       /* Enable Flash EEPROM */
    byte ROMHM       :1;                                       /* Flash EEPROM only in second half of memory map */
    byte EXSTR0      :1;                                       /* External Access Stretch Bit 0 */
    byte EXSTR1      :1;                                       /* External Access Stretch Bit 1 */
    byte             :1; 
    byte             :1; 
    byte             :1; 
    byte             :1; 
  } Bits;
  struct {
    byte         :1;
    byte         :1;
    byte grpEXSTR :2;
    byte         :1;
    byte         :1;
    byte         :1;
    byte         :1;
  } MergedBits;
} MISCSTR;
extern volatile MISCSTR _MISC @(REG_BASE + 0x00000013UL);
#define MISC                            _MISC.Byte
#define MISC_ROMON                      _MISC.Bits.ROMON
#define MISC_ROMHM                      _MISC.Bits.ROMHM
#define MISC_EXSTR0                     _MISC.Bits.EXSTR0
#define MISC_EXSTR1                     _MISC.Bits.EXSTR1
#define MISC_EXSTR                      _MISC.MergedBits.grpEXSTR

#define MISC_ROMON_MASK                 1U
#define MISC_ROMHM_MASK                 2U
#define MISC_EXSTR0_MASK                4U
#define MISC_EXSTR1_MASK                8U
#define MISC_EXSTR_MASK                 12U
#define MISC_EXSTR_BITNUM               2U


/*** ITCR - Interrupt Test Control Register; 0x00000015 ***/
typedef union {
  byte Byte;
  struct {
    byte ADR0        :1;                                       /* Test register select Bit 0 */
    byte ADR1        :1;                                       /* Test register select Bit 1 */
    byte ADR2        :1;                                       /* Test register select Bit 2 */
    byte ADR3        :1;                                       /* Test register select Bit 3 */
    byte WRTINT      :1;                                       /* Write to the Interrupt Test Registers */
    byte             :1; 
    byte             :1; 
    byte             :1; 
  } Bits;
  struct {
    byte grpADR  :4;
    byte         :1;
    byte         :1;
    byte         :1;
    byte         :1;
  } MergedBits;
} ITCRSTR;
extern volatile ITCRSTR _ITCR @(REG_BASE + 0x00000015UL);
#define ITCR                            _ITCR.Byte
#define ITCR_ADR0                       _ITCR.Bits.ADR0
#define ITCR_ADR1                       _ITCR.Bits.ADR1
#define ITCR_ADR2                       _ITCR.Bits.ADR2
#define ITCR_ADR3                       _ITCR.Bits.ADR3
#define ITCR_WRTINT                     _ITCR.Bits.WRTINT
#define ITCR_ADR                        _ITCR.MergedBits.grpADR

#define ITCR_ADR0_MASK                  1U
#define ITCR_ADR1_MASK                  2U
#define ITCR_ADR2_MASK                  4U
#define ITCR_ADR3_MASK                  8U
#define ITCR_WRTINT_MASK                16U
#define ITCR_ADR_MASK                   15U
#define ITCR_ADR_BITNUM                 0U


/*** ITEST - Interrupt Test Register; 0x00000016 ***/
typedef union {
  byte Byte;
  struct {
    byte INT0        :1;                                       /* Interrupt Test Register Bit 0 */
    byte INT2        :1;                                       /* Interrupt Test Register Bit 1 */
    byte INT4        :1;                                       /* Interrupt Test Register Bit 2 */
    byte INT6        :1;                                       /* Interrupt Test Register Bit 3 */
    byte INT8        :1;                                       /* Interrupt Test Register Bit 4 */
    byte INTA        :1;                                       /* Interrupt Test Register Bit 5 */
    byte INTC        :1;                                       /* Interrupt Test Register Bit 6 */
    byte INTE        :1;                                       /* Interrupt Test Register Bit 7 */
  } Bits;
} ITESTSTR;
extern volatile ITESTSTR _ITEST @(REG_BASE + 0x00000016UL);
#define ITEST                           _ITEST.Byte
#define ITEST_INT0                      _ITEST.Bits.INT0
#define ITEST_INT2                      _ITEST.Bits.INT2
#define ITEST_INT4                      _ITEST.Bits.INT4
#define ITEST_INT6                      _ITEST.Bits.INT6
#define ITEST_INT8                      _ITEST.Bits.INT8
#define ITEST_INTA                      _ITEST.Bits.INTA
#define ITEST_INTC                      _ITEST.Bits.INTC
#define ITEST_INTE                      _ITEST.Bits.INTE

#define ITEST_INT0_MASK                 1U
#define ITEST_INT2_MASK                 2U
#define ITEST_INT4_MASK                 4U
#define ITEST_INT6_MASK                 8U
#define ITEST_INT8_MASK                 16U
#define ITEST_INTA_MASK                 32U
#define ITEST_INTC_MASK                 64U
#define ITEST_INTE_MASK                 128U


/*** PARTID - Part ID Register; 0x0000001A ***/
typedef union {
  word Word;
   /* Overlapped registers: */
  struct {
    /*** PARTIDH - Part ID Register High; 0x0000001A ***/
    union {
      byte Byte;
      struct {
        byte ID8         :1;                                       /* Part ID Register Bit 8 */
        byte ID9         :1;                                       /* Part ID Register Bit 9 */
        byte ID10        :1;                                       /* Part ID Register Bit 10 */
        byte ID11        :1;                                       /* Part ID Register Bit 11 */
        byte ID12        :1;                                       /* Part ID Register Bit 12 */
        byte ID13        :1;                                       /* Part ID Register Bit 13 */
        byte ID14        :1;                                       /* Part ID Register Bit 14 */
        byte ID15        :1;                                       /* Part ID Register Bit 15 */
      } Bits;
    } PARTIDHSTR;
    #define PARTIDH                     _PARTID.Overlap_STR.PARTIDHSTR.Byte
    #define PARTIDH_ID8                 _PARTID.Overlap_STR.PARTIDHSTR.Bits.ID8
    #define PARTIDH_ID9                 _PARTID.Overlap_STR.PARTIDHSTR.Bits.ID9
    #define PARTIDH_ID10                _PARTID.Overlap_STR.PARTIDHSTR.Bits.ID10
    #define PARTIDH_ID11                _PARTID.Overlap_STR.PARTIDHSTR.Bits.ID11
    #define PARTIDH_ID12                _PARTID.Overlap_STR.PARTIDHSTR.Bits.ID12
    #define PARTIDH_ID13                _PARTID.Overlap_STR.PARTIDHSTR.Bits.ID13
    #define PARTIDH_ID14                _PARTID.Overlap_STR.PARTIDHSTR.Bits.ID14
    #define PARTIDH_ID15                _PARTID.Overlap_STR.PARTIDHSTR.Bits.ID15
    
    #define PARTIDH_ID8_MASK            1U
    #define PARTIDH_ID9_MASK            2U
    #define PARTIDH_ID10_MASK           4U
    #define PARTIDH_ID11_MASK           8U
    #define PARTIDH_ID12_MASK           16U
    #define PARTIDH_ID13_MASK           32U
    #define PARTIDH_ID14_MASK           64U
    #define PARTIDH_ID15_MASK           128U
    

    /*** PARTIDL - Part ID Register Low; 0x0000001B ***/
    union {
      byte Byte;
      struct {
        byte ID0         :1;                                       /* Part ID Register Bit 0 */
        byte ID1         :1;                                       /* Part ID Register Bit 1 */
        byte ID2         :1;                                       /* Part ID Register Bit 2 */
        byte ID3         :1;                                       /* Part ID Register Bit 3 */
        byte ID4         :1;                                       /* Part ID Register Bit 4 */
        byte ID5         :1;                                       /* Part ID Register Bit 5 */
        byte ID6         :1;                                       /* Part ID Register Bit 6 */
        byte ID7         :1;                                       /* Part ID Register Bit 7 */
      } Bits;
    } PARTIDLSTR;
    #define PARTIDL                     _PARTID.Overlap_STR.PARTIDLSTR.Byte
    #define PARTIDL_ID0                 _PARTID.Overlap_STR.PARTIDLSTR.Bits.ID0
    #define PARTIDL_ID1                 _PARTID.Overlap_STR.PARTIDLSTR.Bits.ID1
    #define PARTIDL_ID2                 _PARTID.Overlap_STR.PARTIDLSTR.Bits.ID2
    #define PARTIDL_ID3                 _PARTID.Overlap_STR.PARTIDLSTR.Bits.ID3
    #define PARTIDL_ID4                 _PARTID.Overlap_STR.PARTIDLSTR.Bits.ID4
    #define PARTIDL_ID5                 _PARTID.Overlap_STR.PARTIDLSTR.Bits.ID5
    #define PARTIDL_ID6                 _PARTID.Overlap_STR.PARTIDLSTR.Bits.ID6
    #define PARTIDL_ID7                 _PARTID.Overlap_STR.PARTIDLSTR.Bits.ID7
    
    #define PARTIDL_ID0_MASK            1U
    #define PARTIDL_ID1_MASK            2U
    #define PARTIDL_ID2_MASK            4U
    #define PARTIDL_ID3_MASK            8U
    #define PARTIDL_ID4_MASK            16U
    #define PARTIDL_ID5_MASK            32U
    #define PARTIDL_ID6_MASK            64U
    #define PARTIDL_ID7_MASK            128U
    
  } Overlap_STR;

  struct {
    word ID0         :1;                                       /* Part ID Register Bit 0 */
    word ID1         :1;                                       /* Part ID Register Bit 1 */
    word ID2         :1;                                       /* Part ID Register Bit 2 */
    word ID3         :1;                                       /* Part ID Register Bit 3 */
    word ID4         :1;                                       /* Part ID Register Bit 4 */
    word ID5         :1;                                       /* Part ID Register Bit 5 */
    word ID6         :1;                                       /* Part ID Register Bit 6 */
    word ID7         :1;                                       /* Part ID Register Bit 7 */
    word ID8         :1;                                       /* Part ID Register Bit 8 */
    word ID9         :1;                                       /* Part ID Register Bit 9 */
    word ID10        :1;                                       /* Part ID Register Bit 10 */
    word ID11        :1;                                       /* Part ID Register Bit 11 */
    word ID12        :1;                                       /* Part ID Register Bit 12 */
    word ID13        :1;                                       /* Part ID Register Bit 13 */
    word ID14        :1;                                       /* Part ID Register Bit 14 */
    word ID15        :1;                                       /* Part ID Register Bit 15 */
  } Bits;
} PARTIDSTR;
extern volatile PARTIDSTR _PARTID @(REG_BASE + 0x0000001AUL);
#define PARTID                          _PARTID.Word
#define PARTID_ID0                      _PARTID.Bits.ID0
#define PARTID_ID1                      _PARTID.Bits.ID1
#define PARTID_ID2                      _PARTID.Bits.ID2
#define PARTID_ID3                      _PARTID.Bits.ID3
#define PARTID_ID4                      _PARTID.Bits.ID4
#define PARTID_ID5                      _PARTID.Bits.ID5
#define PARTID_ID6                      _PARTID.Bits.ID6
#define PARTID_ID7                      _PARTID.Bits.ID7
#define PARTID_ID8                      _PARTID.Bits.ID8
#define PARTID_ID9                      _PARTID.Bits.ID9
#define PARTID_ID10                     _PARTID.Bits.ID10
#define PARTID_ID11                     _PARTID.Bits.ID11
#define PARTID_ID12                     _PARTID.Bits.ID12
#define PARTID_ID13                     _PARTID.Bits.ID13
#define PARTID_ID14                     _PARTID.Bits.ID14
#define PARTID_ID15                     _PARTID.Bits.ID15

#define PARTID_ID0_MASK                 1U
#define PARTID_ID1_MASK                 2U
#define PARTID_ID2_MASK                 4U
#define PARTID_ID3_MASK                 8U
#define PARTID_ID4_MASK                 16U
#define PARTID_ID5_MASK                 32U
#define PARTID_ID6_MASK                 64U
#define PARTID_ID7_MASK                 128U
#define PARTID_ID8_MASK                 256U
#define PARTID_ID9_MASK                 512U
#define PARTID_ID10_MASK                1024U
#define PARTID_ID11_MASK                2048U
#define PARTID_ID12_MASK                4096U
#define PARTID_ID13_MASK                8192U
#define PARTID_ID14_MASK                16384U
#define PARTID_ID15_MASK                32768U


/*** MEMSIZ0 - Memory Size Register Zero; 0x0000001C ***/
typedef union {
  byte Byte;
  struct {
    byte ram_sw0     :1;                                       /* Allocated System RAM Memory Space Bit 0 */
    byte ram_sw1     :1;                                       /* Allocated System RAM Memory Space Bit 1 */
    byte ram_sw2     :1;                                       /* Allocated System RAM Memory Space Bit 2 */
    byte             :1; 
    byte eep_sw0     :1;                                       /* Allocated EEPROM Memory Space Bit 0 */
    byte eep_sw1     :1;                                       /* Allocated EEPROM Memory Space Bit 1 */
    byte             :1; 
    byte reg_sw0     :1;                                       /* Allocated System Register Space */
  } Bits;
  struct {
    byte grpram_sw :3;
    byte         :1;
    byte grpeep_sw :2;
    byte         :1;
    byte grpreg_sw :1;
  } MergedBits;
} MEMSIZ0STR;
extern volatile MEMSIZ0STR _MEMSIZ0 @(REG_BASE + 0x0000001CUL);
#define MEMSIZ0                         _MEMSIZ0.Byte
#define MEMSIZ0_ram_sw0                 _MEMSIZ0.Bits.ram_sw0
#define MEMSIZ0_ram_sw1                 _MEMSIZ0.Bits.ram_sw1
#define MEMSIZ0_ram_sw2                 _MEMSIZ0.Bits.ram_sw2
#define MEMSIZ0_eep_sw0                 _MEMSIZ0.Bits.eep_sw0
#define MEMSIZ0_eep_sw1                 _MEMSIZ0.Bits.eep_sw1
#define MEMSIZ0_reg_sw0                 _MEMSIZ0.Bits.reg_sw0
/* MEMSIZ_ARR: Access 2 MEMSIZx registers in an array */
#define MEMSIZ_ARR                      ((volatile byte *) &MEMSIZ0)
#define MEMSIZ0_ram_sw                  _MEMSIZ0.MergedBits.grpram_sw
#define MEMSIZ0_eep_sw                  _MEMSIZ0.MergedBits.grpeep_sw

#define MEMSIZ0_ram_sw0_MASK            1U
#define MEMSIZ0_ram_sw1_MASK            2U
#define MEMSIZ0_ram_sw2_MASK            4U
#define MEMSIZ0_eep_sw0_MASK            16U
#define MEMSIZ0_eep_sw1_MASK            32U
#define MEMSIZ0_reg_sw0_MASK            128U
#define MEMSIZ0_ram_sw_MASK             7U
#define MEMSIZ0_ram_sw_BITNUM           0U
#define MEMSIZ0_eep_sw_MASK             48U
#define MEMSIZ0_eep_sw_BITNUM           4U


/*** MEMSIZ1 - Memory Size Register One; 0x0000001D ***/
typedef union {
  byte Byte;
  struct {
    byte pag_sw0     :1;                                       /* Allocated Off-Chip Memory Options Bit 0 */
    byte pag_sw1     :1;                                       /* Allocated Off-Chip Memory Options Bit 1 */
    byte             :1; 
    byte             :1; 
    byte             :1; 
    byte             :1; 
    byte rom_sw0     :1;                                       /* Allocated Flash EEPROM/ROM Physical Memory Space Bit 0 */
    byte rom_sw1     :1;                                       /* Allocated Flash EEPROM/ROM Physical Memory Space Bit 1 */
  } Bits;
  struct {
    byte grppag_sw :2;
    byte         :1;
    byte         :1;
    byte         :1;
    byte         :1;
    byte grprom_sw :2;
  } MergedBits;
} MEMSIZ1STR;
extern volatile MEMSIZ1STR _MEMSIZ1 @(REG_BASE + 0x0000001DUL);
#define MEMSIZ1                         _MEMSIZ1.Byte
#define MEMSIZ1_pag_sw0                 _MEMSIZ1.Bits.pag_sw0
#define MEMSIZ1_pag_sw1                 _MEMSIZ1.Bits.pag_sw1
#define MEMSIZ1_rom_sw0                 _MEMSIZ1.Bits.rom_sw0
#define MEMSIZ1_rom_sw1                 _MEMSIZ1.Bits.rom_sw1
#define MEMSIZ1_pag_sw                  _MEMSIZ1.MergedBits.grppag_sw
#define MEMSIZ1_rom_sw                  _MEMSIZ1.MergedBits.grprom_sw

#define MEMSIZ1_pag_sw0_MASK            1U
#define MEMSIZ1_pag_sw1_MASK            2U
#define MEMSIZ1_rom_sw0_MASK            64U
#define MEMSIZ1_rom_sw1_MASK            128U
#define MEMSIZ1_pag_sw_MASK             3U
#define MEMSIZ1_pag_sw_BITNUM           0U
#define MEMSIZ1_rom_sw_MASK             192U
#define MEMSIZ1_rom_sw_BITNUM           6U


/*** INTCR - Interrupt Control Register; 0x0000001E ***/
typedef union {
  byte Byte;
  struct {
    byte             :1; 
    byte             :1; 
    byte             :1; 
    byte             :1; 
    byte             :1; 
    byte             :1; 
    byte IRQEN       :1;                                       /* External IRQ Enable */
    byte IRQE        :1;                                       /* IRQ Select Edge Sensitive Only */
  } Bits;
} INTCRSTR;
extern volatile INTCRSTR _INTCR @(REG_BASE + 0x0000001EUL);
#define INTCR                           _INTCR.Byte
#define INTCR_IRQEN                     _INTCR.Bits.IRQEN
#define INTCR_IRQE                      _INTCR.Bits.IRQE

#define INTCR_IRQEN_MASK                64U
#define INTCR_IRQE_MASK                 128U


/*** HPRIO - Highest Priority I Interrupt; 0x0000001F ***/
typedef union {
  byte Byte;
  struct {
    byte             :1; 
    byte PSEL1       :1;                                       /* Highest Priority I Interrupt Bit 1 */
    byte PSEL2       :1;                                       /* Highest Priority I Interrupt Bit 2 */
    byte PSEL3       :1;                                       /* Highest Priority I Interrupt Bit 3 */
    byte PSEL4       :1;                                       /* Highest Priority I Interrupt Bit 4 */
    byte PSEL5       :1;                                       /* Highest Priority I Interrupt Bit 5 */
    byte PSEL6       :1;                                       /* Highest Priority I Interrupt Bit 6 */
    byte PSEL7       :1;                                       /* Highest Priority I Interrupt Bit 7 */
  } Bits;
  struct {
    byte         :1;
    byte grpPSEL_1 :7;
  } MergedBits;
} HPRIOSTR;
extern volatile HPRIOSTR _HPRIO @(REG_BASE + 0x0000001FUL);
#define HPRIO                           _HPRIO.Byte
#define HPRIO_PSEL1                     _HPRIO.Bits.PSEL1
#define HPRIO_PSEL2                     _HPRIO.Bits.PSEL2
#define HPRIO_PSEL3                     _HPRIO.Bits.PSEL3
#define HPRIO_PSEL4                     _HPRIO.Bits.PSEL4
#define HPRIO_PSEL5                     _HPRIO.Bits.PSEL5
#define HPRIO_PSEL6                     _HPRIO.Bits.PSEL6
#define HPRIO_PSEL7                     _HPRIO.Bits.PSEL7
#define HPRIO_PSEL_1                    _HPRIO.MergedBits.grpPSEL_1
#define HPRIO_PSEL                      HPRIO_PSEL_1

#define HPRIO_PSEL1_MASK                2U
#define HPRIO_PSEL2_MASK                4U
#define HPRIO_PSEL3_MASK                8U
#define HPRIO_PSEL4_MASK                16U
#define HPRIO_PSEL5_MASK                32U
#define HPRIO_PSEL6_MASK                64U
#define HPRIO_PSEL7_MASK                128U
#define HPRIO_PSEL_1_MASK               254U
#define HPRIO_PSEL_1_BITNUM             1U


/*** BKPCT0 - Breakpoint Control Register 0; 0x00000028 ***/
typedef union {
  byte Byte;
  struct {
    byte             :1; 
    byte             :1; 
    byte             :1; 
    byte             :1; 
    byte BKTAG       :1;                                       /* Breakpoint on Tag */
    byte BKBDM       :1;                                       /* Breakpoint Background Debug Mode Enable */
    byte BKFULL      :1;                                       /* Full Breakpoint Mode Enable */
    byte BKEN        :1;                                       /* Breakpoint Enable */
  } Bits;
} BKPCT0STR;
extern volatile BKPCT0STR _BKPCT0 @(REG_BASE + 0x00000028UL);
#define BKPCT0                          _BKPCT0.Byte
#define BKPCT0_BKTAG                    _BKPCT0.Bits.BKTAG
#define BKPCT0_BKBDM                    _BKPCT0.Bits.BKBDM
#define BKPCT0_BKFULL                   _BKPCT0.Bits.BKFULL
#define BKPCT0_BKEN                     _BKPCT0.Bits.BKEN
/* BKPCT_ARR: Access 2 BKPCTx registers in an array */
#define BKPCT_ARR                       ((volatile byte *) &BKPCT0)

#define BKPCT0_BKTAG_MASK               16U
#define BKPCT0_BKBDM_MASK               32U
#define BKPCT0_BKFULL_MASK              64U
#define BKPCT0_BKEN_MASK                128U


/*** BKPCT1 - Breakpoint Control Register 1; 0x00000029 ***/
typedef union {
  byte Byte;
  struct {
    byte BK1RW       :1;                                       /* R/W Compare Value 1 */
    byte BK1RWE      :1;                                       /* R/W Compare Enable 1 */
    byte BK0RW       :1;                                       /* R/W Compare Value 0 */
    byte BK0RWE      :1;                                       /* R/W Compare Enable 0 */
    byte BK1MBL      :1;                                       /* Breakpoint Mask Low Byte for Second Address */
    byte BK1MBH      :1;                                       /* Breakpoint Mask High Byte for Second Address */
    byte BK0MBL      :1;                                       /* Breakpoint Mask Low Byte for First Address */
    byte BK0MBH      :1;                                       /* Breakpoint Mask High Byte for First Address */
  } Bits;
} BKPCT1STR;
extern volatile BKPCT1STR _BKPCT1 @(REG_BASE + 0x00000029UL);
#define BKPCT1                          _BKPCT1.Byte
#define BKPCT1_BK1RW                    _BKPCT1.Bits.BK1RW
#define BKPCT1_BK1RWE                   _BKPCT1.Bits.BK1RWE
#define BKPCT1_BK0RW                    _BKPCT1.Bits.BK0RW
#define BKPCT1_BK0RWE                   _BKPCT1.Bits.BK0RWE
#define BKPCT1_BK1MBL                   _BKPCT1.Bits.BK1MBL
#define BKPCT1_BK1MBH                   _BKPCT1.Bits.BK1MBH
#define BKPCT1_BK0MBL                   _BKPCT1.Bits.BK0MBL
#define BKPCT1_BK0MBH                   _BKPCT1.Bits.BK0MBH

#define BKPCT1_BK1RW_MASK               1U
#define BKPCT1_BK1RWE_MASK              2U
#define BKPCT1_BK0RW_MASK               4U
#define BKPCT1_BK0RWE_MASK              8U
#define BKPCT1_BK1MBL_MASK              16U
#define BKPCT1_BK1MBH_MASK              32U
#define BKPCT1_BK0MBL_MASK              64U
#define BKPCT1_BK0MBH_MASK              128U


/*** BKP0X - First Address Memory Expansion Breakpoint Register; 0x0000002A ***/
typedef union {
  byte Byte;
  struct {
    byte BK0V0       :1;                                       /* First Address Breakpoint Expansion Address Value Bit 0 */
    byte BK0V1       :1;                                       /* First Address Breakpoint Expansion Address Value Bit 1 */
    byte BK0V2       :1;                                       /* First Address Breakpoint Expansion Address Value Bit 2 */
    byte BK0V3       :1;                                       /* First Address Breakpoint Expansion Address Value Bit 3 */
    byte BK0V4       :1;                                       /* First Address Breakpoint Expansion Address Value Bit 4 */
    byte BK0V5       :1;                                       /* First Address Breakpoint Expansion Address Value Bit 5 */
    byte             :1; 
    byte             :1; 
  } Bits;
  struct {
    byte grpBK0V :6;
    byte         :1;
    byte         :1;
  } MergedBits;
} BKP0XSTR;
extern volatile BKP0XSTR _BKP0X @(REG_BASE + 0x0000002AUL);
#define BKP0X                           _BKP0X.Byte
#define BKP0X_BK0V0                     _BKP0X.Bits.BK0V0
#define BKP0X_BK0V1                     _BKP0X.Bits.BK0V1
#define BKP0X_BK0V2                     _BKP0X.Bits.BK0V2
#define BKP0X_BK0V3                     _BKP0X.Bits.BK0V3
#define BKP0X_BK0V4                     _BKP0X.Bits.BK0V4
#define BKP0X_BK0V5                     _BKP0X.Bits.BK0V5
#define BKP0X_BK0V                      _BKP0X.MergedBits.grpBK0V

#define BKP0X_BK0V0_MASK                1U
#define BKP0X_BK0V1_MASK                2U
#define BKP0X_BK0V2_MASK                4U
#define BKP0X_BK0V3_MASK                8U
#define BKP0X_BK0V4_MASK                16U
#define BKP0X_BK0V5_MASK                32U
#define BKP0X_BK0V_MASK                 63U
#define BKP0X_BK0V_BITNUM               0U


/*** BKP0H - First Address High Byte Breakpoint Register; 0x0000002B ***/
typedef union {
  byte Byte;
  struct {
    byte BIT8        :1;                                       /* First Address Breakpoint Register Bit 8 */
    byte BIT9        :1;                                       /* First Address Breakpoint Register Bit 9 */
    byte BIT10       :1;                                       /* First Address Breakpoint Register Bit 10 */
    byte BIT11       :1;                                       /* First Address Breakpoint Register Bit 11 */
    byte BIT12       :1;                                       /* First Address Breakpoint Register Bit 12 */
    byte BIT13       :1;                                       /* First Address Breakpoint Register Bit 13 */
    byte BIT14       :1;                                       /* First Address Breakpoint Register Bit 14 */
    byte BIT15       :1;                                       /* First Address Breakpoint Register Bit 15 */
  } Bits;
} BKP0HSTR;
extern volatile BKP0HSTR _BKP0H @(REG_BASE + 0x0000002BUL);
#define BKP0H                           _BKP0H.Byte
#define BKP0H_BIT8                      _BKP0H.Bits.BIT8
#define BKP0H_BIT9                      _BKP0H.Bits.BIT9
#define BKP0H_BIT10                     _BKP0H.Bits.BIT10
#define BKP0H_BIT11                     _BKP0H.Bits.BIT11
#define BKP0H_BIT12                     _BKP0H.Bits.BIT12
#define BKP0H_BIT13                     _BKP0H.Bits.BIT13
#define BKP0H_BIT14                     _BKP0H.Bits.BIT14
#define BKP0H_BIT15                     _BKP0H.Bits.BIT15

#define BKP0H_BIT8_MASK                 1U
#define BKP0H_BIT9_MASK                 2U
#define BKP0H_BIT10_MASK                4U
#define BKP0H_BIT11_MASK                8U
#define BKP0H_BIT12_MASK                16U
#define BKP0H_BIT13_MASK                32U
#define BKP0H_BIT14_MASK                64U
#define BKP0H_BIT15_MASK                128U


/*** BKP0L - First Address Low Byte Breakpoint Register; 0x0000002C ***/
typedef union {
  byte Byte;
  struct {
    byte BIT0        :1;                                       /* First Address Breakpoint Register Bit 0 */
    byte BIT1        :1;                                       /* First Address Breakpoint Register Bit 1 */
    byte BIT2        :1;                                       /* First Address Breakpoint Register Bit 2 */
    byte BIT3        :1;                                       /* First Address Breakpoint Register Bit 3 */
    byte BIT4        :1;                                       /* First Address Breakpoint Register Bit 4 */
    byte BIT5        :1;                                       /* First Address Breakpoint Register Bit 5 */
    byte BIT6        :1;                                       /* First Address Breakpoint Register Bit 6 */
    byte BIT7        :1;                                       /* First Address Breakpoint Register Bit 7 */
  } Bits;
} BKP0LSTR;
extern volatile BKP0LSTR _BKP0L @(REG_BASE + 0x0000002CUL);
#define BKP0L                           _BKP0L.Byte
#define BKP0L_BIT0                      _BKP0L.Bits.BIT0
#define BKP0L_BIT1                      _BKP0L.Bits.BIT1
#define BKP0L_BIT2                      _BKP0L.Bits.BIT2
#define BKP0L_BIT3                      _BKP0L.Bits.BIT3
#define BKP0L_BIT4                      _BKP0L.Bits.BIT4
#define BKP0L_BIT5                      _BKP0L.Bits.BIT5
#define BKP0L_BIT6                      _BKP0L.Bits.BIT6
#define BKP0L_BIT7                      _BKP0L.Bits.BIT7

#define BKP0L_BIT0_MASK                 1U
#define BKP0L_BIT1_MASK                 2U
#define BKP0L_BIT2_MASK                 4U
#define BKP0L_BIT3_MASK                 8U
#define BKP0L_BIT4_MASK                 16U
#define BKP0L_BIT5_MASK                 32U
#define BKP0L_BIT6_MASK                 64U
#define BKP0L_BIT7_MASK                 128U


/*** BKP1X - Second Address Memory Expansion Breakpoint Register; 0x0000002D ***/
typedef union {
  byte Byte;
  struct {
    byte BK1V0       :1;                                       /* Second Address Breakpoint Expansion Address Value Bit 0 */
    byte BK1V1       :1;                                       /* Second Address Breakpoint Expansion Address Value Bit 1 */
    byte BK1V2       :1;                                       /* Second Address Breakpoint Expansion Address Value Bit 2 */
    byte BK1V3       :1;                                       /* Second Address Breakpoint Expansion Address Value Bit 3 */
    byte BK1V4       :1;                                       /* Second Address Breakpoint Expansion Address Value Bit 4 */
    byte BK1V5       :1;                                       /* Second Address Breakpoint Expansion Address Value Bit 5 */
    byte             :1; 
    byte             :1; 
  } Bits;
  struct {
    byte grpBK1V :6;
    byte         :1;
    byte         :1;
  } MergedBits;
} BKP1XSTR;
extern volatile BKP1XSTR _BKP1X @(REG_BASE + 0x0000002DUL);
#define BKP1X                           _BKP1X.Byte
#define BKP1X_BK1V0                     _BKP1X.Bits.BK1V0
#define BKP1X_BK1V1                     _BKP1X.Bits.BK1V1
#define BKP1X_BK1V2                     _BKP1X.Bits.BK1V2
#define BKP1X_BK1V3                     _BKP1X.Bits.BK1V3
#define BKP1X_BK1V4                     _BKP1X.Bits.BK1V4
#define BKP1X_BK1V5                     _BKP1X.Bits.BK1V5
#define BKP1X_BK1V                      _BKP1X.MergedBits.grpBK1V

#define BKP1X_BK1V0_MASK                1U
#define BKP1X_BK1V1_MASK                2U
#define BKP1X_BK1V2_MASK                4U
#define BKP1X_BK1V3_MASK                8U
#define BKP1X_BK1V4_MASK                16U
#define BKP1X_BK1V5_MASK                32U
#define BKP1X_BK1V_MASK                 63U
#define BKP1X_BK1V_BITNUM               0U


/*** BKP1H - Data (Second Address) High Byte Breakpoint Register; 0x0000002E ***/
typedef union {
  byte Byte;
  struct {
    byte BIT8        :1;                                       /* Data (Second Address) Breakpoint Register Bit 8 */
    byte BIT9        :1;                                       /* Data (Second Address) Breakpoint Register Bit 9 */
    byte BIT10       :1;                                       /* Data (Second Address) Breakpoint Register Bit 10 */
    byte BIT11       :1;                                       /* Data (Second Address) Breakpoint Register Bit 11 */
    byte BIT12       :1;                                       /* Data (Second Address) Breakpoint Register Bit 12 */
    byte BIT13       :1;                                       /* Data (Second Address) Breakpoint Register Bit 13 */
    byte BIT14       :1;                                       /* Data (Second Address) Breakpoint Register Bit 14 */
    byte BIT15       :1;                                       /* Data (Second Address) Breakpoint Register Bit 15 */
  } Bits;
} BKP1HSTR;
extern volatile BKP1HSTR _BKP1H @(REG_BASE + 0x0000002EUL);
#define BKP1H                           _BKP1H.Byte
#define BKP1H_BIT8                      _BKP1H.Bits.BIT8
#define BKP1H_BIT9                      _BKP1H.Bits.BIT9
#define BKP1H_BIT10                     _BKP1H.Bits.BIT10
#define BKP1H_BIT11                     _BKP1H.Bits.BIT11
#define BKP1H_BIT12                     _BKP1H.Bits.BIT12
#define BKP1H_BIT13                     _BKP1H.Bits.BIT13
#define BKP1H_BIT14                     _BKP1H.Bits.BIT14
#define BKP1H_BIT15                     _BKP1H.Bits.BIT15

#define BKP1H_BIT8_MASK                 1U
#define BKP1H_BIT9_MASK                 2U
#define BKP1H_BIT10_MASK                4U
#define BKP1H_BIT11_MASK                8U
#define BKP1H_BIT12_MASK                16U
#define BKP1H_BIT13_MASK                32U
#define BKP1H_BIT14_MASK                64U
#define BKP1H_BIT15_MASK                128U


/*** BKP1L - Data (Second Address) Low Byte Breakpoint Register; 0x0000002F ***/
typedef union {
  byte Byte;
  struct {
    byte BIT0        :1;                                       /* Data (Second Address) Breakpoint Register Bit 0 */
    byte BIT1        :1;                                       /* Data (Second Address) Breakpoint Register Bit 1 */
    byte BIT2        :1;                                       /* Data (Second Address) Breakpoint Register Bit 2 */
    byte BIT3        :1;                                       /* Data (Second Address) Breakpoint Register Bit 3 */
    byte BIT4        :1;                                       /* Data (Second Address) Breakpoint Register Bit 4 */
    byte BIT5        :1;                                       /* Data (Second Address) Breakpoint Register Bit 5 */
    byte BIT6        :1;                                       /* Data (Second Address) Breakpoint Register Bit 6 */
    byte BIT7        :1;                                       /* Data (Second Address) Breakpoint Register Bit 7 */
  } Bits;
} BKP1LSTR;
extern volatile BKP1LSTR _BKP1L @(REG_BASE + 0x0000002FUL);
#define BKP1L                           _BKP1L.Byte
#define BKP1L_BIT0                      _BKP1L.Bits.BIT0
#define BKP1L_BIT1                      _BKP1L.Bits.BIT1
#define BKP1L_BIT2                      _BKP1L.Bits.BIT2
#define BKP1L_BIT3                      _BKP1L.Bits.BIT3
#define BKP1L_BIT4                      _BKP1L.Bits.BIT4
#define BKP1L_BIT5                      _BKP1L.Bits.BIT5
#define BKP1L_BIT6                      _BKP1L.Bits.BIT6
#define BKP1L_BIT7                      _BKP1L.Bits.BIT7

#define BKP1L_BIT0_MASK                 1U
#define BKP1L_BIT1_MASK                 2U
#define BKP1L_BIT2_MASK                 4U
#define BKP1L_BIT3_MASK                 8U
#define BKP1L_BIT4_MASK                 16U
#define BKP1L_BIT5_MASK                 32U
#define BKP1L_BIT6_MASK                 64U
#define BKP1L_BIT7_MASK                 128U


/*** PPAGE - Page Index Register; 0x00000030 ***/
typedef union {
  byte Byte;
  struct {
    byte PIX0        :1;                                       /* Page Index Register Bit 0 */
    byte PIX1        :1;                                       /* Page Index Register Bit 1 */
    byte PIX2        :1;                                       /* Page Index Register Bit 2 */
    byte PIX3        :1;                                       /* Page Index Register Bit 3 */
    byte PIX4        :1;                                       /* Page Index Register Bit 4 */
    byte PIX5        :1;                                       /* Page Index Register Bit 5 */
    byte             :1; 
    byte             :1; 
  } Bits;
  struct {
    byte grpPIX  :6;
    byte         :1;
    byte         :1;
  } MergedBits;
} PPAGESTR;
extern volatile PPAGESTR _PPAGE @(REG_BASE + 0x00000030UL);
#define PPAGE                           _PPAGE.Byte
#define PPAGE_PIX0                      _PPAGE.Bits.PIX0
#define PPAGE_PIX1                      _PPAGE.Bits.PIX1
#define PPAGE_PIX2                      _PPAGE.Bits.PIX2
#define PPAGE_PIX3                      _PPAGE.Bits.PIX3
#define PPAGE_PIX4                      _PPAGE.Bits.PIX4
#define PPAGE_PIX5                      _PPAGE.Bits.PIX5
#define PPAGE_PIX                       _PPAGE.MergedBits.grpPIX

#define PPAGE_PIX0_MASK                 1U
#define PPAGE_PIX1_MASK                 2U
#define PPAGE_PIX2_MASK                 4U
#define PPAGE_PIX3_MASK                 8U
#define PPAGE_PIX4_MASK                 16U
#define PPAGE_PIX5_MASK                 32U
#define PPAGE_PIX_MASK                  63U
#define PPAGE_PIX_BITNUM                0U


/*** PORTK - Port K Data Register; 0x00000032 ***/
typedef union {
  byte Byte;
  struct {
    byte BIT0        :1;                                       /* Port K Bit 0 */
    byte BIT1        :1;                                       /* Port K Bit 1 */
    byte BIT2        :1;                                       /* Port K Bit 2 */
    byte BIT3        :1;                                       /* Port K Bit 3 */
    byte BIT4        :1;                                       /* Port K Bit 4 */
    byte BIT5        :1;                                       /* Port K Bit 5 */
    byte             :1; 
    byte BIT7        :1;                                       /* Port K Bit 7 */
  } Bits;
  struct {
    byte grpBIT  :6;
    byte         :1;
    byte grpBIT_7 :1;
  } MergedBits;
} PORTKSTR;
extern volatile PORTKSTR _PORTK @(REG_BASE + 0x00000032UL);
#define PORTK                           _PORTK.Byte
#define PORTK_BIT0                      _PORTK.Bits.BIT0
#define PORTK_BIT1                      _PORTK.Bits.BIT1
#define PORTK_BIT2                      _PORTK.Bits.BIT2
#define PORTK_BIT3                      _PORTK.Bits.BIT3
#define PORTK_BIT4                      _PORTK.Bits.BIT4
#define PORTK_BIT5                      _PORTK.Bits.BIT5
#define PORTK_BIT7                      _PORTK.Bits.BIT7
#define PORTK_BIT                       _PORTK.MergedBits.grpBIT

#define PORTK_BIT0_MASK                 1U
#define PORTK_BIT1_MASK                 2U
#define PORTK_BIT2_MASK                 4U
#define PORTK_BIT3_MASK                 8U
#define PORTK_BIT4_MASK                 16U
#define PORTK_BIT5_MASK                 32U
#define PORTK_BIT7_MASK                 128U
#define PORTK_BIT_MASK                  63U
#define PORTK_BIT_BITNUM                0U


/*** DDRK - Port K Data Direction Register; 0x00000033 ***/
typedef union {
  byte Byte;
  struct {
    byte BIT0        :1;                                       /* Port K Data Direction Bit 0 */
    byte BIT1        :1;                                       /* Port K Data Direction Bit 1 */
    byte BIT2        :1;                                       /* Port K Data Direction Bit 2 */
    byte BIT3        :1;                                       /* Port K Data Direction Bit 3 */
    byte BIT4        :1;                                       /* Port K Data Direction Bit 4 */
    byte BIT5        :1;                                       /* Port K Data Direction Bit 5 */
    byte             :1; 
    byte BIT7        :1;                                       /* Port K Data Direction Bit 7 */
  } Bits;
  struct {
    byte grpBIT  :6;
    byte         :1;
    byte grpBIT_7 :1;
  } MergedBits;
} DDRKSTR;
extern volatile DDRKSTR _DDRK @(REG_BASE + 0x00000033UL);
#define DDRK                            _DDRK.Byte
#define DDRK_BIT0                       _DDRK.Bits.BIT0
#define DDRK_BIT1                       _DDRK.Bits.BIT1
#define DDRK_BIT2                       _DDRK.Bits.BIT2
#define DDRK_BIT3                       _DDRK.Bits.BIT3
#define DDRK_BIT4                       _DDRK.Bits.BIT4
#define DDRK_BIT5                       _DDRK.Bits.BIT5
#define DDRK_BIT7                       _DDRK.Bits.BIT7
#define DDRK_BIT                        _DDRK.MergedBits.grpBIT

#define DDRK_BIT0_MASK                  1U
#define DDRK_BIT1_MASK                  2U
#define DDRK_BIT2_MASK                  4U
#define DDRK_BIT3_MASK                  8U
#define DDRK_BIT4_MASK                  16U
#define DDRK_BIT5_MASK                  32U
#define DDRK_BIT7_MASK                  128U
#define DDRK_BIT_MASK                   63U
#define DDRK_BIT_BITNUM                 0U


/*** SYNR - CRG Synthesizer Register; 0x00000034 ***/
typedef union {
  byte Byte;
  struct {
    byte SYN0        :1;                                       /* CRG Synthesizer Bit 0 */
    byte SYN1        :1;                                       /* CRG Synthesizer Bit 1 */
    byte SYN2        :1;                                       /* CRG Synthesizer Bit 2 */
    byte SYN3        :1;                                       /* CRG Synthesizer Bit 3 */
    byte SYN4        :1;                                       /* CRG Synthesizer Bit 4 */
    byte SYN5        :1;                                       /* CRG Synthesizer Bit 5 */
    byte             :1; 
    byte             :1; 
  } Bits;
  struct {
    byte grpSYN  :6;
    byte         :1;
    byte         :1;
  } MergedBits;
} SYNRSTR;
extern volatile SYNRSTR _SYNR @(REG_BASE + 0x00000034UL);
#define SYNR                            _SYNR.Byte
#define SYNR_SYN0                       _SYNR.Bits.SYN0
#define SYNR_SYN1                       _SYNR.Bits.SYN1
#define SYNR_SYN2                       _SYNR.Bits.SYN2
#define SYNR_SYN3                       _SYNR.Bits.SYN3
#define SYNR_SYN4                       _SYNR.Bits.SYN4
#define SYNR_SYN5                       _SYNR.Bits.SYN5
#define SYNR_SYN                        _SYNR.MergedBits.grpSYN

#define SYNR_SYN0_MASK                  1U
#define SYNR_SYN1_MASK                  2U
#define SYNR_SYN2_MASK                  4U
#define SYNR_SYN3_MASK                  8U
#define SYNR_SYN4_MASK                  16U
#define SYNR_SYN5_MASK                  32U
#define SYNR_SYN_MASK                   63U
#define SYNR_SYN_BITNUM                 0U


/*** REFDV - CRG Reference Divider Register; 0x00000035 ***/
typedef union {
  byte Byte;
  struct {
    byte REFDV0      :1;                                       /* CRG Reference Divider Bit 0 */
    byte REFDV1      :1;                                       /* CRG Reference Divider Bit 1 */
    byte REFDV2      :1;                                       /* CRG Reference Divider Bit 2 */
    byte REFDV3      :1;                                       /* CRG Reference Divider Bit 3 */
    byte             :1; 
    byte             :1; 
    byte             :1; 
    byte             :1; 
  } Bits;
  struct {
    byte grpREFDV :4;
    byte         :1;
    byte         :1;
    byte         :1;
    byte         :1;
  } MergedBits;
} REFDVSTR;
extern volatile REFDVSTR _REFDV @(REG_BASE + 0x00000035UL);
#define REFDV                           _REFDV.Byte
#define REFDV_REFDV0                    _REFDV.Bits.REFDV0
#define REFDV_REFDV1                    _REFDV.Bits.REFDV1
#define REFDV_REFDV2                    _REFDV.Bits.REFDV2
#define REFDV_REFDV3                    _REFDV.Bits.REFDV3
#define REFDV_REFDV                     _REFDV.MergedBits.grpREFDV

#define REFDV_REFDV0_MASK               1U
#define REFDV_REFDV1_MASK               2U
#define REFDV_REFDV2_MASK               4U
#define REFDV_REFDV3_MASK               8U
#define REFDV_REFDV_MASK                15U
#define REFDV_REFDV_BITNUM              0U


/*** CRGFLG - CRG Flags Register; 0x00000037 ***/
typedef union {
  byte Byte;
  struct {
    byte SCM         :1;                                       /* Self-clock mode Status */
    byte SCMIF       :1;                                       /* Self-clock mode Interrupt Flag */
    byte TRACK       :1;                                       /* Track Status */
    byte LOCK        :1;                                       /* Lock Status */
    byte LOCKIF      :1;                                       /* PLL Lock Interrupt Flag */
    byte             :1; 
    byte PORF        :1;                                       /* Power on Reset Flag */
    byte RTIF        :1;                                       /* Real Time Interrupt Flag */
  } Bits;
} CRGFLGSTR;
extern volatile CRGFLGSTR _CRGFLG @(REG_BASE + 0x00000037UL);
#define CRGFLG                          _CRGFLG.Byte
#define CRGFLG_SCM                      _CRGFLG.Bits.SCM
#define CRGFLG_SCMIF                    _CRGFLG.Bits.SCMIF
#define CRGFLG_TRACK                    _CRGFLG.Bits.TRACK
#define CRGFLG_LOCK                     _CRGFLG.Bits.LOCK
#define CRGFLG_LOCKIF                   _CRGFLG.Bits.LOCKIF
#define CRGFLG_PORF                     _CRGFLG.Bits.PORF
#define CRGFLG_RTIF                     _CRGFLG.Bits.RTIF

#define CRGFLG_SCM_MASK                 1U
#define CRGFLG_SCMIF_MASK               2U
#define CRGFLG_TRACK_MASK               4U
#define CRGFLG_LOCK_MASK                8U
#define CRGFLG_LOCKIF_MASK              16U
#define CRGFLG_PORF_MASK                64U
#define CRGFLG_RTIF_MASK                128U


/*** CRGINT - CRG Interrupt Enable Register; 0x00000038 ***/
typedef union {
  byte Byte;
  struct {
    byte             :1; 
    byte SCMIE       :1;                                       /* Self-clock mode Interrupt Enable */
    byte             :1; 
    byte             :1; 
    byte LOCKIE      :1;                                       /* Lock Interrupt Enable */
    byte             :1; 
    byte             :1; 
    byte RTIE        :1;                                       /* Real Time Interrupt Enable */
  } Bits;
} CRGINTSTR;
extern volatile CRGINTSTR _CRGINT @(REG_BASE + 0x00000038UL);
#define CRGINT                          _CRGINT.Byte
#define CRGINT_SCMIE                    _CRGINT.Bits.SCMIE
#define CRGINT_LOCKIE                   _CRGINT.Bits.LOCKIE
#define CRGINT_RTIE                     _CRGINT.Bits.RTIE

#define CRGINT_SCMIE_MASK               2U
#define CRGINT_LOCKIE_MASK              16U
#define CRGINT_RTIE_MASK                128U


/*** CLKSEL - CRG Clock Select Register; 0x00000039 ***/
typedef union {
  byte Byte;
  struct {
    byte COPWAI      :1;                                       /* COP stops in WAIT mode */
    byte RTIWAI      :1;                                       /* RTI stops in WAIT mode */
    byte CWAI        :1;                                       /* CLK24 and CLK23 stop in WAIT mode */
    byte PLLWAI      :1;                                       /* PLL stops in WAIT mode */
    byte ROAWAI      :1;                                       /* Reduced Oscillator Amplitude in WAIT mode */
    byte SYSWAI      :1;                                       /* System clocks stop in WAIT mode */
    byte PSTP        :1;                                       /* Pseudo Stop */
    byte PLLSEL      :1;                                       /* PLL selected for system clock */
  } Bits;
} CLKSELSTR;
extern volatile CLKSELSTR _CLKSEL @(REG_BASE + 0x00000039UL);
#define CLKSEL                          _CLKSEL.Byte
#define CLKSEL_COPWAI                   _CLKSEL.Bits.COPWAI
#define CLKSEL_RTIWAI                   _CLKSEL.Bits.RTIWAI
#define CLKSEL_CWAI                     _CLKSEL.Bits.CWAI
#define CLKSEL_PLLWAI                   _CLKSEL.Bits.PLLWAI
#define CLKSEL_ROAWAI                   _CLKSEL.Bits.ROAWAI
#define CLKSEL_SYSWAI                   _CLKSEL.Bits.SYSWAI
#define CLKSEL_PSTP                     _CLKSEL.Bits.PSTP
#define CLKSEL_PLLSEL                   _CLKSEL.Bits.PLLSEL

#define CLKSEL_COPWAI_MASK              1U
#define CLKSEL_RTIWAI_MASK              2U
#define CLKSEL_CWAI_MASK                4U
#define CLKSEL_PLLWAI_MASK              8U
#define CLKSEL_ROAWAI_MASK              16U
#define CLKSEL_SYSWAI_MASK              32U
#define CLKSEL_PSTP_MASK                64U
#define CLKSEL_PLLSEL_MASK              128U


/*** PLLCTL - CRG PLL Control Register; 0x0000003A ***/
typedef union {
  byte Byte;
  struct {
    byte SCME        :1;                                       /* Self-clock mode enable */
    byte PCE         :1;                                       /* COP Enable during Pseudo Stop Bit */
    byte PRE         :1;                                       /* RTI Enable during Pseudo Stop Bit */
    byte             :1; 
    byte ACQ         :1;                                       /* Acquisition */
    byte AUTO        :1;                                       /* Automatic Bandwidth Control */
    byte PLLON       :1;                                       /* Phase Lock Loop On */
    byte CME         :1;                                       /* Clock Monitor Enable */
  } Bits;
} PLLCTLSTR;
extern volatile PLLCTLSTR _PLLCTL @(REG_BASE + 0x0000003AUL);
#define PLLCTL                          _PLLCTL.Byte
#define PLLCTL_SCME                     _PLLCTL.Bits.SCME
#define PLLCTL_PCE                      _PLLCTL.Bits.PCE
#define PLLCTL_PRE                      _PLLCTL.Bits.PRE
#define PLLCTL_ACQ                      _PLLCTL.Bits.ACQ
#define PLLCTL_AUTO                     _PLLCTL.Bits.AUTO
#define PLLCTL_PLLON                    _PLLCTL.Bits.PLLON
#define PLLCTL_CME                      _PLLCTL.Bits.CME

#define PLLCTL_SCME_MASK                1U
#define PLLCTL_PCE_MASK                 2U
#define PLLCTL_PRE_MASK                 4U
#define PLLCTL_ACQ_MASK                 16U
#define PLLCTL_AUTO_MASK                32U
#define PLLCTL_PLLON_MASK               64U
#define PLLCTL_CME_MASK                 128U


/*** RTICTL - CRG RTI Control Register; 0x0000003B ***/
typedef union {
  byte Byte;
  struct {
    byte RTR0        :1;                                       /* Real Time Interrupt Modulus Counter Select Bit 0 */
    byte RTR1        :1;                                       /* Real Time Interrupt Modulus Counter Select Bit 1 */
    byte RTR2        :1;                                       /* Real Time Interrupt Modulus Counter Select Bit 2 */
    byte RTR3        :1;                                       /* Real Time Interrupt Modulus Counter Select Bit 3 */
    byte RTR4        :1;                                       /* Real Time Interrupt Prescale Rate Select Bit 4 */
    byte RTR5        :1;                                       /* Real Time Interrupt Prescale Rate Select Bit 5 */
    byte RTR6        :1;                                       /* Real Time Interrupt Prescale Rate Select Bit 6 */
    byte             :1; 
  } Bits;
  struct {
    byte grpRTR  :7;
    byte         :1;
  } MergedBits;
} RTICTLSTR;
extern volatile RTICTLSTR _RTICTL @(REG_BASE + 0x0000003BUL);
#define RTICTL                          _RTICTL.Byte
#define RTICTL_RTR0                     _RTICTL.Bits.RTR0
#define RTICTL_RTR1                     _RTICTL.Bits.RTR1
#define RTICTL_RTR2                     _RTICTL.Bits.RTR2
#define RTICTL_RTR3                     _RTICTL.Bits.RTR3
#define RTICTL_RTR4                     _RTICTL.Bits.RTR4
#define RTICTL_RTR5                     _RTICTL.Bits.RTR5
#define RTICTL_RTR6                     _RTICTL.Bits.RTR6
#define RTICTL_RTR                      _RTICTL.MergedBits.grpRTR

#define RTICTL_RTR0_MASK                1U
#define RTICTL_RTR1_MASK                2U
#define RTICTL_RTR2_MASK                4U
#define RTICTL_RTR3_MASK                8U
#define RTICTL_RTR4_MASK                16U
#define RTICTL_RTR5_MASK                32U
#define RTICTL_RTR6_MASK                64U
#define RTICTL_RTR_MASK                 127U
#define RTICTL_RTR_BITNUM               0U


/*** COPCTL - CRG COP Control Register; 0x0000003C ***/
typedef union {
  byte Byte;
  struct {
    byte CR0         :1;                                       /* COP Watchdog Timer Rate select Bit 0 */
    byte CR1         :1;                                       /* COP Watchdog Timer Rate select Bit 1 */
    byte CR2         :1;                                       /* COP Watchdog Timer Rate select Bit 2 */
    byte             :1; 
    byte             :1; 
    byte             :1; 
    byte RSBCK       :1;                                       /* COP and RTI stop in Active BDM mode Bit */
    byte WCOP        :1;                                       /* Window COP mode */
  } Bits;
  struct {
    byte grpCR   :3;
    byte         :1;
    byte         :1;
    byte         :1;
    byte         :1;
    byte         :1;
  } MergedBits;
} COPCTLSTR;
extern volatile COPCTLSTR _COPCTL @(REG_BASE + 0x0000003CUL);
#define COPCTL                          _COPCTL.Byte
#define COPCTL_CR0                      _COPCTL.Bits.CR0
#define COPCTL_CR1                      _COPCTL.Bits.CR1
#define COPCTL_CR2                      _COPCTL.Bits.CR2
#define COPCTL_RSBCK                    _COPCTL.Bits.RSBCK
#define COPCTL_WCOP                     _COPCTL.Bits.WCOP
#define COPCTL_CR                       _COPCTL.MergedBits.grpCR

#define COPCTL_CR0_MASK                 1U
#define COPCTL_CR1_MASK                 2U
#define COPCTL_CR2_MASK                 4U
#define COPCTL_RSBCK_MASK               64U
#define COPCTL_WCOP_MASK                128U
#define COPCTL_CR_MASK                  7U
#define COPCTL_CR_BITNUM                0U


/*** ARMCOP - CRG COP Timer Arm/Reset Register; 0x0000003F ***/
typedef union {
  byte Byte;
  struct {
    byte BIT0        :1;                                       /* CRG COP Timer Arm/Reset Bit 0 */
    byte BIT1        :1;                                       /* CRG COP Timer Arm/Reset Bit 1 */
    byte BIT2        :1;                                       /* CRG COP Timer Arm/Reset Bit 2 */
    byte BIT3        :1;                                       /* CRG COP Timer Arm/Reset Bit 3 */
    byte BIT4        :1;                                       /* CRG COP Timer Arm/Reset Bit 4 */
    byte BIT5        :1;                                       /* CRG COP Timer Arm/Reset Bit 5 */
    byte BIT6        :1;                                       /* CRG COP Timer Arm/Reset Bit 6 */
    byte BIT7        :1;                                       /* CRG COP Timer Arm/Reset Bit 7 */
  } Bits;
} ARMCOPSTR;
extern volatile ARMCOPSTR _ARMCOP @(REG_BASE + 0x0000003FUL);
#define ARMCOP                          _ARMCOP.Byte
#define ARMCOP_BIT0                     _ARMCOP.Bits.BIT0
#define ARMCOP_BIT1                     _ARMCOP.Bits.BIT1
#define ARMCOP_BIT2                     _ARMCOP.Bits.BIT2
#define ARMCOP_BIT3                     _ARMCOP.Bits.BIT3
#define ARMCOP_BIT4                     _ARMCOP.Bits.BIT4
#define ARMCOP_BIT5                     _ARMCOP.Bits.BIT5
#define ARMCOP_BIT6                     _ARMCOP.Bits.BIT6
#define ARMCOP_BIT7                     _ARMCOP.Bits.BIT7

#define ARMCOP_BIT0_MASK                1U
#define ARMCOP_BIT1_MASK                2U
#define ARMCOP_BIT2_MASK                4U
#define ARMCOP_BIT3_MASK                8U
#define ARMCOP_BIT4_MASK                16U
#define ARMCOP_BIT5_MASK                32U
#define ARMCOP_BIT6_MASK                64U
#define ARMCOP_BIT7_MASK                128U


/*** TIOS - Timer Input Capture/Output Compare Select; 0x00000040 ***/
typedef union {
  byte Byte;
  struct {
    byte IOS0        :1;                                       /* Input Capture or Output Compare Channel Configuration Bit 0 */
    byte IOS1        :1;                                       /* Input Capture or Output Compare Channel Configuration Bit 1 */
    byte IOS2        :1;                                       /* Input Capture or Output Compare Channel Configuration Bit 2 */
    byte IOS3        :1;                                       /* Input Capture or Output Compare Channel Configuration Bit 3 */
    byte IOS4        :1;                                       /* Input Capture or Output Compare Channel Configuration Bit 4 */
    byte IOS5        :1;                                       /* Input Capture or Output Compare Channel Configuration Bit 5 */
    byte IOS6        :1;                                       /* Input Capture or Output Compare Channel Configuration Bit 6 */
    byte IOS7        :1;                                       /* Input Capture or Output Compare Channel Configuration Bit 7 */
  } Bits;
} TIOSSTR;
extern volatile TIOSSTR _TIOS @(REG_BASE + 0x00000040UL);
#define TIOS                            _TIOS.Byte
#define TIOS_IOS0                       _TIOS.Bits.IOS0
#define TIOS_IOS1                       _TIOS.Bits.IOS1
#define TIOS_IOS2                       _TIOS.Bits.IOS2
#define TIOS_IOS3                       _TIOS.Bits.IOS3
#define TIOS_IOS4                       _TIOS.Bits.IOS4
#define TIOS_IOS5                       _TIOS.Bits.IOS5
#define TIOS_IOS6                       _TIOS.Bits.IOS6
#define TIOS_IOS7                       _TIOS.Bits.IOS7

#define TIOS_IOS0_MASK                  1U
#define TIOS_IOS1_MASK                  2U
#define TIOS_IOS2_MASK                  4U
#define TIOS_IOS3_MASK                  8U
#define TIOS_IOS4_MASK                  16U
#define TIOS_IOS5_MASK                  32U
#define TIOS_IOS6_MASK                  64U
#define TIOS_IOS7_MASK                  128U


/*** CFORC - Timer Compare Force Register; 0x00000041 ***/
typedef union {
  byte Byte;
  struct {
    byte FOC0        :1;                                       /* Force Output Compare Action for Channel 0 */
    byte FOC1        :1;                                       /* Force Output Compare Action for Channel 1 */
    byte FOC2        :1;                                       /* Force Output Compare Action for Channel 2 */
    byte FOC3        :1;                                       /* Force Output Compare Action for Channel 3 */
    byte FOC4        :1;                                       /* Force Output Compare Action for Channel 4 */
    byte FOC5        :1;                                       /* Force Output Compare Action for Channel 5 */
    byte FOC6        :1;                                       /* Force Output Compare Action for Channel 6 */
    byte FOC7        :1;                                       /* Force Output Compare Action for Channel 7 */
  } Bits;
} CFORCSTR;
extern volatile CFORCSTR _CFORC @(REG_BASE + 0x00000041UL);
#define CFORC                           _CFORC.Byte
#define CFORC_FOC0                      _CFORC.Bits.FOC0
#define CFORC_FOC1                      _CFORC.Bits.FOC1
#define CFORC_FOC2                      _CFORC.Bits.FOC2
#define CFORC_FOC3                      _CFORC.Bits.FOC3
#define CFORC_FOC4                      _CFORC.Bits.FOC4
#define CFORC_FOC5                      _CFORC.Bits.FOC5
#define CFORC_FOC6                      _CFORC.Bits.FOC6
#define CFORC_FOC7                      _CFORC.Bits.FOC7

#define CFORC_FOC0_MASK                 1U
#define CFORC_FOC1_MASK                 2U
#define CFORC_FOC2_MASK                 4U
#define CFORC_FOC3_MASK                 8U
#define CFORC_FOC4_MASK                 16U
#define CFORC_FOC5_MASK                 32U
#define CFORC_FOC6_MASK                 64U
#define CFORC_FOC7_MASK                 128U


/*** OC7M - Output Compare 7 Mask Register; 0x00000042 ***/
typedef union {
  byte Byte;
  struct {
    byte OC7M0       :1;                                       /* Output Compare 7 Mask Bit 0 */
    byte OC7M1       :1;                                       /* Output Compare 7 Mask Bit 1 */
    byte OC7M2       :1;                                       /* Output Compare 7 Mask Bit 2 */
    byte OC7M3       :1;                                       /* Output Compare 7 Mask Bit 3 */
    byte OC7M4       :1;                                       /* Output Compare 7 Mask Bit 4 */
    byte OC7M5       :1;                                       /* Output Compare 7 Mask Bit 5 */
    byte OC7M6       :1;                                       /* Output Compare 7 Mask Bit 6 */
    byte OC7M7       :1;                                       /* Output Compare 7 Mask Bit 7 */
  } Bits;
} OC7MSTR;
extern volatile OC7MSTR _OC7M @(REG_BASE + 0x00000042UL);
#define OC7M                            _OC7M.Byte
#define OC7M_OC7M0                      _OC7M.Bits.OC7M0
#define OC7M_OC7M1                      _OC7M.Bits.OC7M1
#define OC7M_OC7M2                      _OC7M.Bits.OC7M2
#define OC7M_OC7M3                      _OC7M.Bits.OC7M3
#define OC7M_OC7M4                      _OC7M.Bits.OC7M4
#define OC7M_OC7M5                      _OC7M.Bits.OC7M5
#define OC7M_OC7M6                      _OC7M.Bits.OC7M6
#define OC7M_OC7M7                      _OC7M.Bits.OC7M7

#define OC7M_OC7M0_MASK                 1U
#define OC7M_OC7M1_MASK                 2U
#define OC7M_OC7M2_MASK                 4U
#define OC7M_OC7M3_MASK                 8U
#define OC7M_OC7M4_MASK                 16U
#define OC7M_OC7M5_MASK                 32U
#define OC7M_OC7M6_MASK                 64U
#define OC7M_OC7M7_MASK                 128U


/*** OC7D - Output Compare 7 Data Register; 0x00000043 ***/
typedef union {
  byte Byte;
} OC7DSTR;
extern volatile OC7DSTR _OC7D @(REG_BASE + 0x00000043UL);
#define OC7D                            _OC7D.Byte


/*** TCNT - Timer Count Register; 0x00000044 ***/
typedef union {
  word Word;
   /* Overlapped registers: */
  struct {
    /*** TCNTHi - Timer Count Register High; 0x00000044 ***/
    union {
      byte Byte;
    } TCNTHiSTR;
    #define TCNTHi                      _TCNT.Overlap_STR.TCNTHiSTR.Byte
    

    /*** TCNTLo - Timer Count Register Low; 0x00000045 ***/
    union {
      byte Byte;
    } TCNTLoSTR;
    #define TCNTLo                      _TCNT.Overlap_STR.TCNTLoSTR.Byte
    
  } Overlap_STR;

} TCNTSTR;
extern volatile TCNTSTR _TCNT @(REG_BASE + 0x00000044UL);
#define TCNT                            _TCNT.Word


/*** TSCR1 - Timer System Control Register1; 0x00000046 ***/
typedef union {
  byte Byte;
  struct {
    byte             :1; 
    byte             :1; 
    byte             :1; 
    byte             :1; 
    byte TFFCA       :1;                                       /* Timer Fast Flag Clear All */
    byte TSFRZ       :1;                                       /* Timer and Modulus Counter Stop While in Freeze Mode */
    byte TSWAI       :1;                                       /* Timer Module Stops While in Wait */
    byte TEN         :1;                                       /* Timer Enable */
  } Bits;
} TSCR1STR;
extern volatile TSCR1STR _TSCR1 @(REG_BASE + 0x00000046UL);
#define TSCR1                           _TSCR1.Byte
#define TSCR1_TFFCA                     _TSCR1.Bits.TFFCA
#define TSCR1_TSFRZ                     _TSCR1.Bits.TSFRZ
#define TSCR1_TSWAI                     _TSCR1.Bits.TSWAI
#define TSCR1_TEN                       _TSCR1.Bits.TEN

#define TSCR1_TFFCA_MASK                16U
#define TSCR1_TSFRZ_MASK                32U
#define TSCR1_TSWAI_MASK                64U
#define TSCR1_TEN_MASK                  128U


/*** TTOV - Timer Toggle On Overflow Register; 0x00000047 ***/
typedef union {
  byte Byte;
  struct {
    byte TOV0        :1;                                       /* Toggle On Overflow Bit 0 */
    byte TOV1        :1;                                       /* Toggle On Overflow Bit 1 */
    byte TOV2        :1;                                       /* Toggle On Overflow Bit 2 */
    byte TOV3        :1;                                       /* Toggle On Overflow Bit 3 */
    byte TOV4        :1;                                       /* Toggle On Overflow Bit 4 */
    byte TOV5        :1;                                       /* Toggle On Overflow Bit 5 */
    byte TOV6        :1;                                       /* Toggle On Overflow Bit 6 */
    byte TOV7        :1;                                       /* Toggle On Overflow Bit 7 */
  } Bits;
} TTOVSTR;
extern volatile TTOVSTR _TTOV @(REG_BASE + 0x00000047UL);
#define TTOV                            _TTOV.Byte
#define TTOV_TOV0                       _TTOV.Bits.TOV0
#define TTOV_TOV1                       _TTOV.Bits.TOV1
#define TTOV_TOV2                       _TTOV.Bits.TOV2
#define TTOV_TOV3                       _TTOV.Bits.TOV3
#define TTOV_TOV4                       _TTOV.Bits.TOV4
#define TTOV_TOV5                       _TTOV.Bits.TOV5
#define TTOV_TOV6                       _TTOV.Bits.TOV6
#define TTOV_TOV7                       _TTOV.Bits.TOV7

#define TTOV_TOV0_MASK                  1U
#define TTOV_TOV1_MASK                  2U
#define TTOV_TOV2_MASK                  4U
#define TTOV_TOV3_MASK                  8U
#define TTOV_TOV4_MASK                  16U
#define TTOV_TOV5_MASK                  32U
#define TTOV_TOV6_MASK                  64U
#define TTOV_TOV7_MASK                  128U


/*** TCTL1 - Timer Control Register 1; 0x00000048 ***/
typedef union {
  byte Byte;
  struct {
    byte OL4         :1;                                       /* Output Level Bit 4 */
    byte OM4         :1;                                       /* Output Mode Bit 4 */
    byte OL5         :1;                                       /* Output Level Bit 5 */
    byte OM5         :1;                                       /* Output Mode Bit 5 */
    byte OL6         :1;                                       /* Output Level Bit 6 */
    byte OM6         :1;                                       /* Output Mode Bit 6 */
    byte OL7         :1;                                       /* Output Level Bit 7 */
    byte OM7         :1;                                       /* Output Mode Bit 7 */
  } Bits;
} TCTL1STR;
extern volatile TCTL1STR _TCTL1 @(REG_BASE + 0x00000048UL);
#define TCTL1                           _TCTL1.Byte
#define TCTL1_OL4                       _TCTL1.Bits.OL4
#define TCTL1_OM4                       _TCTL1.Bits.OM4
#define TCTL1_OL5                       _TCTL1.Bits.OL5
#define TCTL1_OM5                       _TCTL1.Bits.OM5
#define TCTL1_OL6                       _TCTL1.Bits.OL6
#define TCTL1_OM6                       _TCTL1.Bits.OM6
#define TCTL1_OL7                       _TCTL1.Bits.OL7
#define TCTL1_OM7                       _TCTL1.Bits.OM7

#define TCTL1_OL4_MASK                  1U
#define TCTL1_OM4_MASK                  2U
#define TCTL1_OL5_MASK                  4U
#define TCTL1_OM5_MASK                  8U
#define TCTL1_OL6_MASK                  16U
#define TCTL1_OM6_MASK                  32U
#define TCTL1_OL7_MASK                  64U
#define TCTL1_OM7_MASK                  128U


/*** TCTL2 - Timer Control Register 2; 0x00000049 ***/
typedef union {
  byte Byte;
  struct {
    byte OL0         :1;                                       /* Output Level Bit 0 */
    byte OM0         :1;                                       /* Output Mode Bit 0 */
    byte OL1         :1;                                       /* Output Level Bit 1 */
    byte OM1         :1;                                       /* Output Mode Bit 1 */
    byte OL2         :1;                                       /* Output Level Bit 2 */
    byte OM2         :1;                                       /* Output Mode Bit 2 */
    byte OL3         :1;                                       /* Output Level Bit 3 */
    byte OM3         :1;                                       /* Output Mode Bit 3 */
  } Bits;
} TCTL2STR;
extern volatile TCTL2STR _TCTL2 @(REG_BASE + 0x00000049UL);
#define TCTL2                           _TCTL2.Byte
#define TCTL2_OL0                       _TCTL2.Bits.OL0
#define TCTL2_OM0                       _TCTL2.Bits.OM0
#define TCTL2_OL1                       _TCTL2.Bits.OL1
#define TCTL2_OM1                       _TCTL2.Bits.OM1
#define TCTL2_OL2                       _TCTL2.Bits.OL2
#define TCTL2_OM2                       _TCTL2.Bits.OM2
#define TCTL2_OL3                       _TCTL2.Bits.OL3
#define TCTL2_OM3                       _TCTL2.Bits.OM3

#define TCTL2_OL0_MASK                  1U
#define TCTL2_OM0_MASK                  2U
#define TCTL2_OL1_MASK                  4U
#define TCTL2_OM1_MASK                  8U
#define TCTL2_OL2_MASK                  16U
#define TCTL2_OM2_MASK                  32U
#define TCTL2_OL3_MASK                  64U
#define TCTL2_OM3_MASK                  128U


/*** TCTL3 - Timer Control Register 3; 0x0000004A ***/
typedef union {
  byte Byte;
  struct {
    byte EDG4A       :1;                                       /* Input Capture Edge Control 4A */
    byte EDG4B       :1;                                       /* Input Capture Edge Control 4B */
    byte EDG5A       :1;                                       /* Input Capture Edge Control 5A */
    byte EDG5B       :1;                                       /* Input Capture Edge Control 5B */
    byte EDG6A       :1;                                       /* Input Capture Edge Control 6A */
    byte EDG6B       :1;                                       /* Input Capture Edge Control 6B */
    byte EDG7A       :1;                                       /* Input Capture Edge Control 7A */
    byte EDG7B       :1;                                       /* Input Capture Edge Control 7B */
  } Bits;
  struct {
    byte grpEDG4x :2;
    byte grpEDG5x :2;
    byte grpEDG6x :2;
    byte grpEDG7x :2;
  } MergedBits;
} TCTL3STR;
extern volatile TCTL3STR _TCTL3 @(REG_BASE + 0x0000004AUL);
#define TCTL3                           _TCTL3.Byte
#define TCTL3_EDG4A                     _TCTL3.Bits.EDG4A
#define TCTL3_EDG4B                     _TCTL3.Bits.EDG4B
#define TCTL3_EDG5A                     _TCTL3.Bits.EDG5A
#define TCTL3_EDG5B                     _TCTL3.Bits.EDG5B
#define TCTL3_EDG6A                     _TCTL3.Bits.EDG6A
#define TCTL3_EDG6B                     _TCTL3.Bits.EDG6B
#define TCTL3_EDG7A                     _TCTL3.Bits.EDG7A
#define TCTL3_EDG7B                     _TCTL3.Bits.EDG7B
#define TCTL3_EDG4x                     _TCTL3.MergedBits.grpEDG4x
#define TCTL3_EDG5x                     _TCTL3.MergedBits.grpEDG5x
#define TCTL3_EDG6x                     _TCTL3.MergedBits.grpEDG6x
#define TCTL3_EDG7x                     _TCTL3.MergedBits.grpEDG7x

#define TCTL3_EDG4A_MASK                1U
#define TCTL3_EDG4B_MASK                2U
#define TCTL3_EDG5A_MASK                4U
#define TCTL3_EDG5B_MASK                8U
#define TCTL3_EDG6A_MASK                16U
#define TCTL3_EDG6B_MASK                32U
#define TCTL3_EDG7A_MASK                64U
#define TCTL3_EDG7B_MASK                128U
#define TCTL3_EDG4x_MASK                3U
#define TCTL3_EDG4x_BITNUM              0U
#define TCTL3_EDG5x_MASK                12U
#define TCTL3_EDG5x_BITNUM              2U
#define TCTL3_EDG6x_MASK                48U
#define TCTL3_EDG6x_BITNUM              4U
#define TCTL3_EDG7x_MASK                192U
#define TCTL3_EDG7x_BITNUM              6U


/*** TCTL4 - Timer Control Register 4; 0x0000004B ***/
typedef union {
  byte Byte;
  struct {
    byte EDG0A       :1;                                       /* Input Capture Edge Control 0A */
    byte EDG0B       :1;                                       /* Input Capture Edge Control 0B */
    byte EDG1A       :1;                                       /* Input Capture Edge Control 1A */
    byte EDG1B       :1;                                       /* Input Capture Edge Control 1B */
    byte EDG2A       :1;                                       /* Input Capture Edge Control 2A */
    byte EDG2B       :1;                                       /* Input Capture Edge Control 2B */
    byte EDG3A       :1;                                       /* Input Capture Edge Control 3A */
    byte EDG3B       :1;                                       /* Input Capture Edge Control 3B */
  } Bits;
  struct {
    byte grpEDG0x :2;
    byte grpEDG1x :2;
    byte grpEDG2x :2;
    byte grpEDG3x :2;
  } MergedBits;
} TCTL4STR;
extern volatile TCTL4STR _TCTL4 @(REG_BASE + 0x0000004BUL);
#define TCTL4                           _TCTL4.Byte
#define TCTL4_EDG0A                     _TCTL4.Bits.EDG0A
#define TCTL4_EDG0B                     _TCTL4.Bits.EDG0B
#define TCTL4_EDG1A                     _TCTL4.Bits.EDG1A
#define TCTL4_EDG1B                     _TCTL4.Bits.EDG1B
#define TCTL4_EDG2A                     _TCTL4.Bits.EDG2A
#define TCTL4_EDG2B                     _TCTL4.Bits.EDG2B
#define TCTL4_EDG3A                     _TCTL4.Bits.EDG3A
#define TCTL4_EDG3B                     _TCTL4.Bits.EDG3B
#define TCTL4_EDG0x                     _TCTL4.MergedBits.grpEDG0x
#define TCTL4_EDG1x                     _TCTL4.MergedBits.grpEDG1x
#define TCTL4_EDG2x                     _TCTL4.MergedBits.grpEDG2x
#define TCTL4_EDG3x                     _TCTL4.MergedBits.grpEDG3x

#define TCTL4_EDG0A_MASK                1U
#define TCTL4_EDG0B_MASK                2U
#define TCTL4_EDG1A_MASK                4U
#define TCTL4_EDG1B_MASK                8U
#define TCTL4_EDG2A_MASK                16U
#define TCTL4_EDG2B_MASK                32U
#define TCTL4_EDG3A_MASK                64U
#define TCTL4_EDG3B_MASK                128U
#define TCTL4_EDG0x_MASK                3U
#define TCTL4_EDG0x_BITNUM              0U
#define TCTL4_EDG1x_MASK                12U
#define TCTL4_EDG1x_BITNUM              2U
#define TCTL4_EDG2x_MASK                48U
#define TCTL4_EDG2x_BITNUM              4U
#define TCTL4_EDG3x_MASK                192U
#define TCTL4_EDG3x_BITNUM              6U


/*** TIE - Timer Interrupt Enable Register; 0x0000004C ***/
typedef union {
  byte Byte;
  struct {
    byte C0I         :1;                                       /* Input Capture/Output Compare Interrupt Enable Bit 0 */
    byte C1I         :1;                                       /* Input Capture/Output Compare Interrupt Enable Bit 1 */
    byte C2I         :1;                                       /* Input Capture/Output Compare Interrupt Enable Bit 2 */
    byte C3I         :1;                                       /* Input Capture/Output Compare Interrupt Enable Bit 3 */
    byte C4I         :1;                                       /* Input Capture/Output Compare Interrupt Enable Bit 4 */
    byte C5I         :1;                                       /* Input Capture/Output Compare Interrupt Enable Bit 5 */
    byte C6I         :1;                                       /* Input Capture/Output Compare Interrupt Enable Bit 6 */
    byte C7I         :1;                                       /* Input Capture/Output Compare Interrupt Enable Bit 7 */
  } Bits;
} TIESTR;
extern volatile TIESTR _TIE @(REG_BASE + 0x0000004CUL);
#define TIE                             _TIE.Byte
#define TIE_C0I                         _TIE.Bits.C0I
#define TIE_C1I                         _TIE.Bits.C1I
#define TIE_C2I                         _TIE.Bits.C2I
#define TIE_C3I                         _TIE.Bits.C3I
#define TIE_C4I                         _TIE.Bits.C4I
#define TIE_C5I                         _TIE.Bits.C5I
#define TIE_C6I                         _TIE.Bits.C6I
#define TIE_C7I                         _TIE.Bits.C7I

#define TIE_C0I_MASK                    1U
#define TIE_C1I_MASK                    2U
#define TIE_C2I_MASK                    4U
#define TIE_C3I_MASK                    8U
#define TIE_C4I_MASK                    16U
#define TIE_C5I_MASK                    32U
#define TIE_C6I_MASK                    64U
#define TIE_C7I_MASK                    128U


/*** TSCR2 - Timer System Control Register 2; 0x0000004D ***/
typedef union {
  byte Byte;
  struct {
    byte PR0         :1;                                       /* Timer Prescaler Select Bit 0 */
    byte PR1         :1;                                       /* Timer Prescaler Select Bit 1 */
    byte PR2         :1;                                       /* Timer Prescaler Select Bit 2 */
    byte TCRE        :1;                                       /* Timer Counter Reset Enable */
    byte             :1; 
    byte             :1; 
    byte             :1; 
    byte TOI         :1;                                       /* Timer Overflow Interrupt Enable */
  } Bits;
  struct {
    byte grpPR   :3;
    byte         :1;
    byte         :1;
    byte         :1;
    byte         :1;
    byte         :1;
  } MergedBits;
} TSCR2STR;
extern volatile TSCR2STR _TSCR2 @(REG_BASE + 0x0000004DUL);
#define TSCR2                           _TSCR2.Byte
#define TSCR2_PR0                       _TSCR2.Bits.PR0
#define TSCR2_PR1                       _TSCR2.Bits.PR1
#define TSCR2_PR2                       _TSCR2.Bits.PR2
#define TSCR2_TCRE                      _TSCR2.Bits.TCRE
#define TSCR2_TOI                       _TSCR2.Bits.TOI
#define TSCR2_PR                        _TSCR2.MergedBits.grpPR

#define TSCR2_PR0_MASK                  1U
#define TSCR2_PR1_MASK                  2U
#define TSCR2_PR2_MASK                  4U
#define TSCR2_TCRE_MASK                 8U
#define TSCR2_TOI_MASK                  128U
#define TSCR2_PR_MASK                   7U
#define TSCR2_PR_BITNUM                 0U


/*** TFLG1 - Main Timer Interrupt Flag 1; 0x0000004E ***/
typedef union {
  byte Byte;
  struct {
    byte C0F         :1;                                       /* Input Capture/Output Compare Channel Flag 0 */
    byte C1F         :1;                                       /* Input Capture/Output Compare Channel Flag 1 */
    byte C2F         :1;                                       /* Input Capture/Output Compare Channel Flag 2 */
    byte C3F         :1;                                       /* Input Capture/Output Compare Channel Flag 3 */
    byte C4F         :1;                                       /* Input Capture/Output Compare Channel Flag 4 */
    byte C5F         :1;                                       /* Input Capture/Output Compare Channel Flag 5 */
    byte C6F         :1;                                       /* Input Capture/Output Compare Channel Flag 6 */
    byte C7F         :1;                                       /* Input Capture/Output Compare Channel Flag 7 */
  } Bits;
} TFLG1STR;
extern volatile TFLG1STR _TFLG1 @(REG_BASE + 0x0000004EUL);
#define TFLG1                           _TFLG1.Byte
#define TFLG1_C0F                       _TFLG1.Bits.C0F
#define TFLG1_C1F                       _TFLG1.Bits.C1F
#define TFLG1_C2F                       _TFLG1.Bits.C2F
#define TFLG1_C3F                       _TFLG1.Bits.C3F
#define TFLG1_C4F                       _TFLG1.Bits.C4F
#define TFLG1_C5F                       _TFLG1.Bits.C5F
#define TFLG1_C6F                       _TFLG1.Bits.C6F
#define TFLG1_C7F                       _TFLG1.Bits.C7F

#define TFLG1_C0F_MASK                  1U
#define TFLG1_C1F_MASK                  2U
#define TFLG1_C2F_MASK                  4U
#define TFLG1_C3F_MASK                  8U
#define TFLG1_C4F_MASK                  16U
#define TFLG1_C5F_MASK                  32U
#define TFLG1_C6F_MASK                  64U
#define TFLG1_C7F_MASK                  128U


/*** TFLG2 - Main Timer Interrupt Flag 2; 0x0000004F ***/
typedef union {
  byte Byte;
  struct {
    byte             :1; 
    byte             :1; 
    byte             :1; 
    byte             :1; 
    byte             :1; 
    byte             :1; 
    byte             :1; 
    byte TOF         :1;                                       /* Timer Overflow Flag */
  } Bits;
} TFLG2STR;
extern volatile TFLG2STR _TFLG2 @(REG_BASE + 0x0000004FUL);
#define TFLG2                           _TFLG2.Byte
#define TFLG2_TOF                       _TFLG2.Bits.TOF

#define TFLG2_TOF_MASK                  128U


/*** TC0 - Timer Input Capture/Output Compare Register 0; 0x00000050 ***/
typedef union {
  word Word;
   /* Overlapped registers: */
  struct {
    /*** TC0Hi - Timer Input Capture/Output Compare Register 0 High; 0x00000050 ***/
    union {
      byte Byte;
    } TC0HiSTR;
    #define TC0Hi                       _TC0.Overlap_STR.TC0HiSTR.Byte
    

    /*** TC0Lo - Timer Input Capture/Output Compare Register 0 Low; 0x00000051 ***/
    union {
      byte Byte;
    } TC0LoSTR;
    #define TC0Lo                       _TC0.Overlap_STR.TC0LoSTR.Byte
    
  } Overlap_STR;

} TC0STR;
extern volatile TC0STR _TC0 @(REG_BASE + 0x00000050UL);
#define TC0                             _TC0.Word
/* TC_ARR: Access 8 TCx registers in an array */
#define TC_ARR                          ((volatile word *) &TC0)


/*** TC1 - Timer Input Capture/Output Compare Register 1; 0x00000052 ***/
typedef union {
  word Word;
   /* Overlapped registers: */
  struct {
    /*** TC1Hi - Timer Input Capture/Output Compare Register 1 High; 0x00000052 ***/
    union {
      byte Byte;
    } TC1HiSTR;
    #define TC1Hi                       _TC1.Overlap_STR.TC1HiSTR.Byte
    

    /*** TC1Lo - Timer Input Capture/Output Compare Register 1 Low; 0x00000053 ***/
    union {
      byte Byte;
    } TC1LoSTR;
    #define TC1Lo                       _TC1.Overlap_STR.TC1LoSTR.Byte
    
  } Overlap_STR;

} TC1STR;
extern volatile TC1STR _TC1 @(REG_BASE + 0x00000052UL);
#define TC1                             _TC1.Word


/*** TC2 - Timer Input Capture/Output Compare Register 2; 0x00000054 ***/
typedef union {
  word Word;
   /* Overlapped registers: */
  struct {
    /*** TC2Hi - Timer Input Capture/Output Compare Register 2 High; 0x00000054 ***/
    union {
      byte Byte;
    } TC2HiSTR;
    #define TC2Hi                       _TC2.Overlap_STR.TC2HiSTR.Byte
    

    /*** TC2Lo - Timer Input Capture/Output Compare Register 2 Low; 0x00000055 ***/
    union {
      byte Byte;
    } TC2LoSTR;
    #define TC2Lo                       _TC2.Overlap_STR.TC2LoSTR.Byte
    
  } Overlap_STR;

} TC2STR;
extern volatile TC2STR _TC2 @(REG_BASE + 0x00000054UL);
#define TC2                             _TC2.Word


/*** TC3 - Timer Input Capture/Output Compare Register 3; 0x00000056 ***/
typedef union {
  word Word;
   /* Overlapped registers: */
  struct {
    /*** TC3Hi - Timer Input Capture/Output Compare Register 3 High; 0x00000056 ***/
    union {
      byte Byte;
    } TC3HiSTR;
    #define TC3Hi                       _TC3.Overlap_STR.TC3HiSTR.Byte
    

    /*** TC3Lo - Timer Input Capture/Output Compare Register 3 Low; 0x00000057 ***/
    union {
      byte Byte;
    } TC3LoSTR;
    #define TC3Lo                       _TC3.Overlap_STR.TC3LoSTR.Byte
    
  } Overlap_STR;

} TC3STR;
extern volatile TC3STR _TC3 @(REG_BASE + 0x00000056UL);
#define TC3                             _TC3.Word


/*** TC4 - Timer Input Capture/Output Compare Register 4; 0x00000058 ***/
typedef union {
  word Word;
   /* Overlapped registers: */
  struct {
    /*** TC4Hi - Timer Input Capture/Output Compare Register 4 High; 0x00000058 ***/
    union {
      byte Byte;
    } TC4HiSTR;
    #define TC4Hi                       _TC4.Overlap_STR.TC4HiSTR.Byte
    

    /*** TC4Lo - Timer Input Capture/Output Compare Register 4 Low; 0x00000059 ***/
    union {
      byte Byte;
    } TC4LoSTR;
    #define TC4Lo                       _TC4.Overlap_STR.TC4LoSTR.Byte
    
  } Overlap_STR;

} TC4STR;
extern volatile TC4STR _TC4 @(REG_BASE + 0x00000058UL);
#define TC4                             _TC4.Word


/*** TC5 - Timer Input Capture/Output Compare Register 5; 0x0000005A ***/
typedef union {
  word Word;
   /* Overlapped registers: */
  struct {
    /*** TC5Hi - Timer Input Capture/Output Compare Register 5 High; 0x0000005A ***/
    union {
      byte Byte;
    } TC5HiSTR;
    #define TC5Hi                       _TC5.Overlap_STR.TC5HiSTR.Byte
    

    /*** TC5Lo - Timer Input Capture/Output Compare Register 5 Low; 0x0000005B ***/
    union {
      byte Byte;
    } TC5LoSTR;
    #define TC5Lo                       _TC5.Overlap_STR.TC5LoSTR.Byte
    
  } Overlap_STR;

} TC5STR;
extern volatile TC5STR _TC5 @(REG_BASE + 0x0000005AUL);
#define TC5                             _TC5.Word


/*** TC6 - Timer Input Capture/Output Compare Register 6; 0x0000005C ***/
typedef union {
  word Word;
   /* Overlapped registers: */
  struct {
    /*** TC6Hi - Timer Input Capture/Output Compare Register 6 High; 0x0000005C ***/
    union {
      byte Byte;
    } TC6HiSTR;
    #define TC6Hi                       _TC6.Overlap_STR.TC6HiSTR.Byte
    

    /*** TC6Lo - Timer Input Capture/Output Compare Register 6 Low; 0x0000005D ***/
    union {
      byte Byte;
    } TC6LoSTR;
    #define TC6Lo                       _TC6.Overlap_STR.TC6LoSTR.Byte
    
  } Overlap_STR;

} TC6STR;
extern volatile TC6STR _TC6 @(REG_BASE + 0x0000005CUL);
#define TC6                             _TC6.Word


/*** TC7 - Timer Input Capture/Output Compare Register 7; 0x0000005E ***/
typedef union {
  word Word;
   /* Overlapped registers: */
  struct {
    /*** TC7Hi - Timer Input Capture/Output Compare Register 7 High; 0x0000005E ***/
    union {
      byte Byte;
    } TC7HiSTR;
    #define TC7Hi                       _TC7.Overlap_STR.TC7HiSTR.Byte
    

    /*** TC7Lo - Timer Input Capture/Output Compare Register 7 Low; 0x0000005F ***/
    union {
      byte Byte;
    } TC7LoSTR;
    #define TC7Lo                       _TC7.Overlap_STR.TC7LoSTR.Byte
    
  } Overlap_STR;

} TC7STR;
extern volatile TC7STR _TC7 @(REG_BASE + 0x0000005EUL);
#define TC7                             _TC7.Word


/*** PACTL - 16-Bit Pulse Accumulator A Control Register; 0x00000060 ***/
typedef union {
  byte Byte;
  struct {
    byte PAI         :1;                                       /* Pulse Accumulator Input Interrupt enable */
    byte PAOVI       :1;                                       /* Pulse Accumulator A Overflow Interrupt enable */
    byte CLK0        :1;                                       /* Clock Select Bit 0 */
    byte CLK1        :1;                                       /* Clock Select Bit 1 */
    byte PEDGE       :1;                                       /* Pulse Accumulator Edge Control */
    byte PAMOD       :1;                                       /* Pulse Accumulator Mode */
    byte PAEN        :1;                                       /* Pulse Accumulator A System Enable */
    byte             :1; 
  } Bits;
  struct {
    byte         :1;
    byte         :1;
    byte grpCLK  :2;
    byte         :1;
    byte         :1;
    byte         :1;
    byte         :1;
  } MergedBits;
} PACTLSTR;
extern volatile PACTLSTR _PACTL @(REG_BASE + 0x00000060UL);
#define PACTL                           _PACTL.Byte
#define PACTL_PAI                       _PACTL.Bits.PAI
#define PACTL_PAOVI                     _PACTL.Bits.PAOVI
#define PACTL_CLK0                      _PACTL.Bits.CLK0
#define PACTL_CLK1                      _PACTL.Bits.CLK1
#define PACTL_PEDGE                     _PACTL.Bits.PEDGE
#define PACTL_PAMOD                     _PACTL.Bits.PAMOD
#define PACTL_PAEN                      _PACTL.Bits.PAEN
#define PACTL_CLK                       _PACTL.MergedBits.grpCLK

#define PACTL_PAI_MASK                  1U
#define PACTL_PAOVI_MASK                2U
#define PACTL_CLK0_MASK                 4U
#define PACTL_CLK1_MASK                 8U
#define PACTL_PEDGE_MASK                16U
#define PACTL_PAMOD_MASK                32U
#define PACTL_PAEN_MASK                 64U
#define PACTL_CLK_MASK                  12U
#define PACTL_CLK_BITNUM                2U


/*** PAFLG - Pulse Accumulator A Flag Register; 0x00000061 ***/
typedef union {
  byte Byte;
  struct {
    byte PAIF        :1;                                       /* Pulse Accumulator Input edge Flag */
    byte PAOVF       :1;                                       /* Pulse Accumulator A Overflow Flag */
    byte             :1; 
    byte             :1; 
    byte             :1; 
    byte             :1; 
    byte             :1; 
    byte             :1; 
  } Bits;
} PAFLGSTR;
extern volatile PAFLGSTR _PAFLG @(REG_BASE + 0x00000061UL);
#define PAFLG                           _PAFLG.Byte
#define PAFLG_PAIF                      _PAFLG.Bits.PAIF
#define PAFLG_PAOVF                     _PAFLG.Bits.PAOVF

#define PAFLG_PAIF_MASK                 1U
#define PAFLG_PAOVF_MASK                2U


/*** PACN32 - Pulse Accumulators Count 32 Register; 0x00000062 ***/
typedef union {
  word Word;
   /* Overlapped registers: */
  struct {
    /*** PACN3 - Pulse Accumulators Count 3 Register; 0x00000062 ***/
    union {
      byte Byte;
    } PACN3STR;
    #define PACN3                       _PACN32.Overlap_STR.PACN3STR.Byte
    

    /*** PACN2 - Pulse Accumulators Count 2 Register; 0x00000063 ***/
    union {
      byte Byte;
    } PACN2STR;
    #define PACN2                       _PACN32.Overlap_STR.PACN2STR.Byte
    
  } Overlap_STR;

} PACN32STR;
extern volatile PACN32STR _PACN32 @(REG_BASE + 0x00000062UL);
#define PACN32                          _PACN32.Word


/*** PACN10 - Pulse Accumulators Count 10 Register; 0x00000064 ***/
typedef union {
  word Word;
   /* Overlapped registers: */
  struct {
    /*** PACN1 - Pulse Accumulators Count 1 Register; 0x00000064 ***/
    union {
      byte Byte;
    } PACN1STR;
    #define PACN1                       _PACN10.Overlap_STR.PACN1STR.Byte
    

    /*** PACN0 - Pulse Accumulators Count 0 Register; 0x00000065 ***/
    union {
      byte Byte;
    } PACN0STR;
    #define PACN0                       _PACN10.Overlap_STR.PACN0STR.Byte
    
  } Overlap_STR;

} PACN10STR;
extern volatile PACN10STR _PACN10 @(REG_BASE + 0x00000064UL);
#define PACN10                          _PACN10.Word


/*** MCCTL - Modulus Down Counter underflow; 0x00000066 ***/
typedef union {
  byte Byte;
  struct {
    byte MCPR0       :1;                                       /* Modulus Counter Prescaler select 0 */
    byte MCPR1       :1;                                       /* Modulus Counter Prescaler select 1 */
    byte MCEN        :1;                                       /* Modulus Down-Counter Enable */
    byte FLMC        :1;                                       /* Force Load Register into the Modulus Counter Count Register */
    byte ICLAT       :1;                                       /* Input Capture Force Latch Action */
    byte RDMCL       :1;                                       /* Read Modulus Down-Counter Load */
    byte MODMC       :1;                                       /* Modulus Mode Enable */
    byte MCZI        :1;                                       /* Modulus Counter Underflow Interrupt Enable */
  } Bits;
  struct {
    byte grpMCPR :2;
    byte         :1;
    byte         :1;
    byte         :1;
    byte         :1;
    byte         :1;
    byte         :1;
  } MergedBits;
} MCCTLSTR;
extern volatile MCCTLSTR _MCCTL @(REG_BASE + 0x00000066UL);
#define MCCTL                           _MCCTL.Byte
#define MCCTL_MCPR0                     _MCCTL.Bits.MCPR0
#define MCCTL_MCPR1                     _MCCTL.Bits.MCPR1
#define MCCTL_MCEN                      _MCCTL.Bits.MCEN
#define MCCTL_FLMC                      _MCCTL.Bits.FLMC
#define MCCTL_ICLAT                     _MCCTL.Bits.ICLAT
#define MCCTL_RDMCL                     _MCCTL.Bits.RDMCL
#define MCCTL_MODMC                     _MCCTL.Bits.MODMC
#define MCCTL_MCZI                      _MCCTL.Bits.MCZI
#define MCCTL_MCPR                      _MCCTL.MergedBits.grpMCPR

#define MCCTL_MCPR0_MASK                1U
#define MCCTL_MCPR1_MASK                2U
#define MCCTL_MCEN_MASK                 4U
#define MCCTL_FLMC_MASK                 8U
#define MCCTL_ICLAT_MASK                16U
#define MCCTL_RDMCL_MASK                32U
#define MCCTL_MODMC_MASK                64U
#define MCCTL_MCZI_MASK                 128U
#define MCCTL_MCPR_MASK                 3U
#define MCCTL_MCPR_BITNUM               0U


/*** MCFLG - 16-Bit Modulus Down Counter Flag Register; 0x00000067 ***/
typedef union {
  byte Byte;
  struct {
    byte POLF0       :1;                                       /* First Input Capture Polarity Status 0 */
    byte POLF1       :1;                                       /* First Input Capture Polarity Status 1 */
    byte POLF2       :1;                                       /* First Input Capture Polarity Status 2 */
    byte POLF3       :1;                                       /* First Input Capture Polarity Status 3 */
    byte             :1; 
    byte             :1; 
    byte             :1; 
    byte MCZF        :1;                                       /* Modulus Counter Underflow Flag */
  } Bits;
  struct {
    byte grpPOLF :4;
    byte         :1;
    byte         :1;
    byte         :1;
    byte         :1;
  } MergedBits;
} MCFLGSTR;
extern volatile MCFLGSTR _MCFLG @(REG_BASE + 0x00000067UL);
#define MCFLG                           _MCFLG.Byte
#define MCFLG_POLF0                     _MCFLG.Bits.POLF0
#define MCFLG_POLF1                     _MCFLG.Bits.POLF1
#define MCFLG_POLF2                     _MCFLG.Bits.POLF2
#define MCFLG_POLF3                     _MCFLG.Bits.POLF3
#define MCFLG_MCZF                      _MCFLG.Bits.MCZF
#define MCFLG_POLF                      _MCFLG.MergedBits.grpPOLF

#define MCFLG_POLF0_MASK                1U
#define MCFLG_POLF1_MASK                2U
#define MCFLG_POLF2_MASK                4U
#define MCFLG_POLF3_MASK                8U
#define MCFLG_MCZF_MASK                 128U
#define MCFLG_POLF_MASK                 15U
#define MCFLG_POLF_BITNUM               0U


/*** ICPAR - Input Control Pulse Accumulator Register; 0x00000068 ***/
typedef union {
  byte Byte;
  struct {
    byte PA0EN       :1;                                       /* 8-Bit Pulse Accumulator 0 Enable */
    byte PA1EN       :1;                                       /* 8-Bit Pulse Accumulator 1 Enable */
    byte PA2EN       :1;                                       /* 8-Bit Pulse Accumulator 2 Enable */
    byte PA3EN       :1;                                       /* 8-Bit Pulse Accumulator 3 Enable */
    byte             :1; 
    byte             :1; 
    byte             :1; 
    byte             :1; 
  } Bits;
} ICPARSTR;
extern volatile ICPARSTR _ICPAR @(REG_BASE + 0x00000068UL);
#define ICPAR                           _ICPAR.Byte
#define ICPAR_PA0EN                     _ICPAR.Bits.PA0EN
#define ICPAR_PA1EN                     _ICPAR.Bits.PA1EN
#define ICPAR_PA2EN                     _ICPAR.Bits.PA2EN
#define ICPAR_PA3EN                     _ICPAR.Bits.PA3EN

#define ICPAR_PA0EN_MASK                1U
#define ICPAR_PA1EN_MASK                2U
#define ICPAR_PA2EN_MASK                4U
#define ICPAR_PA3EN_MASK                8U


/*** DLYCT - Delay Counter Control Register; 0x00000069 ***/
typedef union {
  byte Byte;
  struct {
    byte DLY0        :1;                                       /* Delay Counter Select 0 */
    byte DLY1        :1;                                       /* Delay Counter Select 1 */
    byte             :1; 
    byte             :1; 
    byte             :1; 
    byte             :1; 
    byte             :1; 
    byte             :1; 
  } Bits;
  struct {
    byte grpDLY  :2;
    byte         :1;
    byte         :1;
    byte         :1;
    byte         :1;
    byte         :1;
    byte         :1;
  } MergedBits;
} DLYCTSTR;
extern volatile DLYCTSTR _DLYCT @(REG_BASE + 0x00000069UL);
#define DLYCT                           _DLYCT.Byte
#define DLYCT_DLY0                      _DLYCT.Bits.DLY0
#define DLYCT_DLY1                      _DLYCT.Bits.DLY1
#define DLYCT_DLY                       _DLYCT.MergedBits.grpDLY

#define DLYCT_DLY0_MASK                 1U
#define DLYCT_DLY1_MASK                 2U
#define DLYCT_DLY_MASK                  3U
#define DLYCT_DLY_BITNUM                0U


/*** ICOVW - Input Control Overwrite Register; 0x0000006A ***/
typedef union {
  byte Byte;
  struct {
    byte NOVW0       :1;                                       /* No Input Capture Overwrite 0 */
    byte NOVW1       :1;                                       /* No Input Capture Overwrite 1 */
    byte NOVW2       :1;                                       /* No Input Capture Overwrite 2 */
    byte NOVW3       :1;                                       /* No Input Capture Overwrite 3 */
    byte NOVW4       :1;                                       /* No Input Capture Overwrite 4 */
    byte NOVW5       :1;                                       /* No Input Capture Overwrite 5 */
    byte NOVW6       :1;                                       /* No Input Capture Overwrite 6 */
    byte NOVW7       :1;                                       /* No Input Capture Overwrite 7 */
  } Bits;
} ICOVWSTR;
extern volatile ICOVWSTR _ICOVW @(REG_BASE + 0x0000006AUL);
#define ICOVW                           _ICOVW.Byte
#define ICOVW_NOVW0                     _ICOVW.Bits.NOVW0
#define ICOVW_NOVW1                     _ICOVW.Bits.NOVW1
#define ICOVW_NOVW2                     _ICOVW.Bits.NOVW2
#define ICOVW_NOVW3                     _ICOVW.Bits.NOVW3
#define ICOVW_NOVW4                     _ICOVW.Bits.NOVW4
#define ICOVW_NOVW5                     _ICOVW.Bits.NOVW5
#define ICOVW_NOVW6                     _ICOVW.Bits.NOVW6
#define ICOVW_NOVW7                     _ICOVW.Bits.NOVW7

#define ICOVW_NOVW0_MASK                1U
#define ICOVW_NOVW1_MASK                2U
#define ICOVW_NOVW2_MASK                4U
#define ICOVW_NOVW3_MASK                8U
#define ICOVW_NOVW4_MASK                16U
#define ICOVW_NOVW5_MASK                32U
#define ICOVW_NOVW6_MASK                64U
#define ICOVW_NOVW7_MASK                128U


/*** ICSYS - Input Control System Control Register; 0x0000006B ***/
typedef union {
  byte Byte;
  struct {
    byte LATQ        :1;                                       /* Input Control Latch or Queue Mode Enable */
    byte BUFEN       :1;                                       /* IC Buffer Enable */
    byte PACMX       :1;                                       /* 8-Bit Pulse Accumulators Maximum Count */
    byte TFMOD       :1;                                       /* Timer Flag-setting Mode */
    byte SH04        :1;                                       /* Share Input action of Input Capture Channels 0 and 4 */
    byte SH15        :1;                                       /* Share Input action of Input Capture Channels 1 and 5 */
    byte SH26        :1;                                       /* Share Input action of Input Capture Channels 2 and 6 */
    byte SH37        :1;                                       /* Share Input action of Input Capture Channels 3 and 7 */
  } Bits;
} ICSYSSTR;
extern volatile ICSYSSTR _ICSYS @(REG_BASE + 0x0000006BUL);
#define ICSYS                           _ICSYS.Byte
#define ICSYS_LATQ                      _ICSYS.Bits.LATQ
#define ICSYS_BUFEN                     _ICSYS.Bits.BUFEN
#define ICSYS_PACMX                     _ICSYS.Bits.PACMX
#define ICSYS_TFMOD                     _ICSYS.Bits.TFMOD
#define ICSYS_SH04                      _ICSYS.Bits.SH04
#define ICSYS_SH15                      _ICSYS.Bits.SH15
#define ICSYS_SH26                      _ICSYS.Bits.SH26
#define ICSYS_SH37                      _ICSYS.Bits.SH37

#define ICSYS_LATQ_MASK                 1U
#define ICSYS_BUFEN_MASK                2U
#define ICSYS_PACMX_MASK                4U
#define ICSYS_TFMOD_MASK                8U
#define ICSYS_SH04_MASK                 16U
#define ICSYS_SH15_MASK                 32U
#define ICSYS_SH26_MASK                 64U
#define ICSYS_SH37_MASK                 128U


/*** PBCTL - 16-Bit Pulse Accumulator B Control Register; 0x00000070 ***/
typedef union {
  byte Byte;
  struct {
    byte             :1; 
    byte PBOVI       :1;                                       /* Pulse Accumulator B Overflow Interrupt enable */
    byte             :1; 
    byte             :1; 
    byte             :1; 
    byte             :1; 
    byte PBEN        :1;                                       /* Pulse Accumulator B System Enable */
    byte             :1; 
  } Bits;
} PBCTLSTR;
extern volatile PBCTLSTR _PBCTL @(REG_BASE + 0x00000070UL);
#define PBCTL                           _PBCTL.Byte
#define PBCTL_PBOVI                     _PBCTL.Bits.PBOVI
#define PBCTL_PBEN                      _PBCTL.Bits.PBEN

#define PBCTL_PBOVI_MASK                2U
#define PBCTL_PBEN_MASK                 64U


/*** PBFLG - Pulse Accumulator B Flag Register; 0x00000071 ***/
typedef union {
  byte Byte;
  struct {
    byte             :1; 
    byte PBOVF       :1;                                       /* Pulse Accumulator B Overflow Flag */
    byte             :1; 
    byte             :1; 
    byte             :1; 
    byte             :1; 
    byte             :1; 
    byte             :1; 
  } Bits;
} PBFLGSTR;
extern volatile PBFLGSTR _PBFLG @(REG_BASE + 0x00000071UL);
#define PBFLG                           _PBFLG.Byte
#define PBFLG_PBOVF                     _PBFLG.Bits.PBOVF

#define PBFLG_PBOVF_MASK                2U


/*** PA32H - 8-Bit Pulse Accumulators Holding 32 Register; 0x00000072 ***/
typedef union {
  word Word;
   /* Overlapped registers: */
  struct {
    /*** PA3H - 8-Bit Pulse Accumulators Holding 3 Register; 0x00000072 ***/
    union {
      byte Byte;
      struct {
        byte BIT0        :1;                                       /* Pulse Accumulator Bit 0 */
        byte BIT1        :1;                                       /* Pulse Accumulator Bit 1 */
        byte BIT2        :1;                                       /* Pulse Accumulator Bit 2 */
        byte BIT3        :1;                                       /* Pulse Accumulator Bit 3 */
        byte BIT4        :1;                                       /* Pulse Accumulator Bit 4 */
        byte BIT5        :1;                                       /* Pulse Accumulator Bit 5 */
        byte BIT6        :1;                                       /* Pulse Accumulator Bit 6 */
        byte BIT7        :1;                                       /* Pulse Accumulator Bit 7 */
      } Bits;
    } PA3HSTR;
    #define PA3H                        _PA32H.Overlap_STR.PA3HSTR.Byte
    #define PA3H_BIT0                   _PA32H.Overlap_STR.PA3HSTR.Bits.BIT0
    #define PA3H_BIT1                   _PA32H.Overlap_STR.PA3HSTR.Bits.BIT1
    #define PA3H_BIT2                   _PA32H.Overlap_STR.PA3HSTR.Bits.BIT2
    #define PA3H_BIT3                   _PA32H.Overlap_STR.PA3HSTR.Bits.BIT3
    #define PA3H_BIT4                   _PA32H.Overlap_STR.PA3HSTR.Bits.BIT4
    #define PA3H_BIT5                   _PA32H.Overlap_STR.PA3HSTR.Bits.BIT5
    #define PA3H_BIT6                   _PA32H.Overlap_STR.PA3HSTR.Bits.BIT6
    #define PA3H_BIT7                   _PA32H.Overlap_STR.PA3HSTR.Bits.BIT7
    
    #define PA3H_BIT0_MASK              1U
    #define PA3H_BIT1_MASK              2U
    #define PA3H_BIT2_MASK              4U
    #define PA3H_BIT3_MASK              8U
    #define PA3H_BIT4_MASK              16U
    #define PA3H_BIT5_MASK              32U
    #define PA3H_BIT6_MASK              64U
    #define PA3H_BIT7_MASK              128U
    

    /*** PA2H - 8-Bit Pulse Accumulators Holding 2 Register; 0x00000073 ***/
    union {
      byte Byte;
      struct {
        byte BIT0        :1;                                       /* Pulse Accumulator Bit 0 */
        byte BIT1        :1;                                       /* Pulse Accumulator Bit 1 */
        byte BIT2        :1;                                       /* Pulse Accumulator Bit 2 */
        byte BIT3        :1;                                       /* Pulse Accumulator Bit 3 */
        byte BIT4        :1;                                       /* Pulse Accumulator Bit 4 */
        byte BIT5        :1;                                       /* Pulse Accumulator Bit 5 */
        byte BIT6        :1;                                       /* Pulse Accumulator Bit 6 */
        byte BIT7        :1;                                       /* Pulse Accumulator Bit 7 */
      } Bits;
    } PA2HSTR;
    #define PA2H                        _PA32H.Overlap_STR.PA2HSTR.Byte
    #define PA2H_BIT0                   _PA32H.Overlap_STR.PA2HSTR.Bits.BIT0
    #define PA2H_BIT1                   _PA32H.Overlap_STR.PA2HSTR.Bits.BIT1
    #define PA2H_BIT2                   _PA32H.Overlap_STR.PA2HSTR.Bits.BIT2
    #define PA2H_BIT3                   _PA32H.Overlap_STR.PA2HSTR.Bits.BIT3
    #define PA2H_BIT4                   _PA32H.Overlap_STR.PA2HSTR.Bits.BIT4
    #define PA2H_BIT5                   _PA32H.Overlap_STR.PA2HSTR.Bits.BIT5
    #define PA2H_BIT6                   _PA32H.Overlap_STR.PA2HSTR.Bits.BIT6
    #define PA2H_BIT7                   _PA32H.Overlap_STR.PA2HSTR.Bits.BIT7
    
    #define PA2H_BIT0_MASK              1U
    #define PA2H_BIT1_MASK              2U
    #define PA2H_BIT2_MASK              4U
    #define PA2H_BIT3_MASK              8U
    #define PA2H_BIT4_MASK              16U
    #define PA2H_BIT5_MASK              32U
    #define PA2H_BIT6_MASK              64U
    #define PA2H_BIT7_MASK              128U
    
  } Overlap_STR;

  struct {
    word BIT0        :1;                                       /* Pulse Accumulator Bit 0 */
    word BIT1        :1;                                       /* Pulse Accumulator Bit 1 */
    word BIT2        :1;                                       /* Pulse Accumulator Bit 2 */
    word BIT3        :1;                                       /* Pulse Accumulator Bit 3 */
    word BIT4        :1;                                       /* Pulse Accumulator Bit 4 */
    word BIT5        :1;                                       /* Pulse Accumulator Bit 5 */
    word BIT6        :1;                                       /* Pulse Accumulator Bit 6 */
    word BIT7        :1;                                       /* Pulse Accumulator Bit 7 */
    word BIT8        :1;                                       /* Pulse Accumulator Bit 8 */
    word BIT9        :1;                                       /* Pulse Accumulator Bit 9 */
    word BIT10       :1;                                       /* Pulse Accumulator Bit 10 */
    word BIT11       :1;                                       /* Pulse Accumulator Bit 11 */
    word BIT12       :1;                                       /* Pulse Accumulator Bit 12 */
    word BIT13       :1;                                       /* Pulse Accumulator Bit 13 */
    word BIT14       :1;                                       /* Pulse Accumulator Bit 14 */
    word BIT15       :1;                                       /* Pulse Accumulator Bit 15 */
  } Bits;
} PA32HSTR;
extern volatile PA32HSTR _PA32H @(REG_BASE + 0x00000072UL);
#define PA32H                           _PA32H.Word
#define PA32H_BIT0                      _PA32H.Bits.BIT0
#define PA32H_BIT1                      _PA32H.Bits.BIT1
#define PA32H_BIT2                      _PA32H.Bits.BIT2
#define PA32H_BIT3                      _PA32H.Bits.BIT3
#define PA32H_BIT4                      _PA32H.Bits.BIT4
#define PA32H_BIT5                      _PA32H.Bits.BIT5
#define PA32H_BIT6                      _PA32H.Bits.BIT6
#define PA32H_BIT7                      _PA32H.Bits.BIT7
#define PA32H_BIT8                      _PA32H.Bits.BIT8
#define PA32H_BIT9                      _PA32H.Bits.BIT9
#define PA32H_BIT10                     _PA32H.Bits.BIT10
#define PA32H_BIT11                     _PA32H.Bits.BIT11
#define PA32H_BIT12                     _PA32H.Bits.BIT12
#define PA32H_BIT13                     _PA32H.Bits.BIT13
#define PA32H_BIT14                     _PA32H.Bits.BIT14
#define PA32H_BIT15                     _PA32H.Bits.BIT15

#define PA32H_BIT0_MASK                 1U
#define PA32H_BIT1_MASK                 2U
#define PA32H_BIT2_MASK                 4U
#define PA32H_BIT3_MASK                 8U
#define PA32H_BIT4_MASK                 16U
#define PA32H_BIT5_MASK                 32U
#define PA32H_BIT6_MASK                 64U
#define PA32H_BIT7_MASK                 128U
#define PA32H_BIT8_MASK                 256U
#define PA32H_BIT9_MASK                 512U
#define PA32H_BIT10_MASK                1024U
#define PA32H_BIT11_MASK                2048U
#define PA32H_BIT12_MASK                4096U
#define PA32H_BIT13_MASK                8192U
#define PA32H_BIT14_MASK                16384U
#define PA32H_BIT15_MASK                32768U


/*** PA10H - 8-Bit Pulse Accumulators Holding 10 Register; 0x00000074 ***/
typedef union {
  word Word;
   /* Overlapped registers: */
  struct {
    /*** PA1H - 8-Bit Pulse Accumulators Holding 1 Register; 0x00000074 ***/
    union {
      byte Byte;
      struct {
        byte BIT0        :1;                                       /* Pulse Accumulator Bit 0 */
        byte BIT1        :1;                                       /* Pulse Accumulator Bit 1 */
        byte BIT2        :1;                                       /* Pulse Accumulator Bit 2 */
        byte BIT3        :1;                                       /* Pulse Accumulator Bit 3 */
        byte BIT4        :1;                                       /* Pulse Accumulator Bit 4 */
        byte BIT5        :1;                                       /* Pulse Accumulator Bit 5 */
        byte BIT6        :1;                                       /* Pulse Accumulator Bit 6 */
        byte BIT7        :1;                                       /* Pulse Accumulator Bit 7 */
      } Bits;
    } PA1HSTR;
    #define PA1H                        _PA10H.Overlap_STR.PA1HSTR.Byte
    #define PA1H_BIT0                   _PA10H.Overlap_STR.PA1HSTR.Bits.BIT0
    #define PA1H_BIT1                   _PA10H.Overlap_STR.PA1HSTR.Bits.BIT1
    #define PA1H_BIT2                   _PA10H.Overlap_STR.PA1HSTR.Bits.BIT2
    #define PA1H_BIT3                   _PA10H.Overlap_STR.PA1HSTR.Bits.BIT3
    #define PA1H_BIT4                   _PA10H.Overlap_STR.PA1HSTR.Bits.BIT4
    #define PA1H_BIT5                   _PA10H.Overlap_STR.PA1HSTR.Bits.BIT5
    #define PA1H_BIT6                   _PA10H.Overlap_STR.PA1HSTR.Bits.BIT6
    #define PA1H_BIT7                   _PA10H.Overlap_STR.PA1HSTR.Bits.BIT7
    
    #define PA1H_BIT0_MASK              1U
    #define PA1H_BIT1_MASK              2U
    #define PA1H_BIT2_MASK              4U
    #define PA1H_BIT3_MASK              8U
    #define PA1H_BIT4_MASK              16U
    #define PA1H_BIT5_MASK              32U
    #define PA1H_BIT6_MASK              64U
    #define PA1H_BIT7_MASK              128U
    

    /*** PA0H - 8-Bit Pulse Accumulators Holding 0 Register; 0x00000075 ***/
    union {
      byte Byte;
      struct {
        byte BIT0        :1;                                       /* Pulse Accumulator Bit 0 */
        byte BIT1        :1;                                       /* Pulse Accumulator Bit 1 */
        byte BIT2        :1;                                       /* Pulse Accumulator Bit 2 */
        byte BIT3        :1;                                       /* Pulse Accumulator Bit 3 */
        byte BIT4        :1;                                       /* Pulse Accumulator Bit 4 */
        byte BIT5        :1;                                       /* Pulse Accumulator Bit 5 */
        byte BIT6        :1;                                       /* Pulse Accumulator Bit 6 */
        byte BIT7        :1;                                       /* Pulse Accumulator Bit 7 */
      } Bits;
    } PA0HSTR;
    #define PA0H                        _PA10H.Overlap_STR.PA0HSTR.Byte
    #define PA0H_BIT0                   _PA10H.Overlap_STR.PA0HSTR.Bits.BIT0
    #define PA0H_BIT1                   _PA10H.Overlap_STR.PA0HSTR.Bits.BIT1
    #define PA0H_BIT2                   _PA10H.Overlap_STR.PA0HSTR.Bits.BIT2
    #define PA0H_BIT3                   _PA10H.Overlap_STR.PA0HSTR.Bits.BIT3
    #define PA0H_BIT4                   _PA10H.Overlap_STR.PA0HSTR.Bits.BIT4
    #define PA0H_BIT5                   _PA10H.Overlap_STR.PA0HSTR.Bits.BIT5
    #define PA0H_BIT6                   _PA10H.Overlap_STR.PA0HSTR.Bits.BIT6
    #define PA0H_BIT7                   _PA10H.Overlap_STR.PA0HSTR.Bits.BIT7
    
    #define PA0H_BIT0_MASK              1U
    #define PA0H_BIT1_MASK              2U
    #define PA0H_BIT2_MASK              4U
    #define PA0H_BIT3_MASK              8U
    #define PA0H_BIT4_MASK              16U
    #define PA0H_BIT5_MASK              32U
    #define PA0H_BIT6_MASK              64U
    #define PA0H_BIT7_MASK              128U
    
  } Overlap_STR;

  struct {
    word BIT0        :1;                                       /* Pulse Accumulator Bit 0 */
    word BIT1        :1;                                       /* Pulse Accumulator Bit 1 */
    word BIT2        :1;                                       /* Pulse Accumulator Bit 2 */
    word BIT3        :1;                                       /* Pulse Accumulator Bit 3 */
    word BIT4        :1;                                       /* Pulse Accumulator Bit 4 */
    word BIT5        :1;                                       /* Pulse Accumulator Bit 5 */
    word BIT6        :1;                                       /* Pulse Accumulator Bit 6 */
    word BIT7        :1;                                       /* Pulse Accumulator Bit 7 */
    word BIT8        :1;                                       /* Pulse Accumulator Bit 8 */
    word BIT9        :1;                                       /* Pulse Accumulator Bit 9 */
    word BIT10       :1;                                       /* Pulse Accumulator Bit 10 */
    word BIT11       :1;                                       /* Pulse Accumulator Bit 11 */
    word BIT12       :1;                                       /* Pulse Accumulator Bit 12 */
    word BIT13       :1;                                       /* Pulse Accumulator Bit 13 */
    word BIT14       :1;                                       /* Pulse Accumulator Bit 14 */
    word BIT15       :1;                                       /* Pulse Accumulator Bit 15 */
  } Bits;
} PA10HSTR;
extern volatile PA10HSTR _PA10H @(REG_BASE + 0x00000074UL);
#define PA10H                           _PA10H.Word
#define PA10H_BIT0                      _PA10H.Bits.BIT0
#define PA10H_BIT1                      _PA10H.Bits.BIT1
#define PA10H_BIT2                      _PA10H.Bits.BIT2
#define PA10H_BIT3                      _PA10H.Bits.BIT3
#define PA10H_BIT4                      _PA10H.Bits.BIT4
#define PA10H_BIT5                      _PA10H.Bits.BIT5
#define PA10H_BIT6                      _PA10H.Bits.BIT6
#define PA10H_BIT7                      _PA10H.Bits.BIT7
#define PA10H_BIT8                      _PA10H.Bits.BIT8
#define PA10H_BIT9                      _PA10H.Bits.BIT9
#define PA10H_BIT10                     _PA10H.Bits.BIT10
#define PA10H_BIT11                     _PA10H.Bits.BIT11
#define PA10H_BIT12                     _PA10H.Bits.BIT12
#define PA10H_BIT13                     _PA10H.Bits.BIT13
#define PA10H_BIT14                     _PA10H.Bits.BIT14
#define PA10H_BIT15                     _PA10H.Bits.BIT15

#define PA10H_BIT0_MASK                 1U
#define PA10H_BIT1_MASK                 2U
#define PA10H_BIT2_MASK                 4U
#define PA10H_BIT3_MASK                 8U
#define PA10H_BIT4_MASK                 16U
#define PA10H_BIT5_MASK                 32U
#define PA10H_BIT6_MASK                 64U
#define PA10H_BIT7_MASK                 128U
#define PA10H_BIT8_MASK                 256U
#define PA10H_BIT9_MASK                 512U
#define PA10H_BIT10_MASK                1024U
#define PA10H_BIT11_MASK                2048U
#define PA10H_BIT12_MASK                4096U
#define PA10H_BIT13_MASK                8192U
#define PA10H_BIT14_MASK                16384U
#define PA10H_BIT15_MASK                32768U


/*** MCCNT - Modulus Down-Counter Count Register; 0x00000076 ***/
typedef union {
  word Word;
   /* Overlapped registers: */
  struct {
    /*** MCCNThi - Modulus Down-Counter Count Register High; 0x00000076 ***/
    union {
      byte Byte;
    } MCCNThiSTR;
    #define MCCNThi                     _MCCNT.Overlap_STR.MCCNThiSTR.Byte
    

    /*** MCCNTlo - Modulus Down-Counter Count Register Low; 0x00000077 ***/
    union {
      byte Byte;
    } MCCNTloSTR;
    #define MCCNTlo                     _MCCNT.Overlap_STR.MCCNTloSTR.Byte
    
  } Overlap_STR;

} MCCNTSTR;
extern volatile MCCNTSTR _MCCNT @(REG_BASE + 0x00000076UL);
#define MCCNT                           _MCCNT.Word


/*** TC0H - Timer Input Capture Holding Registers 0; 0x00000078 ***/
typedef union {
  word Word;
   /* Overlapped registers: */
  struct {
    /*** TC0Hhi - Timer Input Capture Holding Registers 0 High; 0x00000078 ***/
    union {
      byte Byte;
      struct {
        byte BIT8        :1;                                       /* Timer Input Capture Holding Bit 8 */
        byte BIT9        :1;                                       /* Timer Input Capture Holding Bit 9 */
        byte BIT10       :1;                                       /* Timer Input Capture Holding Bit 10 */
        byte BIT11       :1;                                       /* Timer Input Capture Holding Bit 11 */
        byte BIT12       :1;                                       /* Timer Input Capture Holding Bit 12 */
        byte BIT13       :1;                                       /* Timer Input Capture Holding Bit 13 */
        byte BIT14       :1;                                       /* Timer Input Capture Holding Bit 14 */
        byte BIT15       :1;                                       /* Timer Input Capture Holding Bit 15 */
      } Bits;
    } TC0HhiSTR;
    #define TC0Hhi                      _TC0H.Overlap_STR.TC0HhiSTR.Byte
    #define TC0Hhi_BIT8                 _TC0H.Overlap_STR.TC0HhiSTR.Bits.BIT8
    #define TC0Hhi_BIT9                 _TC0H.Overlap_STR.TC0HhiSTR.Bits.BIT9
    #define TC0Hhi_BIT10                _TC0H.Overlap_STR.TC0HhiSTR.Bits.BIT10
    #define TC0Hhi_BIT11                _TC0H.Overlap_STR.TC0HhiSTR.Bits.BIT11
    #define TC0Hhi_BIT12                _TC0H.Overlap_STR.TC0HhiSTR.Bits.BIT12
    #define TC0Hhi_BIT13                _TC0H.Overlap_STR.TC0HhiSTR.Bits.BIT13
    #define TC0Hhi_BIT14                _TC0H.Overlap_STR.TC0HhiSTR.Bits.BIT14
    #define TC0Hhi_BIT15                _TC0H.Overlap_STR.TC0HhiSTR.Bits.BIT15
    
    #define TC0Hhi_BIT8_MASK            1U
    #define TC0Hhi_BIT9_MASK            2U
    #define TC0Hhi_BIT10_MASK           4U
    #define TC0Hhi_BIT11_MASK           8U
    #define TC0Hhi_BIT12_MASK           16U
    #define TC0Hhi_BIT13_MASK           32U
    #define TC0Hhi_BIT14_MASK           64U
    #define TC0Hhi_BIT15_MASK           128U
    

    /*** TC0Hlo - Timer Input Capture Holding Registers 0 Low; 0x00000079 ***/
    union {
      byte Byte;
      struct {
        byte BIT0        :1;                                       /* Timer Input Capture Holding Bit 0 */
        byte BIT1        :1;                                       /* Timer Input Capture Holding Bit 1 */
        byte BIT2        :1;                                       /* Timer Input Capture Holding Bit 2 */
        byte BIT3        :1;                                       /* Timer Input Capture Holding Bit 3 */
        byte BIT4        :1;                                       /* Timer Input Capture Holding Bit 4 */
        byte BIT5        :1;                                       /* Timer Input Capture Holding Bit 5 */
        byte BIT6        :1;                                       /* Timer Input Capture Holding Bit 6 */
        byte BIT7        :1;                                       /* Timer Input Capture Holding Bit 7 */
      } Bits;
    } TC0HloSTR;
    #define TC0Hlo                      _TC0H.Overlap_STR.TC0HloSTR.Byte
    #define TC0Hlo_BIT0                 _TC0H.Overlap_STR.TC0HloSTR.Bits.BIT0
    #define TC0Hlo_BIT1                 _TC0H.Overlap_STR.TC0HloSTR.Bits.BIT1
    #define TC0Hlo_BIT2                 _TC0H.Overlap_STR.TC0HloSTR.Bits.BIT2
    #define TC0Hlo_BIT3                 _TC0H.Overlap_STR.TC0HloSTR.Bits.BIT3
    #define TC0Hlo_BIT4                 _TC0H.Overlap_STR.TC0HloSTR.Bits.BIT4
    #define TC0Hlo_BIT5                 _TC0H.Overlap_STR.TC0HloSTR.Bits.BIT5
    #define TC0Hlo_BIT6                 _TC0H.Overlap_STR.TC0HloSTR.Bits.BIT6
    #define TC0Hlo_BIT7                 _TC0H.Overlap_STR.TC0HloSTR.Bits.BIT7
    
    #define TC0Hlo_BIT0_MASK            1U
    #define TC0Hlo_BIT1_MASK            2U
    #define TC0Hlo_BIT2_MASK            4U
    #define TC0Hlo_BIT3_MASK            8U
    #define TC0Hlo_BIT4_MASK            16U
    #define TC0Hlo_BIT5_MASK            32U
    #define TC0Hlo_BIT6_MASK            64U
    #define TC0Hlo_BIT7_MASK            128U
    
  } Overlap_STR;

  struct {
    word BIT0        :1;                                       /* Timer Input Capture Holding Bit 0 */
    word BIT1        :1;                                       /* Timer Input Capture Holding Bit 1 */
    word BIT2        :1;                                       /* Timer Input Capture Holding Bit 2 */
    word BIT3        :1;                                       /* Timer Input Capture Holding Bit 3 */
    word BIT4        :1;                                       /* Timer Input Capture Holding Bit 4 */
    word BIT5        :1;                                       /* Timer Input Capture Holding Bit 5 */
    word BIT6        :1;                                       /* Timer Input Capture Holding Bit 6 */
    word BIT7        :1;                                       /* Timer Input Capture Holding Bit 7 */
    word BIT8        :1;                                       /* Timer Input Capture Holding Bit 8 */
    word BIT9        :1;                                       /* Timer Input Capture Holding Bit 9 */
    word BIT10       :1;                                       /* Timer Input Capture Holding Bit 10 */
    word BIT11       :1;                                       /* Timer Input Capture Holding Bit 11 */
    word BIT12       :1;                                       /* Timer Input Capture Holding Bit 12 */
    word BIT13       :1;                                       /* Timer Input Capture Holding Bit 13 */
    word BIT14       :1;                                       /* Timer Input Capture Holding Bit 14 */
    word BIT15       :1;                                       /* Timer Input Capture Holding Bit 15 */
  } Bits;
} TC0HSTR;
extern volatile TC0HSTR _TC0H @(REG_BASE + 0x00000078UL);
#define TC0H                            _TC0H.Word
#define TC0H_BIT0                       _TC0H.Bits.BIT0
#define TC0H_BIT1                       _TC0H.Bits.BIT1
#define TC0H_BIT2                       _TC0H.Bits.BIT2
#define TC0H_BIT3                       _TC0H.Bits.BIT3
#define TC0H_BIT4                       _TC0H.Bits.BIT4
#define TC0H_BIT5                       _TC0H.Bits.BIT5
#define TC0H_BIT6                       _TC0H.Bits.BIT6
#define TC0H_BIT7                       _TC0H.Bits.BIT7
#define TC0H_BIT8                       _TC0H.Bits.BIT8
#define TC0H_BIT9                       _TC0H.Bits.BIT9
#define TC0H_BIT10                      _TC0H.Bits.BIT10
#define TC0H_BIT11                      _TC0H.Bits.BIT11
#define TC0H_BIT12                      _TC0H.Bits.BIT12
#define TC0H_BIT13                      _TC0H.Bits.BIT13
#define TC0H_BIT14                      _TC0H.Bits.BIT14
#define TC0H_BIT15                      _TC0H.Bits.BIT15

#define TC0H_BIT0_MASK                  1U
#define TC0H_BIT1_MASK                  2U
#define TC0H_BIT2_MASK                  4U
#define TC0H_BIT3_MASK                  8U
#define TC0H_BIT4_MASK                  16U
#define TC0H_BIT5_MASK                  32U
#define TC0H_BIT6_MASK                  64U
#define TC0H_BIT7_MASK                  128U
#define TC0H_BIT8_MASK                  256U
#define TC0H_BIT9_MASK                  512U
#define TC0H_BIT10_MASK                 1024U
#define TC0H_BIT11_MASK                 2048U
#define TC0H_BIT12_MASK                 4096U
#define TC0H_BIT13_MASK                 8192U
#define TC0H_BIT14_MASK                 16384U
#define TC0H_BIT15_MASK                 32768U


/*** TC1H - Timer Input Capture Holding Registers 1; 0x0000007A ***/
typedef union {
  word Word;
   /* Overlapped registers: */
  struct {
    /*** TC1Hhi - Timer Input Capture Holding Registers 1 High; 0x0000007A ***/
    union {
      byte Byte;
      struct {
        byte BIT8        :1;                                       /* Timer Input Capture Holding Bit 8 */
        byte BIT9        :1;                                       /* Timer Input Capture Holding Bit 9 */
        byte BIT10       :1;                                       /* Timer Input Capture Holding Bit 10 */
        byte BIT11       :1;                                       /* Timer Input Capture Holding Bit 11 */
        byte BIT12       :1;                                       /* Timer Input Capture Holding Bit 12 */
        byte BIT13       :1;                                       /* Timer Input Capture Holding Bit 13 */
        byte BIT14       :1;                                       /* Timer Input Capture Holding Bit 14 */
        byte BIT15       :1;                                       /* Timer Input Capture Holding Bit 15 */
      } Bits;
    } TC1HhiSTR;
    #define TC1Hhi                      _TC1H.Overlap_STR.TC1HhiSTR.Byte
    #define TC1Hhi_BIT8                 _TC1H.Overlap_STR.TC1HhiSTR.Bits.BIT8
    #define TC1Hhi_BIT9                 _TC1H.Overlap_STR.TC1HhiSTR.Bits.BIT9
    #define TC1Hhi_BIT10                _TC1H.Overlap_STR.TC1HhiSTR.Bits.BIT10
    #define TC1Hhi_BIT11                _TC1H.Overlap_STR.TC1HhiSTR.Bits.BIT11
    #define TC1Hhi_BIT12                _TC1H.Overlap_STR.TC1HhiSTR.Bits.BIT12
    #define TC1Hhi_BIT13                _TC1H.Overlap_STR.TC1HhiSTR.Bits.BIT13
    #define TC1Hhi_BIT14                _TC1H.Overlap_STR.TC1HhiSTR.Bits.BIT14
    #define TC1Hhi_BIT15                _TC1H.Overlap_STR.TC1HhiSTR.Bits.BIT15
    
    #define TC1Hhi_BIT8_MASK            1U
    #define TC1Hhi_BIT9_MASK            2U
    #define TC1Hhi_BIT10_MASK           4U
    #define TC1Hhi_BIT11_MASK           8U
    #define TC1Hhi_BIT12_MASK           16U
    #define TC1Hhi_BIT13_MASK           32U
    #define TC1Hhi_BIT14_MASK           64U
    #define TC1Hhi_BIT15_MASK           128U
    

    /*** TC1Hlo - Timer Input Capture Holding Registers 1 Low; 0x0000007B ***/
    union {
      byte Byte;
      struct {
        byte BIT0        :1;                                       /* Timer Input Capture Holding Bit 0 */
        byte BIT1        :1;                                       /* Timer Input Capture Holding Bit 1 */
        byte BIT2        :1;                                       /* Timer Input Capture Holding Bit 2 */
        byte BIT3        :1;                                       /* Timer Input Capture Holding Bit 3 */
        byte BIT4        :1;                                       /* Timer Input Capture Holding Bit 4 */
        byte BIT5        :1;                                       /* Timer Input Capture Holding Bit 5 */
        byte BIT6        :1;                                       /* Timer Input Capture Holding Bit 6 */
        byte BIT7        :1;                                       /* Timer Input Capture Holding Bit 7 */
      } Bits;
    } TC1HloSTR;
    #define TC1Hlo                      _TC1H.Overlap_STR.TC1HloSTR.Byte
    #define TC1Hlo_BIT0                 _TC1H.Overlap_STR.TC1HloSTR.Bits.BIT0
    #define TC1Hlo_BIT1                 _TC1H.Overlap_STR.TC1HloSTR.Bits.BIT1
    #define TC1Hlo_BIT2                 _TC1H.Overlap_STR.TC1HloSTR.Bits.BIT2
    #define TC1Hlo_BIT3                 _TC1H.Overlap_STR.TC1HloSTR.Bits.BIT3
    #define TC1Hlo_BIT4                 _TC1H.Overlap_STR.TC1HloSTR.Bits.BIT4
    #define TC1Hlo_BIT5                 _TC1H.Overlap_STR.TC1HloSTR.Bits.BIT5
    #define TC1Hlo_BIT6                 _TC1H.Overlap_STR.TC1HloSTR.Bits.BIT6
    #define TC1Hlo_BIT7                 _TC1H.Overlap_STR.TC1HloSTR.Bits.BIT7
    
    #define TC1Hlo_BIT0_MASK            1U
    #define TC1Hlo_BIT1_MASK            2U
    #define TC1Hlo_BIT2_MASK            4U
    #define TC1Hlo_BIT3_MASK            8U
    #define TC1Hlo_BIT4_MASK            16U
    #define TC1Hlo_BIT5_MASK            32U
    #define TC1Hlo_BIT6_MASK            64U
    #define TC1Hlo_BIT7_MASK            128U
    
  } Overlap_STR;

  struct {
    word BIT0        :1;                                       /* Timer Input Capture Holding Bit 0 */
    word BIT1        :1;                                       /* Timer Input Capture Holding Bit 1 */
    word BIT2        :1;                                       /* Timer Input Capture Holding Bit 2 */
    word BIT3        :1;                                       /* Timer Input Capture Holding Bit 3 */
    word BIT4        :1;                                       /* Timer Input Capture Holding Bit 4 */
    word BIT5        :1;                                       /* Timer Input Capture Holding Bit 5 */
    word BIT6        :1;                                       /* Timer Input Capture Holding Bit 6 */
    word BIT7        :1;                                       /* Timer Input Capture Holding Bit 7 */
    word BIT8        :1;                                       /* Timer Input Capture Holding Bit 8 */
    word BIT9        :1;                                       /* Timer Input Capture Holding Bit 9 */
    word BIT10       :1;                                       /* Timer Input Capture Holding Bit 10 */
    word BIT11       :1;                                       /* Timer Input Capture Holding Bit 11 */
    word BIT12       :1;                                       /* Timer Input Capture Holding Bit 12 */
    word BIT13       :1;                                       /* Timer Input Capture Holding Bit 13 */
    word BIT14       :1;                                       /* Timer Input Capture Holding Bit 14 */
    word BIT15       :1;                                       /* Timer Input Capture Holding Bit 15 */
  } Bits;
} TC1HSTR;
extern volatile TC1HSTR _TC1H @(REG_BASE + 0x0000007AUL);
#define TC1H                            _TC1H.Word
#define TC1H_BIT0                       _TC1H.Bits.BIT0
#define TC1H_BIT1                       _TC1H.Bits.BIT1
#define TC1H_BIT2                       _TC1H.Bits.BIT2
#define TC1H_BIT3                       _TC1H.Bits.BIT3
#define TC1H_BIT4                       _TC1H.Bits.BIT4
#define TC1H_BIT5                       _TC1H.Bits.BIT5
#define TC1H_BIT6                       _TC1H.Bits.BIT6
#define TC1H_BIT7                       _TC1H.Bits.BIT7
#define TC1H_BIT8                       _TC1H.Bits.BIT8
#define TC1H_BIT9                       _TC1H.Bits.BIT9
#define TC1H_BIT10                      _TC1H.Bits.BIT10
#define TC1H_BIT11                      _TC1H.Bits.BIT11
#define TC1H_BIT12                      _TC1H.Bits.BIT12
#define TC1H_BIT13                      _TC1H.Bits.BIT13
#define TC1H_BIT14                      _TC1H.Bits.BIT14
#define TC1H_BIT15                      _TC1H.Bits.BIT15

#define TC1H_BIT0_MASK                  1U
#define TC1H_BIT1_MASK                  2U
#define TC1H_BIT2_MASK                  4U
#define TC1H_BIT3_MASK                  8U
#define TC1H_BIT4_MASK                  16U
#define TC1H_BIT5_MASK                  32U
#define TC1H_BIT6_MASK                  64U
#define TC1H_BIT7_MASK                  128U
#define TC1H_BIT8_MASK                  256U
#define TC1H_BIT9_MASK                  512U
#define TC1H_BIT10_MASK                 1024U
#define TC1H_BIT11_MASK                 2048U
#define TC1H_BIT12_MASK                 4096U
#define TC1H_BIT13_MASK                 8192U
#define TC1H_BIT14_MASK                 16384U
#define TC1H_BIT15_MASK                 32768U


/*** TC2H - Timer Input Capture Holding Registers 2; 0x0000007C ***/
typedef union {
  word Word;
   /* Overlapped registers: */
  struct {
    /*** TC2Hhi - Timer Input Capture Holding Registers 2 High; 0x0000007C ***/
    union {
      byte Byte;
      struct {
        byte BIT8        :1;                                       /* Timer Input Capture Holding Bit 8 */
        byte BIT9        :1;                                       /* Timer Input Capture Holding Bit 9 */
        byte BIT10       :1;                                       /* Timer Input Capture Holding Bit 10 */
        byte BIT11       :1;                                       /* Timer Input Capture Holding Bit 11 */
        byte BIT12       :1;                                       /* Timer Input Capture Holding Bit 12 */
        byte BIT13       :1;                                       /* Timer Input Capture Holding Bit 13 */
        byte BIT14       :1;                                       /* Timer Input Capture Holding Bit 14 */
        byte BIT15       :1;                                       /* Timer Input Capture Holding Bit 15 */
      } Bits;
    } TC2HhiSTR;
    #define TC2Hhi                      _TC2H.Overlap_STR.TC2HhiSTR.Byte
    #define TC2Hhi_BIT8                 _TC2H.Overlap_STR.TC2HhiSTR.Bits.BIT8
    #define TC2Hhi_BIT9                 _TC2H.Overlap_STR.TC2HhiSTR.Bits.BIT9
    #define TC2Hhi_BIT10                _TC2H.Overlap_STR.TC2HhiSTR.Bits.BIT10
    #define TC2Hhi_BIT11                _TC2H.Overlap_STR.TC2HhiSTR.Bits.BIT11
    #define TC2Hhi_BIT12                _TC2H.Overlap_STR.TC2HhiSTR.Bits.BIT12
    #define TC2Hhi_BIT13                _TC2H.Overlap_STR.TC2HhiSTR.Bits.BIT13
    #define TC2Hhi_BIT14                _TC2H.Overlap_STR.TC2HhiSTR.Bits.BIT14
    #define TC2Hhi_BIT15                _TC2H.Overlap_STR.TC2HhiSTR.Bits.BIT15
    
    #define TC2Hhi_BIT8_MASK            1U
    #define TC2Hhi_BIT9_MASK            2U
    #define TC2Hhi_BIT10_MASK           4U
    #define TC2Hhi_BIT11_MASK           8U
    #define TC2Hhi_BIT12_MASK           16U
    #define TC2Hhi_BIT13_MASK           32U
    #define TC2Hhi_BIT14_MASK           64U
    #define TC2Hhi_BIT15_MASK           128U
    

    /*** TC2Hlo - Timer Input Capture Holding Registers 2 Low; 0x0000007D ***/
    union {
      byte Byte;
      struct {
        byte BIT0        :1;                                       /* Timer Input Capture Holding Bit 0 */
        byte BIT1        :1;                                       /* Timer Input Capture Holding Bit 1 */
        byte BIT2        :1;                                       /* Timer Input Capture Holding Bit 2 */
        byte BIT3        :1;                                       /* Timer Input Capture Holding Bit 3 */
        byte BIT4        :1;                                       /* Timer Input Capture Holding Bit 4 */
        byte BIT5        :1;                                       /* Timer Input Capture Holding Bit 5 */
        byte BIT6        :1;                                       /* Timer Input Capture Holding Bit 6 */
        byte BIT7        :1;                                       /* Timer Input Capture Holding Bit 7 */
      } Bits;
    } TC2HloSTR;
    #define TC2Hlo                      _TC2H.Overlap_STR.TC2HloSTR.Byte
    #define TC2Hlo_BIT0                 _TC2H.Overlap_STR.TC2HloSTR.Bits.BIT0
    #define TC2Hlo_BIT1                 _TC2H.Overlap_STR.TC2HloSTR.Bits.BIT1
    #define TC2Hlo_BIT2                 _TC2H.Overlap_STR.TC2HloSTR.Bits.BIT2
    #define TC2Hlo_BIT3                 _TC2H.Overlap_STR.TC2HloSTR.Bits.BIT3
    #define TC2Hlo_BIT4                 _TC2H.Overlap_STR.TC2HloSTR.Bits.BIT4
    #define TC2Hlo_BIT5                 _TC2H.Overlap_STR.TC2HloSTR.Bits.BIT5
    #define TC2Hlo_BIT6                 _TC2H.Overlap_STR.TC2HloSTR.Bits.BIT6
    #define TC2Hlo_BIT7                 _TC2H.Overlap_STR.TC2HloSTR.Bits.BIT7
    
    #define TC2Hlo_BIT0_MASK            1U
    #define TC2Hlo_BIT1_MASK            2U
    #define TC2Hlo_BIT2_MASK            4U
    #define TC2Hlo_BIT3_MASK            8U
    #define TC2Hlo_BIT4_MASK            16U
    #define TC2Hlo_BIT5_MASK            32U
    #define TC2Hlo_BIT6_MASK            64U
    #define TC2Hlo_BIT7_MASK            128U
    
  } Overlap_STR;

  struct {
    word BIT0        :1;                                       /* Timer Input Capture Holding Bit 0 */
    word BIT1        :1;                                       /* Timer Input Capture Holding Bit 1 */
    word BIT2        :1;                                       /* Timer Input Capture Holding Bit 2 */
    word BIT3        :1;                                       /* Timer Input Capture Holding Bit 3 */
    word BIT4        :1;                                       /* Timer Input Capture Holding Bit 4 */
    word BIT5        :1;                                       /* Timer Input Capture Holding Bit 5 */
    word BIT6        :1;                                       /* Timer Input Capture Holding Bit 6 */
    word BIT7        :1;                                       /* Timer Input Capture Holding Bit 7 */
    word BIT8        :1;                                       /* Timer Input Capture Holding Bit 8 */
    word BIT9        :1;                                       /* Timer Input Capture Holding Bit 9 */
    word BIT10       :1;                                       /* Timer Input Capture Holding Bit 10 */
    word BIT11       :1;                                       /* Timer Input Capture Holding Bit 11 */
    word BIT12       :1;                                       /* Timer Input Capture Holding Bit 12 */
    word BIT13       :1;                                       /* Timer Input Capture Holding Bit 13 */
    word BIT14       :1;                                       /* Timer Input Capture Holding Bit 14 */
    word BIT15       :1;                                       /* Timer Input Capture Holding Bit 15 */
  } Bits;
} TC2HSTR;
extern volatile TC2HSTR _TC2H @(REG_BASE + 0x0000007CUL);
#define TC2H                            _TC2H.Word
#define TC2H_BIT0                       _TC2H.Bits.BIT0
#define TC2H_BIT1                       _TC2H.Bits.BIT1
#define TC2H_BIT2                       _TC2H.Bits.BIT2
#define TC2H_BIT3                       _TC2H.Bits.BIT3
#define TC2H_BIT4                       _TC2H.Bits.BIT4
#define TC2H_BIT5                       _TC2H.Bits.BIT5
#define TC2H_BIT6                       _TC2H.Bits.BIT6
#define TC2H_BIT7                       _TC2H.Bits.BIT7
#define TC2H_BIT8                       _TC2H.Bits.BIT8
#define TC2H_BIT9                       _TC2H.Bits.BIT9
#define TC2H_BIT10                      _TC2H.Bits.BIT10
#define TC2H_BIT11                      _TC2H.Bits.BIT11
#define TC2H_BIT12                      _TC2H.Bits.BIT12
#define TC2H_BIT13                      _TC2H.Bits.BIT13
#define TC2H_BIT14                      _TC2H.Bits.BIT14
#define TC2H_BIT15                      _TC2H.Bits.BIT15

#define TC2H_BIT0_MASK                  1U
#define TC2H_BIT1_MASK                  2U
#define TC2H_BIT2_MASK                  4U
#define TC2H_BIT3_MASK                  8U
#define TC2H_BIT4_MASK                  16U
#define TC2H_BIT5_MASK                  32U
#define TC2H_BIT6_MASK                  64U
#define TC2H_BIT7_MASK                  128U
#define TC2H_BIT8_MASK                  256U
#define TC2H_BIT9_MASK                  512U
#define TC2H_BIT10_MASK                 1024U
#define TC2H_BIT11_MASK                 2048U
#define TC2H_BIT12_MASK                 4096U
#define TC2H_BIT13_MASK                 8192U
#define TC2H_BIT14_MASK                 16384U
#define TC2H_BIT15_MASK                 32768U


/*** TC3H - Timer Input Capture Holding Registers 3; 0x0000007E ***/
typedef union {
  word Word;
   /* Overlapped registers: */
  struct {
    /*** TC3Hhi - Timer Input Capture Holding Registers 3 High; 0x0000007E ***/
    union {
      byte Byte;
      struct {
        byte BIT8        :1;                                       /* Timer Input Capture Holding Bit 8 */
        byte BIT9        :1;                                       /* Timer Input Capture Holding Bit 9 */
        byte BIT10       :1;                                       /* Timer Input Capture Holding Bit 10 */
        byte BIT11       :1;                                       /* Timer Input Capture Holding Bit 11 */
        byte BIT12       :1;                                       /* Timer Input Capture Holding Bit 12 */
        byte BIT13       :1;                                       /* Timer Input Capture Holding Bit 13 */
        byte BIT14       :1;                                       /* Timer Input Capture Holding Bit 14 */
        byte BIT15       :1;                                       /* Timer Input Capture Holding Bit 15 */
      } Bits;
    } TC3HhiSTR;
    #define TC3Hhi                      _TC3H.Overlap_STR.TC3HhiSTR.Byte
    #define TC3Hhi_BIT8                 _TC3H.Overlap_STR.TC3HhiSTR.Bits.BIT8
    #define TC3Hhi_BIT9                 _TC3H.Overlap_STR.TC3HhiSTR.Bits.BIT9
    #define TC3Hhi_BIT10                _TC3H.Overlap_STR.TC3HhiSTR.Bits.BIT10
    #define TC3Hhi_BIT11                _TC3H.Overlap_STR.TC3HhiSTR.Bits.BIT11
    #define TC3Hhi_BIT12                _TC3H.Overlap_STR.TC3HhiSTR.Bits.BIT12
    #define TC3Hhi_BIT13                _TC3H.Overlap_STR.TC3HhiSTR.Bits.BIT13
    #define TC3Hhi_BIT14                _TC3H.Overlap_STR.TC3HhiSTR.Bits.BIT14
    #define TC3Hhi_BIT15                _TC3H.Overlap_STR.TC3HhiSTR.Bits.BIT15
    
    #define TC3Hhi_BIT8_MASK            1U
    #define TC3Hhi_BIT9_MASK            2U
    #define TC3Hhi_BIT10_MASK           4U
    #define TC3Hhi_BIT11_MASK           8U
    #define TC3Hhi_BIT12_MASK           16U
    #define TC3Hhi_BIT13_MASK           32U
    #define TC3Hhi_BIT14_MASK           64U
    #define TC3Hhi_BIT15_MASK           128U
    

    /*** TC3Hlo - Timer Input Capture Holding Registers 3 Low; 0x0000007F ***/
    union {
      byte Byte;
      struct {
        byte BIT0        :1;                                       /* Timer Input Capture Holding Bit 0 */
        byte BIT1        :1;                                       /* Timer Input Capture Holding Bit 1 */
        byte BIT2        :1;                                       /* Timer Input Capture Holding Bit 2 */
        byte BIT3        :1;                                       /* Timer Input Capture Holding Bit 3 */
        byte BIT4        :1;                                       /* Timer Input Capture Holding Bit 4 */
        byte BIT5        :1;                                       /* Timer Input Capture Holding Bit 5 */
        byte BIT6        :1;                                       /* Timer Input Capture Holding Bit 6 */
        byte BIT7        :1;                                       /* Timer Input Capture Holding Bit 7 */
      } Bits;
    } TC3HloSTR;
    #define TC3Hlo                      _TC3H.Overlap_STR.TC3HloSTR.Byte
    #define TC3Hlo_BIT0                 _TC3H.Overlap_STR.TC3HloSTR.Bits.BIT0
    #define TC3Hlo_BIT1                 _TC3H.Overlap_STR.TC3HloSTR.Bits.BIT1
    #define TC3Hlo_BIT2                 _TC3H.Overlap_STR.TC3HloSTR.Bits.BIT2
    #define TC3Hlo_BIT3                 _TC3H.Overlap_STR.TC3HloSTR.Bits.BIT3
    #define TC3Hlo_BIT4                 _TC3H.Overlap_STR.TC3HloSTR.Bits.BIT4
    #define TC3Hlo_BIT5                 _TC3H.Overlap_STR.TC3HloSTR.Bits.BIT5
    #define TC3Hlo_BIT6                 _TC3H.Overlap_STR.TC3HloSTR.Bits.BIT6
    #define TC3Hlo_BIT7                 _TC3H.Overlap_STR.TC3HloSTR.Bits.BIT7
    
    #define TC3Hlo_BIT0_MASK            1U
    #define TC3Hlo_BIT1_MASK            2U
    #define TC3Hlo_BIT2_MASK            4U
    #define TC3Hlo_BIT3_MASK            8U
    #define TC3Hlo_BIT4_MASK            16U
    #define TC3Hlo_BIT5_MASK            32U
    #define TC3Hlo_BIT6_MASK            64U
    #define TC3Hlo_BIT7_MASK            128U
    
  } Overlap_STR;

  struct {
    word BIT0        :1;                                       /* Timer Input Capture Holding Bit 0 */
    word BIT1        :1;                                       /* Timer Input Capture Holding Bit 1 */
    word BIT2        :1;                                       /* Timer Input Capture Holding Bit 2 */
    word BIT3        :1;                                       /* Timer Input Capture Holding Bit 3 */
    word BIT4        :1;                                       /* Timer Input Capture Holding Bit 4 */
    word BIT5        :1;                                       /* Timer Input Capture Holding Bit 5 */
    word BIT6        :1;                                       /* Timer Input Capture Holding Bit 6 */
    word BIT7        :1;                                       /* Timer Input Capture Holding Bit 7 */
    word BIT8        :1;                                       /* Timer Input Capture Holding Bit 8 */
    word BIT9        :1;                                       /* Timer Input Capture Holding Bit 9 */
    word BIT10       :1;                                       /* Timer Input Capture Holding Bit 10 */
    word BIT11       :1;                                       /* Timer Input Capture Holding Bit 11 */
    word BIT12       :1;                                       /* Timer Input Capture Holding Bit 12 */
    word BIT13       :1;                                       /* Timer Input Capture Holding Bit 13 */
    word BIT14       :1;                                       /* Timer Input Capture Holding Bit 14 */
    word BIT15       :1;                                       /* Timer Input Capture Holding Bit 15 */
  } Bits;
} TC3HSTR;
extern volatile TC3HSTR _TC3H @(REG_BASE + 0x0000007EUL);
#define TC3H                            _TC3H.Word
#define TC3H_BIT0                       _TC3H.Bits.BIT0
#define TC3H_BIT1                       _TC3H.Bits.BIT1
#define TC3H_BIT2                       _TC3H.Bits.BIT2
#define TC3H_BIT3                       _TC3H.Bits.BIT3
#define TC3H_BIT4                       _TC3H.Bits.BIT4
#define TC3H_BIT5                       _TC3H.Bits.BIT5
#define TC3H_BIT6                       _TC3H.Bits.BIT6
#define TC3H_BIT7                       _TC3H.Bits.BIT7
#define TC3H_BIT8                       _TC3H.Bits.BIT8
#define TC3H_BIT9                       _TC3H.Bits.BIT9
#define TC3H_BIT10                      _TC3H.Bits.BIT10
#define TC3H_BIT11                      _TC3H.Bits.BIT11
#define TC3H_BIT12                      _TC3H.Bits.BIT12
#define TC3H_BIT13                      _TC3H.Bits.BIT13
#define TC3H_BIT14                      _TC3H.Bits.BIT14
#define TC3H_BIT15                      _TC3H.Bits.BIT15

#define TC3H_BIT0_MASK                  1U
#define TC3H_BIT1_MASK                  2U
#define TC3H_BIT2_MASK                  4U
#define TC3H_BIT3_MASK                  8U
#define TC3H_BIT4_MASK                  16U
#define TC3H_BIT5_MASK                  32U
#define TC3H_BIT6_MASK                  64U
#define TC3H_BIT7_MASK                  128U
#define TC3H_BIT8_MASK                  256U
#define TC3H_BIT9_MASK                  512U
#define TC3H_BIT10_MASK                 1024U
#define TC3H_BIT11_MASK                 2048U
#define TC3H_BIT12_MASK                 4096U
#define TC3H_BIT13_MASK                 8192U
#define TC3H_BIT14_MASK                 16384U
#define TC3H_BIT15_MASK                 32768U


/*** ATD0CTL23 - ATD 0 Control Register 23; 0x00000082 ***/
typedef union {
  word Word;
   /* Overlapped registers: */
  struct {
    /*** ATD0CTL2 - ATD 0 Control Register 2; 0x00000082 ***/
    union {
      byte Byte;
      struct {
        byte ASCIF       :1;                                       /* ATD 0 Sequence Complete Interrupt Flag */
        byte ASCIE       :1;                                       /* ATD 0 Sequence Complete Interrupt Enable */
        byte ETRIGE      :1;                                       /* External Trigger Mode enable */
        byte ETRIGP      :1;                                       /* External Trigger Polarity */
        byte ETRIGLE     :1;                                       /* External Trigger Level/Edge control */
        byte AWAI        :1;                                       /* ATD Power Down in Wait Mode */
        byte AFFC        :1;                                       /* ATD Fast Conversion Complete Flag Clear */
        byte ADPU        :1;                                       /* ATD Disable / Power Down */
      } Bits;
    } ATD0CTL2STR;
    #define ATD0CTL2                    _ATD0CTL23.Overlap_STR.ATD0CTL2STR.Byte
    #define ATD0CTL2_ASCIF              _ATD0CTL23.Overlap_STR.ATD0CTL2STR.Bits.ASCIF
    #define ATD0CTL2_ASCIE              _ATD0CTL23.Overlap_STR.ATD0CTL2STR.Bits.ASCIE
    #define ATD0CTL2_ETRIGE             _ATD0CTL23.Overlap_STR.ATD0CTL2STR.Bits.ETRIGE
    #define ATD0CTL2_ETRIGP             _ATD0CTL23.Overlap_STR.ATD0CTL2STR.Bits.ETRIGP
    #define ATD0CTL2_ETRIGLE            _ATD0CTL23.Overlap_STR.ATD0CTL2STR.Bits.ETRIGLE
    #define ATD0CTL2_AWAI               _ATD0CTL23.Overlap_STR.ATD0CTL2STR.Bits.AWAI
    #define ATD0CTL2_AFFC               _ATD0CTL23.Overlap_STR.ATD0CTL2STR.Bits.AFFC
    #define ATD0CTL2_ADPU               _ATD0CTL23.Overlap_STR.ATD0CTL2STR.Bits.ADPU
    
    #define ATD0CTL2_ASCIF_MASK         1U
    #define ATD0CTL2_ASCIE_MASK         2U
    #define ATD0CTL2_ETRIGE_MASK        4U
    #define ATD0CTL2_ETRIGP_MASK        8U
    #define ATD0CTL2_ETRIGLE_MASK       16U
    #define ATD0CTL2_AWAI_MASK          32U
    #define ATD0CTL2_AFFC_MASK          64U
    #define ATD0CTL2_ADPU_MASK          128U
    

    /*** ATD0CTL3 - ATD 0 Control Register 3; 0x00000083 ***/
    union {
      byte Byte;
      struct {
        byte FRZ0        :1;                                       /* Background Debug Freeze Enable Bit 0 */
        byte FRZ1        :1;                                       /* Background Debug Freeze Enable Bit 1 */
        byte FIFO        :1;                                       /* Result Register FIFO Mode */
        byte S1C         :1;                                       /* Conversion Sequence Length 1 */
        byte S2C         :1;                                       /* Conversion Sequence Length 2 */
        byte S4C         :1;                                       /* Conversion Sequence Length 4 */
        byte S8C         :1;                                       /* Conversion Sequence Length 8 */
        byte             :1; 
      } Bits;
      struct {
        byte grpFRZ :2;
        byte     :1;
        byte     :1;
        byte     :1;
        byte     :1;
        byte     :1;
        byte     :1;
      } MergedBits;
    } ATD0CTL3STR;
    #define ATD0CTL3                    _ATD0CTL23.Overlap_STR.ATD0CTL3STR.Byte
    #define ATD0CTL3_FRZ0               _ATD0CTL23.Overlap_STR.ATD0CTL3STR.Bits.FRZ0
    #define ATD0CTL3_FRZ1               _ATD0CTL23.Overlap_STR.ATD0CTL3STR.Bits.FRZ1
    #define ATD0CTL3_FIFO               _ATD0CTL23.Overlap_STR.ATD0CTL3STR.Bits.FIFO
    #define ATD0CTL3_S1C                _ATD0CTL23.Overlap_STR.ATD0CTL3STR.Bits.S1C
    #define ATD0CTL3_S2C                _ATD0CTL23.Overlap_STR.ATD0CTL3STR.Bits.S2C
    #define ATD0CTL3_S4C                _ATD0CTL23.Overlap_STR.ATD0CTL3STR.Bits.S4C
    #define ATD0CTL3_S8C                _ATD0CTL23.Overlap_STR.ATD0CTL3STR.Bits.S8C
    #define ATD0CTL3_FRZ                _ATD0CTL23.Overlap_STR.ATD0CTL3STR.MergedBits.grpFRZ
    
    #define ATD0CTL3_FRZ0_MASK          1U
    #define ATD0CTL3_FRZ1_MASK          2U
    #define ATD0CTL3_FIFO_MASK          4U
    #define ATD0CTL3_S1C_MASK           8U
    #define ATD0CTL3_S2C_MASK           16U
    #define ATD0CTL3_S4C_MASK           32U
    #define ATD0CTL3_S8C_MASK           64U
    #define ATD0CTL3_FRZ_MASK           3U
    #define ATD0CTL3_FRZ_BITNUM         0U
    
  } Overlap_STR;

  struct {
    word FRZ0        :1;                                       /* Background Debug Freeze Enable Bit 0 */
    word FRZ1        :1;                                       /* Background Debug Freeze Enable Bit 1 */
    word FIFO        :1;                                       /* Result Register FIFO Mode */
    word S1C         :1;                                       /* Conversion Sequence Length 1 */
    word S2C         :1;                                       /* Conversion Sequence Length 2 */
    word S4C         :1;                                       /* Conversion Sequence Length 4 */
    word S8C         :1;                                       /* Conversion Sequence Length 8 */
    word             :1; 
    word ASCIF       :1;                                       /* ATD 0 Sequence Complete Interrupt Flag */
    word ASCIE       :1;                                       /* ATD 0 Sequence Complete Interrupt Enable */
    word ETRIGE      :1;                                       /* External Trigger Mode enable */
    word ETRIGP      :1;                                       /* External Trigger Polarity */
    word ETRIGLE     :1;                                       /* External Trigger Level/Edge control */
    word AWAI        :1;                                       /* ATD Power Down in Wait Mode */
    word AFFC        :1;                                       /* ATD Fast Conversion Complete Flag Clear */
    word ADPU        :1;                                       /* ATD Disable / Power Down */
  } Bits;
  struct {
    word grpFRZ  :2;
    word         :1;
    word         :1;
    word         :1;
    word         :1;
    word         :1;
    word         :1;
    word         :1;
    word         :1;
    word         :1;
    word         :1;
    word         :1;
    word         :1;
    word         :1;
    word         :1;
  } MergedBits;
} ATD0CTL23STR;
extern volatile ATD0CTL23STR _ATD0CTL23 @(REG_BASE + 0x00000082UL);
#define ATD0CTL23                       _ATD0CTL23.Word
#define ATD0CTL23_FRZ0                  _ATD0CTL23.Bits.FRZ0
#define ATD0CTL23_FRZ1                  _ATD0CTL23.Bits.FRZ1
#define ATD0CTL23_FIFO                  _ATD0CTL23.Bits.FIFO
#define ATD0CTL23_S1C                   _ATD0CTL23.Bits.S1C
#define ATD0CTL23_S2C                   _ATD0CTL23.Bits.S2C
#define ATD0CTL23_S4C                   _ATD0CTL23.Bits.S4C
#define ATD0CTL23_S8C                   _ATD0CTL23.Bits.S8C
#define ATD0CTL23_ASCIF                 _ATD0CTL23.Bits.ASCIF
#define ATD0CTL23_ASCIE                 _ATD0CTL23.Bits.ASCIE
#define ATD0CTL23_ETRIGE                _ATD0CTL23.Bits.ETRIGE
#define ATD0CTL23_ETRIGP                _ATD0CTL23.Bits.ETRIGP
#define ATD0CTL23_ETRIGLE               _ATD0CTL23.Bits.ETRIGLE
#define ATD0CTL23_AWAI                  _ATD0CTL23.Bits.AWAI
#define ATD0CTL23_AFFC                  _ATD0CTL23.Bits.AFFC
#define ATD0CTL23_ADPU                  _ATD0CTL23.Bits.ADPU
#define ATD0CTL23_FRZ                   _ATD0CTL23.MergedBits.grpFRZ

#define ATD0CTL23_FRZ0_MASK             1U
#define ATD0CTL23_FRZ1_MASK             2U
#define ATD0CTL23_FIFO_MASK             4U
#define ATD0CTL23_S1C_MASK              8U
#define ATD0CTL23_S2C_MASK              16U
#define ATD0CTL23_S4C_MASK              32U
#define ATD0CTL23_S8C_MASK              64U
#define ATD0CTL23_ASCIF_MASK            256U
#define ATD0CTL23_ASCIE_MASK            512U
#define ATD0CTL23_ETRIGE_MASK           1024U
#define ATD0CTL23_ETRIGP_MASK           2048U
#define ATD0CTL23_ETRIGLE_MASK          4096U
#define ATD0CTL23_AWAI_MASK             8192U
#define ATD0CTL23_AFFC_MASK             16384U
#define ATD0CTL23_ADPU_MASK             32768U
#define ATD0CTL23_FRZ_MASK              3U
#define ATD0CTL23_FRZ_BITNUM            0U


/*** ATD0CTL45 - ATD 0 Control Register 45; 0x00000084 ***/
typedef union {
  word Word;
   /* Overlapped registers: */
  struct {
    /*** ATD0CTL4 - ATD 0 Control Register 4; 0x00000084 ***/
    union {
      byte Byte;
      struct {
        byte PRS0        :1;                                       /* ATD Clock Prescaler 0 */
        byte PRS1        :1;                                       /* ATD Clock Prescaler 1 */
        byte PRS2        :1;                                       /* ATD Clock Prescaler 2 */
        byte PRS3        :1;                                       /* ATD Clock Prescaler 3 */
        byte PRS4        :1;                                       /* ATD Clock Prescaler 4 */
        byte SMP0        :1;                                       /* Sample Time Select 0 */
        byte SMP1        :1;                                       /* Sample Time Select 1 */
        byte SRES8       :1;                                       /* ATD Resolution Select */
      } Bits;
      struct {
        byte grpPRS :5;
        byte grpSMP :2;
        byte grpSRES_8 :1;
      } MergedBits;
    } ATD0CTL4STR;
    #define ATD0CTL4                    _ATD0CTL45.Overlap_STR.ATD0CTL4STR.Byte
    #define ATD0CTL4_PRS0               _ATD0CTL45.Overlap_STR.ATD0CTL4STR.Bits.PRS0
    #define ATD0CTL4_PRS1               _ATD0CTL45.Overlap_STR.ATD0CTL4STR.Bits.PRS1
    #define ATD0CTL4_PRS2               _ATD0CTL45.Overlap_STR.ATD0CTL4STR.Bits.PRS2
    #define ATD0CTL4_PRS3               _ATD0CTL45.Overlap_STR.ATD0CTL4STR.Bits.PRS3
    #define ATD0CTL4_PRS4               _ATD0CTL45.Overlap_STR.ATD0CTL4STR.Bits.PRS4
    #define ATD0CTL4_SMP0               _ATD0CTL45.Overlap_STR.ATD0CTL4STR.Bits.SMP0
    #define ATD0CTL4_SMP1               _ATD0CTL45.Overlap_STR.ATD0CTL4STR.Bits.SMP1
    #define ATD0CTL4_SRES8              _ATD0CTL45.Overlap_STR.ATD0CTL4STR.Bits.SRES8
    #define ATD0CTL4_PRS                _ATD0CTL45.Overlap_STR.ATD0CTL4STR.MergedBits.grpPRS
    #define ATD0CTL4_SMP                _ATD0CTL45.Overlap_STR.ATD0CTL4STR.MergedBits.grpSMP
    
    #define ATD0CTL4_PRS0_MASK          1U
    #define ATD0CTL4_PRS1_MASK          2U
    #define ATD0CTL4_PRS2_MASK          4U
    #define ATD0CTL4_PRS3_MASK          8U
    #define ATD0CTL4_PRS4_MASK          16U
    #define ATD0CTL4_SMP0_MASK          32U
    #define ATD0CTL4_SMP1_MASK          64U
    #define ATD0CTL4_SRES8_MASK         128U
    #define ATD0CTL4_PRS_MASK           31U
    #define ATD0CTL4_PRS_BITNUM         0U
    #define ATD0CTL4_SMP_MASK           96U
    #define ATD0CTL4_SMP_BITNUM         5U
    

    /*** ATD0CTL5 - ATD 0 Control Register 5; 0x00000085 ***/
    union {
      byte Byte;
      struct {
        byte CA          :1;                                       /* Analog Input Channel Select Code A */
        byte CB          :1;                                       /* Analog Input Channel Select Code B */
        byte CC          :1;                                       /* Analog Input Channel Select Code C */
        byte             :1; 
        byte MULT        :1;                                       /* Multi-Channel Sample Mode */
        byte SCAN        :1;                                       /* Continuous Conversion Sequence Mode */
        byte DSGN        :1;                                       /* Signed/Unsigned Result Data Mode */
        byte DJM         :1;                                       /* Result Register Data Justification Mode */
      } Bits;
      struct {
        byte grpCx :3;
        byte     :1;
        byte     :1;
        byte     :1;
        byte     :1;
        byte     :1;
      } MergedBits;
    } ATD0CTL5STR;
    #define ATD0CTL5                    _ATD0CTL45.Overlap_STR.ATD0CTL5STR.Byte
    #define ATD0CTL5_CA                 _ATD0CTL45.Overlap_STR.ATD0CTL5STR.Bits.CA
    #define ATD0CTL5_CB                 _ATD0CTL45.Overlap_STR.ATD0CTL5STR.Bits.CB
    #define ATD0CTL5_CC                 _ATD0CTL45.Overlap_STR.ATD0CTL5STR.Bits.CC
    #define ATD0CTL5_MULT               _ATD0CTL45.Overlap_STR.ATD0CTL5STR.Bits.MULT
    #define ATD0CTL5_SCAN               _ATD0CTL45.Overlap_STR.ATD0CTL5STR.Bits.SCAN
    #define ATD0CTL5_DSGN               _ATD0CTL45.Overlap_STR.ATD0CTL5STR.Bits.DSGN
    #define ATD0CTL5_DJM                _ATD0CTL45.Overlap_STR.ATD0CTL5STR.Bits.DJM
    #define ATD0CTL5_Cx                 _ATD0CTL45.Overlap_STR.ATD0CTL5STR.MergedBits.grpCx
    
    #define ATD0CTL5_CA_MASK            1U
    #define ATD0CTL5_CB_MASK            2U
    #define ATD0CTL5_CC_MASK            4U
    #define ATD0CTL5_MULT_MASK          16U
    #define ATD0CTL5_SCAN_MASK          32U
    #define ATD0CTL5_DSGN_MASK          64U
    #define ATD0CTL5_DJM_MASK           128U
    #define ATD0CTL5_Cx_MASK            7U
    #define ATD0CTL5_Cx_BITNUM          0U
    
  } Overlap_STR;

  struct {
    word CA          :1;                                       /* Analog Input Channel Select Code A */
    word CB          :1;                                       /* Analog Input Channel Select Code B */
    word CC          :1;                                       /* Analog Input Channel Select Code C */
    word             :1; 
    word MULT        :1;                                       /* Multi-Channel Sample Mode */
    word SCAN        :1;                                       /* Continuous Conversion Sequence Mode */
    word DSGN        :1;                                       /* Signed/Unsigned Result Data Mode */
    word DJM         :1;                                       /* Result Register Data Justification Mode */
    word PRS0        :1;                                       /* ATD Clock Prescaler 0 */
    word PRS1        :1;                                       /* ATD Clock Prescaler 1 */
    word PRS2        :1;                                       /* ATD Clock Prescaler 2 */
    word PRS3        :1;                                       /* ATD Clock Prescaler 3 */
    word PRS4        :1;                                       /* ATD Clock Prescaler 4 */
    word SMP0        :1;                                       /* Sample Time Select 0 */
    word SMP1        :1;                                       /* Sample Time Select 1 */
    word SRES8       :1;                                       /* ATD Resolution Select */
  } Bits;
  struct {
    word grpCx   :3;
    word         :1;
    word         :1;
    word         :1;
    word         :1;
    word         :1;
    word grpPRS  :5;
    word grpSMP  :2;
    word grpSRES_8 :1;
  } MergedBits;
} ATD0CTL45STR;
extern volatile ATD0CTL45STR _ATD0CTL45 @(REG_BASE + 0x00000084UL);
#define ATD0CTL45                       _ATD0CTL45.Word
#define ATD0CTL45_CA                    _ATD0CTL45.Bits.CA
#define ATD0CTL45_CB                    _ATD0CTL45.Bits.CB
#define ATD0CTL45_CC                    _ATD0CTL45.Bits.CC
#define ATD0CTL45_MULT                  _ATD0CTL45.Bits.MULT
#define ATD0CTL45_SCAN                  _ATD0CTL45.Bits.SCAN
#define ATD0CTL45_DSGN                  _ATD0CTL45.Bits.DSGN
#define ATD0CTL45_DJM                   _ATD0CTL45.Bits.DJM
#define ATD0CTL45_PRS0                  _ATD0CTL45.Bits.PRS0
#define ATD0CTL45_PRS1                  _ATD0CTL45.Bits.PRS1
#define ATD0CTL45_PRS2                  _ATD0CTL45.Bits.PRS2
#define ATD0CTL45_PRS3                  _ATD0CTL45.Bits.PRS3
#define ATD0CTL45_PRS4                  _ATD0CTL45.Bits.PRS4
#define ATD0CTL45_SMP0                  _ATD0CTL45.Bits.SMP0
#define ATD0CTL45_SMP1                  _ATD0CTL45.Bits.SMP1
#define ATD0CTL45_SRES8                 _ATD0CTL45.Bits.SRES8
#define ATD0CTL45_Cx                    _ATD0CTL45.MergedBits.grpCx
#define ATD0CTL45_PRS                   _ATD0CTL45.MergedBits.grpPRS
#define ATD0CTL45_SMP                   _ATD0CTL45.MergedBits.grpSMP

#define ATD0CTL45_CA_MASK               1U
#define ATD0CTL45_CB_MASK               2U
#define ATD0CTL45_CC_MASK               4U
#define ATD0CTL45_MULT_MASK             16U
#define ATD0CTL45_SCAN_MASK             32U
#define ATD0CTL45_DSGN_MASK             64U
#define ATD0CTL45_DJM_MASK              128U
#define ATD0CTL45_PRS0_MASK             256U
#define ATD0CTL45_PRS1_MASK             512U
#define ATD0CTL45_PRS2_MASK             1024U
#define ATD0CTL45_PRS3_MASK             2048U
#define ATD0CTL45_PRS4_MASK             4096U
#define ATD0CTL45_SMP0_MASK             8192U
#define ATD0CTL45_SMP1_MASK             16384U
#define ATD0CTL45_SRES8_MASK            32768U
#define ATD0CTL45_Cx_MASK               7U
#define ATD0CTL45_Cx_BITNUM             0U
#define ATD0CTL45_PRS_MASK              7936U
#define ATD0CTL45_PRS_BITNUM            8U
#define ATD0CTL45_SMP_MASK              24576U
#define ATD0CTL45_SMP_BITNUM            13U


/*** ATD0STAT0 - ATD 0 Status Register 0; 0x00000086 ***/
typedef union {
  byte Byte;
  struct {
    byte CC0         :1;                                       /* Conversion Counter 0 */
    byte CC1         :1;                                       /* Conversion Counter 1 */
    byte CC2         :1;                                       /* Conversion Counter 2 */
    byte             :1; 
    byte FIFOR       :1;                                       /* FIFO Over Run Flag */
    byte ETORF       :1;                                       /* External Trigger Overrun Flag */
    byte             :1; 
    byte SCF         :1;                                       /* Sequence Complete Flag */
  } Bits;
  struct {
    byte grpCC   :3;
    byte         :1;
    byte         :1;
    byte         :1;
    byte         :1;
    byte         :1;
  } MergedBits;
} ATD0STAT0STR;
extern volatile ATD0STAT0STR _ATD0STAT0 @(REG_BASE + 0x00000086UL);
#define ATD0STAT0                       _ATD0STAT0.Byte
#define ATD0STAT0_CC0                   _ATD0STAT0.Bits.CC0
#define ATD0STAT0_CC1                   _ATD0STAT0.Bits.CC1
#define ATD0STAT0_CC2                   _ATD0STAT0.Bits.CC2
#define ATD0STAT0_FIFOR                 _ATD0STAT0.Bits.FIFOR
#define ATD0STAT0_ETORF                 _ATD0STAT0.Bits.ETORF
#define ATD0STAT0_SCF                   _ATD0STAT0.Bits.SCF
#define ATD0STAT0_CC                    _ATD0STAT0.MergedBits.grpCC

#define ATD0STAT0_CC0_MASK              1U
#define ATD0STAT0_CC1_MASK              2U
#define ATD0STAT0_CC2_MASK              4U
#define ATD0STAT0_FIFOR_MASK            16U
#define ATD0STAT0_ETORF_MASK            32U
#define ATD0STAT0_SCF_MASK              128U
#define ATD0STAT0_CC_MASK               7U
#define ATD0STAT0_CC_BITNUM             0U


/*** ATD0TEST1 - ATD0 Test Register; 0x00000089 ***/
typedef union {
  byte Byte;
  struct {
    byte SC          :1;                                       /* Special Channel Conversion Bit */
    byte             :1; 
    byte             :1; 
    byte             :1; 
    byte             :1; 
    byte             :1; 
    byte             :1; 
    byte             :1; 
  } Bits;
} ATD0TEST1STR;
extern volatile ATD0TEST1STR _ATD0TEST1 @(REG_BASE + 0x00000089UL);
#define ATD0TEST1                       _ATD0TEST1.Byte
#define ATD0TEST1_SC                    _ATD0TEST1.Bits.SC

#define ATD0TEST1_SC_MASK               1U


/*** ATD0STAT1 - ATD 0 Status Register 1; 0x0000008B ***/
typedef union {
  byte Byte;
  struct {
    byte CCF0        :1;                                       /* Conversion Complete Flag 0 */
    byte CCF1        :1;                                       /* Conversion Complete Flag 1 */
    byte CCF2        :1;                                       /* Conversion Complete Flag 2 */
    byte CCF3        :1;                                       /* Conversion Complete Flag 3 */
    byte CCF4        :1;                                       /* Conversion Complete Flag 4 */
    byte CCF5        :1;                                       /* Conversion Complete Flag 5 */
    byte CCF6        :1;                                       /* Conversion Complete Flag 6 */
    byte CCF7        :1;                                       /* Conversion Complete Flag 7 */
  } Bits;
} ATD0STAT1STR;
extern volatile ATD0STAT1STR _ATD0STAT1 @(REG_BASE + 0x0000008BUL);
#define ATD0STAT1                       _ATD0STAT1.Byte
#define ATD0STAT1_CCF0                  _ATD0STAT1.Bits.CCF0
#define ATD0STAT1_CCF1                  _ATD0STAT1.Bits.CCF1
#define ATD0STAT1_CCF2                  _ATD0STAT1.Bits.CCF2
#define ATD0STAT1_CCF3                  _ATD0STAT1.Bits.CCF3
#define ATD0STAT1_CCF4                  _ATD0STAT1.Bits.CCF4
#define ATD0STAT1_CCF5                  _ATD0STAT1.Bits.CCF5
#define ATD0STAT1_CCF6                  _ATD0STAT1.Bits.CCF6
#define ATD0STAT1_CCF7                  _ATD0STAT1.Bits.CCF7

#define ATD0STAT1_CCF0_MASK             1U
#define ATD0STAT1_CCF1_MASK             2U
#define ATD0STAT1_CCF2_MASK             4U
#define ATD0STAT1_CCF3_MASK             8U
#define ATD0STAT1_CCF4_MASK             16U
#define ATD0STAT1_CCF5_MASK             32U
#define ATD0STAT1_CCF6_MASK             64U
#define ATD0STAT1_CCF7_MASK             128U


/*** ATD0DIEN - ATD 0 Input Enable Register; 0x0000008D ***/
typedef union {
  byte Byte;
  struct {
    byte IEN0        :1;                                       /* ATD Digital Input Enable on channel 0 */
    byte IEN1        :1;                                       /* ATD Digital Input Enable on channel 1 */
    byte IEN2        :1;                                       /* ATD Digital Input Enable on channel 2 */
    byte IEN3        :1;                                       /* ATD Digital Input Enable on channel 3 */
    byte IEN4        :1;                                       /* ATD Digital Input Enable on channel 4 */
    byte IEN5        :1;                                       /* ATD Digital Input Enable on channel 5 */
    byte IEN6        :1;                                       /* ATD Digital Input Enable on channel 6 */
    byte IEN7        :1;                                       /* ATD Digital Input Enable on channel 7 */
  } Bits;
} ATD0DIENSTR;
extern volatile ATD0DIENSTR _ATD0DIEN @(REG_BASE + 0x0000008DUL);
#define ATD0DIEN                        _ATD0DIEN.Byte
#define ATD0DIEN_IEN0                   _ATD0DIEN.Bits.IEN0
#define ATD0DIEN_IEN1                   _ATD0DIEN.Bits.IEN1
#define ATD0DIEN_IEN2                   _ATD0DIEN.Bits.IEN2
#define ATD0DIEN_IEN3                   _ATD0DIEN.Bits.IEN3
#define ATD0DIEN_IEN4                   _ATD0DIEN.Bits.IEN4
#define ATD0DIEN_IEN5                   _ATD0DIEN.Bits.IEN5
#define ATD0DIEN_IEN6                   _ATD0DIEN.Bits.IEN6
#define ATD0DIEN_IEN7                   _ATD0DIEN.Bits.IEN7

#define ATD0DIEN_IEN0_MASK              1U
#define ATD0DIEN_IEN1_MASK              2U
#define ATD0DIEN_IEN2_MASK              4U
#define ATD0DIEN_IEN3_MASK              8U
#define ATD0DIEN_IEN4_MASK              16U
#define ATD0DIEN_IEN5_MASK              32U
#define ATD0DIEN_IEN6_MASK              64U
#define ATD0DIEN_IEN7_MASK              128U


/*** PORTAD0 - Port AD0 Register; 0x0000008F ***/
typedef union {
  byte Byte;
  struct {
    byte PTAD0       :1;                                       /* A/D Channel 0 (AN0) Digital Input */
    byte PTAD1       :1;                                       /* A/D Channel 1 (AN1) Digital Input */
    byte PTAD2       :1;                                       /* A/D Channel 2 (AN2) Digital Input */
    byte PTAD3       :1;                                       /* A/D Channel 3 (AN3) Digital Input */
    byte PTAD4       :1;                                       /* A/D Channel 4 (AN4) Digital Input */
    byte PTAD5       :1;                                       /* A/D Channel 5 (AN5) Digital Input */
    byte PTAD6       :1;                                       /* A/D Channel 6 (AN6) Digital Input */
    byte PTAD7       :1;                                       /* A/D Channel 7 (AN7) Digital Input */
  } Bits;
} PORTAD0STR;
extern volatile PORTAD0STR _PORTAD0 @(REG_BASE + 0x0000008FUL);
#define PORTAD0                         _PORTAD0.Byte
#define PORTAD0_PTAD0                   _PORTAD0.Bits.PTAD0
#define PORTAD0_PTAD1                   _PORTAD0.Bits.PTAD1
#define PORTAD0_PTAD2                   _PORTAD0.Bits.PTAD2
#define PORTAD0_PTAD3                   _PORTAD0.Bits.PTAD3
#define PORTAD0_PTAD4                   _PORTAD0.Bits.PTAD4
#define PORTAD0_PTAD5                   _PORTAD0.Bits.PTAD5
#define PORTAD0_PTAD6                   _PORTAD0.Bits.PTAD6
#define PORTAD0_PTAD7                   _PORTAD0.Bits.PTAD7

#define PORTAD0_PTAD0_MASK              1U
#define PORTAD0_PTAD1_MASK              2U
#define PORTAD0_PTAD2_MASK              4U
#define PORTAD0_PTAD3_MASK              8U
#define PORTAD0_PTAD4_MASK              16U
#define PORTAD0_PTAD5_MASK              32U
#define PORTAD0_PTAD6_MASK              64U
#define PORTAD0_PTAD7_MASK              128U


/*** ATD0DR0 - ATD 0 Conversion Result Register 0; 0x00000090 ***/
typedef union {
  word Word;
   /* Overlapped registers: */
  struct {
    /*** ATD0DR0H - ATD 0 Conversion Result Register 0 High; 0x00000090 ***/
    union {
      byte Byte;
      struct {
        byte BIT8        :1;                                       /* Bit 8 */
        byte BIT9        :1;                                       /* Bit 9 */
        byte BIT10       :1;                                       /* Bit 10 */
        byte BIT11       :1;                                       /* Bit 11 */
        byte BIT12       :1;                                       /* Bit 12 */
        byte BIT13       :1;                                       /* Bit 13 */
        byte BIT14       :1;                                       /* Bit 14 */
        byte BIT15       :1;                                       /* Bit 15 */
      } Bits;
    } ATD0DR0HSTR;
    #define ATD0DR0H                    _ATD0DR0.Overlap_STR.ATD0DR0HSTR.Byte
    #define ATD0DR0H_BIT8               _ATD0DR0.Overlap_STR.ATD0DR0HSTR.Bits.BIT8
    #define ATD0DR0H_BIT9               _ATD0DR0.Overlap_STR.ATD0DR0HSTR.Bits.BIT9
    #define ATD0DR0H_BIT10              _ATD0DR0.Overlap_STR.ATD0DR0HSTR.Bits.BIT10
    #define ATD0DR0H_BIT11              _ATD0DR0.Overlap_STR.ATD0DR0HSTR.Bits.BIT11
    #define ATD0DR0H_BIT12              _ATD0DR0.Overlap_STR.ATD0DR0HSTR.Bits.BIT12
    #define ATD0DR0H_BIT13              _ATD0DR0.Overlap_STR.ATD0DR0HSTR.Bits.BIT13
    #define ATD0DR0H_BIT14              _ATD0DR0.Overlap_STR.ATD0DR0HSTR.Bits.BIT14
    #define ATD0DR0H_BIT15              _ATD0DR0.Overlap_STR.ATD0DR0HSTR.Bits.BIT15
    
    #define ATD0DR0H_BIT8_MASK          1U
    #define ATD0DR0H_BIT9_MASK          2U
    #define ATD0DR0H_BIT10_MASK         4U
    #define ATD0DR0H_BIT11_MASK         8U
    #define ATD0DR0H_BIT12_MASK         16U
    #define ATD0DR0H_BIT13_MASK         32U
    #define ATD0DR0H_BIT14_MASK         64U
    #define ATD0DR0H_BIT15_MASK         128U
    

    /*** ATD0DR0L - ATD 0 Conversion Result Register 0 Low; 0x00000091 ***/
    union {
      byte Byte;
      struct {
        byte             :1; 
        byte             :1; 
        byte             :1; 
        byte             :1; 
        byte             :1; 
        byte             :1; 
        byte BIT6        :1;                                       /* Bit 6 */
        byte BIT7        :1;                                       /* Bit 7 */
      } Bits;
      struct {
        byte     :1;
        byte     :1;
        byte     :1;
        byte     :1;
        byte     :1;
        byte     :1;
        byte grpBIT_6 :2;
      } MergedBits;
    } ATD0DR0LSTR;
    #define ATD0DR0L                    _ATD0DR0.Overlap_STR.ATD0DR0LSTR.Byte
    #define ATD0DR0L_BIT6               _ATD0DR0.Overlap_STR.ATD0DR0LSTR.Bits.BIT6
    #define ATD0DR0L_BIT7               _ATD0DR0.Overlap_STR.ATD0DR0LSTR.Bits.BIT7
    #define ATD0DR0L_BIT_6              _ATD0DR0.Overlap_STR.ATD0DR0LSTR.MergedBits.grpBIT_6
    #define ATD0DR0L_BIT                ATD0DR0L_BIT_6
    
    #define ATD0DR0L_BIT6_MASK          64U
    #define ATD0DR0L_BIT7_MASK          128U
    #define ATD0DR0L_BIT_6_MASK         192U
    #define ATD0DR0L_BIT_6_BITNUM       6U
    
  } Overlap_STR;

  struct {
    word             :1; 
    word             :1; 
    word             :1; 
    word             :1; 
    word             :1; 
    word             :1; 
    word BIT6        :1;                                       /* Bit 6 */
    word BIT7        :1;                                       /* Bit 7 */
    word BIT8        :1;                                       /* Bit 8 */
    word BIT9        :1;                                       /* Bit 9 */
    word BIT10       :1;                                       /* Bit 10 */
    word BIT11       :1;                                       /* Bit 11 */
    word BIT12       :1;                                       /* Bit 12 */
    word BIT13       :1;                                       /* Bit 13 */
    word BIT14       :1;                                       /* Bit 14 */
    word BIT15       :1;                                       /* Bit 15 */
  } Bits;
  struct {
    word         :1;
    word         :1;
    word         :1;
    word         :1;
    word         :1;
    word         :1;
    word grpBIT_6 :10;
  } MergedBits;
} ATD0DR0STR;
extern volatile ATD0DR0STR _ATD0DR0 @(REG_BASE + 0x00000090UL);
#define ATD0DR0                         _ATD0DR0.Word
#define ATD0DR0_BIT6                    _ATD0DR0.Bits.BIT6
#define ATD0DR0_BIT7                    _ATD0DR0.Bits.BIT7
#define ATD0DR0_BIT8                    _ATD0DR0.Bits.BIT8
#define ATD0DR0_BIT9                    _ATD0DR0.Bits.BIT9
#define ATD0DR0_BIT10                   _ATD0DR0.Bits.BIT10
#define ATD0DR0_BIT11                   _ATD0DR0.Bits.BIT11
#define ATD0DR0_BIT12                   _ATD0DR0.Bits.BIT12
#define ATD0DR0_BIT13                   _ATD0DR0.Bits.BIT13
#define ATD0DR0_BIT14                   _ATD0DR0.Bits.BIT14
#define ATD0DR0_BIT15                   _ATD0DR0.Bits.BIT15
/* ATD0DR_ARR: Access 8 ATD0DRx registers in an array */
#define ATD0DR_ARR                      ((volatile word *) &ATD0DR0)
#define ATD0DR0_BIT_6                   _ATD0DR0.MergedBits.grpBIT_6
#define ATD0DR0_BIT                     ATD0DR0_BIT_6

#define ATD0DR0_BIT6_MASK               64U
#define ATD0DR0_BIT7_MASK               128U
#define ATD0DR0_BIT8_MASK               256U
#define ATD0DR0_BIT9_MASK               512U
#define ATD0DR0_BIT10_MASK              1024U
#define ATD0DR0_BIT11_MASK              2048U
#define ATD0DR0_BIT12_MASK              4096U
#define ATD0DR0_BIT13_MASK              8192U
#define ATD0DR0_BIT14_MASK              16384U
#define ATD0DR0_BIT15_MASK              32768U
#define ATD0DR0_BIT_6_MASK              65472U
#define ATD0DR0_BIT_6_BITNUM            6U


/*** ATD0DR1 - ATD 0 Conversion Result Register 1; 0x00000092 ***/
typedef union {
  word Word;
   /* Overlapped registers: */
  struct {
    /*** ATD0DR1H - ATD 0 Conversion Result Register 1 High; 0x00000092 ***/
    union {
      byte Byte;
      struct {
        byte BIT8        :1;                                       /* Bit 8 */
        byte BIT9        :1;                                       /* Bit 9 */
        byte BIT10       :1;                                       /* Bit 10 */
        byte BIT11       :1;                                       /* Bit 11 */
        byte BIT12       :1;                                       /* Bit 12 */
        byte BIT13       :1;                                       /* Bit 13 */
        byte BIT14       :1;                                       /* Bit 14 */
        byte BIT15       :1;                                       /* Bit 15 */
      } Bits;
    } ATD0DR1HSTR;
    #define ATD0DR1H                    _ATD0DR1.Overlap_STR.ATD0DR1HSTR.Byte
    #define ATD0DR1H_BIT8               _ATD0DR1.Overlap_STR.ATD0DR1HSTR.Bits.BIT8
    #define ATD0DR1H_BIT9               _ATD0DR1.Overlap_STR.ATD0DR1HSTR.Bits.BIT9
    #define ATD0DR1H_BIT10              _ATD0DR1.Overlap_STR.ATD0DR1HSTR.Bits.BIT10
    #define ATD0DR1H_BIT11              _ATD0DR1.Overlap_STR.ATD0DR1HSTR.Bits.BIT11
    #define ATD0DR1H_BIT12              _ATD0DR1.Overlap_STR.ATD0DR1HSTR.Bits.BIT12
    #define ATD0DR1H_BIT13              _ATD0DR1.Overlap_STR.ATD0DR1HSTR.Bits.BIT13
    #define ATD0DR1H_BIT14              _ATD0DR1.Overlap_STR.ATD0DR1HSTR.Bits.BIT14
    #define ATD0DR1H_BIT15              _ATD0DR1.Overlap_STR.ATD0DR1HSTR.Bits.BIT15
    
    #define ATD0DR1H_BIT8_MASK          1U
    #define ATD0DR1H_BIT9_MASK          2U
    #define ATD0DR1H_BIT10_MASK         4U
    #define ATD0DR1H_BIT11_MASK         8U
    #define ATD0DR1H_BIT12_MASK         16U
    #define ATD0DR1H_BIT13_MASK         32U
    #define ATD0DR1H_BIT14_MASK         64U
    #define ATD0DR1H_BIT15_MASK         128U
    

    /*** ATD0DR1L - ATD 0 Conversion Result Register 1 Low; 0x00000093 ***/
    union {
      byte Byte;
      struct {
        byte             :1; 
        byte             :1; 
        byte             :1; 
        byte             :1; 
        byte             :1; 
        byte             :1; 
        byte BIT6        :1;                                       /* Bit 6 */
        byte BIT7        :1;                                       /* Bit 7 */
      } Bits;
      struct {
        byte     :1;
        byte     :1;
        byte     :1;
        byte     :1;
        byte     :1;
        byte     :1;
        byte grpBIT_6 :2;
      } MergedBits;
    } ATD0DR1LSTR;
    #define ATD0DR1L                    _ATD0DR1.Overlap_STR.ATD0DR1LSTR.Byte
    #define ATD0DR1L_BIT6               _ATD0DR1.Overlap_STR.ATD0DR1LSTR.Bits.BIT6
    #define ATD0DR1L_BIT7               _ATD0DR1.Overlap_STR.ATD0DR1LSTR.Bits.BIT7
    #define ATD0DR1L_BIT_6              _ATD0DR1.Overlap_STR.ATD0DR1LSTR.MergedBits.grpBIT_6
    #define ATD0DR1L_BIT                ATD0DR1L_BIT_6
    
    #define ATD0DR1L_BIT6_MASK          64U
    #define ATD0DR1L_BIT7_MASK          128U
    #define ATD0DR1L_BIT_6_MASK         192U
    #define ATD0DR1L_BIT_6_BITNUM       6U
    
  } Overlap_STR;

  struct {
    word             :1; 
    word             :1; 
    word             :1; 
    word             :1; 
    word             :1; 
    word             :1; 
    word BIT6        :1;                                       /* Bit 6 */
    word BIT7        :1;                                       /* Bit 7 */
    word BIT8        :1;                                       /* Bit 8 */
    word BIT9        :1;                                       /* Bit 9 */
    word BIT10       :1;                                       /* Bit 10 */
    word BIT11       :1;                                       /* Bit 11 */
    word BIT12       :1;                                       /* Bit 12 */
    word BIT13       :1;                                       /* Bit 13 */
    word BIT14       :1;                                       /* Bit 14 */
    word BIT15       :1;                                       /* Bit 15 */
  } Bits;
  struct {
    word         :1;
    word         :1;
    word         :1;
    word         :1;
    word         :1;
    word         :1;
    word grpBIT_6 :10;
  } MergedBits;
} ATD0DR1STR;
extern volatile ATD0DR1STR _ATD0DR1 @(REG_BASE + 0x00000092UL);
#define ATD0DR1                         _ATD0DR1.Word
#define ATD0DR1_BIT6                    _ATD0DR1.Bits.BIT6
#define ATD0DR1_BIT7                    _ATD0DR1.Bits.BIT7
#define ATD0DR1_BIT8                    _ATD0DR1.Bits.BIT8
#define ATD0DR1_BIT9                    _ATD0DR1.Bits.BIT9
#define ATD0DR1_BIT10                   _ATD0DR1.Bits.BIT10
#define ATD0DR1_BIT11                   _ATD0DR1.Bits.BIT11
#define ATD0DR1_BIT12                   _ATD0DR1.Bits.BIT12
#define ATD0DR1_BIT13                   _ATD0DR1.Bits.BIT13
#define ATD0DR1_BIT14                   _ATD0DR1.Bits.BIT14
#define ATD0DR1_BIT15                   _ATD0DR1.Bits.BIT15
#define ATD0DR1_BIT_6                   _ATD0DR1.MergedBits.grpBIT_6
#define ATD0DR1_BIT                     ATD0DR1_BIT_6

#define ATD0DR1_BIT6_MASK               64U
#define ATD0DR1_BIT7_MASK               128U
#define ATD0DR1_BIT8_MASK               256U
#define ATD0DR1_BIT9_MASK               512U
#define ATD0DR1_BIT10_MASK              1024U
#define ATD0DR1_BIT11_MASK              2048U
#define ATD0DR1_BIT12_MASK              4096U
#define ATD0DR1_BIT13_MASK              8192U
#define ATD0DR1_BIT14_MASK              16384U
#define ATD0DR1_BIT15_MASK              32768U
#define ATD0DR1_BIT_6_MASK              65472U
#define ATD0DR1_BIT_6_BITNUM            6U


/*** ATD0DR2 - ATD 0 Conversion Result Register 2; 0x00000094 ***/
typedef union {
  word Word;
   /* Overlapped registers: */
  struct {
    /*** ATD0DR2H - ATD 0 Conversion Result Register 2 High; 0x00000094 ***/
    union {
      byte Byte;
      struct {
        byte BIT8        :1;                                       /* Bit 8 */
        byte BIT9        :1;                                       /* Bit 9 */
        byte BIT10       :1;                                       /* Bit 10 */
        byte BIT11       :1;                                       /* Bit 11 */
        byte BIT12       :1;                                       /* Bit 12 */
        byte BIT13       :1;                                       /* Bit 13 */
        byte BIT14       :1;                                       /* Bit 14 */
        byte BIT15       :1;                                       /* Bit 15 */
      } Bits;
    } ATD0DR2HSTR;
    #define ATD0DR2H                    _ATD0DR2.Overlap_STR.ATD0DR2HSTR.Byte
    #define ATD0DR2H_BIT8               _ATD0DR2.Overlap_STR.ATD0DR2HSTR.Bits.BIT8
    #define ATD0DR2H_BIT9               _ATD0DR2.Overlap_STR.ATD0DR2HSTR.Bits.BIT9
    #define ATD0DR2H_BIT10              _ATD0DR2.Overlap_STR.ATD0DR2HSTR.Bits.BIT10
    #define ATD0DR2H_BIT11              _ATD0DR2.Overlap_STR.ATD0DR2HSTR.Bits.BIT11
    #define ATD0DR2H_BIT12              _ATD0DR2.Overlap_STR.ATD0DR2HSTR.Bits.BIT12
    #define ATD0DR2H_BIT13              _ATD0DR2.Overlap_STR.ATD0DR2HSTR.Bits.BIT13
    #define ATD0DR2H_BIT14              _ATD0DR2.Overlap_STR.ATD0DR2HSTR.Bits.BIT14
    #define ATD0DR2H_BIT15              _ATD0DR2.Overlap_STR.ATD0DR2HSTR.Bits.BIT15
    
    #define ATD0DR2H_BIT8_MASK          1U
    #define ATD0DR2H_BIT9_MASK          2U
    #define ATD0DR2H_BIT10_MASK         4U
    #define ATD0DR2H_BIT11_MASK         8U
    #define ATD0DR2H_BIT12_MASK         16U
    #define ATD0DR2H_BIT13_MASK         32U
    #define ATD0DR2H_BIT14_MASK         64U
    #define ATD0DR2H_BIT15_MASK         128U
    

    /*** ATD0DR2L - ATD 0 Conversion Result Register 2 Low; 0x00000095 ***/
    union {
      byte Byte;
      struct {
        byte             :1; 
        byte             :1; 
        byte             :1; 
        byte             :1; 
        byte             :1; 
        byte             :1; 
        byte BIT6        :1;                                       /* Bit 6 */
        byte BIT7        :1;                                       /* Bit 7 */
      } Bits;
      struct {
        byte     :1;
        byte     :1;
        byte     :1;
        byte     :1;
        byte     :1;
        byte     :1;
        byte grpBIT_6 :2;
      } MergedBits;
    } ATD0DR2LSTR;
    #define ATD0DR2L                    _ATD0DR2.Overlap_STR.ATD0DR2LSTR.Byte
    #define ATD0DR2L_BIT6               _ATD0DR2.Overlap_STR.ATD0DR2LSTR.Bits.BIT6
    #define ATD0DR2L_BIT7               _ATD0DR2.Overlap_STR.ATD0DR2LSTR.Bits.BIT7
    #define ATD0DR2L_BIT_6              _ATD0DR2.Overlap_STR.ATD0DR2LSTR.MergedBits.grpBIT_6
    #define ATD0DR2L_BIT                ATD0DR2L_BIT_6
    
    #define ATD0DR2L_BIT6_MASK          64U
    #define ATD0DR2L_BIT7_MASK          128U
    #define ATD0DR2L_BIT_6_MASK         192U
    #define ATD0DR2L_BIT_6_BITNUM       6U
    
  } Overlap_STR;

  struct {
    word             :1; 
    word             :1; 
    word             :1; 
    word             :1; 
    word             :1; 
    word             :1; 
    word BIT6        :1;                                       /* Bit 6 */
    word BIT7        :1;                                       /* Bit 7 */
    word BIT8        :1;                                       /* Bit 8 */
    word BIT9        :1;                                       /* Bit 9 */
    word BIT10       :1;                                       /* Bit 10 */
    word BIT11       :1;                                       /* Bit 11 */
    word BIT12       :1;                                       /* Bit 12 */
    word BIT13       :1;                                       /* Bit 13 */
    word BIT14       :1;                                       /* Bit 14 */
    word BIT15       :1;                                       /* Bit 15 */
  } Bits;
  struct {
    word         :1;
    word         :1;
    word         :1;
    word         :1;
    word         :1;
    word         :1;
    word grpBIT_6 :10;
  } MergedBits;
} ATD0DR2STR;
extern volatile ATD0DR2STR _ATD0DR2 @(REG_BASE + 0x00000094UL);
#define ATD0DR2                         _ATD0DR2.Word
#define ATD0DR2_BIT6                    _ATD0DR2.Bits.BIT6
#define ATD0DR2_BIT7                    _ATD0DR2.Bits.BIT7
#define ATD0DR2_BIT8                    _ATD0DR2.Bits.BIT8
#define ATD0DR2_BIT9                    _ATD0DR2.Bits.BIT9
#define ATD0DR2_BIT10                   _ATD0DR2.Bits.BIT10
#define ATD0DR2_BIT11                   _ATD0DR2.Bits.BIT11
#define ATD0DR2_BIT12                   _ATD0DR2.Bits.BIT12
#define ATD0DR2_BIT13                   _ATD0DR2.Bits.BIT13
#define ATD0DR2_BIT14                   _ATD0DR2.Bits.BIT14
#define ATD0DR2_BIT15                   _ATD0DR2.Bits.BIT15
#define ATD0DR2_BIT_6                   _ATD0DR2.MergedBits.grpBIT_6
#define ATD0DR2_BIT                     ATD0DR2_BIT_6

#define ATD0DR2_BIT6_MASK               64U
#define ATD0DR2_BIT7_MASK               128U
#define ATD0DR2_BIT8_MASK               256U
#define ATD0DR2_BIT9_MASK               512U
#define ATD0DR2_BIT10_MASK              1024U
#define ATD0DR2_BIT11_MASK              2048U
#define ATD0DR2_BIT12_MASK              4096U
#define ATD0DR2_BIT13_MASK              8192U
#define ATD0DR2_BIT14_MASK              16384U
#define ATD0DR2_BIT15_MASK              32768U
#define ATD0DR2_BIT_6_MASK              65472U
#define ATD0DR2_BIT_6_BITNUM            6U


/*** ATD0DR3 - ATD 0 Conversion Result Register 3; 0x00000096 ***/
typedef union {
  word Word;
   /* Overlapped registers: */
  struct {
    /*** ATD0DR3H - ATD 0 Conversion Result Register 3 High; 0x00000096 ***/
    union {
      byte Byte;
      struct {
        byte BIT8        :1;                                       /* Bit 8 */
        byte BIT9        :1;                                       /* Bit 9 */
        byte BIT10       :1;                                       /* Bit 10 */
        byte BIT11       :1;                                       /* Bit 11 */
        byte BIT12       :1;                                       /* Bit 12 */
        byte BIT13       :1;                                       /* Bit 13 */
        byte BIT14       :1;                                       /* Bit 14 */
        byte BIT15       :1;                                       /* Bit 15 */
      } Bits;
    } ATD0DR3HSTR;
    #define ATD0DR3H                    _ATD0DR3.Overlap_STR.ATD0DR3HSTR.Byte
    #define ATD0DR3H_BIT8               _ATD0DR3.Overlap_STR.ATD0DR3HSTR.Bits.BIT8
    #define ATD0DR3H_BIT9               _ATD0DR3.Overlap_STR.ATD0DR3HSTR.Bits.BIT9
    #define ATD0DR3H_BIT10              _ATD0DR3.Overlap_STR.ATD0DR3HSTR.Bits.BIT10
    #define ATD0DR3H_BIT11              _ATD0DR3.Overlap_STR.ATD0DR3HSTR.Bits.BIT11
    #define ATD0DR3H_BIT12              _ATD0DR3.Overlap_STR.ATD0DR3HSTR.Bits.BIT12
    #define ATD0DR3H_BIT13              _ATD0DR3.Overlap_STR.ATD0DR3HSTR.Bits.BIT13
    #define ATD0DR3H_BIT14              _ATD0DR3.Overlap_STR.ATD0DR3HSTR.Bits.BIT14
    #define ATD0DR3H_BIT15              _ATD0DR3.Overlap_STR.ATD0DR3HSTR.Bits.BIT15
    
    #define ATD0DR3H_BIT8_MASK          1U
    #define ATD0DR3H_BIT9_MASK          2U
    #define ATD0DR3H_BIT10_MASK         4U
    #define ATD0DR3H_BIT11_MASK         8U
    #define ATD0DR3H_BIT12_MASK         16U
    #define ATD0DR3H_BIT13_MASK         32U
    #define ATD0DR3H_BIT14_MASK         64U
    #define ATD0DR3H_BIT15_MASK         128U
    

    /*** ATD0DR3L - ATD 0 Conversion Result Register 3 Low; 0x00000097 ***/
    union {
      byte Byte;
      struct {
        byte             :1; 
        byte             :1; 
        byte             :1; 
        byte             :1; 
        byte             :1; 
        byte             :1; 
        byte BIT6        :1;                                       /* Bit 6 */
        byte BIT7        :1;                                       /* Bit 7 */
      } Bits;
      struct {
        byte     :1;
        byte     :1;
        byte     :1;
        byte     :1;
        byte     :1;
        byte     :1;
        byte grpBIT_6 :2;
      } MergedBits;
    } ATD0DR3LSTR;
    #define ATD0DR3L                    _ATD0DR3.Overlap_STR.ATD0DR3LSTR.Byte
    #define ATD0DR3L_BIT6               _ATD0DR3.Overlap_STR.ATD0DR3LSTR.Bits.BIT6
    #define ATD0DR3L_BIT7               _ATD0DR3.Overlap_STR.ATD0DR3LSTR.Bits.BIT7
    #define ATD0DR3L_BIT_6              _ATD0DR3.Overlap_STR.ATD0DR3LSTR.MergedBits.grpBIT_6
    #define ATD0DR3L_BIT                ATD0DR3L_BIT_6
    
    #define ATD0DR3L_BIT6_MASK          64U
    #define ATD0DR3L_BIT7_MASK          128U
    #define ATD0DR3L_BIT_6_MASK         192U
    #define ATD0DR3L_BIT_6_BITNUM       6U
    
  } Overlap_STR;

  struct {
    word             :1; 
    word             :1; 
    word             :1; 
    word             :1; 
    word             :1; 
    word             :1; 
    word BIT6        :1;                                       /* Bit 6 */
    word BIT7        :1;                                       /* Bit 7 */
    word BIT8        :1;                                       /* Bit 8 */
    word BIT9        :1;                                       /* Bit 9 */
    word BIT10       :1;                                       /* Bit 10 */
    word BIT11       :1;                                       /* Bit 11 */
    word BIT12       :1;                                       /* Bit 12 */
    word BIT13       :1;                                       /* Bit 13 */
    word BIT14       :1;                                       /* Bit 14 */
    word BIT15       :1;                                       /* Bit 15 */
  } Bits;
  struct {
    word         :1;
    word         :1;
    word         :1;
    word         :1;
    word         :1;
    word         :1;
    word grpBIT_6 :10;
  } MergedBits;
} ATD0DR3STR;
extern volatile ATD0DR3STR _ATD0DR3 @(REG_BASE + 0x00000096UL);
#define ATD0DR3                         _ATD0DR3.Word
#define ATD0DR3_BIT6                    _ATD0DR3.Bits.BIT6
#define ATD0DR3_BIT7                    _ATD0DR3.Bits.BIT7
#define ATD0DR3_BIT8                    _ATD0DR3.Bits.BIT8
#define ATD0DR3_BIT9                    _ATD0DR3.Bits.BIT9
#define ATD0DR3_BIT10                   _ATD0DR3.Bits.BIT10
#define ATD0DR3_BIT11                   _ATD0DR3.Bits.BIT11
#define ATD0DR3_BIT12                   _ATD0DR3.Bits.BIT12
#define ATD0DR3_BIT13                   _ATD0DR3.Bits.BIT13
#define ATD0DR3_BIT14                   _ATD0DR3.Bits.BIT14
#define ATD0DR3_BIT15                   _ATD0DR3.Bits.BIT15
#define ATD0DR3_BIT_6                   _ATD0DR3.MergedBits.grpBIT_6
#define ATD0DR3_BIT                     ATD0DR3_BIT_6

#define ATD0DR3_BIT6_MASK               64U
#define ATD0DR3_BIT7_MASK               128U
#define ATD0DR3_BIT8_MASK               256U
#define ATD0DR3_BIT9_MASK               512U
#define ATD0DR3_BIT10_MASK              1024U
#define ATD0DR3_BIT11_MASK              2048U
#define ATD0DR3_BIT12_MASK              4096U
#define ATD0DR3_BIT13_MASK              8192U
#define ATD0DR3_BIT14_MASK              16384U
#define ATD0DR3_BIT15_MASK              32768U
#define ATD0DR3_BIT_6_MASK              65472U
#define ATD0DR3_BIT_6_BITNUM            6U


/*** ATD0DR4 - ATD 0 Conversion Result Register 4; 0x00000098 ***/
typedef union {
  word Word;
   /* Overlapped registers: */
  struct {
    /*** ATD0DR4H - ATD 0 Conversion Result Register 4 High; 0x00000098 ***/
    union {
      byte Byte;
      struct {
        byte BIT8        :1;                                       /* Bit 8 */
        byte BIT9        :1;                                       /* Bit 9 */
        byte BIT10       :1;                                       /* Bit 10 */
        byte BIT11       :1;                                       /* Bit 11 */
        byte BIT12       :1;                                       /* Bit 12 */
        byte BIT13       :1;                                       /* Bit 13 */
        byte BIT14       :1;                                       /* Bit 14 */
        byte BIT15       :1;                                       /* Bit 15 */
      } Bits;
    } ATD0DR4HSTR;
    #define ATD0DR4H                    _ATD0DR4.Overlap_STR.ATD0DR4HSTR.Byte
    #define ATD0DR4H_BIT8               _ATD0DR4.Overlap_STR.ATD0DR4HSTR.Bits.BIT8
    #define ATD0DR4H_BIT9               _ATD0DR4.Overlap_STR.ATD0DR4HSTR.Bits.BIT9
    #define ATD0DR4H_BIT10              _ATD0DR4.Overlap_STR.ATD0DR4HSTR.Bits.BIT10
    #define ATD0DR4H_BIT11              _ATD0DR4.Overlap_STR.ATD0DR4HSTR.Bits.BIT11
    #define ATD0DR4H_BIT12              _ATD0DR4.Overlap_STR.ATD0DR4HSTR.Bits.BIT12
    #define ATD0DR4H_BIT13              _ATD0DR4.Overlap_STR.ATD0DR4HSTR.Bits.BIT13
    #define ATD0DR4H_BIT14              _ATD0DR4.Overlap_STR.ATD0DR4HSTR.Bits.BIT14
    #define ATD0DR4H_BIT15              _ATD0DR4.Overlap_STR.ATD0DR4HSTR.Bits.BIT15
    
    #define ATD0DR4H_BIT8_MASK          1U
    #define ATD0DR4H_BIT9_MASK          2U
    #define ATD0DR4H_BIT10_MASK         4U
    #define ATD0DR4H_BIT11_MASK         8U
    #define ATD0DR4H_BIT12_MASK         16U
    #define ATD0DR4H_BIT13_MASK         32U
    #define ATD0DR4H_BIT14_MASK         64U
    #define ATD0DR4H_BIT15_MASK         128U
    

    /*** ATD0DR4L - ATD 0 Conversion Result Register 4 Low; 0x00000099 ***/
    union {
      byte Byte;
      struct {
        byte             :1; 
        byte             :1; 
        byte             :1; 
        byte             :1; 
        byte             :1; 
        byte             :1; 
        byte BIT6        :1;                                       /* Bit 6 */
        byte BIT7        :1;                                       /* Bit 7 */
      } Bits;
      struct {
        byte     :1;
        byte     :1;
        byte     :1;
        byte     :1;
        byte     :1;
        byte     :1;
        byte grpBIT_6 :2;
      } MergedBits;
    } ATD0DR4LSTR;
    #define ATD0DR4L                    _ATD0DR4.Overlap_STR.ATD0DR4LSTR.Byte
    #define ATD0DR4L_BIT6               _ATD0DR4.Overlap_STR.ATD0DR4LSTR.Bits.BIT6
    #define ATD0DR4L_BIT7               _ATD0DR4.Overlap_STR.ATD0DR4LSTR.Bits.BIT7
    #define ATD0DR4L_BIT_6              _ATD0DR4.Overlap_STR.ATD0DR4LSTR.MergedBits.grpBIT_6
    #define ATD0DR4L_BIT                ATD0DR4L_BIT_6
    
    #define ATD0DR4L_BIT6_MASK          64U
    #define ATD0DR4L_BIT7_MASK          128U
    #define ATD0DR4L_BIT_6_MASK         192U
    #define ATD0DR4L_BIT_6_BITNUM       6U
    
  } Overlap_STR;

  struct {
    word             :1; 
    word             :1; 
    word             :1; 
    word             :1; 
    word             :1; 
    word             :1; 
    word BIT6        :1;                                       /* Bit 6 */
    word BIT7        :1;                                       /* Bit 7 */
    word BIT8        :1;                                       /* Bit 8 */
    word BIT9        :1;                                       /* Bit 9 */
    word BIT10       :1;                                       /* Bit 10 */
    word BIT11       :1;                                       /* Bit 11 */
    word BIT12       :1;                                       /* Bit 12 */
    word BIT13       :1;                                       /* Bit 13 */
    word BIT14       :1;                                       /* Bit 14 */
    word BIT15       :1;                                       /* Bit 15 */
  } Bits;
  struct {
    word         :1;
    word         :1;
    word         :1;
    word         :1;
    word         :1;
    word         :1;
    word grpBIT_6 :10;
  } MergedBits;
} ATD0DR4STR;
extern volatile ATD0DR4STR _ATD0DR4 @(REG_BASE + 0x00000098UL);
#define ATD0DR4                         _ATD0DR4.Word
#define ATD0DR4_BIT6                    _ATD0DR4.Bits.BIT6
#define ATD0DR4_BIT7                    _ATD0DR4.Bits.BIT7
#define ATD0DR4_BIT8                    _ATD0DR4.Bits.BIT8
#define ATD0DR4_BIT9                    _ATD0DR4.Bits.BIT9
#define ATD0DR4_BIT10                   _ATD0DR4.Bits.BIT10
#define ATD0DR4_BIT11                   _ATD0DR4.Bits.BIT11
#define ATD0DR4_BIT12                   _ATD0DR4.Bits.BIT12
#define ATD0DR4_BIT13                   _ATD0DR4.Bits.BIT13
#define ATD0DR4_BIT14                   _ATD0DR4.Bits.BIT14
#define ATD0DR4_BIT15                   _ATD0DR4.Bits.BIT15
#define ATD0DR4_BIT_6                   _ATD0DR4.MergedBits.grpBIT_6
#define ATD0DR4_BIT                     ATD0DR4_BIT_6

#define ATD0DR4_BIT6_MASK               64U
#define ATD0DR4_BIT7_MASK               128U
#define ATD0DR4_BIT8_MASK               256U
#define ATD0DR4_BIT9_MASK               512U
#define ATD0DR4_BIT10_MASK              1024U
#define ATD0DR4_BIT11_MASK              2048U
#define ATD0DR4_BIT12_MASK              4096U
#define ATD0DR4_BIT13_MASK              8192U
#define ATD0DR4_BIT14_MASK              16384U
#define ATD0DR4_BIT15_MASK              32768U
#define ATD0DR4_BIT_6_MASK              65472U
#define ATD0DR4_BIT_6_BITNUM            6U


/*** ATD0DR5 - ATD 0 Conversion Result Register 5; 0x0000009A ***/
typedef union {
  word Word;
   /* Overlapped registers: */
  struct {
    /*** ATD0DR5H - ATD 0 Conversion Result Register 5 High; 0x0000009A ***/
    union {
      byte Byte;
      struct {
        byte BIT8        :1;                                       /* Bit 8 */
        byte BIT9        :1;                                       /* Bit 9 */
        byte BIT10       :1;                                       /* Bit 10 */
        byte BIT11       :1;                                       /* Bit 11 */
        byte BIT12       :1;                                       /* Bit 12 */
        byte BIT13       :1;                                       /* Bit 13 */
        byte BIT14       :1;                                       /* Bit 14 */
        byte BIT15       :1;                                       /* Bit 15 */
      } Bits;
    } ATD0DR5HSTR;
    #define ATD0DR5H                    _ATD0DR5.Overlap_STR.ATD0DR5HSTR.Byte
    #define ATD0DR5H_BIT8               _ATD0DR5.Overlap_STR.ATD0DR5HSTR.Bits.BIT8
    #define ATD0DR5H_BIT9               _ATD0DR5.Overlap_STR.ATD0DR5HSTR.Bits.BIT9
    #define ATD0DR5H_BIT10              _ATD0DR5.Overlap_STR.ATD0DR5HSTR.Bits.BIT10
    #define ATD0DR5H_BIT11              _ATD0DR5.Overlap_STR.ATD0DR5HSTR.Bits.BIT11
    #define ATD0DR5H_BIT12              _ATD0DR5.Overlap_STR.ATD0DR5HSTR.Bits.BIT12
    #define ATD0DR5H_BIT13              _ATD0DR5.Overlap_STR.ATD0DR5HSTR.Bits.BIT13
    #define ATD0DR5H_BIT14              _ATD0DR5.Overlap_STR.ATD0DR5HSTR.Bits.BIT14
    #define ATD0DR5H_BIT15              _ATD0DR5.Overlap_STR.ATD0DR5HSTR.Bits.BIT15
    
    #define ATD0DR5H_BIT8_MASK          1U
    #define ATD0DR5H_BIT9_MASK          2U
    #define ATD0DR5H_BIT10_MASK         4U
    #define ATD0DR5H_BIT11_MASK         8U
    #define ATD0DR5H_BIT12_MASK         16U
    #define ATD0DR5H_BIT13_MASK         32U
    #define ATD0DR5H_BIT14_MASK         64U
    #define ATD0DR5H_BIT15_MASK         128U
    

    /*** ATD0DR5L - ATD 0 Conversion Result Register 5 Low; 0x0000009B ***/
    union {
      byte Byte;
      struct {
        byte             :1; 
        byte             :1; 
        byte             :1; 
        byte             :1; 
        byte             :1; 
        byte             :1; 
        byte BIT6        :1;                                       /* Bit 6 */
        byte BIT7        :1;                                       /* Bit 7 */
      } Bits;
      struct {
        byte     :1;
        byte     :1;
        byte     :1;
        byte     :1;
        byte     :1;
        byte     :1;
        byte grpBIT_6 :2;
      } MergedBits;
    } ATD0DR5LSTR;
    #define ATD0DR5L                    _ATD0DR5.Overlap_STR.ATD0DR5LSTR.Byte
    #define ATD0DR5L_BIT6               _ATD0DR5.Overlap_STR.ATD0DR5LSTR.Bits.BIT6
    #define ATD0DR5L_BIT7               _ATD0DR5.Overlap_STR.ATD0DR5LSTR.Bits.BIT7
    #define ATD0DR5L_BIT_6              _ATD0DR5.Overlap_STR.ATD0DR5LSTR.MergedBits.grpBIT_6
    #define ATD0DR5L_BIT                ATD0DR5L_BIT_6
    
    #define ATD0DR5L_BIT6_MASK          64U
    #define ATD0DR5L_BIT7_MASK          128U
    #define ATD0DR5L_BIT_6_MASK         192U
    #define ATD0DR5L_BIT_6_BITNUM       6U
    
  } Overlap_STR;

  struct {
    word             :1; 
    word             :1; 
    word             :1; 
    word             :1; 
    word             :1; 
    word             :1; 
    word BIT6        :1;                                       /* Bit 6 */
    word BIT7        :1;                                       /* Bit 7 */
    word BIT8        :1;                                       /* Bit 8 */
    word BIT9        :1;                                       /* Bit 9 */
    word BIT10       :1;                                       /* Bit 10 */
    word BIT11       :1;                                       /* Bit 11 */
    word BIT12       :1;                                       /* Bit 12 */
    word BIT13       :1;                                       /* Bit 13 */
    word BIT14       :1;                                       /* Bit 14 */
    word BIT15       :1;                                       /* Bit 15 */
  } Bits;
  struct {
    word         :1;
    word         :1;
    word         :1;
    word         :1;
    word         :1;
    word         :1;
    word grpBIT_6 :10;
  } MergedBits;
} ATD0DR5STR;
extern volatile ATD0DR5STR _ATD0DR5 @(REG_BASE + 0x0000009AUL);
#define ATD0DR5                         _ATD0DR5.Word
#define ATD0DR5_BIT6                    _ATD0DR5.Bits.BIT6
#define ATD0DR5_BIT7                    _ATD0DR5.Bits.BIT7
#define ATD0DR5_BIT8                    _ATD0DR5.Bits.BIT8
#define ATD0DR5_BIT9                    _ATD0DR5.Bits.BIT9
#define ATD0DR5_BIT10                   _ATD0DR5.Bits.BIT10
#define ATD0DR5_BIT11                   _ATD0DR5.Bits.BIT11
#define ATD0DR5_BIT12                   _ATD0DR5.Bits.BIT12
#define ATD0DR5_BIT13                   _ATD0DR5.Bits.BIT13
#define ATD0DR5_BIT14                   _ATD0DR5.Bits.BIT14
#define ATD0DR5_BIT15                   _ATD0DR5.Bits.BIT15
#define ATD0DR5_BIT_6                   _ATD0DR5.MergedBits.grpBIT_6
#define ATD0DR5_BIT                     ATD0DR5_BIT_6

#define ATD0DR5_BIT6_MASK               64U
#define ATD0DR5_BIT7_MASK               128U
#define ATD0DR5_BIT8_MASK               256U
#define ATD0DR5_BIT9_MASK               512U
#define ATD0DR5_BIT10_MASK              1024U
#define ATD0DR5_BIT11_MASK              2048U
#define ATD0DR5_BIT12_MASK              4096U
#define ATD0DR5_BIT13_MASK              8192U
#define ATD0DR5_BIT14_MASK              16384U
#define ATD0DR5_BIT15_MASK              32768U
#define ATD0DR5_BIT_6_MASK              65472U
#define ATD0DR5_BIT_6_BITNUM            6U


/*** ATD0DR6 - ATD 0 Conversion Result Register 6; 0x0000009C ***/
typedef union {
  word Word;
   /* Overlapped registers: */
  struct {
    /*** ATD0DR6H - ATD 0 Conversion Result Register 6 High; 0x0000009C ***/
    union {
      byte Byte;
      struct {
        byte BIT8        :1;                                       /* Bit 8 */
        byte BIT9        :1;                                       /* Bit 9 */
        byte BIT10       :1;                                       /* Bit 10 */
        byte BIT11       :1;                                       /* Bit 11 */
        byte BIT12       :1;                                       /* Bit 12 */
        byte BIT13       :1;                                       /* Bit 13 */
        byte BIT14       :1;                                       /* Bit 14 */
        byte BIT15       :1;                                       /* Bit 15 */
      } Bits;
    } ATD0DR6HSTR;
    #define ATD0DR6H                    _ATD0DR6.Overlap_STR.ATD0DR6HSTR.Byte
    #define ATD0DR6H_BIT8               _ATD0DR6.Overlap_STR.ATD0DR6HSTR.Bits.BIT8
    #define ATD0DR6H_BIT9               _ATD0DR6.Overlap_STR.ATD0DR6HSTR.Bits.BIT9
    #define ATD0DR6H_BIT10              _ATD0DR6.Overlap_STR.ATD0DR6HSTR.Bits.BIT10
    #define ATD0DR6H_BIT11              _ATD0DR6.Overlap_STR.ATD0DR6HSTR.Bits.BIT11
    #define ATD0DR6H_BIT12              _ATD0DR6.Overlap_STR.ATD0DR6HSTR.Bits.BIT12
    #define ATD0DR6H_BIT13              _ATD0DR6.Overlap_STR.ATD0DR6HSTR.Bits.BIT13
    #define ATD0DR6H_BIT14              _ATD0DR6.Overlap_STR.ATD0DR6HSTR.Bits.BIT14
    #define ATD0DR6H_BIT15              _ATD0DR6.Overlap_STR.ATD0DR6HSTR.Bits.BIT15
    
    #define ATD0DR6H_BIT8_MASK          1U
    #define ATD0DR6H_BIT9_MASK          2U
    #define ATD0DR6H_BIT10_MASK         4U
    #define ATD0DR6H_BIT11_MASK         8U
    #define ATD0DR6H_BIT12_MASK         16U
    #define ATD0DR6H_BIT13_MASK         32U
    #define ATD0DR6H_BIT14_MASK         64U
    #define ATD0DR6H_BIT15_MASK         128U
    

    /*** ATD0DR6L - ATD 0 Conversion Result Register 6 Low; 0x0000009D ***/
    union {
      byte Byte;
      struct {
        byte             :1; 
        byte             :1; 
        byte             :1; 
        byte             :1; 
        byte             :1; 
        byte             :1; 
        byte BIT6        :1;                                       /* Bit 6 */
        byte BIT7        :1;                                       /* Bit 7 */
      } Bits;
      struct {
        byte     :1;
        byte     :1;
        byte     :1;
        byte     :1;
        byte     :1;
        byte     :1;
        byte grpBIT_6 :2;
      } MergedBits;
    } ATD0DR6LSTR;
    #define ATD0DR6L                    _ATD0DR6.Overlap_STR.ATD0DR6LSTR.Byte
    #define ATD0DR6L_BIT6               _ATD0DR6.Overlap_STR.ATD0DR6LSTR.Bits.BIT6
    #define ATD0DR6L_BIT7               _ATD0DR6.Overlap_STR.ATD0DR6LSTR.Bits.BIT7
    #define ATD0DR6L_BIT_6              _ATD0DR6.Overlap_STR.ATD0DR6LSTR.MergedBits.grpBIT_6
    #define ATD0DR6L_BIT                ATD0DR6L_BIT_6
    
    #define ATD0DR6L_BIT6_MASK          64U
    #define ATD0DR6L_BIT7_MASK          128U
    #define ATD0DR6L_BIT_6_MASK         192U
    #define ATD0DR6L_BIT_6_BITNUM       6U
    
  } Overlap_STR;

  struct {
    word             :1; 
    word             :1; 
    word             :1; 
    word             :1; 
    word             :1; 
    word             :1; 
    word BIT6        :1;                                       /* Bit 6 */
    word BIT7        :1;                                       /* Bit 7 */
    word BIT8        :1;                                       /* Bit 8 */
    word BIT9        :1;                                       /* Bit 9 */
    word BIT10       :1;                                       /* Bit 10 */
    word BIT11       :1;                                       /* Bit 11 */
    word BIT12       :1;                                       /* Bit 12 */
    word BIT13       :1;                                       /* Bit 13 */
    word BIT14       :1;                                       /* Bit 14 */
    word BIT15       :1;                                       /* Bit 15 */
  } Bits;
  struct {
    word         :1;
    word         :1;
    word         :1;
    word         :1;
    word         :1;
    word         :1;
    word grpBIT_6 :10;
  } MergedBits;
} ATD0DR6STR;
extern volatile ATD0DR6STR _ATD0DR6 @(REG_BASE + 0x0000009CUL);
#define ATD0DR6                         _ATD0DR6.Word
#define ATD0DR6_BIT6                    _ATD0DR6.Bits.BIT6
#define ATD0DR6_BIT7                    _ATD0DR6.Bits.BIT7
#define ATD0DR6_BIT8                    _ATD0DR6.Bits.BIT8
#define ATD0DR6_BIT9                    _ATD0DR6.Bits.BIT9
#define ATD0DR6_BIT10                   _ATD0DR6.Bits.BIT10
#define ATD0DR6_BIT11                   _ATD0DR6.Bits.BIT11
#define ATD0DR6_BIT12                   _ATD0DR6.Bits.BIT12
#define ATD0DR6_BIT13                   _ATD0DR6.Bits.BIT13
#define ATD0DR6_BIT14                   _ATD0DR6.Bits.BIT14
#define ATD0DR6_BIT15                   _ATD0DR6.Bits.BIT15
#define ATD0DR6_BIT_6                   _ATD0DR6.MergedBits.grpBIT_6
#define ATD0DR6_BIT                     ATD0DR6_BIT_6

#define ATD0DR6_BIT6_MASK               64U
#define ATD0DR6_BIT7_MASK               128U
#define ATD0DR6_BIT8_MASK               256U
#define ATD0DR6_BIT9_MASK               512U
#define ATD0DR6_BIT10_MASK              1024U
#define ATD0DR6_BIT11_MASK              2048U
#define ATD0DR6_BIT12_MASK              4096U
#define ATD0DR6_BIT13_MASK              8192U
#define ATD0DR6_BIT14_MASK              16384U
#define ATD0DR6_BIT15_MASK              32768U
#define ATD0DR6_BIT_6_MASK              65472U
#define ATD0DR6_BIT_6_BITNUM            6U


/*** ATD0DR7 - ATD 0 Conversion Result Register 7; 0x0000009E ***/
typedef union {
  word Word;
   /* Overlapped registers: */
  struct {
    /*** ATD0DR7H - ATD 0 Conversion Result Register 7 High; 0x0000009E ***/
    union {
      byte Byte;
      struct {
        byte BIT8        :1;                                       /* Bit 8 */
        byte BIT9        :1;                                       /* Bit 9 */
        byte BIT10       :1;                                       /* Bit 10 */
        byte BIT11       :1;                                       /* Bit 11 */
        byte BIT12       :1;                                       /* Bit 12 */
        byte BIT13       :1;                                       /* Bit 13 */
        byte BIT14       :1;                                       /* Bit 14 */
        byte BIT15       :1;                                       /* Bit 15 */
      } Bits;
    } ATD0DR7HSTR;
    #define ATD0DR7H                    _ATD0DR7.Overlap_STR.ATD0DR7HSTR.Byte
    #define ATD0DR7H_BIT8               _ATD0DR7.Overlap_STR.ATD0DR7HSTR.Bits.BIT8
    #define ATD0DR7H_BIT9               _ATD0DR7.Overlap_STR.ATD0DR7HSTR.Bits.BIT9
    #define ATD0DR7H_BIT10              _ATD0DR7.Overlap_STR.ATD0DR7HSTR.Bits.BIT10
    #define ATD0DR7H_BIT11              _ATD0DR7.Overlap_STR.ATD0DR7HSTR.Bits.BIT11
    #define ATD0DR7H_BIT12              _ATD0DR7.Overlap_STR.ATD0DR7HSTR.Bits.BIT12
    #define ATD0DR7H_BIT13              _ATD0DR7.Overlap_STR.ATD0DR7HSTR.Bits.BIT13
    #define ATD0DR7H_BIT14              _ATD0DR7.Overlap_STR.ATD0DR7HSTR.Bits.BIT14
    #define ATD0DR7H_BIT15              _ATD0DR7.Overlap_STR.ATD0DR7HSTR.Bits.BIT15
    
    #define ATD0DR7H_BIT8_MASK          1U
    #define ATD0DR7H_BIT9_MASK          2U
    #define ATD0DR7H_BIT10_MASK         4U
    #define ATD0DR7H_BIT11_MASK         8U
    #define ATD0DR7H_BIT12_MASK         16U
    #define ATD0DR7H_BIT13_MASK         32U
    #define ATD0DR7H_BIT14_MASK         64U
    #define ATD0DR7H_BIT15_MASK         128U
    

    /*** ATD0DR7L - ATD 0 Conversion Result Register 7 Low; 0x0000009F ***/
    union {
      byte Byte;
      struct {
        byte             :1; 
        byte             :1; 
        byte             :1; 
        byte             :1; 
        byte             :1; 
        byte             :1; 
        byte BIT6        :1;                                       /* Bit 6 */
        byte BIT7        :1;                                       /* Bit 7 */
      } Bits;
      struct {
        byte     :1;
        byte     :1;
        byte     :1;
        byte     :1;
        byte     :1;
        byte     :1;
        byte grpBIT_6 :2;
      } MergedBits;
    } ATD0DR7LSTR;
    #define ATD0DR7L                    _ATD0DR7.Overlap_STR.ATD0DR7LSTR.Byte
    #define ATD0DR7L_BIT6               _ATD0DR7.Overlap_STR.ATD0DR7LSTR.Bits.BIT6
    #define ATD0DR7L_BIT7               _ATD0DR7.Overlap_STR.ATD0DR7LSTR.Bits.BIT7
    #define ATD0DR7L_BIT_6              _ATD0DR7.Overlap_STR.ATD0DR7LSTR.MergedBits.grpBIT_6
    #define ATD0DR7L_BIT                ATD0DR7L_BIT_6
    
    #define ATD0DR7L_BIT6_MASK          64U
    #define ATD0DR7L_BIT7_MASK          128U
    #define ATD0DR7L_BIT_6_MASK         192U
    #define ATD0DR7L_BIT_6_BITNUM       6U
    
  } Overlap_STR;

  struct {
    word             :1; 
    word             :1; 
    word             :1; 
    word             :1; 
    word             :1; 
    word             :1; 
    word BIT6        :1;                                       /* Bit 6 */
    word BIT7        :1;                                       /* Bit 7 */
    word BIT8        :1;                                       /* Bit 8 */
    word BIT9        :1;                                       /* Bit 9 */
    word BIT10       :1;                                       /* Bit 10 */
    word BIT11       :1;                                       /* Bit 11 */
    word BIT12       :1;                                       /* Bit 12 */
    word BIT13       :1;                                       /* Bit 13 */
    word BIT14       :1;                                       /* Bit 14 */
    word BIT15       :1;                                       /* Bit 15 */
  } Bits;
  struct {
    word         :1;
    word         :1;
    word         :1;
    word         :1;
    word         :1;
    word         :1;
    word grpBIT_6 :10;
  } MergedBits;
} ATD0DR7STR;
extern volatile ATD0DR7STR _ATD0DR7 @(REG_BASE + 0x0000009EUL);
#define ATD0DR7                         _ATD0DR7.Word
#define ATD0DR7_BIT6                    _ATD0DR7.Bits.BIT6
#define ATD0DR7_BIT7                    _ATD0DR7.Bits.BIT7
#define ATD0DR7_BIT8                    _ATD0DR7.Bits.BIT8
#define ATD0DR7_BIT9                    _ATD0DR7.Bits.BIT9
#define ATD0DR7_BIT10                   _ATD0DR7.Bits.BIT10
#define ATD0DR7_BIT11                   _ATD0DR7.Bits.BIT11
#define ATD0DR7_BIT12                   _ATD0DR7.Bits.BIT12
#define ATD0DR7_BIT13                   _ATD0DR7.Bits.BIT13
#define ATD0DR7_BIT14                   _ATD0DR7.Bits.BIT14
#define ATD0DR7_BIT15                   _ATD0DR7.Bits.BIT15
#define ATD0DR7_BIT_6                   _ATD0DR7.MergedBits.grpBIT_6
#define ATD0DR7_BIT                     ATD0DR7_BIT_6

#define ATD0DR7_BIT6_MASK               64U
#define ATD0DR7_BIT7_MASK               128U
#define ATD0DR7_BIT8_MASK               256U
#define ATD0DR7_BIT9_MASK               512U
#define ATD0DR7_BIT10_MASK              1024U
#define ATD0DR7_BIT11_MASK              2048U
#define ATD0DR7_BIT12_MASK              4096U
#define ATD0DR7_BIT13_MASK              8192U
#define ATD0DR7_BIT14_MASK              16384U
#define ATD0DR7_BIT15_MASK              32768U
#define ATD0DR7_BIT_6_MASK              65472U
#define ATD0DR7_BIT_6_BITNUM            6U


/*** PWME - PWM Enable Register; 0x000000A0 ***/
typedef union {
  byte Byte;
  struct {
    byte PWME0       :1;                                       /* Pulse Width Channel 0 Enable */
    byte PWME1       :1;                                       /* Pulse Width Channel 1 Enable */
    byte PWME2       :1;                                       /* Pulse Width Channel 2 Enable */
    byte PWME3       :1;                                       /* Pulse Width Channel 3 Enable */
    byte PWME4       :1;                                       /* Pulse Width Channel 4 Enable */
    byte PWME5       :1;                                       /* Pulse Width Channel 5 Enable */
    byte PWME6       :1;                                       /* Pulse Width Channel 6 Enable */
    byte PWME7       :1;                                       /* Pulse Width Channel 7 Enable */
  } Bits;
} PWMESTR;
extern volatile PWMESTR _PWME @(REG_BASE + 0x000000A0UL);
#define PWME                            _PWME.Byte
#define PWME_PWME0                      _PWME.Bits.PWME0
#define PWME_PWME1                      _PWME.Bits.PWME1
#define PWME_PWME2                      _PWME.Bits.PWME2
#define PWME_PWME3                      _PWME.Bits.PWME3
#define PWME_PWME4                      _PWME.Bits.PWME4
#define PWME_PWME5                      _PWME.Bits.PWME5
#define PWME_PWME6                      _PWME.Bits.PWME6
#define PWME_PWME7                      _PWME.Bits.PWME7

#define PWME_PWME0_MASK                 1U
#define PWME_PWME1_MASK                 2U
#define PWME_PWME2_MASK                 4U
#define PWME_PWME3_MASK                 8U
#define PWME_PWME4_MASK                 16U
#define PWME_PWME5_MASK                 32U
#define PWME_PWME6_MASK                 64U
#define PWME_PWME7_MASK                 128U


/*** PWMPOL - PWM Polarity Register; 0x000000A1 ***/
typedef union {
  byte Byte;
  struct {
    byte PPOL0       :1;                                       /* Pulse Width Channel 0 Polarity */
    byte PPOL1       :1;                                       /* Pulse Width Channel 1 Polarity */
    byte PPOL2       :1;                                       /* Pulse Width Channel 2 Polarity */
    byte PPOL3       :1;                                       /* Pulse Width Channel 3 Polarity */
    byte PPOL4       :1;                                       /* Pulse Width Channel 4 Polarity */
    byte PPOL5       :1;                                       /* Pulse Width Channel 5 Polarity */
    byte PPOL6       :1;                                       /* Pulse Width Channel 6 Polarity */
    byte PPOL7       :1;                                       /* Pulse Width Channel 7 Polarity */
  } Bits;
} PWMPOLSTR;
extern volatile PWMPOLSTR _PWMPOL @(REG_BASE + 0x000000A1UL);
#define PWMPOL                          _PWMPOL.Byte
#define PWMPOL_PPOL0                    _PWMPOL.Bits.PPOL0
#define PWMPOL_PPOL1                    _PWMPOL.Bits.PPOL1
#define PWMPOL_PPOL2                    _PWMPOL.Bits.PPOL2
#define PWMPOL_PPOL3                    _PWMPOL.Bits.PPOL3
#define PWMPOL_PPOL4                    _PWMPOL.Bits.PPOL4
#define PWMPOL_PPOL5                    _PWMPOL.Bits.PPOL5
#define PWMPOL_PPOL6                    _PWMPOL.Bits.PPOL6
#define PWMPOL_PPOL7                    _PWMPOL.Bits.PPOL7

#define PWMPOL_PPOL0_MASK               1U
#define PWMPOL_PPOL1_MASK               2U
#define PWMPOL_PPOL2_MASK               4U
#define PWMPOL_PPOL3_MASK               8U
#define PWMPOL_PPOL4_MASK               16U
#define PWMPOL_PPOL5_MASK               32U
#define PWMPOL_PPOL6_MASK               64U
#define PWMPOL_PPOL7_MASK               128U


/*** PWMCLK - PWM Clock Select Register; 0x000000A2 ***/
typedef union {
  byte Byte;
  struct {
    byte PCLK0       :1;                                       /* Pulse Width Channel 0 Clock Select */
    byte PCLK1       :1;                                       /* Pulse Width Channel 1 Clock Select */
    byte PCLK2       :1;                                       /* Pulse Width Channel 2 Clock Select */
    byte PCLK3       :1;                                       /* Pulse Width Channel 3 Clock Select */
    byte PCLK4       :1;                                       /* Pulse Width Channel 4 Clock Select */
    byte PCLK5       :1;                                       /* Pulse Width Channel 5 Clock Select */
    byte PCLK6       :1;                                       /* Pulse Width Channel 6 Clock Select */
    byte PCLK7       :1;                                       /* Pulse Width Channel 7 Clock Select */
  } Bits;
} PWMCLKSTR;
extern volatile PWMCLKSTR _PWMCLK @(REG_BASE + 0x000000A2UL);
#define PWMCLK                          _PWMCLK.Byte
#define PWMCLK_PCLK0                    _PWMCLK.Bits.PCLK0
#define PWMCLK_PCLK1                    _PWMCLK.Bits.PCLK1
#define PWMCLK_PCLK2                    _PWMCLK.Bits.PCLK2
#define PWMCLK_PCLK3                    _PWMCLK.Bits.PCLK3
#define PWMCLK_PCLK4                    _PWMCLK.Bits.PCLK4
#define PWMCLK_PCLK5                    _PWMCLK.Bits.PCLK5
#define PWMCLK_PCLK6                    _PWMCLK.Bits.PCLK6
#define PWMCLK_PCLK7                    _PWMCLK.Bits.PCLK7

#define PWMCLK_PCLK0_MASK               1U
#define PWMCLK_PCLK1_MASK               2U
#define PWMCLK_PCLK2_MASK               4U
#define PWMCLK_PCLK3_MASK               8U
#define PWMCLK_PCLK4_MASK               16U
#define PWMCLK_PCLK5_MASK               32U
#define PWMCLK_PCLK6_MASK               64U
#define PWMCLK_PCLK7_MASK               128U


/*** PWMPRCLK - PWM Prescale Clock Select Register; 0x000000A3 ***/
typedef union {
  byte Byte;
  struct {
    byte PCKA0       :1;                                       /* Prescaler Select for Clock A 0 */
    byte PCKA1       :1;                                       /* Prescaler Select for Clock A 1 */
    byte PCKA2       :1;                                       /* Prescaler Select for Clock A 2 */
    byte             :1; 
    byte PCKB0       :1;                                       /* Prescaler Select for Clock B 0 */
    byte PCKB1       :1;                                       /* Prescaler Select for Clock B 1 */
    byte PCKB2       :1;                                       /* Prescaler Select for Clock B 2 */
    byte             :1; 
  } Bits;
  struct {
    byte grpPCKA :3;
    byte         :1;
    byte grpPCKB :3;
    byte         :1;
  } MergedBits;
} PWMPRCLKSTR;
extern volatile PWMPRCLKSTR _PWMPRCLK @(REG_BASE + 0x000000A3UL);
#define PWMPRCLK                        _PWMPRCLK.Byte
#define PWMPRCLK_PCKA0                  _PWMPRCLK.Bits.PCKA0
#define PWMPRCLK_PCKA1                  _PWMPRCLK.Bits.PCKA1
#define PWMPRCLK_PCKA2                  _PWMPRCLK.Bits.PCKA2
#define PWMPRCLK_PCKB0                  _PWMPRCLK.Bits.PCKB0
#define PWMPRCLK_PCKB1                  _PWMPRCLK.Bits.PCKB1
#define PWMPRCLK_PCKB2                  _PWMPRCLK.Bits.PCKB2
#define PWMPRCLK_PCKA                   _PWMPRCLK.MergedBits.grpPCKA
#define PWMPRCLK_PCKB                   _PWMPRCLK.MergedBits.grpPCKB

#define PWMPRCLK_PCKA0_MASK             1U
#define PWMPRCLK_PCKA1_MASK             2U
#define PWMPRCLK_PCKA2_MASK             4U
#define PWMPRCLK_PCKB0_MASK             16U
#define PWMPRCLK_PCKB1_MASK             32U
#define PWMPRCLK_PCKB2_MASK             64U
#define PWMPRCLK_PCKA_MASK              7U
#define PWMPRCLK_PCKA_BITNUM            0U
#define PWMPRCLK_PCKB_MASK              112U
#define PWMPRCLK_PCKB_BITNUM            4U


/*** PWMCAE - PWM Center Align Enable Register; 0x000000A4 ***/
typedef union {
  byte Byte;
  struct {
    byte CAE0        :1;                                       /* Center Aligned Output Mode on channel 0 */
    byte CAE1        :1;                                       /* Center Aligned Output Mode on channel 1 */
    byte CAE2        :1;                                       /* Center Aligned Output Mode on channel 2 */
    byte CAE3        :1;                                       /* Center Aligned Output Mode on channel 3 */
    byte CAE4        :1;                                       /* Center Aligned Output Mode on channel 4 */
    byte CAE5        :1;                                       /* Center Aligned Output Mode on channel 5 */
    byte CAE6        :1;                                       /* Center Aligned Output Mode on channel 6 */
    byte CAE7        :1;                                       /* Center Aligned Output Mode on channel 7 */
  } Bits;
} PWMCAESTR;
extern volatile PWMCAESTR _PWMCAE @(REG_BASE + 0x000000A4UL);
#define PWMCAE                          _PWMCAE.Byte
#define PWMCAE_CAE0                     _PWMCAE.Bits.CAE0
#define PWMCAE_CAE1                     _PWMCAE.Bits.CAE1
#define PWMCAE_CAE2                     _PWMCAE.Bits.CAE2
#define PWMCAE_CAE3                     _PWMCAE.Bits.CAE3
#define PWMCAE_CAE4                     _PWMCAE.Bits.CAE4
#define PWMCAE_CAE5                     _PWMCAE.Bits.CAE5
#define PWMCAE_CAE6                     _PWMCAE.Bits.CAE6
#define PWMCAE_CAE7                     _PWMCAE.Bits.CAE7

#define PWMCAE_CAE0_MASK                1U
#define PWMCAE_CAE1_MASK                2U
#define PWMCAE_CAE2_MASK                4U
#define PWMCAE_CAE3_MASK                8U
#define PWMCAE_CAE4_MASK                16U
#define PWMCAE_CAE5_MASK                32U
#define PWMCAE_CAE6_MASK                64U
#define PWMCAE_CAE7_MASK                128U


/*** PWMCTL - PWM Control Register; 0x000000A5 ***/
typedef union {
  byte Byte;
  struct {
    byte             :1; 
    byte             :1; 
    byte PFRZ        :1;                                       /* PWM Counters Stop in Freeze Mode */
    byte PSWAI       :1;                                       /* PWM Stops in Wait Mode */
    byte CON01       :1;                                       /* Concatenate channels 0 and 1 */
    byte CON23       :1;                                       /* Concatenate channels 2 and 3 */
    byte CON45       :1;                                       /* Concatenate channels 4 and 5 */
    byte CON67       :1;                                       /* Concatenate channels 6 and 7 */
  } Bits;
} PWMCTLSTR;
extern volatile PWMCTLSTR _PWMCTL @(REG_BASE + 0x000000A5UL);
#define PWMCTL                          _PWMCTL.Byte
#define PWMCTL_PFRZ                     _PWMCTL.Bits.PFRZ
#define PWMCTL_PSWAI                    _PWMCTL.Bits.PSWAI
#define PWMCTL_CON01                    _PWMCTL.Bits.CON01
#define PWMCTL_CON23                    _PWMCTL.Bits.CON23
#define PWMCTL_CON45                    _PWMCTL.Bits.CON45
#define PWMCTL_CON67                    _PWMCTL.Bits.CON67

#define PWMCTL_PFRZ_MASK                4U
#define PWMCTL_PSWAI_MASK               8U
#define PWMCTL_CON01_MASK               16U
#define PWMCTL_CON23_MASK               32U
#define PWMCTL_CON45_MASK               64U
#define PWMCTL_CON67_MASK               128U


/*** PWMSCLA - PWM Scale A Register; 0x000000A8 ***/
typedef union {
  byte Byte;
  struct {
    byte BIT0        :1;                                       /* PWM Scale A Bit 0 */
    byte BIT1        :1;                                       /* PWM Scale A Bit 1 */
    byte BIT2        :1;                                       /* PWM Scale A Bit 2 */
    byte BIT3        :1;                                       /* PWM Scale A Bit 3 */
    byte BIT4        :1;                                       /* PWM Scale A Bit 4 */
    byte BIT5        :1;                                       /* PWM Scale A Bit 5 */
    byte BIT6        :1;                                       /* PWM Scale A Bit 6 */
    byte BIT7        :1;                                       /* PWM Scale A Bit 7 */
  } Bits;
} PWMSCLASTR;
extern volatile PWMSCLASTR _PWMSCLA @(REG_BASE + 0x000000A8UL);
#define PWMSCLA                         _PWMSCLA.Byte
#define PWMSCLA_BIT0                    _PWMSCLA.Bits.BIT0
#define PWMSCLA_BIT1                    _PWMSCLA.Bits.BIT1
#define PWMSCLA_BIT2                    _PWMSCLA.Bits.BIT2
#define PWMSCLA_BIT3                    _PWMSCLA.Bits.BIT3
#define PWMSCLA_BIT4                    _PWMSCLA.Bits.BIT4
#define PWMSCLA_BIT5                    _PWMSCLA.Bits.BIT5
#define PWMSCLA_BIT6                    _PWMSCLA.Bits.BIT6
#define PWMSCLA_BIT7                    _PWMSCLA.Bits.BIT7

#define PWMSCLA_BIT0_MASK               1U
#define PWMSCLA_BIT1_MASK               2U
#define PWMSCLA_BIT2_MASK               4U
#define PWMSCLA_BIT3_MASK               8U
#define PWMSCLA_BIT4_MASK               16U
#define PWMSCLA_BIT5_MASK               32U
#define PWMSCLA_BIT6_MASK               64U
#define PWMSCLA_BIT7_MASK               128U


/*** PWMSCLB - PWM Scale B Register; 0x000000A9 ***/
typedef union {
  byte Byte;
  struct {
    byte BIT0        :1;                                       /* PWM Scale B Bit 0 */
    byte BIT1        :1;                                       /* PWM Scale B Bit 1 */
    byte BIT2        :1;                                       /* PWM Scale B Bit 2 */
    byte BIT3        :1;                                       /* PWM Scale B Bit 3 */
    byte BIT4        :1;                                       /* PWM Scale B Bit 4 */
    byte BIT5        :1;                                       /* PWM Scale B Bit 5 */
    byte BIT6        :1;                                       /* PWM Scale B Bit 6 */
    byte BIT7        :1;                                       /* PWM Scale B Bit 7 */
  } Bits;
} PWMSCLBSTR;
extern volatile PWMSCLBSTR _PWMSCLB @(REG_BASE + 0x000000A9UL);
#define PWMSCLB                         _PWMSCLB.Byte
#define PWMSCLB_BIT0                    _PWMSCLB.Bits.BIT0
#define PWMSCLB_BIT1                    _PWMSCLB.Bits.BIT1
#define PWMSCLB_BIT2                    _PWMSCLB.Bits.BIT2
#define PWMSCLB_BIT3                    _PWMSCLB.Bits.BIT3
#define PWMSCLB_BIT4                    _PWMSCLB.Bits.BIT4
#define PWMSCLB_BIT5                    _PWMSCLB.Bits.BIT5
#define PWMSCLB_BIT6                    _PWMSCLB.Bits.BIT6
#define PWMSCLB_BIT7                    _PWMSCLB.Bits.BIT7

#define PWMSCLB_BIT0_MASK               1U
#define PWMSCLB_BIT1_MASK               2U
#define PWMSCLB_BIT2_MASK               4U
#define PWMSCLB_BIT3_MASK               8U
#define PWMSCLB_BIT4_MASK               16U
#define PWMSCLB_BIT5_MASK               32U
#define PWMSCLB_BIT6_MASK               64U
#define PWMSCLB_BIT7_MASK               128U


/*** PWMCNT01 - PWM Channel Counter 01 Register; 0x000000AC ***/
typedef union {
  word Word;
   /* Overlapped registers: */
  struct {
    /*** PWMCNT0 - PWM Channel Counter 0 Register; 0x000000AC ***/
    union {
      byte Byte;
    } PWMCNT0STR;
    #define PWMCNT0                     _PWMCNT01.Overlap_STR.PWMCNT0STR.Byte
    /* PWMCNT_ARR: Access 8 PWMCNTx registers in an array */
    #define PWMCNT_ARR                  ((volatile byte *) &PWMCNT0)
    

    /*** PWMCNT1 - PWM Channel Counter 1 Register; 0x000000AD ***/
    union {
      byte Byte;
    } PWMCNT1STR;
    #define PWMCNT1                     _PWMCNT01.Overlap_STR.PWMCNT1STR.Byte
    
  } Overlap_STR;

} PWMCNT01STR;
extern volatile PWMCNT01STR _PWMCNT01 @(REG_BASE + 0x000000ACUL);
#define PWMCNT01                        _PWMCNT01.Word


/*** PWMCNT23 - PWM Channel Counter 23 Register; 0x000000AE ***/
typedef union {
  word Word;
   /* Overlapped registers: */
  struct {
    /*** PWMCNT2 - PWM Channel Counter 2 Register; 0x000000AE ***/
    union {
      byte Byte;
    } PWMCNT2STR;
    #define PWMCNT2                     _PWMCNT23.Overlap_STR.PWMCNT2STR.Byte
    

    /*** PWMCNT3 - PWM Channel Counter 3 Register; 0x000000AF ***/
    union {
      byte Byte;
    } PWMCNT3STR;
    #define PWMCNT3                     _PWMCNT23.Overlap_STR.PWMCNT3STR.Byte
    
  } Overlap_STR;

} PWMCNT23STR;
extern volatile PWMCNT23STR _PWMCNT23 @(REG_BASE + 0x000000AEUL);
#define PWMCNT23                        _PWMCNT23.Word


/*** PWMCNT45 - PWM Channel Counter 45 Register; 0x000000B0 ***/
typedef union {
  word Word;
   /* Overlapped registers: */
  struct {
    /*** PWMCNT4 - PWM Channel Counter 4 Register; 0x000000B0 ***/
    union {
      byte Byte;
    } PWMCNT4STR;
    #define PWMCNT4                     _PWMCNT45.Overlap_STR.PWMCNT4STR.Byte
    

    /*** PWMCNT5 - PWM Channel Counter 5 Register; 0x000000B1 ***/
    union {
      byte Byte;
    } PWMCNT5STR;
    #define PWMCNT5                     _PWMCNT45.Overlap_STR.PWMCNT5STR.Byte
    
  } Overlap_STR;

} PWMCNT45STR;
extern volatile PWMCNT45STR _PWMCNT45 @(REG_BASE + 0x000000B0UL);
#define PWMCNT45                        _PWMCNT45.Word


/*** PWMCNT67 - PWM Channel Counter 67 Register; 0x000000B2 ***/
typedef union {
  word Word;
   /* Overlapped registers: */
  struct {
    /*** PWMCNT6 - PWM Channel Counter 6 Register; 0x000000B2 ***/
    union {
      byte Byte;
    } PWMCNT6STR;
    #define PWMCNT6                     _PWMCNT67.Overlap_STR.PWMCNT6STR.Byte
    

    /*** PWMCNT7 - PWM Channel Counter 7 Register; 0x000000B3 ***/
    union {
      byte Byte;
    } PWMCNT7STR;
    #define PWMCNT7                     _PWMCNT67.Overlap_STR.PWMCNT7STR.Byte
    
  } Overlap_STR;

} PWMCNT67STR;
extern volatile PWMCNT67STR _PWMCNT67 @(REG_BASE + 0x000000B2UL);
#define PWMCNT67                        _PWMCNT67.Word


/*** PWMPER01 - PWM Channel Period 01 Register; 0x000000B4 ***/
typedef union {
  word Word;
   /* Overlapped registers: */
  struct {
    /*** PWMPER0 - PWM Channel Period 0 Register; 0x000000B4 ***/
    union {
      byte Byte;
    } PWMPER0STR;
    #define PWMPER0                     _PWMPER01.Overlap_STR.PWMPER0STR.Byte
    /* PWMPER_ARR: Access 8 PWMPERx registers in an array */
    #define PWMPER_ARR                  ((volatile byte *) &PWMPER0)
    

    /*** PWMPER1 - PWM Channel Period 1 Register; 0x000000B5 ***/
    union {
      byte Byte;
    } PWMPER1STR;
    #define PWMPER1                     _PWMPER01.Overlap_STR.PWMPER1STR.Byte
    
  } Overlap_STR;

} PWMPER01STR;
extern volatile PWMPER01STR _PWMPER01 @(REG_BASE + 0x000000B4UL);
#define PWMPER01                        _PWMPER01.Word


/*** PWMPER23 - PWM Channel Period 23 Register; 0x000000B6 ***/
typedef union {
  word Word;
   /* Overlapped registers: */
  struct {
    /*** PWMPER2 - PWM Channel Period 2 Register; 0x000000B6 ***/
    union {
      byte Byte;
    } PWMPER2STR;
    #define PWMPER2                     _PWMPER23.Overlap_STR.PWMPER2STR.Byte
    

    /*** PWMPER3 - PWM Channel Period 3 Register; 0x000000B7 ***/
    union {
      byte Byte;
    } PWMPER3STR;
    #define PWMPER3                     _PWMPER23.Overlap_STR.PWMPER3STR.Byte
    
  } Overlap_STR;

} PWMPER23STR;
extern volatile PWMPER23STR _PWMPER23 @(REG_BASE + 0x000000B6UL);
#define PWMPER23                        _PWMPER23.Word


/*** PWMPER45 - PWM Channel Period 45 Register; 0x000000B8 ***/
typedef union {
  word Word;
   /* Overlapped registers: */
  struct {
    /*** PWMPER4 - PWM Channel Period 4 Register; 0x000000B8 ***/
    union {
      byte Byte;
    } PWMPER4STR;
    #define PWMPER4                     _PWMPER45.Overlap_STR.PWMPER4STR.Byte
    

    /*** PWMPER5 - PWM Channel Period 5 Register; 0x000000B9 ***/
    union {
      byte Byte;
    } PWMPER5STR;
    #define PWMPER5                     _PWMPER45.Overlap_STR.PWMPER5STR.Byte
    
  } Overlap_STR;

} PWMPER45STR;
extern volatile PWMPER45STR _PWMPER45 @(REG_BASE + 0x000000B8UL);
#define PWMPER45                        _PWMPER45.Word


/*** PWMPER67 - PWM Channel Period 67 Register; 0x000000BA ***/
typedef union {
  word Word;
   /* Overlapped registers: */
  struct {
    /*** PWMPER6 - PWM Channel Period 6 Register; 0x000000BA ***/
    union {
      byte Byte;
    } PWMPER6STR;
    #define PWMPER6                     _PWMPER67.Overlap_STR.PWMPER6STR.Byte
    

    /*** PWMPER7 - PWM Channel Period 7 Register; 0x000000BB ***/
    union {
      byte Byte;
    } PWMPER7STR;
    #define PWMPER7                     _PWMPER67.Overlap_STR.PWMPER7STR.Byte
    
  } Overlap_STR;

} PWMPER67STR;
extern volatile PWMPER67STR _PWMPER67 @(REG_BASE + 0x000000BAUL);
#define PWMPER67                        _PWMPER67.Word


/*** PWMDTY01 - PWM Channel Duty 01 Register; 0x000000BC ***/
typedef union {
  word Word;
   /* Overlapped registers: */
  struct {
    /*** PWMDTY0 - PWM Channel Duty 0 Register; 0x000000BC ***/
    union {
      byte Byte;
    } PWMDTY0STR;
    #define PWMDTY0                     _PWMDTY01.Overlap_STR.PWMDTY0STR.Byte
    /* PWMDTY_ARR: Access 8 PWMDTYx registers in an array */
    #define PWMDTY_ARR                  ((volatile byte *) &PWMDTY0)
    

    /*** PWMDTY1 - PWM Channel Duty 1 Register; 0x000000BD ***/
    union {
      byte Byte;
    } PWMDTY1STR;
    #define PWMDTY1                     _PWMDTY01.Overlap_STR.PWMDTY1STR.Byte
    
  } Overlap_STR;

} PWMDTY01STR;
extern volatile PWMDTY01STR _PWMDTY01 @(REG_BASE + 0x000000BCUL);
#define PWMDTY01                        _PWMDTY01.Word


/*** PWMDTY23 - PWM Channel Duty 23 Register; 0x000000BE ***/
typedef union {
  word Word;
   /* Overlapped registers: */
  struct {
    /*** PWMDTY2 - PWM Channel Duty 2 Register; 0x000000BE ***/
    union {
      byte Byte;
    } PWMDTY2STR;
    #define PWMDTY2                     _PWMDTY23.Overlap_STR.PWMDTY2STR.Byte
    

    /*** PWMDTY3 - PWM Channel Duty 3 Register; 0x000000BF ***/
    union {
      byte Byte;
    } PWMDTY3STR;
    #define PWMDTY3                     _PWMDTY23.Overlap_STR.PWMDTY3STR.Byte
    
  } Overlap_STR;

} PWMDTY23STR;
extern volatile PWMDTY23STR _PWMDTY23 @(REG_BASE + 0x000000BEUL);
#define PWMDTY23                        _PWMDTY23.Word


/*** PWMDTY45 - PWM Channel Duty 45 Register; 0x000000C0 ***/
typedef union {
  word Word;
   /* Overlapped registers: */
  struct {
    /*** PWMDTY4 - PWM Channel Duty 4 Register; 0x000000C0 ***/
    union {
      byte Byte;
    } PWMDTY4STR;
    #define PWMDTY4                     _PWMDTY45.Overlap_STR.PWMDTY4STR.Byte
    

    /*** PWMDTY5 - PWM Channel Duty 5 Register; 0x000000C1 ***/
    union {
      byte Byte;
    } PWMDTY5STR;
    #define PWMDTY5                     _PWMDTY45.Overlap_STR.PWMDTY5STR.Byte
    
  } Overlap_STR;

} PWMDTY45STR;
extern volatile PWMDTY45STR _PWMDTY45 @(REG_BASE + 0x000000C0UL);
#define PWMDTY45                        _PWMDTY45.Word


/*** PWMDTY67 - PWM Channel Duty 67 Register; 0x000000C2 ***/
typedef union {
  word Word;
   /* Overlapped registers: */
  struct {
    /*** PWMDTY6 - PWM Channel Duty 6 Register; 0x000000C2 ***/
    union {
      byte Byte;
    } PWMDTY6STR;
    #define PWMDTY6                     _PWMDTY67.Overlap_STR.PWMDTY6STR.Byte
    

    /*** PWMDTY7 - PWM Channel Duty 7 Register; 0x000000C3 ***/
    union {
      byte Byte;
    } PWMDTY7STR;
    #define PWMDTY7                     _PWMDTY67.Overlap_STR.PWMDTY7STR.Byte
    
  } Overlap_STR;

} PWMDTY67STR;
extern volatile PWMDTY67STR _PWMDTY67 @(REG_BASE + 0x000000C2UL);
#define PWMDTY67                        _PWMDTY67.Word


/*** PWMSDN - PWM Shutdown Register; 0x000000C4 ***/
typedef union {
  byte Byte;
  struct {
    byte PWM7ENA     :1;                                       /* PWM emergency shutdown Enable */
    byte PWM7INL     :1;                                       /* PWM shutdown active input level for ch. 7 */
    byte PWM7IN      :1;                                       /* PWM channel 7 input status */
    byte             :1; 
    byte PWMLVL      :1;                                       /* PWM shutdown output Level */
    byte PWMRSTRT    :1;                                       /* PWM Restart */
    byte PWMIE       :1;                                       /* PWM Interrupt Enable */
    byte PWMIF       :1;                                       /* PWM Interrupt Flag */
  } Bits;
} PWMSDNSTR;
extern volatile PWMSDNSTR _PWMSDN @(REG_BASE + 0x000000C4UL);
#define PWMSDN                          _PWMSDN.Byte
#define PWMSDN_PWM7ENA                  _PWMSDN.Bits.PWM7ENA
#define PWMSDN_PWM7INL                  _PWMSDN.Bits.PWM7INL
#define PWMSDN_PWM7IN                   _PWMSDN.Bits.PWM7IN
#define PWMSDN_PWMLVL                   _PWMSDN.Bits.PWMLVL
#define PWMSDN_PWMRSTRT                 _PWMSDN.Bits.PWMRSTRT
#define PWMSDN_PWMIE                    _PWMSDN.Bits.PWMIE
#define PWMSDN_PWMIF                    _PWMSDN.Bits.PWMIF

#define PWMSDN_PWM7ENA_MASK             1U
#define PWMSDN_PWM7INL_MASK             2U
#define PWMSDN_PWM7IN_MASK              4U
#define PWMSDN_PWMLVL_MASK              16U
#define PWMSDN_PWMRSTRT_MASK            32U
#define PWMSDN_PWMIE_MASK               64U
#define PWMSDN_PWMIF_MASK               128U


/*** SCI0BD - SCI 0 Baud Rate Register; 0x000000C8 ***/
typedef union {
  word Word;
   /* Overlapped registers: */
  struct {
    /*** SCI0BDH - SCI 0 Baud Rate Register High; 0x000000C8 ***/
    union {
      byte Byte;
      struct {
        byte SBR8        :1;                                       /* SCI Baud Rate Bit 8 */
        byte SBR9        :1;                                       /* SCI Baud Rate Bit 9 */
        byte SBR10       :1;                                       /* SCI Baud Rate Bit 10 */
        byte SBR11       :1;                                       /* SCI Baud Rate Bit 11 */
        byte SBR12       :1;                                       /* SCI Baud Rate Bit 12 */
        byte             :1; 
        byte             :1; 
        byte             :1; 
      } Bits;
      struct {
        byte grpSBR_8 :5;
        byte     :1;
        byte     :1;
        byte     :1;
      } MergedBits;
    } SCI0BDHSTR;
    #define SCI0BDH                     _SCI0BD.Overlap_STR.SCI0BDHSTR.Byte
    #define SCI0BDH_SBR8                _SCI0BD.Overlap_STR.SCI0BDHSTR.Bits.SBR8
    #define SCI0BDH_SBR9                _SCI0BD.Overlap_STR.SCI0BDHSTR.Bits.SBR9
    #define SCI0BDH_SBR10               _SCI0BD.Overlap_STR.SCI0BDHSTR.Bits.SBR10
    #define SCI0BDH_SBR11               _SCI0BD.Overlap_STR.SCI0BDHSTR.Bits.SBR11
    #define SCI0BDH_SBR12               _SCI0BD.Overlap_STR.SCI0BDHSTR.Bits.SBR12
    #define SCI0BDH_SBR_8               _SCI0BD.Overlap_STR.SCI0BDHSTR.MergedBits.grpSBR_8
    #define SCI0BDH_SBR                 SCI0BDH_SBR_8
    
    #define SCI0BDH_SBR8_MASK           1U
    #define SCI0BDH_SBR9_MASK           2U
    #define SCI0BDH_SBR10_MASK          4U
    #define SCI0BDH_SBR11_MASK          8U
    #define SCI0BDH_SBR12_MASK          16U
    #define SCI0BDH_SBR_8_MASK          31U
    #define SCI0BDH_SBR_8_BITNUM        0U
    

    /*** SCI0BDL - SCI 0 Baud Rate Register Low; 0x000000C9 ***/
    union {
      byte Byte;
      struct {
        byte SBR0        :1;                                       /* SCI Baud Rate Bit 0 */
        byte SBR1        :1;                                       /* SCI Baud Rate Bit 1 */
        byte SBR2        :1;                                       /* SCI Baud Rate Bit 2 */
        byte SBR3        :1;                                       /* SCI Baud Rate Bit 3 */
        byte SBR4        :1;                                       /* SCI Baud Rate Bit 4 */
        byte SBR5        :1;                                       /* SCI Baud Rate Bit 5 */
        byte SBR6        :1;                                       /* SCI Baud Rate Bit 6 */
        byte SBR7        :1;                                       /* SCI Baud Rate Bit 7 */
      } Bits;
    } SCI0BDLSTR;
    #define SCI0BDL                     _SCI0BD.Overlap_STR.SCI0BDLSTR.Byte
    #define SCI0BDL_SBR0                _SCI0BD.Overlap_STR.SCI0BDLSTR.Bits.SBR0
    #define SCI0BDL_SBR1                _SCI0BD.Overlap_STR.SCI0BDLSTR.Bits.SBR1
    #define SCI0BDL_SBR2                _SCI0BD.Overlap_STR.SCI0BDLSTR.Bits.SBR2
    #define SCI0BDL_SBR3                _SCI0BD.Overlap_STR.SCI0BDLSTR.Bits.SBR3
    #define SCI0BDL_SBR4                _SCI0BD.Overlap_STR.SCI0BDLSTR.Bits.SBR4
    #define SCI0BDL_SBR5                _SCI0BD.Overlap_STR.SCI0BDLSTR.Bits.SBR5
    #define SCI0BDL_SBR6                _SCI0BD.Overlap_STR.SCI0BDLSTR.Bits.SBR6
    #define SCI0BDL_SBR7                _SCI0BD.Overlap_STR.SCI0BDLSTR.Bits.SBR7
    
    #define SCI0BDL_SBR0_MASK           1U
    #define SCI0BDL_SBR1_MASK           2U
    #define SCI0BDL_SBR2_MASK           4U
    #define SCI0BDL_SBR3_MASK           8U
    #define SCI0BDL_SBR4_MASK           16U
    #define SCI0BDL_SBR5_MASK           32U
    #define SCI0BDL_SBR6_MASK           64U
    #define SCI0BDL_SBR7_MASK           128U
    
  } Overlap_STR;

  struct {
    word SBR0        :1;                                       /* SCI Baud Rate Bit 0 */
    word SBR1        :1;                                       /* SCI Baud Rate Bit 1 */
    word SBR2        :1;                                       /* SCI Baud Rate Bit 2 */
    word SBR3        :1;                                       /* SCI Baud Rate Bit 3 */
    word SBR4        :1;                                       /* SCI Baud Rate Bit 4 */
    word SBR5        :1;                                       /* SCI Baud Rate Bit 5 */
    word SBR6        :1;                                       /* SCI Baud Rate Bit 6 */
    word SBR7        :1;                                       /* SCI Baud Rate Bit 7 */
    word SBR8        :1;                                       /* SCI Baud Rate Bit 8 */
    word SBR9        :1;                                       /* SCI Baud Rate Bit 9 */
    word SBR10       :1;                                       /* SCI Baud Rate Bit 10 */
    word SBR11       :1;                                       /* SCI Baud Rate Bit 11 */
    word SBR12       :1;                                       /* SCI Baud Rate Bit 12 */
    word             :1; 
    word             :1; 
    word             :1; 
  } Bits;
  struct {
    word grpSBR  :13;
    word         :1;
    word         :1;
    word         :1;
  } MergedBits;
} SCI0BDSTR;
extern volatile SCI0BDSTR _SCI0BD @(REG_BASE + 0x000000C8UL);
#define SCI0BD                          _SCI0BD.Word
#define SCI0BD_SBR0                     _SCI0BD.Bits.SBR0
#define SCI0BD_SBR1                     _SCI0BD.Bits.SBR1
#define SCI0BD_SBR2                     _SCI0BD.Bits.SBR2
#define SCI0BD_SBR3                     _SCI0BD.Bits.SBR3
#define SCI0BD_SBR4                     _SCI0BD.Bits.SBR4
#define SCI0BD_SBR5                     _SCI0BD.Bits.SBR5
#define SCI0BD_SBR6                     _SCI0BD.Bits.SBR6
#define SCI0BD_SBR7                     _SCI0BD.Bits.SBR7
#define SCI0BD_SBR8                     _SCI0BD.Bits.SBR8
#define SCI0BD_SBR9                     _SCI0BD.Bits.SBR9
#define SCI0BD_SBR10                    _SCI0BD.Bits.SBR10
#define SCI0BD_SBR11                    _SCI0BD.Bits.SBR11
#define SCI0BD_SBR12                    _SCI0BD.Bits.SBR12
#define SCI0BD_SBR                      _SCI0BD.MergedBits.grpSBR

#define SCI0BD_SBR0_MASK                1U
#define SCI0BD_SBR1_MASK                2U
#define SCI0BD_SBR2_MASK                4U
#define SCI0BD_SBR3_MASK                8U
#define SCI0BD_SBR4_MASK                16U
#define SCI0BD_SBR5_MASK                32U
#define SCI0BD_SBR6_MASK                64U
#define SCI0BD_SBR7_MASK                128U
#define SCI0BD_SBR8_MASK                256U
#define SCI0BD_SBR9_MASK                512U
#define SCI0BD_SBR10_MASK               1024U
#define SCI0BD_SBR11_MASK               2048U
#define SCI0BD_SBR12_MASK               4096U
#define SCI0BD_SBR_MASK                 8191U
#define SCI0BD_SBR_BITNUM               0U


/*** SCI0CR1 - SCI 0 Control Register 1; 0x000000CA ***/
typedef union {
  byte Byte;
  struct {
    byte PT          :1;                                       /* Parity Type Bit */
    byte PE          :1;                                       /* Parity Enable Bit */
    byte ILT         :1;                                       /* Idle Line Type Bit */
    byte WAKE        :1;                                       /* Wakeup Condition Bit */
    byte M           :1;                                       /* Data Format Mode Bit */
    byte RSRC        :1;                                       /* Receiver Source Bit */
    byte SCISWAI     :1;                                       /* SCI Stop in Wait Mode Bit */
    byte LOOPS       :1;                                       /* Loop Select Bit */
  } Bits;
} SCI0CR1STR;
extern volatile SCI0CR1STR _SCI0CR1 @(REG_BASE + 0x000000CAUL);
#define SCI0CR1                         _SCI0CR1.Byte
#define SCI0CR1_PT                      _SCI0CR1.Bits.PT
#define SCI0CR1_PE                      _SCI0CR1.Bits.PE
#define SCI0CR1_ILT                     _SCI0CR1.Bits.ILT
#define SCI0CR1_WAKE                    _SCI0CR1.Bits.WAKE
#define SCI0CR1_M                       _SCI0CR1.Bits.M
#define SCI0CR1_RSRC                    _SCI0CR1.Bits.RSRC
#define SCI0CR1_SCISWAI                 _SCI0CR1.Bits.SCISWAI
#define SCI0CR1_LOOPS                   _SCI0CR1.Bits.LOOPS

#define SCI0CR1_PT_MASK                 1U
#define SCI0CR1_PE_MASK                 2U
#define SCI0CR1_ILT_MASK                4U
#define SCI0CR1_WAKE_MASK               8U
#define SCI0CR1_M_MASK                  16U
#define SCI0CR1_RSRC_MASK               32U
#define SCI0CR1_SCISWAI_MASK            64U
#define SCI0CR1_LOOPS_MASK              128U


/*** SCI0CR2 - SCI 0 Control Register 2; 0x000000CB ***/
typedef union {
  byte Byte;
  struct {
    byte SBK         :1;                                       /* Send Break Bit */
    byte RWU         :1;                                       /* Receiver Wakeup Bit */
    byte RE          :1;                                       /* Receiver Enable Bit */
    byte TE          :1;                                       /* Transmitter Enable Bit */
    byte ILIE        :1;                                       /* Idle Line Interrupt Enable Bit */
    byte RIE         :1;                                       /* Receiver Full Interrupt Enable Bit */
    byte TCIE        :1;                                       /* Transmission Complete Interrupt Enable Bit */
    byte SCTIE       :1;                                       /* Transmitter Interrupt Enable Bit */
  } Bits;
} SCI0CR2STR;
extern volatile SCI0CR2STR _SCI0CR2 @(REG_BASE + 0x000000CBUL);
#define SCI0CR2                         _SCI0CR2.Byte
#define SCI0CR2_SBK                     _SCI0CR2.Bits.SBK
#define SCI0CR2_RWU                     _SCI0CR2.Bits.RWU
#define SCI0CR2_RE                      _SCI0CR2.Bits.RE
#define SCI0CR2_TE                      _SCI0CR2.Bits.TE
#define SCI0CR2_ILIE                    _SCI0CR2.Bits.ILIE
#define SCI0CR2_RIE                     _SCI0CR2.Bits.RIE
#define SCI0CR2_TCIE                    _SCI0CR2.Bits.TCIE
#define SCI0CR2_SCTIE                   _SCI0CR2.Bits.SCTIE

#define SCI0CR2_SBK_MASK                1U
#define SCI0CR2_RWU_MASK                2U
#define SCI0CR2_RE_MASK                 4U
#define SCI0CR2_TE_MASK                 8U
#define SCI0CR2_ILIE_MASK               16U
#define SCI0CR2_RIE_MASK                32U
#define SCI0CR2_TCIE_MASK               64U
#define SCI0CR2_SCTIE_MASK              128U


/*** SCI0SR1 - SCI 0 Status Register 1; 0x000000CC ***/
typedef union {
  byte Byte;
  struct {
    byte PF          :1;                                       /* Parity Error Flag */
    byte FE          :1;                                       /* Framing Error Flag */
    byte NF          :1;                                       /* Noise Flag */
    byte OR          :1;                                       /* Overrun Flag */
    byte IDLE        :1;                                       /* Idle Line Flag */
    byte RDRF        :1;                                       /* Receive Data Register Full Flag */
    byte TC          :1;                                       /* Transmit Complete Flag */
    byte TDRE        :1;                                       /* Transmit Data Register Empty Flag */
  } Bits;
} SCI0SR1STR;
extern volatile SCI0SR1STR _SCI0SR1 @(REG_BASE + 0x000000CCUL);
#define SCI0SR1                         _SCI0SR1.Byte
#define SCI0SR1_PF                      _SCI0SR1.Bits.PF
#define SCI0SR1_FE                      _SCI0SR1.Bits.FE
#define SCI0SR1_NF                      _SCI0SR1.Bits.NF
#define SCI0SR1_OR                      _SCI0SR1.Bits.OR
#define SCI0SR1_IDLE                    _SCI0SR1.Bits.IDLE
#define SCI0SR1_RDRF                    _SCI0SR1.Bits.RDRF
#define SCI0SR1_TC                      _SCI0SR1.Bits.TC
#define SCI0SR1_TDRE                    _SCI0SR1.Bits.TDRE

#define SCI0SR1_PF_MASK                 1U
#define SCI0SR1_FE_MASK                 2U
#define SCI0SR1_NF_MASK                 4U
#define SCI0SR1_OR_MASK                 8U
#define SCI0SR1_IDLE_MASK               16U
#define SCI0SR1_RDRF_MASK               32U
#define SCI0SR1_TC_MASK                 64U
#define SCI0SR1_TDRE_MASK               128U


/*** SCI0SR2 - SCI 0 Status Register 2; 0x000000CD ***/
typedef union {
  byte Byte;
  struct {
    byte RAF         :1;                                       /* Receiver Active Flag */
    byte TXDIR       :1;                                       /* Transmitter pin data direction in Single-Wire mode */
    byte BRK13       :1;                                       /* Break Transmit character length */
    byte             :1; 
    byte             :1; 
    byte             :1; 
    byte             :1; 
    byte             :1; 
  } Bits;
} SCI0SR2STR;
extern volatile SCI0SR2STR _SCI0SR2 @(REG_BASE + 0x000000CDUL);
#define SCI0SR2                         _SCI0SR2.Byte
#define SCI0SR2_RAF                     _SCI0SR2.Bits.RAF
#define SCI0SR2_TXDIR                   _SCI0SR2.Bits.TXDIR
#define SCI0SR2_BRK13                   _SCI0SR2.Bits.BRK13

#define SCI0SR2_RAF_MASK                1U
#define SCI0SR2_TXDIR_MASK              2U
#define SCI0SR2_BRK13_MASK              4U


/*** SCI0DRH - SCI 0 Data Register High; 0x000000CE ***/
typedef union {
  byte Byte;
  struct {
    byte             :1; 
    byte             :1; 
    byte             :1; 
    byte             :1; 
    byte             :1; 
    byte             :1; 
    byte T8          :1;                                       /* Transmit Bit 8 */
    byte R8          :1;                                       /* Received Bit 8 */
  } Bits;
} SCI0DRHSTR;
extern volatile SCI0DRHSTR _SCI0DRH @(REG_BASE + 0x000000CEUL);
#define SCI0DRH                         _SCI0DRH.Byte
#define SCI0DRH_T8                      _SCI0DRH.Bits.T8
#define SCI0DRH_R8                      _SCI0DRH.Bits.R8

#define SCI0DRH_T8_MASK                 64U
#define SCI0DRH_R8_MASK                 128U


/*** SCI0DRL - SCI 0 Data Register Low; 0x000000CF ***/
typedef union {
  byte Byte;
  struct {
    byte R0_T0       :1;                                       /* Received bit 0 or Transmit bit 0 */
    byte R1_T1       :1;                                       /* Received bit 1 or Transmit bit 1 */
    byte R2_T2       :1;                                       /* Received bit 2 or Transmit bit 2 */
    byte R3_T3       :1;                                       /* Received bit 3 or Transmit bit 3 */
    byte R4_T4       :1;                                       /* Received bit 4 or Transmit bit 4 */
    byte R5_T5       :1;                                       /* Received bit 5 or Transmit bit 5 */
    byte R6_T6       :1;                                       /* Received bit 6 or Transmit bit 6 */
    byte R7_T7       :1;                                       /* Received bit 7 or Transmit bit 7 */
  } Bits;
} SCI0DRLSTR;
extern volatile SCI0DRLSTR _SCI0DRL @(REG_BASE + 0x000000CFUL);
#define SCI0DRL                         _SCI0DRL.Byte
#define SCI0DRL_R0_T0                   _SCI0DRL.Bits.R0_T0
#define SCI0DRL_R1_T1                   _SCI0DRL.Bits.R1_T1
#define SCI0DRL_R2_T2                   _SCI0DRL.Bits.R2_T2
#define SCI0DRL_R3_T3                   _SCI0DRL.Bits.R3_T3
#define SCI0DRL_R4_T4                   _SCI0DRL.Bits.R4_T4
#define SCI0DRL_R5_T5                   _SCI0DRL.Bits.R5_T5
#define SCI0DRL_R6_T6                   _SCI0DRL.Bits.R6_T6
#define SCI0DRL_R7_T7                   _SCI0DRL.Bits.R7_T7

#define SCI0DRL_R0_T0_MASK              1U
#define SCI0DRL_R1_T1_MASK              2U
#define SCI0DRL_R2_T2_MASK              4U
#define SCI0DRL_R3_T3_MASK              8U
#define SCI0DRL_R4_T4_MASK              16U
#define SCI0DRL_R5_T5_MASK              32U
#define SCI0DRL_R6_T6_MASK              64U
#define SCI0DRL_R7_T7_MASK              128U


/*** SCI1BD - SCI 1 Baud Rate Register; 0x000000D0 ***/
typedef union {
  word Word;
   /* Overlapped registers: */
  struct {
    /*** SCI1BDH - SCI 1 Baud Rate Register High; 0x000000D0 ***/
    union {
      byte Byte;
      struct {
        byte SBR8        :1;                                       /* SCI Baud Rate Bit 8 */
        byte SBR9        :1;                                       /* SCI Baud Rate Bit 9 */
        byte SBR10       :1;                                       /* SCI Baud Rate Bit 10 */
        byte SBR11       :1;                                       /* SCI Baud Rate Bit 11 */
        byte SBR12       :1;                                       /* SCI Baud Rate Bit 12 */
        byte             :1; 
        byte             :1; 
        byte             :1; 
      } Bits;
      struct {
        byte grpSBR_8 :5;
        byte     :1;
        byte     :1;
        byte     :1;
      } MergedBits;
    } SCI1BDHSTR;
    #define SCI1BDH                     _SCI1BD.Overlap_STR.SCI1BDHSTR.Byte
    #define SCI1BDH_SBR8                _SCI1BD.Overlap_STR.SCI1BDHSTR.Bits.SBR8
    #define SCI1BDH_SBR9                _SCI1BD.Overlap_STR.SCI1BDHSTR.Bits.SBR9
    #define SCI1BDH_SBR10               _SCI1BD.Overlap_STR.SCI1BDHSTR.Bits.SBR10
    #define SCI1BDH_SBR11               _SCI1BD.Overlap_STR.SCI1BDHSTR.Bits.SBR11
    #define SCI1BDH_SBR12               _SCI1BD.Overlap_STR.SCI1BDHSTR.Bits.SBR12
    #define SCI1BDH_SBR_8               _SCI1BD.Overlap_STR.SCI1BDHSTR.MergedBits.grpSBR_8
    #define SCI1BDH_SBR                 SCI1BDH_SBR_8
    
    #define SCI1BDH_SBR8_MASK           1U
    #define SCI1BDH_SBR9_MASK           2U
    #define SCI1BDH_SBR10_MASK          4U
    #define SCI1BDH_SBR11_MASK          8U
    #define SCI1BDH_SBR12_MASK          16U
    #define SCI1BDH_SBR_8_MASK          31U
    #define SCI1BDH_SBR_8_BITNUM        0U
    

    /*** SCI1BDL - SCI 1 Baud Rate Register Low; 0x000000D1 ***/
    union {
      byte Byte;
      struct {
        byte SBR0        :1;                                       /* SCI Baud Rate Bit 0 */
        byte SBR1        :1;                                       /* SCI Baud Rate Bit 1 */
        byte SBR2        :1;                                       /* SCI Baud Rate Bit 2 */
        byte SBR3        :1;                                       /* SCI Baud Rate Bit 3 */
        byte SBR4        :1;                                       /* SCI Baud Rate Bit 4 */
        byte SBR5        :1;                                       /* SCI Baud Rate Bit 5 */
        byte SBR6        :1;                                       /* SCI Baud Rate Bit 6 */
        byte SBR7        :1;                                       /* SCI Baud Rate Bit 7 */
      } Bits;
    } SCI1BDLSTR;
    #define SCI1BDL                     _SCI1BD.Overlap_STR.SCI1BDLSTR.Byte
    #define SCI1BDL_SBR0                _SCI1BD.Overlap_STR.SCI1BDLSTR.Bits.SBR0
    #define SCI1BDL_SBR1                _SCI1BD.Overlap_STR.SCI1BDLSTR.Bits.SBR1
    #define SCI1BDL_SBR2                _SCI1BD.Overlap_STR.SCI1BDLSTR.Bits.SBR2
    #define SCI1BDL_SBR3                _SCI1BD.Overlap_STR.SCI1BDLSTR.Bits.SBR3
    #define SCI1BDL_SBR4                _SCI1BD.Overlap_STR.SCI1BDLSTR.Bits.SBR4
    #define SCI1BDL_SBR5                _SCI1BD.Overlap_STR.SCI1BDLSTR.Bits.SBR5
    #define SCI1BDL_SBR6                _SCI1BD.Overlap_STR.SCI1BDLSTR.Bits.SBR6
    #define SCI1BDL_SBR7                _SCI1BD.Overlap_STR.SCI1BDLSTR.Bits.SBR7
    
    #define SCI1BDL_SBR0_MASK           1U
    #define SCI1BDL_SBR1_MASK           2U
    #define SCI1BDL_SBR2_MASK           4U
    #define SCI1BDL_SBR3_MASK           8U
    #define SCI1BDL_SBR4_MASK           16U
    #define SCI1BDL_SBR5_MASK           32U
    #define SCI1BDL_SBR6_MASK           64U
    #define SCI1BDL_SBR7_MASK           128U
    
  } Overlap_STR;

  struct {
    word SBR0        :1;                                       /* SCI Baud Rate Bit 0 */
    word SBR1        :1;                                       /* SCI Baud Rate Bit 1 */
    word SBR2        :1;                                       /* SCI Baud Rate Bit 2 */
    word SBR3        :1;                                       /* SCI Baud Rate Bit 3 */
    word SBR4        :1;                                       /* SCI Baud Rate Bit 4 */
    word SBR5        :1;                                       /* SCI Baud Rate Bit 5 */
    word SBR6        :1;                                       /* SCI Baud Rate Bit 6 */
    word SBR7        :1;                                       /* SCI Baud Rate Bit 7 */
    word SBR8        :1;                                       /* SCI Baud Rate Bit 8 */
    word SBR9        :1;                                       /* SCI Baud Rate Bit 9 */
    word SBR10       :1;                                       /* SCI Baud Rate Bit 10 */
    word SBR11       :1;                                       /* SCI Baud Rate Bit 11 */
    word SBR12       :1;                                       /* SCI Baud Rate Bit 12 */
    word             :1; 
    word             :1; 
    word             :1; 
  } Bits;
  struct {
    word grpSBR  :13;
    word         :1;
    word         :1;
    word         :1;
  } MergedBits;
} SCI1BDSTR;
extern volatile SCI1BDSTR _SCI1BD @(REG_BASE + 0x000000D0UL);
#define SCI1BD                          _SCI1BD.Word
#define SCI1BD_SBR0                     _SCI1BD.Bits.SBR0
#define SCI1BD_SBR1                     _SCI1BD.Bits.SBR1
#define SCI1BD_SBR2                     _SCI1BD.Bits.SBR2
#define SCI1BD_SBR3                     _SCI1BD.Bits.SBR3
#define SCI1BD_SBR4                     _SCI1BD.Bits.SBR4
#define SCI1BD_SBR5                     _SCI1BD.Bits.SBR5
#define SCI1BD_SBR6                     _SCI1BD.Bits.SBR6
#define SCI1BD_SBR7                     _SCI1BD.Bits.SBR7
#define SCI1BD_SBR8                     _SCI1BD.Bits.SBR8
#define SCI1BD_SBR9                     _SCI1BD.Bits.SBR9
#define SCI1BD_SBR10                    _SCI1BD.Bits.SBR10
#define SCI1BD_SBR11                    _SCI1BD.Bits.SBR11
#define SCI1BD_SBR12                    _SCI1BD.Bits.SBR12
#define SCI1BD_SBR                      _SCI1BD.MergedBits.grpSBR

#define SCI1BD_SBR0_MASK                1U
#define SCI1BD_SBR1_MASK                2U
#define SCI1BD_SBR2_MASK                4U
#define SCI1BD_SBR3_MASK                8U
#define SCI1BD_SBR4_MASK                16U
#define SCI1BD_SBR5_MASK                32U
#define SCI1BD_SBR6_MASK                64U
#define SCI1BD_SBR7_MASK                128U
#define SCI1BD_SBR8_MASK                256U
#define SCI1BD_SBR9_MASK                512U
#define SCI1BD_SBR10_MASK               1024U
#define SCI1BD_SBR11_MASK               2048U
#define SCI1BD_SBR12_MASK               4096U
#define SCI1BD_SBR_MASK                 8191U
#define SCI1BD_SBR_BITNUM               0U


/*** SCI1CR1 - SCI 1 Control Register 1; 0x000000D2 ***/
typedef union {
  byte Byte;
  struct {
    byte PT          :1;                                       /* Parity Type Bit */
    byte PE          :1;                                       /* Parity Enable Bit */
    byte ILT         :1;                                       /* Idle Line Type Bit */
    byte WAKE        :1;                                       /* Wakeup Condition Bit */
    byte M           :1;                                       /* Data Format Mode Bit */
    byte RSRC        :1;                                       /* Receiver Source Bit */
    byte SCISWAI     :1;                                       /* SCI Stop in Wait Mode Bit */
    byte LOOPS       :1;                                       /* Loop Select Bit */
  } Bits;
} SCI1CR1STR;
extern volatile SCI1CR1STR _SCI1CR1 @(REG_BASE + 0x000000D2UL);
#define SCI1CR1                         _SCI1CR1.Byte
#define SCI1CR1_PT                      _SCI1CR1.Bits.PT
#define SCI1CR1_PE                      _SCI1CR1.Bits.PE
#define SCI1CR1_ILT                     _SCI1CR1.Bits.ILT
#define SCI1CR1_WAKE                    _SCI1CR1.Bits.WAKE
#define SCI1CR1_M                       _SCI1CR1.Bits.M
#define SCI1CR1_RSRC                    _SCI1CR1.Bits.RSRC
#define SCI1CR1_SCISWAI                 _SCI1CR1.Bits.SCISWAI
#define SCI1CR1_LOOPS                   _SCI1CR1.Bits.LOOPS

#define SCI1CR1_PT_MASK                 1U
#define SCI1CR1_PE_MASK                 2U
#define SCI1CR1_ILT_MASK                4U
#define SCI1CR1_WAKE_MASK               8U
#define SCI1CR1_M_MASK                  16U
#define SCI1CR1_RSRC_MASK               32U
#define SCI1CR1_SCISWAI_MASK            64U
#define SCI1CR1_LOOPS_MASK              128U


/*** SCI1CR2 - SCI 1 Control Register 2; 0x000000D3 ***/
typedef union {
  byte Byte;
  struct {
    byte SBK         :1;                                       /* Send Break Bit */
    byte RWU         :1;                                       /* Receiver Wakeup Bit */
    byte RE          :1;                                       /* Receiver Enable Bit */
    byte TE          :1;                                       /* Transmitter Enable Bit */
    byte ILIE        :1;                                       /* Idle Line Interrupt Enable Bit */
    byte RIE         :1;                                       /* Receiver Full Interrupt Enable Bit */
    byte TCIE        :1;                                       /* Transmission Complete Interrupt Enable Bit */
    byte SCTIE       :1;                                       /* Transmitter Interrupt Enable Bit */
  } Bits;
} SCI1CR2STR;
extern volatile SCI1CR2STR _SCI1CR2 @(REG_BASE + 0x000000D3UL);
#define SCI1CR2                         _SCI1CR2.Byte
#define SCI1CR2_SBK                     _SCI1CR2.Bits.SBK
#define SCI1CR2_RWU                     _SCI1CR2.Bits.RWU
#define SCI1CR2_RE                      _SCI1CR2.Bits.RE
#define SCI1CR2_TE                      _SCI1CR2.Bits.TE
#define SCI1CR2_ILIE                    _SCI1CR2.Bits.ILIE
#define SCI1CR2_RIE                     _SCI1CR2.Bits.RIE
#define SCI1CR2_TCIE                    _SCI1CR2.Bits.TCIE
#define SCI1CR2_SCTIE                   _SCI1CR2.Bits.SCTIE

#define SCI1CR2_SBK_MASK                1U
#define SCI1CR2_RWU_MASK                2U
#define SCI1CR2_RE_MASK                 4U
#define SCI1CR2_TE_MASK                 8U
#define SCI1CR2_ILIE_MASK               16U
#define SCI1CR2_RIE_MASK                32U
#define SCI1CR2_TCIE_MASK               64U
#define SCI1CR2_SCTIE_MASK              128U


/*** SCI1SR1 - SCI 1 Status Register 1; 0x000000D4 ***/
typedef union {
  byte Byte;
  struct {
    byte PF          :1;                                       /* Parity Error Flag */
    byte FE          :1;                                       /* Framing Error Flag */
    byte NF          :1;                                       /* Noise Flag */
    byte OR          :1;                                       /* Overrun Flag */
    byte IDLE        :1;                                       /* Idle Line Flag */
    byte RDRF        :1;                                       /* Receive Data Register Full Flag */
    byte TC          :1;                                       /* Transmit Complete Flag */
    byte TDRE        :1;                                       /* Transmit Data Register Empty Flag */
  } Bits;
} SCI1SR1STR;
extern volatile SCI1SR1STR _SCI1SR1 @(REG_BASE + 0x000000D4UL);
#define SCI1SR1                         _SCI1SR1.Byte
#define SCI1SR1_PF                      _SCI1SR1.Bits.PF
#define SCI1SR1_FE                      _SCI1SR1.Bits.FE
#define SCI1SR1_NF                      _SCI1SR1.Bits.NF
#define SCI1SR1_OR                      _SCI1SR1.Bits.OR
#define SCI1SR1_IDLE                    _SCI1SR1.Bits.IDLE
#define SCI1SR1_RDRF                    _SCI1SR1.Bits.RDRF
#define SCI1SR1_TC                      _SCI1SR1.Bits.TC
#define SCI1SR1_TDRE                    _SCI1SR1.Bits.TDRE

#define SCI1SR1_PF_MASK                 1U
#define SCI1SR1_FE_MASK                 2U
#define SCI1SR1_NF_MASK                 4U
#define SCI1SR1_OR_MASK                 8U
#define SCI1SR1_IDLE_MASK               16U
#define SCI1SR1_RDRF_MASK               32U
#define SCI1SR1_TC_MASK                 64U
#define SCI1SR1_TDRE_MASK               128U


/*** SCI1SR2 - SCI 1 Status Register 2; 0x000000D5 ***/
typedef union {
  byte Byte;
  struct {
    byte RAF         :1;                                       /* Receiver Active Flag */
    byte TXDIR       :1;                                       /* Transmitter pin data direction in Single-Wire mode */
    byte BRK13       :1;                                       /* Break Transmit character length */
    byte             :1; 
    byte             :1; 
    byte             :1; 
    byte             :1; 
    byte             :1; 
  } Bits;
} SCI1SR2STR;
extern volatile SCI1SR2STR _SCI1SR2 @(REG_BASE + 0x000000D5UL);
#define SCI1SR2                         _SCI1SR2.Byte
#define SCI1SR2_RAF                     _SCI1SR2.Bits.RAF
#define SCI1SR2_TXDIR                   _SCI1SR2.Bits.TXDIR
#define SCI1SR2_BRK13                   _SCI1SR2.Bits.BRK13

#define SCI1SR2_RAF_MASK                1U
#define SCI1SR2_TXDIR_MASK              2U
#define SCI1SR2_BRK13_MASK              4U


/*** SCI1DRH - SCI 1 Data Register High; 0x000000D6 ***/
typedef union {
  byte Byte;
  struct {
    byte             :1; 
    byte             :1; 
    byte             :1; 
    byte             :1; 
    byte             :1; 
    byte             :1; 
    byte T8          :1;                                       /* Transmit Bit 8 */
    byte R8          :1;                                       /* Received Bit 8 */
  } Bits;
} SCI1DRHSTR;
extern volatile SCI1DRHSTR _SCI1DRH @(REG_BASE + 0x000000D6UL);
#define SCI1DRH                         _SCI1DRH.Byte
#define SCI1DRH_T8                      _SCI1DRH.Bits.T8
#define SCI1DRH_R8                      _SCI1DRH.Bits.R8

#define SCI1DRH_T8_MASK                 64U
#define SCI1DRH_R8_MASK                 128U


/*** SCI1DRL - SCI 1 Data Register Low; 0x000000D7 ***/
typedef union {
  byte Byte;
  struct {
    byte R0_T0       :1;                                       /* Received bit 0 or Transmit bit 0 */
    byte R1_T1       :1;                                       /* Received bit 1 or Transmit bit 1 */
    byte R2_T2       :1;                                       /* Received bit 2 or Transmit bit 2 */
    byte R3_T3       :1;                                       /* Received bit 3 or Transmit bit 3 */
    byte R4_T4       :1;                                       /* Received bit 4 or Transmit bit 4 */
    byte R5_T5       :1;                                       /* Received bit 5 or Transmit bit 5 */
    byte R6_T6       :1;                                       /* Received bit 6 or Transmit bit 6 */
    byte R7_T7       :1;                                       /* Received bit 7 or Transmit bit 7 */
  } Bits;
} SCI1DRLSTR;
extern volatile SCI1DRLSTR _SCI1DRL @(REG_BASE + 0x000000D7UL);
#define SCI1DRL                         _SCI1DRL.Byte
#define SCI1DRL_R0_T0                   _SCI1DRL.Bits.R0_T0
#define SCI1DRL_R1_T1                   _SCI1DRL.Bits.R1_T1
#define SCI1DRL_R2_T2                   _SCI1DRL.Bits.R2_T2
#define SCI1DRL_R3_T3                   _SCI1DRL.Bits.R3_T3
#define SCI1DRL_R4_T4                   _SCI1DRL.Bits.R4_T4
#define SCI1DRL_R5_T5                   _SCI1DRL.Bits.R5_T5
#define SCI1DRL_R6_T6                   _SCI1DRL.Bits.R6_T6
#define SCI1DRL_R7_T7                   _SCI1DRL.Bits.R7_T7

#define SCI1DRL_R0_T0_MASK              1U
#define SCI1DRL_R1_T1_MASK              2U
#define SCI1DRL_R2_T2_MASK              4U
#define SCI1DRL_R3_T3_MASK              8U
#define SCI1DRL_R4_T4_MASK              16U
#define SCI1DRL_R5_T5_MASK              32U
#define SCI1DRL_R6_T6_MASK              64U
#define SCI1DRL_R7_T7_MASK              128U


/*** SPI0CR1 - SPI 0 Control Register; 0x000000D8 ***/
typedef union {
  byte Byte;
  struct {
    byte LSBFE       :1;                                       /* SPI LSB-First Enable */
    byte SSOE        :1;                                       /* Slave Select Output Enable */
    byte CPHA        :1;                                       /* SPI Clock Phase Bit */
    byte CPOL        :1;                                       /* SPI Clock Polarity Bit */
    byte MSTR        :1;                                       /* SPI Master/Slave Mode Select Bit */
    byte SPTIE       :1;                                       /* SPI Transmit Interrupt Enable */
    byte SPE         :1;                                       /* SPI System Enable Bit */
    byte SPIE        :1;                                       /* SPI Interrupt Enable Bit */
  } Bits;
} SPI0CR1STR;
extern volatile SPI0CR1STR _SPI0CR1 @(REG_BASE + 0x000000D8UL);
#define SPI0CR1                         _SPI0CR1.Byte
#define SPI0CR1_LSBFE                   _SPI0CR1.Bits.LSBFE
#define SPI0CR1_SSOE                    _SPI0CR1.Bits.SSOE
#define SPI0CR1_CPHA                    _SPI0CR1.Bits.CPHA
#define SPI0CR1_CPOL                    _SPI0CR1.Bits.CPOL
#define SPI0CR1_MSTR                    _SPI0CR1.Bits.MSTR
#define SPI0CR1_SPTIE                   _SPI0CR1.Bits.SPTIE
#define SPI0CR1_SPE                     _SPI0CR1.Bits.SPE
#define SPI0CR1_SPIE                    _SPI0CR1.Bits.SPIE

#define SPI0CR1_LSBFE_MASK              1U
#define SPI0CR1_SSOE_MASK               2U
#define SPI0CR1_CPHA_MASK               4U
#define SPI0CR1_CPOL_MASK               8U
#define SPI0CR1_MSTR_MASK               16U
#define SPI0CR1_SPTIE_MASK              32U
#define SPI0CR1_SPE_MASK                64U
#define SPI0CR1_SPIE_MASK               128U


/*** SPI0CR2 - SPI 0 Control Register 2; 0x000000D9 ***/
typedef union {
  byte Byte;
  struct {
    byte SPC0        :1;                                       /* Serial Pin Control Bit 0 */
    byte SPISWAI     :1;                                       /* SPI Stop in Wait Mode Bit */
    byte             :1; 
    byte BIDIROE     :1;                                       /* Output enable in the Bidirectional mode of operation */
    byte MODFEN      :1;                                       /* Mode Fault Enable Bit */
    byte             :1; 
    byte             :1; 
    byte             :1; 
  } Bits;
} SPI0CR2STR;
extern volatile SPI0CR2STR _SPI0CR2 @(REG_BASE + 0x000000D9UL);
#define SPI0CR2                         _SPI0CR2.Byte
#define SPI0CR2_SPC0                    _SPI0CR2.Bits.SPC0
#define SPI0CR2_SPISWAI                 _SPI0CR2.Bits.SPISWAI
#define SPI0CR2_BIDIROE                 _SPI0CR2.Bits.BIDIROE
#define SPI0CR2_MODFEN                  _SPI0CR2.Bits.MODFEN

#define SPI0CR2_SPC0_MASK               1U
#define SPI0CR2_SPISWAI_MASK            2U
#define SPI0CR2_BIDIROE_MASK            8U
#define SPI0CR2_MODFEN_MASK             16U


/*** SPI0BR - SPI 0 Baud Rate Register; 0x000000DA ***/
typedef union {
  byte Byte;
  struct {
    byte SPR0        :1;                                       /* SPI Baud Rate Selection Bit 0 */
    byte SPR1        :1;                                       /* SPI Baud Rate Selection Bit 1 */
    byte SPR2        :1;                                       /* SPI Baud Rate Selection Bit 2 */
    byte             :1; 
    byte SPPR0       :1;                                       /* SPI Baud Rate Preselection Bits 0 */
    byte SPPR1       :1;                                       /* SPI Baud Rate Preselection Bits 1 */
    byte SPPR2       :1;                                       /* SPI Baud Rate Preselection Bits 2 */
    byte             :1; 
  } Bits;
  struct {
    byte grpSPR  :3;
    byte         :1;
    byte grpSPPR :3;
    byte         :1;
  } MergedBits;
} SPI0BRSTR;
extern volatile SPI0BRSTR _SPI0BR @(REG_BASE + 0x000000DAUL);
#define SPI0BR                          _SPI0BR.Byte
#define SPI0BR_SPR0                     _SPI0BR.Bits.SPR0
#define SPI0BR_SPR1                     _SPI0BR.Bits.SPR1
#define SPI0BR_SPR2                     _SPI0BR.Bits.SPR2
#define SPI0BR_SPPR0                    _SPI0BR.Bits.SPPR0
#define SPI0BR_SPPR1                    _SPI0BR.Bits.SPPR1
#define SPI0BR_SPPR2                    _SPI0BR.Bits.SPPR2
#define SPI0BR_SPR                      _SPI0BR.MergedBits.grpSPR
#define SPI0BR_SPPR                     _SPI0BR.MergedBits.grpSPPR

#define SPI0BR_SPR0_MASK                1U
#define SPI0BR_SPR1_MASK                2U
#define SPI0BR_SPR2_MASK                4U
#define SPI0BR_SPPR0_MASK               16U
#define SPI0BR_SPPR1_MASK               32U
#define SPI0BR_SPPR2_MASK               64U
#define SPI0BR_SPR_MASK                 7U
#define SPI0BR_SPR_BITNUM               0U
#define SPI0BR_SPPR_MASK                112U
#define SPI0BR_SPPR_BITNUM              4U


/*** SPI0SR - SPI 0 Status Register; 0x000000DB ***/
typedef union {
  byte Byte;
  struct {
    byte             :1; 
    byte             :1; 
    byte             :1; 
    byte             :1; 
    byte MODF        :1;                                       /* Mode Fault Flag */
    byte SPTEF       :1;                                       /* SPI Transmit Empty Interrupt Flag */
    byte             :1; 
    byte SPIF        :1;                                       /* SPIF Receive Interrupt Flag */
  } Bits;
} SPI0SRSTR;
extern volatile SPI0SRSTR _SPI0SR @(REG_BASE + 0x000000DBUL);
#define SPI0SR                          _SPI0SR.Byte
#define SPI0SR_MODF                     _SPI0SR.Bits.MODF
#define SPI0SR_SPTEF                    _SPI0SR.Bits.SPTEF
#define SPI0SR_SPIF                     _SPI0SR.Bits.SPIF

#define SPI0SR_MODF_MASK                16U
#define SPI0SR_SPTEF_MASK               32U
#define SPI0SR_SPIF_MASK                128U


/*** SPI0DR - SPI 0 Data Register; 0x000000DD ***/
typedef union {
  byte Byte;
} SPI0DRSTR;
extern volatile SPI0DRSTR _SPI0DR @(REG_BASE + 0x000000DDUL);
#define SPI0DR                          _SPI0DR.Byte


/*** IBAD - IIC Address Register; 0x000000E0 ***/
typedef union {
  byte Byte;
  struct {
    byte             :1; 
    byte ADR1        :1;                                       /* Slave Address Bit 1 */
    byte ADR2        :1;                                       /* Slave Address Bit 2 */
    byte ADR3        :1;                                       /* Slave Address Bit 3 */
    byte ADR4        :1;                                       /* Slave Address Bit 4 */
    byte ADR5        :1;                                       /* Slave Address Bit 5 */
    byte ADR6        :1;                                       /* Slave Address Bit 6 */
    byte ADR7        :1;                                       /* Slave Address Bit 7 */
  } Bits;
  struct {
    byte         :1;
    byte grpADR_1 :7;
  } MergedBits;
} IBADSTR;
extern volatile IBADSTR _IBAD @(REG_BASE + 0x000000E0UL);
#define IBAD                            _IBAD.Byte
#define IBAD_ADR1                       _IBAD.Bits.ADR1
#define IBAD_ADR2                       _IBAD.Bits.ADR2
#define IBAD_ADR3                       _IBAD.Bits.ADR3
#define IBAD_ADR4                       _IBAD.Bits.ADR4
#define IBAD_ADR5                       _IBAD.Bits.ADR5
#define IBAD_ADR6                       _IBAD.Bits.ADR6
#define IBAD_ADR7                       _IBAD.Bits.ADR7
#define IBAD_ADR_1                      _IBAD.MergedBits.grpADR_1
#define IBAD_ADR                        IBAD_ADR_1

#define IBAD_ADR1_MASK                  2U
#define IBAD_ADR2_MASK                  4U
#define IBAD_ADR3_MASK                  8U
#define IBAD_ADR4_MASK                  16U
#define IBAD_ADR5_MASK                  32U
#define IBAD_ADR6_MASK                  64U
#define IBAD_ADR7_MASK                  128U
#define IBAD_ADR_1_MASK                 254U
#define IBAD_ADR_1_BITNUM               1U


/*** IBFD - IIC Frequency Divider Register; 0x000000E1 ***/
typedef union {
  byte Byte;
  struct {
    byte IBC0        :1;                                       /* I-Bus Clock Rate 0 */
    byte IBC1        :1;                                       /* I-Bus Clock Rate 1 */
    byte IBC2        :1;                                       /* I-Bus Clock Rate 2 */
    byte IBC3        :1;                                       /* I-Bus Clock Rate 3 */
    byte IBC4        :1;                                       /* I-Bus Clock Rate 4 */
    byte IBC5        :1;                                       /* I-Bus Clock Rate 5 */
    byte IBC6        :1;                                       /* I-Bus Clock Rate 6 */
    byte IBC7        :1;                                       /* I-Bus Clock Rate 7 */
  } Bits;
} IBFDSTR;
extern volatile IBFDSTR _IBFD @(REG_BASE + 0x000000E1UL);
#define IBFD                            _IBFD.Byte
#define IBFD_IBC0                       _IBFD.Bits.IBC0
#define IBFD_IBC1                       _IBFD.Bits.IBC1
#define IBFD_IBC2                       _IBFD.Bits.IBC2
#define IBFD_IBC3                       _IBFD.Bits.IBC3
#define IBFD_IBC4                       _IBFD.Bits.IBC4
#define IBFD_IBC5                       _IBFD.Bits.IBC5
#define IBFD_IBC6                       _IBFD.Bits.IBC6
#define IBFD_IBC7                       _IBFD.Bits.IBC7

#define IBFD_IBC0_MASK                  1U
#define IBFD_IBC1_MASK                  2U
#define IBFD_IBC2_MASK                  4U
#define IBFD_IBC3_MASK                  8U
#define IBFD_IBC4_MASK                  16U
#define IBFD_IBC5_MASK                  32U
#define IBFD_IBC6_MASK                  64U
#define IBFD_IBC7_MASK                  128U


/*** IBCR - IIC Control Register; 0x000000E2 ***/
typedef union {
  byte Byte;
  struct {
    byte IBSWAI      :1;                                       /* I-Bus Interface Stop in WAIT mode */
    byte             :1; 
    byte RSTA        :1;                                       /* Repeat Start */
    byte TXAK        :1;                                       /* Transmit Acknowledge enable */
    byte TX_RX       :1;                                       /* Transmit/Receive mode select bit */
    byte MS_SL       :1;                                       /* Master/Slave mode select bit */
    byte IBIE        :1;                                       /* I-Bus Interrupt Enable */
    byte IBEN        :1;                                       /* I-Bus Enable */
  } Bits;
} IBCRSTR;
extern volatile IBCRSTR _IBCR @(REG_BASE + 0x000000E2UL);
#define IBCR                            _IBCR.Byte
#define IBCR_IBSWAI                     _IBCR.Bits.IBSWAI
#define IBCR_RSTA                       _IBCR.Bits.RSTA
#define IBCR_TXAK                       _IBCR.Bits.TXAK
#define IBCR_TX_RX                      _IBCR.Bits.TX_RX
#define IBCR_MS_SL                      _IBCR.Bits.MS_SL
#define IBCR_IBIE                       _IBCR.Bits.IBIE
#define IBCR_IBEN                       _IBCR.Bits.IBEN

#define IBCR_IBSWAI_MASK                1U
#define IBCR_RSTA_MASK                  4U
#define IBCR_TXAK_MASK                  8U
#define IBCR_TX_RX_MASK                 16U
#define IBCR_MS_SL_MASK                 32U
#define IBCR_IBIE_MASK                  64U
#define IBCR_IBEN_MASK                  128U


/*** IBSR - IIC Status Register; 0x000000E3 ***/
typedef union {
  byte Byte;
  struct {
    byte RXAK        :1;                                       /* Received Acknowledge */
    byte IBIF        :1;                                       /* I-Bus Interrupt */
    byte SRW         :1;                                       /* Slave Read/Write */
    byte             :1; 
    byte IBAL        :1;                                       /* Arbitration Lost */
    byte IBB         :1;                                       /* Bus busy bit */
    byte IAAS        :1;                                       /* Addressed as a slave bit */
    byte TCF         :1;                                       /* Data transferring bit */
  } Bits;
} IBSRSTR;
extern volatile IBSRSTR _IBSR @(REG_BASE + 0x000000E3UL);
#define IBSR                            _IBSR.Byte
#define IBSR_RXAK                       _IBSR.Bits.RXAK
#define IBSR_IBIF                       _IBSR.Bits.IBIF
#define IBSR_SRW                        _IBSR.Bits.SRW
#define IBSR_IBAL                       _IBSR.Bits.IBAL
#define IBSR_IBB                        _IBSR.Bits.IBB
#define IBSR_IAAS                       _IBSR.Bits.IAAS
#define IBSR_TCF                        _IBSR.Bits.TCF

#define IBSR_RXAK_MASK                  1U
#define IBSR_IBIF_MASK                  2U
#define IBSR_SRW_MASK                   4U
#define IBSR_IBAL_MASK                  16U
#define IBSR_IBB_MASK                   32U
#define IBSR_IAAS_MASK                  64U
#define IBSR_TCF_MASK                   128U


/*** IBDR - IIC Data I/O Register; 0x000000E4 ***/
typedef union {
  byte Byte;
  struct {
    byte D0          :1;                                       /* IIC Data Bit 0 */
    byte D1          :1;                                       /* IIC Data Bit 1 */
    byte D2          :1;                                       /* IIC Data Bit 2 */
    byte D3          :1;                                       /* IIC Data Bit 3 */
    byte D4          :1;                                       /* IIC Data Bit 4 */
    byte D5          :1;                                       /* IIC Data Bit 5 */
    byte D6          :1;                                       /* IIC Data Bit 6 */
    byte D7          :1;                                       /* IIC Data Bit 7 */
  } Bits;
} IBDRSTR;
extern volatile IBDRSTR _IBDR @(REG_BASE + 0x000000E4UL);
#define IBDR                            _IBDR.Byte
#define IBDR_D0                         _IBDR.Bits.D0
#define IBDR_D1                         _IBDR.Bits.D1
#define IBDR_D2                         _IBDR.Bits.D2
#define IBDR_D3                         _IBDR.Bits.D3
#define IBDR_D4                         _IBDR.Bits.D4
#define IBDR_D5                         _IBDR.Bits.D5
#define IBDR_D6                         _IBDR.Bits.D6
#define IBDR_D7                         _IBDR.Bits.D7

#define IBDR_D0_MASK                    1U
#define IBDR_D1_MASK                    2U
#define IBDR_D2_MASK                    4U
#define IBDR_D3_MASK                    8U
#define IBDR_D4_MASK                    16U
#define IBDR_D5_MASK                    32U
#define IBDR_D6_MASK                    64U
#define IBDR_D7_MASK                    128U


/*** SPI1CR1 - SPI 1 Control Register; 0x000000F0 ***/
typedef union {
  byte Byte;
  struct {
    byte LSBFE       :1;                                       /* SPI LSB-First Enable */
    byte SSOE        :1;                                       /* Slave Select Output Enable */
    byte CPHA        :1;                                       /* SPI Clock Phase Bit */
    byte CPOL        :1;                                       /* SPI Clock Polarity Bit */
    byte MSTR        :1;                                       /* SPI Master/Slave Mode Select Bit */
    byte SPTIE       :1;                                       /* SPI Transmit Interrupt Enable */
    byte SPE         :1;                                       /* SPI System Enable Bit */
    byte SPIE        :1;                                       /* SPI Interrupt Enable Bit */
  } Bits;
} SPI1CR1STR;
extern volatile SPI1CR1STR _SPI1CR1 @(REG_BASE + 0x000000F0UL);
#define SPI1CR1                         _SPI1CR1.Byte
#define SPI1CR1_LSBFE                   _SPI1CR1.Bits.LSBFE
#define SPI1CR1_SSOE                    _SPI1CR1.Bits.SSOE
#define SPI1CR1_CPHA                    _SPI1CR1.Bits.CPHA
#define SPI1CR1_CPOL                    _SPI1CR1.Bits.CPOL
#define SPI1CR1_MSTR                    _SPI1CR1.Bits.MSTR
#define SPI1CR1_SPTIE                   _SPI1CR1.Bits.SPTIE
#define SPI1CR1_SPE                     _SPI1CR1.Bits.SPE
#define SPI1CR1_SPIE                    _SPI1CR1.Bits.SPIE

#define SPI1CR1_LSBFE_MASK              1U
#define SPI1CR1_SSOE_MASK               2U
#define SPI1CR1_CPHA_MASK               4U
#define SPI1CR1_CPOL_MASK               8U
#define SPI1CR1_MSTR_MASK               16U
#define SPI1CR1_SPTIE_MASK              32U
#define SPI1CR1_SPE_MASK                64U
#define SPI1CR1_SPIE_MASK               128U


/*** SPI1CR2 - SPI 1 Control Register 2; 0x000000F1 ***/
typedef union {
  byte Byte;
  struct {
    byte SPC0        :1;                                       /* Serial Pin Control Bit 0 */
    byte SPISWAI     :1;                                       /* SPI Stop in Wait Mode Bit */
    byte             :1; 
    byte BIDIROE     :1;                                       /* Output enable in the Bidirectional mode of operation */
    byte MODFEN      :1;                                       /* Mode Fault Enable Bit */
    byte             :1; 
    byte             :1; 
    byte             :1; 
  } Bits;
} SPI1CR2STR;
extern volatile SPI1CR2STR _SPI1CR2 @(REG_BASE + 0x000000F1UL);
#define SPI1CR2                         _SPI1CR2.Byte
#define SPI1CR2_SPC0                    _SPI1CR2.Bits.SPC0
#define SPI1CR2_SPISWAI                 _SPI1CR2.Bits.SPISWAI
#define SPI1CR2_BIDIROE                 _SPI1CR2.Bits.BIDIROE
#define SPI1CR2_MODFEN                  _SPI1CR2.Bits.MODFEN

#define SPI1CR2_SPC0_MASK               1U
#define SPI1CR2_SPISWAI_MASK            2U
#define SPI1CR2_BIDIROE_MASK            8U
#define SPI1CR2_MODFEN_MASK             16U


/*** SPI1BR - SPI 1 Baud Rate Register; 0x000000F2 ***/
typedef union {
  byte Byte;
  struct {
    byte SPR0        :1;                                       /* SPI Baud Rate Selection Bit 0 */
    byte SPR1        :1;                                       /* SPI Baud Rate Selection Bit 1 */
    byte SPR2        :1;                                       /* SPI Baud Rate Selection Bit 2 */
    byte             :1; 
    byte SPPR0       :1;                                       /* SPI Baud Rate Preselection Bits 0 */
    byte SPPR1       :1;                                       /* SPI Baud Rate Preselection Bits 1 */
    byte SPPR2       :1;                                       /* SPI Baud Rate Preselection Bits 2 */
    byte             :1; 
  } Bits;
  struct {
    byte grpSPR  :3;
    byte         :1;
    byte grpSPPR :3;
    byte         :1;
  } MergedBits;
} SPI1BRSTR;
extern volatile SPI1BRSTR _SPI1BR @(REG_BASE + 0x000000F2UL);
#define SPI1BR                          _SPI1BR.Byte
#define SPI1BR_SPR0                     _SPI1BR.Bits.SPR0
#define SPI1BR_SPR1                     _SPI1BR.Bits.SPR1
#define SPI1BR_SPR2                     _SPI1BR.Bits.SPR2
#define SPI1BR_SPPR0                    _SPI1BR.Bits.SPPR0
#define SPI1BR_SPPR1                    _SPI1BR.Bits.SPPR1
#define SPI1BR_SPPR2                    _SPI1BR.Bits.SPPR2
#define SPI1BR_SPR                      _SPI1BR.MergedBits.grpSPR
#define SPI1BR_SPPR                     _SPI1BR.MergedBits.grpSPPR

#define SPI1BR_SPR0_MASK                1U
#define SPI1BR_SPR1_MASK                2U
#define SPI1BR_SPR2_MASK                4U
#define SPI1BR_SPPR0_MASK               16U
#define SPI1BR_SPPR1_MASK               32U
#define SPI1BR_SPPR2_MASK               64U
#define SPI1BR_SPR_MASK                 7U
#define SPI1BR_SPR_BITNUM               0U
#define SPI1BR_SPPR_MASK                112U
#define SPI1BR_SPPR_BITNUM              4U


/*** SPI1SR - SPI 1 Status Register; 0x000000F3 ***/
typedef union {
  byte Byte;
  struct {
    byte             :1; 
    byte             :1; 
    byte             :1; 
    byte             :1; 
    byte MODF        :1;                                       /* Mode Fault Flag */
    byte SPTEF       :1;                                       /* SPI Transmit Empty Interrupt Flag */
    byte             :1; 
    byte SPIF        :1;                                       /* SPIF Receive Interrupt Flag */
  } Bits;
} SPI1SRSTR;
extern volatile SPI1SRSTR _SPI1SR @(REG_BASE + 0x000000F3UL);
#define SPI1SR                          _SPI1SR.Byte
#define SPI1SR_MODF                     _SPI1SR.Bits.MODF
#define SPI1SR_SPTEF                    _SPI1SR.Bits.SPTEF
#define SPI1SR_SPIF                     _SPI1SR.Bits.SPIF

#define SPI1SR_MODF_MASK                16U
#define SPI1SR_SPTEF_MASK               32U
#define SPI1SR_SPIF_MASK                128U


/*** SPI1DR - SPI 1 Data Register; 0x000000F5 ***/
typedef union {
  byte Byte;
} SPI1DRSTR;
extern volatile SPI1DRSTR _SPI1DR @(REG_BASE + 0x000000F5UL);
#define SPI1DR                          _SPI1DR.Byte


/*** SPI2CR1 - SPI 2 Control Register; 0x000000F8 ***/
typedef union {
  byte Byte;
  struct {
    byte LSBFE       :1;                                       /* SPI LSB-First Enable */
    byte SSOE        :1;                                       /* Slave Select Output Enable */
    byte CPHA        :1;                                       /* SPI Clock Phase Bit */
    byte CPOL        :1;                                       /* SPI Clock Polarity Bit */
    byte MSTR        :1;                                       /* SPI Master/Slave Mode Select Bit */
    byte SPTIE       :1;                                       /* SPI Transmit Interrupt Enable */
    byte SPE         :1;                                       /* SPI System Enable Bit */
    byte SPIE        :1;                                       /* SPI Interrupt Enable Bit */
  } Bits;
} SPI2CR1STR;
extern volatile SPI2CR1STR _SPI2CR1 @(REG_BASE + 0x000000F8UL);
#define SPI2CR1                         _SPI2CR1.Byte
#define SPI2CR1_LSBFE                   _SPI2CR1.Bits.LSBFE
#define SPI2CR1_SSOE                    _SPI2CR1.Bits.SSOE
#define SPI2CR1_CPHA                    _SPI2CR1.Bits.CPHA
#define SPI2CR1_CPOL                    _SPI2CR1.Bits.CPOL
#define SPI2CR1_MSTR                    _SPI2CR1.Bits.MSTR
#define SPI2CR1_SPTIE                   _SPI2CR1.Bits.SPTIE
#define SPI2CR1_SPE                     _SPI2CR1.Bits.SPE
#define SPI2CR1_SPIE                    _SPI2CR1.Bits.SPIE

#define SPI2CR1_LSBFE_MASK              1U
#define SPI2CR1_SSOE_MASK               2U
#define SPI2CR1_CPHA_MASK               4U
#define SPI2CR1_CPOL_MASK               8U
#define SPI2CR1_MSTR_MASK               16U
#define SPI2CR1_SPTIE_MASK              32U
#define SPI2CR1_SPE_MASK                64U
#define SPI2CR1_SPIE_MASK               128U


/*** SPI2CR2 - SPI 2 Control Register 2; 0x000000F9 ***/
typedef union {
  byte Byte;
  struct {
    byte SPC0        :1;                                       /* Serial Pin Control Bit 0 */
    byte SPISWAI     :1;                                       /* SPI Stop in Wait Mode Bit */
    byte             :1; 
    byte BIDIROE     :1;                                       /* Output enable in the Bidirectional mode of operation */
    byte MODFEN      :1;                                       /* Mode Fault Enable Bit */
    byte             :1; 
    byte             :1; 
    byte             :1; 
  } Bits;
} SPI2CR2STR;
extern volatile SPI2CR2STR _SPI2CR2 @(REG_BASE + 0x000000F9UL);
#define SPI2CR2                         _SPI2CR2.Byte
#define SPI2CR2_SPC0                    _SPI2CR2.Bits.SPC0
#define SPI2CR2_SPISWAI                 _SPI2CR2.Bits.SPISWAI
#define SPI2CR2_BIDIROE                 _SPI2CR2.Bits.BIDIROE
#define SPI2CR2_MODFEN                  _SPI2CR2.Bits.MODFEN

#define SPI2CR2_SPC0_MASK               1U
#define SPI2CR2_SPISWAI_MASK            2U
#define SPI2CR2_BIDIROE_MASK            8U
#define SPI2CR2_MODFEN_MASK             16U


/*** SPI2BR - SPI 2 Baud Rate Register; 0x000000FA ***/
typedef union {
  byte Byte;
  struct {
    byte SPR0        :1;                                       /* SPI Baud Rate Selection Bit 0 */
    byte SPR1        :1;                                       /* SPI Baud Rate Selection Bit 1 */
    byte SPR2        :1;                                       /* SPI Baud Rate Selection Bit 2 */
    byte             :1; 
    byte SPPR0       :1;                                       /* SPI Baud Rate Preselection Bits 0 */
    byte SPPR1       :1;                                       /* SPI Baud Rate Preselection Bits 1 */
    byte SPPR2       :1;                                       /* SPI Baud Rate Preselection Bits 2 */
    byte             :1; 
  } Bits;
  struct {
    byte grpSPR  :3;
    byte         :1;
    byte grpSPPR :3;
    byte         :1;
  } MergedBits;
} SPI2BRSTR;
extern volatile SPI2BRSTR _SPI2BR @(REG_BASE + 0x000000FAUL);
#define SPI2BR                          _SPI2BR.Byte
#define SPI2BR_SPR0                     _SPI2BR.Bits.SPR0
#define SPI2BR_SPR1                     _SPI2BR.Bits.SPR1
#define SPI2BR_SPR2                     _SPI2BR.Bits.SPR2
#define SPI2BR_SPPR0                    _SPI2BR.Bits.SPPR0
#define SPI2BR_SPPR1                    _SPI2BR.Bits.SPPR1
#define SPI2BR_SPPR2                    _SPI2BR.Bits.SPPR2
#define SPI2BR_SPR                      _SPI2BR.MergedBits.grpSPR
#define SPI2BR_SPPR                     _SPI2BR.MergedBits.grpSPPR

#define SPI2BR_SPR0_MASK                1U
#define SPI2BR_SPR1_MASK                2U
#define SPI2BR_SPR2_MASK                4U
#define SPI2BR_SPPR0_MASK               16U
#define SPI2BR_SPPR1_MASK               32U
#define SPI2BR_SPPR2_MASK               64U
#define SPI2BR_SPR_MASK                 7U
#define SPI2BR_SPR_BITNUM               0U
#define SPI2BR_SPPR_MASK                112U
#define SPI2BR_SPPR_BITNUM              4U


/*** SPI2SR - SPI 2 Status Register; 0x000000FB ***/
typedef union {
  byte Byte;
  struct {
    byte             :1; 
    byte             :1; 
    byte             :1; 
    byte             :1; 
    byte MODF        :1;                                       /* Mode Fault Flag */
    byte SPTEF       :1;                                       /* SPI Transmit Empty Interrupt Flag */
    byte             :1; 
    byte SPIF        :1;                                       /* SPIF Receive Interrupt Flag */
  } Bits;
} SPI2SRSTR;
extern volatile SPI2SRSTR _SPI2SR @(REG_BASE + 0x000000FBUL);
#define SPI2SR                          _SPI2SR.Byte
#define SPI2SR_MODF                     _SPI2SR.Bits.MODF
#define SPI2SR_SPTEF                    _SPI2SR.Bits.SPTEF
#define SPI2SR_SPIF                     _SPI2SR.Bits.SPIF

#define SPI2SR_MODF_MASK                16U
#define SPI2SR_SPTEF_MASK               32U
#define SPI2SR_SPIF_MASK                128U


/*** SPI2DR - SPI 2 Data Register; 0x000000FD ***/
typedef union {
  byte Byte;
} SPI2DRSTR;
extern volatile SPI2DRSTR _SPI2DR @(REG_BASE + 0x000000FDUL);
#define SPI2DR                          _SPI2DR.Byte


/*** FCLKDIV - Flash Clock Divider Register; 0x00000100 ***/
typedef union {
  byte Byte;
  struct {
    byte FDIV0       :1;                                       /* Flash Clock Divider Bit 0 */
    byte FDIV1       :1;                                       /* Flash Clock Divider Bit 1 */
    byte FDIV2       :1;                                       /* Flash Clock Divider Bit 2 */
    byte FDIV3       :1;                                       /* Flash Clock Divider Bit 3 */
    byte FDIV4       :1;                                       /* Flash Clock Divider Bit 4 */
    byte FDIV5       :1;                                       /* Flash Clock Divider Bit 5 */
    byte PRDIV8      :1;                                       /* Enable Prescaler by 8 */
    byte FDIVLD      :1;                                       /* Flash Clock Divider Loaded */
  } Bits;
  struct {
    byte grpFDIV :6;
    byte grpPRDIV_8 :1;
    byte         :1;
  } MergedBits;
} FCLKDIVSTR;
extern volatile FCLKDIVSTR _FCLKDIV @(REG_BASE + 0x00000100UL);
#define FCLKDIV                         _FCLKDIV.Byte
#define FCLKDIV_FDIV0                   _FCLKDIV.Bits.FDIV0
#define FCLKDIV_FDIV1                   _FCLKDIV.Bits.FDIV1
#define FCLKDIV_FDIV2                   _FCLKDIV.Bits.FDIV2
#define FCLKDIV_FDIV3                   _FCLKDIV.Bits.FDIV3
#define FCLKDIV_FDIV4                   _FCLKDIV.Bits.FDIV4
#define FCLKDIV_FDIV5                   _FCLKDIV.Bits.FDIV5
#define FCLKDIV_PRDIV8                  _FCLKDIV.Bits.PRDIV8
#define FCLKDIV_FDIVLD                  _FCLKDIV.Bits.FDIVLD
#define FCLKDIV_FDIV                    _FCLKDIV.MergedBits.grpFDIV

#define FCLKDIV_FDIV0_MASK              1U
#define FCLKDIV_FDIV1_MASK              2U
#define FCLKDIV_FDIV2_MASK              4U
#define FCLKDIV_FDIV3_MASK              8U
#define FCLKDIV_FDIV4_MASK              16U
#define FCLKDIV_FDIV5_MASK              32U
#define FCLKDIV_PRDIV8_MASK             64U
#define FCLKDIV_FDIVLD_MASK             128U
#define FCLKDIV_FDIV_MASK               63U
#define FCLKDIV_FDIV_BITNUM             0U


/*** FSEC - Flash Security Register; 0x00000101 ***/
typedef union {
  byte Byte;
  struct {
    byte SEC0        :1;                                       /* Memory security bit 0 */
    byte SEC1        :1;                                       /* Memory security bit 1 */
    byte NV2         :1;                                       /* Non Volatile flag bit 2 */
    byte NV3         :1;                                       /* Non Volatile flag bit 3 */
    byte NV4         :1;                                       /* Non Volatile flag bit 4 */
    byte NV5         :1;                                       /* Non Volatile flag bit 5 */
    byte KEYEN0      :1;                                       /* Backdoor Key Security Enable Bit 0 */
    byte KEYEN1      :1;                                       /* Backdoor Key Security Enable Bit 1 */
  } Bits;
  struct {
    byte grpSEC  :2;
    byte grpNV_2 :4;
    byte grpKEYEN :2;
  } MergedBits;
} FSECSTR;
extern volatile FSECSTR _FSEC @(REG_BASE + 0x00000101UL);
#define FSEC                            _FSEC.Byte
#define FSEC_SEC0                       _FSEC.Bits.SEC0
#define FSEC_SEC1                       _FSEC.Bits.SEC1
#define FSEC_NV2                        _FSEC.Bits.NV2
#define FSEC_NV3                        _FSEC.Bits.NV3
#define FSEC_NV4                        _FSEC.Bits.NV4
#define FSEC_NV5                        _FSEC.Bits.NV5
#define FSEC_KEYEN0                     _FSEC.Bits.KEYEN0
#define FSEC_KEYEN1                     _FSEC.Bits.KEYEN1
#define FSEC_SEC                        _FSEC.MergedBits.grpSEC
#define FSEC_NV_2                       _FSEC.MergedBits.grpNV_2
#define FSEC_KEYEN                      _FSEC.MergedBits.grpKEYEN
#define FSEC_NV                         FSEC_NV_2

#define FSEC_SEC0_MASK                  1U
#define FSEC_SEC1_MASK                  2U
#define FSEC_NV2_MASK                   4U
#define FSEC_NV3_MASK                   8U
#define FSEC_NV4_MASK                   16U
#define FSEC_NV5_MASK                   32U
#define FSEC_KEYEN0_MASK                64U
#define FSEC_KEYEN1_MASK                128U
#define FSEC_SEC_MASK                   3U
#define FSEC_SEC_BITNUM                 0U
#define FSEC_NV_2_MASK                  60U
#define FSEC_NV_2_BITNUM                2U
#define FSEC_KEYEN_MASK                 192U
#define FSEC_KEYEN_BITNUM               6U


/*** FCNFG - Flash Configuration Register; 0x00000103 ***/
typedef union {
  byte Byte;
  struct {
    byte BKSEL0      :1;                                       /* Register bank select 0 */
    byte BKSEL1      :1;                                       /* Register bank select 1 */
    byte             :1; 
    byte             :1; 
    byte             :1; 
    byte KEYACC      :1;                                       /* Enable Security Key Writing */
    byte CCIE        :1;                                       /* Command Complete Interrupt Enable */
    byte CBEIE       :1;                                       /* Command Buffers Empty Interrupt Enable */
  } Bits;
  struct {
    byte grpBKSEL :2;
    byte         :1;
    byte         :1;
    byte         :1;
    byte         :1;
    byte         :1;
    byte         :1;
  } MergedBits;
} FCNFGSTR;
extern volatile FCNFGSTR _FCNFG @(REG_BASE + 0x00000103UL);
#define FCNFG                           _FCNFG.Byte
#define FCNFG_BKSEL0                    _FCNFG.Bits.BKSEL0
#define FCNFG_BKSEL1                    _FCNFG.Bits.BKSEL1
#define FCNFG_KEYACC                    _FCNFG.Bits.KEYACC
#define FCNFG_CCIE                      _FCNFG.Bits.CCIE
#define FCNFG_CBEIE                     _FCNFG.Bits.CBEIE
#define FCNFG_BKSEL                     _FCNFG.MergedBits.grpBKSEL

#define FCNFG_BKSEL0_MASK               1U
#define FCNFG_BKSEL1_MASK               2U
#define FCNFG_KEYACC_MASK               32U
#define FCNFG_CCIE_MASK                 64U
#define FCNFG_CBEIE_MASK                128U
#define FCNFG_BKSEL_MASK                3U
#define FCNFG_BKSEL_BITNUM              0U


/*** FPROT - Flash Protection Register; 0x00000104 ***/
typedef union {
  byte Byte;
  struct {
    byte FPLS0       :1;                                       /* Flash Protection Lower Address size 0 */
    byte FPLS1       :1;                                       /* Flash Protection Lower Address size 1 */
    byte FPLDIS      :1;                                       /* Flash Protection Lower address range disable */
    byte FPHS0       :1;                                       /* Flash Protection Higher address size 0 */
    byte FPHS1       :1;                                       /* Flash Protection Higher address size 1 */
    byte FPHDIS      :1;                                       /* Flash Protection Higher address range disable */
    byte NV6         :1;                                       /* Non Volatile Flag Bit */
    byte FPOPEN      :1;                                       /* Opens the flash block or subsections of it for program or erase */
  } Bits;
  struct {
    byte grpFPLS :2;
    byte         :1;
    byte grpFPHS :2;
    byte         :1;
    byte grpNV_6 :1;
    byte         :1;
  } MergedBits;
} FPROTSTR;
extern volatile FPROTSTR _FPROT @(REG_BASE + 0x00000104UL);
#define FPROT                           _FPROT.Byte
#define FPROT_FPLS0                     _FPROT.Bits.FPLS0
#define FPROT_FPLS1                     _FPROT.Bits.FPLS1
#define FPROT_FPLDIS                    _FPROT.Bits.FPLDIS
#define FPROT_FPHS0                     _FPROT.Bits.FPHS0
#define FPROT_FPHS1                     _FPROT.Bits.FPHS1
#define FPROT_FPHDIS                    _FPROT.Bits.FPHDIS
#define FPROT_NV6                       _FPROT.Bits.NV6
#define FPROT_FPOPEN                    _FPROT.Bits.FPOPEN
#define FPROT_FPLS                      _FPROT.MergedBits.grpFPLS
#define FPROT_FPHS                      _FPROT.MergedBits.grpFPHS

#define FPROT_FPLS0_MASK                1U
#define FPROT_FPLS1_MASK                2U
#define FPROT_FPLDIS_MASK               4U
#define FPROT_FPHS0_MASK                8U
#define FPROT_FPHS1_MASK                16U
#define FPROT_FPHDIS_MASK               32U
#define FPROT_NV6_MASK                  64U
#define FPROT_FPOPEN_MASK               128U
#define FPROT_FPLS_MASK                 3U
#define FPROT_FPLS_BITNUM               0U
#define FPROT_FPHS_MASK                 24U
#define FPROT_FPHS_BITNUM               3U


/*** FSTAT - Flash Status Register; 0x00000105 ***/
typedef union {
  byte Byte;
  struct {
    byte             :1; 
    byte             :1; 
    byte BLANK       :1;                                       /* Blank Verify Flag */
    byte             :1; 
    byte ACCERR      :1;                                       /* Access error */
    byte PVIOL       :1;                                       /* Protection violation */
    byte CCIF        :1;                                       /* Command Complete Interrupt Flag */
    byte CBEIF       :1;                                       /* Command Buffers Empty Interrupt Flag */
  } Bits;
} FSTATSTR;
extern volatile FSTATSTR _FSTAT @(REG_BASE + 0x00000105UL);
#define FSTAT                           _FSTAT.Byte
#define FSTAT_BLANK                     _FSTAT.Bits.BLANK
#define FSTAT_ACCERR                    _FSTAT.Bits.ACCERR
#define FSTAT_PVIOL                     _FSTAT.Bits.PVIOL
#define FSTAT_CCIF                      _FSTAT.Bits.CCIF
#define FSTAT_CBEIF                     _FSTAT.Bits.CBEIF

#define FSTAT_BLANK_MASK                4U
#define FSTAT_ACCERR_MASK               16U
#define FSTAT_PVIOL_MASK                32U
#define FSTAT_CCIF_MASK                 64U
#define FSTAT_CBEIF_MASK                128U


/*** FCMD - Flash Command Buffer and Register; 0x00000106 ***/
typedef union {
  byte Byte;
  struct {
    byte CMDB0       :1;                                       /* NVM User Mode Command Bit 0 */
    byte             :1; 
    byte CMDB2       :1;                                       /* NVM User Mode Command Bit 2 */
    byte             :1; 
    byte             :1; 
    byte CMDB5       :1;                                       /* NVM User Mode Command Bit 5 */
    byte CMDB6       :1;                                       /* NVM User Mode Command Bit 6 */
    byte             :1; 
  } Bits;
  struct {
    byte grpCMDB :1;
    byte         :1;
    byte grpCMDB_2 :1;
    byte         :1;
    byte         :1;
    byte grpCMDB_5 :2;
    byte         :1;
  } MergedBits;
} FCMDSTR;
extern volatile FCMDSTR _FCMD @(REG_BASE + 0x00000106UL);
#define FCMD                            _FCMD.Byte
#define FCMD_CMDB0                      _FCMD.Bits.CMDB0
#define FCMD_CMDB2                      _FCMD.Bits.CMDB2
#define FCMD_CMDB5                      _FCMD.Bits.CMDB5
#define FCMD_CMDB6                      _FCMD.Bits.CMDB6
#define FCMD_CMDB_5                     _FCMD.MergedBits.grpCMDB_5
#define FCMD_CMDB                       FCMD_CMDB_5

#define FCMD_CMDB0_MASK                 1U
#define FCMD_CMDB2_MASK                 4U
#define FCMD_CMDB5_MASK                 32U
#define FCMD_CMDB6_MASK                 64U
#define FCMD_CMDB_5_MASK                96U
#define FCMD_CMDB_5_BITNUM              5U


/*** ECLKDIV - EEPROM Clock Divider Register; 0x00000110 ***/
typedef union {
  byte Byte;
  struct {
    byte EDIV0       :1;                                       /* EEPROM Clock Divider 0 */
    byte EDIV1       :1;                                       /* EEPROM Clock Divider 1 */
    byte EDIV2       :1;                                       /* EEPROM Clock Divider 2 */
    byte EDIV3       :1;                                       /* EEPROM Clock Divider 3 */
    byte EDIV4       :1;                                       /* EEPROM Clock Divider 4 */
    byte EDIV5       :1;                                       /* EEPROM Clock Divider 5 */
    byte PRDIV8      :1;                                       /* Enable Prescaler by 8 */
    byte EDIVLD      :1;                                       /* EEPROM Clock Divider Loaded */
  } Bits;
  struct {
    byte grpEDIV :6;
    byte grpPRDIV_8 :1;
    byte         :1;
  } MergedBits;
} ECLKDIVSTR;
extern volatile ECLKDIVSTR _ECLKDIV @(REG_BASE + 0x00000110UL);
#define ECLKDIV                         _ECLKDIV.Byte
#define ECLKDIV_EDIV0                   _ECLKDIV.Bits.EDIV0
#define ECLKDIV_EDIV1                   _ECLKDIV.Bits.EDIV1
#define ECLKDIV_EDIV2                   _ECLKDIV.Bits.EDIV2
#define ECLKDIV_EDIV3                   _ECLKDIV.Bits.EDIV3
#define ECLKDIV_EDIV4                   _ECLKDIV.Bits.EDIV4
#define ECLKDIV_EDIV5                   _ECLKDIV.Bits.EDIV5
#define ECLKDIV_PRDIV8                  _ECLKDIV.Bits.PRDIV8
#define ECLKDIV_EDIVLD                  _ECLKDIV.Bits.EDIVLD
#define ECLKDIV_EDIV                    _ECLKDIV.MergedBits.grpEDIV

#define ECLKDIV_EDIV0_MASK              1U
#define ECLKDIV_EDIV1_MASK              2U
#define ECLKDIV_EDIV2_MASK              4U
#define ECLKDIV_EDIV3_MASK              8U
#define ECLKDIV_EDIV4_MASK              16U
#define ECLKDIV_EDIV5_MASK              32U
#define ECLKDIV_PRDIV8_MASK             64U
#define ECLKDIV_EDIVLD_MASK             128U
#define ECLKDIV_EDIV_MASK               63U
#define ECLKDIV_EDIV_BITNUM             0U


/*** ECNFG - EEPROM Configuration Register; 0x00000113 ***/
typedef union {
  byte Byte;
  struct {
    byte             :1; 
    byte             :1; 
    byte             :1; 
    byte             :1; 
    byte             :1; 
    byte             :1; 
    byte CCIE        :1;                                       /* Command Complete Interrupt Enable */
    byte CBEIE       :1;                                       /* Command Buffers Empty Interrupt Enable */
  } Bits;
} ECNFGSTR;
extern volatile ECNFGSTR _ECNFG @(REG_BASE + 0x00000113UL);
#define ECNFG                           _ECNFG.Byte
#define ECNFG_CCIE                      _ECNFG.Bits.CCIE
#define ECNFG_CBEIE                     _ECNFG.Bits.CBEIE

#define ECNFG_CCIE_MASK                 64U
#define ECNFG_CBEIE_MASK                128U


/*** EPROT - EEPROM Protection Register; 0x00000114 ***/
typedef union {
  byte Byte;
  struct {
    byte EP0         :1;                                       /* EEPROM Protection address size 0 */
    byte EP1         :1;                                       /* EEPROM Protection address size 1 */
    byte EP2         :1;                                       /* EEPROM Protection address size 2 */
    byte EPDIS       :1;                                       /* EEPROM Protection disable */
    byte NV4         :1;                                       /* Non Volatile Flag Bit 4 */
    byte NV5         :1;                                       /* Non Volatile Flag Bit 5 */
    byte NV6         :1;                                       /* Non Volatile Flag Bit 6 */
    byte EPOPEN      :1;                                       /* Opens the EEPROM block or a subsection of it for program or erase */
  } Bits;
  struct {
    byte grpEP   :3;
    byte         :1;
    byte grpNV_4 :3;
    byte         :1;
  } MergedBits;
} EPROTSTR;
extern volatile EPROTSTR _EPROT @(REG_BASE + 0x00000114UL);
#define EPROT                           _EPROT.Byte
#define EPROT_EP0                       _EPROT.Bits.EP0
#define EPROT_EP1                       _EPROT.Bits.EP1
#define EPROT_EP2                       _EPROT.Bits.EP2
#define EPROT_EPDIS                     _EPROT.Bits.EPDIS
#define EPROT_NV4                       _EPROT.Bits.NV4
#define EPROT_NV5                       _EPROT.Bits.NV5
#define EPROT_NV6                       _EPROT.Bits.NV6
#define EPROT_EPOPEN                    _EPROT.Bits.EPOPEN
#define EPROT_EP                        _EPROT.MergedBits.grpEP
#define EPROT_NV_4                      _EPROT.MergedBits.grpNV_4
#define EPROT_NV                        EPROT_NV_4

#define EPROT_EP0_MASK                  1U
#define EPROT_EP1_MASK                  2U
#define EPROT_EP2_MASK                  4U
#define EPROT_EPDIS_MASK                8U
#define EPROT_NV4_MASK                  16U
#define EPROT_NV5_MASK                  32U
#define EPROT_NV6_MASK                  64U
#define EPROT_EPOPEN_MASK               128U
#define EPROT_EP_MASK                   7U
#define EPROT_EP_BITNUM                 0U
#define EPROT_NV_4_MASK                 112U
#define EPROT_NV_4_BITNUM               4U


/*** ESTAT - EEPROM Status Register; 0x00000115 ***/
typedef union {
  byte Byte;
  struct {
    byte             :1; 
    byte             :1; 
    byte BLANK       :1;                                       /* Blank Verify Flag */
    byte             :1; 
    byte ACCERR      :1;                                       /* Access error */
    byte PVIOL       :1;                                       /* Protection violation */
    byte CCIF        :1;                                       /* Command Complete Interrupt Flag */
    byte CBEIF       :1;                                       /* Command Buffer Empty Interrupt Flag */
  } Bits;
} ESTATSTR;
extern volatile ESTATSTR _ESTAT @(REG_BASE + 0x00000115UL);
#define ESTAT                           _ESTAT.Byte
#define ESTAT_BLANK                     _ESTAT.Bits.BLANK
#define ESTAT_ACCERR                    _ESTAT.Bits.ACCERR
#define ESTAT_PVIOL                     _ESTAT.Bits.PVIOL
#define ESTAT_CCIF                      _ESTAT.Bits.CCIF
#define ESTAT_CBEIF                     _ESTAT.Bits.CBEIF

#define ESTAT_BLANK_MASK                4U
#define ESTAT_ACCERR_MASK               16U
#define ESTAT_PVIOL_MASK                32U
#define ESTAT_CCIF_MASK                 64U
#define ESTAT_CBEIF_MASK                128U


/*** ECMD - EEPROM Command Buffer and Register; 0x00000116 ***/
typedef union {
  byte Byte;
  struct {
    byte CMDB0       :1;                                       /* EEPROM User Mode Command 0 */
    byte             :1; 
    byte CMDB2       :1;                                       /* EEPROM User Mode Command 2 */
    byte             :1; 
    byte             :1; 
    byte CMDB5       :1;                                       /* EEPROM User Mode Command 5 */
    byte CMDB6       :1;                                       /* EEPROM User Mode Command 6 */
    byte             :1; 
  } Bits;
  struct {
    byte grpCMDB :1;
    byte         :1;
    byte grpCMDB_2 :1;
    byte         :1;
    byte         :1;
    byte grpCMDB_5 :2;
    byte         :1;
  } MergedBits;
} ECMDSTR;
extern volatile ECMDSTR _ECMD @(REG_BASE + 0x00000116UL);
#define ECMD                            _ECMD.Byte
#define ECMD_CMDB0                      _ECMD.Bits.CMDB0
#define ECMD_CMDB2                      _ECMD.Bits.CMDB2
#define ECMD_CMDB5                      _ECMD.Bits.CMDB5
#define ECMD_CMDB6                      _ECMD.Bits.CMDB6
#define ECMD_CMDB_5                     _ECMD.MergedBits.grpCMDB_5
#define ECMD_CMDB                       ECMD_CMDB_5

#define ECMD_CMDB0_MASK                 1U
#define ECMD_CMDB2_MASK                 4U
#define ECMD_CMDB5_MASK                 32U
#define ECMD_CMDB6_MASK                 64U
#define ECMD_CMDB_5_MASK                96U
#define ECMD_CMDB_5_BITNUM              5U


/*** ATD1CTL23 - ATD 1 Control Register 23; 0x00000122 ***/
typedef union {
  word Word;
   /* Overlapped registers: */
  struct {
    /*** ATD1CTL2 - ATD 1 Control Register 2; 0x00000122 ***/
    union {
      byte Byte;
      struct {
        byte ASCIF       :1;                                       /* ATD 1 Sequence Complete Interrupt Flag */
        byte ASCIE       :1;                                       /* ATD 1 Sequence Complete Interrupt Enable */
        byte ETRIGE      :1;                                       /* External Trigger Mode enable */
        byte ETRIGP      :1;                                       /* External Trigger Polarity */
        byte ETRIGLE     :1;                                       /* External Trigger Level/Edge control */
        byte AWAI        :1;                                       /* ATD Power Down in Wait Mode */
        byte AFFC        :1;                                       /* ATD Fast Conversion Complete Flag Clear */
        byte ADPU        :1;                                       /* ATD Disable / Power Down */
      } Bits;
    } ATD1CTL2STR;
    #define ATD1CTL2                    _ATD1CTL23.Overlap_STR.ATD1CTL2STR.Byte
    #define ATD1CTL2_ASCIF              _ATD1CTL23.Overlap_STR.ATD1CTL2STR.Bits.ASCIF
    #define ATD1CTL2_ASCIE              _ATD1CTL23.Overlap_STR.ATD1CTL2STR.Bits.ASCIE
    #define ATD1CTL2_ETRIGE             _ATD1CTL23.Overlap_STR.ATD1CTL2STR.Bits.ETRIGE
    #define ATD1CTL2_ETRIGP             _ATD1CTL23.Overlap_STR.ATD1CTL2STR.Bits.ETRIGP
    #define ATD1CTL2_ETRIGLE            _ATD1CTL23.Overlap_STR.ATD1CTL2STR.Bits.ETRIGLE
    #define ATD1CTL2_AWAI               _ATD1CTL23.Overlap_STR.ATD1CTL2STR.Bits.AWAI
    #define ATD1CTL2_AFFC               _ATD1CTL23.Overlap_STR.ATD1CTL2STR.Bits.AFFC
    #define ATD1CTL2_ADPU               _ATD1CTL23.Overlap_STR.ATD1CTL2STR.Bits.ADPU
    
    #define ATD1CTL2_ASCIF_MASK         1U
    #define ATD1CTL2_ASCIE_MASK         2U
    #define ATD1CTL2_ETRIGE_MASK        4U
    #define ATD1CTL2_ETRIGP_MASK        8U
    #define ATD1CTL2_ETRIGLE_MASK       16U
    #define ATD1CTL2_AWAI_MASK          32U
    #define ATD1CTL2_AFFC_MASK          64U
    #define ATD1CTL2_ADPU_MASK          128U
    

    /*** ATD1CTL3 - ATD 1 Control Register 3; 0x00000123 ***/
    union {
      byte Byte;
      struct {
        byte FRZ0        :1;                                       /* Background Debug Freeze Enable Bit 0 */
        byte FRZ1        :1;                                       /* Background Debug Freeze Enable Bit 1 */
        byte FIFO        :1;                                       /* Result Register FIFO Mode */
        byte S1C         :1;                                       /* Conversion Sequence Length 1 */
        byte S2C         :1;                                       /* Conversion Sequence Length 2 */
        byte S4C         :1;                                       /* Conversion Sequence Length 4 */
        byte S8C         :1;                                       /* Conversion Sequence Length 8 */
        byte             :1; 
      } Bits;
      struct {
        byte grpFRZ :2;
        byte     :1;
        byte     :1;
        byte     :1;
        byte     :1;
        byte     :1;
        byte     :1;
      } MergedBits;
    } ATD1CTL3STR;
    #define ATD1CTL3                    _ATD1CTL23.Overlap_STR.ATD1CTL3STR.Byte
    #define ATD1CTL3_FRZ0               _ATD1CTL23.Overlap_STR.ATD1CTL3STR.Bits.FRZ0
    #define ATD1CTL3_FRZ1               _ATD1CTL23.Overlap_STR.ATD1CTL3STR.Bits.FRZ1
    #define ATD1CTL3_FIFO               _ATD1CTL23.Overlap_STR.ATD1CTL3STR.Bits.FIFO
    #define ATD1CTL3_S1C                _ATD1CTL23.Overlap_STR.ATD1CTL3STR.Bits.S1C
    #define ATD1CTL3_S2C                _ATD1CTL23.Overlap_STR.ATD1CTL3STR.Bits.S2C
    #define ATD1CTL3_S4C                _ATD1CTL23.Overlap_STR.ATD1CTL3STR.Bits.S4C
    #define ATD1CTL3_S8C                _ATD1CTL23.Overlap_STR.ATD1CTL3STR.Bits.S8C
    #define ATD1CTL3_FRZ                _ATD1CTL23.Overlap_STR.ATD1CTL3STR.MergedBits.grpFRZ
    
    #define ATD1CTL3_FRZ0_MASK          1U
    #define ATD1CTL3_FRZ1_MASK          2U
    #define ATD1CTL3_FIFO_MASK          4U
    #define ATD1CTL3_S1C_MASK           8U
    #define ATD1CTL3_S2C_MASK           16U
    #define ATD1CTL3_S4C_MASK           32U
    #define ATD1CTL3_S8C_MASK           64U
    #define ATD1CTL3_FRZ_MASK           3U
    #define ATD1CTL3_FRZ_BITNUM         0U
    
  } Overlap_STR;

  struct {
    word FRZ0        :1;                                       /* Background Debug Freeze Enable Bit 0 */
    word FRZ1        :1;                                       /* Background Debug Freeze Enable Bit 1 */
    word FIFO        :1;                                       /* Result Register FIFO Mode */
    word S1C         :1;                                       /* Conversion Sequence Length 1 */
    word S2C         :1;                                       /* Conversion Sequence Length 2 */
    word S4C         :1;                                       /* Conversion Sequence Length 4 */
    word S8C         :1;                                       /* Conversion Sequence Length 8 */
    word             :1; 
    word ASCIF       :1;                                       /* ATD 1 Sequence Complete Interrupt Flag */
    word ASCIE       :1;                                       /* ATD 1 Sequence Complete Interrupt Enable */
    word ETRIGE      :1;                                       /* External Trigger Mode enable */
    word ETRIGP      :1;                                       /* External Trigger Polarity */
    word ETRIGLE     :1;                                       /* External Trigger Level/Edge control */
    word AWAI        :1;                                       /* ATD Power Down in Wait Mode */
    word AFFC        :1;                                       /* ATD Fast Conversion Complete Flag Clear */
    word ADPU        :1;                                       /* ATD Disable / Power Down */
  } Bits;
  struct {
    word grpFRZ  :2;
    word         :1;
    word         :1;
    word         :1;
    word         :1;
    word         :1;
    word         :1;
    word         :1;
    word         :1;
    word         :1;
    word         :1;
    word         :1;
    word         :1;
    word         :1;
    word         :1;
  } MergedBits;
} ATD1CTL23STR;
extern volatile ATD1CTL23STR _ATD1CTL23 @(REG_BASE + 0x00000122UL);
#define ATD1CTL23                       _ATD1CTL23.Word
#define ATD1CTL23_FRZ0                  _ATD1CTL23.Bits.FRZ0
#define ATD1CTL23_FRZ1                  _ATD1CTL23.Bits.FRZ1
#define ATD1CTL23_FIFO                  _ATD1CTL23.Bits.FIFO
#define ATD1CTL23_S1C                   _ATD1CTL23.Bits.S1C
#define ATD1CTL23_S2C                   _ATD1CTL23.Bits.S2C
#define ATD1CTL23_S4C                   _ATD1CTL23.Bits.S4C
#define ATD1CTL23_S8C                   _ATD1CTL23.Bits.S8C
#define ATD1CTL23_ASCIF                 _ATD1CTL23.Bits.ASCIF
#define ATD1CTL23_ASCIE                 _ATD1CTL23.Bits.ASCIE
#define ATD1CTL23_ETRIGE                _ATD1CTL23.Bits.ETRIGE
#define ATD1CTL23_ETRIGP                _ATD1CTL23.Bits.ETRIGP
#define ATD1CTL23_ETRIGLE               _ATD1CTL23.Bits.ETRIGLE
#define ATD1CTL23_AWAI                  _ATD1CTL23.Bits.AWAI
#define ATD1CTL23_AFFC                  _ATD1CTL23.Bits.AFFC
#define ATD1CTL23_ADPU                  _ATD1CTL23.Bits.ADPU
#define ATD1CTL23_FRZ                   _ATD1CTL23.MergedBits.grpFRZ

#define ATD1CTL23_FRZ0_MASK             1U
#define ATD1CTL23_FRZ1_MASK             2U
#define ATD1CTL23_FIFO_MASK             4U
#define ATD1CTL23_S1C_MASK              8U
#define ATD1CTL23_S2C_MASK              16U
#define ATD1CTL23_S4C_MASK              32U
#define ATD1CTL23_S8C_MASK              64U
#define ATD1CTL23_ASCIF_MASK            256U
#define ATD1CTL23_ASCIE_MASK            512U
#define ATD1CTL23_ETRIGE_MASK           1024U
#define ATD1CTL23_ETRIGP_MASK           2048U
#define ATD1CTL23_ETRIGLE_MASK          4096U
#define ATD1CTL23_AWAI_MASK             8192U
#define ATD1CTL23_AFFC_MASK             16384U
#define ATD1CTL23_ADPU_MASK             32768U
#define ATD1CTL23_FRZ_MASK              3U
#define ATD1CTL23_FRZ_BITNUM            0U


/*** ATD1CTL45 - ATD 1 Control Register 45; 0x00000124 ***/
typedef union {
  word Word;
   /* Overlapped registers: */
  struct {
    /*** ATD1CTL4 - ATD 1 Control Register 4; 0x00000124 ***/
    union {
      byte Byte;
      struct {
        byte PRS0        :1;                                       /* ATD Clock Prescaler 0 */
        byte PRS1        :1;                                       /* ATD Clock Prescaler 1 */
        byte PRS2        :1;                                       /* ATD Clock Prescaler 2 */
        byte PRS3        :1;                                       /* ATD Clock Prescaler 3 */
        byte PRS4        :1;                                       /* ATD Clock Prescaler 4 */
        byte SMP0        :1;                                       /* Sample Time Select 0 */
        byte SMP1        :1;                                       /* Sample Time Select 1 */
        byte SRES8       :1;                                       /* ATD Resolution Select */
      } Bits;
      struct {
        byte grpPRS :5;
        byte grpSMP :2;
        byte grpSRES_8 :1;
      } MergedBits;
    } ATD1CTL4STR;
    #define ATD1CTL4                    _ATD1CTL45.Overlap_STR.ATD1CTL4STR.Byte
    #define ATD1CTL4_PRS0               _ATD1CTL45.Overlap_STR.ATD1CTL4STR.Bits.PRS0
    #define ATD1CTL4_PRS1               _ATD1CTL45.Overlap_STR.ATD1CTL4STR.Bits.PRS1
    #define ATD1CTL4_PRS2               _ATD1CTL45.Overlap_STR.ATD1CTL4STR.Bits.PRS2
    #define ATD1CTL4_PRS3               _ATD1CTL45.Overlap_STR.ATD1CTL4STR.Bits.PRS3
    #define ATD1CTL4_PRS4               _ATD1CTL45.Overlap_STR.ATD1CTL4STR.Bits.PRS4
    #define ATD1CTL4_SMP0               _ATD1CTL45.Overlap_STR.ATD1CTL4STR.Bits.SMP0
    #define ATD1CTL4_SMP1               _ATD1CTL45.Overlap_STR.ATD1CTL4STR.Bits.SMP1
    #define ATD1CTL4_SRES8              _ATD1CTL45.Overlap_STR.ATD1CTL4STR.Bits.SRES8
    #define ATD1CTL4_PRS                _ATD1CTL45.Overlap_STR.ATD1CTL4STR.MergedBits.grpPRS
    #define ATD1CTL4_SMP                _ATD1CTL45.Overlap_STR.ATD1CTL4STR.MergedBits.grpSMP
    
    #define ATD1CTL4_PRS0_MASK          1U
    #define ATD1CTL4_PRS1_MASK          2U
    #define ATD1CTL4_PRS2_MASK          4U
    #define ATD1CTL4_PRS3_MASK          8U
    #define ATD1CTL4_PRS4_MASK          16U
    #define ATD1CTL4_SMP0_MASK          32U
    #define ATD1CTL4_SMP1_MASK          64U
    #define ATD1CTL4_SRES8_MASK         128U
    #define ATD1CTL4_PRS_MASK           31U
    #define ATD1CTL4_PRS_BITNUM         0U
    #define ATD1CTL4_SMP_MASK           96U
    #define ATD1CTL4_SMP_BITNUM         5U
    

    /*** ATD1CTL5 - ATD 1 Control Register 5; 0x00000125 ***/
    union {
      byte Byte;
      struct {
        byte CA          :1;                                       /* Analog Input Channel Select Code A */
        byte CB          :1;                                       /* Analog Input Channel Select Code B */
        byte CC          :1;                                       /* Analog Input Channel Select Code C */
        byte             :1; 
        byte MULT        :1;                                       /* Multi-Channel Sample Mode */
        byte SCAN        :1;                                       /* Continuous Conversion Sequence Mode */
        byte DSGN        :1;                                       /* Signed/Unsigned Result Data Mode */
        byte DJM         :1;                                       /* Result Register Data Justification Mode */
      } Bits;
      struct {
        byte grpCx :3;
        byte     :1;
        byte     :1;
        byte     :1;
        byte     :1;
        byte     :1;
      } MergedBits;
    } ATD1CTL5STR;
    #define ATD1CTL5                    _ATD1CTL45.Overlap_STR.ATD1CTL5STR.Byte
    #define ATD1CTL5_CA                 _ATD1CTL45.Overlap_STR.ATD1CTL5STR.Bits.CA
    #define ATD1CTL5_CB                 _ATD1CTL45.Overlap_STR.ATD1CTL5STR.Bits.CB
    #define ATD1CTL5_CC                 _ATD1CTL45.Overlap_STR.ATD1CTL5STR.Bits.CC
    #define ATD1CTL5_MULT               _ATD1CTL45.Overlap_STR.ATD1CTL5STR.Bits.MULT
    #define ATD1CTL5_SCAN               _ATD1CTL45.Overlap_STR.ATD1CTL5STR.Bits.SCAN
    #define ATD1CTL5_DSGN               _ATD1CTL45.Overlap_STR.ATD1CTL5STR.Bits.DSGN
    #define ATD1CTL5_DJM                _ATD1CTL45.Overlap_STR.ATD1CTL5STR.Bits.DJM
    #define ATD1CTL5_Cx                 _ATD1CTL45.Overlap_STR.ATD1CTL5STR.MergedBits.grpCx
    
    #define ATD1CTL5_CA_MASK            1U
    #define ATD1CTL5_CB_MASK            2U
    #define ATD1CTL5_CC_MASK            4U
    #define ATD1CTL5_MULT_MASK          16U
    #define ATD1CTL5_SCAN_MASK          32U
    #define ATD1CTL5_DSGN_MASK          64U
    #define ATD1CTL5_DJM_MASK           128U
    #define ATD1CTL5_Cx_MASK            7U
    #define ATD1CTL5_Cx_BITNUM          0U
    
  } Overlap_STR;

  struct {
    word CA          :1;                                       /* Analog Input Channel Select Code A */
    word CB          :1;                                       /* Analog Input Channel Select Code B */
    word CC          :1;                                       /* Analog Input Channel Select Code C */
    word             :1; 
    word MULT        :1;                                       /* Multi-Channel Sample Mode */
    word SCAN        :1;                                       /* Continuous Conversion Sequence Mode */
    word DSGN        :1;                                       /* Signed/Unsigned Result Data Mode */
    word DJM         :1;                                       /* Result Register Data Justification Mode */
    word PRS0        :1;                                       /* ATD Clock Prescaler 0 */
    word PRS1        :1;                                       /* ATD Clock Prescaler 1 */
    word PRS2        :1;                                       /* ATD Clock Prescaler 2 */
    word PRS3        :1;                                       /* ATD Clock Prescaler 3 */
    word PRS4        :1;                                       /* ATD Clock Prescaler 4 */
    word SMP0        :1;                                       /* Sample Time Select 0 */
    word SMP1        :1;                                       /* Sample Time Select 1 */
    word SRES8       :1;                                       /* ATD Resolution Select */
  } Bits;
  struct {
    word grpCx   :3;
    word         :1;
    word         :1;
    word         :1;
    word         :1;
    word         :1;
    word grpPRS  :5;
    word grpSMP  :2;
    word grpSRES_8 :1;
  } MergedBits;
} ATD1CTL45STR;
extern volatile ATD1CTL45STR _ATD1CTL45 @(REG_BASE + 0x00000124UL);
#define ATD1CTL45                       _ATD1CTL45.Word
#define ATD1CTL45_CA                    _ATD1CTL45.Bits.CA
#define ATD1CTL45_CB                    _ATD1CTL45.Bits.CB
#define ATD1CTL45_CC                    _ATD1CTL45.Bits.CC
#define ATD1CTL45_MULT                  _ATD1CTL45.Bits.MULT
#define ATD1CTL45_SCAN                  _ATD1CTL45.Bits.SCAN
#define ATD1CTL45_DSGN                  _ATD1CTL45.Bits.DSGN
#define ATD1CTL45_DJM                   _ATD1CTL45.Bits.DJM
#define ATD1CTL45_PRS0                  _ATD1CTL45.Bits.PRS0
#define ATD1CTL45_PRS1                  _ATD1CTL45.Bits.PRS1
#define ATD1CTL45_PRS2                  _ATD1CTL45.Bits.PRS2
#define ATD1CTL45_PRS3                  _ATD1CTL45.Bits.PRS3
#define ATD1CTL45_PRS4                  _ATD1CTL45.Bits.PRS4
#define ATD1CTL45_SMP0                  _ATD1CTL45.Bits.SMP0
#define ATD1CTL45_SMP1                  _ATD1CTL45.Bits.SMP1
#define ATD1CTL45_SRES8                 _ATD1CTL45.Bits.SRES8
#define ATD1CTL45_Cx                    _ATD1CTL45.MergedBits.grpCx
#define ATD1CTL45_PRS                   _ATD1CTL45.MergedBits.grpPRS
#define ATD1CTL45_SMP                   _ATD1CTL45.MergedBits.grpSMP

#define ATD1CTL45_CA_MASK               1U
#define ATD1CTL45_CB_MASK               2U
#define ATD1CTL45_CC_MASK               4U
#define ATD1CTL45_MULT_MASK             16U
#define ATD1CTL45_SCAN_MASK             32U
#define ATD1CTL45_DSGN_MASK             64U
#define ATD1CTL45_DJM_MASK              128U
#define ATD1CTL45_PRS0_MASK             256U
#define ATD1CTL45_PRS1_MASK             512U
#define ATD1CTL45_PRS2_MASK             1024U
#define ATD1CTL45_PRS3_MASK             2048U
#define ATD1CTL45_PRS4_MASK             4096U
#define ATD1CTL45_SMP0_MASK             8192U
#define ATD1CTL45_SMP1_MASK             16384U
#define ATD1CTL45_SRES8_MASK            32768U
#define ATD1CTL45_Cx_MASK               7U
#define ATD1CTL45_Cx_BITNUM             0U
#define ATD1CTL45_PRS_MASK              7936U
#define ATD1CTL45_PRS_BITNUM            8U
#define ATD1CTL45_SMP_MASK              24576U
#define ATD1CTL45_SMP_BITNUM            13U


/*** ATD1STAT0 - ATD 1 Status Register 0; 0x00000126 ***/
typedef union {
  byte Byte;
  struct {
    byte CC0         :1;                                       /* Conversion Counter 0 */
    byte CC1         :1;                                       /* Conversion Counter 1 */
    byte CC2         :1;                                       /* Conversion Counter 2 */
    byte             :1; 
    byte FIFOR       :1;                                       /* FIFO Over Run Flag */
    byte ETORF       :1;                                       /* External Trigger Overrun Flag */
    byte             :1; 
    byte SCF         :1;                                       /* Sequence Complete Flag */
  } Bits;
  struct {
    byte grpCC   :3;
    byte         :1;
    byte         :1;
    byte         :1;
    byte         :1;
    byte         :1;
  } MergedBits;
} ATD1STAT0STR;
extern volatile ATD1STAT0STR _ATD1STAT0 @(REG_BASE + 0x00000126UL);
#define ATD1STAT0                       _ATD1STAT0.Byte
#define ATD1STAT0_CC0                   _ATD1STAT0.Bits.CC0
#define ATD1STAT0_CC1                   _ATD1STAT0.Bits.CC1
#define ATD1STAT0_CC2                   _ATD1STAT0.Bits.CC2
#define ATD1STAT0_FIFOR                 _ATD1STAT0.Bits.FIFOR
#define ATD1STAT0_ETORF                 _ATD1STAT0.Bits.ETORF
#define ATD1STAT0_SCF                   _ATD1STAT0.Bits.SCF
#define ATD1STAT0_CC                    _ATD1STAT0.MergedBits.grpCC

#define ATD1STAT0_CC0_MASK              1U
#define ATD1STAT0_CC1_MASK              2U
#define ATD1STAT0_CC2_MASK              4U
#define ATD1STAT0_FIFOR_MASK            16U
#define ATD1STAT0_ETORF_MASK            32U
#define ATD1STAT0_SCF_MASK              128U
#define ATD1STAT0_CC_MASK               7U
#define ATD1STAT0_CC_BITNUM             0U


/*** ATD1TEST1 - ATD1 Test Register; 0x00000129 ***/
typedef union {
  byte Byte;
  struct {
    byte SC          :1;                                       /* Special Channel Conversion Bit */
    byte             :1; 
    byte             :1; 
    byte             :1; 
    byte             :1; 
    byte             :1; 
    byte             :1; 
    byte             :1; 
  } Bits;
} ATD1TEST1STR;
extern volatile ATD1TEST1STR _ATD1TEST1 @(REG_BASE + 0x00000129UL);
#define ATD1TEST1                       _ATD1TEST1.Byte
#define ATD1TEST1_SC                    _ATD1TEST1.Bits.SC

#define ATD1TEST1_SC_MASK               1U


/*** ATD1STAT1 - ATD 1 Status Register 1; 0x0000012B ***/
typedef union {
  byte Byte;
  struct {
    byte CCF0        :1;                                       /* Conversion Complete Flag 0 */
    byte CCF1        :1;                                       /* Conversion Complete Flag 1 */
    byte CCF2        :1;                                       /* Conversion Complete Flag 2 */
    byte CCF3        :1;                                       /* Conversion Complete Flag 3 */
    byte CCF4        :1;                                       /* Conversion Complete Flag 4 */
    byte CCF5        :1;                                       /* Conversion Complete Flag 5 */
    byte CCF6        :1;                                       /* Conversion Complete Flag 6 */
    byte CCF7        :1;                                       /* Conversion Complete Flag 7 */
  } Bits;
} ATD1STAT1STR;
extern volatile ATD1STAT1STR _ATD1STAT1 @(REG_BASE + 0x0000012BUL);
#define ATD1STAT1                       _ATD1STAT1.Byte
#define ATD1STAT1_CCF0                  _ATD1STAT1.Bits.CCF0
#define ATD1STAT1_CCF1                  _ATD1STAT1.Bits.CCF1
#define ATD1STAT1_CCF2                  _ATD1STAT1.Bits.CCF2
#define ATD1STAT1_CCF3                  _ATD1STAT1.Bits.CCF3
#define ATD1STAT1_CCF4                  _ATD1STAT1.Bits.CCF4
#define ATD1STAT1_CCF5                  _ATD1STAT1.Bits.CCF5
#define ATD1STAT1_CCF6                  _ATD1STAT1.Bits.CCF6
#define ATD1STAT1_CCF7                  _ATD1STAT1.Bits.CCF7

#define ATD1STAT1_CCF0_MASK             1U
#define ATD1STAT1_CCF1_MASK             2U
#define ATD1STAT1_CCF2_MASK             4U
#define ATD1STAT1_CCF3_MASK             8U
#define ATD1STAT1_CCF4_MASK             16U
#define ATD1STAT1_CCF5_MASK             32U
#define ATD1STAT1_CCF6_MASK             64U
#define ATD1STAT1_CCF7_MASK             128U


/*** ATD1DIEN - ATD 1 Input Enable Register; 0x0000012D ***/
typedef union {
  byte Byte;
  struct {
    byte IEN0        :1;                                       /* ATD Digital Input Enable on channel 0 */
    byte IEN1        :1;                                       /* ATD Digital Input Enable on channel 1 */
    byte IEN2        :1;                                       /* ATD Digital Input Enable on channel 2 */
    byte IEN3        :1;                                       /* ATD Digital Input Enable on channel 3 */
    byte IEN4        :1;                                       /* ATD Digital Input Enable on channel 4 */
    byte IEN5        :1;                                       /* ATD Digital Input Enable on channel 5 */
    byte IEN6        :1;                                       /* ATD Digital Input Enable on channel 6 */
    byte IEN7        :1;                                       /* ATD Digital Input Enable on channel 7 */
  } Bits;
} ATD1DIENSTR;
extern volatile ATD1DIENSTR _ATD1DIEN @(REG_BASE + 0x0000012DUL);
#define ATD1DIEN                        _ATD1DIEN.Byte
#define ATD1DIEN_IEN0                   _ATD1DIEN.Bits.IEN0
#define ATD1DIEN_IEN1                   _ATD1DIEN.Bits.IEN1
#define ATD1DIEN_IEN2                   _ATD1DIEN.Bits.IEN2
#define ATD1DIEN_IEN3                   _ATD1DIEN.Bits.IEN3
#define ATD1DIEN_IEN4                   _ATD1DIEN.Bits.IEN4
#define ATD1DIEN_IEN5                   _ATD1DIEN.Bits.IEN5
#define ATD1DIEN_IEN6                   _ATD1DIEN.Bits.IEN6
#define ATD1DIEN_IEN7                   _ATD1DIEN.Bits.IEN7

#define ATD1DIEN_IEN0_MASK              1U
#define ATD1DIEN_IEN1_MASK              2U
#define ATD1DIEN_IEN2_MASK              4U
#define ATD1DIEN_IEN3_MASK              8U
#define ATD1DIEN_IEN4_MASK              16U
#define ATD1DIEN_IEN5_MASK              32U
#define ATD1DIEN_IEN6_MASK              64U
#define ATD1DIEN_IEN7_MASK              128U


/*** PORTAD1 - Port AD1 Register; 0x0000012F ***/
typedef union {
  byte Byte;
  struct {
    byte PTAD0       :1;                                       /* A/D Channel 0 (AN0) Digital Input */
    byte PTAD1       :1;                                       /* A/D Channel 1 (AN1) Digital Input */
    byte PTAD2       :1;                                       /* A/D Channel 2 (AN2) Digital Input */
    byte PTAD3       :1;                                       /* A/D Channel 3 (AN3) Digital Input */
    byte PTAD4       :1;                                       /* A/D Channel 4 (AN4) Digital Input */
    byte PTAD5       :1;                                       /* A/D Channel 5 (AN5) Digital Input */
    byte PTAD6       :1;                                       /* A/D Channel 6 (AN6) Digital Input */
    byte PTAD7       :1;                                       /* A/D Channel 7 (AN7) Digital Input */
  } Bits;
} PORTAD1STR;
extern volatile PORTAD1STR _PORTAD1 @(REG_BASE + 0x0000012FUL);
#define PORTAD1                         _PORTAD1.Byte
#define PORTAD1_PTAD0                   _PORTAD1.Bits.PTAD0
#define PORTAD1_PTAD1                   _PORTAD1.Bits.PTAD1
#define PORTAD1_PTAD2                   _PORTAD1.Bits.PTAD2
#define PORTAD1_PTAD3                   _PORTAD1.Bits.PTAD3
#define PORTAD1_PTAD4                   _PORTAD1.Bits.PTAD4
#define PORTAD1_PTAD5                   _PORTAD1.Bits.PTAD5
#define PORTAD1_PTAD6                   _PORTAD1.Bits.PTAD6
#define PORTAD1_PTAD7                   _PORTAD1.Bits.PTAD7

#define PORTAD1_PTAD0_MASK              1U
#define PORTAD1_PTAD1_MASK              2U
#define PORTAD1_PTAD2_MASK              4U
#define PORTAD1_PTAD3_MASK              8U
#define PORTAD1_PTAD4_MASK              16U
#define PORTAD1_PTAD5_MASK              32U
#define PORTAD1_PTAD6_MASK              64U
#define PORTAD1_PTAD7_MASK              128U


/*** ATD1DR0 - ATD 1 Conversion Result Register 0; 0x00000130 ***/
typedef union {
  word Word;
   /* Overlapped registers: */
  struct {
    /*** ATD1DR0H - ATD 1 Conversion Result Register 0 High; 0x00000130 ***/
    union {
      byte Byte;
      struct {
        byte BIT8        :1;                                       /* Bit 8 */
        byte BIT9        :1;                                       /* Bit 9 */
        byte BIT10       :1;                                       /* Bit 10 */
        byte BIT11       :1;                                       /* Bit 11 */
        byte BIT12       :1;                                       /* Bit 12 */
        byte BIT13       :1;                                       /* Bit 13 */
        byte BIT14       :1;                                       /* Bit 14 */
        byte BIT15       :1;                                       /* Bit 15 */
      } Bits;
    } ATD1DR0HSTR;
    #define ATD1DR0H                    _ATD1DR0.Overlap_STR.ATD1DR0HSTR.Byte
    #define ATD1DR0H_BIT8               _ATD1DR0.Overlap_STR.ATD1DR0HSTR.Bits.BIT8
    #define ATD1DR0H_BIT9               _ATD1DR0.Overlap_STR.ATD1DR0HSTR.Bits.BIT9
    #define ATD1DR0H_BIT10              _ATD1DR0.Overlap_STR.ATD1DR0HSTR.Bits.BIT10
    #define ATD1DR0H_BIT11              _ATD1DR0.Overlap_STR.ATD1DR0HSTR.Bits.BIT11
    #define ATD1DR0H_BIT12              _ATD1DR0.Overlap_STR.ATD1DR0HSTR.Bits.BIT12
    #define ATD1DR0H_BIT13              _ATD1DR0.Overlap_STR.ATD1DR0HSTR.Bits.BIT13
    #define ATD1DR0H_BIT14              _ATD1DR0.Overlap_STR.ATD1DR0HSTR.Bits.BIT14
    #define ATD1DR0H_BIT15              _ATD1DR0.Overlap_STR.ATD1DR0HSTR.Bits.BIT15
    
    #define ATD1DR0H_BIT8_MASK          1U
    #define ATD1DR0H_BIT9_MASK          2U
    #define ATD1DR0H_BIT10_MASK         4U
    #define ATD1DR0H_BIT11_MASK         8U
    #define ATD1DR0H_BIT12_MASK         16U
    #define ATD1DR0H_BIT13_MASK         32U
    #define ATD1DR0H_BIT14_MASK         64U
    #define ATD1DR0H_BIT15_MASK         128U
    

    /*** ATD1DR0L - ATD 1 Conversion Result Register 0 Low; 0x00000131 ***/
    union {
      byte Byte;
      struct {
        byte             :1; 
        byte             :1; 
        byte             :1; 
        byte             :1; 
        byte             :1; 
        byte             :1; 
        byte BIT6        :1;                                       /* Bit 6 */
        byte BIT7        :1;                                       /* Bit 7 */
      } Bits;
      struct {
        byte     :1;
        byte     :1;
        byte     :1;
        byte     :1;
        byte     :1;
        byte     :1;
        byte grpBIT_6 :2;
      } MergedBits;
    } ATD1DR0LSTR;
    #define ATD1DR0L                    _ATD1DR0.Overlap_STR.ATD1DR0LSTR.Byte
    #define ATD1DR0L_BIT6               _ATD1DR0.Overlap_STR.ATD1DR0LSTR.Bits.BIT6
    #define ATD1DR0L_BIT7               _ATD1DR0.Overlap_STR.ATD1DR0LSTR.Bits.BIT7
    #define ATD1DR0L_BIT_6              _ATD1DR0.Overlap_STR.ATD1DR0LSTR.MergedBits.grpBIT_6
    #define ATD1DR0L_BIT                ATD1DR0L_BIT_6
    
    #define ATD1DR0L_BIT6_MASK          64U
    #define ATD1DR0L_BIT7_MASK          128U
    #define ATD1DR0L_BIT_6_MASK         192U
    #define ATD1DR0L_BIT_6_BITNUM       6U
    
  } Overlap_STR;

  struct {
    word             :1; 
    word             :1; 
    word             :1; 
    word             :1; 
    word             :1; 
    word             :1; 
    word BIT6        :1;                                       /* Bit 6 */
    word BIT7        :1;                                       /* Bit 7 */
    word BIT8        :1;                                       /* Bit 8 */
    word BIT9        :1;                                       /* Bit 9 */
    word BIT10       :1;                                       /* Bit 10 */
    word BIT11       :1;                                       /* Bit 11 */
    word BIT12       :1;                                       /* Bit 12 */
    word BIT13       :1;                                       /* Bit 13 */
    word BIT14       :1;                                       /* Bit 14 */
    word BIT15       :1;                                       /* Bit 15 */
  } Bits;
  struct {
    word         :1;
    word         :1;
    word         :1;
    word         :1;
    word         :1;
    word         :1;
    word grpBIT_6 :10;
  } MergedBits;
} ATD1DR0STR;
extern volatile ATD1DR0STR _ATD1DR0 @(REG_BASE + 0x00000130UL);
#define ATD1DR0                         _ATD1DR0.Word
#define ATD1DR0_BIT6                    _ATD1DR0.Bits.BIT6
#define ATD1DR0_BIT7                    _ATD1DR0.Bits.BIT7
#define ATD1DR0_BIT8                    _ATD1DR0.Bits.BIT8
#define ATD1DR0_BIT9                    _ATD1DR0.Bits.BIT9
#define ATD1DR0_BIT10                   _ATD1DR0.Bits.BIT10
#define ATD1DR0_BIT11                   _ATD1DR0.Bits.BIT11
#define ATD1DR0_BIT12                   _ATD1DR0.Bits.BIT12
#define ATD1DR0_BIT13                   _ATD1DR0.Bits.BIT13
#define ATD1DR0_BIT14                   _ATD1DR0.Bits.BIT14
#define ATD1DR0_BIT15                   _ATD1DR0.Bits.BIT15
/* ATD1DR_ARR: Access 8 ATD1DRx registers in an array */
#define ATD1DR_ARR                      ((volatile word *) &ATD1DR0)
#define ATD1DR0_BIT_6                   _ATD1DR0.MergedBits.grpBIT_6
#define ATD1DR0_BIT                     ATD1DR0_BIT_6

#define ATD1DR0_BIT6_MASK               64U
#define ATD1DR0_BIT7_MASK               128U
#define ATD1DR0_BIT8_MASK               256U
#define ATD1DR0_BIT9_MASK               512U
#define ATD1DR0_BIT10_MASK              1024U
#define ATD1DR0_BIT11_MASK              2048U
#define ATD1DR0_BIT12_MASK              4096U
#define ATD1DR0_BIT13_MASK              8192U
#define ATD1DR0_BIT14_MASK              16384U
#define ATD1DR0_BIT15_MASK              32768U
#define ATD1DR0_BIT_6_MASK              65472U
#define ATD1DR0_BIT_6_BITNUM            6U


/*** ATD1DR1 - ATD 1 Conversion Result Register 1; 0x00000132 ***/
typedef union {
  word Word;
   /* Overlapped registers: */
  struct {
    /*** ATD1DR1H - ATD 1 Conversion Result Register 1 High; 0x00000132 ***/
    union {
      byte Byte;
      struct {
        byte BIT8        :1;                                       /* Bit 8 */
        byte BIT9        :1;                                       /* Bit 9 */
        byte BIT10       :1;                                       /* Bit 10 */
        byte BIT11       :1;                                       /* Bit 11 */
        byte BIT12       :1;                                       /* Bit 12 */
        byte BIT13       :1;                                       /* Bit 13 */
        byte BIT14       :1;                                       /* Bit 14 */
        byte BIT15       :1;                                       /* Bit 15 */
      } Bits;
    } ATD1DR1HSTR;
    #define ATD1DR1H                    _ATD1DR1.Overlap_STR.ATD1DR1HSTR.Byte
    #define ATD1DR1H_BIT8               _ATD1DR1.Overlap_STR.ATD1DR1HSTR.Bits.BIT8
    #define ATD1DR1H_BIT9               _ATD1DR1.Overlap_STR.ATD1DR1HSTR.Bits.BIT9
    #define ATD1DR1H_BIT10              _ATD1DR1.Overlap_STR.ATD1DR1HSTR.Bits.BIT10
    #define ATD1DR1H_BIT11              _ATD1DR1.Overlap_STR.ATD1DR1HSTR.Bits.BIT11
    #define ATD1DR1H_BIT12              _ATD1DR1.Overlap_STR.ATD1DR1HSTR.Bits.BIT12
    #define ATD1DR1H_BIT13              _ATD1DR1.Overlap_STR.ATD1DR1HSTR.Bits.BIT13
    #define ATD1DR1H_BIT14              _ATD1DR1.Overlap_STR.ATD1DR1HSTR.Bits.BIT14
    #define ATD1DR1H_BIT15              _ATD1DR1.Overlap_STR.ATD1DR1HSTR.Bits.BIT15
    
    #define ATD1DR1H_BIT8_MASK          1U
    #define ATD1DR1H_BIT9_MASK          2U
    #define ATD1DR1H_BIT10_MASK         4U
    #define ATD1DR1H_BIT11_MASK         8U
    #define ATD1DR1H_BIT12_MASK         16U
    #define ATD1DR1H_BIT13_MASK         32U
    #define ATD1DR1H_BIT14_MASK         64U
    #define ATD1DR1H_BIT15_MASK         128U
    

    /*** ATD1DR1L - ATD 1 Conversion Result Register 1 Low; 0x00000133 ***/
    union {
      byte Byte;
      struct {
        byte             :1; 
        byte             :1; 
        byte             :1; 
        byte             :1; 
        byte             :1; 
        byte             :1; 
        byte BIT6        :1;                                       /* Bit 6 */
        byte BIT7        :1;                                       /* Bit 7 */
      } Bits;
      struct {
        byte     :1;
        byte     :1;
        byte     :1;
        byte     :1;
        byte     :1;
        byte     :1;
        byte grpBIT_6 :2;
      } MergedBits;
    } ATD1DR1LSTR;
    #define ATD1DR1L                    _ATD1DR1.Overlap_STR.ATD1DR1LSTR.Byte
    #define ATD1DR1L_BIT6               _ATD1DR1.Overlap_STR.ATD1DR1LSTR.Bits.BIT6
    #define ATD1DR1L_BIT7               _ATD1DR1.Overlap_STR.ATD1DR1LSTR.Bits.BIT7
    #define ATD1DR1L_BIT_6              _ATD1DR1.Overlap_STR.ATD1DR1LSTR.MergedBits.grpBIT_6
    #define ATD1DR1L_BIT                ATD1DR1L_BIT_6
    
    #define ATD1DR1L_BIT6_MASK          64U
    #define ATD1DR1L_BIT7_MASK          128U
    #define ATD1DR1L_BIT_6_MASK         192U
    #define ATD1DR1L_BIT_6_BITNUM       6U
    
  } Overlap_STR;

  struct {
    word             :1; 
    word             :1; 
    word             :1; 
    word             :1; 
    word             :1; 
    word             :1; 
    word BIT6        :1;                                       /* Bit 6 */
    word BIT7        :1;                                       /* Bit 7 */
    word BIT8        :1;                                       /* Bit 8 */
    word BIT9        :1;                                       /* Bit 9 */
    word BIT10       :1;                                       /* Bit 10 */
    word BIT11       :1;                                       /* Bit 11 */
    word BIT12       :1;                                       /* Bit 12 */
    word BIT13       :1;                                       /* Bit 13 */
    word BIT14       :1;                                       /* Bit 14 */
    word BIT15       :1;                                       /* Bit 15 */
  } Bits;
  struct {
    word         :1;
    word         :1;
    word         :1;
    word         :1;
    word         :1;
    word         :1;
    word grpBIT_6 :10;
  } MergedBits;
} ATD1DR1STR;
extern volatile ATD1DR1STR _ATD1DR1 @(REG_BASE + 0x00000132UL);
#define ATD1DR1                         _ATD1DR1.Word
#define ATD1DR1_BIT6                    _ATD1DR1.Bits.BIT6
#define ATD1DR1_BIT7                    _ATD1DR1.Bits.BIT7
#define ATD1DR1_BIT8                    _ATD1DR1.Bits.BIT8
#define ATD1DR1_BIT9                    _ATD1DR1.Bits.BIT9
#define ATD1DR1_BIT10                   _ATD1DR1.Bits.BIT10
#define ATD1DR1_BIT11                   _ATD1DR1.Bits.BIT11
#define ATD1DR1_BIT12                   _ATD1DR1.Bits.BIT12
#define ATD1DR1_BIT13                   _ATD1DR1.Bits.BIT13
#define ATD1DR1_BIT14                   _ATD1DR1.Bits.BIT14
#define ATD1DR1_BIT15                   _ATD1DR1.Bits.BIT15
#define ATD1DR1_BIT_6                   _ATD1DR1.MergedBits.grpBIT_6
#define ATD1DR1_BIT                     ATD1DR1_BIT_6

#define ATD1DR1_BIT6_MASK               64U
#define ATD1DR1_BIT7_MASK               128U
#define ATD1DR1_BIT8_MASK               256U
#define ATD1DR1_BIT9_MASK               512U
#define ATD1DR1_BIT10_MASK              1024U
#define ATD1DR1_BIT11_MASK              2048U
#define ATD1DR1_BIT12_MASK              4096U
#define ATD1DR1_BIT13_MASK              8192U
#define ATD1DR1_BIT14_MASK              16384U
#define ATD1DR1_BIT15_MASK              32768U
#define ATD1DR1_BIT_6_MASK              65472U
#define ATD1DR1_BIT_6_BITNUM            6U


/*** ATD1DR2 - ATD 1 Conversion Result Register 2; 0x00000134 ***/
typedef union {
  word Word;
   /* Overlapped registers: */
  struct {
    /*** ATD1DR2H - ATD 1 Conversion Result Register 2 High; 0x00000134 ***/
    union {
      byte Byte;
      struct {
        byte BIT8        :1;                                       /* Bit 8 */
        byte BIT9        :1;                                       /* Bit 9 */
        byte BIT10       :1;                                       /* Bit 10 */
        byte BIT11       :1;                                       /* Bit 11 */
        byte BIT12       :1;                                       /* Bit 12 */
        byte BIT13       :1;                                       /* Bit 13 */
        byte BIT14       :1;                                       /* Bit 14 */
        byte BIT15       :1;                                       /* Bit 15 */
      } Bits;
    } ATD1DR2HSTR;
    #define ATD1DR2H                    _ATD1DR2.Overlap_STR.ATD1DR2HSTR.Byte
    #define ATD1DR2H_BIT8               _ATD1DR2.Overlap_STR.ATD1DR2HSTR.Bits.BIT8
    #define ATD1DR2H_BIT9               _ATD1DR2.Overlap_STR.ATD1DR2HSTR.Bits.BIT9
    #define ATD1DR2H_BIT10              _ATD1DR2.Overlap_STR.ATD1DR2HSTR.Bits.BIT10
    #define ATD1DR2H_BIT11              _ATD1DR2.Overlap_STR.ATD1DR2HSTR.Bits.BIT11
    #define ATD1DR2H_BIT12              _ATD1DR2.Overlap_STR.ATD1DR2HSTR.Bits.BIT12
    #define ATD1DR2H_BIT13              _ATD1DR2.Overlap_STR.ATD1DR2HSTR.Bits.BIT13
    #define ATD1DR2H_BIT14              _ATD1DR2.Overlap_STR.ATD1DR2HSTR.Bits.BIT14
    #define ATD1DR2H_BIT15              _ATD1DR2.Overlap_STR.ATD1DR2HSTR.Bits.BIT15
    
    #define ATD1DR2H_BIT8_MASK          1U
    #define ATD1DR2H_BIT9_MASK          2U
    #define ATD1DR2H_BIT10_MASK         4U
    #define ATD1DR2H_BIT11_MASK         8U
    #define ATD1DR2H_BIT12_MASK         16U
    #define ATD1DR2H_BIT13_MASK         32U
    #define ATD1DR2H_BIT14_MASK         64U
    #define ATD1DR2H_BIT15_MASK         128U
    

    /*** ATD1DR2L - ATD 1 Conversion Result Register 2 Low; 0x00000135 ***/
    union {
      byte Byte;
      struct {
        byte             :1; 
        byte             :1; 
        byte             :1; 
        byte             :1; 
        byte             :1; 
        byte             :1; 
        byte BIT6        :1;                                       /* Bit 6 */
        byte BIT7        :1;                                       /* Bit 7 */
      } Bits;
      struct {
        byte     :1;
        byte     :1;
        byte     :1;
        byte     :1;
        byte     :1;
        byte     :1;
        byte grpBIT_6 :2;
      } MergedBits;
    } ATD1DR2LSTR;
    #define ATD1DR2L                    _ATD1DR2.Overlap_STR.ATD1DR2LSTR.Byte
    #define ATD1DR2L_BIT6               _ATD1DR2.Overlap_STR.ATD1DR2LSTR.Bits.BIT6
    #define ATD1DR2L_BIT7               _ATD1DR2.Overlap_STR.ATD1DR2LSTR.Bits.BIT7
    #define ATD1DR2L_BIT_6              _ATD1DR2.Overlap_STR.ATD1DR2LSTR.MergedBits.grpBIT_6
    #define ATD1DR2L_BIT                ATD1DR2L_BIT_6
    
    #define ATD1DR2L_BIT6_MASK          64U
    #define ATD1DR2L_BIT7_MASK          128U
    #define ATD1DR2L_BIT_6_MASK         192U
    #define ATD1DR2L_BIT_6_BITNUM       6U
    
  } Overlap_STR;

  struct {
    word             :1; 
    word             :1; 
    word             :1; 
    word             :1; 
    word             :1; 
    word             :1; 
    word BIT6        :1;                                       /* Bit 6 */
    word BIT7        :1;                                       /* Bit 7 */
    word BIT8        :1;                                       /* Bit 8 */
    word BIT9        :1;                                       /* Bit 9 */
    word BIT10       :1;                                       /* Bit 10 */
    word BIT11       :1;                                       /* Bit 11 */
    word BIT12       :1;                                       /* Bit 12 */
    word BIT13       :1;                                       /* Bit 13 */
    word BIT14       :1;                                       /* Bit 14 */
    word BIT15       :1;                                       /* Bit 15 */
  } Bits;
  struct {
    word         :1;
    word         :1;
    word         :1;
    word         :1;
    word         :1;
    word         :1;
    word grpBIT_6 :10;
  } MergedBits;
} ATD1DR2STR;
extern volatile ATD1DR2STR _ATD1DR2 @(REG_BASE + 0x00000134UL);
#define ATD1DR2                         _ATD1DR2.Word
#define ATD1DR2_BIT6                    _ATD1DR2.Bits.BIT6
#define ATD1DR2_BIT7                    _ATD1DR2.Bits.BIT7
#define ATD1DR2_BIT8                    _ATD1DR2.Bits.BIT8
#define ATD1DR2_BIT9                    _ATD1DR2.Bits.BIT9
#define ATD1DR2_BIT10                   _ATD1DR2.Bits.BIT10
#define ATD1DR2_BIT11                   _ATD1DR2.Bits.BIT11
#define ATD1DR2_BIT12                   _ATD1DR2.Bits.BIT12
#define ATD1DR2_BIT13                   _ATD1DR2.Bits.BIT13
#define ATD1DR2_BIT14                   _ATD1DR2.Bits.BIT14
#define ATD1DR2_BIT15                   _ATD1DR2.Bits.BIT15
#define ATD1DR2_BIT_6                   _ATD1DR2.MergedBits.grpBIT_6
#define ATD1DR2_BIT                     ATD1DR2_BIT_6

#define ATD1DR2_BIT6_MASK               64U
#define ATD1DR2_BIT7_MASK               128U
#define ATD1DR2_BIT8_MASK               256U
#define ATD1DR2_BIT9_MASK               512U
#define ATD1DR2_BIT10_MASK              1024U
#define ATD1DR2_BIT11_MASK              2048U
#define ATD1DR2_BIT12_MASK              4096U
#define ATD1DR2_BIT13_MASK              8192U
#define ATD1DR2_BIT14_MASK              16384U
#define ATD1DR2_BIT15_MASK              32768U
#define ATD1DR2_BIT_6_MASK              65472U
#define ATD1DR2_BIT_6_BITNUM            6U


/*** ATD1DR3 - ATD 1 Conversion Result Register 3; 0x00000136 ***/
typedef union {
  word Word;
   /* Overlapped registers: */
  struct {
    /*** ATD1DR3H - ATD 1 Conversion Result Register 3 High; 0x00000136 ***/
    union {
      byte Byte;
      struct {
        byte BIT8        :1;                                       /* Bit 8 */
        byte BIT9        :1;                                       /* Bit 9 */
        byte BIT10       :1;                                       /* Bit 10 */
        byte BIT11       :1;                                       /* Bit 11 */
        byte BIT12       :1;                                       /* Bit 12 */
        byte BIT13       :1;                                       /* Bit 13 */
        byte BIT14       :1;                                       /* Bit 14 */
        byte BIT15       :1;                                       /* Bit 15 */
      } Bits;
    } ATD1DR3HSTR;
    #define ATD1DR3H                    _ATD1DR3.Overlap_STR.ATD1DR3HSTR.Byte
    #define ATD1DR3H_BIT8               _ATD1DR3.Overlap_STR.ATD1DR3HSTR.Bits.BIT8
    #define ATD1DR3H_BIT9               _ATD1DR3.Overlap_STR.ATD1DR3HSTR.Bits.BIT9
    #define ATD1DR3H_BIT10              _ATD1DR3.Overlap_STR.ATD1DR3HSTR.Bits.BIT10
    #define ATD1DR3H_BIT11              _ATD1DR3.Overlap_STR.ATD1DR3HSTR.Bits.BIT11
    #define ATD1DR3H_BIT12              _ATD1DR3.Overlap_STR.ATD1DR3HSTR.Bits.BIT12
    #define ATD1DR3H_BIT13              _ATD1DR3.Overlap_STR.ATD1DR3HSTR.Bits.BIT13
    #define ATD1DR3H_BIT14              _ATD1DR3.Overlap_STR.ATD1DR3HSTR.Bits.BIT14
    #define ATD1DR3H_BIT15              _ATD1DR3.Overlap_STR.ATD1DR3HSTR.Bits.BIT15
    
    #define ATD1DR3H_BIT8_MASK          1U
    #define ATD1DR3H_BIT9_MASK          2U
    #define ATD1DR3H_BIT10_MASK         4U
    #define ATD1DR3H_BIT11_MASK         8U
    #define ATD1DR3H_BIT12_MASK         16U
    #define ATD1DR3H_BIT13_MASK         32U
    #define ATD1DR3H_BIT14_MASK         64U
    #define ATD1DR3H_BIT15_MASK         128U
    

    /*** ATD1DR3L - ATD 1 Conversion Result Register 3 Low; 0x00000137 ***/
    union {
      byte Byte;
      struct {
        byte             :1; 
        byte             :1; 
        byte             :1; 
        byte             :1; 
        byte             :1; 
        byte             :1; 
        byte BIT6        :1;                                       /* Bit 6 */
        byte BIT7        :1;                                       /* Bit 7 */
      } Bits;
      struct {
        byte     :1;
        byte     :1;
        byte     :1;
        byte     :1;
        byte     :1;
        byte     :1;
        byte grpBIT_6 :2;
      } MergedBits;
    } ATD1DR3LSTR;
    #define ATD1DR3L                    _ATD1DR3.Overlap_STR.ATD1DR3LSTR.Byte
    #define ATD1DR3L_BIT6               _ATD1DR3.Overlap_STR.ATD1DR3LSTR.Bits.BIT6
    #define ATD1DR3L_BIT7               _ATD1DR3.Overlap_STR.ATD1DR3LSTR.Bits.BIT7
    #define ATD1DR3L_BIT_6              _ATD1DR3.Overlap_STR.ATD1DR3LSTR.MergedBits.grpBIT_6
    #define ATD1DR3L_BIT                ATD1DR3L_BIT_6
    
    #define ATD1DR3L_BIT6_MASK          64U
    #define ATD1DR3L_BIT7_MASK          128U
    #define ATD1DR3L_BIT_6_MASK         192U
    #define ATD1DR3L_BIT_6_BITNUM       6U
    
  } Overlap_STR;

  struct {
    word             :1; 
    word             :1; 
    word             :1; 
    word             :1; 
    word             :1; 
    word             :1; 
    word BIT6        :1;                                       /* Bit 6 */
    word BIT7        :1;                                       /* Bit 7 */
    word BIT8        :1;                                       /* Bit 8 */
    word BIT9        :1;                                       /* Bit 9 */
    word BIT10       :1;                                       /* Bit 10 */
    word BIT11       :1;                                       /* Bit 11 */
    word BIT12       :1;                                       /* Bit 12 */
    word BIT13       :1;                                       /* Bit 13 */
    word BIT14       :1;                                       /* Bit 14 */
    word BIT15       :1;                                       /* Bit 15 */
  } Bits;
  struct {
    word         :1;
    word         :1;
    word         :1;
    word         :1;
    word         :1;
    word         :1;
    word grpBIT_6 :10;
  } MergedBits;
} ATD1DR3STR;
extern volatile ATD1DR3STR _ATD1DR3 @(REG_BASE + 0x00000136UL);
#define ATD1DR3                         _ATD1DR3.Word
#define ATD1DR3_BIT6                    _ATD1DR3.Bits.BIT6
#define ATD1DR3_BIT7                    _ATD1DR3.Bits.BIT7
#define ATD1DR3_BIT8                    _ATD1DR3.Bits.BIT8
#define ATD1DR3_BIT9                    _ATD1DR3.Bits.BIT9
#define ATD1DR3_BIT10                   _ATD1DR3.Bits.BIT10
#define ATD1DR3_BIT11                   _ATD1DR3.Bits.BIT11
#define ATD1DR3_BIT12                   _ATD1DR3.Bits.BIT12
#define ATD1DR3_BIT13                   _ATD1DR3.Bits.BIT13
#define ATD1DR3_BIT14                   _ATD1DR3.Bits.BIT14
#define ATD1DR3_BIT15                   _ATD1DR3.Bits.BIT15
#define ATD1DR3_BIT_6                   _ATD1DR3.MergedBits.grpBIT_6
#define ATD1DR3_BIT                     ATD1DR3_BIT_6

#define ATD1DR3_BIT6_MASK               64U
#define ATD1DR3_BIT7_MASK               128U
#define ATD1DR3_BIT8_MASK               256U
#define ATD1DR3_BIT9_MASK               512U
#define ATD1DR3_BIT10_MASK              1024U
#define ATD1DR3_BIT11_MASK              2048U
#define ATD1DR3_BIT12_MASK              4096U
#define ATD1DR3_BIT13_MASK              8192U
#define ATD1DR3_BIT14_MASK              16384U
#define ATD1DR3_BIT15_MASK              32768U
#define ATD1DR3_BIT_6_MASK              65472U
#define ATD1DR3_BIT_6_BITNUM            6U


/*** ATD1DR4 - ATD 1 Conversion Result Register 4; 0x00000138 ***/
typedef union {
  word Word;
   /* Overlapped registers: */
  struct {
    /*** ATD1DR4H - ATD 1 Conversion Result Register 4 High; 0x00000138 ***/
    union {
      byte Byte;
      struct {
        byte BIT8        :1;                                       /* Bit 8 */
        byte BIT9        :1;                                       /* Bit 9 */
        byte BIT10       :1;                                       /* Bit 10 */
        byte BIT11       :1;                                       /* Bit 11 */
        byte BIT12       :1;                                       /* Bit 12 */
        byte BIT13       :1;                                       /* Bit 13 */
        byte BIT14       :1;                                       /* Bit 14 */
        byte BIT15       :1;                                       /* Bit 15 */
      } Bits;
    } ATD1DR4HSTR;
    #define ATD1DR4H                    _ATD1DR4.Overlap_STR.ATD1DR4HSTR.Byte
    #define ATD1DR4H_BIT8               _ATD1DR4.Overlap_STR.ATD1DR4HSTR.Bits.BIT8
    #define ATD1DR4H_BIT9               _ATD1DR4.Overlap_STR.ATD1DR4HSTR.Bits.BIT9
    #define ATD1DR4H_BIT10              _ATD1DR4.Overlap_STR.ATD1DR4HSTR.Bits.BIT10
    #define ATD1DR4H_BIT11              _ATD1DR4.Overlap_STR.ATD1DR4HSTR.Bits.BIT11
    #define ATD1DR4H_BIT12              _ATD1DR4.Overlap_STR.ATD1DR4HSTR.Bits.BIT12
    #define ATD1DR4H_BIT13              _ATD1DR4.Overlap_STR.ATD1DR4HSTR.Bits.BIT13
    #define ATD1DR4H_BIT14              _ATD1DR4.Overlap_STR.ATD1DR4HSTR.Bits.BIT14
    #define ATD1DR4H_BIT15              _ATD1DR4.Overlap_STR.ATD1DR4HSTR.Bits.BIT15
    
    #define ATD1DR4H_BIT8_MASK          1U
    #define ATD1DR4H_BIT9_MASK          2U
    #define ATD1DR4H_BIT10_MASK         4U
    #define ATD1DR4H_BIT11_MASK         8U
    #define ATD1DR4H_BIT12_MASK         16U
    #define ATD1DR4H_BIT13_MASK         32U
    #define ATD1DR4H_BIT14_MASK         64U
    #define ATD1DR4H_BIT15_MASK         128U
    

    /*** ATD1DR4L - ATD 1 Conversion Result Register 4 Low; 0x00000139 ***/
    union {
      byte Byte;
      struct {
        byte             :1; 
        byte             :1; 
        byte             :1; 
        byte             :1; 
        byte             :1; 
        byte             :1; 
        byte BIT6        :1;                                       /* Bit 6 */
        byte BIT7        :1;                                       /* Bit 7 */
      } Bits;
      struct {
        byte     :1;
        byte     :1;
        byte     :1;
        byte     :1;
        byte     :1;
        byte     :1;
        byte grpBIT_6 :2;
      } MergedBits;
    } ATD1DR4LSTR;
    #define ATD1DR4L                    _ATD1DR4.Overlap_STR.ATD1DR4LSTR.Byte
    #define ATD1DR4L_BIT6               _ATD1DR4.Overlap_STR.ATD1DR4LSTR.Bits.BIT6
    #define ATD1DR4L_BIT7               _ATD1DR4.Overlap_STR.ATD1DR4LSTR.Bits.BIT7
    #define ATD1DR4L_BIT_6              _ATD1DR4.Overlap_STR.ATD1DR4LSTR.MergedBits.grpBIT_6
    #define ATD1DR4L_BIT                ATD1DR4L_BIT_6
    
    #define ATD1DR4L_BIT6_MASK          64U
    #define ATD1DR4L_BIT7_MASK          128U
    #define ATD1DR4L_BIT_6_MASK         192U
    #define ATD1DR4L_BIT_6_BITNUM       6U
    
  } Overlap_STR;

  struct {
    word             :1; 
    word             :1; 
    word             :1; 
    word             :1; 
    word             :1; 
    word             :1; 
    word BIT6        :1;                                       /* Bit 6 */
    word BIT7        :1;                                       /* Bit 7 */
    word BIT8        :1;                                       /* Bit 8 */
    word BIT9        :1;                                       /* Bit 9 */
    word BIT10       :1;                                       /* Bit 10 */
    word BIT11       :1;                                       /* Bit 11 */
    word BIT12       :1;                                       /* Bit 12 */
    word BIT13       :1;                                       /* Bit 13 */
    word BIT14       :1;                                       /* Bit 14 */
    word BIT15       :1;                                       /* Bit 15 */
  } Bits;
  struct {
    word         :1;
    word         :1;
    word         :1;
    word         :1;
    word         :1;
    word         :1;
    word grpBIT_6 :10;
  } MergedBits;
} ATD1DR4STR;
extern volatile ATD1DR4STR _ATD1DR4 @(REG_BASE + 0x00000138UL);
#define ATD1DR4                         _ATD1DR4.Word
#define ATD1DR4_BIT6                    _ATD1DR4.Bits.BIT6
#define ATD1DR4_BIT7                    _ATD1DR4.Bits.BIT7
#define ATD1DR4_BIT8                    _ATD1DR4.Bits.BIT8
#define ATD1DR4_BIT9                    _ATD1DR4.Bits.BIT9
#define ATD1DR4_BIT10                   _ATD1DR4.Bits.BIT10
#define ATD1DR4_BIT11                   _ATD1DR4.Bits.BIT11
#define ATD1DR4_BIT12                   _ATD1DR4.Bits.BIT12
#define ATD1DR4_BIT13                   _ATD1DR4.Bits.BIT13
#define ATD1DR4_BIT14                   _ATD1DR4.Bits.BIT14
#define ATD1DR4_BIT15                   _ATD1DR4.Bits.BIT15
#define ATD1DR4_BIT_6                   _ATD1DR4.MergedBits.grpBIT_6
#define ATD1DR4_BIT                     ATD1DR4_BIT_6

#define ATD1DR4_BIT6_MASK               64U
#define ATD1DR4_BIT7_MASK               128U
#define ATD1DR4_BIT8_MASK               256U
#define ATD1DR4_BIT9_MASK               512U
#define ATD1DR4_BIT10_MASK              1024U
#define ATD1DR4_BIT11_MASK              2048U
#define ATD1DR4_BIT12_MASK              4096U
#define ATD1DR4_BIT13_MASK              8192U
#define ATD1DR4_BIT14_MASK              16384U
#define ATD1DR4_BIT15_MASK              32768U
#define ATD1DR4_BIT_6_MASK              65472U
#define ATD1DR4_BIT_6_BITNUM            6U


/*** ATD1DR5 - ATD 1 Conversion Result Register 5; 0x0000013A ***/
typedef union {
  word Word;
   /* Overlapped registers: */
  struct {
    /*** ATD1DR5H - ATD 1 Conversion Result Register 5 High; 0x0000013A ***/
    union {
      byte Byte;
      struct {
        byte BIT8        :1;                                       /* Bit 8 */
        byte BIT9        :1;                                       /* Bit 9 */
        byte BIT10       :1;                                       /* Bit 10 */
        byte BIT11       :1;                                       /* Bit 11 */
        byte BIT12       :1;                                       /* Bit 12 */
        byte BIT13       :1;                                       /* Bit 13 */
        byte BIT14       :1;                                       /* Bit 14 */
        byte BIT15       :1;                                       /* Bit 15 */
      } Bits;
    } ATD1DR5HSTR;
    #define ATD1DR5H                    _ATD1DR5.Overlap_STR.ATD1DR5HSTR.Byte
    #define ATD1DR5H_BIT8               _ATD1DR5.Overlap_STR.ATD1DR5HSTR.Bits.BIT8
    #define ATD1DR5H_BIT9               _ATD1DR5.Overlap_STR.ATD1DR5HSTR.Bits.BIT9
    #define ATD1DR5H_BIT10              _ATD1DR5.Overlap_STR.ATD1DR5HSTR.Bits.BIT10
    #define ATD1DR5H_BIT11              _ATD1DR5.Overlap_STR.ATD1DR5HSTR.Bits.BIT11
    #define ATD1DR5H_BIT12              _ATD1DR5.Overlap_STR.ATD1DR5HSTR.Bits.BIT12
    #define ATD1DR5H_BIT13              _ATD1DR5.Overlap_STR.ATD1DR5HSTR.Bits.BIT13
    #define ATD1DR5H_BIT14              _ATD1DR5.Overlap_STR.ATD1DR5HSTR.Bits.BIT14
    #define ATD1DR5H_BIT15              _ATD1DR5.Overlap_STR.ATD1DR5HSTR.Bits.BIT15
    
    #define ATD1DR5H_BIT8_MASK          1U
    #define ATD1DR5H_BIT9_MASK          2U
    #define ATD1DR5H_BIT10_MASK         4U
    #define ATD1DR5H_BIT11_MASK         8U
    #define ATD1DR5H_BIT12_MASK         16U
    #define ATD1DR5H_BIT13_MASK         32U
    #define ATD1DR5H_BIT14_MASK         64U
    #define ATD1DR5H_BIT15_MASK         128U
    

    /*** ATD1DR5L - ATD 1 Conversion Result Register 5 Low; 0x0000013B ***/
    union {
      byte Byte;
      struct {
        byte             :1; 
        byte             :1; 
        byte             :1; 
        byte             :1; 
        byte             :1; 
        byte             :1; 
        byte BIT6        :1;                                       /* Bit 6 */
        byte BIT7        :1;                                       /* Bit 7 */
      } Bits;
      struct {
        byte     :1;
        byte     :1;
        byte     :1;
        byte     :1;
        byte     :1;
        byte     :1;
        byte grpBIT_6 :2;
      } MergedBits;
    } ATD1DR5LSTR;
    #define ATD1DR5L                    _ATD1DR5.Overlap_STR.ATD1DR5LSTR.Byte
    #define ATD1DR5L_BIT6               _ATD1DR5.Overlap_STR.ATD1DR5LSTR.Bits.BIT6
    #define ATD1DR5L_BIT7               _ATD1DR5.Overlap_STR.ATD1DR5LSTR.Bits.BIT7
    #define ATD1DR5L_BIT_6              _ATD1DR5.Overlap_STR.ATD1DR5LSTR.MergedBits.grpBIT_6
    #define ATD1DR5L_BIT                ATD1DR5L_BIT_6
    
    #define ATD1DR5L_BIT6_MASK          64U
    #define ATD1DR5L_BIT7_MASK          128U
    #define ATD1DR5L_BIT_6_MASK         192U
    #define ATD1DR5L_BIT_6_BITNUM       6U
    
  } Overlap_STR;

  struct {
    word             :1; 
    word             :1; 
    word             :1; 
    word             :1; 
    word             :1; 
    word             :1; 
    word BIT6        :1;                                       /* Bit 6 */
    word BIT7        :1;                                       /* Bit 7 */
    word BIT8        :1;                                       /* Bit 8 */
    word BIT9        :1;                                       /* Bit 9 */
    word BIT10       :1;                                       /* Bit 10 */
    word BIT11       :1;                                       /* Bit 11 */
    word BIT12       :1;                                       /* Bit 12 */
    word BIT13       :1;                                       /* Bit 13 */
    word BIT14       :1;                                       /* Bit 14 */
    word BIT15       :1;                                       /* Bit 15 */
  } Bits;
  struct {
    word         :1;
    word         :1;
    word         :1;
    word         :1;
    word         :1;
    word         :1;
    word grpBIT_6 :10;
  } MergedBits;
} ATD1DR5STR;
extern volatile ATD1DR5STR _ATD1DR5 @(REG_BASE + 0x0000013AUL);
#define ATD1DR5                         _ATD1DR5.Word
#define ATD1DR5_BIT6                    _ATD1DR5.Bits.BIT6
#define ATD1DR5_BIT7                    _ATD1DR5.Bits.BIT7
#define ATD1DR5_BIT8                    _ATD1DR5.Bits.BIT8
#define ATD1DR5_BIT9                    _ATD1DR5.Bits.BIT9
#define ATD1DR5_BIT10                   _ATD1DR5.Bits.BIT10
#define ATD1DR5_BIT11                   _ATD1DR5.Bits.BIT11
#define ATD1DR5_BIT12                   _ATD1DR5.Bits.BIT12
#define ATD1DR5_BIT13                   _ATD1DR5.Bits.BIT13
#define ATD1DR5_BIT14                   _ATD1DR5.Bits.BIT14
#define ATD1DR5_BIT15                   _ATD1DR5.Bits.BIT15
#define ATD1DR5_BIT_6                   _ATD1DR5.MergedBits.grpBIT_6
#define ATD1DR5_BIT                     ATD1DR5_BIT_6

#define ATD1DR5_BIT6_MASK               64U
#define ATD1DR5_BIT7_MASK               128U
#define ATD1DR5_BIT8_MASK               256U
#define ATD1DR5_BIT9_MASK               512U
#define ATD1DR5_BIT10_MASK              1024U
#define ATD1DR5_BIT11_MASK              2048U
#define ATD1DR5_BIT12_MASK              4096U
#define ATD1DR5_BIT13_MASK              8192U
#define ATD1DR5_BIT14_MASK              16384U
#define ATD1DR5_BIT15_MASK              32768U
#define ATD1DR5_BIT_6_MASK              65472U
#define ATD1DR5_BIT_6_BITNUM            6U


/*** ATD1DR6 - ATD 1 Conversion Result Register 6; 0x0000013C ***/
typedef union {
  word Word;
   /* Overlapped registers: */
  struct {
    /*** ATD1DR6H - ATD 1 Conversion Result Register 6 High; 0x0000013C ***/
    union {
      byte Byte;
      struct {
        byte BIT8        :1;                                       /* Bit 8 */
        byte BIT9        :1;                                       /* Bit 9 */
        byte BIT10       :1;                                       /* Bit 10 */
        byte BIT11       :1;                                       /* Bit 11 */
        byte BIT12       :1;                                       /* Bit 12 */
        byte BIT13       :1;                                       /* Bit 13 */
        byte BIT14       :1;                                       /* Bit 14 */
        byte BIT15       :1;                                       /* Bit 15 */
      } Bits;
    } ATD1DR6HSTR;
    #define ATD1DR6H                    _ATD1DR6.Overlap_STR.ATD1DR6HSTR.Byte
    #define ATD1DR6H_BIT8               _ATD1DR6.Overlap_STR.ATD1DR6HSTR.Bits.BIT8
    #define ATD1DR6H_BIT9               _ATD1DR6.Overlap_STR.ATD1DR6HSTR.Bits.BIT9
    #define ATD1DR6H_BIT10              _ATD1DR6.Overlap_STR.ATD1DR6HSTR.Bits.BIT10
    #define ATD1DR6H_BIT11              _ATD1DR6.Overlap_STR.ATD1DR6HSTR.Bits.BIT11
    #define ATD1DR6H_BIT12              _ATD1DR6.Overlap_STR.ATD1DR6HSTR.Bits.BIT12
    #define ATD1DR6H_BIT13              _ATD1DR6.Overlap_STR.ATD1DR6HSTR.Bits.BIT13
    #define ATD1DR6H_BIT14              _ATD1DR6.Overlap_STR.ATD1DR6HSTR.Bits.BIT14
    #define ATD1DR6H_BIT15              _ATD1DR6.Overlap_STR.ATD1DR6HSTR.Bits.BIT15
    
    #define ATD1DR6H_BIT8_MASK          1U
    #define ATD1DR6H_BIT9_MASK          2U
    #define ATD1DR6H_BIT10_MASK         4U
    #define ATD1DR6H_BIT11_MASK         8U
    #define ATD1DR6H_BIT12_MASK         16U
    #define ATD1DR6H_BIT13_MASK         32U
    #define ATD1DR6H_BIT14_MASK         64U
    #define ATD1DR6H_BIT15_MASK         128U
    

    /*** ATD1DR6L - ATD 1 Conversion Result Register 6 Low; 0x0000013D ***/
    union {
      byte Byte;
      struct {
        byte             :1; 
        byte             :1; 
        byte             :1; 
        byte             :1; 
        byte             :1; 
        byte             :1; 
        byte BIT6        :1;                                       /* Bit 6 */
        byte BIT7        :1;                                       /* Bit 7 */
      } Bits;
      struct {
        byte     :1;
        byte     :1;
        byte     :1;
        byte     :1;
        byte     :1;
        byte     :1;
        byte grpBIT_6 :2;
      } MergedBits;
    } ATD1DR6LSTR;
    #define ATD1DR6L                    _ATD1DR6.Overlap_STR.ATD1DR6LSTR.Byte
    #define ATD1DR6L_BIT6               _ATD1DR6.Overlap_STR.ATD1DR6LSTR.Bits.BIT6
    #define ATD1DR6L_BIT7               _ATD1DR6.Overlap_STR.ATD1DR6LSTR.Bits.BIT7
    #define ATD1DR6L_BIT_6              _ATD1DR6.Overlap_STR.ATD1DR6LSTR.MergedBits.grpBIT_6
    #define ATD1DR6L_BIT                ATD1DR6L_BIT_6
    
    #define ATD1DR6L_BIT6_MASK          64U
    #define ATD1DR6L_BIT7_MASK          128U
    #define ATD1DR6L_BIT_6_MASK         192U
    #define ATD1DR6L_BIT_6_BITNUM       6U
    
  } Overlap_STR;

  struct {
    word             :1; 
    word             :1; 
    word             :1; 
    word             :1; 
    word             :1; 
    word             :1; 
    word BIT6        :1;                                       /* Bit 6 */
    word BIT7        :1;                                       /* Bit 7 */
    word BIT8        :1;                                       /* Bit 8 */
    word BIT9        :1;                                       /* Bit 9 */
    word BIT10       :1;                                       /* Bit 10 */
    word BIT11       :1;                                       /* Bit 11 */
    word BIT12       :1;                                       /* Bit 12 */
    word BIT13       :1;                                       /* Bit 13 */
    word BIT14       :1;                                       /* Bit 14 */
    word BIT15       :1;                                       /* Bit 15 */
  } Bits;
  struct {
    word         :1;
    word         :1;
    word         :1;
    word         :1;
    word         :1;
    word         :1;
    word grpBIT_6 :10;
  } MergedBits;
} ATD1DR6STR;
extern volatile ATD1DR6STR _ATD1DR6 @(REG_BASE + 0x0000013CUL);
#define ATD1DR6                         _ATD1DR6.Word
#define ATD1DR6_BIT6                    _ATD1DR6.Bits.BIT6
#define ATD1DR6_BIT7                    _ATD1DR6.Bits.BIT7
#define ATD1DR6_BIT8                    _ATD1DR6.Bits.BIT8
#define ATD1DR6_BIT9                    _ATD1DR6.Bits.BIT9
#define ATD1DR6_BIT10                   _ATD1DR6.Bits.BIT10
#define ATD1DR6_BIT11                   _ATD1DR6.Bits.BIT11
#define ATD1DR6_BIT12                   _ATD1DR6.Bits.BIT12
#define ATD1DR6_BIT13                   _ATD1DR6.Bits.BIT13
#define ATD1DR6_BIT14                   _ATD1DR6.Bits.BIT14
#define ATD1DR6_BIT15                   _ATD1DR6.Bits.BIT15
#define ATD1DR6_BIT_6                   _ATD1DR6.MergedBits.grpBIT_6
#define ATD1DR6_BIT                     ATD1DR6_BIT_6

#define ATD1DR6_BIT6_MASK               64U
#define ATD1DR6_BIT7_MASK               128U
#define ATD1DR6_BIT8_MASK               256U
#define ATD1DR6_BIT9_MASK               512U
#define ATD1DR6_BIT10_MASK              1024U
#define ATD1DR6_BIT11_MASK              2048U
#define ATD1DR6_BIT12_MASK              4096U
#define ATD1DR6_BIT13_MASK              8192U
#define ATD1DR6_BIT14_MASK              16384U
#define ATD1DR6_BIT15_MASK              32768U
#define ATD1DR6_BIT_6_MASK              65472U
#define ATD1DR6_BIT_6_BITNUM            6U


/*** ATD1DR7 - ATD 1 Conversion Result Register 7; 0x0000013E ***/
typedef union {
  word Word;
   /* Overlapped registers: */
  struct {
    /*** ATD1DR7H - ATD 1 Conversion Result Register 7 High; 0x0000013E ***/
    union {
      byte Byte;
      struct {
        byte BIT8        :1;                                       /* Bit 8 */
        byte BIT9        :1;                                       /* Bit 9 */
        byte BIT10       :1;                                       /* Bit 10 */
        byte BIT11       :1;                                       /* Bit 11 */
        byte BIT12       :1;                                       /* Bit 12 */
        byte BIT13       :1;                                       /* Bit 13 */
        byte BIT14       :1;                                       /* Bit 14 */
        byte BIT15       :1;                                       /* Bit 15 */
      } Bits;
    } ATD1DR7HSTR;
    #define ATD1DR7H                    _ATD1DR7.Overlap_STR.ATD1DR7HSTR.Byte
    #define ATD1DR7H_BIT8               _ATD1DR7.Overlap_STR.ATD1DR7HSTR.Bits.BIT8
    #define ATD1DR7H_BIT9               _ATD1DR7.Overlap_STR.ATD1DR7HSTR.Bits.BIT9
    #define ATD1DR7H_BIT10              _ATD1DR7.Overlap_STR.ATD1DR7HSTR.Bits.BIT10
    #define ATD1DR7H_BIT11              _ATD1DR7.Overlap_STR.ATD1DR7HSTR.Bits.BIT11
    #define ATD1DR7H_BIT12              _ATD1DR7.Overlap_STR.ATD1DR7HSTR.Bits.BIT12
    #define ATD1DR7H_BIT13              _ATD1DR7.Overlap_STR.ATD1DR7HSTR.Bits.BIT13
    #define ATD1DR7H_BIT14              _ATD1DR7.Overlap_STR.ATD1DR7HSTR.Bits.BIT14
    #define ATD1DR7H_BIT15              _ATD1DR7.Overlap_STR.ATD1DR7HSTR.Bits.BIT15
    
    #define ATD1DR7H_BIT8_MASK          1U
    #define ATD1DR7H_BIT9_MASK          2U
    #define ATD1DR7H_BIT10_MASK         4U
    #define ATD1DR7H_BIT11_MASK         8U
    #define ATD1DR7H_BIT12_MASK         16U
    #define ATD1DR7H_BIT13_MASK         32U
    #define ATD1DR7H_BIT14_MASK         64U
    #define ATD1DR7H_BIT15_MASK         128U
    

    /*** ATD1DR7L - ATD 1 Conversion Result Register 7 Low; 0x0000013F ***/
    union {
      byte Byte;
      struct {
        byte             :1; 
        byte             :1; 
        byte             :1; 
        byte             :1; 
        byte             :1; 
        byte             :1; 
        byte BIT6        :1;                                       /* Bit 6 */
        byte BIT7        :1;                                       /* Bit 7 */
      } Bits;
      struct {
        byte     :1;
        byte     :1;
        byte     :1;
        byte     :1;
        byte     :1;
        byte     :1;
        byte grpBIT_6 :2;
      } MergedBits;
    } ATD1DR7LSTR;
    #define ATD1DR7L                    _ATD1DR7.Overlap_STR.ATD1DR7LSTR.Byte
    #define ATD1DR7L_BIT6               _ATD1DR7.Overlap_STR.ATD1DR7LSTR.Bits.BIT6
    #define ATD1DR7L_BIT7               _ATD1DR7.Overlap_STR.ATD1DR7LSTR.Bits.BIT7
    #define ATD1DR7L_BIT_6              _ATD1DR7.Overlap_STR.ATD1DR7LSTR.MergedBits.grpBIT_6
    #define ATD1DR7L_BIT                ATD1DR7L_BIT_6
    
    #define ATD1DR7L_BIT6_MASK          64U
    #define ATD1DR7L_BIT7_MASK          128U
    #define ATD1DR7L_BIT_6_MASK         192U
    #define ATD1DR7L_BIT_6_BITNUM       6U
    
  } Overlap_STR;

  struct {
    word             :1; 
    word             :1; 
    word             :1; 
    word             :1; 
    word             :1; 
    word             :1; 
    word BIT6        :1;                                       /* Bit 6 */
    word BIT7        :1;                                       /* Bit 7 */
    word BIT8        :1;                                       /* Bit 8 */
    word BIT9        :1;                                       /* Bit 9 */
    word BIT10       :1;                                       /* Bit 10 */
    word BIT11       :1;                                       /* Bit 11 */
    word BIT12       :1;                                       /* Bit 12 */
    word BIT13       :1;                                       /* Bit 13 */
    word BIT14       :1;                                       /* Bit 14 */
    word BIT15       :1;                                       /* Bit 15 */
  } Bits;
  struct {
    word         :1;
    word         :1;
    word         :1;
    word         :1;
    word         :1;
    word         :1;
    word grpBIT_6 :10;
  } MergedBits;
} ATD1DR7STR;
extern volatile ATD1DR7STR _ATD1DR7 @(REG_BASE + 0x0000013EUL);
#define ATD1DR7                         _ATD1DR7.Word
#define ATD1DR7_BIT6                    _ATD1DR7.Bits.BIT6
#define ATD1DR7_BIT7                    _ATD1DR7.Bits.BIT7
#define ATD1DR7_BIT8                    _ATD1DR7.Bits.BIT8
#define ATD1DR7_BIT9                    _ATD1DR7.Bits.BIT9
#define ATD1DR7_BIT10                   _ATD1DR7.Bits.BIT10
#define ATD1DR7_BIT11                   _ATD1DR7.Bits.BIT11
#define ATD1DR7_BIT12                   _ATD1DR7.Bits.BIT12
#define ATD1DR7_BIT13                   _ATD1DR7.Bits.BIT13
#define ATD1DR7_BIT14                   _ATD1DR7.Bits.BIT14
#define ATD1DR7_BIT15                   _ATD1DR7.Bits.BIT15
#define ATD1DR7_BIT_6                   _ATD1DR7.MergedBits.grpBIT_6
#define ATD1DR7_BIT                     ATD1DR7_BIT_6

#define ATD1DR7_BIT6_MASK               64U
#define ATD1DR7_BIT7_MASK               128U
#define ATD1DR7_BIT8_MASK               256U
#define ATD1DR7_BIT9_MASK               512U
#define ATD1DR7_BIT10_MASK              1024U
#define ATD1DR7_BIT11_MASK              2048U
#define ATD1DR7_BIT12_MASK              4096U
#define ATD1DR7_BIT13_MASK              8192U
#define ATD1DR7_BIT14_MASK              16384U
#define ATD1DR7_BIT15_MASK              32768U
#define ATD1DR7_BIT_6_MASK              65472U
#define ATD1DR7_BIT_6_BITNUM            6U


/*** CAN0CTL0 - MSCAN 0 Control 0 Register; 0x00000140 ***/
typedef union {
  byte Byte;
  struct {
    byte INITRQ      :1;                                       /* Initialization Mode Request */
    byte SLPRQ       :1;                                       /* Sleep Mode Request */
    byte WUPE        :1;                                       /* Wake-Up Enable */
    byte TIME        :1;                                       /* Timer Enable */
    byte SYNCH       :1;                                       /* Synchronized Status */
    byte CSWAI       :1;                                       /* CAN Stops in Wait Mode */
    byte RXACT       :1;                                       /* Receiver Active Status */
    byte RXFRM       :1;                                       /* Received Frame Flag */
  } Bits;
} CAN0CTL0STR;
extern volatile CAN0CTL0STR _CAN0CTL0 @(REG_BASE + 0x00000140UL);
#define CAN0CTL0                        _CAN0CTL0.Byte
#define CAN0CTL0_INITRQ                 _CAN0CTL0.Bits.INITRQ
#define CAN0CTL0_SLPRQ                  _CAN0CTL0.Bits.SLPRQ
#define CAN0CTL0_WUPE                   _CAN0CTL0.Bits.WUPE
#define CAN0CTL0_TIME                   _CAN0CTL0.Bits.TIME
#define CAN0CTL0_SYNCH                  _CAN0CTL0.Bits.SYNCH
#define CAN0CTL0_CSWAI                  _CAN0CTL0.Bits.CSWAI
#define CAN0CTL0_RXACT                  _CAN0CTL0.Bits.RXACT
#define CAN0CTL0_RXFRM                  _CAN0CTL0.Bits.RXFRM
/* CAN0CTL_ARR: Access 2 CAN0CTLx registers in an array */
#define CAN0CTL_ARR                     ((volatile byte *) &CAN0CTL0)

#define CAN0CTL0_INITRQ_MASK            1U
#define CAN0CTL0_SLPRQ_MASK             2U
#define CAN0CTL0_WUPE_MASK              4U
#define CAN0CTL0_TIME_MASK              8U
#define CAN0CTL0_SYNCH_MASK             16U
#define CAN0CTL0_CSWAI_MASK             32U
#define CAN0CTL0_RXACT_MASK             64U
#define CAN0CTL0_RXFRM_MASK             128U


/*** CAN0CTL1 - MSCAN 0 Control 1 Register; 0x00000141 ***/
typedef union {
  byte Byte;
  struct {
    byte INITAK      :1;                                       /* Initialization Mode Acknowledge */
    byte SLPAK       :1;                                       /* Sleep Mode Acknowledge */
    byte WUPM        :1;                                       /* Wake-Up Mode */
    byte             :1; 
    byte LISTEN      :1;                                       /* Listen Only Mode */
    byte LOOPB       :1;                                       /* Loop Back Self Test Mode */
    byte CLKSRC      :1;                                       /* MSCAN 0 Clock Source */
    byte CANE        :1;                                       /* MSCAN 0 Enable */
  } Bits;
} CAN0CTL1STR;
extern volatile CAN0CTL1STR _CAN0CTL1 @(REG_BASE + 0x00000141UL);
#define CAN0CTL1                        _CAN0CTL1.Byte
#define CAN0CTL1_INITAK                 _CAN0CTL1.Bits.INITAK
#define CAN0CTL1_SLPAK                  _CAN0CTL1.Bits.SLPAK
#define CAN0CTL1_WUPM                   _CAN0CTL1.Bits.WUPM
#define CAN0CTL1_LISTEN                 _CAN0CTL1.Bits.LISTEN
#define CAN0CTL1_LOOPB                  _CAN0CTL1.Bits.LOOPB
#define CAN0CTL1_CLKSRC                 _CAN0CTL1.Bits.CLKSRC
#define CAN0CTL1_CANE                   _CAN0CTL1.Bits.CANE

#define CAN0CTL1_INITAK_MASK            1U
#define CAN0CTL1_SLPAK_MASK             2U
#define CAN0CTL1_WUPM_MASK              4U
#define CAN0CTL1_LISTEN_MASK            16U
#define CAN0CTL1_LOOPB_MASK             32U
#define CAN0CTL1_CLKSRC_MASK            64U
#define CAN0CTL1_CANE_MASK              128U


/*** CAN0BTR0 - MSCAN 0 Bus Timing Register 0; 0x00000142 ***/
typedef union {
  byte Byte;
  struct {
    byte BRP0        :1;                                       /* Baud Rate Prescaler 0 */
    byte BRP1        :1;                                       /* Baud Rate Prescaler 1 */
    byte BRP2        :1;                                       /* Baud Rate Prescaler 2 */
    byte BRP3        :1;                                       /* Baud Rate Prescaler 3 */
    byte BRP4        :1;                                       /* Baud Rate Prescaler 4 */
    byte BRP5        :1;                                       /* Baud Rate Prescaler 5 */
    byte SJW0        :1;                                       /* Synchronization Jump Width 0 */
    byte SJW1        :1;                                       /* Synchronization Jump Width 1 */
  } Bits;
  struct {
    byte grpBRP  :6;
    byte grpSJW  :2;
  } MergedBits;
} CAN0BTR0STR;
extern volatile CAN0BTR0STR _CAN0BTR0 @(REG_BASE + 0x00000142UL);
#define CAN0BTR0                        _CAN0BTR0.Byte
#define CAN0BTR0_BRP0                   _CAN0BTR0.Bits.BRP0
#define CAN0BTR0_BRP1                   _CAN0BTR0.Bits.BRP1
#define CAN0BTR0_BRP2                   _CAN0BTR0.Bits.BRP2
#define CAN0BTR0_BRP3                   _CAN0BTR0.Bits.BRP3
#define CAN0BTR0_BRP4                   _CAN0BTR0.Bits.BRP4
#define CAN0BTR0_BRP5                   _CAN0BTR0.Bits.BRP5
#define CAN0BTR0_SJW0                   _CAN0BTR0.Bits.SJW0
#define CAN0BTR0_SJW1                   _CAN0BTR0.Bits.SJW1
/* CAN0BTR_ARR: Access 2 CAN0BTRx registers in an array */
#define CAN0BTR_ARR                     ((volatile byte *) &CAN0BTR0)
#define CAN0BTR0_BRP                    _CAN0BTR0.MergedBits.grpBRP
#define CAN0BTR0_SJW                    _CAN0BTR0.MergedBits.grpSJW

#define CAN0BTR0_BRP0_MASK              1U
#define CAN0BTR0_BRP1_MASK              2U
#define CAN0BTR0_BRP2_MASK              4U
#define CAN0BTR0_BRP3_MASK              8U
#define CAN0BTR0_BRP4_MASK              16U
#define CAN0BTR0_BRP5_MASK              32U
#define CAN0BTR0_SJW0_MASK              64U
#define CAN0BTR0_SJW1_MASK              128U
#define CAN0BTR0_BRP_MASK               63U
#define CAN0BTR0_BRP_BITNUM             0U
#define CAN0BTR0_SJW_MASK               192U
#define CAN0BTR0_SJW_BITNUM             6U


/*** CAN0BTR1 - MSCAN 0 Bus Timing Register 1; 0x00000143 ***/
typedef union {
  byte Byte;
  struct {
    byte TSEG10      :1;                                       /* Time Segment 10 */
    byte TSEG11      :1;                                       /* Time Segment 11 */
    byte TSEG12      :1;                                       /* Time Segment 12 */
    byte TSEG13      :1;                                       /* Time Segment 13 */
    byte TSEG20      :1;                                       /* Time Segment 20 */
    byte TSEG21      :1;                                       /* Time Segment 21 */
    byte TSEG22      :1;                                       /* Time Segment 22 */
    byte SAMP        :1;                                       /* Sampling */
  } Bits;
  struct {
    byte grpTSEG_10 :4;
    byte grpTSEG_20 :3;
    byte         :1;
  } MergedBits;
} CAN0BTR1STR;
extern volatile CAN0BTR1STR _CAN0BTR1 @(REG_BASE + 0x00000143UL);
#define CAN0BTR1                        _CAN0BTR1.Byte
#define CAN0BTR1_TSEG10                 _CAN0BTR1.Bits.TSEG10
#define CAN0BTR1_TSEG11                 _CAN0BTR1.Bits.TSEG11
#define CAN0BTR1_TSEG12                 _CAN0BTR1.Bits.TSEG12
#define CAN0BTR1_TSEG13                 _CAN0BTR1.Bits.TSEG13
#define CAN0BTR1_TSEG20                 _CAN0BTR1.Bits.TSEG20
#define CAN0BTR1_TSEG21                 _CAN0BTR1.Bits.TSEG21
#define CAN0BTR1_TSEG22                 _CAN0BTR1.Bits.TSEG22
#define CAN0BTR1_SAMP                   _CAN0BTR1.Bits.SAMP
#define CAN0BTR1_TSEG_10                _CAN0BTR1.MergedBits.grpTSEG_10
#define CAN0BTR1_TSEG_20                _CAN0BTR1.MergedBits.grpTSEG_20
#define CAN0BTR1_TSEG                   CAN0BTR1_TSEG_10

#define CAN0BTR1_TSEG10_MASK            1U
#define CAN0BTR1_TSEG11_MASK            2U
#define CAN0BTR1_TSEG12_MASK            4U
#define CAN0BTR1_TSEG13_MASK            8U
#define CAN0BTR1_TSEG20_MASK            16U
#define CAN0BTR1_TSEG21_MASK            32U
#define CAN0BTR1_TSEG22_MASK            64U
#define CAN0BTR1_SAMP_MASK              128U
#define CAN0BTR1_TSEG_10_MASK           15U
#define CAN0BTR1_TSEG_10_BITNUM         0U
#define CAN0BTR1_TSEG_20_MASK           112U
#define CAN0BTR1_TSEG_20_BITNUM         4U


/*** CAN0RFLG - MSCAN 0 Receiver Flag Register; 0x00000144 ***/
typedef union {
  byte Byte;
  struct {
    byte RXF         :1;                                       /* Receive Buffer Full */
    byte OVRIF       :1;                                       /* Overrun Interrupt Flag */
    byte TSTAT0      :1;                                       /* Transmitter Status Bit 0 */
    byte TSTAT1      :1;                                       /* Transmitter Status Bit 1 */
    byte RSTAT0      :1;                                       /* Receiver Status Bit 0 */
    byte RSTAT1      :1;                                       /* Receiver Status Bit 1 */
    byte CSCIF       :1;                                       /* CAN Status Change Interrupt Flag */
    byte WUPIF       :1;                                       /* Wake-up Interrupt Flag */
  } Bits;
  struct {
    byte         :1;
    byte         :1;
    byte grpTSTAT :2;
    byte grpRSTAT :2;
    byte         :1;
    byte         :1;
  } MergedBits;
} CAN0RFLGSTR;
extern volatile CAN0RFLGSTR _CAN0RFLG @(REG_BASE + 0x00000144UL);
#define CAN0RFLG                        _CAN0RFLG.Byte
#define CAN0RFLG_RXF                    _CAN0RFLG.Bits.RXF
#define CAN0RFLG_OVRIF                  _CAN0RFLG.Bits.OVRIF
#define CAN0RFLG_TSTAT0                 _CAN0RFLG.Bits.TSTAT0
#define CAN0RFLG_TSTAT1                 _CAN0RFLG.Bits.TSTAT1
#define CAN0RFLG_RSTAT0                 _CAN0RFLG.Bits.RSTAT0
#define CAN0RFLG_RSTAT1                 _CAN0RFLG.Bits.RSTAT1
#define CAN0RFLG_CSCIF                  _CAN0RFLG.Bits.CSCIF
#define CAN0RFLG_WUPIF                  _CAN0RFLG.Bits.WUPIF
#define CAN0RFLG_TSTAT                  _CAN0RFLG.MergedBits.grpTSTAT
#define CAN0RFLG_RSTAT                  _CAN0RFLG.MergedBits.grpRSTAT

#define CAN0RFLG_RXF_MASK               1U
#define CAN0RFLG_OVRIF_MASK             2U
#define CAN0RFLG_TSTAT0_MASK            4U
#define CAN0RFLG_TSTAT1_MASK            8U
#define CAN0RFLG_RSTAT0_MASK            16U
#define CAN0RFLG_RSTAT1_MASK            32U
#define CAN0RFLG_CSCIF_MASK             64U
#define CAN0RFLG_WUPIF_MASK             128U
#define CAN0RFLG_TSTAT_MASK             12U
#define CAN0RFLG_TSTAT_BITNUM           2U
#define CAN0RFLG_RSTAT_MASK             48U
#define CAN0RFLG_RSTAT_BITNUM           4U


/*** CAN0RIER - MSCAN 0 Receiver Interrupt Enable Register; 0x00000145 ***/
typedef union {
  byte Byte;
  struct {
    byte RXFIE       :1;                                       /* Receiver Full Interrupt Enable */
    byte OVRIE       :1;                                       /* Overrun Interrupt Enable */
    byte TSTATE0     :1;                                       /* Transmitter Status Change Enable 0 */
    byte TSTATE1     :1;                                       /* Transmitter Status Change Enable 1 */
    byte RSTATE0     :1;                                       /* Receiver Status Change Enable 0 */
    byte RSTATE1     :1;                                       /* Receiver Status Change Enable 1 */
    byte CSCIE       :1;                                       /* CAN Status Change Interrupt Enable */
    byte WUPIE       :1;                                       /* Wake-up Interrupt Enable */
  } Bits;
  struct {
    byte         :1;
    byte         :1;
    byte grpTSTATE :2;
    byte grpRSTATE :2;
    byte         :1;
    byte         :1;
  } MergedBits;
} CAN0RIERSTR;
extern volatile CAN0RIERSTR _CAN0RIER @(REG_BASE + 0x00000145UL);
#define CAN0RIER                        _CAN0RIER.Byte
#define CAN0RIER_RXFIE                  _CAN0RIER.Bits.RXFIE
#define CAN0RIER_OVRIE                  _CAN0RIER.Bits.OVRIE
#define CAN0RIER_TSTATE0                _CAN0RIER.Bits.TSTATE0
#define CAN0RIER_TSTATE1                _CAN0RIER.Bits.TSTATE1
#define CAN0RIER_RSTATE0                _CAN0RIER.Bits.RSTATE0
#define CAN0RIER_RSTATE1                _CAN0RIER.Bits.RSTATE1
#define CAN0RIER_CSCIE                  _CAN0RIER.Bits.CSCIE
#define CAN0RIER_WUPIE                  _CAN0RIER.Bits.WUPIE
#define CAN0RIER_TSTATE                 _CAN0RIER.MergedBits.grpTSTATE
#define CAN0RIER_RSTATE                 _CAN0RIER.MergedBits.grpRSTATE

#define CAN0RIER_RXFIE_MASK             1U
#define CAN0RIER_OVRIE_MASK             2U
#define CAN0RIER_TSTATE0_MASK           4U
#define CAN0RIER_TSTATE1_MASK           8U
#define CAN0RIER_RSTATE0_MASK           16U
#define CAN0RIER_RSTATE1_MASK           32U
#define CAN0RIER_CSCIE_MASK             64U
#define CAN0RIER_WUPIE_MASK             128U
#define CAN0RIER_TSTATE_MASK            12U
#define CAN0RIER_TSTATE_BITNUM          2U
#define CAN0RIER_RSTATE_MASK            48U
#define CAN0RIER_RSTATE_BITNUM          4U


/*** CAN0TFLG - MSCAN 0 Transmitter Flag Register; 0x00000146 ***/
typedef union {
  byte Byte;
  struct {
    byte TXE0        :1;                                       /* Transmitter Buffer Empty 0 */
    byte TXE1        :1;                                       /* Transmitter Buffer Empty 1 */
    byte TXE2        :1;                                       /* Transmitter Buffer Empty 2 */
    byte             :1; 
    byte             :1; 
    byte             :1; 
    byte             :1; 
    byte             :1; 
  } Bits;
  struct {
    byte grpTXE  :3;
    byte         :1;
    byte         :1;
    byte         :1;
    byte         :1;
    byte         :1;
  } MergedBits;
} CAN0TFLGSTR;
extern volatile CAN0TFLGSTR _CAN0TFLG @(REG_BASE + 0x00000146UL);
#define CAN0TFLG                        _CAN0TFLG.Byte
#define CAN0TFLG_TXE0                   _CAN0TFLG.Bits.TXE0
#define CAN0TFLG_TXE1                   _CAN0TFLG.Bits.TXE1
#define CAN0TFLG_TXE2                   _CAN0TFLG.Bits.TXE2
#define CAN0TFLG_TXE                    _CAN0TFLG.MergedBits.grpTXE

#define CAN0TFLG_TXE0_MASK              1U
#define CAN0TFLG_TXE1_MASK              2U
#define CAN0TFLG_TXE2_MASK              4U
#define CAN0TFLG_TXE_MASK               7U
#define CAN0TFLG_TXE_BITNUM             0U


/*** CAN0TIER - MSCAN 0 Transmitter Interrupt Enable Register; 0x00000147 ***/
typedef union {
  byte Byte;
  struct {
    byte TXEIE0      :1;                                       /* Transmitter Empty Interrupt Enable 0 */
    byte TXEIE1      :1;                                       /* Transmitter Empty Interrupt Enable 1 */
    byte TXEIE2      :1;                                       /* Transmitter Empty Interrupt Enable 2 */
    byte             :1; 
    byte             :1; 
    byte             :1; 
    byte             :1; 
    byte             :1; 
  } Bits;
  struct {
    byte grpTXEIE :3;
    byte         :1;
    byte         :1;
    byte         :1;
    byte         :1;
    byte         :1;
  } MergedBits;
} CAN0TIERSTR;
extern volatile CAN0TIERSTR _CAN0TIER @(REG_BASE + 0x00000147UL);
#define CAN0TIER                        _CAN0TIER.Byte
#define CAN0TIER_TXEIE0                 _CAN0TIER.Bits.TXEIE0
#define CAN0TIER_TXEIE1                 _CAN0TIER.Bits.TXEIE1
#define CAN0TIER_TXEIE2                 _CAN0TIER.Bits.TXEIE2
#define CAN0TIER_TXEIE                  _CAN0TIER.MergedBits.grpTXEIE

#define CAN0TIER_TXEIE0_MASK            1U
#define CAN0TIER_TXEIE1_MASK            2U
#define CAN0TIER_TXEIE2_MASK            4U
#define CAN0TIER_TXEIE_MASK             7U
#define CAN0TIER_TXEIE_BITNUM           0U


/*** CAN0TARQ - MSCAN 0 Transmitter Message Abort Request; 0x00000148 ***/
typedef union {
  byte Byte;
  struct {
    byte ABTRQ0      :1;                                       /* Abort Request 0 */
    byte ABTRQ1      :1;                                       /* Abort Request 1 */
    byte ABTRQ2      :1;                                       /* Abort Request 2 */
    byte             :1; 
    byte             :1; 
    byte             :1; 
    byte             :1; 
    byte             :1; 
  } Bits;
  struct {
    byte grpABTRQ :3;
    byte         :1;
    byte         :1;
    byte         :1;
    byte         :1;
    byte         :1;
  } MergedBits;
} CAN0TARQSTR;
extern volatile CAN0TARQSTR _CAN0TARQ @(REG_BASE + 0x00000148UL);
#define CAN0TARQ                        _CAN0TARQ.Byte
#define CAN0TARQ_ABTRQ0                 _CAN0TARQ.Bits.ABTRQ0
#define CAN0TARQ_ABTRQ1                 _CAN0TARQ.Bits.ABTRQ1
#define CAN0TARQ_ABTRQ2                 _CAN0TARQ.Bits.ABTRQ2
#define CAN0TARQ_ABTRQ                  _CAN0TARQ.MergedBits.grpABTRQ

#define CAN0TARQ_ABTRQ0_MASK            1U
#define CAN0TARQ_ABTRQ1_MASK            2U
#define CAN0TARQ_ABTRQ2_MASK            4U
#define CAN0TARQ_ABTRQ_MASK             7U
#define CAN0TARQ_ABTRQ_BITNUM           0U


/*** CAN0TAAK - MSCAN 0 Transmitter Message Abort Control; 0x00000149 ***/
typedef union {
  byte Byte;
  struct {
    byte ABTAK0      :1;                                       /* Abort Acknowledge 0 */
    byte ABTAK1      :1;                                       /* Abort Acknowledge 1 */
    byte ABTAK2      :1;                                       /* Abort Acknowledge 2 */
    byte             :1; 
    byte             :1; 
    byte             :1; 
    byte             :1; 
    byte             :1; 
  } Bits;
  struct {
    byte grpABTAK :3;
    byte         :1;
    byte         :1;
    byte         :1;
    byte         :1;
    byte         :1;
  } MergedBits;
} CAN0TAAKSTR;
extern volatile CAN0TAAKSTR _CAN0TAAK @(REG_BASE + 0x00000149UL);
#define CAN0TAAK                        _CAN0TAAK.Byte
#define CAN0TAAK_ABTAK0                 _CAN0TAAK.Bits.ABTAK0
#define CAN0TAAK_ABTAK1                 _CAN0TAAK.Bits.ABTAK1
#define CAN0TAAK_ABTAK2                 _CAN0TAAK.Bits.ABTAK2
#define CAN0TAAK_ABTAK                  _CAN0TAAK.MergedBits.grpABTAK

#define CAN0TAAK_ABTAK0_MASK            1U
#define CAN0TAAK_ABTAK1_MASK            2U
#define CAN0TAAK_ABTAK2_MASK            4U
#define CAN0TAAK_ABTAK_MASK             7U
#define CAN0TAAK_ABTAK_BITNUM           0U


/*** CAN0TBSEL - MSCAN 0 Transmit Buffer Selection; 0x0000014A ***/
typedef union {
  byte Byte;
  struct {
    byte TX0         :1;                                       /* Transmit Buffer Select 0 */
    byte TX1         :1;                                       /* Transmit Buffer Select 1 */
    byte TX2         :1;                                       /* Transmit Buffer Select 2 */
    byte             :1; 
    byte             :1; 
    byte             :1; 
    byte             :1; 
    byte             :1; 
  } Bits;
  struct {
    byte grpTX   :3;
    byte         :1;
    byte         :1;
    byte         :1;
    byte         :1;
    byte         :1;
  } MergedBits;
} CAN0TBSELSTR;
extern volatile CAN0TBSELSTR _CAN0TBSEL @(REG_BASE + 0x0000014AUL);
#define CAN0TBSEL                       _CAN0TBSEL.Byte
#define CAN0TBSEL_TX0                   _CAN0TBSEL.Bits.TX0
#define CAN0TBSEL_TX1                   _CAN0TBSEL.Bits.TX1
#define CAN0TBSEL_TX2                   _CAN0TBSEL.Bits.TX2
#define CAN0TBSEL_TX                    _CAN0TBSEL.MergedBits.grpTX

#define CAN0TBSEL_TX0_MASK              1U
#define CAN0TBSEL_TX1_MASK              2U
#define CAN0TBSEL_TX2_MASK              4U
#define CAN0TBSEL_TX_MASK               7U
#define CAN0TBSEL_TX_BITNUM             0U


/*** CAN0IDAC - MSCAN 0 Identifier Acceptance Control Register; 0x0000014B ***/
typedef union {
  byte Byte;
  struct {
    byte IDHIT0      :1;                                       /* Identifier Acceptance Hit Indicator 0 */
    byte IDHIT1      :1;                                       /* Identifier Acceptance Hit Indicator 1 */
    byte IDHIT2      :1;                                       /* Identifier Acceptance Hit Indicator 2 */
    byte             :1; 
    byte IDAM0       :1;                                       /* Identifier Acceptance Mode 0 */
    byte IDAM1       :1;                                       /* Identifier Acceptance Mode 1 */
    byte             :1; 
    byte             :1; 
  } Bits;
  struct {
    byte grpIDHIT :3;
    byte         :1;
    byte grpIDAM :2;
    byte         :1;
    byte         :1;
  } MergedBits;
} CAN0IDACSTR;
extern volatile CAN0IDACSTR _CAN0IDAC @(REG_BASE + 0x0000014BUL);
#define CAN0IDAC                        _CAN0IDAC.Byte
#define CAN0IDAC_IDHIT0                 _CAN0IDAC.Bits.IDHIT0
#define CAN0IDAC_IDHIT1                 _CAN0IDAC.Bits.IDHIT1
#define CAN0IDAC_IDHIT2                 _CAN0IDAC.Bits.IDHIT2
#define CAN0IDAC_IDAM0                  _CAN0IDAC.Bits.IDAM0
#define CAN0IDAC_IDAM1                  _CAN0IDAC.Bits.IDAM1
#define CAN0IDAC_IDHIT                  _CAN0IDAC.MergedBits.grpIDHIT
#define CAN0IDAC_IDAM                   _CAN0IDAC.MergedBits.grpIDAM

#define CAN0IDAC_IDHIT0_MASK            1U
#define CAN0IDAC_IDHIT1_MASK            2U
#define CAN0IDAC_IDHIT2_MASK            4U
#define CAN0IDAC_IDAM0_MASK             16U
#define CAN0IDAC_IDAM1_MASK             32U
#define CAN0IDAC_IDHIT_MASK             7U
#define CAN0IDAC_IDHIT_BITNUM           0U
#define CAN0IDAC_IDAM_MASK              48U
#define CAN0IDAC_IDAM_BITNUM            4U


/*** CAN0RXERR - MSCAN 0 Receive Error Counter Register; 0x0000014E ***/
typedef union {
  byte Byte;
  struct {
    byte RXERR0      :1;                                       /* Bit 0 */
    byte RXERR1      :1;                                       /* Bit 1 */
    byte RXERR2      :1;                                       /* Bit 2 */
    byte RXERR3      :1;                                       /* Bit 3 */
    byte RXERR4      :1;                                       /* Bit 4 */
    byte RXERR5      :1;                                       /* Bit 5 */
    byte RXERR6      :1;                                       /* Bit 6 */
    byte RXERR7      :1;                                       /* Bit 7 */
  } Bits;
} CAN0RXERRSTR;
extern volatile CAN0RXERRSTR _CAN0RXERR @(REG_BASE + 0x0000014EUL);
#define CAN0RXERR                       _CAN0RXERR.Byte
#define CAN0RXERR_RXERR0                _CAN0RXERR.Bits.RXERR0
#define CAN0RXERR_RXERR1                _CAN0RXERR.Bits.RXERR1
#define CAN0RXERR_RXERR2                _CAN0RXERR.Bits.RXERR2
#define CAN0RXERR_RXERR3                _CAN0RXERR.Bits.RXERR3
#define CAN0RXERR_RXERR4                _CAN0RXERR.Bits.RXERR4
#define CAN0RXERR_RXERR5                _CAN0RXERR.Bits.RXERR5
#define CAN0RXERR_RXERR6                _CAN0RXERR.Bits.RXERR6
#define CAN0RXERR_RXERR7                _CAN0RXERR.Bits.RXERR7

#define CAN0RXERR_RXERR0_MASK           1U
#define CAN0RXERR_RXERR1_MASK           2U
#define CAN0RXERR_RXERR2_MASK           4U
#define CAN0RXERR_RXERR3_MASK           8U
#define CAN0RXERR_RXERR4_MASK           16U
#define CAN0RXERR_RXERR5_MASK           32U
#define CAN0RXERR_RXERR6_MASK           64U
#define CAN0RXERR_RXERR7_MASK           128U


/*** CAN0TXERR - MSCAN 0 Transmit Error Counter Register; 0x0000014F ***/
typedef union {
  byte Byte;
  struct {
    byte TXERR0      :1;                                       /* Bit 0 */
    byte TXERR1      :1;                                       /* Bit 1 */
    byte TXERR2      :1;                                       /* Bit 2 */
    byte TXERR3      :1;                                       /* Bit 3 */
    byte TXERR4      :1;                                       /* Bit 4 */
    byte TXERR5      :1;                                       /* Bit 5 */
    byte TXERR6      :1;                                       /* Bit 6 */
    byte TXERR7      :1;                                       /* Bit 7 */
  } Bits;
} CAN0TXERRSTR;
extern volatile CAN0TXERRSTR _CAN0TXERR @(REG_BASE + 0x0000014FUL);
#define CAN0TXERR                       _CAN0TXERR.Byte
#define CAN0TXERR_TXERR0                _CAN0TXERR.Bits.TXERR0
#define CAN0TXERR_TXERR1                _CAN0TXERR.Bits.TXERR1
#define CAN0TXERR_TXERR2                _CAN0TXERR.Bits.TXERR2
#define CAN0TXERR_TXERR3                _CAN0TXERR.Bits.TXERR3
#define CAN0TXERR_TXERR4                _CAN0TXERR.Bits.TXERR4
#define CAN0TXERR_TXERR5                _CAN0TXERR.Bits.TXERR5
#define CAN0TXERR_TXERR6                _CAN0TXERR.Bits.TXERR6
#define CAN0TXERR_TXERR7                _CAN0TXERR.Bits.TXERR7

#define CAN0TXERR_TXERR0_MASK           1U
#define CAN0TXERR_TXERR1_MASK           2U
#define CAN0TXERR_TXERR2_MASK           4U
#define CAN0TXERR_TXERR3_MASK           8U
#define CAN0TXERR_TXERR4_MASK           16U
#define CAN0TXERR_TXERR5_MASK           32U
#define CAN0TXERR_TXERR6_MASK           64U
#define CAN0TXERR_TXERR7_MASK           128U


/*** CAN0IDAR0 - MSCAN 0 Identifier Acceptance Register 0; 0x00000150 ***/
typedef union {
  byte Byte;
  struct {
    byte AC0         :1;                                       /* Acceptance Code Bit 0 */
    byte AC1         :1;                                       /* Acceptance Code Bit 1 */
    byte AC2         :1;                                       /* Acceptance Code Bit 2 */
    byte AC3         :1;                                       /* Acceptance Code Bit 3 */
    byte AC4         :1;                                       /* Acceptance Code Bit 4 */
    byte AC5         :1;                                       /* Acceptance Code Bit 5 */
    byte AC6         :1;                                       /* Acceptance Code Bit 6 */
    byte AC7         :1;                                       /* Acceptance Code Bit 7 */
  } Bits;
} CAN0IDAR0STR;
extern volatile CAN0IDAR0STR _CAN0IDAR0 @(REG_BASE + 0x00000150UL);
#define CAN0IDAR0                       _CAN0IDAR0.Byte
#define CAN0IDAR0_AC0                   _CAN0IDAR0.Bits.AC0
#define CAN0IDAR0_AC1                   _CAN0IDAR0.Bits.AC1
#define CAN0IDAR0_AC2                   _CAN0IDAR0.Bits.AC2
#define CAN0IDAR0_AC3                   _CAN0IDAR0.Bits.AC3
#define CAN0IDAR0_AC4                   _CAN0IDAR0.Bits.AC4
#define CAN0IDAR0_AC5                   _CAN0IDAR0.Bits.AC5
#define CAN0IDAR0_AC6                   _CAN0IDAR0.Bits.AC6
#define CAN0IDAR0_AC7                   _CAN0IDAR0.Bits.AC7
/* CAN0IDAR_ARR: Access 4 CAN0IDARx registers in an array */
#define CAN0IDAR_ARR                    ((volatile byte *) &CAN0IDAR0)

#define CAN0IDAR0_AC0_MASK              1U
#define CAN0IDAR0_AC1_MASK              2U
#define CAN0IDAR0_AC2_MASK              4U
#define CAN0IDAR0_AC3_MASK              8U
#define CAN0IDAR0_AC4_MASK              16U
#define CAN0IDAR0_AC5_MASK              32U
#define CAN0IDAR0_AC6_MASK              64U
#define CAN0IDAR0_AC7_MASK              128U


/*** CAN0IDAR1 - MSCAN 0 Identifier Acceptance Register 1; 0x00000151 ***/
typedef union {
  byte Byte;
  struct {
    byte AC0         :1;                                       /* Acceptance Code Bit 0 */
    byte AC1         :1;                                       /* Acceptance Code Bit 1 */
    byte AC2         :1;                                       /* Acceptance Code Bit 2 */
    byte AC3         :1;                                       /* Acceptance Code Bit 3 */
    byte AC4         :1;                                       /* Acceptance Code Bit 4 */
    byte AC5         :1;                                       /* Acceptance Code Bit 5 */
    byte AC6         :1;                                       /* Acceptance Code Bit 6 */
    byte AC7         :1;                                       /* Acceptance Code Bit 7 */
  } Bits;
} CAN0IDAR1STR;
extern volatile CAN0IDAR1STR _CAN0IDAR1 @(REG_BASE + 0x00000151UL);
#define CAN0IDAR1                       _CAN0IDAR1.Byte
#define CAN0IDAR1_AC0                   _CAN0IDAR1.Bits.AC0
#define CAN0IDAR1_AC1                   _CAN0IDAR1.Bits.AC1
#define CAN0IDAR1_AC2                   _CAN0IDAR1.Bits.AC2
#define CAN0IDAR1_AC3                   _CAN0IDAR1.Bits.AC3
#define CAN0IDAR1_AC4                   _CAN0IDAR1.Bits.AC4
#define CAN0IDAR1_AC5                   _CAN0IDAR1.Bits.AC5
#define CAN0IDAR1_AC6                   _CAN0IDAR1.Bits.AC6
#define CAN0IDAR1_AC7                   _CAN0IDAR1.Bits.AC7

#define CAN0IDAR1_AC0_MASK              1U
#define CAN0IDAR1_AC1_MASK              2U
#define CAN0IDAR1_AC2_MASK              4U
#define CAN0IDAR1_AC3_MASK              8U
#define CAN0IDAR1_AC4_MASK              16U
#define CAN0IDAR1_AC5_MASK              32U
#define CAN0IDAR1_AC6_MASK              64U
#define CAN0IDAR1_AC7_MASK              128U


/*** CAN0IDAR2 - MSCAN 0 Identifier Acceptance Register 2; 0x00000152 ***/
typedef union {
  byte Byte;
  struct {
    byte AC0         :1;                                       /* Acceptance Code Bit 0 */
    byte AC1         :1;                                       /* Acceptance Code Bit 1 */
    byte AC2         :1;                                       /* Acceptance Code Bit 2 */
    byte AC3         :1;                                       /* Acceptance Code Bit 3 */
    byte AC4         :1;                                       /* Acceptance Code Bit 4 */
    byte AC5         :1;                                       /* Acceptance Code Bit 5 */
    byte AC6         :1;                                       /* Acceptance Code Bit 6 */
    byte AC7         :1;                                       /* Acceptance Code Bit 7 */
  } Bits;
} CAN0IDAR2STR;
extern volatile CAN0IDAR2STR _CAN0IDAR2 @(REG_BASE + 0x00000152UL);
#define CAN0IDAR2                       _CAN0IDAR2.Byte
#define CAN0IDAR2_AC0                   _CAN0IDAR2.Bits.AC0
#define CAN0IDAR2_AC1                   _CAN0IDAR2.Bits.AC1
#define CAN0IDAR2_AC2                   _CAN0IDAR2.Bits.AC2
#define CAN0IDAR2_AC3                   _CAN0IDAR2.Bits.AC3
#define CAN0IDAR2_AC4                   _CAN0IDAR2.Bits.AC4
#define CAN0IDAR2_AC5                   _CAN0IDAR2.Bits.AC5
#define CAN0IDAR2_AC6                   _CAN0IDAR2.Bits.AC6
#define CAN0IDAR2_AC7                   _CAN0IDAR2.Bits.AC7

#define CAN0IDAR2_AC0_MASK              1U
#define CAN0IDAR2_AC1_MASK              2U
#define CAN0IDAR2_AC2_MASK              4U
#define CAN0IDAR2_AC3_MASK              8U
#define CAN0IDAR2_AC4_MASK              16U
#define CAN0IDAR2_AC5_MASK              32U
#define CAN0IDAR2_AC6_MASK              64U
#define CAN0IDAR2_AC7_MASK              128U


/*** CAN0IDAR3 - MSCAN 0 Identifier Acceptance Register 3; 0x00000153 ***/
typedef union {
  byte Byte;
  struct {
    byte AC0         :1;                                       /* Acceptance Code Bit 0 */
    byte AC1         :1;                                       /* Acceptance Code Bit 1 */
    byte AC2         :1;                                       /* Acceptance Code Bit 2 */
    byte AC3         :1;                                       /* Acceptance Code Bit 3 */
    byte AC4         :1;                                       /* Acceptance Code Bit 4 */
    byte AC5         :1;                                       /* Acceptance Code Bit 5 */
    byte AC6         :1;                                       /* Acceptance Code Bit 6 */
    byte AC7         :1;                                       /* Acceptance Code Bit 7 */
  } Bits;
} CAN0IDAR3STR;
extern volatile CAN0IDAR3STR _CAN0IDAR3 @(REG_BASE + 0x00000153UL);
#define CAN0IDAR3                       _CAN0IDAR3.Byte
#define CAN0IDAR3_AC0                   _CAN0IDAR3.Bits.AC0
#define CAN0IDAR3_AC1                   _CAN0IDAR3.Bits.AC1
#define CAN0IDAR3_AC2                   _CAN0IDAR3.Bits.AC2
#define CAN0IDAR3_AC3                   _CAN0IDAR3.Bits.AC3
#define CAN0IDAR3_AC4                   _CAN0IDAR3.Bits.AC4
#define CAN0IDAR3_AC5                   _CAN0IDAR3.Bits.AC5
#define CAN0IDAR3_AC6                   _CAN0IDAR3.Bits.AC6
#define CAN0IDAR3_AC7                   _CAN0IDAR3.Bits.AC7

#define CAN0IDAR3_AC0_MASK              1U
#define CAN0IDAR3_AC1_MASK              2U
#define CAN0IDAR3_AC2_MASK              4U
#define CAN0IDAR3_AC3_MASK              8U
#define CAN0IDAR3_AC4_MASK              16U
#define CAN0IDAR3_AC5_MASK              32U
#define CAN0IDAR3_AC6_MASK              64U
#define CAN0IDAR3_AC7_MASK              128U


/*** CAN0IDMR0 - MSCAN 0 Identifier Mask Register 0; 0x00000154 ***/
typedef union {
  byte Byte;
  struct {
    byte AM0         :1;                                       /* Acceptance Mask Bit 0 */
    byte AM1         :1;                                       /* Acceptance Mask Bit 1 */
    byte AM2         :1;                                       /* Acceptance Mask Bit 2 */
    byte AM3         :1;                                       /* Acceptance Mask Bit 3 */
    byte AM4         :1;                                       /* Acceptance Mask Bit 4 */
    byte AM5         :1;                                       /* Acceptance Mask Bit 5 */
    byte AM6         :1;                                       /* Acceptance Mask Bit 6 */
    byte AM7         :1;                                       /* Acceptance Mask Bit 7 */
  } Bits;
} CAN0IDMR0STR;
extern volatile CAN0IDMR0STR _CAN0IDMR0 @(REG_BASE + 0x00000154UL);
#define CAN0IDMR0                       _CAN0IDMR0.Byte
#define CAN0IDMR0_AM0                   _CAN0IDMR0.Bits.AM0
#define CAN0IDMR0_AM1                   _CAN0IDMR0.Bits.AM1
#define CAN0IDMR0_AM2                   _CAN0IDMR0.Bits.AM2
#define CAN0IDMR0_AM3                   _CAN0IDMR0.Bits.AM3
#define CAN0IDMR0_AM4                   _CAN0IDMR0.Bits.AM4
#define CAN0IDMR0_AM5                   _CAN0IDMR0.Bits.AM5
#define CAN0IDMR0_AM6                   _CAN0IDMR0.Bits.AM6
#define CAN0IDMR0_AM7                   _CAN0IDMR0.Bits.AM7
/* CAN0IDMR_ARR: Access 4 CAN0IDMRx registers in an array */
#define CAN0IDMR_ARR                    ((volatile byte *) &CAN0IDMR0)

#define CAN0IDMR0_AM0_MASK              1U
#define CAN0IDMR0_AM1_MASK              2U
#define CAN0IDMR0_AM2_MASK              4U
#define CAN0IDMR0_AM3_MASK              8U
#define CAN0IDMR0_AM4_MASK              16U
#define CAN0IDMR0_AM5_MASK              32U
#define CAN0IDMR0_AM6_MASK              64U
#define CAN0IDMR0_AM7_MASK              128U


/*** CAN0IDMR1 - MSCAN 0 Identifier Mask Register 1; 0x00000155 ***/
typedef union {
  byte Byte;
  struct {
    byte AM0         :1;                                       /* Acceptance Mask Bit 0 */
    byte AM1         :1;                                       /* Acceptance Mask Bit 1 */
    byte AM2         :1;                                       /* Acceptance Mask Bit 2 */
    byte AM3         :1;                                       /* Acceptance Mask Bit 3 */
    byte AM4         :1;                                       /* Acceptance Mask Bit 4 */
    byte AM5         :1;                                       /* Acceptance Mask Bit 5 */
    byte AM6         :1;                                       /* Acceptance Mask Bit 6 */
    byte AM7         :1;                                       /* Acceptance Mask Bit 7 */
  } Bits;
} CAN0IDMR1STR;
extern volatile CAN0IDMR1STR _CAN0IDMR1 @(REG_BASE + 0x00000155UL);
#define CAN0IDMR1                       _CAN0IDMR1.Byte
#define CAN0IDMR1_AM0                   _CAN0IDMR1.Bits.AM0
#define CAN0IDMR1_AM1                   _CAN0IDMR1.Bits.AM1
#define CAN0IDMR1_AM2                   _CAN0IDMR1.Bits.AM2
#define CAN0IDMR1_AM3                   _CAN0IDMR1.Bits.AM3
#define CAN0IDMR1_AM4                   _CAN0IDMR1.Bits.AM4
#define CAN0IDMR1_AM5                   _CAN0IDMR1.Bits.AM5
#define CAN0IDMR1_AM6                   _CAN0IDMR1.Bits.AM6
#define CAN0IDMR1_AM7                   _CAN0IDMR1.Bits.AM7

#define CAN0IDMR1_AM0_MASK              1U
#define CAN0IDMR1_AM1_MASK              2U
#define CAN0IDMR1_AM2_MASK              4U
#define CAN0IDMR1_AM3_MASK              8U
#define CAN0IDMR1_AM4_MASK              16U
#define CAN0IDMR1_AM5_MASK              32U
#define CAN0IDMR1_AM6_MASK              64U
#define CAN0IDMR1_AM7_MASK              128U


/*** CAN0IDMR2 - MSCAN 0 Identifier Mask Register 2; 0x00000156 ***/
typedef union {
  byte Byte;
  struct {
    byte AM0         :1;                                       /* Acceptance Mask Bit 0 */
    byte AM1         :1;                                       /* Acceptance Mask Bit 1 */
    byte AM2         :1;                                       /* Acceptance Mask Bit 2 */
    byte AM3         :1;                                       /* Acceptance Mask Bit 3 */
    byte AM4         :1;                                       /* Acceptance Mask Bit 4 */
    byte AM5         :1;                                       /* Acceptance Mask Bit 5 */
    byte AM6         :1;                                       /* Acceptance Mask Bit 6 */
    byte AM7         :1;                                       /* Acceptance Mask Bit 7 */
  } Bits;
} CAN0IDMR2STR;
extern volatile CAN0IDMR2STR _CAN0IDMR2 @(REG_BASE + 0x00000156UL);
#define CAN0IDMR2                       _CAN0IDMR2.Byte
#define CAN0IDMR2_AM0                   _CAN0IDMR2.Bits.AM0
#define CAN0IDMR2_AM1                   _CAN0IDMR2.Bits.AM1
#define CAN0IDMR2_AM2                   _CAN0IDMR2.Bits.AM2
#define CAN0IDMR2_AM3                   _CAN0IDMR2.Bits.AM3
#define CAN0IDMR2_AM4                   _CAN0IDMR2.Bits.AM4
#define CAN0IDMR2_AM5                   _CAN0IDMR2.Bits.AM5
#define CAN0IDMR2_AM6                   _CAN0IDMR2.Bits.AM6
#define CAN0IDMR2_AM7                   _CAN0IDMR2.Bits.AM7

#define CAN0IDMR2_AM0_MASK              1U
#define CAN0IDMR2_AM1_MASK              2U
#define CAN0IDMR2_AM2_MASK              4U
#define CAN0IDMR2_AM3_MASK              8U
#define CAN0IDMR2_AM4_MASK              16U
#define CAN0IDMR2_AM5_MASK              32U
#define CAN0IDMR2_AM6_MASK              64U
#define CAN0IDMR2_AM7_MASK              128U


/*** CAN0IDMR3 - MSCAN 0 Identifier Mask Register 3; 0x00000157 ***/
typedef union {
  byte Byte;
  struct {
    byte AM0         :1;                                       /* Acceptance Mask Bit 0 */
    byte AM1         :1;                                       /* Acceptance Mask Bit 1 */
    byte AM2         :1;                                       /* Acceptance Mask Bit 2 */
    byte AM3         :1;                                       /* Acceptance Mask Bit 3 */
    byte AM4         :1;                                       /* Acceptance Mask Bit 4 */
    byte AM5         :1;                                       /* Acceptance Mask Bit 5 */
    byte AM6         :1;                                       /* Acceptance Mask Bit 6 */
    byte AM7         :1;                                       /* Acceptance Mask Bit 7 */
  } Bits;
} CAN0IDMR3STR;
extern volatile CAN0IDMR3STR _CAN0IDMR3 @(REG_BASE + 0x00000157UL);
#define CAN0IDMR3                       _CAN0IDMR3.Byte
#define CAN0IDMR3_AM0                   _CAN0IDMR3.Bits.AM0
#define CAN0IDMR3_AM1                   _CAN0IDMR3.Bits.AM1
#define CAN0IDMR3_AM2                   _CAN0IDMR3.Bits.AM2
#define CAN0IDMR3_AM3                   _CAN0IDMR3.Bits.AM3
#define CAN0IDMR3_AM4                   _CAN0IDMR3.Bits.AM4
#define CAN0IDMR3_AM5                   _CAN0IDMR3.Bits.AM5
#define CAN0IDMR3_AM6                   _CAN0IDMR3.Bits.AM6
#define CAN0IDMR3_AM7                   _CAN0IDMR3.Bits.AM7

#define CAN0IDMR3_AM0_MASK              1U
#define CAN0IDMR3_AM1_MASK              2U
#define CAN0IDMR3_AM2_MASK              4U
#define CAN0IDMR3_AM3_MASK              8U
#define CAN0IDMR3_AM4_MASK              16U
#define CAN0IDMR3_AM5_MASK              32U
#define CAN0IDMR3_AM6_MASK              64U
#define CAN0IDMR3_AM7_MASK              128U


/*** CAN0IDAR4 - MSCAN 0 Identifier Acceptance Register 4; 0x00000158 ***/
typedef union {
  byte Byte;
  struct {
    byte AC0         :1;                                       /* Acceptance Code Bit 0 */
    byte AC1         :1;                                       /* Acceptance Code Bit 1 */
    byte AC2         :1;                                       /* Acceptance Code Bit 2 */
    byte AC3         :1;                                       /* Acceptance Code Bit 3 */
    byte AC4         :1;                                       /* Acceptance Code Bit 4 */
    byte AC5         :1;                                       /* Acceptance Code Bit 5 */
    byte AC6         :1;                                       /* Acceptance Code Bit 6 */
    byte AC7         :1;                                       /* Acceptance Code Bit 7 */
  } Bits;
} CAN0IDAR4STR;
extern volatile CAN0IDAR4STR _CAN0IDAR4 @(REG_BASE + 0x00000158UL);
#define CAN0IDAR4                       _CAN0IDAR4.Byte
#define CAN0IDAR4_AC0                   _CAN0IDAR4.Bits.AC0
#define CAN0IDAR4_AC1                   _CAN0IDAR4.Bits.AC1
#define CAN0IDAR4_AC2                   _CAN0IDAR4.Bits.AC2
#define CAN0IDAR4_AC3                   _CAN0IDAR4.Bits.AC3
#define CAN0IDAR4_AC4                   _CAN0IDAR4.Bits.AC4
#define CAN0IDAR4_AC5                   _CAN0IDAR4.Bits.AC5
#define CAN0IDAR4_AC6                   _CAN0IDAR4.Bits.AC6
#define CAN0IDAR4_AC7                   _CAN0IDAR4.Bits.AC7

#define CAN0IDAR4_AC0_MASK              1U
#define CAN0IDAR4_AC1_MASK              2U
#define CAN0IDAR4_AC2_MASK              4U
#define CAN0IDAR4_AC3_MASK              8U
#define CAN0IDAR4_AC4_MASK              16U
#define CAN0IDAR4_AC5_MASK              32U
#define CAN0IDAR4_AC6_MASK              64U
#define CAN0IDAR4_AC7_MASK              128U


/*** CAN0IDAR5 - MSCAN 0 Identifier Acceptance Register 5; 0x00000159 ***/
typedef union {
  byte Byte;
  struct {
    byte AC0         :1;                                       /* Acceptance Code Bit 0 */
    byte AC1         :1;                                       /* Acceptance Code Bit 1 */
    byte AC2         :1;                                       /* Acceptance Code Bit 2 */
    byte AC3         :1;                                       /* Acceptance Code Bit 3 */
    byte AC4         :1;                                       /* Acceptance Code Bit 4 */
    byte AC5         :1;                                       /* Acceptance Code Bit 5 */
    byte AC6         :1;                                       /* Acceptance Code Bit 6 */
    byte AC7         :1;                                       /* Acceptance Code Bit 7 */
  } Bits;
} CAN0IDAR5STR;
extern volatile CAN0IDAR5STR _CAN0IDAR5 @(REG_BASE + 0x00000159UL);
#define CAN0IDAR5                       _CAN0IDAR5.Byte
#define CAN0IDAR5_AC0                   _CAN0IDAR5.Bits.AC0
#define CAN0IDAR5_AC1                   _CAN0IDAR5.Bits.AC1
#define CAN0IDAR5_AC2                   _CAN0IDAR5.Bits.AC2
#define CAN0IDAR5_AC3                   _CAN0IDAR5.Bits.AC3
#define CAN0IDAR5_AC4                   _CAN0IDAR5.Bits.AC4
#define CAN0IDAR5_AC5                   _CAN0IDAR5.Bits.AC5
#define CAN0IDAR5_AC6                   _CAN0IDAR5.Bits.AC6
#define CAN0IDAR5_AC7                   _CAN0IDAR5.Bits.AC7

#define CAN0IDAR5_AC0_MASK              1U
#define CAN0IDAR5_AC1_MASK              2U
#define CAN0IDAR5_AC2_MASK              4U
#define CAN0IDAR5_AC3_MASK              8U
#define CAN0IDAR5_AC4_MASK              16U
#define CAN0IDAR5_AC5_MASK              32U
#define CAN0IDAR5_AC6_MASK              64U
#define CAN0IDAR5_AC7_MASK              128U


/*** CAN0IDAR6 - MSCAN 0 Identifier Acceptance Register 6; 0x0000015A ***/
typedef union {
  byte Byte;
  struct {
    byte AC0         :1;                                       /* Acceptance Code Bit 0 */
    byte AC1         :1;                                       /* Acceptance Code Bit 1 */
    byte AC2         :1;                                       /* Acceptance Code Bit 2 */
    byte AC3         :1;                                       /* Acceptance Code Bit 3 */
    byte AC4         :1;                                       /* Acceptance Code Bit 4 */
    byte AC5         :1;                                       /* Acceptance Code Bit 5 */
    byte AC6         :1;                                       /* Acceptance Code Bit 6 */
    byte AC7         :1;                                       /* Acceptance Code Bit 7 */
  } Bits;
} CAN0IDAR6STR;
extern volatile CAN0IDAR6STR _CAN0IDAR6 @(REG_BASE + 0x0000015AUL);
#define CAN0IDAR6                       _CAN0IDAR6.Byte
#define CAN0IDAR6_AC0                   _CAN0IDAR6.Bits.AC0
#define CAN0IDAR6_AC1                   _CAN0IDAR6.Bits.AC1
#define CAN0IDAR6_AC2                   _CAN0IDAR6.Bits.AC2
#define CAN0IDAR6_AC3                   _CAN0IDAR6.Bits.AC3
#define CAN0IDAR6_AC4                   _CAN0IDAR6.Bits.AC4
#define CAN0IDAR6_AC5                   _CAN0IDAR6.Bits.AC5
#define CAN0IDAR6_AC6                   _CAN0IDAR6.Bits.AC6
#define CAN0IDAR6_AC7                   _CAN0IDAR6.Bits.AC7

#define CAN0IDAR6_AC0_MASK              1U
#define CAN0IDAR6_AC1_MASK              2U
#define CAN0IDAR6_AC2_MASK              4U
#define CAN0IDAR6_AC3_MASK              8U
#define CAN0IDAR6_AC4_MASK              16U
#define CAN0IDAR6_AC5_MASK              32U
#define CAN0IDAR6_AC6_MASK              64U
#define CAN0IDAR6_AC7_MASK              128U


/*** CAN0IDAR7 - MSCAN 0 Identifier Acceptance Register 7; 0x0000015B ***/
typedef union {
  byte Byte;
  struct {
    byte AC0         :1;                                       /* Acceptance Code Bit 0 */
    byte AC1         :1;                                       /* Acceptance Code Bit 1 */
    byte AC2         :1;                                       /* Acceptance Code Bit 2 */
    byte AC3         :1;                                       /* Acceptance Code Bit 3 */
    byte AC4         :1;                                       /* Acceptance Code Bit 4 */
    byte AC5         :1;                                       /* Acceptance Code Bit 5 */
    byte AC6         :1;                                       /* Acceptance Code Bit 6 */
    byte AC7         :1;                                       /* Acceptance Code Bit 7 */
  } Bits;
} CAN0IDAR7STR;
extern volatile CAN0IDAR7STR _CAN0IDAR7 @(REG_BASE + 0x0000015BUL);
#define CAN0IDAR7                       _CAN0IDAR7.Byte
#define CAN0IDAR7_AC0                   _CAN0IDAR7.Bits.AC0
#define CAN0IDAR7_AC1                   _CAN0IDAR7.Bits.AC1
#define CAN0IDAR7_AC2                   _CAN0IDAR7.Bits.AC2
#define CAN0IDAR7_AC3                   _CAN0IDAR7.Bits.AC3
#define CAN0IDAR7_AC4                   _CAN0IDAR7.Bits.AC4
#define CAN0IDAR7_AC5                   _CAN0IDAR7.Bits.AC5
#define CAN0IDAR7_AC6                   _CAN0IDAR7.Bits.AC6
#define CAN0IDAR7_AC7                   _CAN0IDAR7.Bits.AC7

#define CAN0IDAR7_AC0_MASK              1U
#define CAN0IDAR7_AC1_MASK              2U
#define CAN0IDAR7_AC2_MASK              4U
#define CAN0IDAR7_AC3_MASK              8U
#define CAN0IDAR7_AC4_MASK              16U
#define CAN0IDAR7_AC5_MASK              32U
#define CAN0IDAR7_AC6_MASK              64U
#define CAN0IDAR7_AC7_MASK              128U


/*** CAN0IDMR4 - MSCAN 0 Identifier Mask Register 4; 0x0000015C ***/
typedef union {
  byte Byte;
  struct {
    byte AM0         :1;                                       /* Acceptance Mask Bit 0 */
    byte AM1         :1;                                       /* Acceptance Mask Bit 1 */
    byte AM2         :1;                                       /* Acceptance Mask Bit 2 */
    byte AM3         :1;                                       /* Acceptance Mask Bit 3 */
    byte AM4         :1;                                       /* Acceptance Mask Bit 4 */
    byte AM5         :1;                                       /* Acceptance Mask Bit 5 */
    byte AM6         :1;                                       /* Acceptance Mask Bit 6 */
    byte AM7         :1;                                       /* Acceptance Mask Bit 7 */
  } Bits;
} CAN0IDMR4STR;
extern volatile CAN0IDMR4STR _CAN0IDMR4 @(REG_BASE + 0x0000015CUL);
#define CAN0IDMR4                       _CAN0IDMR4.Byte
#define CAN0IDMR4_AM0                   _CAN0IDMR4.Bits.AM0
#define CAN0IDMR4_AM1                   _CAN0IDMR4.Bits.AM1
#define CAN0IDMR4_AM2                   _CAN0IDMR4.Bits.AM2
#define CAN0IDMR4_AM3                   _CAN0IDMR4.Bits.AM3
#define CAN0IDMR4_AM4                   _CAN0IDMR4.Bits.AM4
#define CAN0IDMR4_AM5                   _CAN0IDMR4.Bits.AM5
#define CAN0IDMR4_AM6                   _CAN0IDMR4.Bits.AM6
#define CAN0IDMR4_AM7                   _CAN0IDMR4.Bits.AM7

#define CAN0IDMR4_AM0_MASK              1U
#define CAN0IDMR4_AM1_MASK              2U
#define CAN0IDMR4_AM2_MASK              4U
#define CAN0IDMR4_AM3_MASK              8U
#define CAN0IDMR4_AM4_MASK              16U
#define CAN0IDMR4_AM5_MASK              32U
#define CAN0IDMR4_AM6_MASK              64U
#define CAN0IDMR4_AM7_MASK              128U


/*** CAN0IDMR5 - MSCAN 0 Identifier Mask Register 5; 0x0000015D ***/
typedef union {
  byte Byte;
  struct {
    byte AM0         :1;                                       /* Acceptance Mask Bit 0 */
    byte AM1         :1;                                       /* Acceptance Mask Bit 1 */
    byte AM2         :1;                                       /* Acceptance Mask Bit 2 */
    byte AM3         :1;                                       /* Acceptance Mask Bit 3 */
    byte AM4         :1;                                       /* Acceptance Mask Bit 4 */
    byte AM5         :1;                                       /* Acceptance Mask Bit 5 */
    byte AM6         :1;                                       /* Acceptance Mask Bit 6 */
    byte AM7         :1;                                       /* Acceptance Mask Bit 7 */
  } Bits;
} CAN0IDMR5STR;
extern volatile CAN0IDMR5STR _CAN0IDMR5 @(REG_BASE + 0x0000015DUL);
#define CAN0IDMR5                       _CAN0IDMR5.Byte
#define CAN0IDMR5_AM0                   _CAN0IDMR5.Bits.AM0
#define CAN0IDMR5_AM1                   _CAN0IDMR5.Bits.AM1
#define CAN0IDMR5_AM2                   _CAN0IDMR5.Bits.AM2
#define CAN0IDMR5_AM3                   _CAN0IDMR5.Bits.AM3
#define CAN0IDMR5_AM4                   _CAN0IDMR5.Bits.AM4
#define CAN0IDMR5_AM5                   _CAN0IDMR5.Bits.AM5
#define CAN0IDMR5_AM6                   _CAN0IDMR5.Bits.AM6
#define CAN0IDMR5_AM7                   _CAN0IDMR5.Bits.AM7

#define CAN0IDMR5_AM0_MASK              1U
#define CAN0IDMR5_AM1_MASK              2U
#define CAN0IDMR5_AM2_MASK              4U
#define CAN0IDMR5_AM3_MASK              8U
#define CAN0IDMR5_AM4_MASK              16U
#define CAN0IDMR5_AM5_MASK              32U
#define CAN0IDMR5_AM6_MASK              64U
#define CAN0IDMR5_AM7_MASK              128U


/*** CAN0IDMR6 - MSCAN 0 Identifier Mask Register 6; 0x0000015E ***/
typedef union {
  byte Byte;
  struct {
    byte AM0         :1;                                       /* Acceptance Mask Bit 0 */
    byte AM1         :1;                                       /* Acceptance Mask Bit 1 */
    byte AM2         :1;                                       /* Acceptance Mask Bit 2 */
    byte AM3         :1;                                       /* Acceptance Mask Bit 3 */
    byte AM4         :1;                                       /* Acceptance Mask Bit 4 */
    byte AM5         :1;                                       /* Acceptance Mask Bit 5 */
    byte AM6         :1;                                       /* Acceptance Mask Bit 6 */
    byte AM7         :1;                                       /* Acceptance Mask Bit 7 */
  } Bits;
} CAN0IDMR6STR;
extern volatile CAN0IDMR6STR _CAN0IDMR6 @(REG_BASE + 0x0000015EUL);
#define CAN0IDMR6                       _CAN0IDMR6.Byte
#define CAN0IDMR6_AM0                   _CAN0IDMR6.Bits.AM0
#define CAN0IDMR6_AM1                   _CAN0IDMR6.Bits.AM1
#define CAN0IDMR6_AM2                   _CAN0IDMR6.Bits.AM2
#define CAN0IDMR6_AM3                   _CAN0IDMR6.Bits.AM3
#define CAN0IDMR6_AM4                   _CAN0IDMR6.Bits.AM4
#define CAN0IDMR6_AM5                   _CAN0IDMR6.Bits.AM5
#define CAN0IDMR6_AM6                   _CAN0IDMR6.Bits.AM6
#define CAN0IDMR6_AM7                   _CAN0IDMR6.Bits.AM7

#define CAN0IDMR6_AM0_MASK              1U
#define CAN0IDMR6_AM1_MASK              2U
#define CAN0IDMR6_AM2_MASK              4U
#define CAN0IDMR6_AM3_MASK              8U
#define CAN0IDMR6_AM4_MASK              16U
#define CAN0IDMR6_AM5_MASK              32U
#define CAN0IDMR6_AM6_MASK              64U
#define CAN0IDMR6_AM7_MASK              128U


/*** CAN0IDMR7 - MSCAN 0 Identifier Mask Register 7; 0x0000015F ***/
typedef union {
  byte Byte;
  struct {
    byte AM0         :1;                                       /* Acceptance Mask Bit 0 */
    byte AM1         :1;                                       /* Acceptance Mask Bit 1 */
    byte AM2         :1;                                       /* Acceptance Mask Bit 2 */
    byte AM3         :1;                                       /* Acceptance Mask Bit 3 */
    byte AM4         :1;                                       /* Acceptance Mask Bit 4 */
    byte AM5         :1;                                       /* Acceptance Mask Bit 5 */
    byte AM6         :1;                                       /* Acceptance Mask Bit 6 */
    byte AM7         :1;                                       /* Acceptance Mask Bit 7 */
  } Bits;
} CAN0IDMR7STR;
extern volatile CAN0IDMR7STR _CAN0IDMR7 @(REG_BASE + 0x0000015FUL);
#define CAN0IDMR7                       _CAN0IDMR7.Byte
#define CAN0IDMR7_AM0                   _CAN0IDMR7.Bits.AM0
#define CAN0IDMR7_AM1                   _CAN0IDMR7.Bits.AM1
#define CAN0IDMR7_AM2                   _CAN0IDMR7.Bits.AM2
#define CAN0IDMR7_AM3                   _CAN0IDMR7.Bits.AM3
#define CAN0IDMR7_AM4                   _CAN0IDMR7.Bits.AM4
#define CAN0IDMR7_AM5                   _CAN0IDMR7.Bits.AM5
#define CAN0IDMR7_AM6                   _CAN0IDMR7.Bits.AM6
#define CAN0IDMR7_AM7                   _CAN0IDMR7.Bits.AM7

#define CAN0IDMR7_AM0_MASK              1U
#define CAN0IDMR7_AM1_MASK              2U
#define CAN0IDMR7_AM2_MASK              4U
#define CAN0IDMR7_AM3_MASK              8U
#define CAN0IDMR7_AM4_MASK              16U
#define CAN0IDMR7_AM5_MASK              32U
#define CAN0IDMR7_AM6_MASK              64U
#define CAN0IDMR7_AM7_MASK              128U


/*** CAN0RXIDR0 - MSCAN 0 Receive Identifier Register 0; 0x00000160 ***/
typedef union {
  byte Byte;
  struct {
    byte ID21        :1;                                       /* Extended format identifier Bit 21 */
    byte ID22        :1;                                       /* Extended format identifier Bit 22 */
    byte ID23        :1;                                       /* Extended format identifier Bit 23 */
    byte ID24        :1;                                       /* Extended format identifier Bit 24 */
    byte ID25        :1;                                       /* Extended format identifier Bit 25 */
    byte ID26        :1;                                       /* Extended format identifier Bit 26 */
    byte ID27        :1;                                       /* Extended format identifier Bit 27 */
    byte ID28        :1;                                       /* Extended format identifier Bit 28 */
  } Bits;
} CAN0RXIDR0STR;
extern volatile CAN0RXIDR0STR _CAN0RXIDR0 @(REG_BASE + 0x00000160UL);
#define CAN0RXIDR0                      _CAN0RXIDR0.Byte
#define CAN0RXIDR0_ID21                 _CAN0RXIDR0.Bits.ID21
#define CAN0RXIDR0_ID22                 _CAN0RXIDR0.Bits.ID22
#define CAN0RXIDR0_ID23                 _CAN0RXIDR0.Bits.ID23
#define CAN0RXIDR0_ID24                 _CAN0RXIDR0.Bits.ID24
#define CAN0RXIDR0_ID25                 _CAN0RXIDR0.Bits.ID25
#define CAN0RXIDR0_ID26                 _CAN0RXIDR0.Bits.ID26
#define CAN0RXIDR0_ID27                 _CAN0RXIDR0.Bits.ID27
#define CAN0RXIDR0_ID28                 _CAN0RXIDR0.Bits.ID28
/* CAN0RXIDR_ARR: Access 4 CAN0RXIDRx registers in an array */
#define CAN0RXIDR_ARR                   ((volatile byte *) &CAN0RXIDR0)

#define CAN0RXIDR0_ID21_MASK            1U
#define CAN0RXIDR0_ID22_MASK            2U
#define CAN0RXIDR0_ID23_MASK            4U
#define CAN0RXIDR0_ID24_MASK            8U
#define CAN0RXIDR0_ID25_MASK            16U
#define CAN0RXIDR0_ID26_MASK            32U
#define CAN0RXIDR0_ID27_MASK            64U
#define CAN0RXIDR0_ID28_MASK            128U


/*** CAN0RXIDR1 - MSCAN 0 Receive Identifier Register 1; 0x00000161 ***/
typedef union {
  byte Byte;
  struct {
    byte ID15        :1;                                       /* Extended format identifier Bit 15 */
    byte ID16        :1;                                       /* Extended format identifier Bit 16 */
    byte ID17        :1;                                       /* Extended format identifier Bit 17 */
    byte IDE         :1;                                       /* ID Extended */
    byte SRR         :1;                                       /* Substitute Remote Request */
    byte ID18        :1;                                       /* Extended format identifier Bit 18 */
    byte ID19        :1;                                       /* Extended format identifier Bit 19 */
    byte ID20        :1;                                       /* Extended format identifier Bit 20 */
  } Bits;
  struct {
    byte grpID_15 :3;
    byte         :1;
    byte         :1;
    byte grpID_18 :3;
  } MergedBits;
} CAN0RXIDR1STR;
extern volatile CAN0RXIDR1STR _CAN0RXIDR1 @(REG_BASE + 0x00000161UL);
#define CAN0RXIDR1                      _CAN0RXIDR1.Byte
#define CAN0RXIDR1_ID15                 _CAN0RXIDR1.Bits.ID15
#define CAN0RXIDR1_ID16                 _CAN0RXIDR1.Bits.ID16
#define CAN0RXIDR1_ID17                 _CAN0RXIDR1.Bits.ID17
#define CAN0RXIDR1_IDE                  _CAN0RXIDR1.Bits.IDE
#define CAN0RXIDR1_SRR                  _CAN0RXIDR1.Bits.SRR
#define CAN0RXIDR1_ID18                 _CAN0RXIDR1.Bits.ID18
#define CAN0RXIDR1_ID19                 _CAN0RXIDR1.Bits.ID19
#define CAN0RXIDR1_ID20                 _CAN0RXIDR1.Bits.ID20
#define CAN0RXIDR1_ID_15                _CAN0RXIDR1.MergedBits.grpID_15
#define CAN0RXIDR1_ID_18                _CAN0RXIDR1.MergedBits.grpID_18
#define CAN0RXIDR1_ID                   CAN0RXIDR1_ID_15

#define CAN0RXIDR1_ID15_MASK            1U
#define CAN0RXIDR1_ID16_MASK            2U
#define CAN0RXIDR1_ID17_MASK            4U
#define CAN0RXIDR1_IDE_MASK             8U
#define CAN0RXIDR1_SRR_MASK             16U
#define CAN0RXIDR1_ID18_MASK            32U
#define CAN0RXIDR1_ID19_MASK            64U
#define CAN0RXIDR1_ID20_MASK            128U
#define CAN0RXIDR1_ID_15_MASK           7U
#define CAN0RXIDR1_ID_15_BITNUM         0U
#define CAN0RXIDR1_ID_18_MASK           224U
#define CAN0RXIDR1_ID_18_BITNUM         5U


/*** CAN0RXIDR2 - MSCAN 0 Receive Identifier Register 2; 0x00000162 ***/
typedef union {
  byte Byte;
  struct {
    byte ID7         :1;                                       /* Extended format identifier Bit 7 */
    byte ID8         :1;                                       /* Extended format identifier Bit 8 */
    byte ID9         :1;                                       /* Extended format identifier Bit 9 */
    byte ID10        :1;                                       /* Extended format identifier Bit 10 */
    byte ID11        :1;                                       /* Extended format identifier Bit 11 */
    byte ID12        :1;                                       /* Extended format identifier Bit 12 */
    byte ID13        :1;                                       /* Extended format identifier Bit 13 */
    byte ID14        :1;                                       /* Extended format identifier Bit 14 */
  } Bits;
} CAN0RXIDR2STR;
extern volatile CAN0RXIDR2STR _CAN0RXIDR2 @(REG_BASE + 0x00000162UL);
#define CAN0RXIDR2                      _CAN0RXIDR2.Byte
#define CAN0RXIDR2_ID7                  _CAN0RXIDR2.Bits.ID7
#define CAN0RXIDR2_ID8                  _CAN0RXIDR2.Bits.ID8
#define CAN0RXIDR2_ID9                  _CAN0RXIDR2.Bits.ID9
#define CAN0RXIDR2_ID10                 _CAN0RXIDR2.Bits.ID10
#define CAN0RXIDR2_ID11                 _CAN0RXIDR2.Bits.ID11
#define CAN0RXIDR2_ID12                 _CAN0RXIDR2.Bits.ID12
#define CAN0RXIDR2_ID13                 _CAN0RXIDR2.Bits.ID13
#define CAN0RXIDR2_ID14                 _CAN0RXIDR2.Bits.ID14

#define CAN0RXIDR2_ID7_MASK             1U
#define CAN0RXIDR2_ID8_MASK             2U
#define CAN0RXIDR2_ID9_MASK             4U
#define CAN0RXIDR2_ID10_MASK            8U
#define CAN0RXIDR2_ID11_MASK            16U
#define CAN0RXIDR2_ID12_MASK            32U
#define CAN0RXIDR2_ID13_MASK            64U
#define CAN0RXIDR2_ID14_MASK            128U


/*** CAN0RXIDR3 - MSCAN 0 Receive Identifier Register 3; 0x00000163 ***/
typedef union {
  byte Byte;
  struct {
    byte RTR         :1;                                       /* Remote Transmission Request */
    byte ID0         :1;                                       /* Extended format identifier Bit 0 */
    byte ID1         :1;                                       /* Extended format identifier Bit 1 */
    byte ID2         :1;                                       /* Extended format identifier Bit 2 */
    byte ID3         :1;                                       /* Extended format identifier Bit 3 */
    byte ID4         :1;                                       /* Extended format identifier Bit 4 */
    byte ID5         :1;                                       /* Extended format identifier Bit 5 */
    byte ID6         :1;                                       /* Extended format identifier Bit 6 */
  } Bits;
  struct {
    byte         :1;
    byte grpID   :7;
  } MergedBits;
} CAN0RXIDR3STR;
extern volatile CAN0RXIDR3STR _CAN0RXIDR3 @(REG_BASE + 0x00000163UL);
#define CAN0RXIDR3                      _CAN0RXIDR3.Byte
#define CAN0RXIDR3_RTR                  _CAN0RXIDR3.Bits.RTR
#define CAN0RXIDR3_ID0                  _CAN0RXIDR3.Bits.ID0
#define CAN0RXIDR3_ID1                  _CAN0RXIDR3.Bits.ID1
#define CAN0RXIDR3_ID2                  _CAN0RXIDR3.Bits.ID2
#define CAN0RXIDR3_ID3                  _CAN0RXIDR3.Bits.ID3
#define CAN0RXIDR3_ID4                  _CAN0RXIDR3.Bits.ID4
#define CAN0RXIDR3_ID5                  _CAN0RXIDR3.Bits.ID5
#define CAN0RXIDR3_ID6                  _CAN0RXIDR3.Bits.ID6
#define CAN0RXIDR3_ID                   _CAN0RXIDR3.MergedBits.grpID

#define CAN0RXIDR3_RTR_MASK             1U
#define CAN0RXIDR3_ID0_MASK             2U
#define CAN0RXIDR3_ID1_MASK             4U
#define CAN0RXIDR3_ID2_MASK             8U
#define CAN0RXIDR3_ID3_MASK             16U
#define CAN0RXIDR3_ID4_MASK             32U
#define CAN0RXIDR3_ID5_MASK             64U
#define CAN0RXIDR3_ID6_MASK             128U
#define CAN0RXIDR3_ID_MASK              254U
#define CAN0RXIDR3_ID_BITNUM            1U


/*** CAN0RXDSR0 - MSCAN 0 Receive Data Segment Register 0; 0x00000164 ***/
typedef union {
  byte Byte;
  struct {
    byte DB0         :1;                                       /* Data Bit 0 */
    byte DB1         :1;                                       /* Data Bit 1 */
    byte DB2         :1;                                       /* Data Bit 2 */
    byte DB3         :1;                                       /* Data Bit 3 */
    byte DB4         :1;                                       /* Data Bit 4 */
    byte DB5         :1;                                       /* Data Bit 5 */
    byte DB6         :1;                                       /* Data Bit 6 */
    byte DB7         :1;                                       /* Data Bit 7 */
  } Bits;
} CAN0RXDSR0STR;
extern volatile CAN0RXDSR0STR _CAN0RXDSR0 @(REG_BASE + 0x00000164UL);
#define CAN0RXDSR0                      _CAN0RXDSR0.Byte
#define CAN0RXDSR0_DB0                  _CAN0RXDSR0.Bits.DB0
#define CAN0RXDSR0_DB1                  _CAN0RXDSR0.Bits.DB1
#define CAN0RXDSR0_DB2                  _CAN0RXDSR0.Bits.DB2
#define CAN0RXDSR0_DB3                  _CAN0RXDSR0.Bits.DB3
#define CAN0RXDSR0_DB4                  _CAN0RXDSR0.Bits.DB4
#define CAN0RXDSR0_DB5                  _CAN0RXDSR0.Bits.DB5
#define CAN0RXDSR0_DB6                  _CAN0RXDSR0.Bits.DB6
#define CAN0RXDSR0_DB7                  _CAN0RXDSR0.Bits.DB7
/* CAN0RXDSR_ARR: Access 8 CAN0RXDSRx registers in an array */
#define CAN0RXDSR_ARR                   ((volatile byte *) &CAN0RXDSR0)

#define CAN0RXDSR0_DB0_MASK             1U
#define CAN0RXDSR0_DB1_MASK             2U
#define CAN0RXDSR0_DB2_MASK             4U
#define CAN0RXDSR0_DB3_MASK             8U
#define CAN0RXDSR0_DB4_MASK             16U
#define CAN0RXDSR0_DB5_MASK             32U
#define CAN0RXDSR0_DB6_MASK             64U
#define CAN0RXDSR0_DB7_MASK             128U


/*** CAN0RXDSR1 - MSCAN 0 Receive Data Segment Register 1; 0x00000165 ***/
typedef union {
  byte Byte;
  struct {
    byte DB0         :1;                                       /* Data Bit 0 */
    byte DB1         :1;                                       /* Data Bit 1 */
    byte DB2         :1;                                       /* Data Bit 2 */
    byte DB3         :1;                                       /* Data Bit 3 */
    byte DB4         :1;                                       /* Data Bit 4 */
    byte DB5         :1;                                       /* Data Bit 5 */
    byte DB6         :1;                                       /* Data Bit 6 */
    byte DB7         :1;                                       /* Data Bit 7 */
  } Bits;
} CAN0RXDSR1STR;
extern volatile CAN0RXDSR1STR _CAN0RXDSR1 @(REG_BASE + 0x00000165UL);
#define CAN0RXDSR1                      _CAN0RXDSR1.Byte
#define CAN0RXDSR1_DB0                  _CAN0RXDSR1.Bits.DB0
#define CAN0RXDSR1_DB1                  _CAN0RXDSR1.Bits.DB1
#define CAN0RXDSR1_DB2                  _CAN0RXDSR1.Bits.DB2
#define CAN0RXDSR1_DB3                  _CAN0RXDSR1.Bits.DB3
#define CAN0RXDSR1_DB4                  _CAN0RXDSR1.Bits.DB4
#define CAN0RXDSR1_DB5                  _CAN0RXDSR1.Bits.DB5
#define CAN0RXDSR1_DB6                  _CAN0RXDSR1.Bits.DB6
#define CAN0RXDSR1_DB7                  _CAN0RXDSR1.Bits.DB7

#define CAN0RXDSR1_DB0_MASK             1U
#define CAN0RXDSR1_DB1_MASK             2U
#define CAN0RXDSR1_DB2_MASK             4U
#define CAN0RXDSR1_DB3_MASK             8U
#define CAN0RXDSR1_DB4_MASK             16U
#define CAN0RXDSR1_DB5_MASK             32U
#define CAN0RXDSR1_DB6_MASK             64U
#define CAN0RXDSR1_DB7_MASK             128U


/*** CAN0RXDSR2 - MSCAN 0 Receive Data Segment Register 2; 0x00000166 ***/
typedef union {
  byte Byte;
  struct {
    byte DB0         :1;                                       /* Data Bit 0 */
    byte DB1         :1;                                       /* Data Bit 1 */
    byte DB2         :1;                                       /* Data Bit 2 */
    byte DB3         :1;                                       /* Data Bit 3 */
    byte DB4         :1;                                       /* Data Bit 4 */
    byte DB5         :1;                                       /* Data Bit 5 */
    byte DB6         :1;                                       /* Data Bit 6 */
    byte DB7         :1;                                       /* Data Bit 7 */
  } Bits;
} CAN0RXDSR2STR;
extern volatile CAN0RXDSR2STR _CAN0RXDSR2 @(REG_BASE + 0x00000166UL);
#define CAN0RXDSR2                      _CAN0RXDSR2.Byte
#define CAN0RXDSR2_DB0                  _CAN0RXDSR2.Bits.DB0
#define CAN0RXDSR2_DB1                  _CAN0RXDSR2.Bits.DB1
#define CAN0RXDSR2_DB2                  _CAN0RXDSR2.Bits.DB2
#define CAN0RXDSR2_DB3                  _CAN0RXDSR2.Bits.DB3
#define CAN0RXDSR2_DB4                  _CAN0RXDSR2.Bits.DB4
#define CAN0RXDSR2_DB5                  _CAN0RXDSR2.Bits.DB5
#define CAN0RXDSR2_DB6                  _CAN0RXDSR2.Bits.DB6
#define CAN0RXDSR2_DB7                  _CAN0RXDSR2.Bits.DB7

#define CAN0RXDSR2_DB0_MASK             1U
#define CAN0RXDSR2_DB1_MASK             2U
#define CAN0RXDSR2_DB2_MASK             4U
#define CAN0RXDSR2_DB3_MASK             8U
#define CAN0RXDSR2_DB4_MASK             16U
#define CAN0RXDSR2_DB5_MASK             32U
#define CAN0RXDSR2_DB6_MASK             64U
#define CAN0RXDSR2_DB7_MASK             128U


/*** CAN0RXDSR3 - MSCAN 0 Receive Data Segment Register 3; 0x00000167 ***/
typedef union {
  byte Byte;
  struct {
    byte DB0         :1;                                       /* Data Bit 0 */
    byte DB1         :1;                                       /* Data Bit 1 */
    byte DB2         :1;                                       /* Data Bit 2 */
    byte DB3         :1;                                       /* Data Bit 3 */
    byte DB4         :1;                                       /* Data Bit 4 */
    byte DB5         :1;                                       /* Data Bit 5 */
    byte DB6         :1;                                       /* Data Bit 6 */
    byte DB7         :1;                                       /* Data Bit 7 */
  } Bits;
} CAN0RXDSR3STR;
extern volatile CAN0RXDSR3STR _CAN0RXDSR3 @(REG_BASE + 0x00000167UL);
#define CAN0RXDSR3                      _CAN0RXDSR3.Byte
#define CAN0RXDSR3_DB0                  _CAN0RXDSR3.Bits.DB0
#define CAN0RXDSR3_DB1                  _CAN0RXDSR3.Bits.DB1
#define CAN0RXDSR3_DB2                  _CAN0RXDSR3.Bits.DB2
#define CAN0RXDSR3_DB3                  _CAN0RXDSR3.Bits.DB3
#define CAN0RXDSR3_DB4                  _CAN0RXDSR3.Bits.DB4
#define CAN0RXDSR3_DB5                  _CAN0RXDSR3.Bits.DB5
#define CAN0RXDSR3_DB6                  _CAN0RXDSR3.Bits.DB6
#define CAN0RXDSR3_DB7                  _CAN0RXDSR3.Bits.DB7

#define CAN0RXDSR3_DB0_MASK             1U
#define CAN0RXDSR3_DB1_MASK             2U
#define CAN0RXDSR3_DB2_MASK             4U
#define CAN0RXDSR3_DB3_MASK             8U
#define CAN0RXDSR3_DB4_MASK             16U
#define CAN0RXDSR3_DB5_MASK             32U
#define CAN0RXDSR3_DB6_MASK             64U
#define CAN0RXDSR3_DB7_MASK             128U


/*** CAN0RXDSR4 - MSCAN 0 Receive Data Segment Register 4; 0x00000168 ***/
typedef union {
  byte Byte;
  struct {
    byte DB0         :1;                                       /* Data Bit 0 */
    byte DB1         :1;                                       /* Data Bit 1 */
    byte DB2         :1;                                       /* Data Bit 2 */
    byte DB3         :1;                                       /* Data Bit 3 */
    byte DB4         :1;                                       /* Data Bit 4 */
    byte DB5         :1;                                       /* Data Bit 5 */
    byte DB6         :1;                                       /* Data Bit 6 */
    byte DB7         :1;                                       /* Data Bit 7 */
  } Bits;
} CAN0RXDSR4STR;
extern volatile CAN0RXDSR4STR _CAN0RXDSR4 @(REG_BASE + 0x00000168UL);
#define CAN0RXDSR4                      _CAN0RXDSR4.Byte
#define CAN0RXDSR4_DB0                  _CAN0RXDSR4.Bits.DB0
#define CAN0RXDSR4_DB1                  _CAN0RXDSR4.Bits.DB1
#define CAN0RXDSR4_DB2                  _CAN0RXDSR4.Bits.DB2
#define CAN0RXDSR4_DB3                  _CAN0RXDSR4.Bits.DB3
#define CAN0RXDSR4_DB4                  _CAN0RXDSR4.Bits.DB4
#define CAN0RXDSR4_DB5                  _CAN0RXDSR4.Bits.DB5
#define CAN0RXDSR4_DB6                  _CAN0RXDSR4.Bits.DB6
#define CAN0RXDSR4_DB7                  _CAN0RXDSR4.Bits.DB7

#define CAN0RXDSR4_DB0_MASK             1U
#define CAN0RXDSR4_DB1_MASK             2U
#define CAN0RXDSR4_DB2_MASK             4U
#define CAN0RXDSR4_DB3_MASK             8U
#define CAN0RXDSR4_DB4_MASK             16U
#define CAN0RXDSR4_DB5_MASK             32U
#define CAN0RXDSR4_DB6_MASK             64U
#define CAN0RXDSR4_DB7_MASK             128U


/*** CAN0RXDSR5 - MSCAN 0 Receive Data Segment Register 5; 0x00000169 ***/
typedef union {
  byte Byte;
  struct {
    byte DB0         :1;                                       /* Data Bit 0 */
    byte DB1         :1;                                       /* Data Bit 1 */
    byte DB2         :1;                                       /* Data Bit 2 */
    byte DB3         :1;                                       /* Data Bit 3 */
    byte DB4         :1;                                       /* Data Bit 4 */
    byte DB5         :1;                                       /* Data Bit 5 */
    byte DB6         :1;                                       /* Data Bit 6 */
    byte DB7         :1;                                       /* Data Bit 7 */
  } Bits;
} CAN0RXDSR5STR;
extern volatile CAN0RXDSR5STR _CAN0RXDSR5 @(REG_BASE + 0x00000169UL);
#define CAN0RXDSR5                      _CAN0RXDSR5.Byte
#define CAN0RXDSR5_DB0                  _CAN0RXDSR5.Bits.DB0
#define CAN0RXDSR5_DB1                  _CAN0RXDSR5.Bits.DB1
#define CAN0RXDSR5_DB2                  _CAN0RXDSR5.Bits.DB2
#define CAN0RXDSR5_DB3                  _CAN0RXDSR5.Bits.DB3
#define CAN0RXDSR5_DB4                  _CAN0RXDSR5.Bits.DB4
#define CAN0RXDSR5_DB5                  _CAN0RXDSR5.Bits.DB5
#define CAN0RXDSR5_DB6                  _CAN0RXDSR5.Bits.DB6
#define CAN0RXDSR5_DB7                  _CAN0RXDSR5.Bits.DB7

#define CAN0RXDSR5_DB0_MASK             1U
#define CAN0RXDSR5_DB1_MASK             2U
#define CAN0RXDSR5_DB2_MASK             4U
#define CAN0RXDSR5_DB3_MASK             8U
#define CAN0RXDSR5_DB4_MASK             16U
#define CAN0RXDSR5_DB5_MASK             32U
#define CAN0RXDSR5_DB6_MASK             64U
#define CAN0RXDSR5_DB7_MASK             128U


/*** CAN0RXDSR6 - MSCAN 0 Receive Data Segment Register 6; 0x0000016A ***/
typedef union {
  byte Byte;
  struct {
    byte DB0         :1;                                       /* Data Bit 0 */
    byte DB1         :1;                                       /* Data Bit 1 */
    byte DB2         :1;                                       /* Data Bit 2 */
    byte DB3         :1;                                       /* Data Bit 3 */
    byte DB4         :1;                                       /* Data Bit 4 */
    byte DB5         :1;                                       /* Data Bit 5 */
    byte DB6         :1;                                       /* Data Bit 6 */
    byte DB7         :1;                                       /* Data Bit 7 */
  } Bits;
} CAN0RXDSR6STR;
extern volatile CAN0RXDSR6STR _CAN0RXDSR6 @(REG_BASE + 0x0000016AUL);
#define CAN0RXDSR6                      _CAN0RXDSR6.Byte
#define CAN0RXDSR6_DB0                  _CAN0RXDSR6.Bits.DB0
#define CAN0RXDSR6_DB1                  _CAN0RXDSR6.Bits.DB1
#define CAN0RXDSR6_DB2                  _CAN0RXDSR6.Bits.DB2
#define CAN0RXDSR6_DB3                  _CAN0RXDSR6.Bits.DB3
#define CAN0RXDSR6_DB4                  _CAN0RXDSR6.Bits.DB4
#define CAN0RXDSR6_DB5                  _CAN0RXDSR6.Bits.DB5
#define CAN0RXDSR6_DB6                  _CAN0RXDSR6.Bits.DB6
#define CAN0RXDSR6_DB7                  _CAN0RXDSR6.Bits.DB7

#define CAN0RXDSR6_DB0_MASK             1U
#define CAN0RXDSR6_DB1_MASK             2U
#define CAN0RXDSR6_DB2_MASK             4U
#define CAN0RXDSR6_DB3_MASK             8U
#define CAN0RXDSR6_DB4_MASK             16U
#define CAN0RXDSR6_DB5_MASK             32U
#define CAN0RXDSR6_DB6_MASK             64U
#define CAN0RXDSR6_DB7_MASK             128U


/*** CAN0RXDSR7 - MSCAN 0 Receive Data Segment Register 7; 0x0000016B ***/
typedef union {
  byte Byte;
  struct {
    byte DB0         :1;                                       /* Data Bit 0 */
    byte DB1         :1;                                       /* Data Bit 1 */
    byte DB2         :1;                                       /* Data Bit 2 */
    byte DB3         :1;                                       /* Data Bit 3 */
    byte DB4         :1;                                       /* Data Bit 4 */
    byte DB5         :1;                                       /* Data Bit 5 */
    byte DB6         :1;                                       /* Data Bit 6 */
    byte DB7         :1;                                       /* Data Bit 7 */
  } Bits;
} CAN0RXDSR7STR;
extern volatile CAN0RXDSR7STR _CAN0RXDSR7 @(REG_BASE + 0x0000016BUL);
#define CAN0RXDSR7                      _CAN0RXDSR7.Byte
#define CAN0RXDSR7_DB0                  _CAN0RXDSR7.Bits.DB0
#define CAN0RXDSR7_DB1                  _CAN0RXDSR7.Bits.DB1
#define CAN0RXDSR7_DB2                  _CAN0RXDSR7.Bits.DB2
#define CAN0RXDSR7_DB3                  _CAN0RXDSR7.Bits.DB3
#define CAN0RXDSR7_DB4                  _CAN0RXDSR7.Bits.DB4
#define CAN0RXDSR7_DB5                  _CAN0RXDSR7.Bits.DB5
#define CAN0RXDSR7_DB6                  _CAN0RXDSR7.Bits.DB6
#define CAN0RXDSR7_DB7                  _CAN0RXDSR7.Bits.DB7

#define CAN0RXDSR7_DB0_MASK             1U
#define CAN0RXDSR7_DB1_MASK             2U
#define CAN0RXDSR7_DB2_MASK             4U
#define CAN0RXDSR7_DB3_MASK             8U
#define CAN0RXDSR7_DB4_MASK             16U
#define CAN0RXDSR7_DB5_MASK             32U
#define CAN0RXDSR7_DB6_MASK             64U
#define CAN0RXDSR7_DB7_MASK             128U


/*** CAN0RXDLR - MSCAN 0 Receive Data Length Register; 0x0000016C ***/
typedef union {
  byte Byte;
  struct {
    byte DLC0        :1;                                       /* Data Length Code Bit 0 */
    byte DLC1        :1;                                       /* Data Length Code Bit 1 */
    byte DLC2        :1;                                       /* Data Length Code Bit 2 */
    byte DLC3        :1;                                       /* Data Length Code Bit 3 */
    byte             :1; 
    byte             :1; 
    byte             :1; 
    byte             :1; 
  } Bits;
  struct {
    byte grpDLC  :4;
    byte         :1;
    byte         :1;
    byte         :1;
    byte         :1;
  } MergedBits;
} CAN0RXDLRSTR;
extern volatile CAN0RXDLRSTR _CAN0RXDLR @(REG_BASE + 0x0000016CUL);
#define CAN0RXDLR                       _CAN0RXDLR.Byte
#define CAN0RXDLR_DLC0                  _CAN0RXDLR.Bits.DLC0
#define CAN0RXDLR_DLC1                  _CAN0RXDLR.Bits.DLC1
#define CAN0RXDLR_DLC2                  _CAN0RXDLR.Bits.DLC2
#define CAN0RXDLR_DLC3                  _CAN0RXDLR.Bits.DLC3
#define CAN0RXDLR_DLC                   _CAN0RXDLR.MergedBits.grpDLC

#define CAN0RXDLR_DLC0_MASK             1U
#define CAN0RXDLR_DLC1_MASK             2U
#define CAN0RXDLR_DLC2_MASK             4U
#define CAN0RXDLR_DLC3_MASK             8U
#define CAN0RXDLR_DLC_MASK              15U
#define CAN0RXDLR_DLC_BITNUM            0U


/*** CAN0RXTSR - MSCAN 0 Receive Time Stamp Register; 0x0000016E ***/
typedef union {
  word Word;
   /* Overlapped registers: */
  struct {
    /*** CAN0RXTSRH - MSCAN 0 Receive Time Stamp Register High; 0x0000016E ***/
    union {
      byte Byte;
      struct {
        byte TSR8        :1;                                       /* Time Stamp Bit 8 */
        byte TSR9        :1;                                       /* Time Stamp Bit 9 */
        byte TSR10       :1;                                       /* Time Stamp Bit 10 */
        byte TSR11       :1;                                       /* Time Stamp Bit 11 */
        byte TSR12       :1;                                       /* Time Stamp Bit 12 */
        byte TSR13       :1;                                       /* Time Stamp Bit 13 */
        byte TSR14       :1;                                       /* Time Stamp Bit 14 */
        byte TSR15       :1;                                       /* Time Stamp Bit 15 */
      } Bits;
    } CAN0RXTSRHSTR;
    #define CAN0RXTSRH                  _CAN0RXTSR.Overlap_STR.CAN0RXTSRHSTR.Byte
    #define CAN0RXTSRH_TSR8             _CAN0RXTSR.Overlap_STR.CAN0RXTSRHSTR.Bits.TSR8
    #define CAN0RXTSRH_TSR9             _CAN0RXTSR.Overlap_STR.CAN0RXTSRHSTR.Bits.TSR9
    #define CAN0RXTSRH_TSR10            _CAN0RXTSR.Overlap_STR.CAN0RXTSRHSTR.Bits.TSR10
    #define CAN0RXTSRH_TSR11            _CAN0RXTSR.Overlap_STR.CAN0RXTSRHSTR.Bits.TSR11
    #define CAN0RXTSRH_TSR12            _CAN0RXTSR.Overlap_STR.CAN0RXTSRHSTR.Bits.TSR12
    #define CAN0RXTSRH_TSR13            _CAN0RXTSR.Overlap_STR.CAN0RXTSRHSTR.Bits.TSR13
    #define CAN0RXTSRH_TSR14            _CAN0RXTSR.Overlap_STR.CAN0RXTSRHSTR.Bits.TSR14
    #define CAN0RXTSRH_TSR15            _CAN0RXTSR.Overlap_STR.CAN0RXTSRHSTR.Bits.TSR15
    
    #define CAN0RXTSRH_TSR8_MASK        1U
    #define CAN0RXTSRH_TSR9_MASK        2U
    #define CAN0RXTSRH_TSR10_MASK       4U
    #define CAN0RXTSRH_TSR11_MASK       8U
    #define CAN0RXTSRH_TSR12_MASK       16U
    #define CAN0RXTSRH_TSR13_MASK       32U
    #define CAN0RXTSRH_TSR14_MASK       64U
    #define CAN0RXTSRH_TSR15_MASK       128U
    

    /*** CAN0RXTSRL - MSCAN 0 Receive Time Stamp Register Low; 0x0000016F ***/
    union {
      byte Byte;
      struct {
        byte TSR0        :1;                                       /* Time Stamp Bit 0 */
        byte TSR1        :1;                                       /* Time Stamp Bit 1 */
        byte TSR2        :1;                                       /* Time Stamp Bit 2 */
        byte TSR3        :1;                                       /* Time Stamp Bit 3 */
        byte TSR4        :1;                                       /* Time Stamp Bit 4 */
        byte TSR5        :1;                                       /* Time Stamp Bit 5 */
        byte TSR6        :1;                                       /* Time Stamp Bit 6 */
        byte TSR7        :1;                                       /* Time Stamp Bit 7 */
      } Bits;
    } CAN0RXTSRLSTR;
    #define CAN0RXTSRL                  _CAN0RXTSR.Overlap_STR.CAN0RXTSRLSTR.Byte
    #define CAN0RXTSRL_TSR0             _CAN0RXTSR.Overlap_STR.CAN0RXTSRLSTR.Bits.TSR0
    #define CAN0RXTSRL_TSR1             _CAN0RXTSR.Overlap_STR.CAN0RXTSRLSTR.Bits.TSR1
    #define CAN0RXTSRL_TSR2             _CAN0RXTSR.Overlap_STR.CAN0RXTSRLSTR.Bits.TSR2
    #define CAN0RXTSRL_TSR3             _CAN0RXTSR.Overlap_STR.CAN0RXTSRLSTR.Bits.TSR3
    #define CAN0RXTSRL_TSR4             _CAN0RXTSR.Overlap_STR.CAN0RXTSRLSTR.Bits.TSR4
    #define CAN0RXTSRL_TSR5             _CAN0RXTSR.Overlap_STR.CAN0RXTSRLSTR.Bits.TSR5
    #define CAN0RXTSRL_TSR6             _CAN0RXTSR.Overlap_STR.CAN0RXTSRLSTR.Bits.TSR6
    #define CAN0RXTSRL_TSR7             _CAN0RXTSR.Overlap_STR.CAN0RXTSRLSTR.Bits.TSR7
    
    #define CAN0RXTSRL_TSR0_MASK        1U
    #define CAN0RXTSRL_TSR1_MASK        2U
    #define CAN0RXTSRL_TSR2_MASK        4U
    #define CAN0RXTSRL_TSR3_MASK        8U
    #define CAN0RXTSRL_TSR4_MASK        16U
    #define CAN0RXTSRL_TSR5_MASK        32U
    #define CAN0RXTSRL_TSR6_MASK        64U
    #define CAN0RXTSRL_TSR7_MASK        128U
    
  } Overlap_STR;

  struct {
    word TSR0        :1;                                       /* Time Stamp Bit 0 */
    word TSR1        :1;                                       /* Time Stamp Bit 1 */
    word TSR2        :1;                                       /* Time Stamp Bit 2 */
    word TSR3        :1;                                       /* Time Stamp Bit 3 */
    word TSR4        :1;                                       /* Time Stamp Bit 4 */
    word TSR5        :1;                                       /* Time Stamp Bit 5 */
    word TSR6        :1;                                       /* Time Stamp Bit 6 */
    word TSR7        :1;                                       /* Time Stamp Bit 7 */
    word TSR8        :1;                                       /* Time Stamp Bit 8 */
    word TSR9        :1;                                       /* Time Stamp Bit 9 */
    word TSR10       :1;                                       /* Time Stamp Bit 10 */
    word TSR11       :1;                                       /* Time Stamp Bit 11 */
    word TSR12       :1;                                       /* Time Stamp Bit 12 */
    word TSR13       :1;                                       /* Time Stamp Bit 13 */
    word TSR14       :1;                                       /* Time Stamp Bit 14 */
    word TSR15       :1;                                       /* Time Stamp Bit 15 */
  } Bits;
} CAN0RXTSRSTR;
extern volatile CAN0RXTSRSTR _CAN0RXTSR @(REG_BASE + 0x0000016EUL);
#define CAN0RXTSR                       _CAN0RXTSR.Word
#define CAN0RXTSR_TSR0                  _CAN0RXTSR.Bits.TSR0
#define CAN0RXTSR_TSR1                  _CAN0RXTSR.Bits.TSR1
#define CAN0RXTSR_TSR2                  _CAN0RXTSR.Bits.TSR2
#define CAN0RXTSR_TSR3                  _CAN0RXTSR.Bits.TSR3
#define CAN0RXTSR_TSR4                  _CAN0RXTSR.Bits.TSR4
#define CAN0RXTSR_TSR5                  _CAN0RXTSR.Bits.TSR5
#define CAN0RXTSR_TSR6                  _CAN0RXTSR.Bits.TSR6
#define CAN0RXTSR_TSR7                  _CAN0RXTSR.Bits.TSR7
#define CAN0RXTSR_TSR8                  _CAN0RXTSR.Bits.TSR8
#define CAN0RXTSR_TSR9                  _CAN0RXTSR.Bits.TSR9
#define CAN0RXTSR_TSR10                 _CAN0RXTSR.Bits.TSR10
#define CAN0RXTSR_TSR11                 _CAN0RXTSR.Bits.TSR11
#define CAN0RXTSR_TSR12                 _CAN0RXTSR.Bits.TSR12
#define CAN0RXTSR_TSR13                 _CAN0RXTSR.Bits.TSR13
#define CAN0RXTSR_TSR14                 _CAN0RXTSR.Bits.TSR14
#define CAN0RXTSR_TSR15                 _CAN0RXTSR.Bits.TSR15

#define CAN0RXTSR_TSR0_MASK             1U
#define CAN0RXTSR_TSR1_MASK             2U
#define CAN0RXTSR_TSR2_MASK             4U
#define CAN0RXTSR_TSR3_MASK             8U
#define CAN0RXTSR_TSR4_MASK             16U
#define CAN0RXTSR_TSR5_MASK             32U
#define CAN0RXTSR_TSR6_MASK             64U
#define CAN0RXTSR_TSR7_MASK             128U
#define CAN0RXTSR_TSR8_MASK             256U
#define CAN0RXTSR_TSR9_MASK             512U
#define CAN0RXTSR_TSR10_MASK            1024U
#define CAN0RXTSR_TSR11_MASK            2048U
#define CAN0RXTSR_TSR12_MASK            4096U
#define CAN0RXTSR_TSR13_MASK            8192U
#define CAN0RXTSR_TSR14_MASK            16384U
#define CAN0RXTSR_TSR15_MASK            32768U


/*** CAN0TXIDR0 - MSCAN 0 Transmit Identifier Register 0; 0x00000170 ***/
typedef union {
  byte Byte;
  struct {
    byte ID21        :1;                                       /* Extended format identifier Bit 21 */
    byte ID22        :1;                                       /* Extended format identifier Bit 22 */
    byte ID23        :1;                                       /* Extended format identifier Bit 23 */
    byte ID24        :1;                                       /* Extended format identifier Bit 24 */
    byte ID25        :1;                                       /* Extended format identifier Bit 25 */
    byte ID26        :1;                                       /* Extended format identifier Bit 26 */
    byte ID27        :1;                                       /* Extended format identifier Bit 27 */
    byte ID28        :1;                                       /* Extended format identifier Bit 28 */
  } Bits;
} CAN0TXIDR0STR;
extern volatile CAN0TXIDR0STR _CAN0TXIDR0 @(REG_BASE + 0x00000170UL);
#define CAN0TXIDR0                      _CAN0TXIDR0.Byte
#define CAN0TXIDR0_ID21                 _CAN0TXIDR0.Bits.ID21
#define CAN0TXIDR0_ID22                 _CAN0TXIDR0.Bits.ID22
#define CAN0TXIDR0_ID23                 _CAN0TXIDR0.Bits.ID23
#define CAN0TXIDR0_ID24                 _CAN0TXIDR0.Bits.ID24
#define CAN0TXIDR0_ID25                 _CAN0TXIDR0.Bits.ID25
#define CAN0TXIDR0_ID26                 _CAN0TXIDR0.Bits.ID26
#define CAN0TXIDR0_ID27                 _CAN0TXIDR0.Bits.ID27
#define CAN0TXIDR0_ID28                 _CAN0TXIDR0.Bits.ID28
/* CAN0TXIDR_ARR: Access 4 CAN0TXIDRx registers in an array */
#define CAN0TXIDR_ARR                   ((volatile byte *) &CAN0TXIDR0)

#define CAN0TXIDR0_ID21_MASK            1U
#define CAN0TXIDR0_ID22_MASK            2U
#define CAN0TXIDR0_ID23_MASK            4U
#define CAN0TXIDR0_ID24_MASK            8U
#define CAN0TXIDR0_ID25_MASK            16U
#define CAN0TXIDR0_ID26_MASK            32U
#define CAN0TXIDR0_ID27_MASK            64U
#define CAN0TXIDR0_ID28_MASK            128U


/*** CAN0TXIDR1 - MSCAN 0 Transmit Identifier Register 1; 0x00000171 ***/
typedef union {
  byte Byte;
  struct {
    byte ID15        :1;                                       /* Extended format identifier Bit 15 */
    byte ID16        :1;                                       /* Extended format identifier Bit 16 */
    byte ID17        :1;                                       /* Extended format identifier Bit 17 */
    byte IDE         :1;                                       /* ID Extended */
    byte SRR         :1;                                       /* Substitute Remote Request */
    byte ID18        :1;                                       /* Extended format identifier Bit 18 */
    byte ID19        :1;                                       /* Extended format identifier Bit 19 */
    byte ID20        :1;                                       /* Extended format identifier Bit 20 */
  } Bits;
  struct {
    byte grpID_15 :3;
    byte         :1;
    byte         :1;
    byte grpID_18 :3;
  } MergedBits;
} CAN0TXIDR1STR;
extern volatile CAN0TXIDR1STR _CAN0TXIDR1 @(REG_BASE + 0x00000171UL);
#define CAN0TXIDR1                      _CAN0TXIDR1.Byte
#define CAN0TXIDR1_ID15                 _CAN0TXIDR1.Bits.ID15
#define CAN0TXIDR1_ID16                 _CAN0TXIDR1.Bits.ID16
#define CAN0TXIDR1_ID17                 _CAN0TXIDR1.Bits.ID17
#define CAN0TXIDR1_IDE                  _CAN0TXIDR1.Bits.IDE
#define CAN0TXIDR1_SRR                  _CAN0TXIDR1.Bits.SRR
#define CAN0TXIDR1_ID18                 _CAN0TXIDR1.Bits.ID18
#define CAN0TXIDR1_ID19                 _CAN0TXIDR1.Bits.ID19
#define CAN0TXIDR1_ID20                 _CAN0TXIDR1.Bits.ID20
#define CAN0TXIDR1_ID_15                _CAN0TXIDR1.MergedBits.grpID_15
#define CAN0TXIDR1_ID_18                _CAN0TXIDR1.MergedBits.grpID_18
#define CAN0TXIDR1_ID                   CAN0TXIDR1_ID_15

#define CAN0TXIDR1_ID15_MASK            1U
#define CAN0TXIDR1_ID16_MASK            2U
#define CAN0TXIDR1_ID17_MASK            4U
#define CAN0TXIDR1_IDE_MASK             8U
#define CAN0TXIDR1_SRR_MASK             16U
#define CAN0TXIDR1_ID18_MASK            32U
#define CAN0TXIDR1_ID19_MASK            64U
#define CAN0TXIDR1_ID20_MASK            128U
#define CAN0TXIDR1_ID_15_MASK           7U
#define CAN0TXIDR1_ID_15_BITNUM         0U
#define CAN0TXIDR1_ID_18_MASK           224U
#define CAN0TXIDR1_ID_18_BITNUM         5U


/*** CAN0TXIDR2 - MSCAN 0 Transmit Identifier Register 2; 0x00000172 ***/
typedef union {
  byte Byte;
  struct {
    byte ID7         :1;                                       /* Extended format identifier Bit 7 */
    byte ID8         :1;                                       /* Extended format identifier Bit 8 */
    byte ID9         :1;                                       /* Extended format identifier Bit 9 */
    byte ID10        :1;                                       /* Extended format identifier Bit 10 */
    byte ID11        :1;                                       /* Extended format identifier Bit 11 */
    byte ID12        :1;                                       /* Extended format identifier Bit 12 */
    byte ID13        :1;                                       /* Extended format identifier Bit 13 */
    byte ID14        :1;                                       /* Extended format identifier Bit 14 */
  } Bits;
} CAN0TXIDR2STR;
extern volatile CAN0TXIDR2STR _CAN0TXIDR2 @(REG_BASE + 0x00000172UL);
#define CAN0TXIDR2                      _CAN0TXIDR2.Byte
#define CAN0TXIDR2_ID7                  _CAN0TXIDR2.Bits.ID7
#define CAN0TXIDR2_ID8                  _CAN0TXIDR2.Bits.ID8
#define CAN0TXIDR2_ID9                  _CAN0TXIDR2.Bits.ID9
#define CAN0TXIDR2_ID10                 _CAN0TXIDR2.Bits.ID10
#define CAN0TXIDR2_ID11                 _CAN0TXIDR2.Bits.ID11
#define CAN0TXIDR2_ID12                 _CAN0TXIDR2.Bits.ID12
#define CAN0TXIDR2_ID13                 _CAN0TXIDR2.Bits.ID13
#define CAN0TXIDR2_ID14                 _CAN0TXIDR2.Bits.ID14

#define CAN0TXIDR2_ID7_MASK             1U
#define CAN0TXIDR2_ID8_MASK             2U
#define CAN0TXIDR2_ID9_MASK             4U
#define CAN0TXIDR2_ID10_MASK            8U
#define CAN0TXIDR2_ID11_MASK            16U
#define CAN0TXIDR2_ID12_MASK            32U
#define CAN0TXIDR2_ID13_MASK            64U
#define CAN0TXIDR2_ID14_MASK            128U


/*** CAN0TXIDR3 - MSCAN 0 Transmit Identifier Register 3; 0x00000173 ***/
typedef union {
  byte Byte;
  struct {
    byte RTR         :1;                                       /* Remote Transmission Request */
    byte ID0         :1;                                       /* Extended format identifier Bit 0 */
    byte ID1         :1;                                       /* Extended format identifier Bit 1 */
    byte ID2         :1;                                       /* Extended format identifier Bit 2 */
    byte ID3         :1;                                       /* Extended format identifier Bit 3 */
    byte ID4         :1;                                       /* Extended format identifier Bit 4 */
    byte ID5         :1;                                       /* Extended format identifier Bit 5 */
    byte ID6         :1;                                       /* Extended format identifier Bit 6 */
  } Bits;
  struct {
    byte         :1;
    byte grpID   :7;
  } MergedBits;
} CAN0TXIDR3STR;
extern volatile CAN0TXIDR3STR _CAN0TXIDR3 @(REG_BASE + 0x00000173UL);
#define CAN0TXIDR3                      _CAN0TXIDR3.Byte
#define CAN0TXIDR3_RTR                  _CAN0TXIDR3.Bits.RTR
#define CAN0TXIDR3_ID0                  _CAN0TXIDR3.Bits.ID0
#define CAN0TXIDR3_ID1                  _CAN0TXIDR3.Bits.ID1
#define CAN0TXIDR3_ID2                  _CAN0TXIDR3.Bits.ID2
#define CAN0TXIDR3_ID3                  _CAN0TXIDR3.Bits.ID3
#define CAN0TXIDR3_ID4                  _CAN0TXIDR3.Bits.ID4
#define CAN0TXIDR3_ID5                  _CAN0TXIDR3.Bits.ID5
#define CAN0TXIDR3_ID6                  _CAN0TXIDR3.Bits.ID6
#define CAN0TXIDR3_ID                   _CAN0TXIDR3.MergedBits.grpID

#define CAN0TXIDR3_RTR_MASK             1U
#define CAN0TXIDR3_ID0_MASK             2U
#define CAN0TXIDR3_ID1_MASK             4U
#define CAN0TXIDR3_ID2_MASK             8U
#define CAN0TXIDR3_ID3_MASK             16U
#define CAN0TXIDR3_ID4_MASK             32U
#define CAN0TXIDR3_ID5_MASK             64U
#define CAN0TXIDR3_ID6_MASK             128U
#define CAN0TXIDR3_ID_MASK              254U
#define CAN0TXIDR3_ID_BITNUM            1U


/*** CAN0TXDSR0 - MSCAN 0 Transmit Data Segment Register 0; 0x00000174 ***/
typedef union {
  byte Byte;
  struct {
    byte DB0         :1;                                       /* Data Bit 0 */
    byte DB1         :1;                                       /* Data Bit 1 */
    byte DB2         :1;                                       /* Data Bit 2 */
    byte DB3         :1;                                       /* Data Bit 3 */
    byte DB4         :1;                                       /* Data Bit 4 */
    byte DB5         :1;                                       /* Data Bit 5 */
    byte DB6         :1;                                       /* Data Bit 6 */
    byte DB7         :1;                                       /* Data Bit 7 */
  } Bits;
} CAN0TXDSR0STR;
extern volatile CAN0TXDSR0STR _CAN0TXDSR0 @(REG_BASE + 0x00000174UL);
#define CAN0TXDSR0                      _CAN0TXDSR0.Byte
#define CAN0TXDSR0_DB0                  _CAN0TXDSR0.Bits.DB0
#define CAN0TXDSR0_DB1                  _CAN0TXDSR0.Bits.DB1
#define CAN0TXDSR0_DB2                  _CAN0TXDSR0.Bits.DB2
#define CAN0TXDSR0_DB3                  _CAN0TXDSR0.Bits.DB3
#define CAN0TXDSR0_DB4                  _CAN0TXDSR0.Bits.DB4
#define CAN0TXDSR0_DB5                  _CAN0TXDSR0.Bits.DB5
#define CAN0TXDSR0_DB6                  _CAN0TXDSR0.Bits.DB6
#define CAN0TXDSR0_DB7                  _CAN0TXDSR0.Bits.DB7
/* CAN0TXDSR_ARR: Access 8 CAN0TXDSRx registers in an array */
#define CAN0TXDSR_ARR                   ((volatile byte *) &CAN0TXDSR0)

#define CAN0TXDSR0_DB0_MASK             1U
#define CAN0TXDSR0_DB1_MASK             2U
#define CAN0TXDSR0_DB2_MASK             4U
#define CAN0TXDSR0_DB3_MASK             8U
#define CAN0TXDSR0_DB4_MASK             16U
#define CAN0TXDSR0_DB5_MASK             32U
#define CAN0TXDSR0_DB6_MASK             64U
#define CAN0TXDSR0_DB7_MASK             128U


/*** CAN0TXDSR1 - MSCAN 0 Transmit Data Segment Register 1; 0x00000175 ***/
typedef union {
  byte Byte;
  struct {
    byte DB0         :1;                                       /* Data Bit 0 */
    byte DB1         :1;                                       /* Data Bit 1 */
    byte DB2         :1;                                       /* Data Bit 2 */
    byte DB3         :1;                                       /* Data Bit 3 */
    byte DB4         :1;                                       /* Data Bit 4 */
    byte DB5         :1;                                       /* Data Bit 5 */
    byte DB6         :1;                                       /* Data Bit 6 */
    byte DB7         :1;                                       /* Data Bit 7 */
  } Bits;
} CAN0TXDSR1STR;
extern volatile CAN0TXDSR1STR _CAN0TXDSR1 @(REG_BASE + 0x00000175UL);
#define CAN0TXDSR1                      _CAN0TXDSR1.Byte
#define CAN0TXDSR1_DB0                  _CAN0TXDSR1.Bits.DB0
#define CAN0TXDSR1_DB1                  _CAN0TXDSR1.Bits.DB1
#define CAN0TXDSR1_DB2                  _CAN0TXDSR1.Bits.DB2
#define CAN0TXDSR1_DB3                  _CAN0TXDSR1.Bits.DB3
#define CAN0TXDSR1_DB4                  _CAN0TXDSR1.Bits.DB4
#define CAN0TXDSR1_DB5                  _CAN0TXDSR1.Bits.DB5
#define CAN0TXDSR1_DB6                  _CAN0TXDSR1.Bits.DB6
#define CAN0TXDSR1_DB7                  _CAN0TXDSR1.Bits.DB7

#define CAN0TXDSR1_DB0_MASK             1U
#define CAN0TXDSR1_DB1_MASK             2U
#define CAN0TXDSR1_DB2_MASK             4U
#define CAN0TXDSR1_DB3_MASK             8U
#define CAN0TXDSR1_DB4_MASK             16U
#define CAN0TXDSR1_DB5_MASK             32U
#define CAN0TXDSR1_DB6_MASK             64U
#define CAN0TXDSR1_DB7_MASK             128U


/*** CAN0TXDSR2 - MSCAN 0 Transmit Data Segment Register 2; 0x00000176 ***/
typedef union {
  byte Byte;
  struct {
    byte DB0         :1;                                       /* Data Bit 0 */
    byte DB1         :1;                                       /* Data Bit 1 */
    byte DB2         :1;                                       /* Data Bit 2 */
    byte DB3         :1;                                       /* Data Bit 3 */
    byte DB4         :1;                                       /* Data Bit 4 */
    byte DB5         :1;                                       /* Data Bit 5 */
    byte DB6         :1;                                       /* Data Bit 6 */
    byte DB7         :1;                                       /* Data Bit 7 */
  } Bits;
} CAN0TXDSR2STR;
extern volatile CAN0TXDSR2STR _CAN0TXDSR2 @(REG_BASE + 0x00000176UL);
#define CAN0TXDSR2                      _CAN0TXDSR2.Byte
#define CAN0TXDSR2_DB0                  _CAN0TXDSR2.Bits.DB0
#define CAN0TXDSR2_DB1                  _CAN0TXDSR2.Bits.DB1
#define CAN0TXDSR2_DB2                  _CAN0TXDSR2.Bits.DB2
#define CAN0TXDSR2_DB3                  _CAN0TXDSR2.Bits.DB3
#define CAN0TXDSR2_DB4                  _CAN0TXDSR2.Bits.DB4
#define CAN0TXDSR2_DB5                  _CAN0TXDSR2.Bits.DB5
#define CAN0TXDSR2_DB6                  _CAN0TXDSR2.Bits.DB6
#define CAN0TXDSR2_DB7                  _CAN0TXDSR2.Bits.DB7

#define CAN0TXDSR2_DB0_MASK             1U
#define CAN0TXDSR2_DB1_MASK             2U
#define CAN0TXDSR2_DB2_MASK             4U
#define CAN0TXDSR2_DB3_MASK             8U
#define CAN0TXDSR2_DB4_MASK             16U
#define CAN0TXDSR2_DB5_MASK             32U
#define CAN0TXDSR2_DB6_MASK             64U
#define CAN0TXDSR2_DB7_MASK             128U


/*** CAN0TXDSR3 - MSCAN 0 Transmit Data Segment Register 3; 0x00000177 ***/
typedef union {
  byte Byte;
  struct {
    byte DB0         :1;                                       /* Data Bit 0 */
    byte DB1         :1;                                       /* Data Bit 1 */
    byte DB2         :1;                                       /* Data Bit 2 */
    byte DB3         :1;                                       /* Data Bit 3 */
    byte DB4         :1;                                       /* Data Bit 4 */
    byte DB5         :1;                                       /* Data Bit 5 */
    byte DB6         :1;                                       /* Data Bit 6 */
    byte DB7         :1;                                       /* Data Bit 7 */
  } Bits;
} CAN0TXDSR3STR;
extern volatile CAN0TXDSR3STR _CAN0TXDSR3 @(REG_BASE + 0x00000177UL);
#define CAN0TXDSR3                      _CAN0TXDSR3.Byte
#define CAN0TXDSR3_DB0                  _CAN0TXDSR3.Bits.DB0
#define CAN0TXDSR3_DB1                  _CAN0TXDSR3.Bits.DB1
#define CAN0TXDSR3_DB2                  _CAN0TXDSR3.Bits.DB2
#define CAN0TXDSR3_DB3                  _CAN0TXDSR3.Bits.DB3
#define CAN0TXDSR3_DB4                  _CAN0TXDSR3.Bits.DB4
#define CAN0TXDSR3_DB5                  _CAN0TXDSR3.Bits.DB5
#define CAN0TXDSR3_DB6                  _CAN0TXDSR3.Bits.DB6
#define CAN0TXDSR3_DB7                  _CAN0TXDSR3.Bits.DB7

#define CAN0TXDSR3_DB0_MASK             1U
#define CAN0TXDSR3_DB1_MASK             2U
#define CAN0TXDSR3_DB2_MASK             4U
#define CAN0TXDSR3_DB3_MASK             8U
#define CAN0TXDSR3_DB4_MASK             16U
#define CAN0TXDSR3_DB5_MASK             32U
#define CAN0TXDSR3_DB6_MASK             64U
#define CAN0TXDSR3_DB7_MASK             128U


/*** CAN0TXDSR4 - MSCAN 0 Transmit Data Segment Register 4; 0x00000178 ***/
typedef union {
  byte Byte;
  struct {
    byte DB0         :1;                                       /* Data Bit 0 */
    byte DB1         :1;                                       /* Data Bit 1 */
    byte DB2         :1;                                       /* Data Bit 2 */
    byte DB3         :1;                                       /* Data Bit 3 */
    byte DB4         :1;                                       /* Data Bit 4 */
    byte DB5         :1;                                       /* Data Bit 5 */
    byte DB6         :1;                                       /* Data Bit 6 */
    byte DB7         :1;                                       /* Data Bit 7 */
  } Bits;
} CAN0TXDSR4STR;
extern volatile CAN0TXDSR4STR _CAN0TXDSR4 @(REG_BASE + 0x00000178UL);
#define CAN0TXDSR4                      _CAN0TXDSR4.Byte
#define CAN0TXDSR4_DB0                  _CAN0TXDSR4.Bits.DB0
#define CAN0TXDSR4_DB1                  _CAN0TXDSR4.Bits.DB1
#define CAN0TXDSR4_DB2                  _CAN0TXDSR4.Bits.DB2
#define CAN0TXDSR4_DB3                  _CAN0TXDSR4.Bits.DB3
#define CAN0TXDSR4_DB4                  _CAN0TXDSR4.Bits.DB4
#define CAN0TXDSR4_DB5                  _CAN0TXDSR4.Bits.DB5
#define CAN0TXDSR4_DB6                  _CAN0TXDSR4.Bits.DB6
#define CAN0TXDSR4_DB7                  _CAN0TXDSR4.Bits.DB7

#define CAN0TXDSR4_DB0_MASK             1U
#define CAN0TXDSR4_DB1_MASK             2U
#define CAN0TXDSR4_DB2_MASK             4U
#define CAN0TXDSR4_DB3_MASK             8U
#define CAN0TXDSR4_DB4_MASK             16U
#define CAN0TXDSR4_DB5_MASK             32U
#define CAN0TXDSR4_DB6_MASK             64U
#define CAN0TXDSR4_DB7_MASK             128U


/*** CAN0TXDSR5 - MSCAN 0 Transmit Data Segment Register 5; 0x00000179 ***/
typedef union {
  byte Byte;
  struct {
    byte DB0         :1;                                       /* Data Bit 0 */
    byte DB1         :1;                                       /* Data Bit 1 */
    byte DB2         :1;                                       /* Data Bit 2 */
    byte DB3         :1;                                       /* Data Bit 3 */
    byte DB4         :1;                                       /* Data Bit 4 */
    byte DB5         :1;                                       /* Data Bit 5 */
    byte DB6         :1;                                       /* Data Bit 6 */
    byte DB7         :1;                                       /* Data Bit 7 */
  } Bits;
} CAN0TXDSR5STR;
extern volatile CAN0TXDSR5STR _CAN0TXDSR5 @(REG_BASE + 0x00000179UL);
#define CAN0TXDSR5                      _CAN0TXDSR5.Byte
#define CAN0TXDSR5_DB0                  _CAN0TXDSR5.Bits.DB0
#define CAN0TXDSR5_DB1                  _CAN0TXDSR5.Bits.DB1
#define CAN0TXDSR5_DB2                  _CAN0TXDSR5.Bits.DB2
#define CAN0TXDSR5_DB3                  _CAN0TXDSR5.Bits.DB3
#define CAN0TXDSR5_DB4                  _CAN0TXDSR5.Bits.DB4
#define CAN0TXDSR5_DB5                  _CAN0TXDSR5.Bits.DB5
#define CAN0TXDSR5_DB6                  _CAN0TXDSR5.Bits.DB6
#define CAN0TXDSR5_DB7                  _CAN0TXDSR5.Bits.DB7

#define CAN0TXDSR5_DB0_MASK             1U
#define CAN0TXDSR5_DB1_MASK             2U
#define CAN0TXDSR5_DB2_MASK             4U
#define CAN0TXDSR5_DB3_MASK             8U
#define CAN0TXDSR5_DB4_MASK             16U
#define CAN0TXDSR5_DB5_MASK             32U
#define CAN0TXDSR5_DB6_MASK             64U
#define CAN0TXDSR5_DB7_MASK             128U


/*** CAN0TXDSR6 - MSCAN 0 Transmit Data Segment Register 6; 0x0000017A ***/
typedef union {
  byte Byte;
  struct {
    byte DB0         :1;                                       /* Data Bit 0 */
    byte DB1         :1;                                       /* Data Bit 1 */
    byte DB2         :1;                                       /* Data Bit 2 */
    byte DB3         :1;                                       /* Data Bit 3 */
    byte DB4         :1;                                       /* Data Bit 4 */
    byte DB5         :1;                                       /* Data Bit 5 */
    byte DB6         :1;                                       /* Data Bit 6 */
    byte DB7         :1;                                       /* Data Bit 7 */
  } Bits;
} CAN0TXDSR6STR;
extern volatile CAN0TXDSR6STR _CAN0TXDSR6 @(REG_BASE + 0x0000017AUL);
#define CAN0TXDSR6                      _CAN0TXDSR6.Byte
#define CAN0TXDSR6_DB0                  _CAN0TXDSR6.Bits.DB0
#define CAN0TXDSR6_DB1                  _CAN0TXDSR6.Bits.DB1
#define CAN0TXDSR6_DB2                  _CAN0TXDSR6.Bits.DB2
#define CAN0TXDSR6_DB3                  _CAN0TXDSR6.Bits.DB3
#define CAN0TXDSR6_DB4                  _CAN0TXDSR6.Bits.DB4
#define CAN0TXDSR6_DB5                  _CAN0TXDSR6.Bits.DB5
#define CAN0TXDSR6_DB6                  _CAN0TXDSR6.Bits.DB6
#define CAN0TXDSR6_DB7                  _CAN0TXDSR6.Bits.DB7

#define CAN0TXDSR6_DB0_MASK             1U
#define CAN0TXDSR6_DB1_MASK             2U
#define CAN0TXDSR6_DB2_MASK             4U
#define CAN0TXDSR6_DB3_MASK             8U
#define CAN0TXDSR6_DB4_MASK             16U
#define CAN0TXDSR6_DB5_MASK             32U
#define CAN0TXDSR6_DB6_MASK             64U
#define CAN0TXDSR6_DB7_MASK             128U


/*** CAN0TXDSR7 - MSCAN 0 Transmit Data Segment Register 7; 0x0000017B ***/
typedef union {
  byte Byte;
  struct {
    byte DB0         :1;                                       /* Data Bit 0 */
    byte DB1         :1;                                       /* Data Bit 1 */
    byte DB2         :1;                                       /* Data Bit 2 */
    byte DB3         :1;                                       /* Data Bit 3 */
    byte DB4         :1;                                       /* Data Bit 4 */
    byte DB5         :1;                                       /* Data Bit 5 */
    byte DB6         :1;                                       /* Data Bit 6 */
    byte DB7         :1;                                       /* Data Bit 7 */
  } Bits;
} CAN0TXDSR7STR;
extern volatile CAN0TXDSR7STR _CAN0TXDSR7 @(REG_BASE + 0x0000017BUL);
#define CAN0TXDSR7                      _CAN0TXDSR7.Byte
#define CAN0TXDSR7_DB0                  _CAN0TXDSR7.Bits.DB0
#define CAN0TXDSR7_DB1                  _CAN0TXDSR7.Bits.DB1
#define CAN0TXDSR7_DB2                  _CAN0TXDSR7.Bits.DB2
#define CAN0TXDSR7_DB3                  _CAN0TXDSR7.Bits.DB3
#define CAN0TXDSR7_DB4                  _CAN0TXDSR7.Bits.DB4
#define CAN0TXDSR7_DB5                  _CAN0TXDSR7.Bits.DB5
#define CAN0TXDSR7_DB6                  _CAN0TXDSR7.Bits.DB6
#define CAN0TXDSR7_DB7                  _CAN0TXDSR7.Bits.DB7

#define CAN0TXDSR7_DB0_MASK             1U
#define CAN0TXDSR7_DB1_MASK             2U
#define CAN0TXDSR7_DB2_MASK             4U
#define CAN0TXDSR7_DB3_MASK             8U
#define CAN0TXDSR7_DB4_MASK             16U
#define CAN0TXDSR7_DB5_MASK             32U
#define CAN0TXDSR7_DB6_MASK             64U
#define CAN0TXDSR7_DB7_MASK             128U


/*** CAN0TXDLR - MSCAN 0 Transmit Data Length Register; 0x0000017C ***/
typedef union {
  byte Byte;
  struct {
    byte DLC0        :1;                                       /* Data Length Code Bit 0 */
    byte DLC1        :1;                                       /* Data Length Code Bit 1 */
    byte DLC2        :1;                                       /* Data Length Code Bit 2 */
    byte DLC3        :1;                                       /* Data Length Code Bit 3 */
    byte             :1; 
    byte             :1; 
    byte             :1; 
    byte             :1; 
  } Bits;
  struct {
    byte grpDLC  :4;
    byte         :1;
    byte         :1;
    byte         :1;
    byte         :1;
  } MergedBits;
} CAN0TXDLRSTR;
extern volatile CAN0TXDLRSTR _CAN0TXDLR @(REG_BASE + 0x0000017CUL);
#define CAN0TXDLR                       _CAN0TXDLR.Byte
#define CAN0TXDLR_DLC0                  _CAN0TXDLR.Bits.DLC0
#define CAN0TXDLR_DLC1                  _CAN0TXDLR.Bits.DLC1
#define CAN0TXDLR_DLC2                  _CAN0TXDLR.Bits.DLC2
#define CAN0TXDLR_DLC3                  _CAN0TXDLR.Bits.DLC3
#define CAN0TXDLR_DLC                   _CAN0TXDLR.MergedBits.grpDLC

#define CAN0TXDLR_DLC0_MASK             1U
#define CAN0TXDLR_DLC1_MASK             2U
#define CAN0TXDLR_DLC2_MASK             4U
#define CAN0TXDLR_DLC3_MASK             8U
#define CAN0TXDLR_DLC_MASK              15U
#define CAN0TXDLR_DLC_BITNUM            0U


/*** CAN0TXTBPR - MSCAN 0 Transmit Buffer Priority; 0x0000017D ***/
typedef union {
  byte Byte;
  struct {
    byte PRIO0       :1;                                       /* Transmit Buffer Priority Bit 0 */
    byte PRIO1       :1;                                       /* Transmit Buffer Priority Bit 1 */
    byte PRIO2       :1;                                       /* Transmit Buffer Priority Bit 2 */
    byte PRIO3       :1;                                       /* Transmit Buffer Priority Bit 3 */
    byte PRIO4       :1;                                       /* Transmit Buffer Priority Bit 4 */
    byte PRIO5       :1;                                       /* Transmit Buffer Priority Bit 5 */
    byte PRIO6       :1;                                       /* Transmit Buffer Priority Bit 6 */
    byte PRIO7       :1;                                       /* Transmit Buffer Priority Bit 7 */
  } Bits;
} CAN0TXTBPRSTR;
extern volatile CAN0TXTBPRSTR _CAN0TXTBPR @(REG_BASE + 0x0000017DUL);
#define CAN0TXTBPR                      _CAN0TXTBPR.Byte
#define CAN0TXTBPR_PRIO0                _CAN0TXTBPR.Bits.PRIO0
#define CAN0TXTBPR_PRIO1                _CAN0TXTBPR.Bits.PRIO1
#define CAN0TXTBPR_PRIO2                _CAN0TXTBPR.Bits.PRIO2
#define CAN0TXTBPR_PRIO3                _CAN0TXTBPR.Bits.PRIO3
#define CAN0TXTBPR_PRIO4                _CAN0TXTBPR.Bits.PRIO4
#define CAN0TXTBPR_PRIO5                _CAN0TXTBPR.Bits.PRIO5
#define CAN0TXTBPR_PRIO6                _CAN0TXTBPR.Bits.PRIO6
#define CAN0TXTBPR_PRIO7                _CAN0TXTBPR.Bits.PRIO7

#define CAN0TXTBPR_PRIO0_MASK           1U
#define CAN0TXTBPR_PRIO1_MASK           2U
#define CAN0TXTBPR_PRIO2_MASK           4U
#define CAN0TXTBPR_PRIO3_MASK           8U
#define CAN0TXTBPR_PRIO4_MASK           16U
#define CAN0TXTBPR_PRIO5_MASK           32U
#define CAN0TXTBPR_PRIO6_MASK           64U
#define CAN0TXTBPR_PRIO7_MASK           128U


/*** CAN0TXTSR - MSCAN 0 Transmit Time Stamp Register; 0x0000017E ***/
typedef union {
  word Word;
   /* Overlapped registers: */
  struct {
    /*** CAN0TXTSRH - MSCAN 0 Transmit Time Stamp Register High; 0x0000017E ***/
    union {
      byte Byte;
      struct {
        byte TSR8        :1;                                       /* Time Stamp Bit 8 */
        byte TSR9        :1;                                       /* Time Stamp Bit 9 */
        byte TSR10       :1;                                       /* Time Stamp Bit 10 */
        byte TSR11       :1;                                       /* Time Stamp Bit 11 */
        byte TSR12       :1;                                       /* Time Stamp Bit 12 */
        byte TSR13       :1;                                       /* Time Stamp Bit 13 */
        byte TSR14       :1;                                       /* Time Stamp Bit 14 */
        byte TSR15       :1;                                       /* Time Stamp Bit 15 */
      } Bits;
    } CAN0TXTSRHSTR;
    #define CAN0TXTSRH                  _CAN0TXTSR.Overlap_STR.CAN0TXTSRHSTR.Byte
    #define CAN0TXTSRH_TSR8             _CAN0TXTSR.Overlap_STR.CAN0TXTSRHSTR.Bits.TSR8
    #define CAN0TXTSRH_TSR9             _CAN0TXTSR.Overlap_STR.CAN0TXTSRHSTR.Bits.TSR9
    #define CAN0TXTSRH_TSR10            _CAN0TXTSR.Overlap_STR.CAN0TXTSRHSTR.Bits.TSR10
    #define CAN0TXTSRH_TSR11            _CAN0TXTSR.Overlap_STR.CAN0TXTSRHSTR.Bits.TSR11
    #define CAN0TXTSRH_TSR12            _CAN0TXTSR.Overlap_STR.CAN0TXTSRHSTR.Bits.TSR12
    #define CAN0TXTSRH_TSR13            _CAN0TXTSR.Overlap_STR.CAN0TXTSRHSTR.Bits.TSR13
    #define CAN0TXTSRH_TSR14            _CAN0TXTSR.Overlap_STR.CAN0TXTSRHSTR.Bits.TSR14
    #define CAN0TXTSRH_TSR15            _CAN0TXTSR.Overlap_STR.CAN0TXTSRHSTR.Bits.TSR15
    
    #define CAN0TXTSRH_TSR8_MASK        1U
    #define CAN0TXTSRH_TSR9_MASK        2U
    #define CAN0TXTSRH_TSR10_MASK       4U
    #define CAN0TXTSRH_TSR11_MASK       8U
    #define CAN0TXTSRH_TSR12_MASK       16U
    #define CAN0TXTSRH_TSR13_MASK       32U
    #define CAN0TXTSRH_TSR14_MASK       64U
    #define CAN0TXTSRH_TSR15_MASK       128U
    

    /*** CAN0TXTSRL - MSCAN 0 Transmit Time Stamp Register Low; 0x0000017F ***/
    union {
      byte Byte;
      struct {
        byte TSR0        :1;                                       /* Time Stamp Bit 0 */
        byte TSR1        :1;                                       /* Time Stamp Bit 1 */
        byte TSR2        :1;                                       /* Time Stamp Bit 2 */
        byte TSR3        :1;                                       /* Time Stamp Bit 3 */
        byte TSR4        :1;                                       /* Time Stamp Bit 4 */
        byte TSR5        :1;                                       /* Time Stamp Bit 5 */
        byte TSR6        :1;                                       /* Time Stamp Bit 6 */
        byte TSR7        :1;                                       /* Time Stamp Bit 7 */
      } Bits;
    } CAN0TXTSRLSTR;
    #define CAN0TXTSRL                  _CAN0TXTSR.Overlap_STR.CAN0TXTSRLSTR.Byte
    #define CAN0TXTSRL_TSR0             _CAN0TXTSR.Overlap_STR.CAN0TXTSRLSTR.Bits.TSR0
    #define CAN0TXTSRL_TSR1             _CAN0TXTSR.Overlap_STR.CAN0TXTSRLSTR.Bits.TSR1
    #define CAN0TXTSRL_TSR2             _CAN0TXTSR.Overlap_STR.CAN0TXTSRLSTR.Bits.TSR2
    #define CAN0TXTSRL_TSR3             _CAN0TXTSR.Overlap_STR.CAN0TXTSRLSTR.Bits.TSR3
    #define CAN0TXTSRL_TSR4             _CAN0TXTSR.Overlap_STR.CAN0TXTSRLSTR.Bits.TSR4
    #define CAN0TXTSRL_TSR5             _CAN0TXTSR.Overlap_STR.CAN0TXTSRLSTR.Bits.TSR5
    #define CAN0TXTSRL_TSR6             _CAN0TXTSR.Overlap_STR.CAN0TXTSRLSTR.Bits.TSR6
    #define CAN0TXTSRL_TSR7             _CAN0TXTSR.Overlap_STR.CAN0TXTSRLSTR.Bits.TSR7
    
    #define CAN0TXTSRL_TSR0_MASK        1U
    #define CAN0TXTSRL_TSR1_MASK        2U
    #define CAN0TXTSRL_TSR2_MASK        4U
    #define CAN0TXTSRL_TSR3_MASK        8U
    #define CAN0TXTSRL_TSR4_MASK        16U
    #define CAN0TXTSRL_TSR5_MASK        32U
    #define CAN0TXTSRL_TSR6_MASK        64U
    #define CAN0TXTSRL_TSR7_MASK        128U
    
  } Overlap_STR;

  struct {
    word TSR0        :1;                                       /* Time Stamp Bit 0 */
    word TSR1        :1;                                       /* Time Stamp Bit 1 */
    word TSR2        :1;                                       /* Time Stamp Bit 2 */
    word TSR3        :1;                                       /* Time Stamp Bit 3 */
    word TSR4        :1;                                       /* Time Stamp Bit 4 */
    word TSR5        :1;                                       /* Time Stamp Bit 5 */
    word TSR6        :1;                                       /* Time Stamp Bit 6 */
    word TSR7        :1;                                       /* Time Stamp Bit 7 */
    word TSR8        :1;                                       /* Time Stamp Bit 8 */
    word TSR9        :1;                                       /* Time Stamp Bit 9 */
    word TSR10       :1;                                       /* Time Stamp Bit 10 */
    word TSR11       :1;                                       /* Time Stamp Bit 11 */
    word TSR12       :1;                                       /* Time Stamp Bit 12 */
    word TSR13       :1;                                       /* Time Stamp Bit 13 */
    word TSR14       :1;                                       /* Time Stamp Bit 14 */
    word TSR15       :1;                                       /* Time Stamp Bit 15 */
  } Bits;
} CAN0TXTSRSTR;
extern volatile CAN0TXTSRSTR _CAN0TXTSR @(REG_BASE + 0x0000017EUL);
#define CAN0TXTSR                       _CAN0TXTSR.Word
#define CAN0TXTSR_TSR0                  _CAN0TXTSR.Bits.TSR0
#define CAN0TXTSR_TSR1                  _CAN0TXTSR.Bits.TSR1
#define CAN0TXTSR_TSR2                  _CAN0TXTSR.Bits.TSR2
#define CAN0TXTSR_TSR3                  _CAN0TXTSR.Bits.TSR3
#define CAN0TXTSR_TSR4                  _CAN0TXTSR.Bits.TSR4
#define CAN0TXTSR_TSR5                  _CAN0TXTSR.Bits.TSR5
#define CAN0TXTSR_TSR6                  _CAN0TXTSR.Bits.TSR6
#define CAN0TXTSR_TSR7                  _CAN0TXTSR.Bits.TSR7
#define CAN0TXTSR_TSR8                  _CAN0TXTSR.Bits.TSR8
#define CAN0TXTSR_TSR9                  _CAN0TXTSR.Bits.TSR9
#define CAN0TXTSR_TSR10                 _CAN0TXTSR.Bits.TSR10
#define CAN0TXTSR_TSR11                 _CAN0TXTSR.Bits.TSR11
#define CAN0TXTSR_TSR12                 _CAN0TXTSR.Bits.TSR12
#define CAN0TXTSR_TSR13                 _CAN0TXTSR.Bits.TSR13
#define CAN0TXTSR_TSR14                 _CAN0TXTSR.Bits.TSR14
#define CAN0TXTSR_TSR15                 _CAN0TXTSR.Bits.TSR15

#define CAN0TXTSR_TSR0_MASK             1U
#define CAN0TXTSR_TSR1_MASK             2U
#define CAN0TXTSR_TSR2_MASK             4U
#define CAN0TXTSR_TSR3_MASK             8U
#define CAN0TXTSR_TSR4_MASK             16U
#define CAN0TXTSR_TSR5_MASK             32U
#define CAN0TXTSR_TSR6_MASK             64U
#define CAN0TXTSR_TSR7_MASK             128U
#define CAN0TXTSR_TSR8_MASK             256U
#define CAN0TXTSR_TSR9_MASK             512U
#define CAN0TXTSR_TSR10_MASK            1024U
#define CAN0TXTSR_TSR11_MASK            2048U
#define CAN0TXTSR_TSR12_MASK            4096U
#define CAN0TXTSR_TSR13_MASK            8192U
#define CAN0TXTSR_TSR14_MASK            16384U
#define CAN0TXTSR_TSR15_MASK            32768U


/*** PTT - Port T I/O Register; 0x00000240 ***/
typedef union {
  byte Byte;
  struct {
    byte PTT0        :1;                                       /* Port T Bit 0 */
    byte PTT1        :1;                                       /* Port T Bit 1 */
    byte PTT2        :1;                                       /* Port T Bit 2 */
    byte PTT3        :1;                                       /* Port T Bit 3 */
    byte PTT4        :1;                                       /* Port T Bit 4 */
    byte PTT5        :1;                                       /* Port T Bit 5 */
    byte PTT6        :1;                                       /* Port T Bit 6 */
    byte PTT7        :1;                                       /* Port T Bit 7 */
  } Bits;
} PTTSTR;
extern volatile PTTSTR _PTT @(REG_BASE + 0x00000240UL);
#define PTT                             _PTT.Byte
#define PTT_PTT0                        _PTT.Bits.PTT0
#define PTT_PTT1                        _PTT.Bits.PTT1
#define PTT_PTT2                        _PTT.Bits.PTT2
#define PTT_PTT3                        _PTT.Bits.PTT3
#define PTT_PTT4                        _PTT.Bits.PTT4
#define PTT_PTT5                        _PTT.Bits.PTT5
#define PTT_PTT6                        _PTT.Bits.PTT6
#define PTT_PTT7                        _PTT.Bits.PTT7

#define PTT_PTT0_MASK                   1U
#define PTT_PTT1_MASK                   2U
#define PTT_PTT2_MASK                   4U
#define PTT_PTT3_MASK                   8U
#define PTT_PTT4_MASK                   16U
#define PTT_PTT5_MASK                   32U
#define PTT_PTT6_MASK                   64U
#define PTT_PTT7_MASK                   128U


/*** PTIT - Port T Input Register; 0x00000241 ***/
typedef union {
  byte Byte;
  struct {
    byte PTIT0       :1;                                       /* Port T Bit 0 */
    byte PTIT1       :1;                                       /* Port T Bit 1 */
    byte PTIT2       :1;                                       /* Port T Bit 2 */
    byte PTIT3       :1;                                       /* Port T Bit 3 */
    byte PTIT4       :1;                                       /* Port T Bit 4 */
    byte PTIT5       :1;                                       /* Port T Bit 5 */
    byte PTIT6       :1;                                       /* Port T Bit 6 */
    byte PTIT7       :1;                                       /* Port T Bit 7 */
  } Bits;
} PTITSTR;
extern volatile PTITSTR _PTIT @(REG_BASE + 0x00000241UL);
#define PTIT                            _PTIT.Byte
#define PTIT_PTIT0                      _PTIT.Bits.PTIT0
#define PTIT_PTIT1                      _PTIT.Bits.PTIT1
#define PTIT_PTIT2                      _PTIT.Bits.PTIT2
#define PTIT_PTIT3                      _PTIT.Bits.PTIT3
#define PTIT_PTIT4                      _PTIT.Bits.PTIT4
#define PTIT_PTIT5                      _PTIT.Bits.PTIT5
#define PTIT_PTIT6                      _PTIT.Bits.PTIT6
#define PTIT_PTIT7                      _PTIT.Bits.PTIT7

#define PTIT_PTIT0_MASK                 1U
#define PTIT_PTIT1_MASK                 2U
#define PTIT_PTIT2_MASK                 4U
#define PTIT_PTIT3_MASK                 8U
#define PTIT_PTIT4_MASK                 16U
#define PTIT_PTIT5_MASK                 32U
#define PTIT_PTIT6_MASK                 64U
#define PTIT_PTIT7_MASK                 128U


/*** DDRT - Port T Data Direction Register; 0x00000242 ***/
typedef union {
  byte Byte;
  struct {
    byte DDRT0       :1;                                       /* Data Direction Port T Bit 0 */
    byte DDRT1       :1;                                       /* Data Direction Port T Bit 1 */
    byte DDRT2       :1;                                       /* Data Direction Port T Bit 2 */
    byte DDRT3       :1;                                       /* Data Direction Port T Bit 3 */
    byte DDRT4       :1;                                       /* Data Direction Port T Bit 4 */
    byte DDRT5       :1;                                       /* Data Direction Port T Bit 5 */
    byte DDRT6       :1;                                       /* Data Direction Port T Bit 6 */
    byte DDRT7       :1;                                       /* Data Direction Port T Bit 7 */
  } Bits;
} DDRTSTR;
extern volatile DDRTSTR _DDRT @(REG_BASE + 0x00000242UL);
#define DDRT                            _DDRT.Byte
#define DDRT_DDRT0                      _DDRT.Bits.DDRT0
#define DDRT_DDRT1                      _DDRT.Bits.DDRT1
#define DDRT_DDRT2                      _DDRT.Bits.DDRT2
#define DDRT_DDRT3                      _DDRT.Bits.DDRT3
#define DDRT_DDRT4                      _DDRT.Bits.DDRT4
#define DDRT_DDRT5                      _DDRT.Bits.DDRT5
#define DDRT_DDRT6                      _DDRT.Bits.DDRT6
#define DDRT_DDRT7                      _DDRT.Bits.DDRT7

#define DDRT_DDRT0_MASK                 1U
#define DDRT_DDRT1_MASK                 2U
#define DDRT_DDRT2_MASK                 4U
#define DDRT_DDRT3_MASK                 8U
#define DDRT_DDRT4_MASK                 16U
#define DDRT_DDRT5_MASK                 32U
#define DDRT_DDRT6_MASK                 64U
#define DDRT_DDRT7_MASK                 128U


/*** RDRT - Port T Reduced Drive Register; 0x00000243 ***/
typedef union {
  byte Byte;
  struct {
    byte RDRT0       :1;                                       /* Reduced Drive Port T Bit 0 */
    byte RDRT1       :1;                                       /* Reduced Drive Port T Bit 1 */
    byte RDRT2       :1;                                       /* Reduced Drive Port T Bit 2 */
    byte RDRT3       :1;                                       /* Reduced Drive Port T Bit 3 */
    byte RDRT4       :1;                                       /* Reduced Drive Port T Bit 4 */
    byte RDRT5       :1;                                       /* Reduced Drive Port T Bit 5 */
    byte RDRT6       :1;                                       /* Reduced Drive Port T Bit 6 */
    byte RDRT7       :1;                                       /* Reduced Drive Port T Bit 7 */
  } Bits;
} RDRTSTR;
extern volatile RDRTSTR _RDRT @(REG_BASE + 0x00000243UL);
#define RDRT                            _RDRT.Byte
#define RDRT_RDRT0                      _RDRT.Bits.RDRT0
#define RDRT_RDRT1                      _RDRT.Bits.RDRT1
#define RDRT_RDRT2                      _RDRT.Bits.RDRT2
#define RDRT_RDRT3                      _RDRT.Bits.RDRT3
#define RDRT_RDRT4                      _RDRT.Bits.RDRT4
#define RDRT_RDRT5                      _RDRT.Bits.RDRT5
#define RDRT_RDRT6                      _RDRT.Bits.RDRT6
#define RDRT_RDRT7                      _RDRT.Bits.RDRT7

#define RDRT_RDRT0_MASK                 1U
#define RDRT_RDRT1_MASK                 2U
#define RDRT_RDRT2_MASK                 4U
#define RDRT_RDRT3_MASK                 8U
#define RDRT_RDRT4_MASK                 16U
#define RDRT_RDRT5_MASK                 32U
#define RDRT_RDRT6_MASK                 64U
#define RDRT_RDRT7_MASK                 128U


/*** PERT - Port T Pull Device Enable Register; 0x00000244 ***/
typedef union {
  byte Byte;
  struct {
    byte PERT0       :1;                                       /* Pull Device Enable Port T Bit 0 */
    byte PERT1       :1;                                       /* Pull Device Enable Port T Bit 1 */
    byte PERT2       :1;                                       /* Pull Device Enable Port T Bit 2 */
    byte PERT3       :1;                                       /* Pull Device Enable Port T Bit 3 */
    byte PERT4       :1;                                       /* Pull Device Enable Port T Bit 4 */
    byte PERT5       :1;                                       /* Pull Device Enable Port T Bit 5 */
    byte PERT6       :1;                                       /* Pull Device Enable Port T Bit 6 */
    byte PERT7       :1;                                       /* Pull Device Enable Port T Bit 7 */
  } Bits;
} PERTSTR;
extern volatile PERTSTR _PERT @(REG_BASE + 0x00000244UL);
#define PERT                            _PERT.Byte
#define PERT_PERT0                      _PERT.Bits.PERT0
#define PERT_PERT1                      _PERT.Bits.PERT1
#define PERT_PERT2                      _PERT.Bits.PERT2
#define PERT_PERT3                      _PERT.Bits.PERT3
#define PERT_PERT4                      _PERT.Bits.PERT4
#define PERT_PERT5                      _PERT.Bits.PERT5
#define PERT_PERT6                      _PERT.Bits.PERT6
#define PERT_PERT7                      _PERT.Bits.PERT7

#define PERT_PERT0_MASK                 1U
#define PERT_PERT1_MASK                 2U
#define PERT_PERT2_MASK                 4U
#define PERT_PERT3_MASK                 8U
#define PERT_PERT4_MASK                 16U
#define PERT_PERT5_MASK                 32U
#define PERT_PERT6_MASK                 64U
#define PERT_PERT7_MASK                 128U


/*** PPST - Port T Polarity Select Register; 0x00000245 ***/
typedef union {
  byte Byte;
  struct {
    byte PPST0       :1;                                       /* Pull Select Port T Bit 0 */
    byte PPST1       :1;                                       /* Pull Select Port T Bit 1 */
    byte PPST2       :1;                                       /* Pull Select Port T Bit 2 */
    byte PPST3       :1;                                       /* Pull Select Port T Bit 3 */
    byte PPST4       :1;                                       /* Pull Select Port T Bit 4 */
    byte PPST5       :1;                                       /* Pull Select Port T Bit 5 */
    byte PPST6       :1;                                       /* Pull Select Port T Bit 6 */
    byte PPST7       :1;                                       /* Pull Select Port T Bit 7 */
  } Bits;
} PPSTSTR;
extern volatile PPSTSTR _PPST @(REG_BASE + 0x00000245UL);
#define PPST                            _PPST.Byte
#define PPST_PPST0                      _PPST.Bits.PPST0
#define PPST_PPST1                      _PPST.Bits.PPST1
#define PPST_PPST2                      _PPST.Bits.PPST2
#define PPST_PPST3                      _PPST.Bits.PPST3
#define PPST_PPST4                      _PPST.Bits.PPST4
#define PPST_PPST5                      _PPST.Bits.PPST5
#define PPST_PPST6                      _PPST.Bits.PPST6
#define PPST_PPST7                      _PPST.Bits.PPST7

#define PPST_PPST0_MASK                 1U
#define PPST_PPST1_MASK                 2U
#define PPST_PPST2_MASK                 4U
#define PPST_PPST3_MASK                 8U
#define PPST_PPST4_MASK                 16U
#define PPST_PPST5_MASK                 32U
#define PPST_PPST6_MASK                 64U
#define PPST_PPST7_MASK                 128U


/*** PTS - Port S I/O Register; 0x00000248 ***/
typedef union {
  byte Byte;
  struct {
    byte PTS0        :1;                                       /* Port S Bit 0 */
    byte PTS1        :1;                                       /* Port S Bit 1 */
    byte PTS2        :1;                                       /* Port S Bit 2 */
    byte PTS3        :1;                                       /* Port S Bit 3 */
    byte PTS4        :1;                                       /* Port S Bit 4 */
    byte PTS5        :1;                                       /* Port S Bit 5 */
    byte PTS6        :1;                                       /* Port S Bit 6 */
    byte PTS7        :1;                                       /* Port S Bit 7 */
  } Bits;
} PTSSTR;
extern volatile PTSSTR _PTS @(REG_BASE + 0x00000248UL);
#define PTS                             _PTS.Byte
#define PTS_PTS0                        _PTS.Bits.PTS0
#define PTS_PTS1                        _PTS.Bits.PTS1
#define PTS_PTS2                        _PTS.Bits.PTS2
#define PTS_PTS3                        _PTS.Bits.PTS3
#define PTS_PTS4                        _PTS.Bits.PTS4
#define PTS_PTS5                        _PTS.Bits.PTS5
#define PTS_PTS6                        _PTS.Bits.PTS6
#define PTS_PTS7                        _PTS.Bits.PTS7

#define PTS_PTS0_MASK                   1U
#define PTS_PTS1_MASK                   2U
#define PTS_PTS2_MASK                   4U
#define PTS_PTS3_MASK                   8U
#define PTS_PTS4_MASK                   16U
#define PTS_PTS5_MASK                   32U
#define PTS_PTS6_MASK                   64U
#define PTS_PTS7_MASK                   128U


/*** PTIS - Port S Input Register; 0x00000249 ***/
typedef union {
  byte Byte;
  struct {
    byte PTIS0       :1;                                       /* Port S Bit 0 */
    byte PTIS1       :1;                                       /* Port S Bit 1 */
    byte PTIS2       :1;                                       /* Port S Bit 2 */
    byte PTIS3       :1;                                       /* Port S Bit 3 */
    byte PTIS4       :1;                                       /* Port S Bit 4 */
    byte PTIS5       :1;                                       /* Port S Bit 5 */
    byte PTIS6       :1;                                       /* Port S Bit 6 */
    byte PTIS7       :1;                                       /* Port S Bit 7 */
  } Bits;
} PTISSTR;
extern volatile PTISSTR _PTIS @(REG_BASE + 0x00000249UL);
#define PTIS                            _PTIS.Byte
#define PTIS_PTIS0                      _PTIS.Bits.PTIS0
#define PTIS_PTIS1                      _PTIS.Bits.PTIS1
#define PTIS_PTIS2                      _PTIS.Bits.PTIS2
#define PTIS_PTIS3                      _PTIS.Bits.PTIS3
#define PTIS_PTIS4                      _PTIS.Bits.PTIS4
#define PTIS_PTIS5                      _PTIS.Bits.PTIS5
#define PTIS_PTIS6                      _PTIS.Bits.PTIS6
#define PTIS_PTIS7                      _PTIS.Bits.PTIS7

#define PTIS_PTIS0_MASK                 1U
#define PTIS_PTIS1_MASK                 2U
#define PTIS_PTIS2_MASK                 4U
#define PTIS_PTIS3_MASK                 8U
#define PTIS_PTIS4_MASK                 16U
#define PTIS_PTIS5_MASK                 32U
#define PTIS_PTIS6_MASK                 64U
#define PTIS_PTIS7_MASK                 128U


/*** DDRS - Port S Data Direction Register; 0x0000024A ***/
typedef union {
  byte Byte;
  struct {
    byte DDRS0       :1;                                       /* Data Direction Port S Bit 0 */
    byte DDRS1       :1;                                       /* Data Direction Port S Bit 1 */
    byte DDRS2       :1;                                       /* Data Direction Port S Bit 2 */
    byte DDRS3       :1;                                       /* Data Direction Port S Bit 3 */
    byte DDRS4       :1;                                       /* Data Direction Port S Bit 4 */
    byte DDRS5       :1;                                       /* Data Direction Port S Bit 5 */
    byte DDRS6       :1;                                       /* Data Direction Port S Bit 6 */
    byte DDRS7       :1;                                       /* Data Direction Port S Bit 7 */
  } Bits;
} DDRSSTR;
extern volatile DDRSSTR _DDRS @(REG_BASE + 0x0000024AUL);
#define DDRS                            _DDRS.Byte
#define DDRS_DDRS0                      _DDRS.Bits.DDRS0
#define DDRS_DDRS1                      _DDRS.Bits.DDRS1
#define DDRS_DDRS2                      _DDRS.Bits.DDRS2
#define DDRS_DDRS3                      _DDRS.Bits.DDRS3
#define DDRS_DDRS4                      _DDRS.Bits.DDRS4
#define DDRS_DDRS5                      _DDRS.Bits.DDRS5
#define DDRS_DDRS6                      _DDRS.Bits.DDRS6
#define DDRS_DDRS7                      _DDRS.Bits.DDRS7

#define DDRS_DDRS0_MASK                 1U
#define DDRS_DDRS1_MASK                 2U
#define DDRS_DDRS2_MASK                 4U
#define DDRS_DDRS3_MASK                 8U
#define DDRS_DDRS4_MASK                 16U
#define DDRS_DDRS5_MASK                 32U
#define DDRS_DDRS6_MASK                 64U
#define DDRS_DDRS7_MASK                 128U


/*** RDRS - Port S Reduced Drive Register; 0x0000024B ***/
typedef union {
  byte Byte;
  struct {
    byte RDRS0       :1;                                       /* Reduced Drive Port S Bit 0 */
    byte RDRS1       :1;                                       /* Reduced Drive Port S Bit 1 */
    byte RDRS2       :1;                                       /* Reduced Drive Port S Bit 2 */
    byte RDRS3       :1;                                       /* Reduced Drive Port S Bit 3 */
    byte RDRS4       :1;                                       /* Reduced Drive Port S Bit 4 */
    byte RDRS5       :1;                                       /* Reduced Drive Port S Bit 5 */
    byte RDRS6       :1;                                       /* Reduced Drive Port S Bit 6 */
    byte RDRS7       :1;                                       /* Reduced Drive Port S Bit 7 */
  } Bits;
} RDRSSTR;
extern volatile RDRSSTR _RDRS @(REG_BASE + 0x0000024BUL);
#define RDRS                            _RDRS.Byte
#define RDRS_RDRS0                      _RDRS.Bits.RDRS0
#define RDRS_RDRS1                      _RDRS.Bits.RDRS1
#define RDRS_RDRS2                      _RDRS.Bits.RDRS2
#define RDRS_RDRS3                      _RDRS.Bits.RDRS3
#define RDRS_RDRS4                      _RDRS.Bits.RDRS4
#define RDRS_RDRS5                      _RDRS.Bits.RDRS5
#define RDRS_RDRS6                      _RDRS.Bits.RDRS6
#define RDRS_RDRS7                      _RDRS.Bits.RDRS7

#define RDRS_RDRS0_MASK                 1U
#define RDRS_RDRS1_MASK                 2U
#define RDRS_RDRS2_MASK                 4U
#define RDRS_RDRS3_MASK                 8U
#define RDRS_RDRS4_MASK                 16U
#define RDRS_RDRS5_MASK                 32U
#define RDRS_RDRS6_MASK                 64U
#define RDRS_RDRS7_MASK                 128U


/*** PERS - Port S Pull Device Enable Register; 0x0000024C ***/
typedef union {
  byte Byte;
  struct {
    byte PERS0       :1;                                       /* Pull Device Enable Port S Bit 0 */
    byte PERS1       :1;                                       /* Pull Device Enable Port S Bit 1 */
    byte PERS2       :1;                                       /* Pull Device Enable Port S Bit 2 */
    byte PERS3       :1;                                       /* Pull Device Enable Port S Bit 3 */
    byte PERS4       :1;                                       /* Pull Device Enable Port S Bit 4 */
    byte PERS5       :1;                                       /* Pull Device Enable Port S Bit 5 */
    byte PERS6       :1;                                       /* Pull Device Enable Port S Bit 6 */
    byte PERS7       :1;                                       /* Pull Device Enable Port S Bit 7 */
  } Bits;
} PERSSTR;
extern volatile PERSSTR _PERS @(REG_BASE + 0x0000024CUL);
#define PERS                            _PERS.Byte
#define PERS_PERS0                      _PERS.Bits.PERS0
#define PERS_PERS1                      _PERS.Bits.PERS1
#define PERS_PERS2                      _PERS.Bits.PERS2
#define PERS_PERS3                      _PERS.Bits.PERS3
#define PERS_PERS4                      _PERS.Bits.PERS4
#define PERS_PERS5                      _PERS.Bits.PERS5
#define PERS_PERS6                      _PERS.Bits.PERS6
#define PERS_PERS7                      _PERS.Bits.PERS7

#define PERS_PERS0_MASK                 1U
#define PERS_PERS1_MASK                 2U
#define PERS_PERS2_MASK                 4U
#define PERS_PERS3_MASK                 8U
#define PERS_PERS4_MASK                 16U
#define PERS_PERS5_MASK                 32U
#define PERS_PERS6_MASK                 64U
#define PERS_PERS7_MASK                 128U


/*** PPSS - Port S Polarity Select Register; 0x0000024D ***/
typedef union {
  byte Byte;
  struct {
    byte PPSS0       :1;                                       /* Pull Select Port S Bit 0 */
    byte PPSS1       :1;                                       /* Pull Select Port S Bit 1 */
    byte PPSS2       :1;                                       /* Pull Select Port S Bit 2 */
    byte PPSS3       :1;                                       /* Pull Select Port S Bit 3 */
    byte PPSS4       :1;                                       /* Pull Select Port S Bit 4 */
    byte PPSS5       :1;                                       /* Pull Select Port S Bit 5 */
    byte PPSS6       :1;                                       /* Pull Select Port S Bit 6 */
    byte PPSS7       :1;                                       /* Pull Select Port S Bit 7 */
  } Bits;
} PPSSSTR;
extern volatile PPSSSTR _PPSS @(REG_BASE + 0x0000024DUL);
#define PPSS                            _PPSS.Byte
#define PPSS_PPSS0                      _PPSS.Bits.PPSS0
#define PPSS_PPSS1                      _PPSS.Bits.PPSS1
#define PPSS_PPSS2                      _PPSS.Bits.PPSS2
#define PPSS_PPSS3                      _PPSS.Bits.PPSS3
#define PPSS_PPSS4                      _PPSS.Bits.PPSS4
#define PPSS_PPSS5                      _PPSS.Bits.PPSS5
#define PPSS_PPSS6                      _PPSS.Bits.PPSS6
#define PPSS_PPSS7                      _PPSS.Bits.PPSS7

#define PPSS_PPSS0_MASK                 1U
#define PPSS_PPSS1_MASK                 2U
#define PPSS_PPSS2_MASK                 4U
#define PPSS_PPSS3_MASK                 8U
#define PPSS_PPSS4_MASK                 16U
#define PPSS_PPSS5_MASK                 32U
#define PPSS_PPSS6_MASK                 64U
#define PPSS_PPSS7_MASK                 128U


/*** WOMS - Port S Wired-Or Mode Register; 0x0000024E ***/
typedef union {
  byte Byte;
  struct {
    byte WOMS0       :1;                                       /* Wired-Or Mode Port S Bit 0 */
    byte WOMS1       :1;                                       /* Wired-Or Mode Port S Bit 1 */
    byte WOMS2       :1;                                       /* Wired-Or Mode Port S Bit 2 */
    byte WOMS3       :1;                                       /* Wired-Or Mode Port S Bit 3 */
    byte WOMS4       :1;                                       /* Wired-Or Mode Port S Bit 4 */
    byte WOMS5       :1;                                       /* Wired-Or Mode Port S Bit 5 */
    byte WOMS6       :1;                                       /* Wired-Or Mode Port S Bit 6 */
    byte WOMS7       :1;                                       /* Wired-Or Mode Port S Bit 7 */
  } Bits;
} WOMSSTR;
extern volatile WOMSSTR _WOMS @(REG_BASE + 0x0000024EUL);
#define WOMS                            _WOMS.Byte
#define WOMS_WOMS0                      _WOMS.Bits.WOMS0
#define WOMS_WOMS1                      _WOMS.Bits.WOMS1
#define WOMS_WOMS2                      _WOMS.Bits.WOMS2
#define WOMS_WOMS3                      _WOMS.Bits.WOMS3
#define WOMS_WOMS4                      _WOMS.Bits.WOMS4
#define WOMS_WOMS5                      _WOMS.Bits.WOMS5
#define WOMS_WOMS6                      _WOMS.Bits.WOMS6
#define WOMS_WOMS7                      _WOMS.Bits.WOMS7

#define WOMS_WOMS0_MASK                 1U
#define WOMS_WOMS1_MASK                 2U
#define WOMS_WOMS2_MASK                 4U
#define WOMS_WOMS3_MASK                 8U
#define WOMS_WOMS4_MASK                 16U
#define WOMS_WOMS5_MASK                 32U
#define WOMS_WOMS6_MASK                 64U
#define WOMS_WOMS7_MASK                 128U


/*** PTM - Port M I/O Register; 0x00000250 ***/
typedef union {
  byte Byte;
  struct {
    byte PTM0        :1;                                       /* Port M Bit 0 */
    byte PTM1        :1;                                       /* Port M Bit 1 */
    byte PTM2        :1;                                       /* Port M Bit 2 */
    byte PTM3        :1;                                       /* Port M Bit 3 */
    byte PTM4        :1;                                       /* Port M Bit 4 */
    byte PTM5        :1;                                       /* Port M Bit 5 */
    byte PTM6        :1;                                       /* Port M Bit 6 */
    byte PTM7        :1;                                       /* Port M Bit 7 */
  } Bits;
} PTMSTR;
extern volatile PTMSTR _PTM @(REG_BASE + 0x00000250UL);
#define PTM                             _PTM.Byte
#define PTM_PTM0                        _PTM.Bits.PTM0
#define PTM_PTM1                        _PTM.Bits.PTM1
#define PTM_PTM2                        _PTM.Bits.PTM2
#define PTM_PTM3                        _PTM.Bits.PTM3
#define PTM_PTM4                        _PTM.Bits.PTM4
#define PTM_PTM5                        _PTM.Bits.PTM5
#define PTM_PTM6                        _PTM.Bits.PTM6
#define PTM_PTM7                        _PTM.Bits.PTM7

#define PTM_PTM0_MASK                   1U
#define PTM_PTM1_MASK                   2U
#define PTM_PTM2_MASK                   4U
#define PTM_PTM3_MASK                   8U
#define PTM_PTM4_MASK                   16U
#define PTM_PTM5_MASK                   32U
#define PTM_PTM6_MASK                   64U
#define PTM_PTM7_MASK                   128U


/*** PTIM - Port M Input Register; 0x00000251 ***/
typedef union {
  byte Byte;
  struct {
    byte PTIM0       :1;                                       /* Port M Bit 0 */
    byte PTIM1       :1;                                       /* Port M Bit 1 */
    byte PTIM2       :1;                                       /* Port M Bit 2 */
    byte PTIM3       :1;                                       /* Port M Bit 3 */
    byte PTIM4       :1;                                       /* Port M Bit 4 */
    byte PTIM5       :1;                                       /* Port M Bit 5 */
    byte PTIM6       :1;                                       /* Port M Bit 6 */
    byte PTIM7       :1;                                       /* Port M Bit 7 */
  } Bits;
} PTIMSTR;
extern volatile PTIMSTR _PTIM @(REG_BASE + 0x00000251UL);
#define PTIM                            _PTIM.Byte
#define PTIM_PTIM0                      _PTIM.Bits.PTIM0
#define PTIM_PTIM1                      _PTIM.Bits.PTIM1
#define PTIM_PTIM2                      _PTIM.Bits.PTIM2
#define PTIM_PTIM3                      _PTIM.Bits.PTIM3
#define PTIM_PTIM4                      _PTIM.Bits.PTIM4
#define PTIM_PTIM5                      _PTIM.Bits.PTIM5
#define PTIM_PTIM6                      _PTIM.Bits.PTIM6
#define PTIM_PTIM7                      _PTIM.Bits.PTIM7

#define PTIM_PTIM0_MASK                 1U
#define PTIM_PTIM1_MASK                 2U
#define PTIM_PTIM2_MASK                 4U
#define PTIM_PTIM3_MASK                 8U
#define PTIM_PTIM4_MASK                 16U
#define PTIM_PTIM5_MASK                 32U
#define PTIM_PTIM6_MASK                 64U
#define PTIM_PTIM7_MASK                 128U


/*** DDRM - Port M Data Direction Register; 0x00000252 ***/
typedef union {
  byte Byte;
  struct {
    byte DDRM0       :1;                                       /* Data Direction Port M Bit 0 */
    byte DDRM1       :1;                                       /* Data Direction Port M Bit 1 */
    byte DDRM2       :1;                                       /* Data Direction Port M Bit 2 */
    byte DDRM3       :1;                                       /* Data Direction Port M Bit 3 */
    byte DDRM4       :1;                                       /* Data Direction Port M Bit 4 */
    byte DDRM5       :1;                                       /* Data Direction Port M Bit 5 */
    byte DDRM6       :1;                                       /* Data Direction Port M Bit 6 */
    byte DDRM7       :1;                                       /* Data Direction Port M Bit 7 */
  } Bits;
} DDRMSTR;
extern volatile DDRMSTR _DDRM @(REG_BASE + 0x00000252UL);
#define DDRM                            _DDRM.Byte
#define DDRM_DDRM0                      _DDRM.Bits.DDRM0
#define DDRM_DDRM1                      _DDRM.Bits.DDRM1
#define DDRM_DDRM2                      _DDRM.Bits.DDRM2
#define DDRM_DDRM3                      _DDRM.Bits.DDRM3
#define DDRM_DDRM4                      _DDRM.Bits.DDRM4
#define DDRM_DDRM5                      _DDRM.Bits.DDRM5
#define DDRM_DDRM6                      _DDRM.Bits.DDRM6
#define DDRM_DDRM7                      _DDRM.Bits.DDRM7

#define DDRM_DDRM0_MASK                 1U
#define DDRM_DDRM1_MASK                 2U
#define DDRM_DDRM2_MASK                 4U
#define DDRM_DDRM3_MASK                 8U
#define DDRM_DDRM4_MASK                 16U
#define DDRM_DDRM5_MASK                 32U
#define DDRM_DDRM6_MASK                 64U
#define DDRM_DDRM7_MASK                 128U


/*** RDRM - Port M Reduced Drive Register; 0x00000253 ***/
typedef union {
  byte Byte;
  struct {
    byte RDRM0       :1;                                       /* Reduced Drive Port M Bit 0 */
    byte RDRM1       :1;                                       /* Reduced Drive Port M Bit 1 */
    byte RDRM2       :1;                                       /* Reduced Drive Port M Bit 2 */
    byte RDRM3       :1;                                       /* Reduced Drive Port M Bit 3 */
    byte RDRM4       :1;                                       /* Reduced Drive Port M Bit 4 */
    byte RDRM5       :1;                                       /* Reduced Drive Port M Bit 5 */
    byte RDRM6       :1;                                       /* Reduced Drive Port M Bit 6 */
    byte RDRM7       :1;                                       /* Reduced Drive Port M Bit 7 */
  } Bits;
} RDRMSTR;
extern volatile RDRMSTR _RDRM @(REG_BASE + 0x00000253UL);
#define RDRM                            _RDRM.Byte
#define RDRM_RDRM0                      _RDRM.Bits.RDRM0
#define RDRM_RDRM1                      _RDRM.Bits.RDRM1
#define RDRM_RDRM2                      _RDRM.Bits.RDRM2
#define RDRM_RDRM3                      _RDRM.Bits.RDRM3
#define RDRM_RDRM4                      _RDRM.Bits.RDRM4
#define RDRM_RDRM5                      _RDRM.Bits.RDRM5
#define RDRM_RDRM6                      _RDRM.Bits.RDRM6
#define RDRM_RDRM7                      _RDRM.Bits.RDRM7

#define RDRM_RDRM0_MASK                 1U
#define RDRM_RDRM1_MASK                 2U
#define RDRM_RDRM2_MASK                 4U
#define RDRM_RDRM3_MASK                 8U
#define RDRM_RDRM4_MASK                 16U
#define RDRM_RDRM5_MASK                 32U
#define RDRM_RDRM6_MASK                 64U
#define RDRM_RDRM7_MASK                 128U


/*** PERM - Port M Pull Device Enable Register; 0x00000254 ***/
typedef union {
  byte Byte;
  struct {
    byte PERM0       :1;                                       /* Pull Device Enable Port M Bit 0 */
    byte PERM1       :1;                                       /* Pull Device Enable Port M Bit 1 */
    byte PERM2       :1;                                       /* Pull Device Enable Port M Bit 2 */
    byte PERM3       :1;                                       /* Pull Device Enable Port M Bit 3 */
    byte PERM4       :1;                                       /* Pull Device Enable Port M Bit 4 */
    byte PERM5       :1;                                       /* Pull Device Enable Port M Bit 5 */
    byte PERM6       :1;                                       /* Pull Device Enable Port M Bit 6 */
    byte PERM7       :1;                                       /* Pull Device Enable Port M Bit 7 */
  } Bits;
} PERMSTR;
extern volatile PERMSTR _PERM @(REG_BASE + 0x00000254UL);
#define PERM                            _PERM.Byte
#define PERM_PERM0                      _PERM.Bits.PERM0
#define PERM_PERM1                      _PERM.Bits.PERM1
#define PERM_PERM2                      _PERM.Bits.PERM2
#define PERM_PERM3                      _PERM.Bits.PERM3
#define PERM_PERM4                      _PERM.Bits.PERM4
#define PERM_PERM5                      _PERM.Bits.PERM5
#define PERM_PERM6                      _PERM.Bits.PERM6
#define PERM_PERM7                      _PERM.Bits.PERM7

#define PERM_PERM0_MASK                 1U
#define PERM_PERM1_MASK                 2U
#define PERM_PERM2_MASK                 4U
#define PERM_PERM3_MASK                 8U
#define PERM_PERM4_MASK                 16U
#define PERM_PERM5_MASK                 32U
#define PERM_PERM6_MASK                 64U
#define PERM_PERM7_MASK                 128U


/*** PPSM - Port M Polarity Select Register; 0x00000255 ***/
typedef union {
  byte Byte;
  struct {
    byte PPSM0       :1;                                       /* Pull Select Port M Bit 0 */
    byte PPSM1       :1;                                       /* Pull Select Port M Bit 1 */
    byte PPSM2       :1;                                       /* Pull Select Port M Bit 2 */
    byte PPSM3       :1;                                       /* Pull Select Port M Bit 3 */
    byte PPSM4       :1;                                       /* Pull Select Port M Bit 4 */
    byte PPSM5       :1;                                       /* Pull Select Port M Bit 5 */
    byte PPSM6       :1;                                       /* Pull Select Port M Bit 6 */
    byte PPSM7       :1;                                       /* Pull Select Port M Bit 7 */
  } Bits;
} PPSMSTR;
extern volatile PPSMSTR _PPSM @(REG_BASE + 0x00000255UL);
#define PPSM                            _PPSM.Byte
#define PPSM_PPSM0                      _PPSM.Bits.PPSM0
#define PPSM_PPSM1                      _PPSM.Bits.PPSM1
#define PPSM_PPSM2                      _PPSM.Bits.PPSM2
#define PPSM_PPSM3                      _PPSM.Bits.PPSM3
#define PPSM_PPSM4                      _PPSM.Bits.PPSM4
#define PPSM_PPSM5                      _PPSM.Bits.PPSM5
#define PPSM_PPSM6                      _PPSM.Bits.PPSM6
#define PPSM_PPSM7                      _PPSM.Bits.PPSM7

#define PPSM_PPSM0_MASK                 1U
#define PPSM_PPSM1_MASK                 2U
#define PPSM_PPSM2_MASK                 4U
#define PPSM_PPSM3_MASK                 8U
#define PPSM_PPSM4_MASK                 16U
#define PPSM_PPSM5_MASK                 32U
#define PPSM_PPSM6_MASK                 64U
#define PPSM_PPSM7_MASK                 128U


/*** WOMM - Port M Wired-Or Mode Register; 0x00000256 ***/
typedef union {
  byte Byte;
  struct {
    byte WOMM0       :1;                                       /* Wired-Or Mode Port M Bit 0 */
    byte WOMM1       :1;                                       /* Wired-Or Mode Port M Bit 1 */
    byte WOMM2       :1;                                       /* Wired-Or Mode Port M Bit 2 */
    byte WOMM3       :1;                                       /* Wired-Or Mode Port M Bit 3 */
    byte WOMM4       :1;                                       /* Wired-Or Mode Port M Bit 4 */
    byte WOMM5       :1;                                       /* Wired-Or Mode Port M Bit 5 */
    byte WOMM6       :1;                                       /* Wired-Or Mode Port M Bit 6 */
    byte WOMM7       :1;                                       /* Wired-Or Mode Port M Bit 7 */
  } Bits;
} WOMMSTR;
extern volatile WOMMSTR _WOMM @(REG_BASE + 0x00000256UL);
#define WOMM                            _WOMM.Byte
#define WOMM_WOMM0                      _WOMM.Bits.WOMM0
#define WOMM_WOMM1                      _WOMM.Bits.WOMM1
#define WOMM_WOMM2                      _WOMM.Bits.WOMM2
#define WOMM_WOMM3                      _WOMM.Bits.WOMM3
#define WOMM_WOMM4                      _WOMM.Bits.WOMM4
#define WOMM_WOMM5                      _WOMM.Bits.WOMM5
#define WOMM_WOMM6                      _WOMM.Bits.WOMM6
#define WOMM_WOMM7                      _WOMM.Bits.WOMM7

#define WOMM_WOMM0_MASK                 1U
#define WOMM_WOMM1_MASK                 2U
#define WOMM_WOMM2_MASK                 4U
#define WOMM_WOMM3_MASK                 8U
#define WOMM_WOMM4_MASK                 16U
#define WOMM_WOMM5_MASK                 32U
#define WOMM_WOMM6_MASK                 64U
#define WOMM_WOMM7_MASK                 128U


/*** MODRR - Module Routing Register; 0x00000257 ***/
typedef union {
  byte Byte;
  struct {
    byte MODRR0      :1;                                       /* CAN0 Routing Bit 0 */
    byte MODRR1      :1;                                       /* CAN0 Routing Bit 1 */
    byte MODRR2      :1;                                       /* CAN4 Routing Bit 0 */
    byte MODRR3      :1;                                       /* CAN4 Routing Bit 1 */
    byte MODRR4      :1;                                       /* SPI0 Routing */
    byte MODRR5      :1;                                       /* SPI1 Routing */
    byte MODRR6      :1;                                       /* SPI2 Routing */
    byte             :1; 
  } Bits;
  struct {
    byte grpMODRR :7;
    byte         :1;
  } MergedBits;
} MODRRSTR;
extern volatile MODRRSTR _MODRR @(REG_BASE + 0x00000257UL);
#define MODRR                           _MODRR.Byte
#define MODRR_MODRR0                    _MODRR.Bits.MODRR0
#define MODRR_MODRR1                    _MODRR.Bits.MODRR1
#define MODRR_MODRR2                    _MODRR.Bits.MODRR2
#define MODRR_MODRR3                    _MODRR.Bits.MODRR3
#define MODRR_MODRR4                    _MODRR.Bits.MODRR4
#define MODRR_MODRR5                    _MODRR.Bits.MODRR5
#define MODRR_MODRR6                    _MODRR.Bits.MODRR6
#define MODRR_MODRR                     _MODRR.MergedBits.grpMODRR

#define MODRR_MODRR0_MASK               1U
#define MODRR_MODRR1_MASK               2U
#define MODRR_MODRR2_MASK               4U
#define MODRR_MODRR3_MASK               8U
#define MODRR_MODRR4_MASK               16U
#define MODRR_MODRR5_MASK               32U
#define MODRR_MODRR6_MASK               64U
#define MODRR_MODRR_MASK                127U
#define MODRR_MODRR_BITNUM              0U


/*** PTP - Port P I/O Register; 0x00000258 ***/
typedef union {
  byte Byte;
  struct {
    byte PTP0        :1;                                       /* Port P Bit 0 */
    byte PTP1        :1;                                       /* Port P Bit 1 */
    byte PTP2        :1;                                       /* Port P Bit 2 */
    byte PTP3        :1;                                       /* Port P Bit 3 */
    byte PTP4        :1;                                       /* Port P Bit 4 */
    byte PTP5        :1;                                       /* Port P Bit 5 */
    byte PTP6        :1;                                       /* Port P Bit 6 */
    byte PTP7        :1;                                       /* Port P Bit 7 */
  } Bits;
} PTPSTR;
extern volatile PTPSTR _PTP @(REG_BASE + 0x00000258UL);
#define PTP                             _PTP.Byte
#define PTP_PTP0                        _PTP.Bits.PTP0
#define PTP_PTP1                        _PTP.Bits.PTP1
#define PTP_PTP2                        _PTP.Bits.PTP2
#define PTP_PTP3                        _PTP.Bits.PTP3
#define PTP_PTP4                        _PTP.Bits.PTP4
#define PTP_PTP5                        _PTP.Bits.PTP5
#define PTP_PTP6                        _PTP.Bits.PTP6
#define PTP_PTP7                        _PTP.Bits.PTP7

#define PTP_PTP0_MASK                   1U
#define PTP_PTP1_MASK                   2U
#define PTP_PTP2_MASK                   4U
#define PTP_PTP3_MASK                   8U
#define PTP_PTP4_MASK                   16U
#define PTP_PTP5_MASK                   32U
#define PTP_PTP6_MASK                   64U
#define PTP_PTP7_MASK                   128U


/*** PTIP - Port P Input Register; 0x00000259 ***/
typedef union {
  byte Byte;
  struct {
    byte PTIP0       :1;                                       /* Port P Bit 0 */
    byte PTIP1       :1;                                       /* Port P Bit 1 */
    byte PTIP2       :1;                                       /* Port P Bit 2 */
    byte PTIP3       :1;                                       /* Port P Bit 3 */
    byte PTIP4       :1;                                       /* Port P Bit 4 */
    byte PTIP5       :1;                                       /* Port P Bit 5 */
    byte PTIP6       :1;                                       /* Port P Bit 6 */
    byte PTIP7       :1;                                       /* Port P Bit 7 */
  } Bits;
} PTIPSTR;
extern volatile PTIPSTR _PTIP @(REG_BASE + 0x00000259UL);
#define PTIP                            _PTIP.Byte
#define PTIP_PTIP0                      _PTIP.Bits.PTIP0
#define PTIP_PTIP1                      _PTIP.Bits.PTIP1
#define PTIP_PTIP2                      _PTIP.Bits.PTIP2
#define PTIP_PTIP3                      _PTIP.Bits.PTIP3
#define PTIP_PTIP4                      _PTIP.Bits.PTIP4
#define PTIP_PTIP5                      _PTIP.Bits.PTIP5
#define PTIP_PTIP6                      _PTIP.Bits.PTIP6
#define PTIP_PTIP7                      _PTIP.Bits.PTIP7

#define PTIP_PTIP0_MASK                 1U
#define PTIP_PTIP1_MASK                 2U
#define PTIP_PTIP2_MASK                 4U
#define PTIP_PTIP3_MASK                 8U
#define PTIP_PTIP4_MASK                 16U
#define PTIP_PTIP5_MASK                 32U
#define PTIP_PTIP6_MASK                 64U
#define PTIP_PTIP7_MASK                 128U


/*** DDRP - Port P Data Direction Register; 0x0000025A ***/
typedef union {
  byte Byte;
  struct {
    byte DDRP0       :1;                                       /* Data Direction Port P Bit 0 */
    byte DDRP1       :1;                                       /* Data Direction Port P Bit 1 */
    byte DDRP2       :1;                                       /* Data Direction Port P Bit 2 */
    byte DDRP3       :1;                                       /* Data Direction Port P Bit 3 */
    byte DDRP4       :1;                                       /* Data Direction Port P Bit 4 */
    byte DDRP5       :1;                                       /* Data Direction Port P Bit 5 */
    byte DDRP6       :1;                                       /* Data Direction Port P Bit 6 */
    byte DDRP7       :1;                                       /* Data Direction Port P Bit 7 */
  } Bits;
} DDRPSTR;
extern volatile DDRPSTR _DDRP @(REG_BASE + 0x0000025AUL);
#define DDRP                            _DDRP.Byte
#define DDRP_DDRP0                      _DDRP.Bits.DDRP0
#define DDRP_DDRP1                      _DDRP.Bits.DDRP1
#define DDRP_DDRP2                      _DDRP.Bits.DDRP2
#define DDRP_DDRP3                      _DDRP.Bits.DDRP3
#define DDRP_DDRP4                      _DDRP.Bits.DDRP4
#define DDRP_DDRP5                      _DDRP.Bits.DDRP5
#define DDRP_DDRP6                      _DDRP.Bits.DDRP6
#define DDRP_DDRP7                      _DDRP.Bits.DDRP7

#define DDRP_DDRP0_MASK                 1U
#define DDRP_DDRP1_MASK                 2U
#define DDRP_DDRP2_MASK                 4U
#define DDRP_DDRP3_MASK                 8U
#define DDRP_DDRP4_MASK                 16U
#define DDRP_DDRP5_MASK                 32U
#define DDRP_DDRP6_MASK                 64U
#define DDRP_DDRP7_MASK                 128U


/*** RDRP - Port P Reduced Drive Register; 0x0000025B ***/
typedef union {
  byte Byte;
  struct {
    byte RDRP0       :1;                                       /* Reduced Drive Port P Bit 0 */
    byte RDRP1       :1;                                       /* Reduced Drive Port P Bit 1 */
    byte RDRP2       :1;                                       /* Reduced Drive Port P Bit 2 */
    byte RDRP3       :1;                                       /* Reduced Drive Port P Bit 3 */
    byte RDRP4       :1;                                       /* Reduced Drive Port P Bit 4 */
    byte RDRP5       :1;                                       /* Reduced Drive Port P Bit 5 */
    byte RDRP6       :1;                                       /* Reduced Drive Port P Bit 6 */
    byte RDRP7       :1;                                       /* Reduced Drive Port P Bit 7 */
  } Bits;
} RDRPSTR;
extern volatile RDRPSTR _RDRP @(REG_BASE + 0x0000025BUL);
#define RDRP                            _RDRP.Byte
#define RDRP_RDRP0                      _RDRP.Bits.RDRP0
#define RDRP_RDRP1                      _RDRP.Bits.RDRP1
#define RDRP_RDRP2                      _RDRP.Bits.RDRP2
#define RDRP_RDRP3                      _RDRP.Bits.RDRP3
#define RDRP_RDRP4                      _RDRP.Bits.RDRP4
#define RDRP_RDRP5                      _RDRP.Bits.RDRP5
#define RDRP_RDRP6                      _RDRP.Bits.RDRP6
#define RDRP_RDRP7                      _RDRP.Bits.RDRP7

#define RDRP_RDRP0_MASK                 1U
#define RDRP_RDRP1_MASK                 2U
#define RDRP_RDRP2_MASK                 4U
#define RDRP_RDRP3_MASK                 8U
#define RDRP_RDRP4_MASK                 16U
#define RDRP_RDRP5_MASK                 32U
#define RDRP_RDRP6_MASK                 64U
#define RDRP_RDRP7_MASK                 128U


/*** PERP - Port P Pull Device Enable Register; 0x0000025C ***/
typedef union {
  byte Byte;
  struct {
    byte PERP0       :1;                                       /* Pull Device Enable Port P Bit 0 */
    byte PERP1       :1;                                       /* Pull Device Enable Port P Bit 1 */
    byte PERP2       :1;                                       /* Pull Device Enable Port P Bit 2 */
    byte PERP3       :1;                                       /* Pull Device Enable Port P Bit 3 */
    byte PERP4       :1;                                       /* Pull Device Enable Port P Bit 4 */
    byte PERP5       :1;                                       /* Pull Device Enable Port P Bit 5 */
    byte PERP6       :1;                                       /* Pull Device Enable Port P Bit 6 */
    byte PERP7       :1;                                       /* Pull Device Enable Port P Bit 7 */
  } Bits;
} PERPSTR;
extern volatile PERPSTR _PERP @(REG_BASE + 0x0000025CUL);
#define PERP                            _PERP.Byte
#define PERP_PERP0                      _PERP.Bits.PERP0
#define PERP_PERP1                      _PERP.Bits.PERP1
#define PERP_PERP2                      _PERP.Bits.PERP2
#define PERP_PERP3                      _PERP.Bits.PERP3
#define PERP_PERP4                      _PERP.Bits.PERP4
#define PERP_PERP5                      _PERP.Bits.PERP5
#define PERP_PERP6                      _PERP.Bits.PERP6
#define PERP_PERP7                      _PERP.Bits.PERP7

#define PERP_PERP0_MASK                 1U
#define PERP_PERP1_MASK                 2U
#define PERP_PERP2_MASK                 4U
#define PERP_PERP3_MASK                 8U
#define PERP_PERP4_MASK                 16U
#define PERP_PERP5_MASK                 32U
#define PERP_PERP6_MASK                 64U
#define PERP_PERP7_MASK                 128U


/*** PPSP - Port P Polarity Select Register; 0x0000025D ***/
typedef union {
  byte Byte;
  struct {
    byte PPSP0       :1;                                       /* Pull Select Port P Bit 0 */
    byte PPSP1       :1;                                       /* Pull Select Port P Bit 1 */
    byte PPSP2       :1;                                       /* Pull Select Port P Bit 2 */
    byte PPSP3       :1;                                       /* Pull Select Port P Bit 3 */
    byte PPSP4       :1;                                       /* Pull Select Port P Bit 4 */
    byte PPSP5       :1;                                       /* Pull Select Port P Bit 5 */
    byte PPSP6       :1;                                       /* Pull Select Port P Bit 6 */
    byte PPSP7       :1;                                       /* Pull Select Port P Bit 7 */
  } Bits;
} PPSPSTR;
extern volatile PPSPSTR _PPSP @(REG_BASE + 0x0000025DUL);
#define PPSP                            _PPSP.Byte
#define PPSP_PPSP0                      _PPSP.Bits.PPSP0
#define PPSP_PPSP1                      _PPSP.Bits.PPSP1
#define PPSP_PPSP2                      _PPSP.Bits.PPSP2
#define PPSP_PPSP3                      _PPSP.Bits.PPSP3
#define PPSP_PPSP4                      _PPSP.Bits.PPSP4
#define PPSP_PPSP5                      _PPSP.Bits.PPSP5
#define PPSP_PPSP6                      _PPSP.Bits.PPSP6
#define PPSP_PPSP7                      _PPSP.Bits.PPSP7

#define PPSP_PPSP0_MASK                 1U
#define PPSP_PPSP1_MASK                 2U
#define PPSP_PPSP2_MASK                 4U
#define PPSP_PPSP3_MASK                 8U
#define PPSP_PPSP4_MASK                 16U
#define PPSP_PPSP5_MASK                 32U
#define PPSP_PPSP6_MASK                 64U
#define PPSP_PPSP7_MASK                 128U


/*** PIEP - Port P Interrupt Enable Register; 0x0000025E ***/
typedef union {
  byte Byte;
  struct {
    byte PIEP0       :1;                                       /* Interrupt Enable Port P Bit 0 */
    byte PIEP1       :1;                                       /* Interrupt Enable Port P Bit 1 */
    byte PIEP2       :1;                                       /* Interrupt Enable Port P Bit 2 */
    byte PIEP3       :1;                                       /* Interrupt Enable Port P Bit 3 */
    byte PIEP4       :1;                                       /* Interrupt Enable Port P Bit 4 */
    byte PIEP5       :1;                                       /* Interrupt Enable Port P Bit 5 */
    byte PIEP6       :1;                                       /* Interrupt Enable Port P Bit 6 */
    byte PIEP7       :1;                                       /* Interrupt Enable Port P Bit 7 */
  } Bits;
} PIEPSTR;
extern volatile PIEPSTR _PIEP @(REG_BASE + 0x0000025EUL);
#define PIEP                            _PIEP.Byte
#define PIEP_PIEP0                      _PIEP.Bits.PIEP0
#define PIEP_PIEP1                      _PIEP.Bits.PIEP1
#define PIEP_PIEP2                      _PIEP.Bits.PIEP2
#define PIEP_PIEP3                      _PIEP.Bits.PIEP3
#define PIEP_PIEP4                      _PIEP.Bits.PIEP4
#define PIEP_PIEP5                      _PIEP.Bits.PIEP5
#define PIEP_PIEP6                      _PIEP.Bits.PIEP6
#define PIEP_PIEP7                      _PIEP.Bits.PIEP7

#define PIEP_PIEP0_MASK                 1U
#define PIEP_PIEP1_MASK                 2U
#define PIEP_PIEP2_MASK                 4U
#define PIEP_PIEP3_MASK                 8U
#define PIEP_PIEP4_MASK                 16U
#define PIEP_PIEP5_MASK                 32U
#define PIEP_PIEP6_MASK                 64U
#define PIEP_PIEP7_MASK                 128U


/*** PIFP - Port P Interrupt Flag Register; 0x0000025F ***/
typedef union {
  byte Byte;
  struct {
    byte PIFP0       :1;                                       /* Interrupt Flags Port P Bit 0 */
    byte PIFP1       :1;                                       /* Interrupt Flags Port P Bit 1 */
    byte PIFP2       :1;                                       /* Interrupt Flags Port P Bit 2 */
    byte PIFP3       :1;                                       /* Interrupt Flags Port P Bit 3 */
    byte PIFP4       :1;                                       /* Interrupt Flags Port P Bit 4 */
    byte PIFP5       :1;                                       /* Interrupt Flags Port P Bit 5 */
    byte PIFP6       :1;                                       /* Interrupt Flags Port P Bit 6 */
    byte PIFP7       :1;                                       /* Interrupt Flags Port P Bit 7 */
  } Bits;
} PIFPSTR;
extern volatile PIFPSTR _PIFP @(REG_BASE + 0x0000025FUL);
#define PIFP                            _PIFP.Byte
#define PIFP_PIFP0                      _PIFP.Bits.PIFP0
#define PIFP_PIFP1                      _PIFP.Bits.PIFP1
#define PIFP_PIFP2                      _PIFP.Bits.PIFP2
#define PIFP_PIFP3                      _PIFP.Bits.PIFP3
#define PIFP_PIFP4                      _PIFP.Bits.PIFP4
#define PIFP_PIFP5                      _PIFP.Bits.PIFP5
#define PIFP_PIFP6                      _PIFP.Bits.PIFP6
#define PIFP_PIFP7                      _PIFP.Bits.PIFP7

#define PIFP_PIFP0_MASK                 1U
#define PIFP_PIFP1_MASK                 2U
#define PIFP_PIFP2_MASK                 4U
#define PIFP_PIFP3_MASK                 8U
#define PIFP_PIFP4_MASK                 16U
#define PIFP_PIFP5_MASK                 32U
#define PIFP_PIFP6_MASK                 64U
#define PIFP_PIFP7_MASK                 128U


/*** PTH - Port H I/O Register; 0x00000260 ***/
typedef union {
  byte Byte;
  struct {
    byte PTH0        :1;                                       /* Port H Bit 0 */
    byte PTH1        :1;                                       /* Port H Bit 1 */
    byte PTH2        :1;                                       /* Port H Bit 2 */
    byte PTH3        :1;                                       /* Port H Bit 3 */
    byte PTH4        :1;                                       /* Port H Bit 4 */
    byte PTH5        :1;                                       /* Port H Bit 5 */
    byte PTH6        :1;                                       /* Port H Bit 6 */
    byte PTH7        :1;                                       /* Port H Bit 7 */
  } Bits;
} PTHSTR;
extern volatile PTHSTR _PTH @(REG_BASE + 0x00000260UL);
#define PTH                             _PTH.Byte
#define PTH_PTH0                        _PTH.Bits.PTH0
#define PTH_PTH1                        _PTH.Bits.PTH1
#define PTH_PTH2                        _PTH.Bits.PTH2
#define PTH_PTH3                        _PTH.Bits.PTH3
#define PTH_PTH4                        _PTH.Bits.PTH4
#define PTH_PTH5                        _PTH.Bits.PTH5
#define PTH_PTH6                        _PTH.Bits.PTH6
#define PTH_PTH7                        _PTH.Bits.PTH7

#define PTH_PTH0_MASK                   1U
#define PTH_PTH1_MASK                   2U
#define PTH_PTH2_MASK                   4U
#define PTH_PTH3_MASK                   8U
#define PTH_PTH4_MASK                   16U
#define PTH_PTH5_MASK                   32U
#define PTH_PTH6_MASK                   64U
#define PTH_PTH7_MASK                   128U


/*** PTIH - Port H Input Register; 0x00000261 ***/
typedef union {
  byte Byte;
  struct {
    byte PTIH0       :1;                                       /* Port H Bit 0 */
    byte PTIH1       :1;                                       /* Port H Bit 1 */
    byte PTIH2       :1;                                       /* Port H Bit 2 */
    byte PTIH3       :1;                                       /* Port H Bit 3 */
    byte PTIH4       :1;                                       /* Port H Bit 4 */
    byte PTIH5       :1;                                       /* Port H Bit 5 */
    byte PTIH6       :1;                                       /* Port H Bit 6 */
    byte PTIH7       :1;                                       /* Port H Bit 7 */
  } Bits;
} PTIHSTR;
extern volatile PTIHSTR _PTIH @(REG_BASE + 0x00000261UL);
#define PTIH                            _PTIH.Byte
#define PTIH_PTIH0                      _PTIH.Bits.PTIH0
#define PTIH_PTIH1                      _PTIH.Bits.PTIH1
#define PTIH_PTIH2                      _PTIH.Bits.PTIH2
#define PTIH_PTIH3                      _PTIH.Bits.PTIH3
#define PTIH_PTIH4                      _PTIH.Bits.PTIH4
#define PTIH_PTIH5                      _PTIH.Bits.PTIH5
#define PTIH_PTIH6                      _PTIH.Bits.PTIH6
#define PTIH_PTIH7                      _PTIH.Bits.PTIH7

#define PTIH_PTIH0_MASK                 1U
#define PTIH_PTIH1_MASK                 2U
#define PTIH_PTIH2_MASK                 4U
#define PTIH_PTIH3_MASK                 8U
#define PTIH_PTIH4_MASK                 16U
#define PTIH_PTIH5_MASK                 32U
#define PTIH_PTIH6_MASK                 64U
#define PTIH_PTIH7_MASK                 128U


/*** DDRH - Port H Data Direction Register; 0x00000262 ***/
typedef union {
  byte Byte;
  struct {
    byte DDRH0       :1;                                       /* Data Direction Port H Bit 0 */
    byte DDRH1       :1;                                       /* Data Direction Port H Bit 1 */
    byte DDRH2       :1;                                       /* Data Direction Port H Bit 2 */
    byte DDRH3       :1;                                       /* Data Direction Port H Bit 3 */
    byte DDRH4       :1;                                       /* Data Direction Port H Bit 4 */
    byte DDRH5       :1;                                       /* Data Direction Port H Bit 5 */
    byte DDRH6       :1;                                       /* Data Direction Port H Bit 6 */
    byte DDRH7       :1;                                       /* Data Direction Port H Bit 7 */
  } Bits;
} DDRHSTR;
extern volatile DDRHSTR _DDRH @(REG_BASE + 0x00000262UL);
#define DDRH                            _DDRH.Byte
#define DDRH_DDRH0                      _DDRH.Bits.DDRH0
#define DDRH_DDRH1                      _DDRH.Bits.DDRH1
#define DDRH_DDRH2                      _DDRH.Bits.DDRH2
#define DDRH_DDRH3                      _DDRH.Bits.DDRH3
#define DDRH_DDRH4                      _DDRH.Bits.DDRH4
#define DDRH_DDRH5                      _DDRH.Bits.DDRH5
#define DDRH_DDRH6                      _DDRH.Bits.DDRH6
#define DDRH_DDRH7                      _DDRH.Bits.DDRH7

#define DDRH_DDRH0_MASK                 1U
#define DDRH_DDRH1_MASK                 2U
#define DDRH_DDRH2_MASK                 4U
#define DDRH_DDRH3_MASK                 8U
#define DDRH_DDRH4_MASK                 16U
#define DDRH_DDRH5_MASK                 32U
#define DDRH_DDRH6_MASK                 64U
#define DDRH_DDRH7_MASK                 128U


/*** RDRH - Port H Reduced Drive Register; 0x00000263 ***/
typedef union {
  byte Byte;
  struct {
    byte RDRH0       :1;                                       /* Reduced Drive Port H Bit 0 */
    byte RDRH1       :1;                                       /* Reduced Drive Port H Bit 1 */
    byte RDRH2       :1;                                       /* Reduced Drive Port H Bit 2 */
    byte RDRH3       :1;                                       /* Reduced Drive Port H Bit 3 */
    byte RDRH4       :1;                                       /* Reduced Drive Port H Bit 4 */
    byte RDRH5       :1;                                       /* Reduced Drive Port H Bit 5 */
    byte RDRH6       :1;                                       /* Reduced Drive Port H Bit 6 */
    byte RDRH7       :1;                                       /* Reduced Drive Port H Bit 7 */
  } Bits;
} RDRHSTR;
extern volatile RDRHSTR _RDRH @(REG_BASE + 0x00000263UL);
#define RDRH                            _RDRH.Byte
#define RDRH_RDRH0                      _RDRH.Bits.RDRH0
#define RDRH_RDRH1                      _RDRH.Bits.RDRH1
#define RDRH_RDRH2                      _RDRH.Bits.RDRH2
#define RDRH_RDRH3                      _RDRH.Bits.RDRH3
#define RDRH_RDRH4                      _RDRH.Bits.RDRH4
#define RDRH_RDRH5                      _RDRH.Bits.RDRH5
#define RDRH_RDRH6                      _RDRH.Bits.RDRH6
#define RDRH_RDRH7                      _RDRH.Bits.RDRH7

#define RDRH_RDRH0_MASK                 1U
#define RDRH_RDRH1_MASK                 2U
#define RDRH_RDRH2_MASK                 4U
#define RDRH_RDRH3_MASK                 8U
#define RDRH_RDRH4_MASK                 16U
#define RDRH_RDRH5_MASK                 32U
#define RDRH_RDRH6_MASK                 64U
#define RDRH_RDRH7_MASK                 128U


/*** PERH - Port H Pull Device Enable Register; 0x00000264 ***/
typedef union {
  byte Byte;
  struct {
    byte PERH0       :1;                                       /* Pull Device Enable Port H Bit 0 */
    byte PERH1       :1;                                       /* Pull Device Enable Port H Bit 1 */
    byte PERH2       :1;                                       /* Pull Device Enable Port H Bit 2 */
    byte PERH3       :1;                                       /* Pull Device Enable Port H Bit 3 */
    byte PERH4       :1;                                       /* Pull Device Enable Port H Bit 4 */
    byte PERH5       :1;                                       /* Pull Device Enable Port H Bit 5 */
    byte PERH6       :1;                                       /* Pull Device Enable Port H Bit 6 */
    byte PERH7       :1;                                       /* Pull Device Enable Port H Bit 7 */
  } Bits;
} PERHSTR;
extern volatile PERHSTR _PERH @(REG_BASE + 0x00000264UL);
#define PERH                            _PERH.Byte
#define PERH_PERH0                      _PERH.Bits.PERH0
#define PERH_PERH1                      _PERH.Bits.PERH1
#define PERH_PERH2                      _PERH.Bits.PERH2
#define PERH_PERH3                      _PERH.Bits.PERH3
#define PERH_PERH4                      _PERH.Bits.PERH4
#define PERH_PERH5                      _PERH.Bits.PERH5
#define PERH_PERH6                      _PERH.Bits.PERH6
#define PERH_PERH7                      _PERH.Bits.PERH7

#define PERH_PERH0_MASK                 1U
#define PERH_PERH1_MASK                 2U
#define PERH_PERH2_MASK                 4U
#define PERH_PERH3_MASK                 8U
#define PERH_PERH4_MASK                 16U
#define PERH_PERH5_MASK                 32U
#define PERH_PERH6_MASK                 64U
#define PERH_PERH7_MASK                 128U


/*** PPSH - Port H Polarity Select Register; 0x00000265 ***/
typedef union {
  byte Byte;
  struct {
    byte PPSH0       :1;                                       /* Pull Select Port H Bit 0 */
    byte PPSH1       :1;                                       /* Pull Select Port H Bit 1 */
    byte PPSH2       :1;                                       /* Pull Select Port H Bit 2 */
    byte PPSH3       :1;                                       /* Pull Select Port H Bit 3 */
    byte PPSH4       :1;                                       /* Pull Select Port H Bit 4 */
    byte PPSH5       :1;                                       /* Pull Select Port H Bit 5 */
    byte PPSH6       :1;                                       /* Pull Select Port H Bit 6 */
    byte PPSH7       :1;                                       /* Pull Select Port H Bit 7 */
  } Bits;
} PPSHSTR;
extern volatile PPSHSTR _PPSH @(REG_BASE + 0x00000265UL);
#define PPSH                            _PPSH.Byte
#define PPSH_PPSH0                      _PPSH.Bits.PPSH0
#define PPSH_PPSH1                      _PPSH.Bits.PPSH1
#define PPSH_PPSH2                      _PPSH.Bits.PPSH2
#define PPSH_PPSH3                      _PPSH.Bits.PPSH3
#define PPSH_PPSH4                      _PPSH.Bits.PPSH4
#define PPSH_PPSH5                      _PPSH.Bits.PPSH5
#define PPSH_PPSH6                      _PPSH.Bits.PPSH6
#define PPSH_PPSH7                      _PPSH.Bits.PPSH7

#define PPSH_PPSH0_MASK                 1U
#define PPSH_PPSH1_MASK                 2U
#define PPSH_PPSH2_MASK                 4U
#define PPSH_PPSH3_MASK                 8U
#define PPSH_PPSH4_MASK                 16U
#define PPSH_PPSH5_MASK                 32U
#define PPSH_PPSH6_MASK                 64U
#define PPSH_PPSH7_MASK                 128U


/*** PIEH - Port H Interrupt Enable Register; 0x00000266 ***/
typedef union {
  byte Byte;
  struct {
    byte PIEH0       :1;                                       /* Interrupt Enable Port H Bit 0 */
    byte PIEH1       :1;                                       /* Interrupt Enable Port H Bit 1 */
    byte PIEH2       :1;                                       /* Interrupt Enable Port H Bit 2 */
    byte PIEH3       :1;                                       /* Interrupt Enable Port H Bit 3 */
    byte PIEH4       :1;                                       /* Interrupt Enable Port H Bit 4 */
    byte PIEH5       :1;                                       /* Interrupt Enable Port H Bit 5 */
    byte PIEH6       :1;                                       /* Interrupt Enable Port H Bit 6 */
    byte PIEH7       :1;                                       /* Interrupt Enable Port H Bit 7 */
  } Bits;
} PIEHSTR;
extern volatile PIEHSTR _PIEH @(REG_BASE + 0x00000266UL);
#define PIEH                            _PIEH.Byte
#define PIEH_PIEH0                      _PIEH.Bits.PIEH0
#define PIEH_PIEH1                      _PIEH.Bits.PIEH1
#define PIEH_PIEH2                      _PIEH.Bits.PIEH2
#define PIEH_PIEH3                      _PIEH.Bits.PIEH3
#define PIEH_PIEH4                      _PIEH.Bits.PIEH4
#define PIEH_PIEH5                      _PIEH.Bits.PIEH5
#define PIEH_PIEH6                      _PIEH.Bits.PIEH6
#define PIEH_PIEH7                      _PIEH.Bits.PIEH7

#define PIEH_PIEH0_MASK                 1U
#define PIEH_PIEH1_MASK                 2U
#define PIEH_PIEH2_MASK                 4U
#define PIEH_PIEH3_MASK                 8U
#define PIEH_PIEH4_MASK                 16U
#define PIEH_PIEH5_MASK                 32U
#define PIEH_PIEH6_MASK                 64U
#define PIEH_PIEH7_MASK                 128U


/*** PIFH - Port H Interrupt Flag Register; 0x00000267 ***/
typedef union {
  byte Byte;
  struct {
    byte PIFH0       :1;                                       /* Interrupt Flags Port H Bit 0 */
    byte PIFH1       :1;                                       /* Interrupt Flags Port H Bit 1 */
    byte PIFH2       :1;                                       /* Interrupt Flags Port H Bit 2 */
    byte PIFH3       :1;                                       /* Interrupt Flags Port H Bit 3 */
    byte PIFH4       :1;                                       /* Interrupt Flags Port H Bit 4 */
    byte PIFH5       :1;                                       /* Interrupt Flags Port H Bit 5 */
    byte PIFH6       :1;                                       /* Interrupt Flags Port H Bit 6 */
    byte PIFH7       :1;                                       /* Interrupt Flags Port H Bit 7 */
  } Bits;
} PIFHSTR;
extern volatile PIFHSTR _PIFH @(REG_BASE + 0x00000267UL);
#define PIFH                            _PIFH.Byte
#define PIFH_PIFH0                      _PIFH.Bits.PIFH0
#define PIFH_PIFH1                      _PIFH.Bits.PIFH1
#define PIFH_PIFH2                      _PIFH.Bits.PIFH2
#define PIFH_PIFH3                      _PIFH.Bits.PIFH3
#define PIFH_PIFH4                      _PIFH.Bits.PIFH4
#define PIFH_PIFH5                      _PIFH.Bits.PIFH5
#define PIFH_PIFH6                      _PIFH.Bits.PIFH6
#define PIFH_PIFH7                      _PIFH.Bits.PIFH7

#define PIFH_PIFH0_MASK                 1U
#define PIFH_PIFH1_MASK                 2U
#define PIFH_PIFH2_MASK                 4U
#define PIFH_PIFH3_MASK                 8U
#define PIFH_PIFH4_MASK                 16U
#define PIFH_PIFH5_MASK                 32U
#define PIFH_PIFH6_MASK                 64U
#define PIFH_PIFH7_MASK                 128U


/*** PTJ - Port J I/O Register; 0x00000268 ***/
typedef union {
  byte Byte;
  struct {
    byte PTJ0        :1;                                       /* Port J Bit 0 */
    byte PTJ1        :1;                                       /* Port J Bit 1 */
    byte             :1; 
    byte             :1; 
    byte             :1; 
    byte             :1; 
    byte PTJ6        :1;                                       /* Port J Bit 6 */
    byte PTJ7        :1;                                       /* Port J Bit 7 */
  } Bits;
  struct {
    byte grpPTJ  :2;
    byte         :1;
    byte         :1;
    byte         :1;
    byte         :1;
    byte grpPTJ_6 :2;
  } MergedBits;
} PTJSTR;
extern volatile PTJSTR _PTJ @(REG_BASE + 0x00000268UL);
#define PTJ                             _PTJ.Byte
#define PTJ_PTJ0                        _PTJ.Bits.PTJ0
#define PTJ_PTJ1                        _PTJ.Bits.PTJ1
#define PTJ_PTJ6                        _PTJ.Bits.PTJ6
#define PTJ_PTJ7                        _PTJ.Bits.PTJ7
#define PTJ_PTJ                         _PTJ.MergedBits.grpPTJ
#define PTJ_PTJ_6                       _PTJ.MergedBits.grpPTJ_6

#define PTJ_PTJ0_MASK                   1U
#define PTJ_PTJ1_MASK                   2U
#define PTJ_PTJ6_MASK                   64U
#define PTJ_PTJ7_MASK                   128U
#define PTJ_PTJ_MASK                    3U
#define PTJ_PTJ_BITNUM                  0U
#define PTJ_PTJ_6_MASK                  192U
#define PTJ_PTJ_6_BITNUM                6U


/*** PTIJ - Port J Input Register; 0x00000269 ***/
typedef union {
  byte Byte;
  struct {
    byte PTIJ0       :1;                                       /* Port J Bit 0 */
    byte PTIJ1       :1;                                       /* Port J Bit 1 */
    byte             :1; 
    byte             :1; 
    byte             :1; 
    byte             :1; 
    byte PTIJ6       :1;                                       /* Port J Bit 6 */
    byte PTIJ7       :1;                                       /* Port J Bit 7 */
  } Bits;
  struct {
    byte grpPTIJ :2;
    byte         :1;
    byte         :1;
    byte         :1;
    byte         :1;
    byte grpPTIJ_6 :2;
  } MergedBits;
} PTIJSTR;
extern volatile PTIJSTR _PTIJ @(REG_BASE + 0x00000269UL);
#define PTIJ                            _PTIJ.Byte
#define PTIJ_PTIJ0                      _PTIJ.Bits.PTIJ0
#define PTIJ_PTIJ1                      _PTIJ.Bits.PTIJ1
#define PTIJ_PTIJ6                      _PTIJ.Bits.PTIJ6
#define PTIJ_PTIJ7                      _PTIJ.Bits.PTIJ7
#define PTIJ_PTIJ                       _PTIJ.MergedBits.grpPTIJ
#define PTIJ_PTIJ_6                     _PTIJ.MergedBits.grpPTIJ_6

#define PTIJ_PTIJ0_MASK                 1U
#define PTIJ_PTIJ1_MASK                 2U
#define PTIJ_PTIJ6_MASK                 64U
#define PTIJ_PTIJ7_MASK                 128U
#define PTIJ_PTIJ_MASK                  3U
#define PTIJ_PTIJ_BITNUM                0U
#define PTIJ_PTIJ_6_MASK                192U
#define PTIJ_PTIJ_6_BITNUM              6U


/*** DDRJ - Port J Data Direction Register; 0x0000026A ***/
typedef union {
  byte Byte;
  struct {
    byte DDRJ0       :1;                                       /* Data Direction Port J Bit 0 */
    byte DDRJ1       :1;                                       /* Data Direction Port J Bit 1 */
    byte             :1; 
    byte             :1; 
    byte             :1; 
    byte             :1; 
    byte DDRJ6       :1;                                       /* Data Direction Port J Bit 6 */
    byte DDRJ7       :1;                                       /* Data Direction Port J Bit 7 */
  } Bits;
  struct {
    byte grpDDRJ :2;
    byte         :1;
    byte         :1;
    byte         :1;
    byte         :1;
    byte grpDDRJ_6 :2;
  } MergedBits;
} DDRJSTR;
extern volatile DDRJSTR _DDRJ @(REG_BASE + 0x0000026AUL);
#define DDRJ                            _DDRJ.Byte
#define DDRJ_DDRJ0                      _DDRJ.Bits.DDRJ0
#define DDRJ_DDRJ1                      _DDRJ.Bits.DDRJ1
#define DDRJ_DDRJ6                      _DDRJ.Bits.DDRJ6
#define DDRJ_DDRJ7                      _DDRJ.Bits.DDRJ7
#define DDRJ_DDRJ                       _DDRJ.MergedBits.grpDDRJ
#define DDRJ_DDRJ_6                     _DDRJ.MergedBits.grpDDRJ_6

#define DDRJ_DDRJ0_MASK                 1U
#define DDRJ_DDRJ1_MASK                 2U
#define DDRJ_DDRJ6_MASK                 64U
#define DDRJ_DDRJ7_MASK                 128U
#define DDRJ_DDRJ_MASK                  3U
#define DDRJ_DDRJ_BITNUM                0U
#define DDRJ_DDRJ_6_MASK                192U
#define DDRJ_DDRJ_6_BITNUM              6U


/*** RDRJ - Port J Reduced Drive Register; 0x0000026B ***/
typedef union {
  byte Byte;
  struct {
    byte RDRJ0       :1;                                       /* Reduced Drive Port J Bit 0 */
    byte RDRJ1       :1;                                       /* Reduced Drive Port J Bit 1 */
    byte             :1; 
    byte             :1; 
    byte             :1; 
    byte             :1; 
    byte RDRJ6       :1;                                       /* Reduced Drive Port J Bit 6 */
    byte RDRJ7       :1;                                       /* Reduced Drive Port J Bit 7 */
  } Bits;
  struct {
    byte grpRDRJ :2;
    byte         :1;
    byte         :1;
    byte         :1;
    byte         :1;
    byte grpRDRJ_6 :2;
  } MergedBits;
} RDRJSTR;
extern volatile RDRJSTR _RDRJ @(REG_BASE + 0x0000026BUL);
#define RDRJ                            _RDRJ.Byte
#define RDRJ_RDRJ0                      _RDRJ.Bits.RDRJ0
#define RDRJ_RDRJ1                      _RDRJ.Bits.RDRJ1
#define RDRJ_RDRJ6                      _RDRJ.Bits.RDRJ6
#define RDRJ_RDRJ7                      _RDRJ.Bits.RDRJ7
#define RDRJ_RDRJ                       _RDRJ.MergedBits.grpRDRJ
#define RDRJ_RDRJ_6                     _RDRJ.MergedBits.grpRDRJ_6

#define RDRJ_RDRJ0_MASK                 1U
#define RDRJ_RDRJ1_MASK                 2U
#define RDRJ_RDRJ6_MASK                 64U
#define RDRJ_RDRJ7_MASK                 128U
#define RDRJ_RDRJ_MASK                  3U
#define RDRJ_RDRJ_BITNUM                0U
#define RDRJ_RDRJ_6_MASK                192U
#define RDRJ_RDRJ_6_BITNUM              6U


/*** PERJ - Port J Pull Device Enable Register; 0x0000026C ***/
typedef union {
  byte Byte;
  struct {
    byte PERJ0       :1;                                       /* Pull Device Enable Port J Bit 0 */
    byte PERJ1       :1;                                       /* Pull Device Enable Port J Bit 1 */
    byte             :1; 
    byte             :1; 
    byte             :1; 
    byte             :1; 
    byte PERJ6       :1;                                       /* Pull Device Enable Port J Bit 6 */
    byte PERJ7       :1;                                       /* Pull Device Enable Port J Bit 7 */
  } Bits;
  struct {
    byte grpPERJ :2;
    byte         :1;
    byte         :1;
    byte         :1;
    byte         :1;
    byte grpPERJ_6 :2;
  } MergedBits;
} PERJSTR;
extern volatile PERJSTR _PERJ @(REG_BASE + 0x0000026CUL);
#define PERJ                            _PERJ.Byte
#define PERJ_PERJ0                      _PERJ.Bits.PERJ0
#define PERJ_PERJ1                      _PERJ.Bits.PERJ1
#define PERJ_PERJ6                      _PERJ.Bits.PERJ6
#define PERJ_PERJ7                      _PERJ.Bits.PERJ7
#define PERJ_PERJ                       _PERJ.MergedBits.grpPERJ
#define PERJ_PERJ_6                     _PERJ.MergedBits.grpPERJ_6

#define PERJ_PERJ0_MASK                 1U
#define PERJ_PERJ1_MASK                 2U
#define PERJ_PERJ6_MASK                 64U
#define PERJ_PERJ7_MASK                 128U
#define PERJ_PERJ_MASK                  3U
#define PERJ_PERJ_BITNUM                0U
#define PERJ_PERJ_6_MASK                192U
#define PERJ_PERJ_6_BITNUM              6U


/*** PPSJ - Port J Polarity Select Register; 0x0000026D ***/
typedef union {
  byte Byte;
  struct {
    byte PPSJ0       :1;                                       /* Pull Select Port J Bit 0 */
    byte PPSJ1       :1;                                       /* Pull Select Port J Bit 1 */
    byte             :1; 
    byte             :1; 
    byte             :1; 
    byte             :1; 
    byte PPSJ6       :1;                                       /* Pull Select Port J Bit 6 */
    byte PPSJ7       :1;                                       /* Pull Select Port J Bit 7 */
  } Bits;
  struct {
    byte grpPPSJ :2;
    byte         :1;
    byte         :1;
    byte         :1;
    byte         :1;
    byte grpPPSJ_6 :2;
  } MergedBits;
} PPSJSTR;
extern volatile PPSJSTR _PPSJ @(REG_BASE + 0x0000026DUL);
#define PPSJ                            _PPSJ.Byte
#define PPSJ_PPSJ0                      _PPSJ.Bits.PPSJ0
#define PPSJ_PPSJ1                      _PPSJ.Bits.PPSJ1
#define PPSJ_PPSJ6                      _PPSJ.Bits.PPSJ6
#define PPSJ_PPSJ7                      _PPSJ.Bits.PPSJ7
#define PPSJ_PPSJ                       _PPSJ.MergedBits.grpPPSJ
#define PPSJ_PPSJ_6                     _PPSJ.MergedBits.grpPPSJ_6

#define PPSJ_PPSJ0_MASK                 1U
#define PPSJ_PPSJ1_MASK                 2U
#define PPSJ_PPSJ6_MASK                 64U
#define PPSJ_PPSJ7_MASK                 128U
#define PPSJ_PPSJ_MASK                  3U
#define PPSJ_PPSJ_BITNUM                0U
#define PPSJ_PPSJ_6_MASK                192U
#define PPSJ_PPSJ_6_BITNUM              6U


/*** PIEJ - Port J Interrupt Enable Register; 0x0000026E ***/
typedef union {
  byte Byte;
  struct {
    byte PIEJ0       :1;                                       /* Interrupt Enable Port J Bit 0 */
    byte PIEJ1       :1;                                       /* Interrupt Enable Port J Bit 1 */
    byte             :1; 
    byte             :1; 
    byte             :1; 
    byte             :1; 
    byte PIEJ6       :1;                                       /* Interrupt Enable Port J Bit 6 */
    byte PIEJ7       :1;                                       /* Interrupt Enable Port J Bit 7 */
  } Bits;
  struct {
    byte grpPIEJ :2;
    byte         :1;
    byte         :1;
    byte         :1;
    byte         :1;
    byte grpPIEJ_6 :2;
  } MergedBits;
} PIEJSTR;
extern volatile PIEJSTR _PIEJ @(REG_BASE + 0x0000026EUL);
#define PIEJ                            _PIEJ.Byte
#define PIEJ_PIEJ0                      _PIEJ.Bits.PIEJ0
#define PIEJ_PIEJ1                      _PIEJ.Bits.PIEJ1
#define PIEJ_PIEJ6                      _PIEJ.Bits.PIEJ6
#define PIEJ_PIEJ7                      _PIEJ.Bits.PIEJ7
#define PIEJ_PIEJ                       _PIEJ.MergedBits.grpPIEJ
#define PIEJ_PIEJ_6                     _PIEJ.MergedBits.grpPIEJ_6

#define PIEJ_PIEJ0_MASK                 1U
#define PIEJ_PIEJ1_MASK                 2U
#define PIEJ_PIEJ6_MASK                 64U
#define PIEJ_PIEJ7_MASK                 128U
#define PIEJ_PIEJ_MASK                  3U
#define PIEJ_PIEJ_BITNUM                0U
#define PIEJ_PIEJ_6_MASK                192U
#define PIEJ_PIEJ_6_BITNUM              6U


/*** PIFJ - Port J Interrupt Flag Register; 0x0000026F ***/
typedef union {
  byte Byte;
  struct {
    byte PIFJ0       :1;                                       /* Interrupt Flags Port J Bit 0 */
    byte PIFJ1       :1;                                       /* Interrupt Flags Port J Bit 1 */
    byte             :1; 
    byte             :1; 
    byte             :1; 
    byte             :1; 
    byte PIFJ6       :1;                                       /* Interrupt Flags Port J Bit 6 */
    byte PIFJ7       :1;                                       /* Interrupt Flags Port J Bit 7 */
  } Bits;
  struct {
    byte grpPIFJ :2;
    byte         :1;
    byte         :1;
    byte         :1;
    byte         :1;
    byte grpPIFJ_6 :2;
  } MergedBits;
} PIFJSTR;
extern volatile PIFJSTR _PIFJ @(REG_BASE + 0x0000026FUL);
#define PIFJ                            _PIFJ.Byte
#define PIFJ_PIFJ0                      _PIFJ.Bits.PIFJ0
#define PIFJ_PIFJ1                      _PIFJ.Bits.PIFJ1
#define PIFJ_PIFJ6                      _PIFJ.Bits.PIFJ6
#define PIFJ_PIFJ7                      _PIFJ.Bits.PIFJ7
#define PIFJ_PIFJ                       _PIFJ.MergedBits.grpPIFJ
#define PIFJ_PIFJ_6                     _PIFJ.MergedBits.grpPIFJ_6

#define PIFJ_PIFJ0_MASK                 1U
#define PIFJ_PIFJ1_MASK                 2U
#define PIFJ_PIFJ6_MASK                 64U
#define PIFJ_PIFJ7_MASK                 128U
#define PIFJ_PIFJ_MASK                  3U
#define PIFJ_PIFJ_BITNUM                0U
#define PIFJ_PIFJ_6_MASK                192U
#define PIFJ_PIFJ_6_BITNUM              6U


/*** CAN4CTL0 - MSCAN4 Control 0 Register; 0x00000280 ***/
typedef union {
  byte Byte;
  struct {
    byte INITRQ      :1;                                       /* Initialization Mode Request */
    byte SLPRQ       :1;                                       /* Sleep Mode Request */
    byte WUPE        :1;                                       /* Wake-Up Enable */
    byte TIME        :1;                                       /* Timer Enable */
    byte SYNCH       :1;                                       /* Synchronized Status */
    byte CSWAI       :1;                                       /* CAN Stops in Wait Mode */
    byte RXACT       :1;                                       /* Receiver Active Status */
    byte RXFRM       :1;                                       /* Received Frame Flag */
  } Bits;
} CAN4CTL0STR;
extern volatile CAN4CTL0STR _CAN4CTL0 @(REG_BASE + 0x00000280UL);
#define CAN4CTL0                        _CAN4CTL0.Byte
#define CAN4CTL0_INITRQ                 _CAN4CTL0.Bits.INITRQ
#define CAN4CTL0_SLPRQ                  _CAN4CTL0.Bits.SLPRQ
#define CAN4CTL0_WUPE                   _CAN4CTL0.Bits.WUPE
#define CAN4CTL0_TIME                   _CAN4CTL0.Bits.TIME
#define CAN4CTL0_SYNCH                  _CAN4CTL0.Bits.SYNCH
#define CAN4CTL0_CSWAI                  _CAN4CTL0.Bits.CSWAI
#define CAN4CTL0_RXACT                  _CAN4CTL0.Bits.RXACT
#define CAN4CTL0_RXFRM                  _CAN4CTL0.Bits.RXFRM
/* CAN4CTL_ARR: Access 2 CAN4CTLx registers in an array */
#define CAN4CTL_ARR                     ((volatile byte *) &CAN4CTL0)

#define CAN4CTL0_INITRQ_MASK            1U
#define CAN4CTL0_SLPRQ_MASK             2U
#define CAN4CTL0_WUPE_MASK              4U
#define CAN4CTL0_TIME_MASK              8U
#define CAN4CTL0_SYNCH_MASK             16U
#define CAN4CTL0_CSWAI_MASK             32U
#define CAN4CTL0_RXACT_MASK             64U
#define CAN4CTL0_RXFRM_MASK             128U


/*** CAN4CTL1 - MSCAN4 Control 1 Register; 0x00000281 ***/
typedef union {
  byte Byte;
  struct {
    byte INITAK      :1;                                       /* Initialization Mode Acknowledge */
    byte SLPAK       :1;                                       /* Sleep Mode Acknowledge */
    byte WUPM        :1;                                       /* Wake-Up Mode */
    byte             :1; 
    byte LISTEN      :1;                                       /* Listen Only Mode */
    byte LOOPB       :1;                                       /* Loop Back Self Test Mode */
    byte CLKSRC      :1;                                       /* MSCAN4 Clock Source */
    byte CANE        :1;                                       /* MSCAN4 Enable */
  } Bits;
} CAN4CTL1STR;
extern volatile CAN4CTL1STR _CAN4CTL1 @(REG_BASE + 0x00000281UL);
#define CAN4CTL1                        _CAN4CTL1.Byte
#define CAN4CTL1_INITAK                 _CAN4CTL1.Bits.INITAK
#define CAN4CTL1_SLPAK                  _CAN4CTL1.Bits.SLPAK
#define CAN4CTL1_WUPM                   _CAN4CTL1.Bits.WUPM
#define CAN4CTL1_LISTEN                 _CAN4CTL1.Bits.LISTEN
#define CAN4CTL1_LOOPB                  _CAN4CTL1.Bits.LOOPB
#define CAN4CTL1_CLKSRC                 _CAN4CTL1.Bits.CLKSRC
#define CAN4CTL1_CANE                   _CAN4CTL1.Bits.CANE

#define CAN4CTL1_INITAK_MASK            1U
#define CAN4CTL1_SLPAK_MASK             2U
#define CAN4CTL1_WUPM_MASK              4U
#define CAN4CTL1_LISTEN_MASK            16U
#define CAN4CTL1_LOOPB_MASK             32U
#define CAN4CTL1_CLKSRC_MASK            64U
#define CAN4CTL1_CANE_MASK              128U


/*** CAN4BTR0 - MSCAN4 Bus Timing Register 0; 0x00000282 ***/
typedef union {
  byte Byte;
  struct {
    byte BRP0        :1;                                       /* Baud Rate Prescaler 0 */
    byte BRP1        :1;                                       /* Baud Rate Prescaler 1 */
    byte BRP2        :1;                                       /* Baud Rate Prescaler 2 */
    byte BRP3        :1;                                       /* Baud Rate Prescaler 3 */
    byte BRP4        :1;                                       /* Baud Rate Prescaler 4 */
    byte BRP5        :1;                                       /* Baud Rate Prescaler 5 */
    byte SJW0        :1;                                       /* Synchronization Jump Width 0 */
    byte SJW1        :1;                                       /* Synchronization Jump Width 1 */
  } Bits;
  struct {
    byte grpBRP  :6;
    byte grpSJW  :2;
  } MergedBits;
} CAN4BTR0STR;
extern volatile CAN4BTR0STR _CAN4BTR0 @(REG_BASE + 0x00000282UL);
#define CAN4BTR0                        _CAN4BTR0.Byte
#define CAN4BTR0_BRP0                   _CAN4BTR0.Bits.BRP0
#define CAN4BTR0_BRP1                   _CAN4BTR0.Bits.BRP1
#define CAN4BTR0_BRP2                   _CAN4BTR0.Bits.BRP2
#define CAN4BTR0_BRP3                   _CAN4BTR0.Bits.BRP3
#define CAN4BTR0_BRP4                   _CAN4BTR0.Bits.BRP4
#define CAN4BTR0_BRP5                   _CAN4BTR0.Bits.BRP5
#define CAN4BTR0_SJW0                   _CAN4BTR0.Bits.SJW0
#define CAN4BTR0_SJW1                   _CAN4BTR0.Bits.SJW1
/* CAN4BTR_ARR: Access 2 CAN4BTRx registers in an array */
#define CAN4BTR_ARR                     ((volatile byte *) &CAN4BTR0)
#define CAN4BTR0_BRP                    _CAN4BTR0.MergedBits.grpBRP
#define CAN4BTR0_SJW                    _CAN4BTR0.MergedBits.grpSJW

#define CAN4BTR0_BRP0_MASK              1U
#define CAN4BTR0_BRP1_MASK              2U
#define CAN4BTR0_BRP2_MASK              4U
#define CAN4BTR0_BRP3_MASK              8U
#define CAN4BTR0_BRP4_MASK              16U
#define CAN4BTR0_BRP5_MASK              32U
#define CAN4BTR0_SJW0_MASK              64U
#define CAN4BTR0_SJW1_MASK              128U
#define CAN4BTR0_BRP_MASK               63U
#define CAN4BTR0_BRP_BITNUM             0U
#define CAN4BTR0_SJW_MASK               192U
#define CAN4BTR0_SJW_BITNUM             6U


/*** CAN4BTR1 - MSCAN4 Bus Timing Register 1; 0x00000283 ***/
typedef union {
  byte Byte;
  struct {
    byte TSEG10      :1;                                       /* Time Segment 10 */
    byte TSEG11      :1;                                       /* Time Segment 11 */
    byte TSEG12      :1;                                       /* Time Segment 12 */
    byte TSEG13      :1;                                       /* Time Segment 13 */
    byte TSEG20      :1;                                       /* Time Segment 20 */
    byte TSEG21      :1;                                       /* Time Segment 21 */
    byte TSEG22      :1;                                       /* Time Segment 22 */
    byte SAMP        :1;                                       /* Sampling */
  } Bits;
  struct {
    byte grpTSEG_10 :4;
    byte grpTSEG_20 :3;
    byte         :1;
  } MergedBits;
} CAN4BTR1STR;
extern volatile CAN4BTR1STR _CAN4BTR1 @(REG_BASE + 0x00000283UL);
#define CAN4BTR1                        _CAN4BTR1.Byte
#define CAN4BTR1_TSEG10                 _CAN4BTR1.Bits.TSEG10
#define CAN4BTR1_TSEG11                 _CAN4BTR1.Bits.TSEG11
#define CAN4BTR1_TSEG12                 _CAN4BTR1.Bits.TSEG12
#define CAN4BTR1_TSEG13                 _CAN4BTR1.Bits.TSEG13
#define CAN4BTR1_TSEG20                 _CAN4BTR1.Bits.TSEG20
#define CAN4BTR1_TSEG21                 _CAN4BTR1.Bits.TSEG21
#define CAN4BTR1_TSEG22                 _CAN4BTR1.Bits.TSEG22
#define CAN4BTR1_SAMP                   _CAN4BTR1.Bits.SAMP
#define CAN4BTR1_TSEG_10                _CAN4BTR1.MergedBits.grpTSEG_10
#define CAN4BTR1_TSEG_20                _CAN4BTR1.MergedBits.grpTSEG_20
#define CAN4BTR1_TSEG                   CAN4BTR1_TSEG_10

#define CAN4BTR1_TSEG10_MASK            1U
#define CAN4BTR1_TSEG11_MASK            2U
#define CAN4BTR1_TSEG12_MASK            4U
#define CAN4BTR1_TSEG13_MASK            8U
#define CAN4BTR1_TSEG20_MASK            16U
#define CAN4BTR1_TSEG21_MASK            32U
#define CAN4BTR1_TSEG22_MASK            64U
#define CAN4BTR1_SAMP_MASK              128U
#define CAN4BTR1_TSEG_10_MASK           15U
#define CAN4BTR1_TSEG_10_BITNUM         0U
#define CAN4BTR1_TSEG_20_MASK           112U
#define CAN4BTR1_TSEG_20_BITNUM         4U


/*** CAN4RFLG - MSCAN4 Receiver Flag Register; 0x00000284 ***/
typedef union {
  byte Byte;
  struct {
    byte RXF         :1;                                       /* Receive Buffer Full */
    byte OVRIF       :1;                                       /* Overrun Interrupt Flag */
    byte TSTAT0      :1;                                       /* Transmitter Status Bit 0 */
    byte TSTAT1      :1;                                       /* Transmitter Status Bit 1 */
    byte RSTAT0      :1;                                       /* Receiver Status Bit 0 */
    byte RSTAT1      :1;                                       /* Receiver Status Bit 1 */
    byte CSCIF       :1;                                       /* CAN Status Change Interrupt Flag */
    byte WUPIF       :1;                                       /* Wake-up Interrupt Flag */
  } Bits;
  struct {
    byte         :1;
    byte         :1;
    byte grpTSTAT :2;
    byte grpRSTAT :2;
    byte         :1;
    byte         :1;
  } MergedBits;
} CAN4RFLGSTR;
extern volatile CAN4RFLGSTR _CAN4RFLG @(REG_BASE + 0x00000284UL);
#define CAN4RFLG                        _CAN4RFLG.Byte
#define CAN4RFLG_RXF                    _CAN4RFLG.Bits.RXF
#define CAN4RFLG_OVRIF                  _CAN4RFLG.Bits.OVRIF
#define CAN4RFLG_TSTAT0                 _CAN4RFLG.Bits.TSTAT0
#define CAN4RFLG_TSTAT1                 _CAN4RFLG.Bits.TSTAT1
#define CAN4RFLG_RSTAT0                 _CAN4RFLG.Bits.RSTAT0
#define CAN4RFLG_RSTAT1                 _CAN4RFLG.Bits.RSTAT1
#define CAN4RFLG_CSCIF                  _CAN4RFLG.Bits.CSCIF
#define CAN4RFLG_WUPIF                  _CAN4RFLG.Bits.WUPIF
#define CAN4RFLG_TSTAT                  _CAN4RFLG.MergedBits.grpTSTAT
#define CAN4RFLG_RSTAT                  _CAN4RFLG.MergedBits.grpRSTAT

#define CAN4RFLG_RXF_MASK               1U
#define CAN4RFLG_OVRIF_MASK             2U
#define CAN4RFLG_TSTAT0_MASK            4U
#define CAN4RFLG_TSTAT1_MASK            8U
#define CAN4RFLG_RSTAT0_MASK            16U
#define CAN4RFLG_RSTAT1_MASK            32U
#define CAN4RFLG_CSCIF_MASK             64U
#define CAN4RFLG_WUPIF_MASK             128U
#define CAN4RFLG_TSTAT_MASK             12U
#define CAN4RFLG_TSTAT_BITNUM           2U
#define CAN4RFLG_RSTAT_MASK             48U
#define CAN4RFLG_RSTAT_BITNUM           4U


/*** CAN4RIER - MSCAN4 Receiver Interrupt Enable Register; 0x00000285 ***/
typedef union {
  byte Byte;
  struct {
    byte RXFIE       :1;                                       /* Receiver Full Interrupt Enable */
    byte OVRIE       :1;                                       /* Overrun Interrupt Enable */
    byte TSTATE0     :1;                                       /* Transmitter Status Change Enable 0 */
    byte TSTATE1     :1;                                       /* Transmitter Status Change Enable 1 */
    byte RSTATE0     :1;                                       /* Receiver Status Change Enable 0 */
    byte RSTATE1     :1;                                       /* Receiver Status Change Enable 1 */
    byte CSCIE       :1;                                       /* CAN Status Change Interrupt Enable */
    byte WUPIE       :1;                                       /* Wake-up Interrupt Enable */
  } Bits;
  struct {
    byte         :1;
    byte         :1;
    byte grpTSTATE :2;
    byte grpRSTATE :2;
    byte         :1;
    byte         :1;
  } MergedBits;
} CAN4RIERSTR;
extern volatile CAN4RIERSTR _CAN4RIER @(REG_BASE + 0x00000285UL);
#define CAN4RIER                        _CAN4RIER.Byte
#define CAN4RIER_RXFIE                  _CAN4RIER.Bits.RXFIE
#define CAN4RIER_OVRIE                  _CAN4RIER.Bits.OVRIE
#define CAN4RIER_TSTATE0                _CAN4RIER.Bits.TSTATE0
#define CAN4RIER_TSTATE1                _CAN4RIER.Bits.TSTATE1
#define CAN4RIER_RSTATE0                _CAN4RIER.Bits.RSTATE0
#define CAN4RIER_RSTATE1                _CAN4RIER.Bits.RSTATE1
#define CAN4RIER_CSCIE                  _CAN4RIER.Bits.CSCIE
#define CAN4RIER_WUPIE                  _CAN4RIER.Bits.WUPIE
#define CAN4RIER_TSTATE                 _CAN4RIER.MergedBits.grpTSTATE
#define CAN4RIER_RSTATE                 _CAN4RIER.MergedBits.grpRSTATE

#define CAN4RIER_RXFIE_MASK             1U
#define CAN4RIER_OVRIE_MASK             2U
#define CAN4RIER_TSTATE0_MASK           4U
#define CAN4RIER_TSTATE1_MASK           8U
#define CAN4RIER_RSTATE0_MASK           16U
#define CAN4RIER_RSTATE1_MASK           32U
#define CAN4RIER_CSCIE_MASK             64U
#define CAN4RIER_WUPIE_MASK             128U
#define CAN4RIER_TSTATE_MASK            12U
#define CAN4RIER_TSTATE_BITNUM          2U
#define CAN4RIER_RSTATE_MASK            48U
#define CAN4RIER_RSTATE_BITNUM          4U


/*** CAN4TFLG - MSCAN4 Transmitter Flag Register; 0x00000286 ***/
typedef union {
  byte Byte;
  struct {
    byte TXE0        :1;                                       /* Transmitter Buffer Empty 0 */
    byte TXE1        :1;                                       /* Transmitter Buffer Empty 1 */
    byte TXE2        :1;                                       /* Transmitter Buffer Empty 2 */
    byte             :1; 
    byte             :1; 
    byte             :1; 
    byte             :1; 
    byte             :1; 
  } Bits;
  struct {
    byte grpTXE  :3;
    byte         :1;
    byte         :1;
    byte         :1;
    byte         :1;
    byte         :1;
  } MergedBits;
} CAN4TFLGSTR;
extern volatile CAN4TFLGSTR _CAN4TFLG @(REG_BASE + 0x00000286UL);
#define CAN4TFLG                        _CAN4TFLG.Byte
#define CAN4TFLG_TXE0                   _CAN4TFLG.Bits.TXE0
#define CAN4TFLG_TXE1                   _CAN4TFLG.Bits.TXE1
#define CAN4TFLG_TXE2                   _CAN4TFLG.Bits.TXE2
#define CAN4TFLG_TXE                    _CAN4TFLG.MergedBits.grpTXE

#define CAN4TFLG_TXE0_MASK              1U
#define CAN4TFLG_TXE1_MASK              2U
#define CAN4TFLG_TXE2_MASK              4U
#define CAN4TFLG_TXE_MASK               7U
#define CAN4TFLG_TXE_BITNUM             0U


/*** CAN4TIER - MSCAN4 Transmitter Interrupt Enable Register; 0x00000287 ***/
typedef union {
  byte Byte;
  struct {
    byte TXEIE0      :1;                                       /* Transmitter Empty Interrupt Enable 0 */
    byte TXEIE1      :1;                                       /* Transmitter Empty Interrupt Enable 1 */
    byte TXEIE2      :1;                                       /* Transmitter Empty Interrupt Enable 2 */
    byte             :1; 
    byte             :1; 
    byte             :1; 
    byte             :1; 
    byte             :1; 
  } Bits;
  struct {
    byte grpTXEIE :3;
    byte         :1;
    byte         :1;
    byte         :1;
    byte         :1;
    byte         :1;
  } MergedBits;
} CAN4TIERSTR;
extern volatile CAN4TIERSTR _CAN4TIER @(REG_BASE + 0x00000287UL);
#define CAN4TIER                        _CAN4TIER.Byte
#define CAN4TIER_TXEIE0                 _CAN4TIER.Bits.TXEIE0
#define CAN4TIER_TXEIE1                 _CAN4TIER.Bits.TXEIE1
#define CAN4TIER_TXEIE2                 _CAN4TIER.Bits.TXEIE2
#define CAN4TIER_TXEIE                  _CAN4TIER.MergedBits.grpTXEIE

#define CAN4TIER_TXEIE0_MASK            1U
#define CAN4TIER_TXEIE1_MASK            2U
#define CAN4TIER_TXEIE2_MASK            4U
#define CAN4TIER_TXEIE_MASK             7U
#define CAN4TIER_TXEIE_BITNUM           0U


/*** CAN4TARQ - MSCAN 4 Transmitter Message Abort Request; 0x00000288 ***/
typedef union {
  byte Byte;
  struct {
    byte ABTRQ0      :1;                                       /* Abort Request 0 */
    byte ABTRQ1      :1;                                       /* Abort Request 1 */
    byte ABTRQ2      :1;                                       /* Abort Request 2 */
    byte             :1; 
    byte             :1; 
    byte             :1; 
    byte             :1; 
    byte             :1; 
  } Bits;
  struct {
    byte grpABTRQ :3;
    byte         :1;
    byte         :1;
    byte         :1;
    byte         :1;
    byte         :1;
  } MergedBits;
} CAN4TARQSTR;
extern volatile CAN4TARQSTR _CAN4TARQ @(REG_BASE + 0x00000288UL);
#define CAN4TARQ                        _CAN4TARQ.Byte
#define CAN4TARQ_ABTRQ0                 _CAN4TARQ.Bits.ABTRQ0
#define CAN4TARQ_ABTRQ1                 _CAN4TARQ.Bits.ABTRQ1
#define CAN4TARQ_ABTRQ2                 _CAN4TARQ.Bits.ABTRQ2
#define CAN4TARQ_ABTRQ                  _CAN4TARQ.MergedBits.grpABTRQ

#define CAN4TARQ_ABTRQ0_MASK            1U
#define CAN4TARQ_ABTRQ1_MASK            2U
#define CAN4TARQ_ABTRQ2_MASK            4U
#define CAN4TARQ_ABTRQ_MASK             7U
#define CAN4TARQ_ABTRQ_BITNUM           0U


/*** CAN4TAAK - MSCAN4 Transmitter Message Abort Control; 0x00000289 ***/
typedef union {
  byte Byte;
  struct {
    byte ABTAK0      :1;                                       /* Abort Acknowledge 0 */
    byte ABTAK1      :1;                                       /* Abort Acknowledge 1 */
    byte ABTAK2      :1;                                       /* Abort Acknowledge 2 */
    byte             :1; 
    byte             :1; 
    byte             :1; 
    byte             :1; 
    byte             :1; 
  } Bits;
  struct {
    byte grpABTAK :3;
    byte         :1;
    byte         :1;
    byte         :1;
    byte         :1;
    byte         :1;
  } MergedBits;
} CAN4TAAKSTR;
extern volatile CAN4TAAKSTR _CAN4TAAK @(REG_BASE + 0x00000289UL);
#define CAN4TAAK                        _CAN4TAAK.Byte
#define CAN4TAAK_ABTAK0                 _CAN4TAAK.Bits.ABTAK0
#define CAN4TAAK_ABTAK1                 _CAN4TAAK.Bits.ABTAK1
#define CAN4TAAK_ABTAK2                 _CAN4TAAK.Bits.ABTAK2
#define CAN4TAAK_ABTAK                  _CAN4TAAK.MergedBits.grpABTAK

#define CAN4TAAK_ABTAK0_MASK            1U
#define CAN4TAAK_ABTAK1_MASK            2U
#define CAN4TAAK_ABTAK2_MASK            4U
#define CAN4TAAK_ABTAK_MASK             7U
#define CAN4TAAK_ABTAK_BITNUM           0U


/*** CAN4TBSEL - MSCAN4 Transmit Buffer Selection; 0x0000028A ***/
typedef union {
  byte Byte;
  struct {
    byte TX0         :1;                                       /* Transmit Buffer Select 0 */
    byte TX1         :1;                                       /* Transmit Buffer Select 1 */
    byte TX2         :1;                                       /* Transmit Buffer Select 2 */
    byte             :1; 
    byte             :1; 
    byte             :1; 
    byte             :1; 
    byte             :1; 
  } Bits;
  struct {
    byte grpTX   :3;
    byte         :1;
    byte         :1;
    byte         :1;
    byte         :1;
    byte         :1;
  } MergedBits;
} CAN4TBSELSTR;
extern volatile CAN4TBSELSTR _CAN4TBSEL @(REG_BASE + 0x0000028AUL);
#define CAN4TBSEL                       _CAN4TBSEL.Byte
#define CAN4TBSEL_TX0                   _CAN4TBSEL.Bits.TX0
#define CAN4TBSEL_TX1                   _CAN4TBSEL.Bits.TX1
#define CAN4TBSEL_TX2                   _CAN4TBSEL.Bits.TX2
#define CAN4TBSEL_TX                    _CAN4TBSEL.MergedBits.grpTX

#define CAN4TBSEL_TX0_MASK              1U
#define CAN4TBSEL_TX1_MASK              2U
#define CAN4TBSEL_TX2_MASK              4U
#define CAN4TBSEL_TX_MASK               7U
#define CAN4TBSEL_TX_BITNUM             0U


/*** CAN4IDAC - MSCAN4 Identifier Acceptance Control Register; 0x0000028B ***/
typedef union {
  byte Byte;
  struct {
    byte IDHIT0      :1;                                       /* Identifier Acceptance Hit Indicator 0 */
    byte IDHIT1      :1;                                       /* Identifier Acceptance Hit Indicator 1 */
    byte IDHIT2      :1;                                       /* Identifier Acceptance Hit Indicator 2 */
    byte             :1; 
    byte IDAM0       :1;                                       /* Identifier Acceptance Mode 0 */
    byte IDAM1       :1;                                       /* Identifier Acceptance Mode 1 */
    byte             :1; 
    byte             :1; 
  } Bits;
  struct {
    byte grpIDHIT :3;
    byte         :1;
    byte grpIDAM :2;
    byte         :1;
    byte         :1;
  } MergedBits;
} CAN4IDACSTR;
extern volatile CAN4IDACSTR _CAN4IDAC @(REG_BASE + 0x0000028BUL);
#define CAN4IDAC                        _CAN4IDAC.Byte
#define CAN4IDAC_IDHIT0                 _CAN4IDAC.Bits.IDHIT0
#define CAN4IDAC_IDHIT1                 _CAN4IDAC.Bits.IDHIT1
#define CAN4IDAC_IDHIT2                 _CAN4IDAC.Bits.IDHIT2
#define CAN4IDAC_IDAM0                  _CAN4IDAC.Bits.IDAM0
#define CAN4IDAC_IDAM1                  _CAN4IDAC.Bits.IDAM1
#define CAN4IDAC_IDHIT                  _CAN4IDAC.MergedBits.grpIDHIT
#define CAN4IDAC_IDAM                   _CAN4IDAC.MergedBits.grpIDAM

#define CAN4IDAC_IDHIT0_MASK            1U
#define CAN4IDAC_IDHIT1_MASK            2U
#define CAN4IDAC_IDHIT2_MASK            4U
#define CAN4IDAC_IDAM0_MASK             16U
#define CAN4IDAC_IDAM1_MASK             32U
#define CAN4IDAC_IDHIT_MASK             7U
#define CAN4IDAC_IDHIT_BITNUM           0U
#define CAN4IDAC_IDAM_MASK              48U
#define CAN4IDAC_IDAM_BITNUM            4U


/*** CAN4RXERR - MSCAN4 Receive Error Counter Register; 0x0000028E ***/
typedef union {
  byte Byte;
  struct {
    byte RXERR0      :1;                                       /* Bit 0 */
    byte RXERR1      :1;                                       /* Bit 1 */
    byte RXERR2      :1;                                       /* Bit 2 */
    byte RXERR3      :1;                                       /* Bit 3 */
    byte RXERR4      :1;                                       /* Bit 4 */
    byte RXERR5      :1;                                       /* Bit 5 */
    byte RXERR6      :1;                                       /* Bit 6 */
    byte RXERR7      :1;                                       /* Bit 7 */
  } Bits;
} CAN4RXERRSTR;
extern volatile CAN4RXERRSTR _CAN4RXERR @(REG_BASE + 0x0000028EUL);
#define CAN4RXERR                       _CAN4RXERR.Byte
#define CAN4RXERR_RXERR0                _CAN4RXERR.Bits.RXERR0
#define CAN4RXERR_RXERR1                _CAN4RXERR.Bits.RXERR1
#define CAN4RXERR_RXERR2                _CAN4RXERR.Bits.RXERR2
#define CAN4RXERR_RXERR3                _CAN4RXERR.Bits.RXERR3
#define CAN4RXERR_RXERR4                _CAN4RXERR.Bits.RXERR4
#define CAN4RXERR_RXERR5                _CAN4RXERR.Bits.RXERR5
#define CAN4RXERR_RXERR6                _CAN4RXERR.Bits.RXERR6
#define CAN4RXERR_RXERR7                _CAN4RXERR.Bits.RXERR7

#define CAN4RXERR_RXERR0_MASK           1U
#define CAN4RXERR_RXERR1_MASK           2U
#define CAN4RXERR_RXERR2_MASK           4U
#define CAN4RXERR_RXERR3_MASK           8U
#define CAN4RXERR_RXERR4_MASK           16U
#define CAN4RXERR_RXERR5_MASK           32U
#define CAN4RXERR_RXERR6_MASK           64U
#define CAN4RXERR_RXERR7_MASK           128U


/*** CAN4TXERR - MSCAN4 Transmit Error Counter Register; 0x0000028F ***/
typedef union {
  byte Byte;
  struct {
    byte TXERR0      :1;                                       /* Bit 0 */
    byte TXERR1      :1;                                       /* Bit 1 */
    byte TXERR2      :1;                                       /* Bit 2 */
    byte TXERR3      :1;                                       /* Bit 3 */
    byte TXERR4      :1;                                       /* Bit 4 */
    byte TXERR5      :1;                                       /* Bit 5 */
    byte TXERR6      :1;                                       /* Bit 6 */
    byte TXERR7      :1;                                       /* Bit 7 */
  } Bits;
} CAN4TXERRSTR;
extern volatile CAN4TXERRSTR _CAN4TXERR @(REG_BASE + 0x0000028FUL);
#define CAN4TXERR                       _CAN4TXERR.Byte
#define CAN4TXERR_TXERR0                _CAN4TXERR.Bits.TXERR0
#define CAN4TXERR_TXERR1                _CAN4TXERR.Bits.TXERR1
#define CAN4TXERR_TXERR2                _CAN4TXERR.Bits.TXERR2
#define CAN4TXERR_TXERR3                _CAN4TXERR.Bits.TXERR3
#define CAN4TXERR_TXERR4                _CAN4TXERR.Bits.TXERR4
#define CAN4TXERR_TXERR5                _CAN4TXERR.Bits.TXERR5
#define CAN4TXERR_TXERR6                _CAN4TXERR.Bits.TXERR6
#define CAN4TXERR_TXERR7                _CAN4TXERR.Bits.TXERR7

#define CAN4TXERR_TXERR0_MASK           1U
#define CAN4TXERR_TXERR1_MASK           2U
#define CAN4TXERR_TXERR2_MASK           4U
#define CAN4TXERR_TXERR3_MASK           8U
#define CAN4TXERR_TXERR4_MASK           16U
#define CAN4TXERR_TXERR5_MASK           32U
#define CAN4TXERR_TXERR6_MASK           64U
#define CAN4TXERR_TXERR7_MASK           128U


/*** CAN4IDAR0 - MSCAN4 Identifier Acceptance Register 0; 0x00000290 ***/
typedef union {
  byte Byte;
  struct {
    byte AC0         :1;                                       /* Acceptance Code Bit 0 */
    byte AC1         :1;                                       /* Acceptance Code Bit 1 */
    byte AC2         :1;                                       /* Acceptance Code Bit 2 */
    byte AC3         :1;                                       /* Acceptance Code Bit 3 */
    byte AC4         :1;                                       /* Acceptance Code Bit 4 */
    byte AC5         :1;                                       /* Acceptance Code Bit 5 */
    byte AC6         :1;                                       /* Acceptance Code Bit 6 */
    byte AC7         :1;                                       /* Acceptance Code Bit 7 */
  } Bits;
} CAN4IDAR0STR;
extern volatile CAN4IDAR0STR _CAN4IDAR0 @(REG_BASE + 0x00000290UL);
#define CAN4IDAR0                       _CAN4IDAR0.Byte
#define CAN4IDAR0_AC0                   _CAN4IDAR0.Bits.AC0
#define CAN4IDAR0_AC1                   _CAN4IDAR0.Bits.AC1
#define CAN4IDAR0_AC2                   _CAN4IDAR0.Bits.AC2
#define CAN4IDAR0_AC3                   _CAN4IDAR0.Bits.AC3
#define CAN4IDAR0_AC4                   _CAN4IDAR0.Bits.AC4
#define CAN4IDAR0_AC5                   _CAN4IDAR0.Bits.AC5
#define CAN4IDAR0_AC6                   _CAN4IDAR0.Bits.AC6
#define CAN4IDAR0_AC7                   _CAN4IDAR0.Bits.AC7
/* CAN4IDAR_ARR: Access 4 CAN4IDARx registers in an array */
#define CAN4IDAR_ARR                    ((volatile byte *) &CAN4IDAR0)

#define CAN4IDAR0_AC0_MASK              1U
#define CAN4IDAR0_AC1_MASK              2U
#define CAN4IDAR0_AC2_MASK              4U
#define CAN4IDAR0_AC3_MASK              8U
#define CAN4IDAR0_AC4_MASK              16U
#define CAN4IDAR0_AC5_MASK              32U
#define CAN4IDAR0_AC6_MASK              64U
#define CAN4IDAR0_AC7_MASK              128U


/*** CAN4IDAR1 - MSCAN4 Identifier Acceptance Register 1; 0x00000291 ***/
typedef union {
  byte Byte;
  struct {
    byte AC0         :1;                                       /* Acceptance Code Bit 0 */
    byte AC1         :1;                                       /* Acceptance Code Bit 1 */
    byte AC2         :1;                                       /* Acceptance Code Bit 2 */
    byte AC3         :1;                                       /* Acceptance Code Bit 3 */
    byte AC4         :1;                                       /* Acceptance Code Bit 4 */
    byte AC5         :1;                                       /* Acceptance Code Bit 5 */
    byte AC6         :1;                                       /* Acceptance Code Bit 6 */
    byte AC7         :1;                                       /* Acceptance Code Bit 7 */
  } Bits;
} CAN4IDAR1STR;
extern volatile CAN4IDAR1STR _CAN4IDAR1 @(REG_BASE + 0x00000291UL);
#define CAN4IDAR1                       _CAN4IDAR1.Byte
#define CAN4IDAR1_AC0                   _CAN4IDAR1.Bits.AC0
#define CAN4IDAR1_AC1                   _CAN4IDAR1.Bits.AC1
#define CAN4IDAR1_AC2                   _CAN4IDAR1.Bits.AC2
#define CAN4IDAR1_AC3                   _CAN4IDAR1.Bits.AC3
#define CAN4IDAR1_AC4                   _CAN4IDAR1.Bits.AC4
#define CAN4IDAR1_AC5                   _CAN4IDAR1.Bits.AC5
#define CAN4IDAR1_AC6                   _CAN4IDAR1.Bits.AC6
#define CAN4IDAR1_AC7                   _CAN4IDAR1.Bits.AC7

#define CAN4IDAR1_AC0_MASK              1U
#define CAN4IDAR1_AC1_MASK              2U
#define CAN4IDAR1_AC2_MASK              4U
#define CAN4IDAR1_AC3_MASK              8U
#define CAN4IDAR1_AC4_MASK              16U
#define CAN4IDAR1_AC5_MASK              32U
#define CAN4IDAR1_AC6_MASK              64U
#define CAN4IDAR1_AC7_MASK              128U


/*** CAN4IDAR2 - MSCAN4 Identifier Acceptance Register 2; 0x00000292 ***/
typedef union {
  byte Byte;
  struct {
    byte AC0         :1;                                       /* Acceptance Code Bit 0 */
    byte AC1         :1;                                       /* Acceptance Code Bit 1 */
    byte AC2         :1;                                       /* Acceptance Code Bit 2 */
    byte AC3         :1;                                       /* Acceptance Code Bit 3 */
    byte AC4         :1;                                       /* Acceptance Code Bit 4 */
    byte AC5         :1;                                       /* Acceptance Code Bit 5 */
    byte AC6         :1;                                       /* Acceptance Code Bit 6 */
    byte AC7         :1;                                       /* Acceptance Code Bit 7 */
  } Bits;
} CAN4IDAR2STR;
extern volatile CAN4IDAR2STR _CAN4IDAR2 @(REG_BASE + 0x00000292UL);
#define CAN4IDAR2                       _CAN4IDAR2.Byte
#define CAN4IDAR2_AC0                   _CAN4IDAR2.Bits.AC0
#define CAN4IDAR2_AC1                   _CAN4IDAR2.Bits.AC1
#define CAN4IDAR2_AC2                   _CAN4IDAR2.Bits.AC2
#define CAN4IDAR2_AC3                   _CAN4IDAR2.Bits.AC3
#define CAN4IDAR2_AC4                   _CAN4IDAR2.Bits.AC4
#define CAN4IDAR2_AC5                   _CAN4IDAR2.Bits.AC5
#define CAN4IDAR2_AC6                   _CAN4IDAR2.Bits.AC6
#define CAN4IDAR2_AC7                   _CAN4IDAR2.Bits.AC7

#define CAN4IDAR2_AC0_MASK              1U
#define CAN4IDAR2_AC1_MASK              2U
#define CAN4IDAR2_AC2_MASK              4U
#define CAN4IDAR2_AC3_MASK              8U
#define CAN4IDAR2_AC4_MASK              16U
#define CAN4IDAR2_AC5_MASK              32U
#define CAN4IDAR2_AC6_MASK              64U
#define CAN4IDAR2_AC7_MASK              128U


/*** CAN4IDAR3 - MSCAN4 Identifier Acceptance Register 3; 0x00000293 ***/
typedef union {
  byte Byte;
  struct {
    byte AC0         :1;                                       /* Acceptance Code Bit 0 */
    byte AC1         :1;                                       /* Acceptance Code Bit 1 */
    byte AC2         :1;                                       /* Acceptance Code Bit 2 */
    byte AC3         :1;                                       /* Acceptance Code Bit 3 */
    byte AC4         :1;                                       /* Acceptance Code Bit 4 */
    byte AC5         :1;                                       /* Acceptance Code Bit 5 */
    byte AC6         :1;                                       /* Acceptance Code Bit 6 */
    byte AC7         :1;                                       /* Acceptance Code Bit 7 */
  } Bits;
} CAN4IDAR3STR;
extern volatile CAN4IDAR3STR _CAN4IDAR3 @(REG_BASE + 0x00000293UL);
#define CAN4IDAR3                       _CAN4IDAR3.Byte
#define CAN4IDAR3_AC0                   _CAN4IDAR3.Bits.AC0
#define CAN4IDAR3_AC1                   _CAN4IDAR3.Bits.AC1
#define CAN4IDAR3_AC2                   _CAN4IDAR3.Bits.AC2
#define CAN4IDAR3_AC3                   _CAN4IDAR3.Bits.AC3
#define CAN4IDAR3_AC4                   _CAN4IDAR3.Bits.AC4
#define CAN4IDAR3_AC5                   _CAN4IDAR3.Bits.AC5
#define CAN4IDAR3_AC6                   _CAN4IDAR3.Bits.AC6
#define CAN4IDAR3_AC7                   _CAN4IDAR3.Bits.AC7

#define CAN4IDAR3_AC0_MASK              1U
#define CAN4IDAR3_AC1_MASK              2U
#define CAN4IDAR3_AC2_MASK              4U
#define CAN4IDAR3_AC3_MASK              8U
#define CAN4IDAR3_AC4_MASK              16U
#define CAN4IDAR3_AC5_MASK              32U
#define CAN4IDAR3_AC6_MASK              64U
#define CAN4IDAR3_AC7_MASK              128U


/*** CAN4IDMR0 - MSCAN4 Identifier Mask Register 0; 0x00000294 ***/
typedef union {
  byte Byte;
  struct {
    byte AM0         :1;                                       /* Acceptance Mask Bit 0 */
    byte AM1         :1;                                       /* Acceptance Mask Bit 1 */
    byte AM2         :1;                                       /* Acceptance Mask Bit 2 */
    byte AM3         :1;                                       /* Acceptance Mask Bit 3 */
    byte AM4         :1;                                       /* Acceptance Mask Bit 4 */
    byte AM5         :1;                                       /* Acceptance Mask Bit 5 */
    byte AM6         :1;                                       /* Acceptance Mask Bit 6 */
    byte AM7         :1;                                       /* Acceptance Mask Bit 7 */
  } Bits;
} CAN4IDMR0STR;
extern volatile CAN4IDMR0STR _CAN4IDMR0 @(REG_BASE + 0x00000294UL);
#define CAN4IDMR0                       _CAN4IDMR0.Byte
#define CAN4IDMR0_AM0                   _CAN4IDMR0.Bits.AM0
#define CAN4IDMR0_AM1                   _CAN4IDMR0.Bits.AM1
#define CAN4IDMR0_AM2                   _CAN4IDMR0.Bits.AM2
#define CAN4IDMR0_AM3                   _CAN4IDMR0.Bits.AM3
#define CAN4IDMR0_AM4                   _CAN4IDMR0.Bits.AM4
#define CAN4IDMR0_AM5                   _CAN4IDMR0.Bits.AM5
#define CAN4IDMR0_AM6                   _CAN4IDMR0.Bits.AM6
#define CAN4IDMR0_AM7                   _CAN4IDMR0.Bits.AM7
/* CAN4IDMR_ARR: Access 4 CAN4IDMRx registers in an array */
#define CAN4IDMR_ARR                    ((volatile byte *) &CAN4IDMR0)

#define CAN4IDMR0_AM0_MASK              1U
#define CAN4IDMR0_AM1_MASK              2U
#define CAN4IDMR0_AM2_MASK              4U
#define CAN4IDMR0_AM3_MASK              8U
#define CAN4IDMR0_AM4_MASK              16U
#define CAN4IDMR0_AM5_MASK              32U
#define CAN4IDMR0_AM6_MASK              64U
#define CAN4IDMR0_AM7_MASK              128U


/*** CAN4IDMR1 - MSCAN4 Identifier Mask Register 1; 0x00000295 ***/
typedef union {
  byte Byte;
  struct {
    byte AM0         :1;                                       /* Acceptance Mask Bit 0 */
    byte AM1         :1;                                       /* Acceptance Mask Bit 1 */
    byte AM2         :1;                                       /* Acceptance Mask Bit 2 */
    byte AM3         :1;                                       /* Acceptance Mask Bit 3 */
    byte AM4         :1;                                       /* Acceptance Mask Bit 4 */
    byte AM5         :1;                                       /* Acceptance Mask Bit 5 */
    byte AM6         :1;                                       /* Acceptance Mask Bit 6 */
    byte AM7         :1;                                       /* Acceptance Mask Bit 7 */
  } Bits;
} CAN4IDMR1STR;
extern volatile CAN4IDMR1STR _CAN4IDMR1 @(REG_BASE + 0x00000295UL);
#define CAN4IDMR1                       _CAN4IDMR1.Byte
#define CAN4IDMR1_AM0                   _CAN4IDMR1.Bits.AM0
#define CAN4IDMR1_AM1                   _CAN4IDMR1.Bits.AM1
#define CAN4IDMR1_AM2                   _CAN4IDMR1.Bits.AM2
#define CAN4IDMR1_AM3                   _CAN4IDMR1.Bits.AM3
#define CAN4IDMR1_AM4                   _CAN4IDMR1.Bits.AM4
#define CAN4IDMR1_AM5                   _CAN4IDMR1.Bits.AM5
#define CAN4IDMR1_AM6                   _CAN4IDMR1.Bits.AM6
#define CAN4IDMR1_AM7                   _CAN4IDMR1.Bits.AM7

#define CAN4IDMR1_AM0_MASK              1U
#define CAN4IDMR1_AM1_MASK              2U
#define CAN4IDMR1_AM2_MASK              4U
#define CAN4IDMR1_AM3_MASK              8U
#define CAN4IDMR1_AM4_MASK              16U
#define CAN4IDMR1_AM5_MASK              32U
#define CAN4IDMR1_AM6_MASK              64U
#define CAN4IDMR1_AM7_MASK              128U


/*** CAN4IDMR2 - MSCAN4 Identifier Mask Register 2; 0x00000296 ***/
typedef union {
  byte Byte;
  struct {
    byte AM0         :1;                                       /* Acceptance Mask Bit 0 */
    byte AM1         :1;                                       /* Acceptance Mask Bit 1 */
    byte AM2         :1;                                       /* Acceptance Mask Bit 2 */
    byte AM3         :1;                                       /* Acceptance Mask Bit 3 */
    byte AM4         :1;                                       /* Acceptance Mask Bit 4 */
    byte AM5         :1;                                       /* Acceptance Mask Bit 5 */
    byte AM6         :1;                                       /* Acceptance Mask Bit 6 */
    byte AM7         :1;                                       /* Acceptance Mask Bit 7 */
  } Bits;
} CAN4IDMR2STR;
extern volatile CAN4IDMR2STR _CAN4IDMR2 @(REG_BASE + 0x00000296UL);
#define CAN4IDMR2                       _CAN4IDMR2.Byte
#define CAN4IDMR2_AM0                   _CAN4IDMR2.Bits.AM0
#define CAN4IDMR2_AM1                   _CAN4IDMR2.Bits.AM1
#define CAN4IDMR2_AM2                   _CAN4IDMR2.Bits.AM2
#define CAN4IDMR2_AM3                   _CAN4IDMR2.Bits.AM3
#define CAN4IDMR2_AM4                   _CAN4IDMR2.Bits.AM4
#define CAN4IDMR2_AM5                   _CAN4IDMR2.Bits.AM5
#define CAN4IDMR2_AM6                   _CAN4IDMR2.Bits.AM6
#define CAN4IDMR2_AM7                   _CAN4IDMR2.Bits.AM7

#define CAN4IDMR2_AM0_MASK              1U
#define CAN4IDMR2_AM1_MASK              2U
#define CAN4IDMR2_AM2_MASK              4U
#define CAN4IDMR2_AM3_MASK              8U
#define CAN4IDMR2_AM4_MASK              16U
#define CAN4IDMR2_AM5_MASK              32U
#define CAN4IDMR2_AM6_MASK              64U
#define CAN4IDMR2_AM7_MASK              128U


/*** CAN4IDMR3 - MSCAN4 Identifier Mask Register 3; 0x00000297 ***/
typedef union {
  byte Byte;
  struct {
    byte AM0         :1;                                       /* Acceptance Mask Bit 0 */
    byte AM1         :1;                                       /* Acceptance Mask Bit 1 */
    byte AM2         :1;                                       /* Acceptance Mask Bit 2 */
    byte AM3         :1;                                       /* Acceptance Mask Bit 3 */
    byte AM4         :1;                                       /* Acceptance Mask Bit 4 */
    byte AM5         :1;                                       /* Acceptance Mask Bit 5 */
    byte AM6         :1;                                       /* Acceptance Mask Bit 6 */
    byte AM7         :1;                                       /* Acceptance Mask Bit 7 */
  } Bits;
} CAN4IDMR3STR;
extern volatile CAN4IDMR3STR _CAN4IDMR3 @(REG_BASE + 0x00000297UL);
#define CAN4IDMR3                       _CAN4IDMR3.Byte
#define CAN4IDMR3_AM0                   _CAN4IDMR3.Bits.AM0
#define CAN4IDMR3_AM1                   _CAN4IDMR3.Bits.AM1
#define CAN4IDMR3_AM2                   _CAN4IDMR3.Bits.AM2
#define CAN4IDMR3_AM3                   _CAN4IDMR3.Bits.AM3
#define CAN4IDMR3_AM4                   _CAN4IDMR3.Bits.AM4
#define CAN4IDMR3_AM5                   _CAN4IDMR3.Bits.AM5
#define CAN4IDMR3_AM6                   _CAN4IDMR3.Bits.AM6
#define CAN4IDMR3_AM7                   _CAN4IDMR3.Bits.AM7

#define CAN4IDMR3_AM0_MASK              1U
#define CAN4IDMR3_AM1_MASK              2U
#define CAN4IDMR3_AM2_MASK              4U
#define CAN4IDMR3_AM3_MASK              8U
#define CAN4IDMR3_AM4_MASK              16U
#define CAN4IDMR3_AM5_MASK              32U
#define CAN4IDMR3_AM6_MASK              64U
#define CAN4IDMR3_AM7_MASK              128U


/*** CAN4IDAR4 - MSCAN4 Identifier Acceptance Register 4; 0x00000298 ***/
typedef union {
  byte Byte;
  struct {
    byte AC0         :1;                                       /* Acceptance Code Bit 0 */
    byte AC1         :1;                                       /* Acceptance Code Bit 1 */
    byte AC2         :1;                                       /* Acceptance Code Bit 2 */
    byte AC3         :1;                                       /* Acceptance Code Bit 3 */
    byte AC4         :1;                                       /* Acceptance Code Bit 4 */
    byte AC5         :1;                                       /* Acceptance Code Bit 5 */
    byte AC6         :1;                                       /* Acceptance Code Bit 6 */
    byte AC7         :1;                                       /* Acceptance Code Bit 7 */
  } Bits;
} CAN4IDAR4STR;
extern volatile CAN4IDAR4STR _CAN4IDAR4 @(REG_BASE + 0x00000298UL);
#define CAN4IDAR4                       _CAN4IDAR4.Byte
#define CAN4IDAR4_AC0                   _CAN4IDAR4.Bits.AC0
#define CAN4IDAR4_AC1                   _CAN4IDAR4.Bits.AC1
#define CAN4IDAR4_AC2                   _CAN4IDAR4.Bits.AC2
#define CAN4IDAR4_AC3                   _CAN4IDAR4.Bits.AC3
#define CAN4IDAR4_AC4                   _CAN4IDAR4.Bits.AC4
#define CAN4IDAR4_AC5                   _CAN4IDAR4.Bits.AC5
#define CAN4IDAR4_AC6                   _CAN4IDAR4.Bits.AC6
#define CAN4IDAR4_AC7                   _CAN4IDAR4.Bits.AC7

#define CAN4IDAR4_AC0_MASK              1U
#define CAN4IDAR4_AC1_MASK              2U
#define CAN4IDAR4_AC2_MASK              4U
#define CAN4IDAR4_AC3_MASK              8U
#define CAN4IDAR4_AC4_MASK              16U
#define CAN4IDAR4_AC5_MASK              32U
#define CAN4IDAR4_AC6_MASK              64U
#define CAN4IDAR4_AC7_MASK              128U


/*** CAN4IDAR5 - MSCAN4 Identifier Acceptance Register 5; 0x00000299 ***/
typedef union {
  byte Byte;
  struct {
    byte AC0         :1;                                       /* Acceptance Code Bit 0 */
    byte AC1         :1;                                       /* Acceptance Code Bit 1 */
    byte AC2         :1;                                       /* Acceptance Code Bit 2 */
    byte AC3         :1;                                       /* Acceptance Code Bit 3 */
    byte AC4         :1;                                       /* Acceptance Code Bit 4 */
    byte AC5         :1;                                       /* Acceptance Code Bit 5 */
    byte AC6         :1;                                       /* Acceptance Code Bit 6 */
    byte AC7         :1;                                       /* Acceptance Code Bit 7 */
  } Bits;
} CAN4IDAR5STR;
extern volatile CAN4IDAR5STR _CAN4IDAR5 @(REG_BASE + 0x00000299UL);
#define CAN4IDAR5                       _CAN4IDAR5.Byte
#define CAN4IDAR5_AC0                   _CAN4IDAR5.Bits.AC0
#define CAN4IDAR5_AC1                   _CAN4IDAR5.Bits.AC1
#define CAN4IDAR5_AC2                   _CAN4IDAR5.Bits.AC2
#define CAN4IDAR5_AC3                   _CAN4IDAR5.Bits.AC3
#define CAN4IDAR5_AC4                   _CAN4IDAR5.Bits.AC4
#define CAN4IDAR5_AC5                   _CAN4IDAR5.Bits.AC5
#define CAN4IDAR5_AC6                   _CAN4IDAR5.Bits.AC6
#define CAN4IDAR5_AC7                   _CAN4IDAR5.Bits.AC7

#define CAN4IDAR5_AC0_MASK              1U
#define CAN4IDAR5_AC1_MASK              2U
#define CAN4IDAR5_AC2_MASK              4U
#define CAN4IDAR5_AC3_MASK              8U
#define CAN4IDAR5_AC4_MASK              16U
#define CAN4IDAR5_AC5_MASK              32U
#define CAN4IDAR5_AC6_MASK              64U
#define CAN4IDAR5_AC7_MASK              128U


/*** CAN4IDAR6 - MSCAN4 Identifier Acceptance Register 6; 0x0000029A ***/
typedef union {
  byte Byte;
  struct {
    byte AC0         :1;                                       /* Acceptance Code Bit 0 */
    byte AC1         :1;                                       /* Acceptance Code Bit 1 */
    byte AC2         :1;                                       /* Acceptance Code Bit 2 */
    byte AC3         :1;                                       /* Acceptance Code Bit 3 */
    byte AC4         :1;                                       /* Acceptance Code Bit 4 */
    byte AC5         :1;                                       /* Acceptance Code Bit 5 */
    byte AC6         :1;                                       /* Acceptance Code Bit 6 */
    byte AC7         :1;                                       /* Acceptance Code Bit 7 */
  } Bits;
} CAN4IDAR6STR;
extern volatile CAN4IDAR6STR _CAN4IDAR6 @(REG_BASE + 0x0000029AUL);
#define CAN4IDAR6                       _CAN4IDAR6.Byte
#define CAN4IDAR6_AC0                   _CAN4IDAR6.Bits.AC0
#define CAN4IDAR6_AC1                   _CAN4IDAR6.Bits.AC1
#define CAN4IDAR6_AC2                   _CAN4IDAR6.Bits.AC2
#define CAN4IDAR6_AC3                   _CAN4IDAR6.Bits.AC3
#define CAN4IDAR6_AC4                   _CAN4IDAR6.Bits.AC4
#define CAN4IDAR6_AC5                   _CAN4IDAR6.Bits.AC5
#define CAN4IDAR6_AC6                   _CAN4IDAR6.Bits.AC6
#define CAN4IDAR6_AC7                   _CAN4IDAR6.Bits.AC7

#define CAN4IDAR6_AC0_MASK              1U
#define CAN4IDAR6_AC1_MASK              2U
#define CAN4IDAR6_AC2_MASK              4U
#define CAN4IDAR6_AC3_MASK              8U
#define CAN4IDAR6_AC4_MASK              16U
#define CAN4IDAR6_AC5_MASK              32U
#define CAN4IDAR6_AC6_MASK              64U
#define CAN4IDAR6_AC7_MASK              128U


/*** CAN4IDAR7 - MSCAN4 Identifier Acceptance Register 7; 0x0000029B ***/
typedef union {
  byte Byte;
  struct {
    byte AC0         :1;                                       /* Acceptance Code Bit 0 */
    byte AC1         :1;                                       /* Acceptance Code Bit 1 */
    byte AC2         :1;                                       /* Acceptance Code Bit 2 */
    byte AC3         :1;                                       /* Acceptance Code Bit 3 */
    byte AC4         :1;                                       /* Acceptance Code Bit 4 */
    byte AC5         :1;                                       /* Acceptance Code Bit 5 */
    byte AC6         :1;                                       /* Acceptance Code Bit 6 */
    byte AC7         :1;                                       /* Acceptance Code Bit 7 */
  } Bits;
} CAN4IDAR7STR;
extern volatile CAN4IDAR7STR _CAN4IDAR7 @(REG_BASE + 0x0000029BUL);
#define CAN4IDAR7                       _CAN4IDAR7.Byte
#define CAN4IDAR7_AC0                   _CAN4IDAR7.Bits.AC0
#define CAN4IDAR7_AC1                   _CAN4IDAR7.Bits.AC1
#define CAN4IDAR7_AC2                   _CAN4IDAR7.Bits.AC2
#define CAN4IDAR7_AC3                   _CAN4IDAR7.Bits.AC3
#define CAN4IDAR7_AC4                   _CAN4IDAR7.Bits.AC4
#define CAN4IDAR7_AC5                   _CAN4IDAR7.Bits.AC5
#define CAN4IDAR7_AC6                   _CAN4IDAR7.Bits.AC6
#define CAN4IDAR7_AC7                   _CAN4IDAR7.Bits.AC7

#define CAN4IDAR7_AC0_MASK              1U
#define CAN4IDAR7_AC1_MASK              2U
#define CAN4IDAR7_AC2_MASK              4U
#define CAN4IDAR7_AC3_MASK              8U
#define CAN4IDAR7_AC4_MASK              16U
#define CAN4IDAR7_AC5_MASK              32U
#define CAN4IDAR7_AC6_MASK              64U
#define CAN4IDAR7_AC7_MASK              128U


/*** CAN4IDMR4 - MSCAN4 Identifier Mask Register 4; 0x0000029C ***/
typedef union {
  byte Byte;
  struct {
    byte AM0         :1;                                       /* Acceptance Mask Bit 0 */
    byte AM1         :1;                                       /* Acceptance Mask Bit 1 */
    byte AM2         :1;                                       /* Acceptance Mask Bit 2 */
    byte AM3         :1;                                       /* Acceptance Mask Bit 3 */
    byte AM4         :1;                                       /* Acceptance Mask Bit 4 */
    byte AM5         :1;                                       /* Acceptance Mask Bit 5 */
    byte AM6         :1;                                       /* Acceptance Mask Bit 6 */
    byte AM7         :1;                                       /* Acceptance Mask Bit 7 */
  } Bits;
} CAN4IDMR4STR;
extern volatile CAN4IDMR4STR _CAN4IDMR4 @(REG_BASE + 0x0000029CUL);
#define CAN4IDMR4                       _CAN4IDMR4.Byte
#define CAN4IDMR4_AM0                   _CAN4IDMR4.Bits.AM0
#define CAN4IDMR4_AM1                   _CAN4IDMR4.Bits.AM1
#define CAN4IDMR4_AM2                   _CAN4IDMR4.Bits.AM2
#define CAN4IDMR4_AM3                   _CAN4IDMR4.Bits.AM3
#define CAN4IDMR4_AM4                   _CAN4IDMR4.Bits.AM4
#define CAN4IDMR4_AM5                   _CAN4IDMR4.Bits.AM5
#define CAN4IDMR4_AM6                   _CAN4IDMR4.Bits.AM6
#define CAN4IDMR4_AM7                   _CAN4IDMR4.Bits.AM7

#define CAN4IDMR4_AM0_MASK              1U
#define CAN4IDMR4_AM1_MASK              2U
#define CAN4IDMR4_AM2_MASK              4U
#define CAN4IDMR4_AM3_MASK              8U
#define CAN4IDMR4_AM4_MASK              16U
#define CAN4IDMR4_AM5_MASK              32U
#define CAN4IDMR4_AM6_MASK              64U
#define CAN4IDMR4_AM7_MASK              128U


/*** CAN4IDMR5 - MSCAN4 Identifier Mask Register 5; 0x0000029D ***/
typedef union {
  byte Byte;
  struct {
    byte AM0         :1;                                       /* Acceptance Mask Bit 0 */
    byte AM1         :1;                                       /* Acceptance Mask Bit 1 */
    byte AM2         :1;                                       /* Acceptance Mask Bit 2 */
    byte AM3         :1;                                       /* Acceptance Mask Bit 3 */
    byte AM4         :1;                                       /* Acceptance Mask Bit 4 */
    byte AM5         :1;                                       /* Acceptance Mask Bit 5 */
    byte AM6         :1;                                       /* Acceptance Mask Bit 6 */
    byte AM7         :1;                                       /* Acceptance Mask Bit 7 */
  } Bits;
} CAN4IDMR5STR;
extern volatile CAN4IDMR5STR _CAN4IDMR5 @(REG_BASE + 0x0000029DUL);
#define CAN4IDMR5                       _CAN4IDMR5.Byte
#define CAN4IDMR5_AM0                   _CAN4IDMR5.Bits.AM0
#define CAN4IDMR5_AM1                   _CAN4IDMR5.Bits.AM1
#define CAN4IDMR5_AM2                   _CAN4IDMR5.Bits.AM2
#define CAN4IDMR5_AM3                   _CAN4IDMR5.Bits.AM3
#define CAN4IDMR5_AM4                   _CAN4IDMR5.Bits.AM4
#define CAN4IDMR5_AM5                   _CAN4IDMR5.Bits.AM5
#define CAN4IDMR5_AM6                   _CAN4IDMR5.Bits.AM6
#define CAN4IDMR5_AM7                   _CAN4IDMR5.Bits.AM7

#define CAN4IDMR5_AM0_MASK              1U
#define CAN4IDMR5_AM1_MASK              2U
#define CAN4IDMR5_AM2_MASK              4U
#define CAN4IDMR5_AM3_MASK              8U
#define CAN4IDMR5_AM4_MASK              16U
#define CAN4IDMR5_AM5_MASK              32U
#define CAN4IDMR5_AM6_MASK              64U
#define CAN4IDMR5_AM7_MASK              128U


/*** CAN4IDMR6 - MSCAN4 Identifier Mask Register 6; 0x0000029E ***/
typedef union {
  byte Byte;
  struct {
    byte AM0         :1;                                       /* Acceptance Mask Bit 0 */
    byte AM1         :1;                                       /* Acceptance Mask Bit 1 */
    byte AM2         :1;                                       /* Acceptance Mask Bit 2 */
    byte AM3         :1;                                       /* Acceptance Mask Bit 3 */
    byte AM4         :1;                                       /* Acceptance Mask Bit 4 */
    byte AM5         :1;                                       /* Acceptance Mask Bit 5 */
    byte AM6         :1;                                       /* Acceptance Mask Bit 6 */
    byte AM7         :1;                                       /* Acceptance Mask Bit 7 */
  } Bits;
} CAN4IDMR6STR;
extern volatile CAN4IDMR6STR _CAN4IDMR6 @(REG_BASE + 0x0000029EUL);
#define CAN4IDMR6                       _CAN4IDMR6.Byte
#define CAN4IDMR6_AM0                   _CAN4IDMR6.Bits.AM0
#define CAN4IDMR6_AM1                   _CAN4IDMR6.Bits.AM1
#define CAN4IDMR6_AM2                   _CAN4IDMR6.Bits.AM2
#define CAN4IDMR6_AM3                   _CAN4IDMR6.Bits.AM3
#define CAN4IDMR6_AM4                   _CAN4IDMR6.Bits.AM4
#define CAN4IDMR6_AM5                   _CAN4IDMR6.Bits.AM5
#define CAN4IDMR6_AM6                   _CAN4IDMR6.Bits.AM6
#define CAN4IDMR6_AM7                   _CAN4IDMR6.Bits.AM7

#define CAN4IDMR6_AM0_MASK              1U
#define CAN4IDMR6_AM1_MASK              2U
#define CAN4IDMR6_AM2_MASK              4U
#define CAN4IDMR6_AM3_MASK              8U
#define CAN4IDMR6_AM4_MASK              16U
#define CAN4IDMR6_AM5_MASK              32U
#define CAN4IDMR6_AM6_MASK              64U
#define CAN4IDMR6_AM7_MASK              128U


/*** CAN4IDMR7 - MSCAN4 Identifier Mask Register 7; 0x0000029F ***/
typedef union {
  byte Byte;
  struct {
    byte AM0         :1;                                       /* Acceptance Mask Bit 0 */
    byte AM1         :1;                                       /* Acceptance Mask Bit 1 */
    byte AM2         :1;                                       /* Acceptance Mask Bit 2 */
    byte AM3         :1;                                       /* Acceptance Mask Bit 3 */
    byte AM4         :1;                                       /* Acceptance Mask Bit 4 */
    byte AM5         :1;                                       /* Acceptance Mask Bit 5 */
    byte AM6         :1;                                       /* Acceptance Mask Bit 6 */
    byte AM7         :1;                                       /* Acceptance Mask Bit 7 */
  } Bits;
} CAN4IDMR7STR;
extern volatile CAN4IDMR7STR _CAN4IDMR7 @(REG_BASE + 0x0000029FUL);
#define CAN4IDMR7                       _CAN4IDMR7.Byte
#define CAN4IDMR7_AM0                   _CAN4IDMR7.Bits.AM0
#define CAN4IDMR7_AM1                   _CAN4IDMR7.Bits.AM1
#define CAN4IDMR7_AM2                   _CAN4IDMR7.Bits.AM2
#define CAN4IDMR7_AM3                   _CAN4IDMR7.Bits.AM3
#define CAN4IDMR7_AM4                   _CAN4IDMR7.Bits.AM4
#define CAN4IDMR7_AM5                   _CAN4IDMR7.Bits.AM5
#define CAN4IDMR7_AM6                   _CAN4IDMR7.Bits.AM6
#define CAN4IDMR7_AM7                   _CAN4IDMR7.Bits.AM7

#define CAN4IDMR7_AM0_MASK              1U
#define CAN4IDMR7_AM1_MASK              2U
#define CAN4IDMR7_AM2_MASK              4U
#define CAN4IDMR7_AM3_MASK              8U
#define CAN4IDMR7_AM4_MASK              16U
#define CAN4IDMR7_AM5_MASK              32U
#define CAN4IDMR7_AM6_MASK              64U
#define CAN4IDMR7_AM7_MASK              128U


/*** CAN4RXIDR0 - MSCAN4 Receive Identifier Register 0; 0x000002A0 ***/
typedef union {
  byte Byte;
  struct {
    byte ID21        :1;                                       /* Extended format identifier Bit 21 */
    byte ID22        :1;                                       /* Extended format identifier Bit 22 */
    byte ID23        :1;                                       /* Extended format identifier Bit 23 */
    byte ID24        :1;                                       /* Extended format identifier Bit 24 */
    byte ID25        :1;                                       /* Extended format identifier Bit 25 */
    byte ID26        :1;                                       /* Extended format identifier Bit 26 */
    byte ID27        :1;                                       /* Extended format identifier Bit 27 */
    byte ID28        :1;                                       /* Extended format identifier Bit 28 */
  } Bits;
} CAN4RXIDR0STR;
extern volatile CAN4RXIDR0STR _CAN4RXIDR0 @(REG_BASE + 0x000002A0UL);
#define CAN4RXIDR0                      _CAN4RXIDR0.Byte
#define CAN4RXIDR0_ID21                 _CAN4RXIDR0.Bits.ID21
#define CAN4RXIDR0_ID22                 _CAN4RXIDR0.Bits.ID22
#define CAN4RXIDR0_ID23                 _CAN4RXIDR0.Bits.ID23
#define CAN4RXIDR0_ID24                 _CAN4RXIDR0.Bits.ID24
#define CAN4RXIDR0_ID25                 _CAN4RXIDR0.Bits.ID25
#define CAN4RXIDR0_ID26                 _CAN4RXIDR0.Bits.ID26
#define CAN4RXIDR0_ID27                 _CAN4RXIDR0.Bits.ID27
#define CAN4RXIDR0_ID28                 _CAN4RXIDR0.Bits.ID28
/* CAN4RXIDR_ARR: Access 4 CAN4RXIDRx registers in an array */
#define CAN4RXIDR_ARR                   ((volatile byte *) &CAN4RXIDR0)

#define CAN4RXIDR0_ID21_MASK            1U
#define CAN4RXIDR0_ID22_MASK            2U
#define CAN4RXIDR0_ID23_MASK            4U
#define CAN4RXIDR0_ID24_MASK            8U
#define CAN4RXIDR0_ID25_MASK            16U
#define CAN4RXIDR0_ID26_MASK            32U
#define CAN4RXIDR0_ID27_MASK            64U
#define CAN4RXIDR0_ID28_MASK            128U


/*** CAN4RXIDR1 - MSCAN4 Receive Identifier Register 1; 0x000002A1 ***/
typedef union {
  byte Byte;
  struct {
    byte ID15        :1;                                       /* Extended format identifier Bit 15 */
    byte ID16        :1;                                       /* Extended format identifier Bit 16 */
    byte ID17        :1;                                       /* Extended format identifier Bit 17 */
    byte IDE         :1;                                       /* ID Extended */
    byte SRR         :1;                                       /* Substitute Remote Request */
    byte ID18        :1;                                       /* Extended format identifier Bit 18 */
    byte ID19        :1;                                       /* Extended format identifier Bit 19 */
    byte ID20        :1;                                       /* Extended format identifier Bit 20 */
  } Bits;
  struct {
    byte grpID_15 :3;
    byte         :1;
    byte         :1;
    byte grpID_18 :3;
  } MergedBits;
} CAN4RXIDR1STR;
extern volatile CAN4RXIDR1STR _CAN4RXIDR1 @(REG_BASE + 0x000002A1UL);
#define CAN4RXIDR1                      _CAN4RXIDR1.Byte
#define CAN4RXIDR1_ID15                 _CAN4RXIDR1.Bits.ID15
#define CAN4RXIDR1_ID16                 _CAN4RXIDR1.Bits.ID16
#define CAN4RXIDR1_ID17                 _CAN4RXIDR1.Bits.ID17
#define CAN4RXIDR1_IDE                  _CAN4RXIDR1.Bits.IDE
#define CAN4RXIDR1_SRR                  _CAN4RXIDR1.Bits.SRR
#define CAN4RXIDR1_ID18                 _CAN4RXIDR1.Bits.ID18
#define CAN4RXIDR1_ID19                 _CAN4RXIDR1.Bits.ID19
#define CAN4RXIDR1_ID20                 _CAN4RXIDR1.Bits.ID20
#define CAN4RXIDR1_ID_15                _CAN4RXIDR1.MergedBits.grpID_15
#define CAN4RXIDR1_ID_18                _CAN4RXIDR1.MergedBits.grpID_18
#define CAN4RXIDR1_ID                   CAN4RXIDR1_ID_15

#define CAN4RXIDR1_ID15_MASK            1U
#define CAN4RXIDR1_ID16_MASK            2U
#define CAN4RXIDR1_ID17_MASK            4U
#define CAN4RXIDR1_IDE_MASK             8U
#define CAN4RXIDR1_SRR_MASK             16U
#define CAN4RXIDR1_ID18_MASK            32U
#define CAN4RXIDR1_ID19_MASK            64U
#define CAN4RXIDR1_ID20_MASK            128U
#define CAN4RXIDR1_ID_15_MASK           7U
#define CAN4RXIDR1_ID_15_BITNUM         0U
#define CAN4RXIDR1_ID_18_MASK           224U
#define CAN4RXIDR1_ID_18_BITNUM         5U


/*** CAN4RXIDR2 - MSCAN4 Receive Identifier Register 2; 0x000002A2 ***/
typedef union {
  byte Byte;
  struct {
    byte ID7         :1;                                       /* Extended format identifier Bit 7 */
    byte ID8         :1;                                       /* Extended format identifier Bit 8 */
    byte ID9         :1;                                       /* Extended format identifier Bit 9 */
    byte ID10        :1;                                       /* Extended format identifier Bit 10 */
    byte ID11        :1;                                       /* Extended format identifier Bit 11 */
    byte ID12        :1;                                       /* Extended format identifier Bit 12 */
    byte ID13        :1;                                       /* Extended format identifier Bit 13 */
    byte ID14        :1;                                       /* Extended format identifier Bit 14 */
  } Bits;
} CAN4RXIDR2STR;
extern volatile CAN4RXIDR2STR _CAN4RXIDR2 @(REG_BASE + 0x000002A2UL);
#define CAN4RXIDR2                      _CAN4RXIDR2.Byte
#define CAN4RXIDR2_ID7                  _CAN4RXIDR2.Bits.ID7
#define CAN4RXIDR2_ID8                  _CAN4RXIDR2.Bits.ID8
#define CAN4RXIDR2_ID9                  _CAN4RXIDR2.Bits.ID9
#define CAN4RXIDR2_ID10                 _CAN4RXIDR2.Bits.ID10
#define CAN4RXIDR2_ID11                 _CAN4RXIDR2.Bits.ID11
#define CAN4RXIDR2_ID12                 _CAN4RXIDR2.Bits.ID12
#define CAN4RXIDR2_ID13                 _CAN4RXIDR2.Bits.ID13
#define CAN4RXIDR2_ID14                 _CAN4RXIDR2.Bits.ID14

#define CAN4RXIDR2_ID7_MASK             1U
#define CAN4RXIDR2_ID8_MASK             2U
#define CAN4RXIDR2_ID9_MASK             4U
#define CAN4RXIDR2_ID10_MASK            8U
#define CAN4RXIDR2_ID11_MASK            16U
#define CAN4RXIDR2_ID12_MASK            32U
#define CAN4RXIDR2_ID13_MASK            64U
#define CAN4RXIDR2_ID14_MASK            128U


/*** CAN4RXIDR3 - MSCAN4 Receive Identifier Register 3; 0x000002A3 ***/
typedef union {
  byte Byte;
  struct {
    byte RTR         :1;                                       /* Remote Transmission Request */
    byte ID0         :1;                                       /* Extended format identifier Bit 0 */
    byte ID1         :1;                                       /* Extended format identifier Bit 1 */
    byte ID2         :1;                                       /* Extended format identifier Bit 2 */
    byte ID3         :1;                                       /* Extended format identifier Bit 3 */
    byte ID4         :1;                                       /* Extended format identifier Bit 4 */
    byte ID5         :1;                                       /* Extended format identifier Bit 5 */
    byte ID6         :1;                                       /* Extended format identifier Bit 6 */
  } Bits;
  struct {
    byte         :1;
    byte grpID   :7;
  } MergedBits;
} CAN4RXIDR3STR;
extern volatile CAN4RXIDR3STR _CAN4RXIDR3 @(REG_BASE + 0x000002A3UL);
#define CAN4RXIDR3                      _CAN4RXIDR3.Byte
#define CAN4RXIDR3_RTR                  _CAN4RXIDR3.Bits.RTR
#define CAN4RXIDR3_ID0                  _CAN4RXIDR3.Bits.ID0
#define CAN4RXIDR3_ID1                  _CAN4RXIDR3.Bits.ID1
#define CAN4RXIDR3_ID2                  _CAN4RXIDR3.Bits.ID2
#define CAN4RXIDR3_ID3                  _CAN4RXIDR3.Bits.ID3
#define CAN4RXIDR3_ID4                  _CAN4RXIDR3.Bits.ID4
#define CAN4RXIDR3_ID5                  _CAN4RXIDR3.Bits.ID5
#define CAN4RXIDR3_ID6                  _CAN4RXIDR3.Bits.ID6
#define CAN4RXIDR3_ID                   _CAN4RXIDR3.MergedBits.grpID

#define CAN4RXIDR3_RTR_MASK             1U
#define CAN4RXIDR3_ID0_MASK             2U
#define CAN4RXIDR3_ID1_MASK             4U
#define CAN4RXIDR3_ID2_MASK             8U
#define CAN4RXIDR3_ID3_MASK             16U
#define CAN4RXIDR3_ID4_MASK             32U
#define CAN4RXIDR3_ID5_MASK             64U
#define CAN4RXIDR3_ID6_MASK             128U
#define CAN4RXIDR3_ID_MASK              254U
#define CAN4RXIDR3_ID_BITNUM            1U


/*** CAN4RXDSR0 - MSCAN4 Receive Data Segment Register 0; 0x000002A4 ***/
typedef union {
  byte Byte;
  struct {
    byte DB0         :1;                                       /* Data Bit 0 */
    byte DB1         :1;                                       /* Data Bit 1 */
    byte DB2         :1;                                       /* Data Bit 2 */
    byte DB3         :1;                                       /* Data Bit 3 */
    byte DB4         :1;                                       /* Data Bit 4 */
    byte DB5         :1;                                       /* Data Bit 5 */
    byte DB6         :1;                                       /* Data Bit 6 */
    byte DB7         :1;                                       /* Data Bit 7 */
  } Bits;
} CAN4RXDSR0STR;
extern volatile CAN4RXDSR0STR _CAN4RXDSR0 @(REG_BASE + 0x000002A4UL);
#define CAN4RXDSR0                      _CAN4RXDSR0.Byte
#define CAN4RXDSR0_DB0                  _CAN4RXDSR0.Bits.DB0
#define CAN4RXDSR0_DB1                  _CAN4RXDSR0.Bits.DB1
#define CAN4RXDSR0_DB2                  _CAN4RXDSR0.Bits.DB2
#define CAN4RXDSR0_DB3                  _CAN4RXDSR0.Bits.DB3
#define CAN4RXDSR0_DB4                  _CAN4RXDSR0.Bits.DB4
#define CAN4RXDSR0_DB5                  _CAN4RXDSR0.Bits.DB5
#define CAN4RXDSR0_DB6                  _CAN4RXDSR0.Bits.DB6
#define CAN4RXDSR0_DB7                  _CAN4RXDSR0.Bits.DB7
/* CAN4RXDSR_ARR: Access 8 CAN4RXDSRx registers in an array */
#define CAN4RXDSR_ARR                   ((volatile byte *) &CAN4RXDSR0)

#define CAN4RXDSR0_DB0_MASK             1U
#define CAN4RXDSR0_DB1_MASK             2U
#define CAN4RXDSR0_DB2_MASK             4U
#define CAN4RXDSR0_DB3_MASK             8U
#define CAN4RXDSR0_DB4_MASK             16U
#define CAN4RXDSR0_DB5_MASK             32U
#define CAN4RXDSR0_DB6_MASK             64U
#define CAN4RXDSR0_DB7_MASK             128U


/*** CAN4RXDSR1 - MSCAN4 Receive Data Segment Register 1; 0x000002A5 ***/
typedef union {
  byte Byte;
  struct {
    byte DB0         :1;                                       /* Data Bit 0 */
    byte DB1         :1;                                       /* Data Bit 1 */
    byte DB2         :1;                                       /* Data Bit 2 */
    byte DB3         :1;                                       /* Data Bit 3 */
    byte DB4         :1;                                       /* Data Bit 4 */
    byte DB5         :1;                                       /* Data Bit 5 */
    byte DB6         :1;                                       /* Data Bit 6 */
    byte DB7         :1;                                       /* Data Bit 7 */
  } Bits;
} CAN4RXDSR1STR;
extern volatile CAN4RXDSR1STR _CAN4RXDSR1 @(REG_BASE + 0x000002A5UL);
#define CAN4RXDSR1                      _CAN4RXDSR1.Byte
#define CAN4RXDSR1_DB0                  _CAN4RXDSR1.Bits.DB0
#define CAN4RXDSR1_DB1                  _CAN4RXDSR1.Bits.DB1
#define CAN4RXDSR1_DB2                  _CAN4RXDSR1.Bits.DB2
#define CAN4RXDSR1_DB3                  _CAN4RXDSR1.Bits.DB3
#define CAN4RXDSR1_DB4                  _CAN4RXDSR1.Bits.DB4
#define CAN4RXDSR1_DB5                  _CAN4RXDSR1.Bits.DB5
#define CAN4RXDSR1_DB6                  _CAN4RXDSR1.Bits.DB6
#define CAN4RXDSR1_DB7                  _CAN4RXDSR1.Bits.DB7

#define CAN4RXDSR1_DB0_MASK             1U
#define CAN4RXDSR1_DB1_MASK             2U
#define CAN4RXDSR1_DB2_MASK             4U
#define CAN4RXDSR1_DB3_MASK             8U
#define CAN4RXDSR1_DB4_MASK             16U
#define CAN4RXDSR1_DB5_MASK             32U
#define CAN4RXDSR1_DB6_MASK             64U
#define CAN4RXDSR1_DB7_MASK             128U


/*** CAN4RXDSR2 - MSCAN4 Receive Data Segment Register 2; 0x000002A6 ***/
typedef union {
  byte Byte;
  struct {
    byte DB0         :1;                                       /* Data Bit 0 */
    byte DB1         :1;                                       /* Data Bit 1 */
    byte DB2         :1;                                       /* Data Bit 2 */
    byte DB3         :1;                                       /* Data Bit 3 */
    byte DB4         :1;                                       /* Data Bit 4 */
    byte DB5         :1;                                       /* Data Bit 5 */
    byte DB6         :1;                                       /* Data Bit 6 */
    byte DB7         :1;                                       /* Data Bit 7 */
  } Bits;
} CAN4RXDSR2STR;
extern volatile CAN4RXDSR2STR _CAN4RXDSR2 @(REG_BASE + 0x000002A6UL);
#define CAN4RXDSR2                      _CAN4RXDSR2.Byte
#define CAN4RXDSR2_DB0                  _CAN4RXDSR2.Bits.DB0
#define CAN4RXDSR2_DB1                  _CAN4RXDSR2.Bits.DB1
#define CAN4RXDSR2_DB2                  _CAN4RXDSR2.Bits.DB2
#define CAN4RXDSR2_DB3                  _CAN4RXDSR2.Bits.DB3
#define CAN4RXDSR2_DB4                  _CAN4RXDSR2.Bits.DB4
#define CAN4RXDSR2_DB5                  _CAN4RXDSR2.Bits.DB5
#define CAN4RXDSR2_DB6                  _CAN4RXDSR2.Bits.DB6
#define CAN4RXDSR2_DB7                  _CAN4RXDSR2.Bits.DB7

#define CAN4RXDSR2_DB0_MASK             1U
#define CAN4RXDSR2_DB1_MASK             2U
#define CAN4RXDSR2_DB2_MASK             4U
#define CAN4RXDSR2_DB3_MASK             8U
#define CAN4RXDSR2_DB4_MASK             16U
#define CAN4RXDSR2_DB5_MASK             32U
#define CAN4RXDSR2_DB6_MASK             64U
#define CAN4RXDSR2_DB7_MASK             128U


/*** CAN4RXDSR3 - MSCAN4 Receive Data Segment Register 3; 0x000002A7 ***/
typedef union {
  byte Byte;
  struct {
    byte DB0         :1;                                       /* Data Bit 0 */
    byte DB1         :1;                                       /* Data Bit 1 */
    byte DB2         :1;                                       /* Data Bit 2 */
    byte DB3         :1;                                       /* Data Bit 3 */
    byte DB4         :1;                                       /* Data Bit 4 */
    byte DB5         :1;                                       /* Data Bit 5 */
    byte DB6         :1;                                       /* Data Bit 6 */
    byte DB7         :1;                                       /* Data Bit 7 */
  } Bits;
} CAN4RXDSR3STR;
extern volatile CAN4RXDSR3STR _CAN4RXDSR3 @(REG_BASE + 0x000002A7UL);
#define CAN4RXDSR3                      _CAN4RXDSR3.Byte
#define CAN4RXDSR3_DB0                  _CAN4RXDSR3.Bits.DB0
#define CAN4RXDSR3_DB1                  _CAN4RXDSR3.Bits.DB1
#define CAN4RXDSR3_DB2                  _CAN4RXDSR3.Bits.DB2
#define CAN4RXDSR3_DB3                  _CAN4RXDSR3.Bits.DB3
#define CAN4RXDSR3_DB4                  _CAN4RXDSR3.Bits.DB4
#define CAN4RXDSR3_DB5                  _CAN4RXDSR3.Bits.DB5
#define CAN4RXDSR3_DB6                  _CAN4RXDSR3.Bits.DB6
#define CAN4RXDSR3_DB7                  _CAN4RXDSR3.Bits.DB7

#define CAN4RXDSR3_DB0_MASK             1U
#define CAN4RXDSR3_DB1_MASK             2U
#define CAN4RXDSR3_DB2_MASK             4U
#define CAN4RXDSR3_DB3_MASK             8U
#define CAN4RXDSR3_DB4_MASK             16U
#define CAN4RXDSR3_DB5_MASK             32U
#define CAN4RXDSR3_DB6_MASK             64U
#define CAN4RXDSR3_DB7_MASK             128U


/*** CAN4RXDSR4 - MSCAN4 Receive Data Segment Register 4; 0x000002A8 ***/
typedef union {
  byte Byte;
  struct {
    byte DB0         :1;                                       /* Data Bit 0 */
    byte DB1         :1;                                       /* Data Bit 1 */
    byte DB2         :1;                                       /* Data Bit 2 */
    byte DB3         :1;                                       /* Data Bit 3 */
    byte DB4         :1;                                       /* Data Bit 4 */
    byte DB5         :1;                                       /* Data Bit 5 */
    byte DB6         :1;                                       /* Data Bit 6 */
    byte DB7         :1;                                       /* Data Bit 7 */
  } Bits;
} CAN4RXDSR4STR;
extern volatile CAN4RXDSR4STR _CAN4RXDSR4 @(REG_BASE + 0x000002A8UL);
#define CAN4RXDSR4                      _CAN4RXDSR4.Byte
#define CAN4RXDSR4_DB0                  _CAN4RXDSR4.Bits.DB0
#define CAN4RXDSR4_DB1                  _CAN4RXDSR4.Bits.DB1
#define CAN4RXDSR4_DB2                  _CAN4RXDSR4.Bits.DB2
#define CAN4RXDSR4_DB3                  _CAN4RXDSR4.Bits.DB3
#define CAN4RXDSR4_DB4                  _CAN4RXDSR4.Bits.DB4
#define CAN4RXDSR4_DB5                  _CAN4RXDSR4.Bits.DB5
#define CAN4RXDSR4_DB6                  _CAN4RXDSR4.Bits.DB6
#define CAN4RXDSR4_DB7                  _CAN4RXDSR4.Bits.DB7

#define CAN4RXDSR4_DB0_MASK             1U
#define CAN4RXDSR4_DB1_MASK             2U
#define CAN4RXDSR4_DB2_MASK             4U
#define CAN4RXDSR4_DB3_MASK             8U
#define CAN4RXDSR4_DB4_MASK             16U
#define CAN4RXDSR4_DB5_MASK             32U
#define CAN4RXDSR4_DB6_MASK             64U
#define CAN4RXDSR4_DB7_MASK             128U


/*** CAN4RXDSR5 - MSCAN4 Receive Data Segment Register 5; 0x000002A9 ***/
typedef union {
  byte Byte;
  struct {
    byte DB0         :1;                                       /* Data Bit 0 */
    byte DB1         :1;                                       /* Data Bit 1 */
    byte DB2         :1;                                       /* Data Bit 2 */
    byte DB3         :1;                                       /* Data Bit 3 */
    byte DB4         :1;                                       /* Data Bit 4 */
    byte DB5         :1;                                       /* Data Bit 5 */
    byte DB6         :1;                                       /* Data Bit 6 */
    byte DB7         :1;                                       /* Data Bit 7 */
  } Bits;
} CAN4RXDSR5STR;
extern volatile CAN4RXDSR5STR _CAN4RXDSR5 @(REG_BASE + 0x000002A9UL);
#define CAN4RXDSR5                      _CAN4RXDSR5.Byte
#define CAN4RXDSR5_DB0                  _CAN4RXDSR5.Bits.DB0
#define CAN4RXDSR5_DB1                  _CAN4RXDSR5.Bits.DB1
#define CAN4RXDSR5_DB2                  _CAN4RXDSR5.Bits.DB2
#define CAN4RXDSR5_DB3                  _CAN4RXDSR5.Bits.DB3
#define CAN4RXDSR5_DB4                  _CAN4RXDSR5.Bits.DB4
#define CAN4RXDSR5_DB5                  _CAN4RXDSR5.Bits.DB5
#define CAN4RXDSR5_DB6                  _CAN4RXDSR5.Bits.DB6
#define CAN4RXDSR5_DB7                  _CAN4RXDSR5.Bits.DB7

#define CAN4RXDSR5_DB0_MASK             1U
#define CAN4RXDSR5_DB1_MASK             2U
#define CAN4RXDSR5_DB2_MASK             4U
#define CAN4RXDSR5_DB3_MASK             8U
#define CAN4RXDSR5_DB4_MASK             16U
#define CAN4RXDSR5_DB5_MASK             32U
#define CAN4RXDSR5_DB6_MASK             64U
#define CAN4RXDSR5_DB7_MASK             128U


/*** CAN4RXDSR6 - MSCAN4 Receive Data Segment Register 6; 0x000002AA ***/
typedef union {
  byte Byte;
  struct {
    byte DB0         :1;                                       /* Data Bit 0 */
    byte DB1         :1;                                       /* Data Bit 1 */
    byte DB2         :1;                                       /* Data Bit 2 */
    byte DB3         :1;                                       /* Data Bit 3 */
    byte DB4         :1;                                       /* Data Bit 4 */
    byte DB5         :1;                                       /* Data Bit 5 */
    byte DB6         :1;                                       /* Data Bit 6 */
    byte DB7         :1;                                       /* Data Bit 7 */
  } Bits;
} CAN4RXDSR6STR;
extern volatile CAN4RXDSR6STR _CAN4RXDSR6 @(REG_BASE + 0x000002AAUL);
#define CAN4RXDSR6                      _CAN4RXDSR6.Byte
#define CAN4RXDSR6_DB0                  _CAN4RXDSR6.Bits.DB0
#define CAN4RXDSR6_DB1                  _CAN4RXDSR6.Bits.DB1
#define CAN4RXDSR6_DB2                  _CAN4RXDSR6.Bits.DB2
#define CAN4RXDSR6_DB3                  _CAN4RXDSR6.Bits.DB3
#define CAN4RXDSR6_DB4                  _CAN4RXDSR6.Bits.DB4
#define CAN4RXDSR6_DB5                  _CAN4RXDSR6.Bits.DB5
#define CAN4RXDSR6_DB6                  _CAN4RXDSR6.Bits.DB6
#define CAN4RXDSR6_DB7                  _CAN4RXDSR6.Bits.DB7

#define CAN4RXDSR6_DB0_MASK             1U
#define CAN4RXDSR6_DB1_MASK             2U
#define CAN4RXDSR6_DB2_MASK             4U
#define CAN4RXDSR6_DB3_MASK             8U
#define CAN4RXDSR6_DB4_MASK             16U
#define CAN4RXDSR6_DB5_MASK             32U
#define CAN4RXDSR6_DB6_MASK             64U
#define CAN4RXDSR6_DB7_MASK             128U


/*** CAN4RXDSR7 - MSCAN4 Receive Data Segment Register 7; 0x000002AB ***/
typedef union {
  byte Byte;
  struct {
    byte DB0         :1;                                       /* Data Bit 0 */
    byte DB1         :1;                                       /* Data Bit 1 */
    byte DB2         :1;                                       /* Data Bit 2 */
    byte DB3         :1;                                       /* Data Bit 3 */
    byte DB4         :1;                                       /* Data Bit 4 */
    byte DB5         :1;                                       /* Data Bit 5 */
    byte DB6         :1;                                       /* Data Bit 6 */
    byte DB7         :1;                                       /* Data Bit 7 */
  } Bits;
} CAN4RXDSR7STR;
extern volatile CAN4RXDSR7STR _CAN4RXDSR7 @(REG_BASE + 0x000002ABUL);
#define CAN4RXDSR7                      _CAN4RXDSR7.Byte
#define CAN4RXDSR7_DB0                  _CAN4RXDSR7.Bits.DB0
#define CAN4RXDSR7_DB1                  _CAN4RXDSR7.Bits.DB1
#define CAN4RXDSR7_DB2                  _CAN4RXDSR7.Bits.DB2
#define CAN4RXDSR7_DB3                  _CAN4RXDSR7.Bits.DB3
#define CAN4RXDSR7_DB4                  _CAN4RXDSR7.Bits.DB4
#define CAN4RXDSR7_DB5                  _CAN4RXDSR7.Bits.DB5
#define CAN4RXDSR7_DB6                  _CAN4RXDSR7.Bits.DB6
#define CAN4RXDSR7_DB7                  _CAN4RXDSR7.Bits.DB7

#define CAN4RXDSR7_DB0_MASK             1U
#define CAN4RXDSR7_DB1_MASK             2U
#define CAN4RXDSR7_DB2_MASK             4U
#define CAN4RXDSR7_DB3_MASK             8U
#define CAN4RXDSR7_DB4_MASK             16U
#define CAN4RXDSR7_DB5_MASK             32U
#define CAN4RXDSR7_DB6_MASK             64U
#define CAN4RXDSR7_DB7_MASK             128U


/*** CAN4RXDLR - MSCAN4 Receive Data Length Register; 0x000002AC ***/
typedef union {
  byte Byte;
  struct {
    byte DLC0        :1;                                       /* Data Length Code Bit 0 */
    byte DLC1        :1;                                       /* Data Length Code Bit 1 */
    byte DLC2        :1;                                       /* Data Length Code Bit 2 */
    byte DLC3        :1;                                       /* Data Length Code Bit 3 */
    byte             :1; 
    byte             :1; 
    byte             :1; 
    byte             :1; 
  } Bits;
  struct {
    byte grpDLC  :4;
    byte         :1;
    byte         :1;
    byte         :1;
    byte         :1;
  } MergedBits;
} CAN4RXDLRSTR;
extern volatile CAN4RXDLRSTR _CAN4RXDLR @(REG_BASE + 0x000002ACUL);
#define CAN4RXDLR                       _CAN4RXDLR.Byte
#define CAN4RXDLR_DLC0                  _CAN4RXDLR.Bits.DLC0
#define CAN4RXDLR_DLC1                  _CAN4RXDLR.Bits.DLC1
#define CAN4RXDLR_DLC2                  _CAN4RXDLR.Bits.DLC2
#define CAN4RXDLR_DLC3                  _CAN4RXDLR.Bits.DLC3
#define CAN4RXDLR_DLC                   _CAN4RXDLR.MergedBits.grpDLC

#define CAN4RXDLR_DLC0_MASK             1U
#define CAN4RXDLR_DLC1_MASK             2U
#define CAN4RXDLR_DLC2_MASK             4U
#define CAN4RXDLR_DLC3_MASK             8U
#define CAN4RXDLR_DLC_MASK              15U
#define CAN4RXDLR_DLC_BITNUM            0U


/*** CAN4RXTSR - MSCAN 4 Receive Time Stamp Register; 0x000002AE ***/
typedef union {
  word Word;
   /* Overlapped registers: */
  struct {
    /*** CAN4RXTSRH - MSCAN 4 Receive Time Stamp Register High; 0x000002AE ***/
    union {
      byte Byte;
      struct {
        byte TSR8        :1;                                       /* Time Stamp Bit 8 */
        byte TSR9        :1;                                       /* Time Stamp Bit 9 */
        byte TSR10       :1;                                       /* Time Stamp Bit 10 */
        byte TSR11       :1;                                       /* Time Stamp Bit 11 */
        byte TSR12       :1;                                       /* Time Stamp Bit 12 */
        byte TSR13       :1;                                       /* Time Stamp Bit 13 */
        byte TSR14       :1;                                       /* Time Stamp Bit 14 */
        byte TSR15       :1;                                       /* Time Stamp Bit 15 */
      } Bits;
    } CAN4RXTSRHSTR;
    #define CAN4RXTSRH                  _CAN4RXTSR.Overlap_STR.CAN4RXTSRHSTR.Byte
    #define CAN4RXTSRH_TSR8             _CAN4RXTSR.Overlap_STR.CAN4RXTSRHSTR.Bits.TSR8
    #define CAN4RXTSRH_TSR9             _CAN4RXTSR.Overlap_STR.CAN4RXTSRHSTR.Bits.TSR9
    #define CAN4RXTSRH_TSR10            _CAN4RXTSR.Overlap_STR.CAN4RXTSRHSTR.Bits.TSR10
    #define CAN4RXTSRH_TSR11            _CAN4RXTSR.Overlap_STR.CAN4RXTSRHSTR.Bits.TSR11
    #define CAN4RXTSRH_TSR12            _CAN4RXTSR.Overlap_STR.CAN4RXTSRHSTR.Bits.TSR12
    #define CAN4RXTSRH_TSR13            _CAN4RXTSR.Overlap_STR.CAN4RXTSRHSTR.Bits.TSR13
    #define CAN4RXTSRH_TSR14            _CAN4RXTSR.Overlap_STR.CAN4RXTSRHSTR.Bits.TSR14
    #define CAN4RXTSRH_TSR15            _CAN4RXTSR.Overlap_STR.CAN4RXTSRHSTR.Bits.TSR15
    
    #define CAN4RXTSRH_TSR8_MASK        1U
    #define CAN4RXTSRH_TSR9_MASK        2U
    #define CAN4RXTSRH_TSR10_MASK       4U
    #define CAN4RXTSRH_TSR11_MASK       8U
    #define CAN4RXTSRH_TSR12_MASK       16U
    #define CAN4RXTSRH_TSR13_MASK       32U
    #define CAN4RXTSRH_TSR14_MASK       64U
    #define CAN4RXTSRH_TSR15_MASK       128U
    

    /*** CAN4RXTSRL - MSCAN 4 Receive Time Stamp Register Low; 0x000002AF ***/
    union {
      byte Byte;
      struct {
        byte TSR0        :1;                                       /* Time Stamp Bit 0 */
        byte TSR1        :1;                                       /* Time Stamp Bit 1 */
        byte TSR2        :1;                                       /* Time Stamp Bit 2 */
        byte TSR3        :1;                                       /* Time Stamp Bit 3 */
        byte TSR4        :1;                                       /* Time Stamp Bit 4 */
        byte TSR5        :1;                                       /* Time Stamp Bit 5 */
        byte TSR6        :1;                                       /* Time Stamp Bit 6 */
        byte TSR7        :1;                                       /* Time Stamp Bit 7 */
      } Bits;
    } CAN4RXTSRLSTR;
    #define CAN4RXTSRL                  _CAN4RXTSR.Overlap_STR.CAN4RXTSRLSTR.Byte
    #define CAN4RXTSRL_TSR0             _CAN4RXTSR.Overlap_STR.CAN4RXTSRLSTR.Bits.TSR0
    #define CAN4RXTSRL_TSR1             _CAN4RXTSR.Overlap_STR.CAN4RXTSRLSTR.Bits.TSR1
    #define CAN4RXTSRL_TSR2             _CAN4RXTSR.Overlap_STR.CAN4RXTSRLSTR.Bits.TSR2
    #define CAN4RXTSRL_TSR3             _CAN4RXTSR.Overlap_STR.CAN4RXTSRLSTR.Bits.TSR3
    #define CAN4RXTSRL_TSR4             _CAN4RXTSR.Overlap_STR.CAN4RXTSRLSTR.Bits.TSR4
    #define CAN4RXTSRL_TSR5             _CAN4RXTSR.Overlap_STR.CAN4RXTSRLSTR.Bits.TSR5
    #define CAN4RXTSRL_TSR6             _CAN4RXTSR.Overlap_STR.CAN4RXTSRLSTR.Bits.TSR6
    #define CAN4RXTSRL_TSR7             _CAN4RXTSR.Overlap_STR.CAN4RXTSRLSTR.Bits.TSR7
    
    #define CAN4RXTSRL_TSR0_MASK        1U
    #define CAN4RXTSRL_TSR1_MASK        2U
    #define CAN4RXTSRL_TSR2_MASK        4U
    #define CAN4RXTSRL_TSR3_MASK        8U
    #define CAN4RXTSRL_TSR4_MASK        16U
    #define CAN4RXTSRL_TSR5_MASK        32U
    #define CAN4RXTSRL_TSR6_MASK        64U
    #define CAN4RXTSRL_TSR7_MASK        128U
    
  } Overlap_STR;

  struct {
    word TSR0        :1;                                       /* Time Stamp Bit 0 */
    word TSR1        :1;                                       /* Time Stamp Bit 1 */
    word TSR2        :1;                                       /* Time Stamp Bit 2 */
    word TSR3        :1;                                       /* Time Stamp Bit 3 */
    word TSR4        :1;                                       /* Time Stamp Bit 4 */
    word TSR5        :1;                                       /* Time Stamp Bit 5 */
    word TSR6        :1;                                       /* Time Stamp Bit 6 */
    word TSR7        :1;                                       /* Time Stamp Bit 7 */
    word TSR8        :1;                                       /* Time Stamp Bit 8 */
    word TSR9        :1;                                       /* Time Stamp Bit 9 */
    word TSR10       :1;                                       /* Time Stamp Bit 10 */
    word TSR11       :1;                                       /* Time Stamp Bit 11 */
    word TSR12       :1;                                       /* Time Stamp Bit 12 */
    word TSR13       :1;                                       /* Time Stamp Bit 13 */
    word TSR14       :1;                                       /* Time Stamp Bit 14 */
    word TSR15       :1;                                       /* Time Stamp Bit 15 */
  } Bits;
} CAN4RXTSRSTR;
extern volatile CAN4RXTSRSTR _CAN4RXTSR @(REG_BASE + 0x000002AEUL);
#define CAN4RXTSR                       _CAN4RXTSR.Word
#define CAN4RXTSR_TSR0                  _CAN4RXTSR.Bits.TSR0
#define CAN4RXTSR_TSR1                  _CAN4RXTSR.Bits.TSR1
#define CAN4RXTSR_TSR2                  _CAN4RXTSR.Bits.TSR2
#define CAN4RXTSR_TSR3                  _CAN4RXTSR.Bits.TSR3
#define CAN4RXTSR_TSR4                  _CAN4RXTSR.Bits.TSR4
#define CAN4RXTSR_TSR5                  _CAN4RXTSR.Bits.TSR5
#define CAN4RXTSR_TSR6                  _CAN4RXTSR.Bits.TSR6
#define CAN4RXTSR_TSR7                  _CAN4RXTSR.Bits.TSR7
#define CAN4RXTSR_TSR8                  _CAN4RXTSR.Bits.TSR8
#define CAN4RXTSR_TSR9                  _CAN4RXTSR.Bits.TSR9
#define CAN4RXTSR_TSR10                 _CAN4RXTSR.Bits.TSR10
#define CAN4RXTSR_TSR11                 _CAN4RXTSR.Bits.TSR11
#define CAN4RXTSR_TSR12                 _CAN4RXTSR.Bits.TSR12
#define CAN4RXTSR_TSR13                 _CAN4RXTSR.Bits.TSR13
#define CAN4RXTSR_TSR14                 _CAN4RXTSR.Bits.TSR14
#define CAN4RXTSR_TSR15                 _CAN4RXTSR.Bits.TSR15

#define CAN4RXTSR_TSR0_MASK             1U
#define CAN4RXTSR_TSR1_MASK             2U
#define CAN4RXTSR_TSR2_MASK             4U
#define CAN4RXTSR_TSR3_MASK             8U
#define CAN4RXTSR_TSR4_MASK             16U
#define CAN4RXTSR_TSR5_MASK             32U
#define CAN4RXTSR_TSR6_MASK             64U
#define CAN4RXTSR_TSR7_MASK             128U
#define CAN4RXTSR_TSR8_MASK             256U
#define CAN4RXTSR_TSR9_MASK             512U
#define CAN4RXTSR_TSR10_MASK            1024U
#define CAN4RXTSR_TSR11_MASK            2048U
#define CAN4RXTSR_TSR12_MASK            4096U
#define CAN4RXTSR_TSR13_MASK            8192U
#define CAN4RXTSR_TSR14_MASK            16384U
#define CAN4RXTSR_TSR15_MASK            32768U


/*** CAN4TXIDR0 - MSCAN4 Transmit Identifier Register 0; 0x000002B0 ***/
typedef union {
  byte Byte;
  struct {
    byte ID21        :1;                                       /* Extended format identifier Bit 21 */
    byte ID22        :1;                                       /* Extended format identifier Bit 22 */
    byte ID23        :1;                                       /* Extended format identifier Bit 23 */
    byte ID24        :1;                                       /* Extended format identifier Bit 24 */
    byte ID25        :1;                                       /* Extended format identifier Bit 25 */
    byte ID26        :1;                                       /* Extended format identifier Bit 26 */
    byte ID27        :1;                                       /* Extended format identifier Bit 27 */
    byte ID28        :1;                                       /* Extended format identifier Bit 28 */
  } Bits;
} CAN4TXIDR0STR;
extern volatile CAN4TXIDR0STR _CAN4TXIDR0 @(REG_BASE + 0x000002B0UL);
#define CAN4TXIDR0                      _CAN4TXIDR0.Byte
#define CAN4TXIDR0_ID21                 _CAN4TXIDR0.Bits.ID21
#define CAN4TXIDR0_ID22                 _CAN4TXIDR0.Bits.ID22
#define CAN4TXIDR0_ID23                 _CAN4TXIDR0.Bits.ID23
#define CAN4TXIDR0_ID24                 _CAN4TXIDR0.Bits.ID24
#define CAN4TXIDR0_ID25                 _CAN4TXIDR0.Bits.ID25
#define CAN4TXIDR0_ID26                 _CAN4TXIDR0.Bits.ID26
#define CAN4TXIDR0_ID27                 _CAN4TXIDR0.Bits.ID27
#define CAN4TXIDR0_ID28                 _CAN4TXIDR0.Bits.ID28
/* CAN4TXIDR_ARR: Access 4 CAN4TXIDRx registers in an array */
#define CAN4TXIDR_ARR                   ((volatile byte *) &CAN4TXIDR0)

#define CAN4TXIDR0_ID21_MASK            1U
#define CAN4TXIDR0_ID22_MASK            2U
#define CAN4TXIDR0_ID23_MASK            4U
#define CAN4TXIDR0_ID24_MASK            8U
#define CAN4TXIDR0_ID25_MASK            16U
#define CAN4TXIDR0_ID26_MASK            32U
#define CAN4TXIDR0_ID27_MASK            64U
#define CAN4TXIDR0_ID28_MASK            128U


/*** CAN4TXIDR1 - MSCAN4 Transmit Identifier Register 1; 0x000002B1 ***/
typedef union {
  byte Byte;
  struct {
    byte ID15        :1;                                       /* Extended format identifier Bit 15 */
    byte ID16        :1;                                       /* Extended format identifier Bit 16 */
    byte ID17        :1;                                       /* Extended format identifier Bit 17 */
    byte IDE         :1;                                       /* ID Extended */
    byte SRR         :1;                                       /* Substitute Remote Request */
    byte ID18        :1;                                       /* Extended format identifier Bit 18 */
    byte ID19        :1;                                       /* Extended format identifier Bit 19 */
    byte ID20        :1;                                       /* Extended format identifier Bit 20 */
  } Bits;
  struct {
    byte grpID_15 :3;
    byte         :1;
    byte         :1;
    byte grpID_18 :3;
  } MergedBits;
} CAN4TXIDR1STR;
extern volatile CAN4TXIDR1STR _CAN4TXIDR1 @(REG_BASE + 0x000002B1UL);
#define CAN4TXIDR1                      _CAN4TXIDR1.Byte
#define CAN4TXIDR1_ID15                 _CAN4TXIDR1.Bits.ID15
#define CAN4TXIDR1_ID16                 _CAN4TXIDR1.Bits.ID16
#define CAN4TXIDR1_ID17                 _CAN4TXIDR1.Bits.ID17
#define CAN4TXIDR1_IDE                  _CAN4TXIDR1.Bits.IDE
#define CAN4TXIDR1_SRR                  _CAN4TXIDR1.Bits.SRR
#define CAN4TXIDR1_ID18                 _CAN4TXIDR1.Bits.ID18
#define CAN4TXIDR1_ID19                 _CAN4TXIDR1.Bits.ID19
#define CAN4TXIDR1_ID20                 _CAN4TXIDR1.Bits.ID20
#define CAN4TXIDR1_ID_15                _CAN4TXIDR1.MergedBits.grpID_15
#define CAN4TXIDR1_ID_18                _CAN4TXIDR1.MergedBits.grpID_18
#define CAN4TXIDR1_ID                   CAN4TXIDR1_ID_15

#define CAN4TXIDR1_ID15_MASK            1U
#define CAN4TXIDR1_ID16_MASK            2U
#define CAN4TXIDR1_ID17_MASK            4U
#define CAN4TXIDR1_IDE_MASK             8U
#define CAN4TXIDR1_SRR_MASK             16U
#define CAN4TXIDR1_ID18_MASK            32U
#define CAN4TXIDR1_ID19_MASK            64U
#define CAN4TXIDR1_ID20_MASK            128U
#define CAN4TXIDR1_ID_15_MASK           7U
#define CAN4TXIDR1_ID_15_BITNUM         0U
#define CAN4TXIDR1_ID_18_MASK           224U
#define CAN4TXIDR1_ID_18_BITNUM         5U


/*** CAN4TXIDR2 - MSCAN4 Transmit Identifier Register 2; 0x000002B2 ***/
typedef union {
  byte Byte;
  struct {
    byte ID7         :1;                                       /* Extended format identifier Bit 7 */
    byte ID8         :1;                                       /* Extended format identifier Bit 8 */
    byte ID9         :1;                                       /* Extended format identifier Bit 9 */
    byte ID10        :1;                                       /* Extended format identifier Bit 10 */
    byte ID11        :1;                                       /* Extended format identifier Bit 11 */
    byte ID12        :1;                                       /* Extended format identifier Bit 12 */
    byte ID13        :1;                                       /* Extended format identifier Bit 13 */
    byte ID14        :1;                                       /* Extended format identifier Bit 14 */
  } Bits;
} CAN4TXIDR2STR;
extern volatile CAN4TXIDR2STR _CAN4TXIDR2 @(REG_BASE + 0x000002B2UL);
#define CAN4TXIDR2                      _CAN4TXIDR2.Byte
#define CAN4TXIDR2_ID7                  _CAN4TXIDR2.Bits.ID7
#define CAN4TXIDR2_ID8                  _CAN4TXIDR2.Bits.ID8
#define CAN4TXIDR2_ID9                  _CAN4TXIDR2.Bits.ID9
#define CAN4TXIDR2_ID10                 _CAN4TXIDR2.Bits.ID10
#define CAN4TXIDR2_ID11                 _CAN4TXIDR2.Bits.ID11
#define CAN4TXIDR2_ID12                 _CAN4TXIDR2.Bits.ID12
#define CAN4TXIDR2_ID13                 _CAN4TXIDR2.Bits.ID13
#define CAN4TXIDR2_ID14                 _CAN4TXIDR2.Bits.ID14

#define CAN4TXIDR2_ID7_MASK             1U
#define CAN4TXIDR2_ID8_MASK             2U
#define CAN4TXIDR2_ID9_MASK             4U
#define CAN4TXIDR2_ID10_MASK            8U
#define CAN4TXIDR2_ID11_MASK            16U
#define CAN4TXIDR2_ID12_MASK            32U
#define CAN4TXIDR2_ID13_MASK            64U
#define CAN4TXIDR2_ID14_MASK            128U


/*** CAN4TXIDR3 - MSCAN4 Transmit Identifier Register 3; 0x000002B3 ***/
typedef union {
  byte Byte;
  struct {
    byte RTR         :1;                                       /* Remote Transmission Request */
    byte ID0         :1;                                       /* Extended format identifier Bit 0 */
    byte ID1         :1;                                       /* Extended format identifier Bit 1 */
    byte ID2         :1;                                       /* Extended format identifier Bit 2 */
    byte ID3         :1;                                       /* Extended format identifier Bit 3 */
    byte ID4         :1;                                       /* Extended format identifier Bit 4 */
    byte ID5         :1;                                       /* Extended format identifier Bit 5 */
    byte ID6         :1;                                       /* Extended format identifier Bit 6 */
  } Bits;
  struct {
    byte         :1;
    byte grpID   :7;
  } MergedBits;
} CAN4TXIDR3STR;
extern volatile CAN4TXIDR3STR _CAN4TXIDR3 @(REG_BASE + 0x000002B3UL);
#define CAN4TXIDR3                      _CAN4TXIDR3.Byte
#define CAN4TXIDR3_RTR                  _CAN4TXIDR3.Bits.RTR
#define CAN4TXIDR3_ID0                  _CAN4TXIDR3.Bits.ID0
#define CAN4TXIDR3_ID1                  _CAN4TXIDR3.Bits.ID1
#define CAN4TXIDR3_ID2                  _CAN4TXIDR3.Bits.ID2
#define CAN4TXIDR3_ID3                  _CAN4TXIDR3.Bits.ID3
#define CAN4TXIDR3_ID4                  _CAN4TXIDR3.Bits.ID4
#define CAN4TXIDR3_ID5                  _CAN4TXIDR3.Bits.ID5
#define CAN4TXIDR3_ID6                  _CAN4TXIDR3.Bits.ID6
#define CAN4TXIDR3_ID                   _CAN4TXIDR3.MergedBits.grpID

#define CAN4TXIDR3_RTR_MASK             1U
#define CAN4TXIDR3_ID0_MASK             2U
#define CAN4TXIDR3_ID1_MASK             4U
#define CAN4TXIDR3_ID2_MASK             8U
#define CAN4TXIDR3_ID3_MASK             16U
#define CAN4TXIDR3_ID4_MASK             32U
#define CAN4TXIDR3_ID5_MASK             64U
#define CAN4TXIDR3_ID6_MASK             128U
#define CAN4TXIDR3_ID_MASK              254U
#define CAN4TXIDR3_ID_BITNUM            1U


/*** CAN4TXDSR0 - MSCAN4 Transmit Data Segment Register 0; 0x000002B4 ***/
typedef union {
  byte Byte;
  struct {
    byte DB0         :1;                                       /* Data Bit 0 */
    byte DB1         :1;                                       /* Data Bit 1 */
    byte DB2         :1;                                       /* Data Bit 2 */
    byte DB3         :1;                                       /* Data Bit 3 */
    byte DB4         :1;                                       /* Data Bit 4 */
    byte DB5         :1;                                       /* Data Bit 5 */
    byte DB6         :1;                                       /* Data Bit 6 */
    byte DB7         :1;                                       /* Data Bit 7 */
  } Bits;
} CAN4TXDSR0STR;
extern volatile CAN4TXDSR0STR _CAN4TXDSR0 @(REG_BASE + 0x000002B4UL);
#define CAN4TXDSR0                      _CAN4TXDSR0.Byte
#define CAN4TXDSR0_DB0                  _CAN4TXDSR0.Bits.DB0
#define CAN4TXDSR0_DB1                  _CAN4TXDSR0.Bits.DB1
#define CAN4TXDSR0_DB2                  _CAN4TXDSR0.Bits.DB2
#define CAN4TXDSR0_DB3                  _CAN4TXDSR0.Bits.DB3
#define CAN4TXDSR0_DB4                  _CAN4TXDSR0.Bits.DB4
#define CAN4TXDSR0_DB5                  _CAN4TXDSR0.Bits.DB5
#define CAN4TXDSR0_DB6                  _CAN4TXDSR0.Bits.DB6
#define CAN4TXDSR0_DB7                  _CAN4TXDSR0.Bits.DB7
/* CAN4TXDSR_ARR: Access 8 CAN4TXDSRx registers in an array */
#define CAN4TXDSR_ARR                   ((volatile byte *) &CAN4TXDSR0)

#define CAN4TXDSR0_DB0_MASK             1U
#define CAN4TXDSR0_DB1_MASK             2U
#define CAN4TXDSR0_DB2_MASK             4U
#define CAN4TXDSR0_DB3_MASK             8U
#define CAN4TXDSR0_DB4_MASK             16U
#define CAN4TXDSR0_DB5_MASK             32U
#define CAN4TXDSR0_DB6_MASK             64U
#define CAN4TXDSR0_DB7_MASK             128U


/*** CAN4TXDSR1 - MSCAN4 Transmit Data Segment Register 1; 0x000002B5 ***/
typedef union {
  byte Byte;
  struct {
    byte DB0         :1;                                       /* Data Bit 0 */
    byte DB1         :1;                                       /* Data Bit 1 */
    byte DB2         :1;                                       /* Data Bit 2 */
    byte DB3         :1;                                       /* Data Bit 3 */
    byte DB4         :1;                                       /* Data Bit 4 */
    byte DB5         :1;                                       /* Data Bit 5 */
    byte DB6         :1;                                       /* Data Bit 6 */
    byte DB7         :1;                                       /* Data Bit 7 */
  } Bits;
} CAN4TXDSR1STR;
extern volatile CAN4TXDSR1STR _CAN4TXDSR1 @(REG_BASE + 0x000002B5UL);
#define CAN4TXDSR1                      _CAN4TXDSR1.Byte
#define CAN4TXDSR1_DB0                  _CAN4TXDSR1.Bits.DB0
#define CAN4TXDSR1_DB1                  _CAN4TXDSR1.Bits.DB1
#define CAN4TXDSR1_DB2                  _CAN4TXDSR1.Bits.DB2
#define CAN4TXDSR1_DB3                  _CAN4TXDSR1.Bits.DB3
#define CAN4TXDSR1_DB4                  _CAN4TXDSR1.Bits.DB4
#define CAN4TXDSR1_DB5                  _CAN4TXDSR1.Bits.DB5
#define CAN4TXDSR1_DB6                  _CAN4TXDSR1.Bits.DB6
#define CAN4TXDSR1_DB7                  _CAN4TXDSR1.Bits.DB7

#define CAN4TXDSR1_DB0_MASK             1U
#define CAN4TXDSR1_DB1_MASK             2U
#define CAN4TXDSR1_DB2_MASK             4U
#define CAN4TXDSR1_DB3_MASK             8U
#define CAN4TXDSR1_DB4_MASK             16U
#define CAN4TXDSR1_DB5_MASK             32U
#define CAN4TXDSR1_DB6_MASK             64U
#define CAN4TXDSR1_DB7_MASK             128U


/*** CAN4TXDSR2 - MSCAN4 Transmit Data Segment Register 2; 0x000002B6 ***/
typedef union {
  byte Byte;
  struct {
    byte DB0         :1;                                       /* Data Bit 0 */
    byte DB1         :1;                                       /* Data Bit 1 */
    byte DB2         :1;                                       /* Data Bit 2 */
    byte DB3         :1;                                       /* Data Bit 3 */
    byte DB4         :1;                                       /* Data Bit 4 */
    byte DB5         :1;                                       /* Data Bit 5 */
    byte DB6         :1;                                       /* Data Bit 6 */
    byte DB7         :1;                                       /* Data Bit 7 */
  } Bits;
} CAN4TXDSR2STR;
extern volatile CAN4TXDSR2STR _CAN4TXDSR2 @(REG_BASE + 0x000002B6UL);
#define CAN4TXDSR2                      _CAN4TXDSR2.Byte
#define CAN4TXDSR2_DB0                  _CAN4TXDSR2.Bits.DB0
#define CAN4TXDSR2_DB1                  _CAN4TXDSR2.Bits.DB1
#define CAN4TXDSR2_DB2                  _CAN4TXDSR2.Bits.DB2
#define CAN4TXDSR2_DB3                  _CAN4TXDSR2.Bits.DB3
#define CAN4TXDSR2_DB4                  _CAN4TXDSR2.Bits.DB4
#define CAN4TXDSR2_DB5                  _CAN4TXDSR2.Bits.DB5
#define CAN4TXDSR2_DB6                  _CAN4TXDSR2.Bits.DB6
#define CAN4TXDSR2_DB7                  _CAN4TXDSR2.Bits.DB7

#define CAN4TXDSR2_DB0_MASK             1U
#define CAN4TXDSR2_DB1_MASK             2U
#define CAN4TXDSR2_DB2_MASK             4U
#define CAN4TXDSR2_DB3_MASK             8U
#define CAN4TXDSR2_DB4_MASK             16U
#define CAN4TXDSR2_DB5_MASK             32U
#define CAN4TXDSR2_DB6_MASK             64U
#define CAN4TXDSR2_DB7_MASK             128U


/*** CAN4TXDSR3 - MSCAN4 Transmit Data Segment Register 3; 0x000002B7 ***/
typedef union {
  byte Byte;
  struct {
    byte DB0         :1;                                       /* Data Bit 0 */
    byte DB1         :1;                                       /* Data Bit 1 */
    byte DB2         :1;                                       /* Data Bit 2 */
    byte DB3         :1;                                       /* Data Bit 3 */
    byte DB4         :1;                                       /* Data Bit 4 */
    byte DB5         :1;                                       /* Data Bit 5 */
    byte DB6         :1;                                       /* Data Bit 6 */
    byte DB7         :1;                                       /* Data Bit 7 */
  } Bits;
} CAN4TXDSR3STR;
extern volatile CAN4TXDSR3STR _CAN4TXDSR3 @(REG_BASE + 0x000002B7UL);
#define CAN4TXDSR3                      _CAN4TXDSR3.Byte
#define CAN4TXDSR3_DB0                  _CAN4TXDSR3.Bits.DB0
#define CAN4TXDSR3_DB1                  _CAN4TXDSR3.Bits.DB1
#define CAN4TXDSR3_DB2                  _CAN4TXDSR3.Bits.DB2
#define CAN4TXDSR3_DB3                  _CAN4TXDSR3.Bits.DB3
#define CAN4TXDSR3_DB4                  _CAN4TXDSR3.Bits.DB4
#define CAN4TXDSR3_DB5                  _CAN4TXDSR3.Bits.DB5
#define CAN4TXDSR3_DB6                  _CAN4TXDSR3.Bits.DB6
#define CAN4TXDSR3_DB7                  _CAN4TXDSR3.Bits.DB7

#define CAN4TXDSR3_DB0_MASK             1U
#define CAN4TXDSR3_DB1_MASK             2U
#define CAN4TXDSR3_DB2_MASK             4U
#define CAN4TXDSR3_DB3_MASK             8U
#define CAN4TXDSR3_DB4_MASK             16U
#define CAN4TXDSR3_DB5_MASK             32U
#define CAN4TXDSR3_DB6_MASK             64U
#define CAN4TXDSR3_DB7_MASK             128U


/*** CAN4TXDSR4 - MSCAN4 Transmit Data Segment Register 4; 0x000002B8 ***/
typedef union {
  byte Byte;
  struct {
    byte DB0         :1;                                       /* Data Bit 0 */
    byte DB1         :1;                                       /* Data Bit 1 */
    byte DB2         :1;                                       /* Data Bit 2 */
    byte DB3         :1;                                       /* Data Bit 3 */
    byte DB4         :1;                                       /* Data Bit 4 */
    byte DB5         :1;                                       /* Data Bit 5 */
    byte DB6         :1;                                       /* Data Bit 6 */
    byte DB7         :1;                                       /* Data Bit 7 */
  } Bits;
} CAN4TXDSR4STR;
extern volatile CAN4TXDSR4STR _CAN4TXDSR4 @(REG_BASE + 0x000002B8UL);
#define CAN4TXDSR4                      _CAN4TXDSR4.Byte
#define CAN4TXDSR4_DB0                  _CAN4TXDSR4.Bits.DB0
#define CAN4TXDSR4_DB1                  _CAN4TXDSR4.Bits.DB1
#define CAN4TXDSR4_DB2                  _CAN4TXDSR4.Bits.DB2
#define CAN4TXDSR4_DB3                  _CAN4TXDSR4.Bits.DB3
#define CAN4TXDSR4_DB4                  _CAN4TXDSR4.Bits.DB4
#define CAN4TXDSR4_DB5                  _CAN4TXDSR4.Bits.DB5
#define CAN4TXDSR4_DB6                  _CAN4TXDSR4.Bits.DB6
#define CAN4TXDSR4_DB7                  _CAN4TXDSR4.Bits.DB7

#define CAN4TXDSR4_DB0_MASK             1U
#define CAN4TXDSR4_DB1_MASK             2U
#define CAN4TXDSR4_DB2_MASK             4U
#define CAN4TXDSR4_DB3_MASK             8U
#define CAN4TXDSR4_DB4_MASK             16U
#define CAN4TXDSR4_DB5_MASK             32U
#define CAN4TXDSR4_DB6_MASK             64U
#define CAN4TXDSR4_DB7_MASK             128U


/*** CAN4TXDSR5 - MSCAN4 Transmit Data Segment Register 5; 0x000002B9 ***/
typedef union {
  byte Byte;
  struct {
    byte DB0         :1;                                       /* Data Bit 0 */
    byte DB1         :1;                                       /* Data Bit 1 */
    byte DB2         :1;                                       /* Data Bit 2 */
    byte DB3         :1;                                       /* Data Bit 3 */
    byte DB4         :1;                                       /* Data Bit 4 */
    byte DB5         :1;                                       /* Data Bit 5 */
    byte DB6         :1;                                       /* Data Bit 6 */
    byte DB7         :1;                                       /* Data Bit 7 */
  } Bits;
} CAN4TXDSR5STR;
extern volatile CAN4TXDSR5STR _CAN4TXDSR5 @(REG_BASE + 0x000002B9UL);
#define CAN4TXDSR5                      _CAN4TXDSR5.Byte
#define CAN4TXDSR5_DB0                  _CAN4TXDSR5.Bits.DB0
#define CAN4TXDSR5_DB1                  _CAN4TXDSR5.Bits.DB1
#define CAN4TXDSR5_DB2                  _CAN4TXDSR5.Bits.DB2
#define CAN4TXDSR5_DB3                  _CAN4TXDSR5.Bits.DB3
#define CAN4TXDSR5_DB4                  _CAN4TXDSR5.Bits.DB4
#define CAN4TXDSR5_DB5                  _CAN4TXDSR5.Bits.DB5
#define CAN4TXDSR5_DB6                  _CAN4TXDSR5.Bits.DB6
#define CAN4TXDSR5_DB7                  _CAN4TXDSR5.Bits.DB7

#define CAN4TXDSR5_DB0_MASK             1U
#define CAN4TXDSR5_DB1_MASK             2U
#define CAN4TXDSR5_DB2_MASK             4U
#define CAN4TXDSR5_DB3_MASK             8U
#define CAN4TXDSR5_DB4_MASK             16U
#define CAN4TXDSR5_DB5_MASK             32U
#define CAN4TXDSR5_DB6_MASK             64U
#define CAN4TXDSR5_DB7_MASK             128U


/*** CAN4TXDSR6 - MSCAN4 Transmit Data Segment Register 6; 0x000002BA ***/
typedef union {
  byte Byte;
  struct {
    byte DB0         :1;                                       /* Data Bit 0 */
    byte DB1         :1;                                       /* Data Bit 1 */
    byte DB2         :1;                                       /* Data Bit 2 */
    byte DB3         :1;                                       /* Data Bit 3 */
    byte DB4         :1;                                       /* Data Bit 4 */
    byte DB5         :1;                                       /* Data Bit 5 */
    byte DB6         :1;                                       /* Data Bit 6 */
    byte DB7         :1;                                       /* Data Bit 7 */
  } Bits;
} CAN4TXDSR6STR;
extern volatile CAN4TXDSR6STR _CAN4TXDSR6 @(REG_BASE + 0x000002BAUL);
#define CAN4TXDSR6                      _CAN4TXDSR6.Byte
#define CAN4TXDSR6_DB0                  _CAN4TXDSR6.Bits.DB0
#define CAN4TXDSR6_DB1                  _CAN4TXDSR6.Bits.DB1
#define CAN4TXDSR6_DB2                  _CAN4TXDSR6.Bits.DB2
#define CAN4TXDSR6_DB3                  _CAN4TXDSR6.Bits.DB3
#define CAN4TXDSR6_DB4                  _CAN4TXDSR6.Bits.DB4
#define CAN4TXDSR6_DB5                  _CAN4TXDSR6.Bits.DB5
#define CAN4TXDSR6_DB6                  _CAN4TXDSR6.Bits.DB6
#define CAN4TXDSR6_DB7                  _CAN4TXDSR6.Bits.DB7

#define CAN4TXDSR6_DB0_MASK             1U
#define CAN4TXDSR6_DB1_MASK             2U
#define CAN4TXDSR6_DB2_MASK             4U
#define CAN4TXDSR6_DB3_MASK             8U
#define CAN4TXDSR6_DB4_MASK             16U
#define CAN4TXDSR6_DB5_MASK             32U
#define CAN4TXDSR6_DB6_MASK             64U
#define CAN4TXDSR6_DB7_MASK             128U


/*** CAN4TXDSR7 - MSCAN4 Transmit Data Segment Register 7; 0x000002BB ***/
typedef union {
  byte Byte;
  struct {
    byte DB0         :1;                                       /* Data Bit 0 */
    byte DB1         :1;                                       /* Data Bit 1 */
    byte DB2         :1;                                       /* Data Bit 2 */
    byte DB3         :1;                                       /* Data Bit 3 */
    byte DB4         :1;                                       /* Data Bit 4 */
    byte DB5         :1;                                       /* Data Bit 5 */
    byte DB6         :1;                                       /* Data Bit 6 */
    byte DB7         :1;                                       /* Data Bit 7 */
  } Bits;
} CAN4TXDSR7STR;
extern volatile CAN4TXDSR7STR _CAN4TXDSR7 @(REG_BASE + 0x000002BBUL);
#define CAN4TXDSR7                      _CAN4TXDSR7.Byte
#define CAN4TXDSR7_DB0                  _CAN4TXDSR7.Bits.DB0
#define CAN4TXDSR7_DB1                  _CAN4TXDSR7.Bits.DB1
#define CAN4TXDSR7_DB2                  _CAN4TXDSR7.Bits.DB2
#define CAN4TXDSR7_DB3                  _CAN4TXDSR7.Bits.DB3
#define CAN4TXDSR7_DB4                  _CAN4TXDSR7.Bits.DB4
#define CAN4TXDSR7_DB5                  _CAN4TXDSR7.Bits.DB5
#define CAN4TXDSR7_DB6                  _CAN4TXDSR7.Bits.DB6
#define CAN4TXDSR7_DB7                  _CAN4TXDSR7.Bits.DB7

#define CAN4TXDSR7_DB0_MASK             1U
#define CAN4TXDSR7_DB1_MASK             2U
#define CAN4TXDSR7_DB2_MASK             4U
#define CAN4TXDSR7_DB3_MASK             8U
#define CAN4TXDSR7_DB4_MASK             16U
#define CAN4TXDSR7_DB5_MASK             32U
#define CAN4TXDSR7_DB6_MASK             64U
#define CAN4TXDSR7_DB7_MASK             128U


/*** CAN4TXDLR - MSCAN4 Transmit Data Length Register; 0x000002BC ***/
typedef union {
  byte Byte;
  struct {
    byte DLC0        :1;                                       /* Data Length Code Bit 0 */
    byte DLC1        :1;                                       /* Data Length Code Bit 1 */
    byte DLC2        :1;                                       /* Data Length Code Bit 2 */
    byte DLC3        :1;                                       /* Data Length Code Bit 3 */
    byte             :1; 
    byte             :1; 
    byte             :1; 
    byte             :1; 
  } Bits;
  struct {
    byte grpDLC  :4;
    byte         :1;
    byte         :1;
    byte         :1;
    byte         :1;
  } MergedBits;
} CAN4TXDLRSTR;
extern volatile CAN4TXDLRSTR _CAN4TXDLR @(REG_BASE + 0x000002BCUL);
#define CAN4TXDLR                       _CAN4TXDLR.Byte
#define CAN4TXDLR_DLC0                  _CAN4TXDLR.Bits.DLC0
#define CAN4TXDLR_DLC1                  _CAN4TXDLR.Bits.DLC1
#define CAN4TXDLR_DLC2                  _CAN4TXDLR.Bits.DLC2
#define CAN4TXDLR_DLC3                  _CAN4TXDLR.Bits.DLC3
#define CAN4TXDLR_DLC                   _CAN4TXDLR.MergedBits.grpDLC

#define CAN4TXDLR_DLC0_MASK             1U
#define CAN4TXDLR_DLC1_MASK             2U
#define CAN4TXDLR_DLC2_MASK             4U
#define CAN4TXDLR_DLC3_MASK             8U
#define CAN4TXDLR_DLC_MASK              15U
#define CAN4TXDLR_DLC_BITNUM            0U


/*** CAN4TXTBPR - MSCAN4 Transmit Transmit Buffer Priority; 0x000002BD ***/
typedef union {
  byte Byte;
  struct {
    byte PRIO0       :1;                                       /* Transmit Buffer Priority Bit 0 */
    byte PRIO1       :1;                                       /* Transmit Buffer Priority Bit 1 */
    byte PRIO2       :1;                                       /* Transmit Buffer Priority Bit 2 */
    byte PRIO3       :1;                                       /* Transmit Buffer Priority Bit 3 */
    byte PRIO4       :1;                                       /* Transmit Buffer Priority Bit 4 */
    byte PRIO5       :1;                                       /* Transmit Buffer Priority Bit 5 */
    byte PRIO6       :1;                                       /* Transmit Buffer Priority Bit 6 */
    byte PRIO7       :1;                                       /* Transmit Buffer Priority Bit 7 */
  } Bits;
} CAN4TXTBPRSTR;
extern volatile CAN4TXTBPRSTR _CAN4TXTBPR @(REG_BASE + 0x000002BDUL);
#define CAN4TXTBPR                      _CAN4TXTBPR.Byte
#define CAN4TXTBPR_PRIO0                _CAN4TXTBPR.Bits.PRIO0
#define CAN4TXTBPR_PRIO1                _CAN4TXTBPR.Bits.PRIO1
#define CAN4TXTBPR_PRIO2                _CAN4TXTBPR.Bits.PRIO2
#define CAN4TXTBPR_PRIO3                _CAN4TXTBPR.Bits.PRIO3
#define CAN4TXTBPR_PRIO4                _CAN4TXTBPR.Bits.PRIO4
#define CAN4TXTBPR_PRIO5                _CAN4TXTBPR.Bits.PRIO5
#define CAN4TXTBPR_PRIO6                _CAN4TXTBPR.Bits.PRIO6
#define CAN4TXTBPR_PRIO7                _CAN4TXTBPR.Bits.PRIO7

#define CAN4TXTBPR_PRIO0_MASK           1U
#define CAN4TXTBPR_PRIO1_MASK           2U
#define CAN4TXTBPR_PRIO2_MASK           4U
#define CAN4TXTBPR_PRIO3_MASK           8U
#define CAN4TXTBPR_PRIO4_MASK           16U
#define CAN4TXTBPR_PRIO5_MASK           32U
#define CAN4TXTBPR_PRIO6_MASK           64U
#define CAN4TXTBPR_PRIO7_MASK           128U


/*** CAN4TXTSR - MSCAN 4 Transmit Time Stamp Register; 0x000002BE ***/
typedef union {
  word Word;
   /* Overlapped registers: */
  struct {
    /*** CAN4TXTSRH - MSCAN 4 Transmit Time Stamp Register High; 0x000002BE ***/
    union {
      byte Byte;
      struct {
        byte TSR8        :1;                                       /* Time Stamp Bit 8 */
        byte TSR9        :1;                                       /* Time Stamp Bit 9 */
        byte TSR10       :1;                                       /* Time Stamp Bit 10 */
        byte TSR11       :1;                                       /* Time Stamp Bit 11 */
        byte TSR12       :1;                                       /* Time Stamp Bit 12 */
        byte TSR13       :1;                                       /* Time Stamp Bit 13 */
        byte TSR14       :1;                                       /* Time Stamp Bit 14 */
        byte TSR15       :1;                                       /* Time Stamp Bit 15 */
      } Bits;
    } CAN4TXTSRHSTR;
    #define CAN4TXTSRH                  _CAN4TXTSR.Overlap_STR.CAN4TXTSRHSTR.Byte
    #define CAN4TXTSRH_TSR8             _CAN4TXTSR.Overlap_STR.CAN4TXTSRHSTR.Bits.TSR8
    #define CAN4TXTSRH_TSR9             _CAN4TXTSR.Overlap_STR.CAN4TXTSRHSTR.Bits.TSR9
    #define CAN4TXTSRH_TSR10            _CAN4TXTSR.Overlap_STR.CAN4TXTSRHSTR.Bits.TSR10
    #define CAN4TXTSRH_TSR11            _CAN4TXTSR.Overlap_STR.CAN4TXTSRHSTR.Bits.TSR11
    #define CAN4TXTSRH_TSR12            _CAN4TXTSR.Overlap_STR.CAN4TXTSRHSTR.Bits.TSR12
    #define CAN4TXTSRH_TSR13            _CAN4TXTSR.Overlap_STR.CAN4TXTSRHSTR.Bits.TSR13
    #define CAN4TXTSRH_TSR14            _CAN4TXTSR.Overlap_STR.CAN4TXTSRHSTR.Bits.TSR14
    #define CAN4TXTSRH_TSR15            _CAN4TXTSR.Overlap_STR.CAN4TXTSRHSTR.Bits.TSR15
    
    #define CAN4TXTSRH_TSR8_MASK        1U
    #define CAN4TXTSRH_TSR9_MASK        2U
    #define CAN4TXTSRH_TSR10_MASK       4U
    #define CAN4TXTSRH_TSR11_MASK       8U
    #define CAN4TXTSRH_TSR12_MASK       16U
    #define CAN4TXTSRH_TSR13_MASK       32U
    #define CAN4TXTSRH_TSR14_MASK       64U
    #define CAN4TXTSRH_TSR15_MASK       128U
    

    /*** CAN4TXTSRL - MSCAN 4 Transmit Time Stamp Register Low; 0x000002BF ***/
    union {
      byte Byte;
      struct {
        byte TSR0        :1;                                       /* Time Stamp Bit 0 */
        byte TSR1        :1;                                       /* Time Stamp Bit 1 */
        byte TSR2        :1;                                       /* Time Stamp Bit 2 */
        byte TSR3        :1;                                       /* Time Stamp Bit 3 */
        byte TSR4        :1;                                       /* Time Stamp Bit 4 */
        byte TSR5        :1;                                       /* Time Stamp Bit 5 */
        byte TSR6        :1;                                       /* Time Stamp Bit 6 */
        byte TSR7        :1;                                       /* Time Stamp Bit 7 */
      } Bits;
    } CAN4TXTSRLSTR;
    #define CAN4TXTSRL                  _CAN4TXTSR.Overlap_STR.CAN4TXTSRLSTR.Byte
    #define CAN4TXTSRL_TSR0             _CAN4TXTSR.Overlap_STR.CAN4TXTSRLSTR.Bits.TSR0
    #define CAN4TXTSRL_TSR1             _CAN4TXTSR.Overlap_STR.CAN4TXTSRLSTR.Bits.TSR1
    #define CAN4TXTSRL_TSR2             _CAN4TXTSR.Overlap_STR.CAN4TXTSRLSTR.Bits.TSR2
    #define CAN4TXTSRL_TSR3             _CAN4TXTSR.Overlap_STR.CAN4TXTSRLSTR.Bits.TSR3
    #define CAN4TXTSRL_TSR4             _CAN4TXTSR.Overlap_STR.CAN4TXTSRLSTR.Bits.TSR4
    #define CAN4TXTSRL_TSR5             _CAN4TXTSR.Overlap_STR.CAN4TXTSRLSTR.Bits.TSR5
    #define CAN4TXTSRL_TSR6             _CAN4TXTSR.Overlap_STR.CAN4TXTSRLSTR.Bits.TSR6
    #define CAN4TXTSRL_TSR7             _CAN4TXTSR.Overlap_STR.CAN4TXTSRLSTR.Bits.TSR7
    
    #define CAN4TXTSRL_TSR0_MASK        1U
    #define CAN4TXTSRL_TSR1_MASK        2U
    #define CAN4TXTSRL_TSR2_MASK        4U
    #define CAN4TXTSRL_TSR3_MASK        8U
    #define CAN4TXTSRL_TSR4_MASK        16U
    #define CAN4TXTSRL_TSR5_MASK        32U
    #define CAN4TXTSRL_TSR6_MASK        64U
    #define CAN4TXTSRL_TSR7_MASK        128U
    
  } Overlap_STR;

  struct {
    word TSR0        :1;                                       /* Time Stamp Bit 0 */
    word TSR1        :1;                                       /* Time Stamp Bit 1 */
    word TSR2        :1;                                       /* Time Stamp Bit 2 */
    word TSR3        :1;                                       /* Time Stamp Bit 3 */
    word TSR4        :1;                                       /* Time Stamp Bit 4 */
    word TSR5        :1;                                       /* Time Stamp Bit 5 */
    word TSR6        :1;                                       /* Time Stamp Bit 6 */
    word TSR7        :1;                                       /* Time Stamp Bit 7 */
    word TSR8        :1;                                       /* Time Stamp Bit 8 */
    word TSR9        :1;                                       /* Time Stamp Bit 9 */
    word TSR10       :1;                                       /* Time Stamp Bit 10 */
    word TSR11       :1;                                       /* Time Stamp Bit 11 */
    word TSR12       :1;                                       /* Time Stamp Bit 12 */
    word TSR13       :1;                                       /* Time Stamp Bit 13 */
    word TSR14       :1;                                       /* Time Stamp Bit 14 */
    word TSR15       :1;                                       /* Time Stamp Bit 15 */
  } Bits;
} CAN4TXTSRSTR;
extern volatile CAN4TXTSRSTR _CAN4TXTSR @(REG_BASE + 0x000002BEUL);
#define CAN4TXTSR                       _CAN4TXTSR.Word
#define CAN4TXTSR_TSR0                  _CAN4TXTSR.Bits.TSR0
#define CAN4TXTSR_TSR1                  _CAN4TXTSR.Bits.TSR1
#define CAN4TXTSR_TSR2                  _CAN4TXTSR.Bits.TSR2
#define CAN4TXTSR_TSR3                  _CAN4TXTSR.Bits.TSR3
#define CAN4TXTSR_TSR4                  _CAN4TXTSR.Bits.TSR4
#define CAN4TXTSR_TSR5                  _CAN4TXTSR.Bits.TSR5
#define CAN4TXTSR_TSR6                  _CAN4TXTSR.Bits.TSR6
#define CAN4TXTSR_TSR7                  _CAN4TXTSR.Bits.TSR7
#define CAN4TXTSR_TSR8                  _CAN4TXTSR.Bits.TSR8
#define CAN4TXTSR_TSR9                  _CAN4TXTSR.Bits.TSR9
#define CAN4TXTSR_TSR10                 _CAN4TXTSR.Bits.TSR10
#define CAN4TXTSR_TSR11                 _CAN4TXTSR.Bits.TSR11
#define CAN4TXTSR_TSR12                 _CAN4TXTSR.Bits.TSR12
#define CAN4TXTSR_TSR13                 _CAN4TXTSR.Bits.TSR13
#define CAN4TXTSR_TSR14                 _CAN4TXTSR.Bits.TSR14
#define CAN4TXTSR_TSR15                 _CAN4TXTSR.Bits.TSR15

#define CAN4TXTSR_TSR0_MASK             1U
#define CAN4TXTSR_TSR1_MASK             2U
#define CAN4TXTSR_TSR2_MASK             4U
#define CAN4TXTSR_TSR3_MASK             8U
#define CAN4TXTSR_TSR4_MASK             16U
#define CAN4TXTSR_TSR5_MASK             32U
#define CAN4TXTSR_TSR6_MASK             64U
#define CAN4TXTSR_TSR7_MASK             128U
#define CAN4TXTSR_TSR8_MASK             256U
#define CAN4TXTSR_TSR9_MASK             512U
#define CAN4TXTSR_TSR10_MASK            1024U
#define CAN4TXTSR_TSR11_MASK            2048U
#define CAN4TXTSR_TSR12_MASK            4096U
#define CAN4TXTSR_TSR13_MASK            8192U
#define CAN4TXTSR_TSR14_MASK            16384U
#define CAN4TXTSR_TSR15_MASK            32768U


/*** BAKEY0 - Backdoor Access Key 0; 0x0000FF00 ***/
typedef union {
  word Word;
  struct {
    word KEY0        :1;                                       /* Backdoor Access Key bits, bit 0 */
    word KEY1        :1;                                       /* Backdoor Access Key bits, bit 1 */
    word KEY2        :1;                                       /* Backdoor Access Key bits, bit 2 */
    word KEY3        :1;                                       /* Backdoor Access Key bits, bit 3 */
    word KEY4        :1;                                       /* Backdoor Access Key bits, bit 4 */
    word KEY5        :1;                                       /* Backdoor Access Key bits, bit 5 */
    word KEY6        :1;                                       /* Backdoor Access Key bits, bit 6 */
    word KEY7        :1;                                       /* Backdoor Access Key bits, bit 7 */
    word KEY8        :1;                                       /* Backdoor Access Key bits, bit 8 */
    word KEY9        :1;                                       /* Backdoor Access Key bits, bit 9 */
    word KEY10       :1;                                       /* Backdoor Access Key bits, bit 10 */
    word KEY11       :1;                                       /* Backdoor Access Key bits, bit 11 */
    word KEY12       :1;                                       /* Backdoor Access Key bits, bit 12 */
    word KEY13       :1;                                       /* Backdoor Access Key bits, bit 13 */
    word KEY14       :1;                                       /* Backdoor Access Key bits, bit 14 */
    word KEY15       :1;                                       /* Backdoor Access Key bits, bit 15 */
  } Bits;
} BAKEY0STR;
/* Tip for register initialization in the user code:  const word BAKEY0_INIT @0x0000FF00 = <BAKEY0_INITVAL>; */
#define _BAKEY0 (*(const BAKEY0STR *)0x0000FF00)
#define BAKEY0                          _BAKEY0.Word
#define BAKEY0_KEY0                     _BAKEY0.Bits.KEY0
#define BAKEY0_KEY1                     _BAKEY0.Bits.KEY1
#define BAKEY0_KEY2                     _BAKEY0.Bits.KEY2
#define BAKEY0_KEY3                     _BAKEY0.Bits.KEY3
#define BAKEY0_KEY4                     _BAKEY0.Bits.KEY4
#define BAKEY0_KEY5                     _BAKEY0.Bits.KEY5
#define BAKEY0_KEY6                     _BAKEY0.Bits.KEY6
#define BAKEY0_KEY7                     _BAKEY0.Bits.KEY7
#define BAKEY0_KEY8                     _BAKEY0.Bits.KEY8
#define BAKEY0_KEY9                     _BAKEY0.Bits.KEY9
#define BAKEY0_KEY10                    _BAKEY0.Bits.KEY10
#define BAKEY0_KEY11                    _BAKEY0.Bits.KEY11
#define BAKEY0_KEY12                    _BAKEY0.Bits.KEY12
#define BAKEY0_KEY13                    _BAKEY0.Bits.KEY13
#define BAKEY0_KEY14                    _BAKEY0.Bits.KEY14
#define BAKEY0_KEY15                    _BAKEY0.Bits.KEY15
/* BAKEY_ARR: Access 4 BAKEYx registers in an array */
#define BAKEY_ARR                       ((volatile word *) &BAKEY0)

#define BAKEY0_KEY0_MASK                1U
#define BAKEY0_KEY1_MASK                2U
#define BAKEY0_KEY2_MASK                4U
#define BAKEY0_KEY3_MASK                8U
#define BAKEY0_KEY4_MASK                16U
#define BAKEY0_KEY5_MASK                32U
#define BAKEY0_KEY6_MASK                64U
#define BAKEY0_KEY7_MASK                128U
#define BAKEY0_KEY8_MASK                256U
#define BAKEY0_KEY9_MASK                512U
#define BAKEY0_KEY10_MASK               1024U
#define BAKEY0_KEY11_MASK               2048U
#define BAKEY0_KEY12_MASK               4096U
#define BAKEY0_KEY13_MASK               8192U
#define BAKEY0_KEY14_MASK               16384U
#define BAKEY0_KEY15_MASK               32768U


/*** BAKEY1 - Backdoor Access Key 1; 0x0000FF02 ***/
typedef union {
  word Word;
  struct {
    word KEY0        :1;                                       /* Backdoor Access Key bits, bit 0 */
    word KEY1        :1;                                       /* Backdoor Access Key bits, bit 1 */
    word KEY2        :1;                                       /* Backdoor Access Key bits, bit 2 */
    word KEY3        :1;                                       /* Backdoor Access Key bits, bit 3 */
    word KEY4        :1;                                       /* Backdoor Access Key bits, bit 4 */
    word KEY5        :1;                                       /* Backdoor Access Key bits, bit 5 */
    word KEY6        :1;                                       /* Backdoor Access Key bits, bit 6 */
    word KEY7        :1;                                       /* Backdoor Access Key bits, bit 7 */
    word KEY8        :1;                                       /* Backdoor Access Key bits, bit 8 */
    word KEY9        :1;                                       /* Backdoor Access Key bits, bit 9 */
    word KEY10       :1;                                       /* Backdoor Access Key bits, bit 10 */
    word KEY11       :1;                                       /* Backdoor Access Key bits, bit 11 */
    word KEY12       :1;                                       /* Backdoor Access Key bits, bit 12 */
    word KEY13       :1;                                       /* Backdoor Access Key bits, bit 13 */
    word KEY14       :1;                                       /* Backdoor Access Key bits, bit 14 */
    word KEY15       :1;                                       /* Backdoor Access Key bits, bit 15 */
  } Bits;
} BAKEY1STR;
/* Tip for register initialization in the user code:  const word BAKEY1_INIT @0x0000FF02 = <BAKEY1_INITVAL>; */
#define _BAKEY1 (*(const BAKEY1STR *)0x0000FF02)
#define BAKEY1                          _BAKEY1.Word
#define BAKEY1_KEY0                     _BAKEY1.Bits.KEY0
#define BAKEY1_KEY1                     _BAKEY1.Bits.KEY1
#define BAKEY1_KEY2                     _BAKEY1.Bits.KEY2
#define BAKEY1_KEY3                     _BAKEY1.Bits.KEY3
#define BAKEY1_KEY4                     _BAKEY1.Bits.KEY4
#define BAKEY1_KEY5                     _BAKEY1.Bits.KEY5
#define BAKEY1_KEY6                     _BAKEY1.Bits.KEY6
#define BAKEY1_KEY7                     _BAKEY1.Bits.KEY7
#define BAKEY1_KEY8                     _BAKEY1.Bits.KEY8
#define BAKEY1_KEY9                     _BAKEY1.Bits.KEY9
#define BAKEY1_KEY10                    _BAKEY1.Bits.KEY10
#define BAKEY1_KEY11                    _BAKEY1.Bits.KEY11
#define BAKEY1_KEY12                    _BAKEY1.Bits.KEY12
#define BAKEY1_KEY13                    _BAKEY1.Bits.KEY13
#define BAKEY1_KEY14                    _BAKEY1.Bits.KEY14
#define BAKEY1_KEY15                    _BAKEY1.Bits.KEY15

#define BAKEY1_KEY0_MASK                1U
#define BAKEY1_KEY1_MASK                2U
#define BAKEY1_KEY2_MASK                4U
#define BAKEY1_KEY3_MASK                8U
#define BAKEY1_KEY4_MASK                16U
#define BAKEY1_KEY5_MASK                32U
#define BAKEY1_KEY6_MASK                64U
#define BAKEY1_KEY7_MASK                128U
#define BAKEY1_KEY8_MASK                256U
#define BAKEY1_KEY9_MASK                512U
#define BAKEY1_KEY10_MASK               1024U
#define BAKEY1_KEY11_MASK               2048U
#define BAKEY1_KEY12_MASK               4096U
#define BAKEY1_KEY13_MASK               8192U
#define BAKEY1_KEY14_MASK               16384U
#define BAKEY1_KEY15_MASK               32768U


/*** BAKEY2 - Backdoor Access Key 2; 0x0000FF04 ***/
typedef union {
  word Word;
  struct {
    word KEY0        :1;                                       /* Backdoor Access Key bits, bit 0 */
    word KEY1        :1;                                       /* Backdoor Access Key bits, bit 1 */
    word KEY2        :1;                                       /* Backdoor Access Key bits, bit 2 */
    word KEY3        :1;                                       /* Backdoor Access Key bits, bit 3 */
    word KEY4        :1;                                       /* Backdoor Access Key bits, bit 4 */
    word KEY5        :1;                                       /* Backdoor Access Key bits, bit 5 */
    word KEY6        :1;                                       /* Backdoor Access Key bits, bit 6 */
    word KEY7        :1;                                       /* Backdoor Access Key bits, bit 7 */
    word KEY8        :1;                                       /* Backdoor Access Key bits, bit 8 */
    word KEY9        :1;                                       /* Backdoor Access Key bits, bit 9 */
    word KEY10       :1;                                       /* Backdoor Access Key bits, bit 10 */
    word KEY11       :1;                                       /* Backdoor Access Key bits, bit 11 */
    word KEY12       :1;                                       /* Backdoor Access Key bits, bit 12 */
    word KEY13       :1;                                       /* Backdoor Access Key bits, bit 13 */
    word KEY14       :1;                                       /* Backdoor Access Key bits, bit 14 */
    word KEY15       :1;                                       /* Backdoor Access Key bits, bit 15 */
  } Bits;
} BAKEY2STR;
/* Tip for register initialization in the user code:  const word BAKEY2_INIT @0x0000FF04 = <BAKEY2_INITVAL>; */
#define _BAKEY2 (*(const BAKEY2STR *)0x0000FF04)
#define BAKEY2                          _BAKEY2.Word
#define BAKEY2_KEY0                     _BAKEY2.Bits.KEY0
#define BAKEY2_KEY1                     _BAKEY2.Bits.KEY1
#define BAKEY2_KEY2                     _BAKEY2.Bits.KEY2
#define BAKEY2_KEY3                     _BAKEY2.Bits.KEY3
#define BAKEY2_KEY4                     _BAKEY2.Bits.KEY4
#define BAKEY2_KEY5                     _BAKEY2.Bits.KEY5
#define BAKEY2_KEY6                     _BAKEY2.Bits.KEY6
#define BAKEY2_KEY7                     _BAKEY2.Bits.KEY7
#define BAKEY2_KEY8                     _BAKEY2.Bits.KEY8
#define BAKEY2_KEY9                     _BAKEY2.Bits.KEY9
#define BAKEY2_KEY10                    _BAKEY2.Bits.KEY10
#define BAKEY2_KEY11                    _BAKEY2.Bits.KEY11
#define BAKEY2_KEY12                    _BAKEY2.Bits.KEY12
#define BAKEY2_KEY13                    _BAKEY2.Bits.KEY13
#define BAKEY2_KEY14                    _BAKEY2.Bits.KEY14
#define BAKEY2_KEY15                    _BAKEY2.Bits.KEY15

#define BAKEY2_KEY0_MASK                1U
#define BAKEY2_KEY1_MASK                2U
#define BAKEY2_KEY2_MASK                4U
#define BAKEY2_KEY3_MASK                8U
#define BAKEY2_KEY4_MASK                16U
#define BAKEY2_KEY5_MASK                32U
#define BAKEY2_KEY6_MASK                64U
#define BAKEY2_KEY7_MASK                128U
#define BAKEY2_KEY8_MASK                256U
#define BAKEY2_KEY9_MASK                512U
#define BAKEY2_KEY10_MASK               1024U
#define BAKEY2_KEY11_MASK               2048U
#define BAKEY2_KEY12_MASK               4096U
#define BAKEY2_KEY13_MASK               8192U
#define BAKEY2_KEY14_MASK               16384U
#define BAKEY2_KEY15_MASK               32768U


/*** BAKEY3 - Backdoor Access Key 3; 0x0000FF06 ***/
typedef union {
  word Word;
  struct {
    word KEY0        :1;                                       /* Backdoor Access Key bits, bit 0 */
    word KEY1        :1;                                       /* Backdoor Access Key bits, bit 1 */
    word KEY2        :1;                                       /* Backdoor Access Key bits, bit 2 */
    word KEY3        :1;                                       /* Backdoor Access Key bits, bit 3 */
    word KEY4        :1;                                       /* Backdoor Access Key bits, bit 4 */
    word KEY5        :1;                                       /* Backdoor Access Key bits, bit 5 */
    word KEY6        :1;                                       /* Backdoor Access Key bits, bit 6 */
    word KEY7        :1;                                       /* Backdoor Access Key bits, bit 7 */
    word KEY8        :1;                                       /* Backdoor Access Key bits, bit 8 */
    word KEY9        :1;                                       /* Backdoor Access Key bits, bit 9 */
    word KEY10       :1;                                       /* Backdoor Access Key bits, bit 10 */
    word KEY11       :1;                                       /* Backdoor Access Key bits, bit 11 */
    word KEY12       :1;                                       /* Backdoor Access Key bits, bit 12 */
    word KEY13       :1;                                       /* Backdoor Access Key bits, bit 13 */
    word KEY14       :1;                                       /* Backdoor Access Key bits, bit 14 */
    word KEY15       :1;                                       /* Backdoor Access Key bits, bit 15 */
  } Bits;
} BAKEY3STR;
/* Tip for register initialization in the user code:  const word BAKEY3_INIT @0x0000FF06 = <BAKEY3_INITVAL>; */
#define _BAKEY3 (*(const BAKEY3STR *)0x0000FF06)
#define BAKEY3                          _BAKEY3.Word
#define BAKEY3_KEY0                     _BAKEY3.Bits.KEY0
#define BAKEY3_KEY1                     _BAKEY3.Bits.KEY1
#define BAKEY3_KEY2                     _BAKEY3.Bits.KEY2
#define BAKEY3_KEY3                     _BAKEY3.Bits.KEY3
#define BAKEY3_KEY4                     _BAKEY3.Bits.KEY4
#define BAKEY3_KEY5                     _BAKEY3.Bits.KEY5
#define BAKEY3_KEY6                     _BAKEY3.Bits.KEY6
#define BAKEY3_KEY7                     _BAKEY3.Bits.KEY7
#define BAKEY3_KEY8                     _BAKEY3.Bits.KEY8
#define BAKEY3_KEY9                     _BAKEY3.Bits.KEY9
#define BAKEY3_KEY10                    _BAKEY3.Bits.KEY10
#define BAKEY3_KEY11                    _BAKEY3.Bits.KEY11
#define BAKEY3_KEY12                    _BAKEY3.Bits.KEY12
#define BAKEY3_KEY13                    _BAKEY3.Bits.KEY13
#define BAKEY3_KEY14                    _BAKEY3.Bits.KEY14
#define BAKEY3_KEY15                    _BAKEY3.Bits.KEY15

#define BAKEY3_KEY0_MASK                1U
#define BAKEY3_KEY1_MASK                2U
#define BAKEY3_KEY2_MASK                4U
#define BAKEY3_KEY3_MASK                8U
#define BAKEY3_KEY4_MASK                16U
#define BAKEY3_KEY5_MASK                32U
#define BAKEY3_KEY6_MASK                64U
#define BAKEY3_KEY7_MASK                128U
#define BAKEY3_KEY8_MASK                256U
#define BAKEY3_KEY9_MASK                512U
#define BAKEY3_KEY10_MASK               1024U
#define BAKEY3_KEY11_MASK               2048U
#define BAKEY3_KEY12_MASK               4096U
#define BAKEY3_KEY13_MASK               8192U
#define BAKEY3_KEY14_MASK               16384U
#define BAKEY3_KEY15_MASK               32768U


/*** NVFPROT3 - Non volatile Block 3 Flash Protection Register; 0x0000FF0A ***/
typedef union {
  byte Byte;
  struct {
    byte FPLS0       :1;                                       /* Flash Protection Lower Address size 0 */
    byte FPLS1       :1;                                       /* Flash Protection Lower Address size 1 */
    byte FPLDIS      :1;                                       /* Flash Protection Lower address range disable */
    byte FPHS0       :1;                                       /* Flash Protection Higher address size 0 */
    byte FPHS1       :1;                                       /* Flash Protection Higher address size 1 */
    byte FPHDIS      :1;                                       /* Flash Protection Higher address range disable */
    byte NV6         :1;                                       /* Non Volatile Flag Bit */
    byte FPOPEN      :1;                                       /* Opens the flash block or subsections of it for program or erase */
  } Bits;
  struct {
    byte grpFPLS :2;
    byte         :1;
    byte grpFPHS :2;
    byte         :1;
    byte grpNV_6 :1;
    byte         :1;
  } MergedBits;
} NVFPROT3STR;
/* Tip for register initialization in the user code:  const byte NVFPROT3_INIT @0x0000FF0A = <NVFPROT3_INITVAL>; */
#define _NVFPROT3 (*(const NVFPROT3STR *)0x0000FF0A)
#define NVFPROT3                        _NVFPROT3.Byte
#define NVFPROT3_FPLS0                  _NVFPROT3.Bits.FPLS0
#define NVFPROT3_FPLS1                  _NVFPROT3.Bits.FPLS1
#define NVFPROT3_FPLDIS                 _NVFPROT3.Bits.FPLDIS
#define NVFPROT3_FPHS0                  _NVFPROT3.Bits.FPHS0
#define NVFPROT3_FPHS1                  _NVFPROT3.Bits.FPHS1
#define NVFPROT3_FPHDIS                 _NVFPROT3.Bits.FPHDIS
#define NVFPROT3_NV6                    _NVFPROT3.Bits.NV6
#define NVFPROT3_FPOPEN                 _NVFPROT3.Bits.FPOPEN
#define NVFPROT3_FPLS                   _NVFPROT3.MergedBits.grpFPLS
#define NVFPROT3_FPHS                   _NVFPROT3.MergedBits.grpFPHS

#define NVFPROT3_FPLS0_MASK             1U
#define NVFPROT3_FPLS1_MASK             2U
#define NVFPROT3_FPLDIS_MASK            4U
#define NVFPROT3_FPHS0_MASK             8U
#define NVFPROT3_FPHS1_MASK             16U
#define NVFPROT3_FPHDIS_MASK            32U
#define NVFPROT3_NV6_MASK               64U
#define NVFPROT3_FPOPEN_MASK            128U
#define NVFPROT3_FPLS_MASK              3U
#define NVFPROT3_FPLS_BITNUM            0U
#define NVFPROT3_FPHS_MASK              24U
#define NVFPROT3_FPHS_BITNUM            3U


/*** NVFPROT2 - Non volatile Block 2 Flash Protection Register; 0x0000FF0B ***/
typedef union {
  byte Byte;
  struct {
    byte FPLS0       :1;                                       /* Flash Protection Lower Address size 0 */
    byte FPLS1       :1;                                       /* Flash Protection Lower Address size 1 */
    byte FPLDIS      :1;                                       /* Flash Protection Lower address range disable */
    byte FPHS0       :1;                                       /* Flash Protection Higher address size 0 */
    byte FPHS1       :1;                                       /* Flash Protection Higher address size 1 */
    byte FPHDIS      :1;                                       /* Flash Protection Higher address range disable */
    byte NV6         :1;                                       /* Non Volatile Flag Bit */
    byte FPOPEN      :1;                                       /* Opens the flash block or subsections of it for program or erase */
  } Bits;
  struct {
    byte grpFPLS :2;
    byte         :1;
    byte grpFPHS :2;
    byte         :1;
    byte grpNV_6 :1;
    byte         :1;
  } MergedBits;
} NVFPROT2STR;
/* Tip for register initialization in the user code:  const byte NVFPROT2_INIT @0x0000FF0B = <NVFPROT2_INITVAL>; */
#define _NVFPROT2 (*(const NVFPROT2STR *)0x0000FF0B)
#define NVFPROT2                        _NVFPROT2.Byte
#define NVFPROT2_FPLS0                  _NVFPROT2.Bits.FPLS0
#define NVFPROT2_FPLS1                  _NVFPROT2.Bits.FPLS1
#define NVFPROT2_FPLDIS                 _NVFPROT2.Bits.FPLDIS
#define NVFPROT2_FPHS0                  _NVFPROT2.Bits.FPHS0
#define NVFPROT2_FPHS1                  _NVFPROT2.Bits.FPHS1
#define NVFPROT2_FPHDIS                 _NVFPROT2.Bits.FPHDIS
#define NVFPROT2_NV6                    _NVFPROT2.Bits.NV6
#define NVFPROT2_FPOPEN                 _NVFPROT2.Bits.FPOPEN
#define NVFPROT2_FPLS                   _NVFPROT2.MergedBits.grpFPLS
#define NVFPROT2_FPHS                   _NVFPROT2.MergedBits.grpFPHS

#define NVFPROT2_FPLS0_MASK             1U
#define NVFPROT2_FPLS1_MASK             2U
#define NVFPROT2_FPLDIS_MASK            4U
#define NVFPROT2_FPHS0_MASK             8U
#define NVFPROT2_FPHS1_MASK             16U
#define NVFPROT2_FPHDIS_MASK            32U
#define NVFPROT2_NV6_MASK               64U
#define NVFPROT2_FPOPEN_MASK            128U
#define NVFPROT2_FPLS_MASK              3U
#define NVFPROT2_FPLS_BITNUM            0U
#define NVFPROT2_FPHS_MASK              24U
#define NVFPROT2_FPHS_BITNUM            3U


/*** NVFPROT1 - Non volatile Block 1 Flash Protection Register; 0x0000FF0C ***/
typedef union {
  byte Byte;
  struct {
    byte FPLS0       :1;                                       /* Flash Protection Lower Address size 0 */
    byte FPLS1       :1;                                       /* Flash Protection Lower Address size 1 */
    byte FPLDIS      :1;                                       /* Flash Protection Lower address range disable */
    byte FPHS0       :1;                                       /* Flash Protection Higher address size 0 */
    byte FPHS1       :1;                                       /* Flash Protection Higher address size 1 */
    byte FPHDIS      :1;                                       /* Flash Protection Higher address range disable */
    byte NV6         :1;                                       /* Non Volatile Flag Bit */
    byte FPOPEN      :1;                                       /* Opens the flash block or subsections of it for program or erase */
  } Bits;
  struct {
    byte grpFPLS :2;
    byte         :1;
    byte grpFPHS :2;
    byte         :1;
    byte grpNV_6 :1;
    byte         :1;
  } MergedBits;
} NVFPROT1STR;
/* Tip for register initialization in the user code:  const byte NVFPROT1_INIT @0x0000FF0C = <NVFPROT1_INITVAL>; */
#define _NVFPROT1 (*(const NVFPROT1STR *)0x0000FF0C)
#define NVFPROT1                        _NVFPROT1.Byte
#define NVFPROT1_FPLS0                  _NVFPROT1.Bits.FPLS0
#define NVFPROT1_FPLS1                  _NVFPROT1.Bits.FPLS1
#define NVFPROT1_FPLDIS                 _NVFPROT1.Bits.FPLDIS
#define NVFPROT1_FPHS0                  _NVFPROT1.Bits.FPHS0
#define NVFPROT1_FPHS1                  _NVFPROT1.Bits.FPHS1
#define NVFPROT1_FPHDIS                 _NVFPROT1.Bits.FPHDIS
#define NVFPROT1_NV6                    _NVFPROT1.Bits.NV6
#define NVFPROT1_FPOPEN                 _NVFPROT1.Bits.FPOPEN
#define NVFPROT1_FPLS                   _NVFPROT1.MergedBits.grpFPLS
#define NVFPROT1_FPHS                   _NVFPROT1.MergedBits.grpFPHS

#define NVFPROT1_FPLS0_MASK             1U
#define NVFPROT1_FPLS1_MASK             2U
#define NVFPROT1_FPLDIS_MASK            4U
#define NVFPROT1_FPHS0_MASK             8U
#define NVFPROT1_FPHS1_MASK             16U
#define NVFPROT1_FPHDIS_MASK            32U
#define NVFPROT1_NV6_MASK               64U
#define NVFPROT1_FPOPEN_MASK            128U
#define NVFPROT1_FPLS_MASK              3U
#define NVFPROT1_FPLS_BITNUM            0U
#define NVFPROT1_FPHS_MASK              24U
#define NVFPROT1_FPHS_BITNUM            3U


/*** NVFPROT0 - Non volatile Block 0 Flash Protection Register; 0x0000FF0D ***/
typedef union {
  byte Byte;
  struct {
    byte FPLS0       :1;                                       /* Flash Protection Lower Address size 0 */
    byte FPLS1       :1;                                       /* Flash Protection Lower Address size 1 */
    byte FPLDIS      :1;                                       /* Flash Protection Lower address range disable */
    byte FPHS0       :1;                                       /* Flash Protection Higher address size 0 */
    byte FPHS1       :1;                                       /* Flash Protection Higher address size 1 */
    byte FPHDIS      :1;                                       /* Flash Protection Higher address range disable */
    byte NV6         :1;                                       /* Non Volatile Flag Bit */
    byte FPOPEN      :1;                                       /* Opens the flash block or subsections of it for program or erase */
  } Bits;
  struct {
    byte grpFPLS :2;
    byte         :1;
    byte grpFPHS :2;
    byte         :1;
    byte grpNV_6 :1;
    byte         :1;
  } MergedBits;
} NVFPROT0STR;
/* Tip for register initialization in the user code:  const byte NVFPROT0_INIT @0x0000FF0D = <NVFPROT0_INITVAL>; */
#define _NVFPROT0 (*(const NVFPROT0STR *)0x0000FF0D)
#define NVFPROT0                        _NVFPROT0.Byte
#define NVFPROT0_FPLS0                  _NVFPROT0.Bits.FPLS0
#define NVFPROT0_FPLS1                  _NVFPROT0.Bits.FPLS1
#define NVFPROT0_FPLDIS                 _NVFPROT0.Bits.FPLDIS
#define NVFPROT0_FPHS0                  _NVFPROT0.Bits.FPHS0
#define NVFPROT0_FPHS1                  _NVFPROT0.Bits.FPHS1
#define NVFPROT0_FPHDIS                 _NVFPROT0.Bits.FPHDIS
#define NVFPROT0_NV6                    _NVFPROT0.Bits.NV6
#define NVFPROT0_FPOPEN                 _NVFPROT0.Bits.FPOPEN
#define NVFPROT0_FPLS                   _NVFPROT0.MergedBits.grpFPLS
#define NVFPROT0_FPHS                   _NVFPROT0.MergedBits.grpFPHS

#define NVFPROT0_FPLS0_MASK             1U
#define NVFPROT0_FPLS1_MASK             2U
#define NVFPROT0_FPLDIS_MASK            4U
#define NVFPROT0_FPHS0_MASK             8U
#define NVFPROT0_FPHS1_MASK             16U
#define NVFPROT0_FPHDIS_MASK            32U
#define NVFPROT0_NV6_MASK               64U
#define NVFPROT0_FPOPEN_MASK            128U
#define NVFPROT0_FPLS_MASK              3U
#define NVFPROT0_FPLS_BITNUM            0U
#define NVFPROT0_FPHS_MASK              24U
#define NVFPROT0_FPHS_BITNUM            3U


/*** NVFSEC - Non volatile Flash Security Register; 0x0000FF0F ***/
typedef union {
  byte Byte;
  struct {
    byte SEC0        :1;                                       /* Memory security bit 0 */
    byte SEC1        :1;                                       /* Memory security bit 1 */
    byte NV2         :1;                                       /* Non Volatile flag bit 2 */
    byte NV3         :1;                                       /* Non Volatile flag bit 3 */
    byte NV4         :1;                                       /* Non Volatile flag bit 4 */
    byte NV5         :1;                                       /* Non Volatile flag bit 5 */
    byte KEYEN0      :1;                                       /* Backdoor Key Security Enable Bit 0 */
    byte KEYEN1      :1;                                       /* Backdoor Key Security Enable Bit 1 */
  } Bits;
  struct {
    byte grpSEC  :2;
    byte grpNV_2 :4;
    byte grpKEYEN :2;
  } MergedBits;
} NVFSECSTR;
/* Tip for register initialization in the user code:  const byte NVFSEC_INIT @0x0000FF0F = <NVFSEC_INITVAL>; */
#define _NVFSEC (*(const NVFSECSTR *)0x0000FF0F)
#define NVFSEC                          _NVFSEC.Byte
#define NVFSEC_SEC0                     _NVFSEC.Bits.SEC0
#define NVFSEC_SEC1                     _NVFSEC.Bits.SEC1
#define NVFSEC_NV2                      _NVFSEC.Bits.NV2
#define NVFSEC_NV3                      _NVFSEC.Bits.NV3
#define NVFSEC_NV4                      _NVFSEC.Bits.NV4
#define NVFSEC_NV5                      _NVFSEC.Bits.NV5
#define NVFSEC_KEYEN0                   _NVFSEC.Bits.KEYEN0
#define NVFSEC_KEYEN1                   _NVFSEC.Bits.KEYEN1
#define NVFSEC_SEC                      _NVFSEC.MergedBits.grpSEC
#define NVFSEC_NV_2                     _NVFSEC.MergedBits.grpNV_2
#define NVFSEC_KEYEN                    _NVFSEC.MergedBits.grpKEYEN
#define NVFSEC_NV                       NVFSEC_NV_2

#define NVFSEC_SEC0_MASK                1U
#define NVFSEC_SEC1_MASK                2U
#define NVFSEC_NV2_MASK                 4U
#define NVFSEC_NV3_MASK                 8U
#define NVFSEC_NV4_MASK                 16U
#define NVFSEC_NV5_MASK                 32U
#define NVFSEC_KEYEN0_MASK              64U
#define NVFSEC_KEYEN1_MASK              128U
#define NVFSEC_SEC_MASK                 3U
#define NVFSEC_SEC_BITNUM               0U
#define NVFSEC_NV_2_MASK                60U
#define NVFSEC_NV_2_BITNUM              2U
#define NVFSEC_KEYEN_MASK               192U
#define NVFSEC_KEYEN_BITNUM             6U


  /* Watchdog reset macro */
#ifndef __RESET_WATCHDOG
#ifdef _lint
  #define __RESET_WATCHDOG()  /* empty */
#else
  #define __RESET_WATCHDOG() (void)(ARMCOP = 0x55U, ARMCOP = 0xAAU)
#endif
#endif /* __RESET_WATCHDOG */


/***********************************************/
/**   D E P R E C I A T E D   S Y M B O L S   **/
/***********************************************/
/* --------------------------------------------------------------------------- */
/* The following symbols were removed, because they were invalid or irrelevant */
/* --------------------------------------------------------------------------- */
#define MCCNTlo_BIT0                     This_symb_has_been_depreciated
#define MCCNTlo_BIT1                     This_symb_has_been_depreciated
#define MCCNTlo_BIT2                     This_symb_has_been_depreciated
#define MCCNTlo_BIT3                     This_symb_has_been_depreciated
#define MCCNTlo_BIT4                     This_symb_has_been_depreciated
#define MCCNTlo_BIT5                     This_symb_has_been_depreciated
#define MCCNTlo_BIT6                     This_symb_has_been_depreciated
#define MCCNTlo_BIT7                     This_symb_has_been_depreciated
#define MCCNThi_BIT8                     This_symb_has_been_depreciated
#define MCCNThi_BIT9                     This_symb_has_been_depreciated
#define MCCNThi_BIT10                    This_symb_has_been_depreciated
#define MCCNThi_BIT11                    This_symb_has_been_depreciated
#define MCCNThi_BIT12                    This_symb_has_been_depreciated
#define MCCNThi_BIT13                    This_symb_has_been_depreciated
#define MCCNThi_BIT14                    This_symb_has_been_depreciated
#define MCCNThi_BIT15                    This_symb_has_been_depreciated
#define MCCNTlo_BIT0_MASK                This_symb_has_been_depreciated
#define MCCNTlo_BIT1_MASK                This_symb_has_been_depreciated
#define MCCNTlo_BIT2_MASK                This_symb_has_been_depreciated
#define MCCNTlo_BIT3_MASK                This_symb_has_been_depreciated
#define MCCNTlo_BIT4_MASK                This_symb_has_been_depreciated
#define MCCNTlo_BIT5_MASK                This_symb_has_been_depreciated
#define MCCNTlo_BIT6_MASK                This_symb_has_been_depreciated
#define MCCNTlo_BIT7_MASK                This_symb_has_been_depreciated
#define MCCNThi_BIT8_MASK                This_symb_has_been_depreciated
#define MCCNThi_BIT9_MASK                This_symb_has_been_depreciated
#define MCCNThi_BIT10_MASK               This_symb_has_been_depreciated
#define MCCNThi_BIT11_MASK               This_symb_has_been_depreciated
#define MCCNThi_BIT12_MASK               This_symb_has_been_depreciated
#define MCCNThi_BIT13_MASK               This_symb_has_been_depreciated
#define MCCNThi_BIT14_MASK               This_symb_has_been_depreciated
#define MCCNThi_BIT15_MASK               This_symb_has_been_depreciated


#ifndef __V30COMPATIBLE__
#pragma OPTION DEL V30toV31Compatible
#endif
/*lint -restore  +esym(961,18.4) +esym(961,19.7) Enable MISRA rule (1.1,18.4,6.4,19.7) checking. */

#endif
