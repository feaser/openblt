MEMORY
{
    FLASH (rx) : ORIGIN = 0x00000000, LENGTH = 8K
    SRAM (rwx) : ORIGIN = 0x20000000, LENGTH = 4K
}

SECTIONS
{
    __STACKSIZE__ = 256;

    .text :
    {
        KEEP(*(.isr_vector))
        *(.text*)
        *(.rodata*)
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
