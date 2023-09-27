/**
 * \file Lcf_Gnuc_Tricore_Tc.lsl
 * \brief Linker command file for Gnuc compiler.
 *
 * \copyright Copyright (c) 2018 Infineon Technologies AG. All rights reserved.
 *
 *
 *
 *                                 IMPORTANT NOTICE
 *
 *
 * Infineon Technologies AG (Infineon) is supplying this file for use
 * exclusively with Infineon's microcontroller products. This file can be freely
 * distributed within development tools that are supporting such microcontroller
 * products.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 * OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 * INFINEON SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 * OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 */
 
OUTPUT_FORMAT("elf32-tricore")
OUTPUT_ARCH(tricore)
ENTRY(_START)

__TRICORE_DERIVATE_MEMORY_MAP__ = 0x270;

LCF_CSA0_SIZE =		8k;
LCF_USTACK0_SIZE =	2k;
LCF_ISTACK0_SIZE =	1k;

LCF_CSA1_SIZE =		8k;
LCF_USTACK1_SIZE =	2k;
LCF_ISTACK1_SIZE =	1k;

LCF_CSA2_SIZE =		8k;
LCF_USTACK2_SIZE =	2k;
LCF_ISTACK2_SIZE =	1k;

LCF_HEAP_SIZE =		4k;

LCF_DSPR2_START =	0x50000000;
LCF_DSPR2_SIZE =	120k;

LCF_DSPR1_START =	0x60000000;
LCF_DSPR1_SIZE =	120k;

LCF_DSPR0_START =	0x70000000;
LCF_DSPR0_SIZE =	112k;

LCF_CSA2_OFFSET	=	(LCF_DSPR2_SIZE - 1k - LCF_CSA2_SIZE);
LCF_ISTACK2_OFFSET =	(LCF_CSA2_OFFSET - 256 - LCF_ISTACK2_SIZE);
LCF_USTACK2_OFFSET =	(LCF_ISTACK2_OFFSET - 256 - LCF_USTACK2_SIZE);

LCF_CSA1_OFFSET	=	(LCF_DSPR1_SIZE - 1k - LCF_CSA1_SIZE);
LCF_ISTACK1_OFFSET =	(LCF_CSA1_OFFSET - 256 - LCF_ISTACK1_SIZE);
LCF_USTACK1_OFFSET =	(LCF_ISTACK1_OFFSET - 256 - LCF_USTACK1_SIZE);

LCF_CSA0_OFFSET	=	(LCF_DSPR0_SIZE - 1k - LCF_CSA0_SIZE);
LCF_ISTACK0_OFFSET =	(LCF_CSA0_OFFSET - 256 - LCF_ISTACK0_SIZE);
LCF_USTACK0_OFFSET =	(LCF_ISTACK0_OFFSET - 256 - LCF_USTACK0_SIZE);

LCF_HEAP0_OFFSET =	(LCF_USTACK0_OFFSET - LCF_HEAP_SIZE);
LCF_HEAP1_OFFSET =	(LCF_USTACK1_OFFSET - LCF_HEAP_SIZE);
LCF_HEAP2_OFFSET =	(LCF_USTACK2_OFFSET - LCF_HEAP_SIZE);

LCF_INTVEC0_START =	0x801F4000;
LCF_TRAPVEC0_START =	0x80000100;
LCF_TRAPVEC1_START =	0x801F6200;
LCF_TRAPVEC2_START =	0x801F6100;

RESET =			0x80000020;

MEMORY
{
	dsram2_local (w!xp): org = 0xd0000000, len = 120K
	dsram2 (w!xp): org = 0x50000000, len = 120K
	psram2 (w!xp): org = 0x50100000, len = 24K
	
	
	dsram1_local (w!xp): org = 0xd0000000, len = 120K
	dsram1 (w!xp): org = 0x60000000, len = 120K
	psram1 (w!xp): org = 0x60100000, len = 24K
	
	
	dsram0_local (w!xp): org = 0xd0000000, len = 112K
	dsram0 (w!xp): org = 0x70000000, len = 112K
	psram0 (w!xp): org = 0x70100000, len = 24K
	
	psram_local (w!xp): org = 0xc0000000, len = 24K
	
	pfls0 (rx!p): org = 0x80000000, len = 2M
	pfls0_nc (rx!p): org = 0xa0000000, len = 2M
	
	pfls1 (rx!p): org = 0x80200000, len = 2M        /*Not used to allocate and sections*/
	pfls1_nc (rx!p): org = 0xa0200000, len = 2M     /*Not used to allocate and sections*/
	
	dfls0 (rx!p): org = 0xaf000000, len = 384K
	
	lmuram (w!xp): org = 0x90000000, len = 32K
	lmuram_nc (w!xp): org = 0xb0000000, len = 32K
	
	edmem (w!xp): org = 0x9f000000, len = 1M
	edmem_nc (w!xp): org = 0xbf000000, len = 1M
}	

/* map local memory address to a global address */ 
REGION_MAP( CPU0 , ORIGIN(dsram0_local), LENGTH(dsram0_local), ORIGIN(dsram0))
REGION_MAP( CPU1 , ORIGIN(dsram1_local), LENGTH(dsram1_local), ORIGIN(dsram1))
REGION_MAP( CPU2 , ORIGIN(dsram2_local), LENGTH(dsram2_local), ORIGIN(dsram2))

/*Un comment one of the below statements to enable CpuX DMI RAM to hold global variables*/
/*REGION_ALIAS( default_ram , dsram0)*/
REGION_ALIAS( default_ram , dsram1)
/*REGION_ALIAS( default_ram , dsram2)*/

CORE_ID = GLOBAL ;

SECTIONS
{
	/*This section is always required as Boot mode header 0 address absolutely restricted at address 0x80000000*/
	.bmhd_0 (0x80000000) : FLAGS(arl)
	{
		BootModeHeader0 = .;
		KEEP (*(.bmhd_0))
	} > pfls0
	
	/*This section is always required as Boot mode header 1 address absolutely restricted at address 0x80020000*/
	.bmhd_1 (0x80020000) : FLAGS(arl)
	{
		BootModeIndex = .;
		KEEP (*(.bmhd_1));
	} > pfls0
	
	/*This section is always required as user start address absolutely restricted at address 0x80000020*/
	.startup (0x80000020) : FLAGS(rxl)
	{
		BootModeIndex = .;
		. = ALIGN(4);
		KEEP (*(.start));
		. = ALIGN(4);
	} > pfls0 =0x800

	/*This section contains the data indirection pointers to interface external devices*/
	.interface_const (0x80000040) :
	{
		__IF_CONST = .;
		KEEP (*(.interface_const));
		. = ALIGN(4);
	} > pfls0
	 
	.traptab_tc0 (LCF_TRAPVEC0_START) :
	{
		PROVIDE(__TRAPTAB_CPU0 = .);
		KEEP (*(.traptab_cpu0));
	} > pfls0

	.zrodata : FLAGS(arzl)
	{
		*(.zrodata)
		*(.zrodata.*)
	} > pfls0
	
	.sdata2 : FLAGS(arsl)
	{
		*(.srodata)
		*(.srodata.*)
	} > pfls0
	
	_SMALL_DATA2_ = SIZEOF(CORE_SEC(.sdata2)) ? ADDR(CORE_SEC(.sdata2)) + 32k : (ADDR(CORE_SEC(.sdata2)) & 0xF0000000) + 32k ;
	__A1_MEM = _SMALL_DATA2_;
	  	
	.rodata : FLAGS(arl)
	{
		*(.rodata)
	*(.rodata.*)
	*(.gnu.linkonce.r.*)
	/*
	 * Create the clear and copy tables that tell the startup code
	 * which memory areas to clear and to copy, respectively.
	 */
	. = ALIGN(4) ;
	PROVIDE(__clear_table = .);
	LONG(0 + ADDR(.CPU2.zbss));     LONG(SIZEOF(.CPU2.zbss));
	LONG(0 + ADDR(.CPU2.bss));    LONG(SIZEOF(.CPU2.bss));
	LONG(0 + ADDR(.CPU1.zbss));     LONG(SIZEOF(.CPU1.zbss));
	LONG(0 + ADDR(.CPU1.bss));    LONG(SIZEOF(.CPU1.bss));
	LONG(0 + ADDR(.CPU0.zbss));     LONG(SIZEOF(.CPU0.zbss));
	LONG(0 + ADDR(.CPU0.bss));    LONG(SIZEOF(.CPU0.bss));
	LONG(0 + ADDR(.zbss));     LONG(SIZEOF(.zbss));
	LONG(0 + ADDR(.sbss));     LONG(SIZEOF(.sbss));
	LONG(0 + ADDR(.bss));    LONG(SIZEOF(.bss));
	LONG(0 + ADDR(.lmu_zbss));    LONG(SIZEOF(.lmu_zbss));
	LONG(0 + ADDR(.lmu_sbss));    LONG(SIZEOF(.lmu_sbss));
	LONG(0 + ADDR(.lmu_bss));    LONG(SIZEOF(.lmu_bss));
	LONG(-1);                 LONG(-1);
	PROVIDE(__copy_table = .) ;
	LONG(LOADADDR(.CPU2.zdata));    LONG(0 + ADDR(.CPU2.zdata));    LONG(SIZEOF(.CPU2.zdata));
	LONG(LOADADDR(.CPU2.data));    LONG(0 + ADDR(.CPU2.data));    LONG(SIZEOF(.CPU2.data));
	LONG(LOADADDR(.CPU1.zdata));    LONG(0 + ADDR(.CPU1.zdata));    LONG(SIZEOF(.CPU1.zdata));
	LONG(LOADADDR(.CPU1.data));    LONG(0 + ADDR(.CPU1.data));    LONG(SIZEOF(.CPU1.data));
	LONG(LOADADDR(.CPU0.zdata));    LONG(0 + ADDR(.CPU0.zdata));    LONG(SIZEOF(.CPU0.zdata));
	LONG(LOADADDR(.CPU0.data));    LONG(0 + ADDR(.CPU0.data));    LONG(SIZEOF(.CPU0.data));
	LONG(LOADADDR(.zdata));    LONG(0 + ADDR(.zdata));    LONG(SIZEOF(.zdata));
	LONG(LOADADDR(.sdata));    LONG(0 + ADDR(.sdata));    LONG(SIZEOF(.sdata));
	LONG(LOADADDR(.data));    LONG(0 + ADDR(.data));    LONG(SIZEOF(.data));
	LONG(LOADADDR(.lmu_zdata));    LONG(0 + ADDR(.lmu_zdata));    LONG(SIZEOF(.lmu_zdata));
	LONG(LOADADDR(.lmu_sdata));    LONG(0 + ADDR(.lmu_sdata));    LONG(SIZEOF(.lmu_sdata));
	LONG(LOADADDR(.lmu_data));    LONG(0 + ADDR(.lmu_data));    LONG(SIZEOF(.lmu_data));
	LONG(LOADADDR(.CPU0.psram_text));    LONG(0 + ADDR(.CPU0.psram_text));    LONG(SIZEOF(.CPU0.psram_text));
	LONG(LOADADDR(.CPU1.psram_text));    LONG(0 + ADDR(.CPU1.psram_text));    LONG(SIZEOF(.CPU1.psram_text));
	LONG(LOADADDR(.CPU2.psram_text));    LONG(0 + ADDR(.CPU2.psram_text));    LONG(SIZEOF(.CPU2.psram_text));
	LONG(-1);                 LONG(-1);                 LONG(-1);
	. = ALIGN(8);
	} > pfls0
	
	.text  : FLAGS(axl)
	{
		*(.text)
		*(.text.*)
		*(.gnu.linkonce.t.*)
	    *(.gnu.warning)        /* .gnu.warning sections are handled specially by elf32.em. */
		. = ALIGN(4);
	} > pfls0
	
	/*
	 * Code executed before calling main extra section for C++ constructor init
	 *  -------------------------Start-----------------------------------------
	 */
	.init :
	{
		PROVIDE(__init_start = .);
		KEEP(*(.init))
		KEEP(*(.init*))
		PROVIDE(__init_end = .);
		. = ALIGN(8);
	
	} > pfls0
	
	.fini :
	{
		PROVIDE(__fini_start = .);
		KEEP(*(.fini))
		KEEP(*(.fini*))
		PROVIDE(__fini_end = .);
		. = ALIGN(8);
	} > pfls0
	
	/*
	 * C++ exception handling tables.  NOTE: gcc emits .eh_frame
	 * sections when compiling C sources with debugging enabled (-g).
	 * If you can be sure that your final application consists
	 * exclusively of C objects (i.e., no C++ objects), you may use
	 * the -R option of the "strip" and "objcopy" utilities to remove
	 * the .eh_frame section from the executable.
	 */
    .eh_frame_hdr :
    { 
        *(.eh_frame_hdr)
    } > pfls0

	.eh_frame  :
	{
		__EH_FRAME_BEGIN__ = . ;
		KEEP (*(.eh_frame))
		__EH_FRAME_END__ = . ;
		. = ALIGN(8);
	} > pfls0
    
    .gcc_except_table  : 
    { 
        __GCC_EXCEPT_TABLE_BEGIN__ = . ; 
        KEEP (*(.gcc_except_table)) 
        __GCC_EXCEPT_TABLE_END__ = . ; 
        . = ALIGN(8); 
    } > pfls0 
	
	/*
	 * Constructors and destructors.
	 */
	.ctors : FLAGS(ar)
	{
		__CTOR_LIST__ = . ;
		LONG((__CTOR_END__ - __CTOR_LIST__) / 4 - 2);
		/*
		 * Code executed before calling main extra section for C++ constructor init
		 *  -------------------------Start-----------------------------------------
		 */
		KEEP (*crtbegin.o(.ctors))
		KEEP (*(EXCLUDE_FILE (*crtend.o ) .ctors))
		KEEP (*(SORT(.ctors.*)))
		KEEP (*(.ctors))
		/*
		 * Code executed before calling main extra section for C++ constructor init
		 *  -------------------------End-----------------------------------------
		 */
		LONG(0) ;
		__CTOR_END__ = . ;
		. = ALIGN(8);
	} > pfls0
	.dtors : FLAGS(ar)
	{
		__DTOR_LIST__ = . ;
		LONG((__DTOR_END__ - __DTOR_LIST__) / 4 - 2);
		/*
		 * Code executed before calling main extra section for C++ distructor init
		 *  -------------------------Start-----------------------------------------
		 */
		KEEP (*crtbegin.o(.dtors))
		KEEP (*(EXCLUDE_FILE (*crtend.o ) .dtors))
		KEEP (*(SORT(.dtors.*)))
		KEEP (*(.dtors))
		/*
		 * Code executed before calling main extra section for C++ distructor init
		 *  -------------------------End-----------------------------------------
		 */
		LONG(0) ;
		__DTOR_END__ = . ;
		. = ALIGN(8);
	} > pfls0

	.traptab_tc2 (LCF_TRAPVEC2_START) :
	{
		PROVIDE(__TRAPTAB_CPU2 = .);
		KEEP (*(.traptab_cpu2));
	} > pfls0
	
	.traptab_tc1 (LCF_TRAPVEC1_START) :
	{
		PROVIDE(__TRAPTAB_CPU1 = .);
		KEEP (*(.traptab_cpu1));
	} > pfls0
}
	
/*Near data sections*/
	
CORE_ID = CPU2 ;

SECTIONS
{	
	CORE_SEC(.zdata) (LCF_DSPR2_START): FLAGS(awzl)
	{
		. = ALIGN(4) ;
		*(.zdata_cpu2)
		*(.zdata_cpu2.*)
		. = ALIGN(2);
	} > dsram2 AT> pfls0
	
	CORE_SEC(.zbss) (NOLOAD): FLAGS(awz)
	{
		. = ALIGN(4) ;
		*(.zbss_cpu2)
		*(.zbss_cpu2.*)
	} > dsram2

}

CORE_ID = CPU1;

SECTIONS
{	
	CORE_SEC(.zdata) (LCF_DSPR1_START): FLAGS(awzl)
	{
		. = ALIGN(4) ;
		*(.zdata_cpu1)
		*(.zdata_cpu1.*)
		. = ALIGN(2);
	} > dsram1 AT> pfls0
	
	CORE_SEC(.zbss): FLAGS(awz)
	{
		. = ALIGN(4) ;
		*(.zbss_cpu1)
		*(.zbss_cpu1.*)
	} > dsram1
}
	
CORE_ID = CPU0;
	
SECTIONS
{	
	CORE_SEC(.zdata) (LCF_DSPR0_START): FLAGS(awzl)
	{
		. = ALIGN(4) ;
		*(.zdata_cpu0)
		*(.zdata_cpu0.*)
		. = ALIGN(2);
	} > dsram0 AT> pfls0
	
	CORE_SEC(.zbss) (NOLOAD): FLAGS(awz)
	{
		. = ALIGN(4) ;
		*(.zbss_cpu0)
		*(.zbss_cpu0.*)
	} > dsram0
}
	
/*RAM sections without cpu sufix will go to default ram defined above with REGION_ALIAS*/
	
CORE_ID = GLOBAL;

SECTIONS
{	
	CORE_SEC(.zdata) : FLAGS(awzl)
	{
		. = ALIGN(4) ;
		*(.zdata)
		*(.zdata.*)
		*(.gnu.linkonce.z.*)
		. = ALIGN(2);
	} > default_ram AT> pfls0
	
	CORE_SEC(.zbss) (NOLOAD) : FLAGS(awz)
	{
		. = ALIGN(4) ;
		*(.zbss)
		*(.zbss.*)
		*(.bbss)
		*(.bbss.*)
		*(.gnu.linkonce.zb.*)	
	} > default_ram

	CORE_SEC(.sdata) : FLAGS(awsl)
	{
		. = ALIGN(4) ;
		*(.sdata)
		*(.sdata.*)
		. = ALIGN(2);
	} > default_ram AT> pfls0
	_SMALL_DATA_ = SIZEOF(CORE_SEC(.sdata)) ? ADDR(CORE_SEC(.sdata)) + 32k : (ADDR(CORE_SEC(.sdata)) & 0xF0000000) + 32k ;
	__A0_MEM = _SMALL_DATA_;
	
	CORE_SEC(.sbss) (NOLOAD): FLAGS(aws)
	{
		. = ALIGN(4) ;
		*(.sbss)
		*(.sbss.*)
	} > default_ram
	
	CORE_SEC(.data) : FLAGS(awl)
	{
		. = ALIGN(4) ;
		*(.data)
		*(.data.*)
		*(.gnu.linkonce.d.*)
		. = ALIGN(2);
	} > default_ram AT> pfls0
	
	CORE_SEC(.bss) (NOLOAD) : FLAGS(aw)
	{
		. = ALIGN(4) ;
		*(.bss)
		*(.bss.*)
		*(.gnu.linkonce.b.*) 	
	} > default_ram
	
	.heap  : FLAGS(aw)
	{
		. = ALIGN(4);
		__HEAP = .;
		. += LCF_HEAP_SIZE;
		__HEAP_END = .;
	} > default_ram
}

CORE_ID = CPU2 ;

SECTIONS
{	
	CORE_SEC(.data) : FLAGS(awl)
	{
		. = ALIGN(4) ;
		*(.data_cpu2)
		*(.data_cpu2.*)
		. = ALIGN(2);
	} > dsram2 AT> pfls0
	
	CORE_SEC(.bss) (NOLOAD): FLAGS(aw)
	{
		. = ALIGN(4) ;
		*(.bss_cpu2)
		*(.bss_cpu2.*)
	} > dsram2
}

CORE_ID = CPU1;

SECTIONS
{	
	CORE_SEC(.data) : FLAGS(awl)
	{
		. = ALIGN(4) ;
		*(.data_cpu1)
		*(.data_cpu1.*)
		. = ALIGN(2);
	} > dsram1 AT> pfls0
	
	CORE_SEC(.bss) (NOLOAD): FLAGS(aw)
	{
		. = ALIGN(4) ;
		*(.bss_cpu1)
		*(.bss_cpu1.*)
	} > dsram1
}

CORE_ID = CPU0;
	
SECTIONS
{	
	CORE_SEC(.data) : FLAGS(awl)
	{
		. = ALIGN(4) ;
		*(.data_cpu0)
		*(.data_cpu0.*)
		. = ALIGN(2);
	} > dsram0 AT> pfls0
	
	CORE_SEC(.bss) (NOLOAD): FLAGS(aw)
	{
		. = ALIGN(4) ;
		*(.bss_cpu0)
		*(.bss_cpu0.*)
	} > dsram0
	
	CORE_SEC(.psram_text)  : FLAGS(awx)
	{
		. = ALIGN(2);
		*(.psram_cpu0)
		*(.cpu0_psram)
		. = ALIGN(2);
	} > psram0 AT> pfls0
}

CORE_ID = CPU2 ;

SECTIONS
{	
	CORE_SEC(.ustack) (LCF_DSPR2_START + LCF_USTACK2_OFFSET):
	{
		PROVIDE(__USTACK2_END = .);
		. = . + LCF_USTACK2_SIZE;
		PROVIDE(__USTACK2 = .);
	} > dsram2
	
	CORE_SEC(.istack) (LCF_DSPR2_START + LCF_ISTACK2_OFFSET):
	{
		PROVIDE(__ISTACK2_END = .);
		. = . + LCF_ISTACK2_SIZE;
		PROVIDE(__ISTACK2 = .);
	} > dsram2
	
	CORE_SEC(.csa) (LCF_DSPR2_START + LCF_CSA2_OFFSET):
	{
		PROVIDE(__CSA2 = .);
		. = . + LCF_CSA2_SIZE;
		PROVIDE(__CSA2_END = .);
	} > dsram2
	
	CORE_SEC(.psram_text)  : FLAGS(awx)
	{
		. = ALIGN(2);
		*(.psram_cpu2)
		*(.cpu2_psram)
		. = ALIGN(2);
	} > psram2 AT> pfls0
}

CORE_ID = CPU1;

SECTIONS
{
	CORE_SEC(.ustack) (LCF_DSPR1_START + LCF_USTACK1_OFFSET):
	{
		PROVIDE(__USTACK1_END = .);
		. = . + LCF_USTACK1_SIZE;
		PROVIDE(__USTACK1 = .);
	} > dsram1
	
	CORE_SEC(.istack) (LCF_DSPR1_START + LCF_ISTACK1_OFFSET):
	{
		PROVIDE(__ISTACK1_END = .);
		. = . + LCF_ISTACK1_SIZE;
		PROVIDE(__ISTACK1 = .);
	} > dsram1
	
	CORE_SEC(.csa) (LCF_DSPR1_START + LCF_CSA1_OFFSET):
	{
		PROVIDE(__CSA1 = .);
		. = . + LCF_CSA1_SIZE;
		PROVIDE(__CSA1_END = .);
	} > dsram1
	
	CORE_SEC(.psram_text)  : FLAGS(awx)
	{
		. = ALIGN(2);
		*(.psram_cpu1)
		*(.cpu1_psram)
		. = ALIGN(2);
	} > psram1 AT> pfls0
}

CORE_ID = CPU0;
	
SECTIONS
{	
	CORE_SEC(.ustack) (LCF_DSPR0_START + LCF_USTACK0_OFFSET):
	{
		PROVIDE(__USTACK0_END = .);
		. = . + LCF_USTACK0_SIZE;
		PROVIDE(__USTACK0 = .);
	} > dsram0
	
	CORE_SEC(.istack) (LCF_DSPR0_START + LCF_ISTACK0_OFFSET):
	{
		PROVIDE(__ISTACK0_END = .);
		. = . + LCF_ISTACK0_SIZE;
		PROVIDE(__ISTACK0 = .);
	} > dsram0
	
	CORE_SEC(.csa) (LCF_DSPR0_START + LCF_CSA0_OFFSET):
	{
		PROVIDE(__CSA0 = .);
		. = . + LCF_CSA0_SIZE;
		PROVIDE(__CSA0_END = .);
	} > dsram0
}	

CORE_ID = GLOBAL;

SECTIONS
{		
	CORE_SEC(.sdata3) : FLAGS(arsl)
	{
		*(.rodata_a8)
		*(.rodata_a8.*)
	} > pfls0
	
	_SMALL_DATA3_ = SIZEOF(CORE_SEC(.sdata3)) ? ADDR(CORE_SEC(.sdata3)) + 32k : (ADDR(CORE_SEC(.sdata3)) & 0xF0000000) + 32k ;
	__A8_MEM = _SMALL_DATA3_;
	
	.lmu_zdata :
	{
		*(.zdata_lmu)
		*(.zdata_lmu.*)
		. = ALIGN(2);
	} > lmuram AT> pfls0
	
	.lmu_zbss :
	{
	*(.zbss_lmu)
	*(.zbss_lmu.*)
	} > lmuram
	
	.lmu_sdata :
	{
		*(.sdata_lmu)
		*(.sdata_lmu.*)
		. = ALIGN(2);
	} > lmuram AT> pfls0
	
	.lmu_sbss :
	{
		*(.sbss_lmu)
		*(.sbss_lmu.*)
	} > lmuram
	
	_SMALL_DATA4_ = SIZEOF(CORE_SEC(.lmu_sdata)) ? ADDR(CORE_SEC(.lmu_sdata)) + 32k : (ADDR(CORE_SEC(.lmu_sdata)) & 0xF0000000) + 32k ;
	__A9_MEM = _SMALL_DATA4_;
	
	.lmu_data :
	{
		*(.data_lmu)
		*(.data_lmu.*)
		*(.lmudata)
		*(.lmudata.*)
		. = ALIGN(2);
	} > lmuram AT> pfls0
	
	.lmu_bss :
	{
		*(.bss_lmu)
		*(.bss_lmu.*)
		*(.lmubss)
		*(.lmubss.*)
	} > lmuram
	
	.inttab_tc0_000 (LCF_INTVEC0_START + 0x0) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_0)); } > pfls0
	.inttab_tc0_001 (LCF_INTVEC0_START + 0x20) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_1)); } > pfls0
	.inttab_tc0_002 (LCF_INTVEC0_START + 0x40) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_2)); } > pfls0
	.inttab_tc0_003 (LCF_INTVEC0_START + 0x60) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_3)); } > pfls0
	.inttab_tc0_004 (LCF_INTVEC0_START + 0x80) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_4)); } > pfls0
	.inttab_tc0_005 (LCF_INTVEC0_START + 0xA0) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_5)); } > pfls0
	.inttab_tc0_006 (LCF_INTVEC0_START + 0xC0) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_6)); } > pfls0
	.inttab_tc0_007 (LCF_INTVEC0_START + 0xE0) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_7)); } > pfls0
	.inttab_tc0_008 (LCF_INTVEC0_START + 0x100) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_8)); } > pfls0
	.inttab_tc0_009 (LCF_INTVEC0_START + 0x120) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_9)); } > pfls0
	.inttab_tc0_00A (LCF_INTVEC0_START + 0x140) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_10)); } > pfls0
	.inttab_tc0_00B (LCF_INTVEC0_START + 0x160) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_11)); } > pfls0
	.inttab_tc0_00C (LCF_INTVEC0_START + 0x180) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_12)); } > pfls0
	.inttab_tc0_00D (LCF_INTVEC0_START + 0x1A0) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_13)); } > pfls0
	.inttab_tc0_00E (LCF_INTVEC0_START + 0x1C0) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_14)); } > pfls0
	.inttab_tc0_00F (LCF_INTVEC0_START + 0x1E0) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_15)); } > pfls0
	.inttab_tc0_010 (LCF_INTVEC0_START + 0x200) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_16)); } > pfls0
	.inttab_tc0_011 (LCF_INTVEC0_START + 0x220) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_17)); } > pfls0
	.inttab_tc0_012 (LCF_INTVEC0_START + 0x240) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_18)); } > pfls0
	.inttab_tc0_013 (LCF_INTVEC0_START + 0x260) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_19)); } > pfls0
	.inttab_tc0_014 (LCF_INTVEC0_START + 0x280) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_20)); } > pfls0
	.inttab_tc0_015 (LCF_INTVEC0_START + 0x2A0) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_21)); } > pfls0
	.inttab_tc0_016 (LCF_INTVEC0_START + 0x2C0) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_22)); } > pfls0
	.inttab_tc0_017 (LCF_INTVEC0_START + 0x2E0) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_23)); } > pfls0
	.inttab_tc0_018 (LCF_INTVEC0_START + 0x300) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_24)); } > pfls0
	.inttab_tc0_019 (LCF_INTVEC0_START + 0x320) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_25)); } > pfls0
	.inttab_tc0_01A (LCF_INTVEC0_START + 0x340) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_26)); } > pfls0
	.inttab_tc0_01B (LCF_INTVEC0_START + 0x360) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_27)); } > pfls0
	.inttab_tc0_01C (LCF_INTVEC0_START + 0x380) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_28)); } > pfls0
	.inttab_tc0_01D (LCF_INTVEC0_START + 0x3A0) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_29)); } > pfls0
	.inttab_tc0_01E (LCF_INTVEC0_START + 0x3C0) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_30)); } > pfls0
	.inttab_tc0_01F (LCF_INTVEC0_START + 0x3E0) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_31)); } > pfls0
	.inttab_tc0_020 (LCF_INTVEC0_START + 0x400) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_32)); } > pfls0
	.inttab_tc0_021 (LCF_INTVEC0_START + 0x420) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_33)); } > pfls0
	.inttab_tc0_022 (LCF_INTVEC0_START + 0x440) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_34)); } > pfls0
	.inttab_tc0_023 (LCF_INTVEC0_START + 0x460) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_35)); } > pfls0
	.inttab_tc0_024 (LCF_INTVEC0_START + 0x480) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_36)); } > pfls0
	.inttab_tc0_025 (LCF_INTVEC0_START + 0x4A0) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_37)); } > pfls0
	.inttab_tc0_026 (LCF_INTVEC0_START + 0x4C0) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_38)); } > pfls0
	.inttab_tc0_027 (LCF_INTVEC0_START + 0x4E0) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_39)); } > pfls0
	.inttab_tc0_028 (LCF_INTVEC0_START + 0x500) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_40)); } > pfls0
	.inttab_tc0_029 (LCF_INTVEC0_START + 0x520) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_41)); } > pfls0
	.inttab_tc0_02A (LCF_INTVEC0_START + 0x540) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_42)); } > pfls0
	.inttab_tc0_02B (LCF_INTVEC0_START + 0x560) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_43)); } > pfls0
	.inttab_tc0_02C (LCF_INTVEC0_START + 0x580) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_44)); } > pfls0
	.inttab_tc0_02D (LCF_INTVEC0_START + 0x5A0) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_45)); } > pfls0
	.inttab_tc0_02E (LCF_INTVEC0_START + 0x5C0) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_46)); } > pfls0
	.inttab_tc0_02F (LCF_INTVEC0_START + 0x5E0) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_47)); } > pfls0
	.inttab_tc0_030 (LCF_INTVEC0_START + 0x600) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_48)); } > pfls0
	.inttab_tc0_031 (LCF_INTVEC0_START + 0x620) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_49)); } > pfls0
	.inttab_tc0_032 (LCF_INTVEC0_START + 0x640) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_50)); } > pfls0
	.inttab_tc0_033 (LCF_INTVEC0_START + 0x660) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_51)); } > pfls0
	.inttab_tc0_034 (LCF_INTVEC0_START + 0x680) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_52)); } > pfls0
	.inttab_tc0_035 (LCF_INTVEC0_START + 0x6A0) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_53)); } > pfls0
	.inttab_tc0_036 (LCF_INTVEC0_START + 0x6C0) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_54)); } > pfls0
	.inttab_tc0_037 (LCF_INTVEC0_START + 0x6E0) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_55)); } > pfls0
	.inttab_tc0_038 (LCF_INTVEC0_START + 0x700) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_56)); } > pfls0
	.inttab_tc0_039 (LCF_INTVEC0_START + 0x720) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_57)); } > pfls0
	.inttab_tc0_03A (LCF_INTVEC0_START + 0x740) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_58)); } > pfls0
	.inttab_tc0_03B (LCF_INTVEC0_START + 0x760) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_59)); } > pfls0
	.inttab_tc0_03C (LCF_INTVEC0_START + 0x780) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_60)); } > pfls0
	.inttab_tc0_03D (LCF_INTVEC0_START + 0x7A0) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_61)); } > pfls0
	.inttab_tc0_03E (LCF_INTVEC0_START + 0x7C0) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_62)); } > pfls0
	.inttab_tc0_03F (LCF_INTVEC0_START + 0x7E0) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_63)); } > pfls0
	.inttab_tc0_040 (LCF_INTVEC0_START + 0x800) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_64)); } > pfls0
	.inttab_tc0_041 (LCF_INTVEC0_START + 0x820) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_65)); } > pfls0
	.inttab_tc0_042 (LCF_INTVEC0_START + 0x840) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_66)); } > pfls0
	.inttab_tc0_043 (LCF_INTVEC0_START + 0x860) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_67)); } > pfls0
	.inttab_tc0_044 (LCF_INTVEC0_START + 0x880) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_68)); } > pfls0
	.inttab_tc0_045 (LCF_INTVEC0_START + 0x8A0) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_69)); } > pfls0
	.inttab_tc0_046 (LCF_INTVEC0_START + 0x8C0) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_70)); } > pfls0
	.inttab_tc0_047 (LCF_INTVEC0_START + 0x8E0) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_71)); } > pfls0
	.inttab_tc0_048 (LCF_INTVEC0_START + 0x900) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_72)); } > pfls0
	.inttab_tc0_049 (LCF_INTVEC0_START + 0x920) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_73)); } > pfls0
	.inttab_tc0_04A (LCF_INTVEC0_START + 0x940) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_74)); } > pfls0
	.inttab_tc0_04B (LCF_INTVEC0_START + 0x960) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_75)); } > pfls0
	.inttab_tc0_04C (LCF_INTVEC0_START + 0x980) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_76)); } > pfls0
	.inttab_tc0_04D (LCF_INTVEC0_START + 0x9A0) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_77)); } > pfls0
	.inttab_tc0_04E (LCF_INTVEC0_START + 0x9C0) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_78)); } > pfls0
	.inttab_tc0_04F (LCF_INTVEC0_START + 0x9E0) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_79)); } > pfls0
	.inttab_tc0_050 (LCF_INTVEC0_START + 0xA00) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_80)); } > pfls0
	.inttab_tc0_051 (LCF_INTVEC0_START + 0xA20) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_81)); } > pfls0
	.inttab_tc0_052 (LCF_INTVEC0_START + 0xA40) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_82)); } > pfls0
	.inttab_tc0_053 (LCF_INTVEC0_START + 0xA60) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_83)); } > pfls0
	.inttab_tc0_054 (LCF_INTVEC0_START + 0xA80) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_84)); } > pfls0
	.inttab_tc0_055 (LCF_INTVEC0_START + 0xAA0) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_85)); } > pfls0
	.inttab_tc0_056 (LCF_INTVEC0_START + 0xAC0) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_86)); } > pfls0
	.inttab_tc0_057 (LCF_INTVEC0_START + 0xAE0) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_87)); } > pfls0
	.inttab_tc0_058 (LCF_INTVEC0_START + 0xB00) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_88)); } > pfls0
	.inttab_tc0_059 (LCF_INTVEC0_START + 0xB20) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_89)); } > pfls0
	.inttab_tc0_05A (LCF_INTVEC0_START + 0xB40) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_90)); } > pfls0
	.inttab_tc0_05B (LCF_INTVEC0_START + 0xB60) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_91)); } > pfls0
	.inttab_tc0_05C (LCF_INTVEC0_START + 0xB80) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_92)); } > pfls0
	.inttab_tc0_05D (LCF_INTVEC0_START + 0xBA0) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_93)); } > pfls0
	.inttab_tc0_05E (LCF_INTVEC0_START + 0xBC0) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_94)); } > pfls0
	.inttab_tc0_05F (LCF_INTVEC0_START + 0xBE0) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_95)); } > pfls0
	.inttab_tc0_060 (LCF_INTVEC0_START + 0xC00) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_96)); } > pfls0
	.inttab_tc0_061 (LCF_INTVEC0_START + 0xC20) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_97)); } > pfls0
	.inttab_tc0_062 (LCF_INTVEC0_START + 0xC40) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_98)); } > pfls0
	.inttab_tc0_063 (LCF_INTVEC0_START + 0xC60) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_99)); } > pfls0
	.inttab_tc0_064 (LCF_INTVEC0_START + 0xC80) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_100)); } > pfls0
	.inttab_tc0_065 (LCF_INTVEC0_START + 0xCA0) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_101)); } > pfls0
	.inttab_tc0_066 (LCF_INTVEC0_START + 0xCC0) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_102)); } > pfls0
	.inttab_tc0_067 (LCF_INTVEC0_START + 0xCE0) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_103)); } > pfls0
	.inttab_tc0_068 (LCF_INTVEC0_START + 0xD00) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_104)); } > pfls0
	.inttab_tc0_069 (LCF_INTVEC0_START + 0xD20) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_105)); } > pfls0
	.inttab_tc0_06A (LCF_INTVEC0_START + 0xD40) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_106)); } > pfls0
	.inttab_tc0_06B (LCF_INTVEC0_START + 0xD60) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_107)); } > pfls0
	.inttab_tc0_06C (LCF_INTVEC0_START + 0xD80) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_108)); } > pfls0
	.inttab_tc0_06D (LCF_INTVEC0_START + 0xDA0) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_109)); } > pfls0
	.inttab_tc0_06E (LCF_INTVEC0_START + 0xDC0) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_110)); } > pfls0
	.inttab_tc0_06F (LCF_INTVEC0_START + 0xDE0) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_111)); } > pfls0
	.inttab_tc0_070 (LCF_INTVEC0_START + 0xE00) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_112)); } > pfls0
	.inttab_tc0_071 (LCF_INTVEC0_START + 0xE20) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_113)); } > pfls0
	.inttab_tc0_072 (LCF_INTVEC0_START + 0xE40) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_114)); } > pfls0
	.inttab_tc0_073 (LCF_INTVEC0_START + 0xE60) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_115)); } > pfls0
	.inttab_tc0_074 (LCF_INTVEC0_START + 0xE80) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_116)); } > pfls0
	.inttab_tc0_075 (LCF_INTVEC0_START + 0xEA0) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_117)); } > pfls0
	.inttab_tc0_076 (LCF_INTVEC0_START + 0xEC0) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_118)); } > pfls0
	.inttab_tc0_077 (LCF_INTVEC0_START + 0xEE0) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_119)); } > pfls0
	.inttab_tc0_078 (LCF_INTVEC0_START + 0xF00) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_120)); } > pfls0
	.inttab_tc0_079 (LCF_INTVEC0_START + 0xF20) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_121)); } > pfls0
	.inttab_tc0_07A (LCF_INTVEC0_START + 0xF40) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_122)); } > pfls0
	.inttab_tc0_07B (LCF_INTVEC0_START + 0xF60) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_123)); } > pfls0
	.inttab_tc0_07C (LCF_INTVEC0_START + 0xF80) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_124)); } > pfls0
	.inttab_tc0_07D (LCF_INTVEC0_START + 0xFA0) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_125)); } > pfls0
	.inttab_tc0_07E (LCF_INTVEC0_START + 0xFC0) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_126)); } > pfls0
	.inttab_tc0_07F (LCF_INTVEC0_START + 0xFE0) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_127)); } > pfls0
	.inttab_tc0_080 (LCF_INTVEC0_START + 0x1000) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_128)); } > pfls0
	.inttab_tc0_081 (LCF_INTVEC0_START + 0x1020) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_129)); } > pfls0
	.inttab_tc0_082 (LCF_INTVEC0_START + 0x1040) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_130)); } > pfls0
	.inttab_tc0_083 (LCF_INTVEC0_START + 0x1060) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_131)); } > pfls0
	.inttab_tc0_084 (LCF_INTVEC0_START + 0x1080) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_132)); } > pfls0
	.inttab_tc0_085 (LCF_INTVEC0_START + 0x10A0) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_133)); } > pfls0
	.inttab_tc0_086 (LCF_INTVEC0_START + 0x10C0) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_134)); } > pfls0
	.inttab_tc0_087 (LCF_INTVEC0_START + 0x10E0) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_135)); } > pfls0
	.inttab_tc0_088 (LCF_INTVEC0_START + 0x1100) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_136)); } > pfls0
	.inttab_tc0_089 (LCF_INTVEC0_START + 0x1120) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_137)); } > pfls0
	.inttab_tc0_08A (LCF_INTVEC0_START + 0x1140) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_138)); } > pfls0
	.inttab_tc0_08B (LCF_INTVEC0_START + 0x1160) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_139)); } > pfls0
	.inttab_tc0_08C (LCF_INTVEC0_START + 0x1180) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_140)); } > pfls0
	.inttab_tc0_08D (LCF_INTVEC0_START + 0x11A0) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_141)); } > pfls0
	.inttab_tc0_08E (LCF_INTVEC0_START + 0x11C0) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_142)); } > pfls0
	.inttab_tc0_08F (LCF_INTVEC0_START + 0x11E0) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_143)); } > pfls0
	.inttab_tc0_090 (LCF_INTVEC0_START + 0x1200) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_144)); } > pfls0
	.inttab_tc0_091 (LCF_INTVEC0_START + 0x1220) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_145)); } > pfls0
	.inttab_tc0_092 (LCF_INTVEC0_START + 0x1240) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_146)); } > pfls0
	.inttab_tc0_093 (LCF_INTVEC0_START + 0x1260) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_147)); } > pfls0
	.inttab_tc0_094 (LCF_INTVEC0_START + 0x1280) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_148)); } > pfls0
	.inttab_tc0_095 (LCF_INTVEC0_START + 0x12A0) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_149)); } > pfls0
	.inttab_tc0_096 (LCF_INTVEC0_START + 0x12C0) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_150)); } > pfls0
	.inttab_tc0_097 (LCF_INTVEC0_START + 0x12E0) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_151)); } > pfls0
	.inttab_tc0_098 (LCF_INTVEC0_START + 0x1300) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_152)); } > pfls0
	.inttab_tc0_099 (LCF_INTVEC0_START + 0x1320) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_153)); } > pfls0
	.inttab_tc0_09A (LCF_INTVEC0_START + 0x1340) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_154)); } > pfls0
	.inttab_tc0_09B (LCF_INTVEC0_START + 0x1360) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_155)); } > pfls0
	.inttab_tc0_09C (LCF_INTVEC0_START + 0x1380) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_156)); } > pfls0
	.inttab_tc0_09D (LCF_INTVEC0_START + 0x13A0) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_157)); } > pfls0
	.inttab_tc0_09E (LCF_INTVEC0_START + 0x13C0) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_158)); } > pfls0
	.inttab_tc0_09F (LCF_INTVEC0_START + 0x13E0) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_159)); } > pfls0
	.inttab_tc0_0A0 (LCF_INTVEC0_START + 0x1400) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_160)); } > pfls0
	.inttab_tc0_0A1 (LCF_INTVEC0_START + 0x1420) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_161)); } > pfls0
	.inttab_tc0_0A2 (LCF_INTVEC0_START + 0x1440) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_162)); } > pfls0
	.inttab_tc0_0A3 (LCF_INTVEC0_START + 0x1460) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_163)); } > pfls0
	.inttab_tc0_0A4 (LCF_INTVEC0_START + 0x1480) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_164)); } > pfls0
	.inttab_tc0_0A5 (LCF_INTVEC0_START + 0x14A0) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_165)); } > pfls0
	.inttab_tc0_0A6 (LCF_INTVEC0_START + 0x14C0) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_166)); } > pfls0
	.inttab_tc0_0A7 (LCF_INTVEC0_START + 0x14E0) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_167)); } > pfls0
	.inttab_tc0_0A8 (LCF_INTVEC0_START + 0x1500) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_168)); } > pfls0
	.inttab_tc0_0A9 (LCF_INTVEC0_START + 0x1520) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_169)); } > pfls0
	.inttab_tc0_0AA (LCF_INTVEC0_START + 0x1540) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_170)); } > pfls0
	.inttab_tc0_0AB (LCF_INTVEC0_START + 0x1560) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_171)); } > pfls0
	.inttab_tc0_0AC (LCF_INTVEC0_START + 0x1580) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_172)); } > pfls0
	.inttab_tc0_0AD (LCF_INTVEC0_START + 0x15A0) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_173)); } > pfls0
	.inttab_tc0_0AE (LCF_INTVEC0_START + 0x15C0) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_174)); } > pfls0
	.inttab_tc0_0AF (LCF_INTVEC0_START + 0x15E0) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_175)); } > pfls0
	.inttab_tc0_0B0 (LCF_INTVEC0_START + 0x1600) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_176)); } > pfls0
	.inttab_tc0_0B1 (LCF_INTVEC0_START + 0x1620) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_177)); } > pfls0
	.inttab_tc0_0B2 (LCF_INTVEC0_START + 0x1640) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_178)); } > pfls0
	.inttab_tc0_0B3 (LCF_INTVEC0_START + 0x1660) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_179)); } > pfls0
	.inttab_tc0_0B4 (LCF_INTVEC0_START + 0x1680) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_180)); } > pfls0
	.inttab_tc0_0B5 (LCF_INTVEC0_START + 0x16A0) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_181)); } > pfls0
	.inttab_tc0_0B6 (LCF_INTVEC0_START + 0x16C0) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_182)); } > pfls0
	.inttab_tc0_0B7 (LCF_INTVEC0_START + 0x16E0) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_183)); } > pfls0
	.inttab_tc0_0B8 (LCF_INTVEC0_START + 0x1700) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_184)); } > pfls0
	.inttab_tc0_0B9 (LCF_INTVEC0_START + 0x1720) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_185)); } > pfls0
	.inttab_tc0_0BA (LCF_INTVEC0_START + 0x1740) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_186)); } > pfls0
	.inttab_tc0_0BB (LCF_INTVEC0_START + 0x1760) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_187)); } > pfls0
	.inttab_tc0_0BC (LCF_INTVEC0_START + 0x1780) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_188)); } > pfls0
	.inttab_tc0_0BD (LCF_INTVEC0_START + 0x17A0) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_189)); } > pfls0
	.inttab_tc0_0BE (LCF_INTVEC0_START + 0x17C0) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_190)); } > pfls0
	.inttab_tc0_0BF (LCF_INTVEC0_START + 0x17E0) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_191)); } > pfls0
	.inttab_tc0_0C0 (LCF_INTVEC0_START + 0x1800) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_192)); } > pfls0
	.inttab_tc0_0C1 (LCF_INTVEC0_START + 0x1820) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_193)); } > pfls0
	.inttab_tc0_0C2 (LCF_INTVEC0_START + 0x1840) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_194)); } > pfls0
	.inttab_tc0_0C3 (LCF_INTVEC0_START + 0x1860) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_195)); } > pfls0
	.inttab_tc0_0C4 (LCF_INTVEC0_START + 0x1880) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_196)); } > pfls0
	.inttab_tc0_0C5 (LCF_INTVEC0_START + 0x18A0) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_197)); } > pfls0
	.inttab_tc0_0C6 (LCF_INTVEC0_START + 0x18C0) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_198)); } > pfls0
	.inttab_tc0_0C7 (LCF_INTVEC0_START + 0x18E0) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_199)); } > pfls0
	.inttab_tc0_0C8 (LCF_INTVEC0_START + 0x1900) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_200)); } > pfls0
	.inttab_tc0_0C9 (LCF_INTVEC0_START + 0x1920) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_201)); } > pfls0
	.inttab_tc0_0CA (LCF_INTVEC0_START + 0x1940) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_202)); } > pfls0
	.inttab_tc0_0CB (LCF_INTVEC0_START + 0x1960) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_203)); } > pfls0
	.inttab_tc0_0CC (LCF_INTVEC0_START + 0x1980) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_204)); } > pfls0
	.inttab_tc0_0CD (LCF_INTVEC0_START + 0x19A0) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_205)); } > pfls0
	.inttab_tc0_0CE (LCF_INTVEC0_START + 0x19C0) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_206)); } > pfls0
	.inttab_tc0_0CF (LCF_INTVEC0_START + 0x19E0) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_207)); } > pfls0
	.inttab_tc0_0D0 (LCF_INTVEC0_START + 0x1A00) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_208)); } > pfls0
	.inttab_tc0_0D1 (LCF_INTVEC0_START + 0x1A20) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_209)); } > pfls0
	.inttab_tc0_0D2 (LCF_INTVEC0_START + 0x1A40) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_210)); } > pfls0
	.inttab_tc0_0D3 (LCF_INTVEC0_START + 0x1A60) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_211)); } > pfls0
	.inttab_tc0_0D4 (LCF_INTVEC0_START + 0x1A80) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_212)); } > pfls0
	.inttab_tc0_0D5 (LCF_INTVEC0_START + 0x1AA0) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_213)); } > pfls0
	.inttab_tc0_0D6 (LCF_INTVEC0_START + 0x1AC0) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_214)); } > pfls0
	.inttab_tc0_0D7 (LCF_INTVEC0_START + 0x1AE0) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_215)); } > pfls0
	.inttab_tc0_0D8 (LCF_INTVEC0_START + 0x1B00) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_216)); } > pfls0
	.inttab_tc0_0D9 (LCF_INTVEC0_START + 0x1B20) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_217)); } > pfls0
	.inttab_tc0_0DA (LCF_INTVEC0_START + 0x1B40) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_218)); } > pfls0
	.inttab_tc0_0DB (LCF_INTVEC0_START + 0x1B60) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_219)); } > pfls0
	.inttab_tc0_0DC (LCF_INTVEC0_START + 0x1B80) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_220)); } > pfls0
	.inttab_tc0_0DD (LCF_INTVEC0_START + 0x1BA0) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_221)); } > pfls0
	.inttab_tc0_0DE (LCF_INTVEC0_START + 0x1BC0) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_222)); } > pfls0
	.inttab_tc0_0DF (LCF_INTVEC0_START + 0x1BE0) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_223)); } > pfls0
	.inttab_tc0_0E0 (LCF_INTVEC0_START + 0x1C00) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_224)); } > pfls0
	.inttab_tc0_0E1 (LCF_INTVEC0_START + 0x1C20) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_225)); } > pfls0
	.inttab_tc0_0E2 (LCF_INTVEC0_START + 0x1C40) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_226)); } > pfls0
	.inttab_tc0_0E3 (LCF_INTVEC0_START + 0x1C60) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_227)); } > pfls0
	.inttab_tc0_0E4 (LCF_INTVEC0_START + 0x1C80) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_228)); } > pfls0
	.inttab_tc0_0E5 (LCF_INTVEC0_START + 0x1CA0) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_229)); } > pfls0
	.inttab_tc0_0E6 (LCF_INTVEC0_START + 0x1CC0) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_230)); } > pfls0
	.inttab_tc0_0E7 (LCF_INTVEC0_START + 0x1CE0) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_231)); } > pfls0
	.inttab_tc0_0E8 (LCF_INTVEC0_START + 0x1D00) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_232)); } > pfls0
	.inttab_tc0_0E9 (LCF_INTVEC0_START + 0x1D20) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_233)); } > pfls0
	.inttab_tc0_0EA (LCF_INTVEC0_START + 0x1D40) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_234)); } > pfls0
	.inttab_tc0_0EB (LCF_INTVEC0_START + 0x1D60) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_235)); } > pfls0
	.inttab_tc0_0EC (LCF_INTVEC0_START + 0x1D80) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_236)); } > pfls0
	.inttab_tc0_0ED (LCF_INTVEC0_START + 0x1DA0) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_237)); } > pfls0
	.inttab_tc0_0EE (LCF_INTVEC0_START + 0x1DC0) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_238)); } > pfls0
	.inttab_tc0_0EF (LCF_INTVEC0_START + 0x1DE0) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_239)); } > pfls0
	.inttab_tc0_0F0 (LCF_INTVEC0_START + 0x1E00) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_240)); } > pfls0
	.inttab_tc0_0F1 (LCF_INTVEC0_START + 0x1E20) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_241)); } > pfls0
	.inttab_tc0_0F2 (LCF_INTVEC0_START + 0x1E40) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_242)); } > pfls0
	.inttab_tc0_0F3 (LCF_INTVEC0_START + 0x1E60) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_243)); } > pfls0
	.inttab_tc0_0F4 (LCF_INTVEC0_START + 0x1E80) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_244)); } > pfls0
	.inttab_tc0_0F5 (LCF_INTVEC0_START + 0x1EA0) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_245)); } > pfls0
	.inttab_tc0_0F6 (LCF_INTVEC0_START + 0x1EC0) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_246)); } > pfls0
	.inttab_tc0_0F7 (LCF_INTVEC0_START + 0x1EE0) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_247)); } > pfls0
	.inttab_tc0_0F8 (LCF_INTVEC0_START + 0x1F00) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_248)); } > pfls0
	.inttab_tc0_0F9 (LCF_INTVEC0_START + 0x1F20) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_249)); } > pfls0
	.inttab_tc0_0FA (LCF_INTVEC0_START + 0x1F40) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_250)); } > pfls0
	.inttab_tc0_0FB (LCF_INTVEC0_START + 0x1F60) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_251)); } > pfls0
	.inttab_tc0_0FC (LCF_INTVEC0_START + 0x1F80) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_252)); } > pfls0
	.inttab_tc0_0FD (LCF_INTVEC0_START + 0x1FA0) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_253)); } > pfls0
	.inttab_tc0_0FE (LCF_INTVEC0_START + 0x1FC0) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_254)); } > pfls0
	.inttab_tc0_0FF (LCF_INTVEC0_START + 0x1FE0) : { . = ALIGN(8) ;  KEEP (*(.intvec_tc0_255)); } > pfls0
	__INTTAB_CPU0 = LCF_INTVEC0_START;
	__INTTAB_CPU1 = LCF_INTVEC0_START; /*Single interrupt table for all CPUs*/
	__INTTAB_CPU2 = LCF_INTVEC0_START; /*Single interrupt table for all CPUs*/

	
	/*
	 * DWARF debug sections.
	 * Symbols in the DWARF debugging sections are relative to the
	 * beginning of the section, so we begin them at 0.
	 */
	/*
	 * DWARF 1
	 */
	.comment         0 : { *(.comment) }
	.debug           0 : { *(.debug) }
	.line            0 : { *(.line) }
	/*
	 * GNU DWARF 1 extensions
	 */
	.debug_srcinfo   0 : { *(.debug_srcinfo) }
	.debug_sfnames   0 : { *(.debug_sfnames) }
	/*
	 * DWARF 1.1 and DWARF 2
	 */
	.debug_aranges   0 : { *(.debug_aranges) }
	.debug_pubnames  0 : { *(.debug_pubnames) }
	/*
	 * DWARF 2
	 */
	.debug_info      0 : { *(.debug_info) }
	.debug_abbrev    0 : { *(.debug_abbrev) }
	.debug_line      0 : { *(.debug_line) }
	.debug_frame     0 : { *(.debug_frame) }
	.debug_str       0 : { *(.debug_str) }
	.debug_loc       0 : { *(.debug_loc) }
	.debug_macinfo   0 : { *(.debug_macinfo) }
	.debug_ranges    0 : { *(.debug_ranges) }
	/*
	 * SGI/MIPS DWARF 2 extensions
	 */
	.debug_weaknames 0 : { *(.debug_weaknames) }
	.debug_funcnames 0 : { *(.debug_funcnames) }
	.debug_typenames 0 : { *(.debug_typenames) }
	.debug_varnames  0 : { *(.debug_varnames) }
	/*
	 * Optional sections that may only appear when relocating.
	 */
	/*
	 * Optional sections that may appear regardless of relocating.
	 */
	.version_info    0 : { *(.version_info) }
	.boffs           0 : { KEEP (*(.boffs)) }
}
