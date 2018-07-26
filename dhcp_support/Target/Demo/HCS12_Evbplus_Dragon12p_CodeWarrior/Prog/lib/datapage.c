/******************************************************************************
  FILE        : datapage.c
  PURPOSE     : paged data access runtime routines
  MACHINE     : Freescale 68HC12 (Target)
  LANGUAGE    : ANSI-C
  HISTORY     : 21.7.96 first version created
******************************************************************************/

#include "hidef.h"

#include "non_bank.sgm"
#include "runtime.sgm"

/*lint --e{957} , MISRA 8.1 REQ, these are runtime support functions and, as such, are not meant to be called in user code; they are only invoked via jumps, in compiler-generated code */
/*lint -estring(553, __OPTION_ACTIVE__) , MISRA 19.11 REQ , __OPTION_ACTIVE__ is a built-in compiler construct to check for active compiler options */

#ifndef __HCS12X__ /* it's different for the HCS12X. See the text below at the #else // __HCS12X__ */

/*
   According to the -Cp option of the compiler the
   __DPAGE__, __PPAGE__ and __EPAGE__ macros are defined.
   If none of them is given as argument, then no page accesses should occur and
   this runtime routine should not be used !
   To be on the save side, the runtime routines are created anyway.
*/

/* Compile with option -DHCS12 to activate this code */
#if defined(HCS12) || defined(_HCS12) || defined(__HCS12__)
#ifndef PPAGE_ADDR
#ifdef __PPAGE_ADR__
#define PPAGE_ADDR __PPAGE_ADR__
#else
#define PPAGE_ADDR (0x30 + REGISTER_BASE)
#endif
#endif
#ifndef __PPAGE__ /* may be set already by option -CPPPAGE */
#define __PPAGE__
#endif
/* Compile with option -DDG128 to activate this code */
#elif defined DG128 /* HC912DG128 derivative has PPAGE register only at 0xFF */
#ifndef PPAGE_ADDR
#define PPAGE_ADDR (0xFF+REGISTER_BASE)
#endif
#ifndef __PPAGE__ /* may be set already by option -CPPPAGE */
#define __PPAGE__
#endif
#elif defined(HC812A4)
/* all setting default to A4 already */
#endif


#if !defined(__EPAGE__) && !defined(__PPAGE__) && !defined(__DPAGE__)
/* as default use all page registers */
#define __DPAGE__
#define __EPAGE__
#define __PPAGE__
#endif

/* modify the following defines to your memory configuration */

#define EPAGE_LOW_BOUND   0x400u
#define EPAGE_HIGH_BOUND  0x7ffu

#define DPAGE_LOW_BOUND   0x7000u
#define DPAGE_HIGH_BOUND  0x7fffu

#define PPAGE_LOW_BOUND   (DPAGE_HIGH_BOUND+1u)
#define PPAGE_HIGH_BOUND  0xBFFFu

#ifndef REGISTER_BASE
#define REGISTER_BASE      0x0u
#endif

#ifndef DPAGE_ADDR
#define DPAGE_ADDR        (0x34u+REGISTER_BASE)
#endif
#ifndef EPAGE_ADDR
#define EPAGE_ADDR        (0x36u+REGISTER_BASE)
#endif
#ifndef PPAGE_ADDR
#define PPAGE_ADDR        (0x35u+REGISTER_BASE)
#endif

/*
  The following parts about the defines are assumed in the code of _GET_PAGE_REG :
  - the memory region controlled by DPAGE is above the area controlled by the EPAGE and
    below the area controlled by the PPAGE.
  - the lower bound of the PPAGE area is equal to be the higher bound of the DPAGE area + 1
*/
#if (EPAGE_LOW_BOUND >= EPAGE_HIGH_BOUND) || (EPAGE_HIGH_BOUND >= DPAGE_LOW_BOUND) || (DPAGE_LOW_BOUND >= DPAGE_HIGH_BOUND) || (DPAGE_HIGH_BOUND >= PPAGE_LOW_BOUND) || (PPAGE_LOW_BOUND >= PPAGE_HIGH_BOUND)
#error /* please adapt _GET_PAGE_REG for this non default page configuration */
#endif

#if (DPAGE_HIGH_BOUND+1u) != PPAGE_LOW_BOUND
#error /* please adapt _GET_PAGE_REG for this non default page configuration */
#endif


/* this module does either control if any access is in the bounds of the specified page or */
/* ,if only one page is specified, just use this page. */
/* This behavior is controlled by the define USE_SEVERAL_PAGES. */
/* If !USE_SEVERAL_PAGES does increase the performance significantly */
/* NOTE : When !USE_SEVERAL_PAGES, the page is also set for accesses outside of the area controlled */
/*        by this single page. But this is should not cause problems because the page is restored to the old value before any other access could occur */

#if !defined(__DPAGE__) && !defined(__EPAGE__) && !defined(__PPAGE__)
/* no page at all is specified */
/* only specifying the right pages will speed up these functions a lot */
#define USE_SEVERAL_PAGES 1
#elif (defined(__DPAGE__) && defined(__EPAGE__)) || (defined(__DPAGE__) && defined(__PPAGE__)) || (defined(__EPAGE__) && defined(__PPAGE__))
/* more than one page register is used */
#define USE_SEVERAL_PAGES 1
#else

#define USE_SEVERAL_PAGES 0

#if defined(__DPAGE__) /* check which pages are used  */
#define PAGE_ADDR PPAGE_ADDR
#elif defined(__EPAGE__)
#define PAGE_ADDR EPAGE_ADDR
#elif defined(__PPAGE__)
#define PAGE_ADDR PPAGE_ADDR
#else /* we do not know which page, decide it at runtime */
#error /* must not happen */
#endif

#endif


#if USE_SEVERAL_PAGES /* only needed for several pages support */
/*--------------------------- _GET_PAGE_REG --------------------------------
  Runtime routine to detect the right register depending on the 16 bit offset part
  of an address.
  This function is only used by the functions below.

  Depending on the compiler options -Cp different versions of _GET_PAGE_REG are produced.

  Arguments :
  - Y : offset part of an address

  Result :
  if address Y is controlled by a page register :
  - X : address of page register if Y is controlled by an page register
  - Zero flag cleared
  - all other registers remain unchanged

  if address Y is not controlled by a page register :
  - Zero flag is set
  - all registers remain unchanged

  --------------------------- _GET_PAGE_REG ----------------------------------*/

#if defined(__DPAGE__)

#ifdef __cplusplus
extern "C"
#endif
#pragma NO_ENTRY
#pragma NO_EXIT
#pragma NO_FRAME

static void NEAR _GET_PAGE_REG(void) { /*lint -esym(528, _GET_PAGE_REG) used in asm code */
  asm {
L_DPAGE:
        CPY     #DPAGE_LOW_BOUND  ;/* test of lower bound of DPAGE */
#if defined(__EPAGE__)
        BLO     L_EPAGE           ;/* EPAGE accesses are possible */
#else
        BLO     L_NOPAGE          ;/* no paged memory below accesses */
#endif
        CPY     #DPAGE_HIGH_BOUND ;/* test of higher bound DPAGE/lower bound PPAGE */
#if defined(__PPAGE__)
        BHI     L_PPAGE           ;/* EPAGE accesses are possible */
#else
        BHI     L_NOPAGE          ;/* no paged memory above accesses */
#endif
FOUND_DPAGE:
        LDX     #DPAGE_ADDR       ;/* load page register address and clear zero flag */
        RTS

#if defined(__PPAGE__)
L_PPAGE:
        CPY     #PPAGE_HIGH_BOUND ;/* test of higher bound of PPAGE */
        BHI     L_NOPAGE
FOUND_PPAGE:
        LDX     #PPAGE_ADDR       ;/* load page register address and clear zero flag */
        RTS
#endif

#if defined(__EPAGE__)
L_EPAGE:
        CPY     #EPAGE_LOW_BOUND  ;/* test of lower bound of EPAGE */
        BLO     L_NOPAGE
        CPY     #EPAGE_HIGH_BOUND ;/* test of higher bound of EPAGE */
        BHI     L_NOPAGE

FOUND_EPAGE:
        LDX     #EPAGE_ADDR       ;/* load page register address and clear zero flag */
        RTS
#endif

L_NOPAGE:
        ORCC    #0x04             ;/* sets zero flag */
        RTS
  }
}

#else /* !defined(__DPAGE__) */

#if defined( __PPAGE__ )

#ifdef __cplusplus
extern "C"
#endif
#pragma NO_ENTRY
#pragma NO_EXIT
#pragma NO_FRAME

static void NEAR _GET_PAGE_REG(void) {	/*lint -esym(528, _GET_PAGE_REG) used in asm code */
  asm {
L_PPAGE:
        CPY     #PPAGE_LOW_BOUND  ;/* test of lower bound of PPAGE */
#if defined( __EPAGE__ )
        BLO     L_EPAGE
#else
        BLO     L_NOPAGE          ;/* no paged memory below */
#endif
        CPY     #PPAGE_HIGH_BOUND ;/* test of higher bound PPAGE */
        BHI     L_NOPAGE
FOUND_PPAGE:
        LDX     #PPAGE_ADDR       ;/* load page register address and clear zero flag */
        RTS
#if defined( __EPAGE__ )
L_EPAGE:
        CPY     #EPAGE_LOW_BOUND  ;/* test of lower bound of EPAGE */
        BLO     L_NOPAGE
        CPY     #EPAGE_HIGH_BOUND ;/* test of higher bound of EPAGE */
        BHI     L_NOPAGE
FOUND_EPAGE:
        LDX     #EPAGE_ADDR       ;/* load page register address and clear zero flag */
        RTS
#endif

L_NOPAGE:                         ;/* not in any allowed page area */
                                  ;/* its a far access to a non paged variable */
        ORCC #0x04                ;/* sets zero flag */
        RTS
  }
}

#else /* !defined(__DPAGE__ ) && !defined( __PPAGE__) */
#if defined(__EPAGE__)

#ifdef __cplusplus
extern "C"
#endif
#pragma NO_ENTRY
#pragma NO_EXIT
#pragma NO_FRAME

static void NEAR _GET_PAGE_REG(void) { /*lint -esym(528, _GET_PAGE_REG) used in asm code */
  asm {
L_EPAGE:
        CPY     #EPAGE_LOW_BOUND  ;/* test of lower bound of EPAGE */
        BLO     L_NOPAGE
        CPY     #EPAGE_HIGH_BOUND ;/* test of higher bound of EPAGE */
        BHI     L_NOPAGE
FOUND_EPAGE:
        LDX     #EPAGE_ADDR       ;/* load page register address and clear zero flag */
        RTS

L_NOPAGE:                         ;/* not in any allowed page area */
                                  ;/* its a far access to a non paged variable */
        ORCC    #0x04             ;/* sets zero flag */
        RTS
  }
}

#endif /*  defined(__EPAGE__) */
#endif /*  defined(__PPAGE__) */
#endif /*  defined(__DPAGE__) */

#endif /* USE_SEVERAL_PAGES */

/*--------------------------- _SET_PAGE --------------------------------
  Runtime routine to set the right page register. This routine is used if the compiler
  does not know the right page register, i.e. if the option -Cp is used for more than
  one page register or if the runtime option is used for one of the -Cp options.

  Arguments :
  - offset part of an address in the Y register
  - page part of an address in the B register

  Result :
  - page part written into the correct page register.
  - the old page register content is destroyed
  - all processor registers remains unchanged
  --------------------------- _SET_PAGE ----------------------------------*/

#ifdef __cplusplus
extern "C"
#endif
#pragma NO_ENTRY
#pragma NO_EXIT
#pragma NO_FRAME

void NEAR _SET_PAGE(void) {
#if USE_SEVERAL_PAGES
  asm {
        PSHX                      ;/* save X register */
        __PIC_JSR(_GET_PAGE_REG)
        BEQ     L_NOPAGE
        STAB    0,X               ;/* set page register */
L_NOPAGE:
        PULX                      ;/* restore X register */
        RTS
  }
#else /* USE_SEVERAL_PAGES */
  asm {
        STAB    PAGE_ADDR         ;/* set page register */
        RTS
  }
#endif /* USE_SEVERAL_PAGES */
}

/*--------------------------- _LOAD_FAR_8 --------------------------------
  This runtime routine is used to access paged memory via a runtime function.
  It may also be used if the compiler option -Cp is not used with the runtime argument.

  Arguments :
  - offset part of an address in the Y register
  - page part of an address in the B register

  Result :
  - value to be read in the B register
  - all other registers remains unchanged
  - all page register still contain the same value
  --------------------------- _LOAD_FAR_8 ----------------------------------*/

#ifdef __cplusplus
extern "C"
#endif
#pragma NO_ENTRY
#pragma NO_EXIT
#pragma NO_FRAME

void NEAR _LOAD_FAR_8(void) {
#if USE_SEVERAL_PAGES
  asm {
        PSHX                      ;/* save X register */
        __PIC_JSR(_GET_PAGE_REG)
        BEQ     L_NOPAGE
        PSHA                      ;/* save A register */
        LDAA    0,X               ;/* save page register */
        STAB    0,X               ;/* set page register */
        LDAB    0,Y               ;/* actual load, overwrites page */
        STAA    0,X               ;/* restore page register */
        PULA                      ;/* restore A register */
        PULX                      ;/* restore X register */
        RTS
L_NOPAGE:
        LDAB    0,Y               ;/* actual load, overwrites page */
        PULX                      ;/* restore X register */
        RTS
  }
#else /* USE_SEVERAL_PAGES */
  asm {
        PSHA                      ;/* save A register */
        LDAA    PAGE_ADDR         ;/* save page register */
        STAB    PAGE_ADDR         ;/* set page register */
        LDAB    0,Y               ;/* actual load, overwrites page */
        STAA    PAGE_ADDR         ;/* restore page register */
        PULA                      ;/* restore A register */
        RTS
  }
#endif /* USE_SEVERAL_PAGES */
}

/*--------------------------- _LOAD_FAR_16 --------------------------------
  This runtime routine is used to access paged memory via a runtime function.
  It may also be used if the compiler  option -Cp is not used with the runtime argument.

  Arguments :
  - offset part of an address in the Y register
  - page part of an address in the B register

  Result :
  - value to be read in the Y register
  - all other registers remains unchanged
  - all page register still contain the same value
  --------------------------- _LOAD_FAR_16 ----------------------------------*/

#ifdef __cplusplus
extern "C"
#endif
#pragma NO_ENTRY
#pragma NO_EXIT
#pragma NO_FRAME

void NEAR _LOAD_FAR_16(void) {
#if USE_SEVERAL_PAGES
  asm {
        PSHX                      ;/* save X register */
        __PIC_JSR(_GET_PAGE_REG)
        BEQ     L_NOPAGE
        PSHA                      ;/* save A register */
        LDAA    0,X               ;/* save page register */
        STAB    0,X               ;/* set page register */
        LDY     0,Y               ;/* actual load, overwrites address */
        STAA    0,X               ;/* restore page register */
        PULA                      ;/* restore A register */
        PULX                      ;/* restore X register */
        RTS
L_NOPAGE:
        LDY     0,Y               ;/* actual load, overwrites address */
        PULX                      ;/* restore X register */
        RTS
  }
#else /* USE_SEVERAL_PAGES */
  asm {
        PSHA                      ;/* save A register */
        LDAA    PAGE_ADDR         ;/* save page register */
        STAB    PAGE_ADDR         ;/* set page register */
        LDY     0,Y               ;/* actual load, overwrites address */
        STAA    PAGE_ADDR         ;/* restore page register */
        PULA                      ;/* restore A register */
        RTS
  }
#endif /* USE_SEVERAL_PAGES */
}
/*--------------------------- _LOAD_FAR_24 --------------------------------
  This runtime routine is used to access paged memory via a runtime function.
  It may also be used if the compiler  option -Cp is not used with the runtime argument.

  Arguments :
  - offset part of an address in the Y register
  - page part of an address in the B register

  Result :
  - value to be read in the Y:B registers
  - all other registers remains unchanged
  - all page register still contain the same value
  --------------------------- _LOAD_FAR_24 ----------------------------------*/

#ifdef __cplusplus
extern "C"
#endif
#pragma NO_ENTRY
#pragma NO_EXIT
#pragma NO_FRAME

void NEAR _LOAD_FAR_24(void) {
#if USE_SEVERAL_PAGES
  asm {
        PSHX                      ;/* save X register */
        __PIC_JSR(_GET_PAGE_REG)
        BEQ     L_NOPAGE
        PSHA                      ;/* save A register */
        LDAA    0,X               ;/* save page register */
        STAB    0,X               ;/* set page register */
        LDAB    0,Y               ;/* actual load, overwrites page of address */
        LDY     1,Y               ;/* actual load, overwrites offset of address */
        STAA    0,X               ;/* restore page register */
        PULA                      ;/* restore A register */
        PULX                      ;/* restore X register */
        RTS
L_NOPAGE:
        LDAB    0,Y               ;/* actual load, overwrites page of address */
        LDY     1,Y               ;/* actual load, overwrites offset of address */
        PULX                      ;/* restore X register */
        RTS
  }
#else /* USE_SEVERAL_PAGES */
  asm {
        PSHA                      ;/* save A register */
        LDAA    PAGE_ADDR         ;/* save page register */
        STAB    PAGE_ADDR         ;/* set page register */
        LDAB    0,Y               ;/* actual load, overwrites page of address */
        LDY     1,Y               ;/* actual load, overwrites offset of address */
        STAA    PAGE_ADDR         ;/* restore page register */
        PULA                      ;/* restore A register */
        RTS
  }
#endif /* USE_SEVERAL_PAGES */

}

/*--------------------------- _LOAD_FAR_32 --------------------------------
  This runtime routine is used to access paged memory via a runtime function.
  It may also be used if the compiler  option -Cp is not used with the runtime argument.

  Arguments :
  - offset part of an address in the Y register
  - page part of an address in the B register

  Result :
  - low 16 bit of value to be read in the D registers
  - high 16 bit of value to be read in the Y registers
  - all other registers remains unchanged
  - all page register still contain the same value
  --------------------------- _LOAD_FAR_32 ----------------------------------*/

#ifdef __cplusplus
extern "C"
#endif
#pragma NO_ENTRY
#pragma NO_EXIT
#pragma NO_FRAME

void NEAR _LOAD_FAR_32(void) {
#if USE_SEVERAL_PAGES
  asm {
        PSHX                      ;/* save X register */
        __PIC_JSR(_GET_PAGE_REG)
        BEQ     L_NOPAGE
        LDAA    0,X               ;/* save page register */
        PSHA                      ;/* put it onto the stack */
        STAB    0,X               ;/* set page register */
        LDD     2,Y               ;/* actual load, low word */
        LDY     0,Y               ;/* actual load, high word */
        MOVB    1,SP+,0,X         ;/* restore page register */
        PULX                      ;/* restore X register */
        RTS
L_NOPAGE:
        LDD     2,Y               ;/* actual load, low word */
        LDY     0,Y               ;/* actual load, high word */
        PULX                      ;/* restore X register */
        RTS
  }
#else /* USE_SEVERAL_PAGES */
  asm {
        LDAA    PAGE_ADDR         ;/* save page register */
        PSHA                      ;/* put it onto the stack */
        STAB    PAGE_ADDR         ;/* set page register */
        LDD     2,Y               ;/* actual load, low word */
        LDY     0,Y               ;/* actual load, high word */
        MOVB    1,SP+,PAGE_ADDR   ;/* restore page register */
        RTS
  }
#endif /* USE_SEVERAL_PAGES */
}

/*--------------------------- _STORE_FAR_8 --------------------------------
  This runtime routine is used to access paged memory via a runtime function.
  It may also be used if the compiler  option -Cp is not used with the runtime argument.

  Arguments :
  - offset part of an address in the Y register
  - page part of an address in the B register
  - value to be stored in the B register

  Result :
  - value stored at the address
  - all registers remains unchanged
  - all page register still contain the same value
  --------------------------- _STORE_FAR_8 ----------------------------------*/

#ifdef __cplusplus
extern "C"
#endif
#pragma NO_ENTRY
#pragma NO_EXIT
#pragma NO_FRAME

void NEAR _STORE_FAR_8(void) {
#if USE_SEVERAL_PAGES
  asm {
        PSHX                      ;/* save X register */
        __PIC_JSR(_GET_PAGE_REG)
        BEQ     L_NOPAGE
        PSHB                      ;/* save B register */
        LDAB    0,X               ;/* save page register */
        MOVB    0,SP, 0,X         ;/* set page register */
        STAA    0,Y               ;/* store the value passed in A */
        STAB    0,X               ;/* restore page register */
        PULB                      ;/* restore B register */
        PULX                      ;/* restore X register */
        RTS
L_NOPAGE:
        STAA    0,Y               ;/* store the value passed in A */
        PULX                      ;/* restore X register */
        RTS
  }
#else /* USE_SEVERAL_PAGES */
  asm {
        PSHB                      ;/* save A register */
        LDAB    PAGE_ADDR         ;/* save page register */
        MOVB    0,SP,PAGE_ADDR    ;/* set page register */
        STAA    0,Y               ;/* store the value passed in A */
        STAB    PAGE_ADDR         ;/* restore page register */
        PULB                      ;/* restore B register */
        RTS
  }
#endif /* USE_SEVERAL_PAGES */
}

/*--------------------------- _STORE_FAR_16 --------------------------------
  This runtime routine is used to access paged memory via a runtime function.
  It may also be used if the compiler  option -Cp is not used with the runtime argument.

  Arguments :
  - offset part of an address in the Y register
  - page part of an address in the B register
  - value to be stored in the X register

  Result :
  - value stored at the address
  - all registers remains unchanged
  - all page register still contain the same value
  --------------------------- _STORE_FAR_16 ----------------------------------*/

#ifdef __cplusplus
extern "C"
#endif
#pragma NO_ENTRY
#pragma NO_EXIT
#pragma NO_FRAME

void NEAR _STORE_FAR_16(void) {
#if USE_SEVERAL_PAGES
  asm {
        PSHX                      ;/* save X register */
        __PIC_JSR(_GET_PAGE_REG)
        BEQ     L_NOPAGE

        PSHA
        LDAA    0,X               ;/* save page register */
        STAB    0,X               ;/* set page register */
        MOVW    1,SP,0,Y          ;/* store the value passed in X */
        STAA    0,X               ;/* restore page register */
        PULA                      ;/* restore A register */
        PULX                      ;/* restore X register */
        RTS

L_NOPAGE:
        STX 0,Y                   ;/* store the value passed in X */
        PULX                      ;/* restore X register */
        RTS
  }
#else /* USE_SEVERAL_PAGES */
  asm {
        PSHA                      ;/* save A register */
        LDAA    PAGE_ADDR         ;/* save page register */
        STAB    PAGE_ADDR         ;/* set page register */
        STX     0,Y               ;/* store the value passed in X */
        STAA    PAGE_ADDR         ;/* restore page register */
        PULA                      ;/* restore A register */
        RTS
  }
#endif /* USE_SEVERAL_PAGES */
}
/*--------------------------- _STORE_FAR_24 --------------------------------
  This runtime routine is used to access paged memory via a runtime function.
  It may also be used if the compiler  option -Cp is not used with the runtime argument.

  Arguments :
  - offset part of an address in the Y register
  - page part of an address in the B register
  - value to be stored in the X:A registers (X : low 16 bit, A : high 8 bit)

  Result :
  - value stored at the address
  - all registers remains unchanged
  - all page register still contain the same value
  --------------------------- _STORE_FAR_24 ----------------------------------*/

#ifdef __cplusplus
extern "C"
#endif
#pragma NO_ENTRY
#pragma NO_EXIT
#pragma NO_FRAME

void NEAR _STORE_FAR_24(void) {
#if USE_SEVERAL_PAGES
  asm {
        PSHX                      ;/* save X register */
        __PIC_JSR(_GET_PAGE_REG)
        BEQ     L_NOPAGE

        PSHA
        LDAA    0,X               ;/* save page register */
        STAB    0,X               ;/* set page register */
        MOVW    1,SP, 1,Y         ;/* store the value passed in X */
        MOVB    0,SP, 0,Y         ;/* store the value passed in A */
        STAA    0,X               ;/* restore page register */
        PULA                      ;/* restore A register */
        PULX                      ;/* restore X register */
        RTS

L_NOPAGE:
        STX     1,Y               ;/* store the value passed in X */
        STAA    0,Y               ;/* store the value passed in X */
        PULX                      ;/* restore X register */
        RTS
  }
#else /* USE_SEVERAL_PAGES */
  asm {
        PSHA                      ;/* save A register */
        LDAA    PAGE_ADDR         ;/* save page register */
        STAB    PAGE_ADDR         ;/* set page register */
        MOVB    0,SP, 0,Y         ;/* store the value passed in A */
        STX     1,Y               ;/* store the value passed in X */
        STAA    PAGE_ADDR         ;/* restore page register */
        PULA                      ;/* restore A register */
        RTS
  }
#endif /* USE_SEVERAL_PAGES */
}
/*--------------------------- _STORE_FAR_32 --------------------------------
  This runtime routine is used to access paged memory via a runtime function.
  It may also be used if the compiler  option -Cp is not used with the runtime argument.

  Arguments :
  - offset part of an address in the Y register
  - page part of an address is on the stack at 3,SP (just below the return address)
  - value to be stored in the X:D registers (D : low 16 bit, X : high 16 bit)

  Result :
  - value stored at the address
  - all registers remains unchanged
  - the page part is removed from the stack
  - all page register still contain the same value
  --------------------------- _STORE_FAR_32 ----------------------------------*/

#ifdef __cplusplus
extern "C"
#endif
#pragma NO_ENTRY
#pragma NO_EXIT
#pragma NO_FRAME

void NEAR _STORE_FAR_32(void) {
#if USE_SEVERAL_PAGES
  asm {
        PSHX                      ;/* save X register */
        __PIC_JSR(_GET_PAGE_REG)
        BEQ     L_NOPAGE

        PSHD
        LDAA    0,X               ;/* save page register */
        MOVB    6,SP, 0,X         ;/* set page register */
        MOVW    2,SP, 0,Y         ;/* store the value passed in X (high word) */
        MOVW    0,SP, 2,Y         ;/* store the value passed in D (low word) */
        STAA    0,X               ;/* restore page register */
        PULD                      ;/* restore A register */
        BRA     done

L_NOPAGE:
        MOVW    0,SP, 0,Y         ;/* store the value passed in X (high word) */
        STD           2,Y         ;/* store the value passed in D (low word) */
done:
        PULX                      ;/* restore X register */
        MOVW    0,SP, 1,+SP       ;/* move return address */
        RTS
  }
#else /* USE_SEVERAL_PAGES */
  asm {
        PSHD                      ;/* save D register */
        LDAA    PAGE_ADDR         ;/* save page register */
        LDAB    4,SP              ;/* load page part of address */
        STAB    PAGE_ADDR         ;/* set page register */
        STX     0,Y               ;/* store the value passed in X */
        MOVW    0,SP, 2,Y         ;/* store the value passed in D (low word) */
        STAA    PAGE_ADDR         ;/* restore page register */
        PULD                      ;/* restore D register */
        MOVW    0,SP, 1,+SP       ;/* move return address */
        RTS
  }
#endif /* USE_SEVERAL_PAGES */
}

/*--------------------------- _FAR_COPY_RC --------------------------------
  This runtime routine is used to access paged memory via a runtime function.
  It may also be used if the compiler  option -Cp is not used with the runtime argument.

  Arguments :
  - offset part of the source int the X register
  - page part of the source in the A register
  - offset part of the dest int the Y register
  - page part of the dest in the B register
  - number of bytes to be copied is defined by the next 2 bytes after the return address.

  Result :
  - memory area copied
  - no registers are saved, i.e. all registers may be destroyed
  - all page register still contain the same value as before the call
  - the function returns after the constant defining the number of bytes to be copied


  stack-structure at the loop-label:
     0,SP : destination offset
     2,SP : source page
     3,SP : destination page
     4,SP : source offset
     6,SP : points to length to be copied. This function returns after the size

  A usual call to this function looks like:

  struct Huge src, dest;
    ; ...
    LDX  #src
    LDAA #PAGE(src)
    LDY  #dest
    LDAB #PAGE(dest)
    JSR  _FAR_COPY_RC
    DC.W sizeof(struct Huge)
    ; ...

  --------------------------- _FAR_COPY_RC ----------------------------------*/

#ifdef __cplusplus
extern "C"
#endif
#pragma NO_ENTRY
#pragma NO_EXIT
#pragma NO_FRAME

void NEAR _FAR_COPY_RC(void) {
#if USE_SEVERAL_PAGES
  asm {
        DEX                       ;/* source addr-=1, because loop counter ends at 1 */
        PSHX                      ;/* save source offset */
        PSHD                      ;/* save both pages */
        DEY                       ;/* destination addr-=1, because loop counter ends at 1 */
        PSHY                      ;/* save destination offset */
        LDY     6,SP              ;/* Load Return address */
        LDX     2,Y+              ;/* Load Size to copy */
        STY     6,SP              ;/* Store adjusted return address */
loop:
        LDD     4,SP              ;/* load source offset */
        LEAY    D,X               ;/* calculate actual source address */
        LDAB    2,SP              ;/* load source page */
        __PIC_JSR(_LOAD_FAR_8)    ;/* load 1 source byte */
        PSHB                      ;/* save value */
        LDD     0+1,SP            ;/* load destination offset */
        LEAY    D,X               ;/* calculate actual destination address */
        PULA                      ;/* restore value */
        LDAB    3,SP              ;/* load destination page */
        __PIC_JSR(_STORE_FAR_8)   ;/* store one byte */
        DEX
        BNE     loop
        LEAS    6,SP              ;/* release stack */
        _SRET                     ;/* debug info only: This is the last instr of a function with a special return */
        RTS                       ;/* return */
  }
#else
  asm {
        PSHD                      ;/* store page registers */
        TFR     X,D
        PSHY                      ;/* temporary space */
        LDY     4,SP              ;/* load return address */
        ADDD    2,Y+              ;/* calculate source end address. Increment return address */
        STY     4,SP
        PULY
        PSHD                      ;/* store src end address */
        LDAB    2,SP              ;/* reload source page */
        LDAA    PAGE_ADDR         ;/* save page register */
        PSHA
loop:
        STAB    PAGE_ADDR         ;/* set source page */
        LDAA    1,X+              ;/* load value */
        MOVB    4,SP, PAGE_ADDR   ;/* set destination page */
        STAA    1,Y+
        CPX     1,SP
        BNE     loop

        LDAA    5,SP+             ;/* restore old page value and release stack */
        STAA    PAGE_ADDR         ;/* store it into page register */
        _SRET                     ;/* debug info only: This is the last instr of a function with a special return */
        RTS
  }
#endif
}

/*--------------------------- _FAR_COPY --------------------------------

  The _FAR_COPY runtime routine was used to copied large memory blocks in previous compiler releases.
  However this release now does use _FAR_COPY_RC instead. The only difference is how the size of 
  the area to be copied is passed into the function. For _FAR_COPY the size is passed on the stack just
  above the return address. _FAR_COPY_RC does expect the return address just after the JSR _FAR_COPY_RC call
  in the code of the caller. This allows for denser code calling _FAR_COPY_RC but does also need a slightly
  larger runtime routine and it is slightly slower.
  The _FAR_COPY routine is here now mainly for compatibility with previous releases. 
  The current compiler does not use it. 
  
--------------------------- _FAR_COPY ----------------------------------*/

#ifdef __cplusplus
extern "C"
#endif
#pragma NO_ENTRY
#pragma NO_EXIT
#pragma NO_FRAME

void NEAR _FAR_COPY(void) {
#if USE_SEVERAL_PAGES
  asm {
        DEX                       ;/* source addr-=1, because loop counter ends at 1 */
        PSHX                      ;/* save source offset */
        PSHD                      ;/* save both pages */
        DEY                       ;/* destination addr-=1, because loop counter ends at 1 */
        PSHY                      ;/* save destination offset */
        LDX     8,SP              ;/* load counter, assuming counter > 0 */

loop:
        LDD     4,SP              ;/* load source offset */
        LEAY    D,X               ;/* calculate actual source address */
        LDAB    2,SP              ;/* load source page */
        __PIC_JSR(_LOAD_FAR_8)    ;/* load 1 source byte */
        PSHB                      ;/* save value */
        LDD     0+1,SP            ;/* load destination offset */
        LEAY    D,X               ;/* calculate actual destination address */
        PULA                      ;/* restore value */
        LDAB    3,SP              ;/* load destination page */
        __PIC_JSR(_STORE_FAR_8)   ;/* store one byte */
        DEX
        BNE     loop
        LDX     6,SP              ;/* load return address */
        LEAS    10,SP             ;/* release stack */
        JMP     0,X               ;/* return */
  }
#else
  asm {
        PSHD                      ;/* store page registers */
        TFR     X,D
        ADDD    4,SP              ;/* calculate source end address */
        STD     4,SP
        PULB                      ;/* reload source page */
        LDAA    PAGE_ADDR         ;/* save page register */
        PSHA
loop:
        STAB    PAGE_ADDR         ;/* set source page */
        LDAA    1,X+              ;/* load value */
        MOVB    1,SP, PAGE_ADDR   ;/* set destination page */
        STAA    1,Y+
        CPX     4,SP
        BNE     loop

        LDAA    2,SP+             ;/* restore old page value and release stack */
        STAA    PAGE_ADDR         ;/* store it into page register */
        LDX     4,SP+             ;/* release stack and load return address */
        JMP     0,X               ;/* return */
  }
#endif
}

#else  /* __HCS12X__  */

/*
  The HCS12X knows two different kind of addresses:
    - Logical addresses. E.g.
       MOVB #page(var),RPAGE
       INC var

    - Global addresses E.g.
       MOVB #page(var),GPAGE
       GLDAA var
       INCA
       GSTAA var

  Global addresses are used with G-Load's and G-Store's, logical addresses are used for all the other instructions
  and occasions. As HC12's or HCS12's do not have the G-Load and G-Store instructions,
  global addresses are not used with these processor families.
  They are only used with HCS12X chips (and maybe future ones deriving from a HCS12X).

  Logical and Global addresses can point to the same object, however the global and logical address of an object
  are different for most objects (actually for all except the registers from 0 to 0x7FF).
  Therefore the compiler needs to transform in between them.

  HCS12X Pointer types:

    The following are logical addresses:
    - all 16 bit pointers
       - "char* __near": always.
       - "char *" in the small and banked memory model
    - 24 bit dpage, epage, ppage or rpage pointers (*1) (note: the first HCS12X compilers may not support these pointer types)
       - "char *__dpage": Note this type only exists for
                          orthogonality with the HC12 A4 chip which has a DPAGE reg.
                          It does not apply to the HCS12X.
       - "char *__epage": 24 bit pointer using the EPAGE register
       - "char *__ppage": 24 bit pointer using the PPAGE register.
                          As the PPAGE is also used for BANKED code,
                          using this pointer type is only legal from non banked code.
       - "char *__rpage": 24 bit pointer using the RPAGE register


    The following are global addresses:
       "char*": in the large memory model (only HCS12X)
       "char* __far": always for HCS12X.

   (*1): For the HC12 and HCS12 "char* __far" and "char*" in the large memory model are also logical.

   Some notes for the HC12/HCS12 programmers.

   The address of a far object for a HC12 and for a HCS12X is different, even if they are at the same place in the memory map.
   For the HC12, a far address is using the logical addresses, for the HCS12X however, far addresses are using global addresses.
   This does cause troubles for the unaware!
   
   The conversion routines implemented in this file support the special HCS12XE RAM mapping (when RAMHM is set).
   To enable this mapping compile this file with the "-MapRAM" compiler option.

  HCS12X Logical Memory map

    Logical Addresses           Used for                shadowed at           page register     Global Address

    0x000000 .. 0x0007FF        Peripheral Registers                          Not Paged         0x000000
    0x??0800 .. 0x??0BFF        Paged EEPROM                                  EPAGE (@0x17)     0x100000+EPAGE*0x0400
    0x000C00 .. 0x000FFF        Non Paged EEPROM        0xFF0800..0xFF0FFF    Not Paged         0x13FC00
    0x??1000 .. 0x??1FFF        Paged RAM                                     RPAGE (@0x16)     0x000000+RPAGE*0x1000
    0x002000 .. 0x003FFF        Non Paged RAM           0xFE1000..0xFF1FFF    Not Paged         0x0FE000
    0x004000 .. 0x007FFF        Non Paged FLASH         0xFC8000..0xFCBFFF    Not Paged         0x7F4000
    0x??8000 .. 0x00BFFF        Paged FLASH                                   PPAGE (@0x30)     0x400000+PPAGE*0x4000
    0x00C000 .. 0x00FFFF        Non Paged FLASH         0xFF8000..0xFFBFFF    Not Paged         0x7FC000

    NA: Not Applicable

  HCS12X Global Memory map

    Global Addresses            Used for                Logical mapped at

    0x000000 .. 0x0007FF        Peripheral Registers    0x000000 .. 0x0007FF
    0x000800 .. 0x000FFF        DMA registers           Not mapped
    0x001000 .. 0x0FFFFF        RAM                     0x??1000 .. 0x??1FFF
    0x0FE000 .. 0x0FFFFF        RAM, Log non paged      0x002000 .. 0x003FFF
    0x100000 .. 0x13FFFF        EEPROM                  0x??0800 .. 0x??0BFF
    0x13FC00 .. 0x13FFFF        EEPROM  non paged       0x000C00 .. 0x000FFF
    0x140000 .. 0x3FFFFF        External Space          Not mapped
    0x400000 .. 0x7FFFFF        FLASH                   0x??8000 .. 0x??BFFF
    0x7F4000 .. 0x7F7FFF        FLASH, Log non paged    0x004000 .. 0x007FFF
    0x7FC000 .. 0x7FFFFF        FLASH, Log non paged    0x00C000 .. 0x00FFFF

  HCS12XE Logical Memory map (with RAMHM set) 

    Logical Addresses           Used for                shadowed at           page register     Global Address

    0x000000 .. 0x0007FF        Peripheral Registers                          Not Paged         0x000000
    0x??0800 .. 0x??0BFF        Paged EEPROM                                  EPAGE             0x100000+EPAGE*0x0400
    0x000C00 .. 0x000FFF        Non Paged EEPROM        0xFF0800..0xFF0FFF    Not Paged         0x13FC00
    0x??1000 .. 0x??1FFF        Paged RAM                                     RPAGE             0x000000+RPAGE*0x1000
    0x002000 .. 0x003FFF        Non Paged RAM           0xFA1000..0xFB1FFF    Not Paged         0x0FA000
    0x004000 .. 0x007FFF        Non Paged RAM           0xFC1000..0xFF1FFF    Not Paged         0x0FC000
    0x??8000 .. 0x00BFFF        Paged FLASH                                   PPAGE             0x400000+PPAGE*0x4000
    0x00C000 .. 0x00FFFF        Non Paged FLASH         0xFF8000..0xFFBFFF    Not Paged         0x7FC000

    NA: Not Applicable

  HCS12X Global Memory map (with RAMHM set) 

    Global Addresses            Used for                Logical mapped at

    0x000000 .. 0x0007FF        Peripheral Registers    0x000000 .. 0x0007FF
    0x000800 .. 0x000FFF        DMA registers           Not mapped
    0x001000 .. 0x0FFFFF        RAM                     0x??1000 .. 0x??1FFF
    0x0FA000 .. 0x0FFFFF        RAM, Log non paged      0x002000 .. 0x007FFF
    0x100000 .. 0x13FFFF        EEPROM                  0x??0800 .. 0x??0BFF
    0x13FC00 .. 0x13FFFF        EEPROM  non paged       0x000C00 .. 0x000FFF
    0x140000 .. 0x3FFFFF        External Space          Not mapped
    0x400000 .. 0x7FFFFF        FLASH                   0x??8000 .. 0x??BFFF
    0x7F4000 .. 0x7F7FFF        FLASH, Log non paged    Not mapped
    0x7FC000 .. 0x7FFFFF        FLASH, Log non paged    0x00C000 .. 0x00FFFF


  How to read this table:
    For logical addresses, the lower 16 bits of the address do determine in which area the address is,
    if this address is paged, then this entry also controls and which of the EPAGE, PPAGE or RPAGE
    page register is controlling the bits 16 to 23 of the address.
    For global addresses, the bits 16 to 23 have to be in the GPAGE register and the lower 16 bits
    have to be used with the special G load or store instructions (e.g. GLDAA).
    As example the logical address 0x123456 is invalid. Because its lower bits 0x3456 are in a
    non paged area, so the page 0x12 does not exist.
    The address 0xFE1020 however does exist. To access it, the RPAGE has to contain 0xFE and the
    offset 0x1020 has to be used.

      ORG $7000
        MOVB #0xFE, 0x16 ; RPAGE
        LDAA 0x1020      ; reads at the logical address 0xFE1020

    Because the last two RAM pages are also accessible directly from 0x2000 to 0x3FFF, the
    following shorter code does read the same memory location:

      ORG $7000
        LDAA 0x2020      ; reads at the logical address 0x2020
                         ;   which maps to the same memory as 0xFE1020

    This memory location also has a global address. For logical 0xFE1020 the global address is 0x0FE020.
    So the following code does once more access the same memory location:

      ORG $7000
        MOVB #0x0F, 0x10 ; GPAGE
        GLDAA 0xE020     ; reads at the global address 0x0FE020
                         ;   which maps to the same memory as the logical addr. 0xFE1020

    Therefore every memory location for the HCS12X has up to 3 different addresses.
    Up to two logical and one global.
    Notes.
      - Not every address has a logical equivalent. The external space is only available in the global address space.

      - The PPAGE must only be set if the code is outside of the 0x8000 to 0xBFFF range.
        If not, the next code fetch will be from the new wrong PPAGE value.

      - Inside of the paged area, the highest pages are allocated first. So all HCS12X's do have the FF pages
        (if they have this memory type at all).

      - For RPAGE, the value 0 is illegal. Otherwise the global addresses would overlap with the registers.

*/

/*lint -e10, -e106, -e30 */
#if __OPTION_ACTIVE__("-MapRAM")
#define __HCS12XE_RAMHM_SET__
#endif
/*lint +e10, +e106, +e30 */

/*--------------------------- pointer conversion operations -------------------------------*/

/*--------------------------- _CONV_GLOBAL_TO_LOGICAL --------------------------------
  Convert 24 bit logical to 24 bit global pointer
    ("char*__far" to "char*__gpage")

  Arguments :
  - B : page part of global address
  - X : 16 offset part of global address

  Postcondition :
  - B == page of returned logical address
  - X == offset of returned logical address
  - Y remains unchanged
  - A remains unchanged
*/
/*--------------------------- Convert 24 bit global to 24 bit logical pointer ----------------------------------*/

/* B:X = Logical(B:X) */
#ifdef __cplusplus
extern "C"
#endif

#pragma NO_FRAME
#pragma NO_ENTRY
#pragma NO_EXIT

void NEAR _CONV_GLOBAL_TO_LOGICAL(void) {
  asm {
        CMPB    #0x40             ;/* flash (0x400000..0x7FFFFF) or not? */
        BLO     Below400000
/* from 0x400000 to 0x7FFFFF */
        CMPB    #0x7F             ;/* check for Unpaged areas 0x7FC000..0x7FFFFF and 0x7F4000..0x7F7FFF */
        BNE     PAGED_FLASH_AREA
#ifndef __HCS12XE_RAMHM_SET__
        BITX    #0x4000
        BEQ     PAGED_FLASH_AREA
#else
        CPX    #0xC000
        BLO     PAGED_FLASH_AREA
#endif
/* from 0x7F4000 to 0x7F7FFF or 0x7FC000 to 0x7FFFFF */
                                  ;/* Note: offset in X is already OK. */
        CLRB                      ;/* logical page == 0 */
        RTS
PAGED_FLASH_AREA:                 ;/* paged flash. Map to 0x8000..0xBFFF */
/* from 0x400000 to 0x7F3FFF  or 0x7F8000 to 0x7FBFFF */
        LSLX                      ; /* shift 24 bit address 2 bits to the left to get correct page in B */
        ROLB
        LSLX
        ROLB
        LSRX                      ; /* shift back to get offset from 0x8000 to 0xBFFF */
        SEC
        RORX
        RTS                       ;/* done */

Below400000:
/* from 0x000000 to 0x3FFFFF */
#if 0 /* How should we handle mapping to External Space. There is no logical equivalent. This is an error case! */
        CMPB    #0x14             ;/* check if above 0x140000. If so, its in the external space */
        BLO     Below140000
        ERROR   !!!!              ;/* this mapping is not possible! What should we do? */
        RTS
Below140000:
/* from 0x000000 to 0x13FFFF */
#endif
        CMPB    #0x10             ;/* if >= 0x100000 it's EEPROM */
        BLO     Below100000
/* from 0x100000 to 0x13FFFF (or 0x3FFFFF) */
        CMPB    #0x13             ;/* check if its is in the non paged EEPROM area at 0x13FC00..0x13FFFF */
        BLO     Below13FC00
        CPX     #0xFC00
        BLO     Below13FC00
/* from 0x13FC00 to 0x13FFFF (or 0x3FFFFF) */
        LEAX    0x1000,X          ;/* same as SUBX #0xF000 // map from 0xFC00 to 0x0C00 */
        CLRB
        RTS
Below13FC00:
/* from 0x100000 to 0x13FBFF */
        PSHA
        TFR     XH,A              ;/* calculate logical page */
        EXG     A,B
        LSRD
        LSRD
        PULA
        ANDX    #0x03FF
        LEAX    0x0800,X          ;/* same as ORX  #0x0800 */
        RTS

Below100000:
/* from 0x000000 to 0x0FFFFF */
        TSTB
        BNE     RAM_AREA
        CPX     #0x1000
        BLO     Below001000
RAM_AREA:
/* from 0x001000 to 0x0FFFFF */
        CMPB    #0x0F
        BNE     PagedRAM_AREA
#ifndef __HCS12XE_RAMHM_SET__
        CPX     #0xE000
        BLO     PagedRAM_AREA
/* from 0x0FE000 to 0x0FFFFF */
        SUBX    #(0xE000-0x2000)  ;/* map 0xE000 to 0x2000 */
#else
        CPX     #0xA000
        BLO     PagedRAM_AREA
/* from 0x0FA000 to 0x0FFFFF */
        SUBX    #(0xA000-0x2000)  ;/* map 0xA000 to 0x2000  */
#endif
        CLRB                      ;/* Page is 0 */
        RTS
PagedRAM_AREA:
/* from 0x001000 to 0x0FDFFF */
        PSHA
        TFR     XH, A             ;/* calculate logical page */
        EXG     A,B
        LSRD
        LSRD
        LSRD
        LSRD
        PULA

        ANDX    #0x0FFF
        LEAX    0x1000,X          ;/* same as ORX #0x1000 */
        RTS

Below001000:
/* from 0x000000 to 0x000FFF */
#if 0
        CMPA    #0x08
        BLO     Below000800
/* from 0x000800 to 0x000FFF */
    /* ??? DMA Regs? */
        RTS
Below000800:
/* from 0x000000 to 0x0007FF */
#endif
        CLRB
        RTS
  }
}

/*--------------------------- _CONV_GLOBAL_TO_NEAR --------------------------------
  Convert 24 bit global to 16 bit logical pointer
    ("char*__far" to "char*")

  Arguments :
  - B : page part of global address
  - X : 16 offset part of global address

  Postcondition :
  - B is undefined
  - A remains unchanged
  - X == offset of returned logical address
  - Y remains unchanged
*/
/*--------------------------- Convert 24 bit global to 16 bit logical pointer ----------------------------------*/

/* X = Logical(B:X) */

#ifdef __cplusplus
extern "C"
#endif

#define _REUSE_CONV_GLOBAL_TO_LOGICAL 1

#pragma NO_FRAME
#pragma NO_ENTRY
#pragma NO_EXIT

void NEAR _CONV_GLOBAL_TO_NEAR(void){
#if _REUSE_CONV_GLOBAL_TO_LOGICAL  /* do we want an optimized version? */
  __asm JMP _CONV_GLOBAL_TO_LOGICAL;  /* offset for NEAR is same as for LOGICAL. */
#else
  asm {
        CMPB    #0x40             ;/* flash (0x400000..0x7FFFFF) or not? */
        BLO     Below400000
/* from 0x400000 to 0x7FFFFF */
#ifndef __HCS12XE_RAMHM_SET__
        CMPB    #0x7F             ;/* check for Unpaged areas 0x7FC000..0x7FFFFF and 0x7F4000..0x7F7FFF */
        BNE     PAGED_FLASH_AREA
        CPX     #0x4000
        BLO     PAGED_FLASH_AREA
/* from 0x7F4000 to 0x7FFFFF */
#else
        CMPB    #0x7F             ;/* check for Unpaged area 0x7FC000..0x7FFFFF */
        BNE     PAGED_FLASH_AREA
        CPX     #0xC000           
        BLO     PAGED_FLASH_AREA
/* from 0x7FC000 to 0x7FFFFF       */
#endif
                                  ;/* note non PAGED flash areas or paged area 0x7F8000..0x7FBFFF which are mapping all correctly */
        RTS
PAGED_FLASH_AREA:                 ;/* paged flash. Map to 0x8000..0xBFFF */
/* from 0x400000 to 0x7F3FFF */
        ANDX    #0x3F00           ;/* cut to 0.. 0x3FFF */
        LEAX    0x8000,X          ;/* same as ORX  #0x8000     ;// move to 0x8000..0xBFFF */
        RTS                       ;/* done */

Below400000:
/* from 0x000000 to 0x3FFFFF */
#if 0 /* How should we handle mapping to External Space. There is no logical equivalent. This is an error case! */
        CMPB    #0x14             ;/* check if above 0x140000. If so, its in the external space */
        BLO     Below140000
        ERROR !!!!                ;/* this mapping is not possible! What should we do? */
        RTS
Below140000:
/* from 0x000000 to 0x13FFFF */
#endif
        CMPB    #0x10             ;/* if >= 0x100000 it's EEPROM */
        BLO     Below100000
/* from 0x100000 to 0x13FFFF (or 0x3FFFFF) */
        CMPB    #0x13             ;/* check if its is in the non paged EEPROM area at 0x13FC00..0x13FFFF */
        BNE     Below13FC00
        CPX     #0xFC00
        BLO     Below13FC00
/* from 0x13FC00 to 0x13FFFF (or 0x3FFFFF) */
        SUBX    #0xF000           ;/* map from 0xFC00 to 0x0C00 */
        RTS
Below13FC00:
/* from 0x100000 to 0x13FBFF */
        ANDX    #0x03FF
        LEAX    0x800,X           ;/* same as ORX  #0x0800 */
        RTS

Below100000:
/* from 0x000000 to 0x0FFFFF */
        TBNE    B,RAM_AREA
        CPX     #0x1000
        BLO     Below001000
RAM_AREA:
/* from 0x001000 to 0x0FFFFF */
        CMPB    #0x0F
        BNE     PagedRAM_AREA
#ifndef __HCS12XE_RAMHM_SET__
        CPX     #0xE000
        BLO     PagedRAM_AREA
/* from 0x0FE000 to 0x0FFFFF */
        SUBX    #(0xE000-0x2000)  ;/* map 0xE000 to 0x2000 */
#else
        CPX     #0xA000
        BLO     PagedRAM_AREA
/* from 0x0FA000 to 0x0FFFFF */
        SUBX    #(0xA000-0x2000)  ;/* map 0xA000 to 0x2000 */
#endif
        RTS
PagedRAM_AREA:
/* from 0x001000 to 0x0FDFFF (0x001000 to 0x0F9FFF if HCS12XE RAM mapping is enabled)  */
        ANDX    #0x0FFF
        LEAX    0x1000,X          ;/* same as ORX #0x1000 */
        RTS

Below001000:
/* from 0x000000 to 0x000FFF */
        RTS
  }
#endif
}

/*--------------------------- _CONV_NEAR_TO_GLOBAL --------------------------------
  Convert 16 bit logical to 24 bit global pointer
    ("char*__near" to "char*__far")

  Arguments :
  - X : 16 bit near pointer

  Postcondition :
  - B == page of returned global address
  - X == offset of returned global address
  - Y remains unchanged
  - A is unspecified
*/
/*--------------------------- Convert 16 bit logical to 24 bit global pointer ----------------------------------*/

/* B:X = Global(X) */

#ifdef __cplusplus
extern "C"
#endif

#pragma NO_FRAME
#pragma NO_ENTRY
#pragma NO_EXIT

void NEAR _CONV_NEAR_TO_GLOBAL(void){
  asm {
    /* syntax: */
    /*  input 16 bit offset is bit15..bit0 */
    /*  ppage values: ppage7..ppage0 */
    /*  epage values: epage7..epage0 */
    /*  dpage values: dpage7..dpage0 */
    /*  rpage values: rpage7..rpage0 */
        PSHX                      ;/* D contains bit15..bit0 */
        TFR     X,D               ;/* D is cheaper to shift */
        LSLD                      ;/* D contains 0 bit14..bit0, C contains bit15 */
        BCC     Below8000         ;/* bit15 == 0? */
        /* from 0x8000 to 0xFFFF */
        LSLD                      ;/* D contains 00 bit13..bit0, C contains bit14 */
        BCC     BelowC000
        LDAB    #0x7F
        PULX
        RTS                       ;/* returns 0b0111 1111 11 bit13...bit0 */
BelowC000:                      ;/* from 0x8000 to 0xBFFF */
        TFR     D,X
        LDAB    __PPAGE_ADR__
        SEC
        RORB
        RORX
        LSRB
        RORX
        LEAS    2,SP
        RTS                       ;/* returns 0b01 ppage7..ppage0 bit13...bit0 */
Below8000:
        LSLD                      ;/* D contains 00 bit13..bit0, C contains bit14 */
        BCC     Below4000
        /* from 0x4000 to 0x7FFF */
        PULX
#ifndef __HCS12XE_RAMHM_SET__
        LDAB    #0x7F
#else
        LEAX    (0xC000-0x4000),X
        LDAB    #0x0F             
#endif
        RTS                       ;/* returns 0b0111 1111 01 bit13...bit0 */

Below4000:
        LSLD                      ;/* D contains 000 bit12..bit0, C contains bit13 */
        BCC     Below2000
        /* from 0x2000 to 0x3FFF */
        PULX
#ifndef __HCS12XE_RAMHM_SET__
        LEAX    (0xE000-0x2000),X
#else
        LEAX    (0xA000-0x2000),X
#endif
        LDAB    #0x0F
        RTS                       ;/* returns 0b0000 1111 111 bit12...bit0 */

Below2000:
        LSLD                      ;/* D contains 0000 bit11..bit0, C contains bit12 */
        BCC     Below1000
        /* from 0x1000 to 0x1FFF */
        LDAB    __RPAGE_ADR__
        LDAA    #0x10
        MUL
        EORB    0,SP
        EORB    #0x10             ;/* clear 1 bit */
        STAB    0,SP
        TFR     A,B
        PULX
        RTS

Below1000:
        LSLD                      ;/* D contains 0000 0 bit10..bit0, C contains bit11 */
        BCC     Below0800
        /* from 0x0800 to 0x0FFF */
        LSLD                      ;/* D contains 0000 00 bit9..bit0, C contains bit10 */
        BCC     Below0C00
    /* from 0x0C00 to 0x0FFF */
        LDAB    #0x13
        PULX
        LEAX     0xF000,X
        RTS                       ;/* returns 0b0001 0011 1111 11 bit9...bit0 */
Below0C00:
    /* from 0x0800 to 0x0BFF */
        LDAB    __EPAGE_ADR__
        LDAA    #0x04
        MUL
        EORB    0,SP
        EORB    #0x08
        STAB    0,SP
        TFR     A,B
        ORAB    #0b00010000
        PULX
        RTS
Below0800:
        PULX
        CLRB
        RTS
  }
}

/*--------------------------- _CONV_STACK_NEAR_TO_GLOBAL --------------------------------
  Convert 16 bit logical of address on the stack 24 bit global pointer
    ("char*__near" to "char*__far")

  Arguments :
  - X : 16 bit near pointer

  Postcondition :
  - B == page of returned global address
  - X == offset of returned global address
  - Y remains unchanged
  - A is unspecified
*/
/*--------------------------- Convert 16 bit logical stack address to 24 bit global pointer ----------------------------------*/

/* B:X = Global(D) */

#ifdef __cplusplus
extern "C"
#endif

#pragma NO_FRAME
#pragma NO_ENTRY
#pragma NO_EXIT

void NEAR _CONV_STACK_NEAR_TO_GLOBAL(void){
  asm {
    /* syntax: */
    /*  input 16 bit offset is bit15..bit0 */
    /*  ppage values: ppage7..ppage0 */
    /*  epage values: epage7..epage0 */
    /*  dpage values: dpage7..dpage0 */
    /*  rpage values: rpage7..rpage0 */
    /* stack must be between $1000 and $3FFF. */
    /* actually placing the stack at $1000 implies that the RPAGE register is not set (and correctly initialized) */
        CPX     #0x2000
        BLO     PAGED_RAM
    /* Map 0x2000 to 0x0FE000 (0x0FA000 for HCS12XE RAM mapping is enabled) */
        LDAB    #0x0F
#ifndef __HCS12XE_RAMHM_SET__
        LEAX    (0xE000-0x2000),X ;/* LEAX is one cycle faster than ADDX # */
#else
        LEAX    (0xA000-0x2000),X ;/* LEAX is one cycle faster than ADDX # */
#endif
        RTS
PAGED_RAM:
        PSHX
        LDAB    __RPAGE_ADR__
        LDAA    #0x20
        MUL
        EORB    0,SP
        EORB    #0x10             ;/* clear 1 bit */
        STAB    0,SP
        TFR     A,B
        PULX
        RTS
  }
}



/*--------------------------- _CONV_LOGICAL_TO_GLOBAL --------------------------------
  Convert 24 bit global to 24 bit logical pointer
    ("char*__far" to "char*__gpage")

  Arguments :
  - B : page part of logical address
  - X : 16 offset part of logical address

  Postcondition :
  - B == page of returned global address
  - X == offset of returned global address
  - Y remains unchanged
  - A remains unchanged
*/
/*--------------------------- Convert 24 bit logical to 24 bit global pointer ----------------------------------*/

/* B:X = Logical(B:X) */

#ifdef __cplusplus
extern "C"
#endif

#pragma NO_FRAME
#pragma NO_ENTRY
#pragma NO_EXIT

void NEAR _CONV_LOGICAL_TO_GLOBAL(void) {

  asm {
        /* syntax: */
        /*  input 16 bit offset is bit15..bit0 */
        /*  ppage values: ppage7..ppage0 */
        /*  epage values: epage7..epage0 */
        /*  dpage values: dpage7..dpage0 */
        /*  rpage values: rpage7..rpage0 */
        PSHA                      ;/* save A across this routine. */
        PSHX                      ;/* D contains bit15..bit0 */
        PSHB                      ;/* store page */
        TFR     X,D               ;/* D is cheaper to shift */
        LSLD                      ;/* D contains 0 bit14..bit0, C contains bit15 */
        BCC     Below8000         ;/* bit15 == 0? */
    /* from 0x8000 to 0xFFFF */
        LSLD                      ;/* D contains 00 bit13..bit0, C contains bit14 */
        BCC     BelowC000
        PULB                      ;/* cleanup stack */
        LDAB    #0x7F
        PULX
        PULA
        RTS                       ;/* returns 0b0111 1111 11 bit13...bit0 */
BelowC000:                      ;/* from 0x8000 to 0xBFFF */
        TFR     D,X
        PULB                      ;/* cleanup stack */
        SEC
        RORB
        RORX
        LSRB
        RORX
        LEAS    2,SP
        PULA
        RTS                       ;/* returns 0b01 ppage7..ppage0 bit13...bit0 */
Below8000:
        LSLD                      ;/* D contains 00 bit13..bit0, C contains bit14 */
        BCC     Below4000
                                  ;/* from 0x4000 to 0x7FFF */
        PULB                      ;/* cleanup stack */
        PULX
#ifndef __HCS12XE_RAMHM_SET__
        LDAB    #0x7F
#else
        LEAX    (0xC000-0x4000),X
        LDAB    #0x0F
#endif
        PULA
        RTS                       ;/* returns 0b0111 1111 01 bit13...bit0 */

Below4000:
        LSLD                      ;/* D contains 000 bit12..bit0, C contains bit13 */
        BCC     Below2000
    /* from 0x2000 to 0x3FFF */
        PULB                      ;/* cleanup stack */
        PULX
#ifndef __HCS12XE_RAMHM_SET__
        LEAX    (0xE000-0x2000),X
#else
        LEAX    (0xA000-0x2000),X 
#endif
        LDAB    #0x0F
        PULA
        RTS                       ;/* returns 0b0000 1111 111 bit12...bit0 */

Below2000:
        LSLD                      ;/* D contains 0000 bit11..bit0, C contains bit12 */
        BCC     Below1000
    /* from 0x1000 to 0x1FFF */
        PULB
        LDAA    #0x10
        MUL
        EORB    0,SP
        EORB    #0x10             ;/* clear 1 bit */
        STAB    0,SP
        TFR     A,B
        PULX
        PULA
        RTS

Below1000:
        LSLD                      ;/* D contains 0000 0 bit10..bit0, C contains bit11 */
        BCC     Below0800
    /* from 0x0800 to 0x0FFF */
        LSLD                      ;/* D contains 0000 00 bit9..bit0, C contains bit10 */
        BCC     Below0C00
    /* from 0x0C00 to 0x0FFF */
        PULB                      ;/* cleanup stack */
        LDAB    #0x13
        PULX
        LEAX    0xF000,X
        PULA
        RTS                       ;/* returns 0b0001 0011 1111 11 bit9...bit0 */
Below0C00:
    /* from 0x0800 to 0x0BFF */
        PULB
        LDAA    #0x04
        MUL
        EORB    0,SP
        EORB    #0x08
        STAB    0,SP
        TFR     A,B
        ORAB    #0b00010000
        PULX
        PULA
        RTS
Below0800:
        PULB
        PULX
        PULA
        CLRB
        RTS
  }
}

/*--------------------------- _FAR_COPY_RC HCS12X Routines --------------------------------
  copy larger far memory blocks
  There are the following memory block copy routines:
      _COPY :                   16 bit logical copies.
                                  Src and dest are both near. Note: implemented in rtshc12.c and not here.
      _FAR_COPY_RC              HC12/HCS12 struct copy routine.
                                  Expects HC12/HCS12 logical 24 bit address.
                                  Note: Does not exist for the HCS12X.
                                  The HC12/HCS12 implementation is implemented above.
      _FAR_COPY_GLOBAL_GLOBAL_RC:
      _FAR_COPY_GLOBAL_LOGICAL_RC:
      _FAR_COPY_LOGICAL_GLOBAL_RC:
      _FAR_COPY_LOGICAL_LOGICAL_RC:
      _FAR_COPY_NEAR_GLOBAL_RC:
      _FAR_COPY_NEAR_LOGICAL_RC:
      _FAR_COPY_GLOBAL_NEAR_RC:
      _FAR_COPY_LOGICAL_NEAR_RC:  HCS12X specific far copy routine. The name describes what the src/dest address format are.
                                    All near src arguments are passed in X, all 24 bit src in X/B.
                                    All near dest arguments are passed in Y, all 24 bit src in Y/A.
                                    (Note: HC12 _FAR_COPY_RC is using X/A as src and Y/B as dest, so the register usage is not the same!)

  Arguments :
  - B:X : src address (for NEAR/_COPY: only X)
  - A:Y : dest address (for NEAR/_COPY: only Y)
  - number of bytes to be copied behind return address (for _COPY: in D register). The number of bytes is always > 0

  Result :
  - memory area copied
  - no registers are saved, i.e. all registers may be destroyed
  - for _COPY: D contains 0.
  - for HCS12X _FAR_COPY_... routines: GPAGE state is unknown
*/


#ifdef __cplusplus
extern "C"
#endif
#pragma NO_ENTRY
#pragma NO_EXIT
#pragma NO_FRAME

void NEAR _FAR_COPY_GLOBAL_GLOBAL_RC(void) {
  asm {
        PSHD
        PSHY
        LDY     4,SP              ;/* load return address */
        LDD     2,Y+              ;/* load size */
        STY     4,SP              ;/* store return address */
        PULY
        PSHD
        LDAB    3,SP
Loop:
        STAB    __GPAGE_ADR__
        GLDAA   1,X+
        MOVB    2,SP,__GPAGE_ADR__
        GSTAA   1,Y+
        DECW    0,SP
        BNE     Loop
        LEAS    4,SP
        _SRET                     ;/* debug info only: This is the last instr of a function with a special return */
        RTS
  }
}

#ifdef __cplusplus
extern "C"
#endif
#pragma NO_ENTRY
#pragma NO_EXIT
#pragma NO_FRAME

void NEAR _SET_PAGE_REG_HCS12X(void) {
  /* Sets the page contained in A to the register controlling the logical addr contained in X. */
  /* saves the old page before and returns it in A together with the page address just below the return address. */
  /* X/Y both remain valid. */
  asm {
        PSHX
        /* 0000..FFFF */
        CPX     #0x8000
        BLO     _LO8000
        LDX     #__PPAGE_ADR__
        BRA      Handle
_LO8000:
        /* 0000..7FFF */
        CPX     #0x1000
        BLO     _LO1000
        LDX     #__RPAGE_ADR__
        BRA      Handle
_LO1000:
        LDX     #__EPAGE_ADR__
Handle:
        LDAA    0,X               ;/* load old page register content */
        STAB    0,X               ;/* set new page register */
        STX     4,SP
        PULX
        RTS
  }
}


#ifdef __cplusplus
extern "C"
#endif
#pragma NO_ENTRY
#pragma NO_EXIT
#pragma NO_FRAME

void NEAR _FAR_COPY_GLOBAL_LOGICAL_RC(void) {
  asm {
        STAB    __GPAGE_ADR__
        EXG     X,Y
        TFR     A,B
        PSHY                      ;/* space to store size */
        PSHX                      ;/* allocate some space where _SET_PAGE_REG_HCS12X can return the page */
        LDY     4,SP              ;/* load return address */
        LDX     2,Y+              ;/* load size */
        STY     4,SP
        LDY     2,SP              ;/* restore dest pointer */
        STX     2,SP              ;/* store size */
        LDX     0,SP              ;/* reload src pointer */
        __PIC_JSR(_SET_PAGE_REG_HCS12X)

Loop:   GLDAB   1,Y+
        STAB    1,X+
        DECW    2,SP
        BNE     Loop

        PULX                      ;/* reload page register address */
        STAA    0,X               ;/* restore old page content (necessary if it was PPAGE) */
        PULX                      ;/* clean up stack */
        _SRET                     ;/* debug info only: This is the last instr of a function with a special return */
        RTS
  }
}

#ifdef __cplusplus
extern "C"
#endif
#pragma NO_ENTRY
#pragma NO_EXIT
#pragma NO_FRAME

void NEAR _FAR_COPY_LOGICAL_GLOBAL_RC(void) {
  asm {
        STAA    __GPAGE_ADR__
        PSHY                      ;/* space to store size */
        PSHX                      ;/* allocate some space where _SET_PAGE_REG_HCS12X can return the page */
        LDY     4,SP              ;/* load return address */
        LDX     2,Y+              ;/* load size */
        STY     4,SP
        LDY     2,SP              ;/* restore dest pointer */
        STX     2,SP              ;/* store size */
        LDX     0,SP              ;/* reload src pointer */

        __PIC_JSR(_SET_PAGE_REG_HCS12X)

Loop:   LDAB    1,X+
        GSTAB   1,Y+
        DECW    2,SP
        BNE     Loop

        PULX
        STAA    0,X               ;/* restore old page content (necessary if it was PPAGE) */
        PULX                      ;/* clean up stack */
        _SRET                     ;/* debug info only: This is the last instr of a function with a special return */
        RTS
  }
}

#ifdef __cplusplus
extern "C"
#endif
#pragma NO_ENTRY
#pragma NO_EXIT
#pragma NO_FRAME

void NEAR _FAR_COPY_LOGICAL_LOGICAL_RC(void) {
  asm {
        PSHA
        __PIC_JSR(_CONV_LOGICAL_TO_GLOBAL);
        PULA
        __PIC_JMP(_FAR_COPY_GLOBAL_LOGICAL_RC);
  }
}

#ifdef __cplusplus
extern "C"
#endif
#pragma NO_ENTRY
#pragma NO_EXIT
#pragma NO_FRAME

void NEAR _FAR_COPY_NEAR_GLOBAL_RC(void) {
  asm {
        CLRB
        __PIC_JMP(_FAR_COPY_LOGICAL_GLOBAL_RC);
  }
}

#ifdef __cplusplus
extern "C"
#endif
#pragma NO_ENTRY
#pragma NO_EXIT
#pragma NO_FRAME

void NEAR _FAR_COPY_NEAR_LOGICAL_RC(void) {
  asm {
        PSHA
        __PIC_JSR(_CONV_NEAR_TO_GLOBAL);
        PULA
        __PIC_JMP(_FAR_COPY_GLOBAL_LOGICAL_RC);
  }
}

#ifdef __cplusplus
extern "C"
#endif
#pragma NO_ENTRY
#pragma NO_EXIT
#pragma NO_FRAME

void NEAR _FAR_COPY_GLOBAL_NEAR_RC(void) {
  asm {
        CLRA                      /* near to logical (we may have to use another runtime if this gets non trivial as well :-( */
        __PIC_JMP(_FAR_COPY_GLOBAL_LOGICAL_RC);
  }
}

#ifdef __cplusplus
extern "C"
#endif
#pragma NO_ENTRY
#pragma NO_EXIT
#pragma NO_FRAME

void NEAR _FAR_COPY_LOGICAL_NEAR_RC(void) {
  asm {
        EXG     A,B
        EXG     X,Y
        PSHA
        __PIC_JSR(_CONV_NEAR_TO_GLOBAL);
        PULA
        EXG     A,B
        EXG     X,Y
        __PIC_JMP(_FAR_COPY_LOGICAL_GLOBAL_RC);
  }
}

/* _FAR_COPY_LOGICAL_GLOBAL: is used by some old wizard generated projects. Not used by current setup anymore */

#ifdef __cplusplus
extern "C"
#endif
#pragma NO_ENTRY
#pragma NO_EXIT
#pragma NO_FRAME

void NEAR _FAR_COPY_LOGICAL_GLOBAL(void) {
  asm {
        STAA    __GPAGE_ADR__
        PSHX                      ;/* allocate some space where _SET_PAGE_REG_HCS12X can return the page */
        __PIC_JSR(_SET_PAGE_REG_HCS12X)

Loop:   LDAB    1,X+
        GSTAB   1,Y+
        DECW    4,SP
        BNE     Loop

        PULX
        STAA    0,X               ;/* restore old page content (necessary if it was PPAGE) */

        LDX     4,SP+             ;/* load return address and clean stack */
        JMP     0,X
  }
}


#endif /* __HCS12X__  */


/*----------------- end of code ------------------------------------------------*/
/*lint --e{766} , runtime.sgm is not a regular header file, it contains a conditionally compiled CODE_SEG pragma */
