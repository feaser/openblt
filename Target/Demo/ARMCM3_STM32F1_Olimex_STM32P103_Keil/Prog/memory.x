; *************************************************************
; *** Scatter-Loading Description File for Demo Program     ***
; *************************************************************

LR_IROM1 0x08006000 0x0001A000  {    ; load region size_region
  ER_IROM1 0x08006000 0x0001A000  {  ; load address = execution address
   *.o (RESET, +First)
   *(InRoot$$Sections)
   .ANY (+RO)
  }
  RW_IRAM1 0x20000000 0x00005000  {  ; RW data
   .ANY (+RW +ZI)
  }
}

