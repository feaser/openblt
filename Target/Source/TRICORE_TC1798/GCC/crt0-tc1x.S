/*
 * crt0-tc1x.S -- Startup code for GNU/TriCore applications.
 *
 *  Copyright (C) 1998-2012 HighTec EDV-Systeme GmbH.
 * 
 * This file is part of GCC.
 *
 * GCC is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 * 
 * GCC is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * Under Section 7 of GPL version 3, you are granted additional
 * permissions described in the GCC Runtime Library Exception, version
 * 3.1, as published by the Free Software Foundation.
 *  
 * You should have received a copy of the GNU General Public License and
 * a copy of the GCC Runtime Library Exception along with this program;
 * see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see
 * <http://www.gnu.org/licenses/>.  */

#ifndef __TRICORE_NAME__
#error Please define __TRICOR_NAME__
#endif    

/* define the Derivate Name as a hexvalue 
 * this name is defined in tricore.specs for specific derivate to 
 * the derivate number as a hexvalue (e.g. TC1796 => 0x1796
 * This name will be used in the memory.x Memory description to
 * to confirm that the crt0.o and the memory.x will be get from 
 * same directory
 */ 
.global __TRICORE_DERIVATE_NAME__
.type __TRICORE_DERIVATE_NAME__,@object
.set __TRICORE_DERIVATE_NAME__,__TRICORE_NAME__

	.section ".startup_code", "awx", @progbits
	.global _start
	.type _start,@function
_start:
	.code32
	j	0f

    	/*
	 * external boot memory configuration word
	 */
	.word __EBMCFG
	.word __EBMCFG
	.word __EBMCFG
	.word __EBMCFG

	/*
	 * default value for this configuration word
	 */
	.global __EBMCFG
	.weak __EBMCFG
	__EBMCFG = 0x0000808c
0:

#;	check whether WDTCON0.ENDINIT is set
#;	clear it if necessary
#;	initialization code must write ENDINIT protected registers

	ld.w	%d1,$wdtcon0			# %d1 = *WDTCON0
	jz.t	%d1,0,endinit_done
	jl	asm_clear_endinit
endinit_done:

	/*
	 * initialize user and interrupt stack pointers
	 */

	movh.a	%sp,hi:__USTACK			# load %sp
	lea	%sp,[%sp]lo:__USTACK
	movh	%d0,hi:__ISTACK			# load $isp
	addi	%d0,%d0,lo:__ISTACK
	mtcr	$isp,%d0
	isync

#;	install trap handlers

	movh	%d0,hi:first_trap_table		#; load $btv
	addi	%d0,%d0,lo:first_trap_table
	mtcr	$btv,%d0
	isync

	/*
	 * initialize call depth counter
	 */

	mfcr	%d0,$psw
#ifdef MAX_CALLDEPTH_64
	andn	%d0,%d0,0x7f			# reset call depth counter
	or	%d0,%d0,0x80			# set CDE bit
#else
	or	%d0,%d0,0x7f			# disable call depth counting
	andn	%d0,%d0,0x80			# clear CDE bit
#endif
	mtcr	$psw,%d0
	isync

	/*
	 * initialize access to system global registers
	 */

	mfcr	%d0,$psw
	or	%d0,%d0,0x100			# set GW bit
	mtcr	$psw,%d0
 	isync

	/*
	 * initialize SDA base pointers
	 */
	.global _SMALL_DATA_,_SMALL_DATA2_,_SMALL_DATA3_,_SMALL_DATA4_
	.weak _SMALL_DATA_,_SMALL_DATA2_,_SMALL_DATA3_,_SMALL_DATA4_

	movh.a	%a0,hi:_SMALL_DATA_		# %a0 addresses .sdata/.sbss
	lea	%a0,[%a0]lo:_SMALL_DATA_
	movh.a	%a1,hi:_SMALL_DATA2_		# %a1 addresses .sdata2/.sbss2
	lea	%a1,[%a1]lo:_SMALL_DATA2_
	movh.a	%a8,hi:_SMALL_DATA3_		# %a8 addresses .sdata3/.sbss3
	lea	%a8,[%a8]lo:_SMALL_DATA3_
	movh.a	%a9,hi:_SMALL_DATA4_		# %a9 addresses .sdata4/.sbss4
	lea	%a9,[%a9]lo:_SMALL_DATA4_


#ifdef READONLY_SYSREGS
	mfcr	%d0,$psw
	andn	%d0,%d0,0x100			# clear GW bit
	mtcr	$psw,%d0
 	isync
#endif


/*
 *	initialize target environment (PLLCLC, BUSCONx, ADDSELx etc)
 *
 *	This is done by a user supplied assembler function __board_init
 */
#;	force PC to remapped ROM address
	movh.a	%a15,hi:__remapped
	lea	%a15,[%a15]lo:__remapped
	nop
	ji	%a15

__remapped:
	# call _board_init
	jl		__board_init

__no_board_init:

	/*
	 * disable code and data protection
	 */

#ifdef DISABLE_CODE_PROTECTION
	mfcr	%d0,$cpm0_0			# disable code protection
	andn	%d0,%d0,0xff
	mtcr	$cpm0_0,%d0
	isync
#endif
#ifdef DISABLE_DATA_PROTECTION
	mfcr	%d0,$dpm0_0			# disable data protection
	andn	%d0,%d0,0xff
	mtcr	$dpm0_0,%d0
	isync
#endif

	/*
	 * initialize software break service (OCDS)
	 */

#ifdef INIT_SBS
	SRCSB0	= 0xfffeffbc
	SWBIPL	= 31

	mov.u	%d0,lo:(SWBIPL + 0x00001000)
	addih	%d0,%d0,hi:(SWBIPL + 0x00001000)
	movh.a	%a5,hi:SRCSB0
	lea	%a5,[%a5]lo:SRCSB0
	st.w	[%a5],%d0
#endif

	/*
	 * disable watchdog timer and set ENDINIT bit
	 */

	jl	_disable_wdt



	/*
	 * initialize context save areas
	 */

	jl	__init_csa



	/*
	 * handle clear table (i.e., fill BSS with zeros)
	 */

	jl	__clear_table_func


	/*
	 * handle copy table (support for romable code)
	 */

	jl	__copy_table_func


	/*
	 * call the initializer, constructors etc.
	*/
	call	_init

	/*
	 * _exit (main (0, NULL));
	 */
    mov     %d4,0               # argc = 0
    sub.a   %sp,8
    st.w    [%sp]0, %d4
    st.w    [%sp]4, %d4
    mov.aa  %a4, %sp            # argv

	call	main		    # int retval = main (0, NULL);
	mov	%d4,%d2
	lea	%sp,[%sp]8	    # remove argv[0]
	mov.u	%d1, 0x900d	    # set exit code i(A14) for the simulator to 
	mov	%d15, %d2	    # 0x900d if the exit status is 0
        movh    %d3, 0xffff
        or      %d2, %d2, %d3
	cmov	%d1,%d15,%d2 
	mov.a	%a14, %d1
	j	_exit		    # _exit (retval);

	debug			    # should never come here



	/*
	 * disable watchdog timer and set ENDINIT bit
	 */

	WDTMASK	= 0xffffff03
	WDTMASKA= 0xffffff01
	WDTMASK2= 0xfffffff0

	.globl	_disable_wdt
	.type _disable_wdt,function

_disable_wdt:					# lock WDT via passwd access
	movh	%d2,hi:WDTMASK
	addi	%d2,%d2,lo:WDTMASK		# %d2 = 0xffffff03
	movh	%d5,hi:WDTMASK2			# %d5 = 0xfffffff0
	addi	%d5,%d5,lo:WDTMASK2
	mov	%d6,0x8				# %d6 = "disable WDT"
	st.w	$wdtcon1,%d6			# WDTCON1.WDTDR = 1

	ld.w	%d0,$wdtcon0			# %d0 = *WDTCON0
	ld.w	%d1,$wdtcon1			# %d1 = *WDTCON1

	and	%d3,%d2,%d0			# %d3 = WDTCON0 & 0xffffff03
	or	%d3,%d3,0xf0			# %d3 |= 0xf0
	and	%d4,%d1,0xc
	or	%d3,%d3,%d4			# %d3 |= (WDTCON1 & 0xc)
	xor	%d3,%d3,0x2			# %d3 ^= 0x2
	st.w	$wdtcon0,%d3			# WDTCON = %d3
	and	%d3,%d3,%d5			# %d3 &= 0xfffffff0
	or	%d3,%d3,0x3			# WDTCON0.{WDTLCK,ENDINIT} = 1
	st.w	$wdtcon0,%d3			# write back new value
	ld.w	%d0,$wdtcon0			# ensure that value is written

	ji	%a11



	/*
	 * initialize context save areas (CSAs), PCXI, LCX and FCX
	 */

	.globl	__init_csa
	.type __init_csa,function

__init_csa:
	movh	%d0,0
	mtcr	$pcxi,%d0
	isync
	movh	%d0,hi:__CSA_BEGIN		#; %d0 = begin of CSA
	addi	%d0,%d0,lo:__CSA_BEGIN
	addi	%d0,%d0,63			#; force alignment (2^6)
	andn	%d0,%d0,63
	movh	%d2,hi:__CSA_END		#; %d2 = end of CSA
	addi	%d2,%d2,lo:__CSA_END
	andn	%d2,%d2,63			#; force alignment (2^6)
	sub	%d2,%d2,%d0
	sh	%d2,%d2,-6			#; %d2 = number of CSAs
	mov.a	%a3,%d0				#; %a3 = address of first CSA
	extr.u	%d0,%d0,28,4			#; %d0 = segment << 16
	sh	%d0,%d0,16
	lea	%a4,0				#; %a4 = previous CSA = 0
	st.a	[%a3], %a4			#; store it in 1st CSA
        mov.aa  %a4,%a3				#; %a4 = current CSA
        lea     %a3,[%a3]64			#; %a3 = %a3->nextCSA
	mov.d	%d1,%a3
	extr.u	%d1,%d1,6,16			#; get CSA index
	or	%d1,%d1,%d0			#; add segment number
	mtcr	$lcx,%d1			#; initialize LCX
	add	%d2,%d2,-2			#; CSAs to initialize -= 2
	mov.a	%a5, %d2			#; %a5 = loop counter
csa_loop:
	mov.d	%d1,%a4				#; %d1 = current CSA address
	extr.u	%d1,%d1,6,16			#; get CSA index
	or	%d1,%d1,%d0			#; add segment number
	st.w	[%a3],%d1			#; store "nextCSA" pointer
        mov.aa  %a4,%a3				#; %a4 = current CSA address
        lea     %a3,[%a3]64			#; %a3 = %a3->nextCSA
	loop	%a5, csa_loop			#; repeat until done

	mov.d	%d1,%a4				#; %d1 = current CSA address
	extr.u	%d1,%d1,6,16			#; get CSA index
	or	%d1,%d1,%d0			#; add segment number
	mtcr	$fcx,%d1			#; initialize FCX
	isync
	ji	%a11



	.global asm_clear_endinit
	.type asm_clear_endinit,@function

asm_clear_endinit:
	ld.w	%d0,$wdtcon0			# %d0 = *WDTCON0
	ld.w	%d1,$wdtcon1			# %d1 = *WDTCON1

	movh	%d2,hi:WDTMASKA
	addi	%d2,%d2,lo:WDTMASKA		# %d2 = 0xffffff01

	and	%d0,%d2,%d0
	or	%d0,%d0,240
	and	%d4,%d1,12
	or	%d0,%d4,%d0
	st.w	$wdtcon0,%d0

	movh	%d4,hi:WDTMASK2			# %d4 = 0xfffffff0
	addi	%d4,%d4,lo:WDTMASK2
	and	%d0,%d4,%d0
	or	%d0,%d0,2			# ENDINIT = 0
	isync
	st.w	$wdtcon0,%d0
	ld.w	%d0,$wdtcon0			# ensure that value is written

	ji	%a11



	/*
	 * handle clear table (i.e., fill BSS with zeros)
	 */
	.global __clear_table_func
	.type __clear_table_func,@function

__clear_table_func:
	mov	%d14,0				# %e14 = 0
	mov	%d15,0
	movh.a	%a13,hi:__clear_table		# %a13 = &first table entry
	lea	%a13,[%a13]lo:__clear_table

__clear_table_next:
	ld.a	%a15,[%a13+]4			# %a15 = current block base
	ld.w	%d3,[%a13+]4			# %d3 = current block length
	jeq	%d3,-1,__clear_table_done	# length == -1 => end of table
	sh	%d0,%d3,-3			# %d0 = length / 8 (doublewords)
	and	%d1,%d3,7			# %d1 = length % 8 (rem. bytes)
	jz	%d0,__clear_word		# block size < 8 => clear word
	addi	%d0,%d0,-1			# else doublewords -= 1
	mov.a	%a2,%d0				# %a2 = loop counter
__clear_dword:
	st.d 	[%a15+]8,%e14			# clear one doubleword
	loop	%a2,__clear_dword
__clear_word:
	jz	%d1,__clear_table_next
	sh	%d0,%d1,-2			# %d0 = length / 4 (words)
	and	%d1,%d1,3			# %d1 = length % 4 (rem. bytes)
	jz	%d0,__clear_hword		# block size < 4 => clear hword
	st.w	[%a15+]4,%d15			# clear one word
__clear_hword:
	jz	%d1,__clear_table_next
	sh	%d0,%d1,-1			# %d0 = length / 2 (halfwords)
	and	%d1,%d1,1			# %d1 = length % 2 (rem. bytes)
	jz	%d0,__clear_byte		# block size < 2 => clear byte
	st.h	[%a15+]2,%d15			# clear one halfword
__clear_byte:
	jz	%d1,__clear_table_next
	st.b	[%a15],%d15			# clear one byte
	j	__clear_table_next		# handle next clear table entry
__clear_table_done:

	ji	%a11



	/*
	 * handle copy table (support for romable code)
	 */
	.global __copy_table_func
	.type __copy_table_func,@function

__copy_table_func:
	movh.a	%a13,hi:__copy_table		# %a13 = &first table entry
	lea	%a13,[%a13]lo:__copy_table

__copy_table_next:
	ld.a	%a15,[%a13+]4			# %a15 = src address
	ld.a	%a14,[%a13+]4			# %a14 = dst address
	ld.w	%d3,[%a13+]4			# %d3 = block length
	jeq	%d3,-1,__copy_table_done	# length == -1 => end of table
	sh	%d0,%d3,-3			# %d0 = length / 8 (doublewords)
	and	%d1,%d3,7			# %d1 = lenght % 8 (rem. bytes)
	jz	%d0,__copy_word			# block size < 8 => copy word
	addi	%d0,%d0,-1			# else doublewords -= 1
	mov.a	%a2,%d0				# %a2 = loop counter
__copy_dword:
	ld.d	%e14,[%a15+]8			# copy one doubleword
	st.d 	[%a14+]8,%e14
	loop	%a2,__copy_dword
__copy_word:
	jz	%d1,__copy_table_next
	sh	%d0,%d1,-2			# %d0 = length / 4 (words)
	and	%d1,%d1,3			# %d1 = lenght % 4 (rem. bytes)
	jz	%d0,__copy_hword		# block size < 4 => copy hword
	ld.w	%d14,[%a15+]4			# copy one word
	st.w	[%a14+]4,%d14
__copy_hword:
	jz	%d1,__copy_table_next
	sh	%d0,%d1,-1			# %d0 = length / 2 (halfwords)
	and	%d1,%d1,1			# %d1 = lenght % 2 (rem. bytes)
	jz	%d0,__copy_byte			# block size < 2 => copy byte
	ld.h	%d14,[%a15+]2			# copy one halfword
	st.h	[%a14+]2,%d14
__copy_byte:
	jz	%d1,__copy_table_next
	ld.b	%d14,[%a15]0			# copy one byte
	st.b	[%a14],%d14
	j	__copy_table_next		# handle next copy table entry
__copy_table_done:

	ji	%a11


/*
 *	dummy board initilization function
	.global __board_init
	.weak __board_init

__board_init:
	ji	%a11
 */

	.global boardSetupTabSize
	.weak boardSetupTabSize
	.global boardSetupTab
	.weak boardSetupTab
boardSetupTab:
boardSetupTabSize:
	

	.global __board_init
	.type __board_init,@function
__board_init:
	.code32

#;
#;	initialize target environment (PLLCLC, BUSCONx, ADDSELx)
#;
#;	this is done by board specific setup table (address/value - pairs)
#;

	movh.a	%a15,hi:boardSetupTabSize
	lea	%a15,[%a15]lo:boardSetupTabSize	# %a14 address of table size
	movh.a	%a14,hi:boardSetupTab
	lea	%a14,[%a14]lo:boardSetupTab	# %a14 address of setup table
	jeq.a	%a14, %a15, no_setup

	ld.a	%a15,[%a15]0			# %a15 = table size
	jz.a	%a15,no_setup
	add.a	%a15,-1				# correction for loop
setup_loop:
	ld.a	%a2,[%a14+]			# %a2 = boardSetupTab.addr
	ld.w	%d2,[%a14+]			# %d2 = boardSetupTab.val
	st.w	[%a2],%d2
	loop	%a15,setup_loop

 	isync
 	nop
 	nop

no_setup:

	ji	%a11

/*============================================================================*
 * Exception handlers (exceptions in startup code)
 *
 * This is a minimal trap vector table, which consists of eight
 * entries, each consisting of eight words (32 bytes).
 *============================================================================*/


#;	.section .traptab, "awx", @progbits

.macro trapentry from=0, to=7
    debug
    mov.u   %d14, \from << 8
    add     %d14, %d14, %d15
    mov.a   %a14, %d14
    addih.a %a14, %a14,0xdead
    j       _exit
    0: j 0b
    nop
    rfe
	.align 5
    .if \to-\from
    trapentry "(\from+1)",\to
    .endif
    .endm

	.align 8
	.globl first_trap_table
first_trap_table:
    trapentry 0, 7


