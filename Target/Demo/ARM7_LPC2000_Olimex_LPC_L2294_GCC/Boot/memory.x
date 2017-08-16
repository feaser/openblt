/* identify the Entry Point  */
ENTRY(_startup)

/* specify the LPC2xxx memory areas. note that the RAM is configured to be only 8kb. this is
 * the smallest in the LPC2xxx series, making this memory layout compatible with all devices 
 * in this serie of microcontrollers. 
 */
MEMORY 
{
  flash           : ORIGIN = 0x00000000, LENGTH = 8K    /* FLASH ROM reserved for the bootloader	  */	
  ram_vectors(A)  : ORIGIN = 0x40000000, LENGTH = 64    /* RAM vectors of the user program          */		 
  ram_monitor(A)  : ORIGIN = 0x40000040, LENGTH = 224   /* variables used by Philips RealMonitor    */		 
  ram_isp_low(A)  : ORIGIN = 0x40000120, LENGTH = 224   /* variables used by Philips ISP bootloader	*/		 
  ram             : ORIGIN = 0x40000200, LENGTH = 7392  /* free RAM area							              */
  ram_isp_high(A) : ORIGIN = 0x40001EE0, LENGTH = 288   /* variables used by Philips ISP bootloader	*/
}

/* define a global symbol _stack_end, placed at the very end of unused RAM */
_stack_end = 0x40001EE0 - 4;

/* now define the output sections  */
SECTIONS 
{
  . = 0;                  /* set location counter to address zero                                   */
  startup : { *(.startup)} >flash		/* the startup code goes into FLASH                             */

	.text :								  /* collect all sections that should go into FLASH after startup           */
	{
		*(.text)						  /* all .text sections (code)                                              */
		*(.rodata)						/* all .rodata sections (constants, strings, etc.)                        */
		*(.rodata*)						/* all .rodata* sections (constants, strings, etc.)                       */
		*(.glue_7)						/* all .glue_7 sections  (no idea what these are)                         */
		*(.glue_7t)						/* all .glue_7t sections (no idea what these are)                         */
		_etext = .;						/* define a global symbol _etext just after the last code byte            */
	} >flash							  /* put all the above into FLASH */
	
	.data :								  /* collect all initialized .data sections that go into RAM                */
	{
		_data = .;						/* create a global symbol marking the start of the .data section          */
		*(.data)						  /* all .data sections                                                     */
		_edata = .;						/* define a global symbol marking the end of the .data section            */
	} >ram AT >flash				/* put all the above into RAM (but load the LMA copy into FLASH)          */

	.bss :								  /* collect all uninitialized .bss sections that go into RAM               */
	{
		_bss_start = .;				/* define a global symbol marking the start of the .bss section           */
		*(.bss)							  /* all .bss sections                                                      */
	} >ram								  /* put all the above in RAM (it will be cleared in the startup code       */

	. = ALIGN(4);						/* advance location counter to the next 32-bit boundary                   */
	_bss_end = . ;					/* define a global symbol marking the end of the .bss section             */
}
	_end = .;							  /* define a global symbol marking the end of application RAM              */
	
