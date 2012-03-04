MEMORY
{
    FLASH (rx) : ORIGIN = 0x08000000, LENGTH = 16K
    SRAM (rwx) : ORIGIN = 0x20000000, LENGTH = 6K
}

SECTIONS
{
    __STACKSIZE__ = 256;

    .text :
    {
        KEEP(*(.isr_vector))
        KEEP(*(.entry))
        *(.entry*)
        *(.text*)
        *(.rodata*)
        _etext = .;
    } > FLASH

    .data : AT (ADDR(.text) + SIZEOF(.text))
    {
        _data = .;
        *(vtable)
        *(.data*)
        _edata = .;
    } > SRAM

    .bss :
    {
        _bss = .;
        *(.bss*)
        *(COMMON)
        _ebss = .;
        _stack = .;
        . = ALIGN(MAX(_stack + __STACKSIZE__ , .), 4);
        _estack = .;
        
    } > SRAM
}
