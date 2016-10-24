; *************************************************************
; *** Scatter-Loading Description File for OpenBLT          ***
; *************************************************************

LR_IROM1 0x08000000 0x00006000  {    ; load region size_region
  ER_IROM1 0x08000000 0x00006000  {  ; load address = execution address
   *.o (RESET, +First)
   *(InRoot$$Sections)
   .ANY (+RO)
  }
  RW_IRAM1 0x20000000 0x00002000  {  ; RW data
   .ANY (+RW +ZI)
  }
}

