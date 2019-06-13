/*###ICF### Section handled by ICF editor, don't touch! ****/
/*-Editor annotation file-*/
/* IcfEditorFile="$TOOLKIT_DIR$\config\ide\IcfEditor\cortex_v1_1.xml" */
/*-Specials-*/
define symbol __ICFEDIT_intvec_start__ = 0x0800C000;
/*-Memory Regions-*/
define symbol __ICFEDIT_region_IROM1_start__  = 0x0800C000;
define symbol __ICFEDIT_region_IROM1_end__    = 0x081FFFFF;
define symbol __ICFEDIT_region_IROM2_start__  = 0x0;
define symbol __ICFEDIT_region_IROM2_end__    = 0x0;
define symbol __ICFEDIT_region_EROM1_start__  = 0x0;
define symbol __ICFEDIT_region_EROM1_end__    = 0x0;
define symbol __ICFEDIT_region_EROM2_start__  = 0x0;
define symbol __ICFEDIT_region_EROM2_end__    = 0x0;
define symbol __ICFEDIT_region_EROM3_start__  = 0x0;
define symbol __ICFEDIT_region_EROM3_end__    = 0x0;
define symbol __ICFEDIT_region_SHARED_start__ = 0x1FFE8000;
define symbol __ICFEDIT_region_SHARED_end__   = 0x1FFE803F;
define symbol __ICFEDIT_region_IRAM1_start__  = 0x1FFE8040;
define symbol __ICFEDIT_region_IRAM1_end__    = 0x1FFFFFFF;
define symbol __ICFEDIT_region_IRAM2_start__  = 0x20000000;
define symbol __ICFEDIT_region_IRAM2_end__    = 0x2001FFFF;
define symbol __ICFEDIT_region_IRAM3_start__  = 0x20020000;
define symbol __ICFEDIT_region_IRAM3_end__    = 0x2003FFFF;
define symbol __ICFEDIT_region_ERAM1_start__  = 0x60000000;
define symbol __ICFEDIT_region_ERAM1_end__    = 0x6FFFFFFF;
define symbol __ICFEDIT_region_ERAM2_start__  = 0xA0000000;
define symbol __ICFEDIT_region_ERAM2_end__    = 0xAFFFFFFF;
define symbol __ICFEDIT_region_ERAM3_start__  = 0x0;
define symbol __ICFEDIT_region_ERAM3_end__    = 0x0;
/*-Sizes-*/
define symbol __ICFEDIT_size_cstack__ = 0x800;
define symbol __ICFEDIT_size_heap__   = 0x1000;
/**** End of ICF editor section. ###ICF###*/

define memory mem with size = 4G;
define region IROM_region   =   mem:[from __ICFEDIT_region_IROM1_start__ to __ICFEDIT_region_IROM1_end__]
                              | mem:[from __ICFEDIT_region_IROM2_start__ to __ICFEDIT_region_IROM2_end__];
define region EROM_region   =   mem:[from __ICFEDIT_region_EROM1_start__ to __ICFEDIT_region_EROM1_end__]
                              | mem:[from __ICFEDIT_region_EROM2_start__ to __ICFEDIT_region_EROM2_end__]
                              | mem:[from __ICFEDIT_region_EROM3_start__ to __ICFEDIT_region_EROM3_end__];
define region IRAM_region   =   mem:[from __ICFEDIT_region_IRAM1_start__ to __ICFEDIT_region_IRAM1_end__]
                              | mem:[from __ICFEDIT_region_IRAM3_start__ to __ICFEDIT_region_IRAM3_end__];
define region IRAM2_region  =   mem:[from __ICFEDIT_region_IRAM2_start__ to __ICFEDIT_region_IRAM2_end__];
define region ERAM_region   =   mem:[from __ICFEDIT_region_ERAM1_start__ to __ICFEDIT_region_ERAM1_end__]
                              | mem:[from __ICFEDIT_region_ERAM2_start__ to __ICFEDIT_region_ERAM2_end__]
                              | mem:[from __ICFEDIT_region_ERAM3_start__ to __ICFEDIT_region_ERAM3_end__];


define block CSTACK    with alignment = 8, size = __ICFEDIT_size_cstack__   { };
define block HEAP      with alignment = 8, size = __ICFEDIT_size_heap__     { };

do not initialize  { section .noinit };
initialize by copy { readwrite };
if (isdefinedsymbol(__USE_DLIB_PERTHREAD))
{
  // Required in a multi-threaded application
  initialize by copy with packing = none { section __DLIB_PERTHREAD };
}

place at address mem:__ICFEDIT_intvec_start__ { readonly section .intvec };
place at address mem:__ICFEDIT_region_SHARED_start__ { readwrite section .shared };

place in IROM_region  { readonly };
place in EROM_region  { readonly section application_specific_ro };
place in IRAM_region  { readwrite, block CSTACK, block HEAP };
place in IRAM2_region { readwrite section .iram2 };
place in ERAM_region  { readwrite section application_specific_rw };
