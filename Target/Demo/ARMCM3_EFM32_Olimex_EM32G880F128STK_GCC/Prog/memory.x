MEMORY
{
    FLASH (rx) : ORIGIN = 0x00002000, LENGTH = 120K
    SRAM (rwx) : ORIGIN = 0x20000000, LENGTH = 16K
}

SECTIONS
{
    __STACKSIZE__ = 1024;

    .text (READONLY): /* The "READONLY" keyword is only supported in GCC11 and later, remove it if using GCC10 or earlier. */
    {
		    KEEP(*(.isr_vector))
		    *(.text*)

		    KEEP(*(.init))
		    KEEP(*(.fini))

		    /* .ctors */
		    *crtbegin.o(.ctors)
		    *crtbegin?.o(.ctors)
		    *(EXCLUDE_FILE(*crtend?.o *crtend.o) .ctors)
		    *(SORT(.ctors.*))
		    *(.ctors)

		    /* .dtors */
     		*crtbegin.o(.dtors)
     		*crtbegin?.o(.dtors)
     		*(EXCLUDE_FILE(*crtend?.o *crtend.o) .dtors)
     		*(SORT(.dtors.*))
     		*(.dtors)

		    *(.rodata*)

		    KEEP(*(.eh_frame*))
        _etext = .;
    } > FLASH

    .data (READONLY): AT (ADDR(.text) + SIZEOF(.text)) /* The "READONLY" keyword is only supported in GCC11 and later, remove it if using GCC10 or earlier. */
    {
        _data = .;
        *(vtable)
        *(.data*)
        *(.ram)
        . = ALIGN (8);
        _edata = .;
    } > SRAM

    .bss (READONLY): /* The "READONLY" keyword is only supported in GCC11 and later, remove it if using GCC10 or earlier. */
    {
        _bss = .;
        *(.bss*)
        *(COMMON)
        . = ALIGN (8);
        *(.ram.b .bss.ram)
        _ebss = .;
        _stack = .;
        . = ALIGN(MAX(_stack + __STACKSIZE__ , .), 4);
        _estack = .;
        
    } > SRAM
}
