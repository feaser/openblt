MEMORY
{
  UNPLACED_SECTIONS (wx) : ORIGIN = 0x100000000, LENGTH = 0
  AHB_Peripherals (wx) : ORIGIN = 0xffe00000, LENGTH = 0x00200000
  VPB_Peripherals (wx) : ORIGIN = 0xe0000000, LENGTH = 0x00200000
  BANK3 (wx) : ORIGIN = 0x83000000, LENGTH = 0x01000000
  BANK2 (wx) : ORIGIN = 0x82000000, LENGTH = 0x01000000
  External_SRAM (wx) : ORIGIN = 0x81000000, LENGTH = 0x00100000
  External_FLASH (rx) : ORIGIN = 0x80000000, LENGTH = 0x00400000
  SRAM (wx) : ORIGIN = 0x40000200, LENGTH = 0x00001CE0
  FLASH (rx) : ORIGIN = 0x00000000, LENGTH = 0x00002000
}


SECTIONS
{
  __AHB_Peripherals_segment_start__ = 0xffe00000;
  __AHB_Peripherals_segment_end__ = 0x00000000;
  __VPB_Peripherals_segment_start__ = 0xe0000000;
  __VPB_Peripherals_segment_end__ = 0xe0200000;
  __BANK3_segment_start__ = 0x83000000;
  __BANK3_segment_end__ = 0x84000000;
  __BANK2_segment_start__ = 0x82000000;
  __BANK2_segment_end__ = 0x83000000;
  __External_SRAM_segment_start__ = 0x81000000;
  __External_SRAM_segment_end__ = 0x81100000;
  __External_FLASH_segment_start__ = 0x80000000;
  __External_FLASH_segment_end__ = 0x80400000;
  __SRAM_segment_start__ = 0x40000200;
  __SRAM_segment_end__ = 0x40001EE0;
  __FLASH_segment_start__ = 0x00000000;
  __FLASH_segment_end__ = 0x00002000;

  __STACKSIZE__ = 1024;
  __STACKSIZE_IRQ__ = 256;
  __STACKSIZE_FIQ__ = 256;
  __STACKSIZE_SVC__ = 0;
  __STACKSIZE_ABT__ = 0;
  __STACKSIZE_UND__ = 0;
  __HEAPSIZE__ = 1024;

  __text2_load_start__ = ALIGN(__External_FLASH_segment_start__ , 4);
  .text2 ALIGN(__External_FLASH_segment_start__ , 4) : AT(ALIGN(__External_FLASH_segment_start__ , 4))
  {
    __text2_start__ = .;
    *(.text2 .text2.*)
  }
  __text2_end__ = __text2_start__ + SIZEOF(.text2);

  __text2_load_end__ = __text2_end__;

  . = ASSERT(__text2_end__ >= __External_FLASH_segment_start__ && __text2_end__ <= (__External_FLASH_segment_start__ + 0x00400000) , "error: .text2 is too large to fit in External_FLASH memory segment");

  __rodata2_load_start__ = ALIGN(__text2_end__ , 4);
  .rodata2 ALIGN(__text2_end__ , 4) : AT(ALIGN(__text2_end__ , 4))
  {
    __rodata2_start__ = .;
    *(.rodata2 .rodata2.*)
  }
  __rodata2_end__ = __rodata2_start__ + SIZEOF(.rodata2);

  __rodata2_load_end__ = __rodata2_end__;

  . = ASSERT(__rodata2_end__ >= __External_FLASH_segment_start__ && __rodata2_end__ <= (__External_FLASH_segment_start__ + 0x00400000) , "error: .rodata2 is too large to fit in External_FLASH memory segment");

  __data2_load_start__ = ALIGN(__rodata2_end__ , 4);
  .data2 ALIGN(__External_SRAM_segment_start__ , 4) : AT(ALIGN(__rodata2_end__ , 4))
  {
    __data2_start__ = .;
    *(.data2 .data2.*)
  }
  __data2_end__ = __data2_start__ + SIZEOF(.data2);

  __data2_load_end__ = __data2_load_start__ + SIZEOF(.data2);

  __External_FLASH_segment_used_end__ = ALIGN(__rodata2_end__ , 4) + SIZEOF(.data2);

  . = ASSERT((__data2_load_start__ + SIZEOF(.data2)) >= __External_FLASH_segment_start__ && (__data2_load_start__ + SIZEOF(.data2)) <= (__External_FLASH_segment_start__ + 0x00400000) , "error: .data2 is too large to fit in External_FLASH memory segment");

  .data2_run ALIGN(__External_SRAM_segment_start__ , 4) (NOLOAD) :
  {
    __data2_run_start__ = .;
    . = MAX(__data2_run_start__ + SIZEOF(.data2), .);
  }
  __data2_run_end__ = __data2_run_start__ + SIZEOF(.data2_run);

  __data2_run_load_end__ = __data2_run_end__;

  . = ASSERT(__data2_run_end__ >= __External_SRAM_segment_start__ && __data2_run_end__ <= (__External_SRAM_segment_start__ + 0x00100000) , "error: .data2_run is too large to fit in External_SRAM memory segment");

  __bss2_load_start__ = ALIGN(__data2_run_end__ , 4);
  .bss2 ALIGN(__data2_run_end__ , 4) (NOLOAD) : AT(ALIGN(__data2_run_end__ , 4))
  {
    __bss2_start__ = .;
    *(.bss2 .bss2.*)
  }
  __bss2_end__ = __bss2_start__ + SIZEOF(.bss2);

  __bss2_load_end__ = __bss2_end__;

  __External_SRAM_segment_used_end__ = ALIGN(__data2_run_end__ , 4) + SIZEOF(.bss2);

  . = ASSERT(__bss2_end__ >= __External_SRAM_segment_start__ && __bss2_end__ <= (__External_SRAM_segment_start__ + 0x00100000) , "error: .bss2 is too large to fit in External_SRAM memory segment");

  __vectors_ram_load_start__ = __SRAM_segment_start__;
  .vectors_ram __SRAM_segment_start__ (NOLOAD) : AT(__SRAM_segment_start__)
  {
    __vectors_ram_start__ = .;
    *(.vectors_ram .vectors_ram.*)
    . = MAX(__vectors_ram_start__ + 0x0000003C , .);
  }
  __vectors_ram_end__ = __vectors_ram_start__ + SIZEOF(.vectors_ram);

  __vectors_ram_load_end__ = __vectors_ram_end__;

  . = ASSERT(__vectors_ram_end__ >= __SRAM_segment_start__ && __vectors_ram_end__ <= (__SRAM_segment_start__ + 0x00004000) , "error: .vectors_ram is too large to fit in SRAM memory segment");

  __vectors_load_start__ = __FLASH_segment_start__;
  .vectors __FLASH_segment_start__ : AT(__FLASH_segment_start__)
  {
    __vectors_start__ = .;
    *(.vectors .vectors.*)
  }
  __vectors_end__ = __vectors_start__ + SIZEOF(.vectors);

  __vectors_load_end__ = __vectors_end__;

  . = ASSERT(__vectors_end__ >= __FLASH_segment_start__ && __vectors_end__ <= (__FLASH_segment_start__ + 0x00040000) , "error: .vectors is too large to fit in FLASH memory segment");

  __init_load_start__ = ALIGN(__vectors_end__ , 4);
  .init ALIGN(__vectors_end__ , 4) : AT(ALIGN(__vectors_end__ , 4))
  {
    __init_start__ = .;
    *(.init .init.*)
  }
  __init_end__ = __init_start__ + SIZEOF(.init);

  __init_load_end__ = __init_end__;

  . = ASSERT(__init_end__ >= __FLASH_segment_start__ && __init_end__ <= (__FLASH_segment_start__ + 0x00040000) , "error: .init is too large to fit in FLASH memory segment");

  __text_load_start__ = ALIGN(__init_end__ , 4);
  .text ALIGN(__init_end__ , 4) : AT(ALIGN(__init_end__ , 4))
  {
    __text_start__ = .;
    *(.text .text.* .glue_7t .glue_7 .gnu.linkonce.t.* .gcc_except_table)
  }
  __text_end__ = __text_start__ + SIZEOF(.text);

  __text_load_end__ = __text_end__;

  . = ASSERT(__text_end__ >= __FLASH_segment_start__ && __text_end__ <= (__FLASH_segment_start__ + 0x00040000) , "error: .text is too large to fit in FLASH memory segment");

  __dtors_load_start__ = ALIGN(__text_end__ , 4);
  .dtors ALIGN(__text_end__ , 4) : AT(ALIGN(__text_end__ , 4))
  {
    __dtors_start__ = .;
    KEEP (*(SORT(.dtors.*))) KEEP (*(.dtors))
  }
  __dtors_end__ = __dtors_start__ + SIZEOF(.dtors);

  __dtors_load_end__ = __dtors_end__;

  . = ASSERT(__dtors_end__ >= __FLASH_segment_start__ && __dtors_end__ <= (__FLASH_segment_start__ + 0x00040000) , "error: .dtors is too large to fit in FLASH memory segment");

  __ctors_load_start__ = ALIGN(__dtors_end__ , 4);
  .ctors ALIGN(__dtors_end__ , 4) : AT(ALIGN(__dtors_end__ , 4))
  {
    __ctors_start__ = .;
    KEEP (*(SORT(.ctors.*))) KEEP (*(.ctors))
  }
  __ctors_end__ = __ctors_start__ + SIZEOF(.ctors);

  __ctors_load_end__ = __ctors_end__;

  . = ASSERT(__ctors_end__ >= __FLASH_segment_start__ && __ctors_end__ <= (__FLASH_segment_start__ + 0x00040000) , "error: .ctors is too large to fit in FLASH memory segment");

  __rodata_load_start__ = ALIGN(__ctors_end__ , 4);
  .rodata ALIGN(__ctors_end__ , 4) : AT(ALIGN(__ctors_end__ , 4))
  {
    __rodata_start__ = .;
    *(.rodata .rodata.* .gnu.linkonce.r.*)
  }
  __rodata_end__ = __rodata_start__ + SIZEOF(.rodata);

  __rodata_load_end__ = __rodata_end__;

  . = ASSERT(__rodata_end__ >= __FLASH_segment_start__ && __rodata_end__ <= (__FLASH_segment_start__ + 0x00040000) , "error: .rodata is too large to fit in FLASH memory segment");

  __data_load_start__ = ALIGN(__rodata_end__ , 4);
  .data ALIGN(__vectors_ram_end__ , 4) : AT(ALIGN(__rodata_end__ , 4))
  {
    __data_start__ = .;
    *(.data .data.* .gnu.linkonce.d.*)
  }
  __data_end__ = __data_start__ + SIZEOF(.data);

  __data_load_end__ = __data_load_start__ + SIZEOF(.data);

  . = ASSERT((__data_load_start__ + SIZEOF(.data)) >= __FLASH_segment_start__ && (__data_load_start__ + SIZEOF(.data)) <= (__FLASH_segment_start__ + 0x00040000) , "error: .data is too large to fit in FLASH memory segment");

  .data_run ALIGN(__vectors_ram_end__ , 4) (NOLOAD) :
  {
    __data_run_start__ = .;
    . = MAX(__data_run_start__ + SIZEOF(.data), .);
  }
  __data_run_end__ = __data_run_start__ + SIZEOF(.data_run);

  __data_run_load_end__ = __data_run_end__;

  . = ASSERT(__data_run_end__ >= __SRAM_segment_start__ && __data_run_end__ <= (__SRAM_segment_start__ + 0x00004000) , "error: .data_run is too large to fit in SRAM memory segment");

  __bss_load_start__ = ALIGN(__data_run_end__ , 4);
  .bss ALIGN(__data_run_end__ , 4) (NOLOAD) : AT(ALIGN(__data_run_end__ , 4))
  {
    __bss_start__ = .;
    *(.bss .bss.* .gnu.linkonce.b.*) *(COMMON)
  }
  __bss_end__ = __bss_start__ + SIZEOF(.bss);

  __bss_load_end__ = __bss_end__;

  . = ASSERT(__bss_end__ >= __SRAM_segment_start__ && __bss_end__ <= (__SRAM_segment_start__ + 0x00004000) , "error: .bss is too large to fit in SRAM memory segment");

  __non_init_load_start__ = ALIGN(__bss_end__ , 4);
  .non_init ALIGN(__bss_end__ , 4) (NOLOAD) : AT(ALIGN(__bss_end__ , 4))
  {
    __non_init_start__ = .;
    *(.non_init .non_init.*)
  }
  __non_init_end__ = __non_init_start__ + SIZEOF(.non_init);

  __non_init_load_end__ = __non_init_end__;

  . = ASSERT(__non_init_end__ >= __SRAM_segment_start__ && __non_init_end__ <= (__SRAM_segment_start__ + 0x00004000) , "error: .non_init is too large to fit in SRAM memory segment");

  __heap_load_start__ = ALIGN(__non_init_end__ , 4);
  .heap ALIGN(__non_init_end__ , 4) (NOLOAD) : AT(ALIGN(__non_init_end__ , 4))
  {
    __heap_start__ = .;
    *(.heap .heap.*)
    . = ALIGN(MAX(__heap_start__ + __HEAPSIZE__ , .), 4);
  }
  __heap_end__ = __heap_start__ + SIZEOF(.heap);

  __heap_load_end__ = __heap_end__;

  . = ASSERT(__heap_end__ >= __SRAM_segment_start__ && __heap_end__ <= (__SRAM_segment_start__ + 0x00004000) , "error: .heap is too large to fit in SRAM memory segment");

  __stack_load_start__ = ALIGN(__heap_end__ , 4);
  .stack ALIGN(__heap_end__ , 4) (NOLOAD) : AT(ALIGN(__heap_end__ , 4))
  {
    __stack_start__ = .;
    *(.stack .stack.*)
    . = ALIGN(MAX(__stack_start__ + __STACKSIZE__ , .), 4);
  }
  __stack_end__ = __stack_start__ + SIZEOF(.stack);

  __stack_load_end__ = __stack_end__;

  . = ASSERT(__stack_end__ >= __SRAM_segment_start__ && __stack_end__ <= (__SRAM_segment_start__ + 0x00004000) , "error: .stack is too large to fit in SRAM memory segment");

  __stack_irq_load_start__ = ALIGN(__stack_end__ , 4);
  .stack_irq ALIGN(__stack_end__ , 4) (NOLOAD) : AT(ALIGN(__stack_end__ , 4))
  {
    __stack_irq_start__ = .;
    *(.stack_irq .stack_irq.*)
    . = ALIGN(MAX(__stack_irq_start__ + __STACKSIZE_IRQ__ , .), 4);
  }
  __stack_irq_end__ = __stack_irq_start__ + SIZEOF(.stack_irq);

  __stack_irq_load_end__ = __stack_irq_end__;

  . = ASSERT(__stack_irq_end__ >= __SRAM_segment_start__ && __stack_irq_end__ <= (__SRAM_segment_start__ + 0x00004000) , "error: .stack_irq is too large to fit in SRAM memory segment");

  __stack_fiq_load_start__ = ALIGN(__stack_irq_end__ , 4);
  .stack_fiq ALIGN(__stack_irq_end__ , 4) (NOLOAD) : AT(ALIGN(__stack_irq_end__ , 4))
  {
    __stack_fiq_start__ = .;
    *(.stack_fiq .stack_fiq.*)
    . = ALIGN(MAX(__stack_fiq_start__ + __STACKSIZE_FIQ__ , .), 4);
  }
  __stack_fiq_end__ = __stack_fiq_start__ + SIZEOF(.stack_fiq);

  __stack_fiq_load_end__ = __stack_fiq_end__;

  . = ASSERT(__stack_fiq_end__ >= __SRAM_segment_start__ && __stack_fiq_end__ <= (__SRAM_segment_start__ + 0x00004000) , "error: .stack_fiq is too large to fit in SRAM memory segment");

  __stack_svc_load_start__ = ALIGN(__stack_fiq_end__ , 4);
  .stack_svc ALIGN(__stack_fiq_end__ , 4) (NOLOAD) : AT(ALIGN(__stack_fiq_end__ , 4))
  {
    __stack_svc_start__ = .;
    *(.stack_svc .stack_svc.*)
    . = ALIGN(MAX(__stack_svc_start__ + __STACKSIZE_SVC__ , .), 4);
  }
  __stack_svc_end__ = __stack_svc_start__ + SIZEOF(.stack_svc);

  __stack_svc_load_end__ = __stack_svc_end__;

  . = ASSERT(__stack_svc_end__ >= __SRAM_segment_start__ && __stack_svc_end__ <= (__SRAM_segment_start__ + 0x00004000) , "error: .stack_svc is too large to fit in SRAM memory segment");

  __stack_abt_load_start__ = ALIGN(__stack_svc_end__ , 4);
  .stack_abt ALIGN(__stack_svc_end__ , 4) (NOLOAD) : AT(ALIGN(__stack_svc_end__ , 4))
  {
    __stack_abt_start__ = .;
    *(.stack_abt .stack_abt.*)
    . = ALIGN(MAX(__stack_abt_start__ + __STACKSIZE_ABT__ , .), 4);
  }
  __stack_abt_end__ = __stack_abt_start__ + SIZEOF(.stack_abt);

  __stack_abt_load_end__ = __stack_abt_end__;

  . = ASSERT(__stack_abt_end__ >= __SRAM_segment_start__ && __stack_abt_end__ <= (__SRAM_segment_start__ + 0x00004000) , "error: .stack_abt is too large to fit in SRAM memory segment");

  __stack_und_load_start__ = ALIGN(__stack_abt_end__ , 4);
  .stack_und ALIGN(__stack_abt_end__ , 4) (NOLOAD) : AT(ALIGN(__stack_abt_end__ , 4))
  {
    __stack_und_start__ = .;
    *(.stack_und .stack_und.*)
    . = ALIGN(MAX(__stack_und_start__ + __STACKSIZE_UND__ , .), 4);
  }
  __stack_und_end__ = __stack_und_start__ + SIZEOF(.stack_und);

  __stack_und_load_end__ = __stack_und_end__;

  . = ASSERT(__stack_und_end__ >= __SRAM_segment_start__ && __stack_und_end__ <= (__SRAM_segment_start__ + 0x00004000) , "error: .stack_und is too large to fit in SRAM memory segment");

  __fast_load_start__ = ALIGN(__data_load_start__ + SIZEOF(.data) , 4);
  .fast ALIGN(__stack_und_end__ , 4) : AT(ALIGN(__data_load_start__ + SIZEOF(.data) , 4))
  {
    __fast_start__ = .;
    *(.fast .fast.*)
  }
  __fast_end__ = __fast_start__ + SIZEOF(.fast);

  __fast_load_end__ = __fast_load_start__ + SIZEOF(.fast);

  __FLASH_segment_used_end__ = ALIGN(__data_load_start__ + SIZEOF(.data) , 4) + SIZEOF(.fast);

  . = ASSERT((__fast_load_start__ + SIZEOF(.fast)) >= __FLASH_segment_start__ && (__fast_load_start__ + SIZEOF(.fast)) <= (__FLASH_segment_start__ + 0x00040000) , "error: .fast is too large to fit in FLASH memory segment");

  .fast_run ALIGN(__stack_und_end__ , 4) (NOLOAD) :
  {
    __fast_run_start__ = .;
    . = MAX(__fast_run_start__ + SIZEOF(.fast), .);
  }
  __fast_run_end__ = __fast_run_start__ + SIZEOF(.fast_run);

  __fast_run_load_end__ = __fast_run_end__;

  __SRAM_segment_used_end__ = ALIGN(__stack_und_end__ , 4) + SIZEOF(.fast_run);

  . = ASSERT(__fast_run_end__ >= __SRAM_segment_start__ && __fast_run_end__ <= (__SRAM_segment_start__ + 0x00004000) , "error: .fast_run is too large to fit in SRAM memory segment");

}

