MEMORY
{
    FLASH (rx) : ORIGIN = 0x00002000, LENGTH = 120K
    SRAM (rwx) : ORIGIN = 0x20000000, LENGTH = 16K
}

SECTIONS
{
    __STACKSIZE__ = 1024;

    .text :
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

    .data : AT (ADDR(.text) + SIZEOF(.text))
    {
        _data = .;
        *(vtable)
        *(.data*)
        *(.ram)
        . = ALIGN (8);
        _edata = .;
    } > SRAM

    .bss :
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
