/*****************************************************
      start12.c - standard startup code
   The startup code may be optimized to special user requests
 ----------------------------------------------------
   Copyright (c) Metrowerks, Basel, Switzerland
               All rights reserved

Note: ROM libraries are not implemented in this startup code
Note: C++ destructors of global objects are NOT yet supported in the HIWARE Object File Format.
      To use this feature, please build your application with the ELF object file format.
 *****************************************************/
/* these macros remove some unused fields in the startup descriptor */
#define __NO_FLAGS_OFFSET       /* we do not need the flags field in the startup data descriptor */
#define __NO_MAIN_OFFSET        /* we do not need the main field in the startup data descriptor */
#define __NO_STACKOFFSET_OFFSET /* we do not need the stackOffset field in the startup data descriptor */

/*#define __BANKED_COPY_DOWN : allow to allocate .copy in flash area */
#if defined(__BANKED_COPY_DOWN) && (!defined(__HCS12X__) || !defined(__ELF_OBJECT_FILE_FORMAT__))
#error /* the __BANKED_COPY_DOWN switch is only supported for the HCS12X with ELF */
       /* (and not for the HC12, HCS12 or for the HIWARE object file format) */
#endif

#include "hidef.h"
#include "start12.h"

/***************************************************************************/
/* Macros to control how the startup code handles the COP:                 */
/* #define _DO_FEED_COP_  : do feed the COP                                */
/* #define _DO_ENABLE_COP_: do enable the COP                              */
/* #define _DO_DISABLE_COP_: disable the COP                               */
/* Without defining any of these, the startup code does NOT handle the COP */
/***************************************************************************/
/* __ONLY_INIT_SP define:                                                  */
/* This define selects an shorter version of the startup code              */
/* which only loads the stack pointer and directly afterwards calls        */
/* main. This version does however NOT initialized global variables        */
/* (So this version is not ANSI compliant!)                                */
/***************************************************************************/
/* __FAR_DATA define:                                                      */
/* By default, the startup code only supports to initialize the default    */
/* kind of memory. If some memory is allocated far in the small or banked  */
/* memory model, then the startup code only supports to initialize this    */
/* memory blocks if __FAR_DATA is defined. If __FAR_DATA is not defined,   */
/* then the linker will issue a message like                               */
/* "L1128: Cutting value _Range beg data member from 0xF01000 to 0x1000"   */
/* and this startup code writes to the cutted address                      */
/***************************************************************************/
/* __BANKED_COPY_DOWN define:                                              */
/* by default, the startup code assumes that the startup data structure    */
/* _startupData, the zero out areas and the .copy section are all          */
/* allocated in NON_BANKED memory. Especially the .copy section can be     */
/* huge if there are many or huge RAM areas to initialize.                 */  
/* For the HCS12X, which also copies the XGATE RAM located code via .copy  */
/* section, the startup code supports to allocate .copy in a banked flash  */
/* The placement of .copy in the prm file has to be adapted when adding or */
/* removing the this macro.                                                */
/* Note: This macro is only supported for the HCS12X and when using ELF    */
/***************************************************************************/

#ifdef __cplusplus
#define __EXTERN_C  extern "C"
#else
#define __EXTERN_C
#endif

/*lint -estring(961,"only preprocessor statements and comments before '#include'") , MISRA 19.1 ADV, non_bank.sgm and default.sgm each contain a conditionally compiled CODE_SEG pragma */

__EXTERN_C void main(void); /* prototype of main function */

#ifndef __ONLY_INIT_SP
#pragma DATA_SEG __NEAR_SEG STARTUP_DATA /* _startupData can be accessed using 16 bit accesses. */
/* This is needed because it contains the stack top, and without stack, far data cannot be accessed */
struct _tagStartup _startupData;  /*   read-only: */
                                  /*   _startupData is allocated in ROM and */
                                  /*   initialized by the linker */
#pragma DATA_SEG DEFAULT
#endif /* __ONLY_INIT_SP */

#if defined(FAR_DATA) && (!defined(__HCS12X__) || defined(__BANKED_COPY_DOWN))
/*lint -e451 non_bank.sgm contains a conditionally compiled CODE_SEG pragma */
#include "non_bank.sgm"
/*lint +e451 */

/* the init function must be in non banked memory if banked variables are used */
/* because _SET_PAGE is called, which may change any page register. */

/*lint -esym(752,_SET_PAGE) , symbol '_SET_PAGE' is referenced in HLI */
__EXTERN_C void _SET_PAGE(void);  /* the inline assembler needs a prototype */
                                  /* this is a runtime routine with a special */
                                  /* calling convention, do not use it in c code! */
#else
/*lint -e451 default.sgm contains a conditionally compiled CODE_SEG pragma */
#include "default.sgm"
/*lint +e451 */
#endif /* defined(FAR_DATA) && (!defined(__HCS12X__) || defined(__BANKED_COPY_DOWN)) */


/* define value and bits for Windef Register */
#ifdef HC812A4
#define WINDEF (*(volatile unsigned char*) 0x37)
#if defined( __BANKED__) || defined(__LARGE__) || defined(__PPAGE__)
#define __ENABLE_PPAGE__ 0x40
#else
#define __ENABLE_PPAGE__ 0x0
#endif
#if defined(__DPAGE__)
#define __ENABLE_DPAGE__ 0x80
#else
#define __ENABLE_DPAGE__ 0x0
#endif
#if defined(__EPAGE__)
#define __ENABLE_EPAGE__ 0x20
#else
#define __ENABLE_EPAGE__ 0x0
#endif
#endif  /* HC812A4 */

#ifdef _HCS12_SERIALMON
      /* for Monitor based software remap the RAM & EEPROM to adhere
         to EB386. Edit RAM and EEPROM sections in PRM file to match these. */
#define ___INITRM      (*(volatile unsigned char *) 0x0010)
#define ___INITRG      (*(volatile unsigned char *) 0x0011)
#define ___INITEE      (*(volatile unsigned char *) 0x0012)
#endif

#if defined(_DO_FEED_COP_)
#define __FEED_COP_IN_HLI()  } asm movb #0x55, _COP_RST_ADR; asm movb #0xAA, _COP_RST_ADR; asm {
#else
#define __FEED_COP_IN_HLI() /* do nothing */
#endif

#ifndef __ONLY_INIT_SP
#if (!defined(FAR_DATA) || defined(__HCS12X__)) && (defined( __BANKED__) || defined(__LARGE__) || defined(__BANKED_COPY_DOWN))
static void __far Init(void)
#else
static void Init(void)
#endif
 {
/* purpose:     1) zero out RAM-areas where data is allocated   */
/*              2) copy initialization data from ROM to RAM     */
/*              3) call global constructors in C++              */
/*   called from: _Startup, LibInits                            */
   asm {
ZeroOut:
#if defined(__HIWARE_OBJECT_FILE_FORMAT__) && defined(__LARGE__)
             LDX   _startupData.pZeroOut:1  ; in the large memory model in the HIWARE format, pZeroOut is a 24 bit pointer
#else
             LDX   _startupData.pZeroOut    ; *pZeroOut
#endif
             LDY   _startupData.nofZeroOuts ; nofZeroOuts
             BEQ   CopyDown                 ; if nothing to zero out

NextZeroOut: PSHY                           ; save nofZeroOuts
#if defined(FAR_DATA)
             LDAB  1,X+                     ; load page of destination address
             LDY   2,X+                     ; load offset of destination address
#if defined(__HCS12X__)
             STAB  __GPAGE_ADR__
#else /* defined(__HCS12X__) */
             __PIC_JSR(_SET_PAGE)           ; sets the page in the correct page register
#endif /* defined(__HCS12X__) */
#else   /* FAR_DATA */
             LDY   2,X+                     ; start address and advance *pZeroOut (X = X+4)
#endif  /* FAR_DATA */

#if defined(__HCS12X__) && defined(FAR_DATA)
             PSHX
             LDX   0,X                      ; byte count
#if defined(__OPTIMIZE_FOR_SIZE__)
             CLRA
NextWord:    GSTAA 1,Y+                     ; clear memory byte
             __FEED_COP_IN_HLI()            ; feed the COP if necessary /*lint !e505 !e522 asm code */
             DBNE  X, NextWord              ; dec byte count
#else
             LDD #0
             LSRX
             BEQ   LoopClrW1                ; do we copy more than 1 byte?
NextWord:    GSTD 2,Y+                      ; clear memory word
             __FEED_COP_IN_HLI()            ; feed the COP if necessary /*lint !e505 !e522 asm code */
             DBNE  X, NextWord              ; dec word count
LoopClrW1:
             BCC   LastClr                  ; handle last byte
             GSTAA 1,Y+											; handle last byte
LastClr:
#endif
             PULX
             LEAX  2,X
#elif defined(__OPTIMIZE_FOR_SIZE__)               /* -os, default */
             LDD   2,X+                     ; byte count
NextWord:    CLR   1,Y+                     ; clear memory byte
             __FEED_COP_IN_HLI()            ; feed the COP if necessary /*lint !e505 !e522 asm code */
             DBNE  D, NextWord              ; dec byte count
#else /* __OPTIMIZE_FOR_TIME__ */
             LDD   2,X+                     ; byte count
             LSRD                           ; /2 and save bit 0 in the carry
             BEQ   LoopClrW1                ; do we copy more than 1 byte?
             PSHX
             LDX   #0
LoopClrW:    STX   2,Y+                     ; Word-Clear
             __FEED_COP_IN_HLI()            ; feed the COP if necessary /*lint !e505 !e522 asm code */
             DBNE  D, LoopClrW
             PULX
LoopClrW1:
             BCC   LastClr                  ; handle last byte
             CLR   1,Y+
LastClr:
#endif /* __OPTIMIZE_FOR_SIZE__/__OPTIMIZE_FOR_TIME__ */
             PULY                           ; restore nofZeroOuts
             DEY                            ; dec nofZeroOuts
             BNE  NextZeroOut
CopyDown:
#if defined(__BANKED_COPY_DOWN)
             LDAA  _startupData.toCopyDownBeg:0 ; get PAGE address of .copy section
             STAA	 __PPAGE_ADR__          ; set PPAGE address
             LDX   _startupData.toCopyDownBeg:1 ; load address of copy down desc.
#elif defined(__ELF_OBJECT_FILE_FORMAT__)
             LDX   _startupData.toCopyDownBeg ; load address of copy down desc.
#else
             LDX   _startupData.toCopyDownBeg:2 ; load address of copy down desc.
#endif
NextBlock:
             LDD   2,X+                     ; size of init-data -> D
             BEQ   funcInits                ; end of copy down desc.
#ifdef FAR_DATA
             PSHD                           ; save counter
             LDAB  1,X+                     ; load destination page
             LDY   2,X+                     ; destination address
#if defined(__HCS12X__)
             STAB  __GPAGE_ADR__
#else  /* __HCS12X__ */
             __PIC_JSR(_SET_PAGE)           ; sets the destinations page register
#endif /* __HCS12X__ */
             PULD                           ; restore counter
#else  /* FAR_DATA */
             LDY   2,X+                     ; load destination address
#endif /* FAR_DATA */

#if defined(__HCS12X__) && defined(FAR_DATA)
#if defined(__OPTIMIZE_FOR_SIZE__)               /* -os, default */
Copy:        PSHA
             LDAA  1,X+
             GSTAA  1,Y+                    ; move a byte from ROM to the data area
             PULA
             __FEED_COP_IN_HLI()            ; feed the COP if necessary /*lint !e505 !e522 asm code */
             DBNE  D,Copy                   ; copy-byte loop
#else
             LSRD                           ; /2 and save bit 0 in the carry
             BEQ    Copy1                   ; do we copy more than 1 byte?

Copy:        PSHD
             LDD   2,X+
             GSTD  2,Y+                     ; move a word from ROM to the data area
             PULD
             __FEED_COP_IN_HLI()            ; feed the COP if necessary /*lint !e505 !e522 asm code */
             DBNE  D,Copy                   ; copy-word loop
Copy1:
             BCC   NextBlock                ; handle last byte?
             LDAA  1,X+
             GSTAA  1,Y+                    ; move a byte from ROM to the data area
#endif
#elif defined(__OPTIMIZE_FOR_SIZE__)               /* -os, default */
Copy:        MOVB  1,X+,1,Y+                ; move a byte from ROM to the data area
             __FEED_COP_IN_HLI()            ; feed the COP if necessary /*lint !e505 !e522 asm code */
             DBNE  D,Copy                   ; copy-byte loop
#else /* __OPTIMIZE_FOR_TIME__ */
             LSRD                           ; /2 and save bit 0 in the carry
             BEQ   Copy1                    ; do we copy more than 1 byte?
Copy:        MOVW  2,X+,2,Y+                ; move a word from ROM to the data area
             __FEED_COP_IN_HLI()            ; feed the COP if necessary /*lint !e505 !e522 asm code */
             DBNE  D,Copy                   ; copy-word loop
Copy1:
             BCC   NextBlock                ; handle last byte?
             MOVB  1,X+,1,Y+                ; copy the last byte
#endif /* __OPTIMIZE_FOR_SIZE__/__OPTIMIZE_FOR_TIME__ */
             BRA   NextBlock
funcInits:                                  ; call of global construtors is only in c++ necessary
#if defined(__cplusplus)
#if defined(__ELF_OBJECT_FILE_FORMAT__)
#if defined( __BANKED__) || defined(__LARGE__)
             LDY   _startupData.nofInitBodies; load number of cpp.
             BEQ   done                     ; if cppcount == 0, goto done
             LDX   _startupData.initBodies  ; load address of first module to initialize
nextInit:
             LEAX   3,X                     ; increment to next init
             PSHX                           ; save address of next function to initialize
             PSHY                           ; save cpp counter
             CALL  [-3,X]                   ; use double indirect call to load the page register also
             PULY                           ; restore cpp counter
             PULX                           ; restore actual address
             DEY                            ; decrement cpp counter
             BNE    nextInit
#else  /* defined( __BANKED__) || defined(__LARGE__) */

             LDD   _startupData.nofInitBodies; load number of cpp.
             BEQ   done                     ; if cppcount == 0, goto done
             LDX   _startupData.initBodies  ; load address of first module to initialize
nextInit:
             LDY   2,X+                     ; load address of first module to initialize
             PSHD
             PSHX                           ; save actual address
             JSR   0,Y                      ; call initialization function
             PULX                           ; restore actual address
             PULD                           ; restore cpp counter
             DBNE D, nextInit
#endif /* defined( __BANKED__) || defined(__LARGE__) */
#else /* __ELF_OBJECT_FILE_FORMAT__  */
             LDX   _startupData.mInits      ; load address of first module to initialize
#if defined( __BANKED__) || defined(__LARGE__)
nextInit:    LDY   3,X+                     ; load address of initialization function
             BEQ   done                     ; stop when address  == 0
                                            ; in common environments the offset of a function is never 0, so this test could be avoided
#ifdef __InitFunctionsMayHaveOffset0__
             BRCLR -1,X, done, 0xff         ; stop when address  == 0
#endif  /* __InitFunctionsMayHaveOffset0__ */
             PSHX                           ; save address of next function to initialize
             CALL  [-3,X]                   ; use double indirect call to load the page register also
#else  /* defined( __BANKED__) || defined(__LARGE__) */
nextInit:
             LDY   2,X+                     ; load address of first module to initialize
             BEQ   done                     ; stop when address of function == 0
             PSHX                           ; save actual address
             JSR   0,Y                      ; call initialization function
#endif /* defined( __BANKED__) || defined(__LARGE__) */
             PULX                           ; restore actual address
             BRA   nextInit
#endif  /* __ELF_OBJECT_FILE_FORMAT__  */
done:
#endif /* __cplusplus */
   }
}
#endif /* __ONLY_INIT_SP */

#if defined( __ELF_OBJECT_FILE_FORMAT__) && defined(__cplusplus ) && 0 /* the call to main does not support to return anymore */

#if !defined(FAR_DATA) && (defined( __BANKED__) || defined(__LARGE__))
static void __far Fini(void)
#else
static void Fini(void)
#endif
{
/* purpose:     1) call global destructors in C++ */
   asm {
#if defined( __BANKED__) || defined(__LARGE__)

             LDY   _startupData.nofFiniBodies; load number of cpp.
             BEQ   done                     ; if cppcount == 0, goto done
             LDX   _startupData.finiBodies  ; load address of first module to finalize
nextInit2:
             LEAX   3,X                     ; increment to next init
             PSHX                           ; save address of next function to finalize
             PSHY                           ; save cpp counter
             CALL  [-3,X]                   ; use double indirect call to load the page register also
             PULY                           ; restore cpp counter
             PULX                           ; restore actual address
             DEY                            ; decrement cpp counter
             BNE    nextInit2
#else  /* defined( __BANKED__) || defined(__LARGE__) */

             LDD   _startupData.nofFiniBodies; load number of cpp.
             BEQ   done                     ; if cppcount == 0, goto done
             LDX   _startupData.finiBodies  ; load address of first module to finalize
nextInit2:
             LDY   2,X+                     ; load address of first module to finalize
             PSHD
             PSHX                           ; save actual address
             JSR   0,Y                      ; call finalize function
             PULX                           ; restore actual address
             PULD                           ; restore cpp counter
             DBNE D, nextInit2
#endif /* defined(__BANKED__) || defined(__LARGE__) */
done:;
   }
}
#endif

/*lint -e451 non_bank.sgm contains a conditionally compiled CODE_SEG pragma */
#include "non_bank.sgm"
/*lint +e451 */

#pragma MESSAGE DISABLE C12053 /* Stack-pointer change not in debugging-information */
#pragma NO_FRAME
#pragma NO_ENTRY
#if !defined(__SMALL__)
#pragma NO_EXIT
#endif

/* The function _Startup must be called in order to initialize global variables and to call main */
/* You can adapt this function or call it from your startup code to implement a different startup */
/* functionality. */

/* You should also setup the needed IO registers as WINDEF (HC12A4 only) or the COP registers to run */
/* on hardware */

/* to set the reset vector several ways are possible : */
/* 1. define the function with "interrupt 0" as done below in the first case */
/* 2. add the following line to your prm file : VECTOR ADDRESS 0xfffe _Startup */
/* of course, even more posibilities exists */
/* the reset vector must be set so that the application has a defined entry point */

#if defined(__SET_RESET_VECTOR__)
__EXTERN_C void __interrupt 0 _Startup(void) {
#else
__EXTERN_C void _Startup(void) {
#endif
/*  purpose:    1)  initialize the stack
                2)  initialize the RAM, copy down init data etc (Init)
                3)  call main;
    parameters: NONE
    called from: _PRESTART-code generated by the Linker
                 or directly referenced by the reset vector */

   /* initialize the stack pointer */
   /*lint -e{960} , MISRA 14.3 REQ, macro INIT_SP_FROM_STARTUP_DESC() expands to HLI code */ 
   /*lint -e{522} , MISRA 14.2 REQ, macro INIT_SP_FROM_STARTUP_DESC() expands to HLI code */    
   INIT_SP_FROM_STARTUP_DESC(); /* HLI macro definition in hidef.h */
#if defined(_HCS12_SERIALMON)
   /* for Monitor based software remap the RAM & EEPROM to adhere
      to EB386. Edit RAM and EEPROM sections in PRM file to match these. */
   ___INITRG = 0x00;  /* lock registers block to 0x0000 */
   ___INITRM = 0x39;  /* lock Ram to end at 0x3FFF */
   ___INITEE = 0x09;  /* lock EEPROM block to end at 0x0fff */
#endif

   /* Here user defined code could be inserted, the stack could be used */
#if defined(_DO_DISABLE_COP_)
   _DISABLE_COP();
#endif

   /* Example : Set up WinDef Register to allow Paging */
#ifdef HC812A4 /* HC12 A4 derivative needs WINDEF to configure which pages are available */
#if  (__ENABLE_EPAGE__ != 0 ||  __ENABLE_DPAGE__ != 0 || __ENABLE_PPAGE__ != 0)
   WINDEF= __ENABLE_EPAGE__ | __ENABLE_DPAGE__  | __ENABLE_PPAGE__;
#endif
#endif

#if (defined(__MAP_RAM__) || defined(__MAP_FLASH__) || defined(__MAP_EXTERNAL__)) && !defined(__DO_SET_MMCTL1__)
#define __DO_SET_MMCTL1__
#endif


#if defined(__DO_SET_MMCTL1__)
  /* Set the MMCTL1 byte. Please use for HCS12XE and change the bits according   */
  /* to your configuration.                                                      */
  /* Note: MMCTL1 is write once therefore please adapt this initialization here. */
  /* This has to be done prior to the call to Init.                              */
#define _MMCTL1_ADR (0x00000013)
#define _MMCTL1_BIT_TGMRAMON (1<<7)  /* EEE Tag RAM and FTM SCRATCH RAM visible in the memory map */
#define _MMCTL1_BIT_EEEIFRON (1<<5)  /* EEE IFR visible in the memory map                         */
#define _MMCTL1_BIT_PGMIFRON (1<<4)  /* Program IFR visible in the memory map                     */
#define _MMCTL1_BIT_RAMHM    (1<<3)  /* RAM only in the higher half of the memory map             */
#define _MMCTL1_BIT_EROMON   (1<<2)  /* Enables emulated Flash or ROM memory in the memory map    */
#define _MMCTL1_BIT_ROMHM    (1<<1)  /* FLASH or ROM only in higher Half of Memory Map            */
#define _MMCTL1_BIT_ROMON    (1<<0)  /* Enable FLASH or ROM in the memory map                     */

#define _MMCTL1_SET(value)   ((*(volatile unsigned char*)_MMCTL1_ADR)= (value))

#if defined(__MAP_FLASH__)
  _MMCTL1_SET(_MMCTL1_BIT_ROMON | _MMCTL1_BIT_EROMON);
#elif defined(__MAP_EXTERNAL__)
  _MMCTL1_SET(_MMCTL1_BIT_ROMON | _MMCTL1_BIT_EROMON | _MMCTL1_BIT_ROMHM);
#else /* RAM */
  _MMCTL1_SET(_MMCTL1_BIT_ROMON | _MMCTL1_BIT_EROMON | _MMCTL1_BIT_RAMHM | _MMCTL1_BIT_ROMHM);
#endif    
#endif

#ifndef __ONLY_INIT_SP
   /*lint -e{522} , MISRA 14.2 REQ, function Init() contains HLI only */
   Init(); /* zero out, copy down, call constructors */
#endif

   /* Here user defined code could be inserted, all global variables are initilized */
#if defined(_DO_ENABLE_COP_)
   _ENABLE_COP(1);
#endif

   /* call main() */
   main();
}

/*lint --e{766} , non_bank.sgm is not a regular header file, it contains a conditionally compiled CODE_SEG pragma */
/*lint +estring(961,"only preprocessor statements and comments before '#include'") */
/*lint +e451 */
