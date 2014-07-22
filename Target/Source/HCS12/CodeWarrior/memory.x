NAMES END /* CodeWarrior will pass all the needed files to the linker by command line. But here you may add your own files too. */

SEGMENTS  /* Here all RAM/ROM areas of the device are listed. Used in PLACEMENT below. */
      /* RAM */
      RAM           = READ_WRITE    0x3800 TO   0x3FFF;
      /* non-paged FLASHs */
      ROM_C000      = READ_ONLY     0xe800 TO   0xFEDF;
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
                          INTO  ROM_C000;
      SSTACK,                 /* allocate stack first to avoid overwriting variables on overflow */
      DEFAULT_RAM         INTO  RAM;
END

ENTRIES /* keep the following unreferenced variables/functios */
    _vectab
END

STACKSIZE 0x100


