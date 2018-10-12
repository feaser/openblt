// ver 1.1 (7/7/04)
// HCS12X Core erasing + unsecuring command file:
// These commands mass erase the chip then program the security byte to 0xFE (unsecured state).

// Evaluate the clock divider to set in ECLKDIV/FCLKDIV registers:

DEFINEVALUEDLG "Information required to unsecure the device" "CLKDIV" 0x49 "To unsecure the device, the command script needs \nthe correct value for ECLKDIV/FCLKDIV onchip\nregisters.\nIf the bus frequency is less than 10 MHz, the value\nto store in ECLKDIV/FCLKDIV is equal to:\n \"bus frequency (kHz) / 175\"\n\nIf the bus frequency is higher than 10 MHz, the value\nto store in ECLKDIV/FCLKDIV is equal to:\n \" bus frequency (kHz) / 1400  + 64\"\n(+64 (0x40) is to set PRDIV8 flag)\n\nDatasheet proposed values:\n\nbus frequency\t\tE/FCLKDIV value (decimal)\n\n 16 \tMHz\t\t73\n  8 \tMHz\t\t39\n  4 \tMHz\t\t19\n  2 \tMHz\t\t9\n  1 \tMHz\t\t4\n"

// An average programming clock of 175 kHz is chosen.

// If the oscillator frequency is less than 10 MHz, the value to store
// in ECLKDIV/FCLKDIV is equal to " oscillator frequency (kHz) / 175 ".

// If the oscillator frequency is higher than 10 MHz, the value to store
// in ECLKDIV/FCLKDIV is equal to " oscillator frequency (kHz) / 1400  + 0x40 (to set PRDIV8 flag)".

// Datasheet proposed values:
//
// oscillator frequency     ECLKDIV/FCLKDIV value (hexadecimal)
// 
//  16 MHz            		$49
//   8 MHz            		$27
//   4 MHz            		$13
//   2 MHz             		$9
//   1 MHz             		$4


FLASH RELEASE   // do not interact with regular flash programming monitor

//mass erase flash
reset
wb 0x03c 0x00	 //disable cop
wait 20
wb 0x100 CLKDIV  // set FCLKDIV clock divider
wb 0x104 0xFF    // FPROT all protection disabled
wb 0x105 0x30    // clear PVIOL and ACCERR in FSTAT register 
wb 0x102 0x00    // clear the WRALL bit in FTSTMOD
wb 0x105 0x02
wb 0x102 0x10    // set the WRALL bit in FTSTMOD to affect all blocks
ww 0x108 0xFFFE
ww 0x10A 0xFFFF
wb 0x106 0x41    // write MASS ERASE command in FCMD register
wb 0x105 0x80    // clear CBEIF in FSTAT register to execute the command 
wait 20          // wait for command to complete

//mass erase eeprom
wb 0x110 CLKDIV  // set ECLKDV clock divider 
wb 0x114 0xFF    // EPROT all protection disabled
wb 0x115 0x30    // clear PVIOL and ACCERR in ESTAT register 
wb 0x112 0x00    // clear the WRALL bit in FTSTMOD
wb 0x115 0x02
ww 0x118 0x0C00  // write to EADDR eeprom address register
ww 0x11A 0x0000  // write to EDATA eeprom data register
wb 0x116 0x41    // write MASS ERASE command in ECMD register
wb 0x115 0x80    // clear CBEIF in ESTAT register to execute the command
wait 20          // wait for command to complete

//reprogram Security byte to Unsecure state
reset
wb 0x03c 0x00	//disable cop
wait 20
wb 0x102 0x00    // clear the WRALL bit in FTSTMOD
wb 0x105 0x02
wb 0x100 CLKDIV  // set FCLKDIV clock divider
wb 0x100 CLKDIV  // set FCLKDIV clock divider
wb 0x104 0xFF    // FPROT all protection disabled
wb 0x105 0x30    // clear PVIOL and ACCERR in FSTAT register 
wb 0x102 0x00    // clear the WRALL bit in FTSTMOD
wb 0x105 0x02
ww 0xFF0E 0xFFFE // write security byte to "Unsecured" state
wb 0x106 0x20    // write MEMORY PROGRAM command in FCMD register
wb 0x105 0x80    // clear CBEIF in FSTAT register to execute the command 
wait 20          // wait for command to complete

reset

undef CLKDIV     // undefine variable 

