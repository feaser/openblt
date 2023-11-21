/* This is a linker parameter file for the MC9S12DG256B */
NAMES END /* CodeWarrior will pass all the needed files to the linker by command line. But here you may add your own files too. */

SEGMENTS  /* Here all RAM/ROM areas of the device are listed. Used in PLACEMENT below. */

/* Register space  */
/*    IO_SEG        = PAGED         0x0000 TO   0x03FF; intentionally not defined */

/* EPROM */
      EEPROM        = READ_ONLY     0x0400 TO   0x0FEF;

/* RAM */
      RAM           = READ_WRITE    0x1000 TO   0x3FFF;

/* non-paged FLASHs */
      ROM_4000      = READ_ONLY     0x4000 TO   0x7FFF;
      ROM_C000      = READ_ONLY     0xC000 TO   0xE7FF; /* last part reserved for OpenBLT */

/* paged FLASH:                     0x8000 TO   0xBFFF; addressed through PPAGE */
      PAGE_30       = READ_ONLY   0x308000 TO 0x30BFFF;
      PAGE_31       = READ_ONLY   0x318000 TO 0x31BFFF;
      PAGE_32       = READ_ONLY   0x328000 TO 0x32BFFF;
      PAGE_33       = READ_ONLY   0x338000 TO 0x33BFFF;
      PAGE_34       = READ_ONLY   0x348000 TO 0x34BFFF;
      PAGE_35       = READ_ONLY   0x358000 TO 0x35BFFF;
      PAGE_36       = READ_ONLY   0x368000 TO 0x36BFFF;
      PAGE_37       = READ_ONLY   0x378000 TO 0x37BFFF;
      PAGE_38       = READ_ONLY   0x388000 TO 0x38BFFF;
      PAGE_39       = READ_ONLY   0x398000 TO 0x39BFFF;
      PAGE_3A       = READ_ONLY   0x3A8000 TO 0x3ABFFF;
      PAGE_3B       = READ_ONLY   0x3B8000 TO 0x3BBFFF;
      PAGE_3C       = READ_ONLY   0x3C8000 TO 0x3CBFFF;
      PAGE_3D       = READ_ONLY   0x3D8000 TO 0x3DBFFF;
/*    PAGE_3E       = READ_ONLY   0x3E8000 TO 0x3EBFFF; not used: equivalent to ROM_4000 */
/*    PAGE_3F       = READ_ONLY   0x3F8000 TO 0x3FBEFF; not used: equivalent to ROM_C000 */
END

PLACEMENT /* here all predefined and user segments are placed into the SEGMENTS defined above. */
      _PRESTART,              /* Used in HIWARE format: jump to _Startup at the code start */
      STARTUP,                /* startup data structures */
      ROM_VAR,                /* constant variables */
      STRINGS,                /* string literals */
      VIRTUAL_TABLE_SEGMENT,  /* C++ virtual table segment */
      DEFAULT_ROM, NON_BANKED,             /* runtime routines which must not be banked */
      COPY                    /* copy down information: how to initialize variables */
                              /* in case you want to use ROM_4000 here as well, make sure
                                 that all files (incl. library files) are compiled with the
                                 option: -OnB=b */
                        INTO  ROM_C000/*, ROM_4000*/;

      OTHER_ROM         INTO  PAGE_30, PAGE_31, PAGE_32, PAGE_33, PAGE_34, PAGE_35, PAGE_36, PAGE_37, 
                              PAGE_38, PAGE_39, PAGE_3A, PAGE_3B, PAGE_3C, PAGE_3D                  ;

      SSTACK,                 /* allocate stack first to avoid overwriting variables on overflow */
    DEFAULT_RAM         INTO  RAM;

END

ENTRIES /* keep the following unreferenced variables */
    _vectab
END

STACKSIZE 0x100


