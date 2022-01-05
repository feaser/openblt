// ver 0.1 (09-Mar-08)
// HCS12P Core erasing + unsecuring command file:
// These commands mass erase the chip then program the security byte to 0xFE (unsecured state).

DEFINEVALUEDLG "Information required to unsecure the device" "FCLKDIV" 17 "To unsecure the device, the command script needs \nthe correct value for the FCLKDIV onchip register.\n\nDatasheet proposed values:\n\noscillator frequency\tFCLKDIV value (decimal)\n\n 16 \tMHz\t\t17\n 12 \tMHz\t\t13\n  8 \tMHz\t\t9\n  4 \tMHz\t\t5\n"

FLASH RELEASE

reset
wb 0x03c 0x00	 //disable cop
wait 20


WB 0x100 FCLKDIV // clock divider

WB 0x106 0x30   // clear any error flags
WB 0x102 0x00   // CCOBIX = 0
WB 0x10A 0x08   // load erase all blocks command
WB 0x106 0x80   // launch command
WAIT 10

reset

WB 0x100 FCLKDIV // clock divider
WB 0x106 0x30   // clear any error flags
WB 0x102 0x00   // CCOBIX = 0
WB 0x10A 0x06   // load program command
WB 0x10B 0x03   // load GPAGE
WB 0x102 0x01   // CCOBIX = 1
WB 0x10A 0xFF   // load addr hi
WB 0x10B 0x08   // load addr lo
WB 0x102 0x02   // CCOBIX = 2
WB 0x10A 0xFF   // load data
WB 0x10B 0xFF   // load data
WB 0x102 0x03   // CCOBIX = 3
WB 0x10A 0xFF   // load data
WB 0x10B 0xFF   // load data
WB 0x102 0x04   // CCOBIX = 4
WB 0x10A 0xFF   // load data
WB 0x10B 0xFF   // load data
WB 0x102 0x05   // CCOBIX = 5
WB 0x10A 0xFF   // load data
WB 0x10B 0xFE   // load data
WB 0x106 0x80   // launch command
WAIT 1

undef FCLKDIV   // undefine variable 
