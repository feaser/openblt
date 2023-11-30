# Getting started

If you're new to MicroTBX-Modbus, one of the first questions will be: How I do setup a Modbus server with it? The goal of this section is to answer exactly that question. As a starting point we'll take an empty embedded software application:

```c
#include "board.h"

void main(void)
{
  /* Initialize the clock, enable peripherals and configure GPIO pins. */
  BoardInit();
    
  /* Enter the program's infinite loop. */  
  for(;;)
  {
      
  } 
}
```

We'll create a Modbus server step-by-step with the following properties:

* Communication using serial communication in RTU mode:
    * Baudrate 19200 bits/second.
    * 8 data-bits
    * even parity
    * 1 stop-bit
* Node address 10.
* 1 coil at address 0 (element number 1), representing an LED.

## Construct the transport layer object

The first step is always the construction of a transport layer object. It's the object that handles the actual transmission and reception of communication packets:

```c
/* Construct a Modbus RTU transport layer object. */
tTbxMbTp modbusTp = TbxMbRtuCreate(10U, TBX_MB_UART_PORT1, TBX_MB_UART_19200BPS,
                                   TBX_MB_UART_1_STOPBITS, TBX_MB_EVEN_PARITY);   

```

## Construct the server channel object

With the transport layer object created, we continue with constructing a server channel object and attaching the transport layer object to it:

```c
/* Construct a Modbus server object. */
tTbxMbServer modbusServer = TbxMbServerCreate(modbusTp);  
```

## Call the task function for event processing

An event task function drives the MicroTBX-Modbus stack. We just need to continuously call it in the program's infinite superloop:

```c
/* Continuously call the Modbus stack event task function. */
TbxMbEventTask();
```

## Configure the callback for handling coil writes

Our example Modbus server should enable a Modbus client to change the state of an LED, whenever it receives a coil write request. For this we'll implement a callback function, with a name of our choosing, and then register this callback function for coil write requests:

```c
tTbxMbServerResult AppWriteCoil(tTbxMbServer channel, uint16_t addr, uint8_t value)
{
  tTbxMbServerResult result = TBX_MB_SERVER_OK;
    
  /* Request to write the coil at address 0? */
  if (addr == 0U)
  {
     if (value == TBX_ON)
     {
       BoardLedOn();    
     }
     else
     {
       BoardLedOff();  
     }
  }
  /* Unsupported coil address. */
  else
  {
    result = TBX_MB_SERVER_ERR_ILLEGAL_DATA_ADDR;
  }
  return result;
}

/* Set the callback for accessing the coils in the Modbus data table. */
TbxMbServerSetCallbackWriteCoil(modbusServer, AppWriteCoil);
```

Assembling it all together into our initial empty application results in this:

```c
#include <microtbx.h>
#include <microtbxmodbus.h>
#include "board.h"

tTbxMbTp modbusTp;
tTbxMbServer modbusServer;

tTbxMbServerResult AppWriteCoil(tTbxMbServer channel, uint16_t addr, uint8_t value);

void main(void)
{
  /* Initialize the clock, enable peripherals and configure GPIO pins. */
  BoardInit();
  /* Construct a Modbus RTU transport layer object. */
  modbusTp = TbxMbRtuCreate(10, TBX_MB_UART_PORT1, TBX_MB_UART_19200BPS,
                            TBX_MB_UART_1_STOPBITS, TBX_MB_EVEN_PARITY);   
  /* Construct a Modbus server object. */
  modbusServer = TbxMbServerCreate(modbusTp); 
  /* Set the callback for accessing the coils in the Modbus data table. */
  TbxMbServerSetCallbackWriteCoil(modbusServer, AppWriteCoil);
    
  /* Enter the program's infinite loop. */  
  for(;;)
  {
    /* Continuously call the Modbus stack event task function. */
    TbxMbEventTask();
  } 
}

tTbxMbServerResult AppWriteCoil(tTbxMbServer channel, uint16_t addr, uint8_t value)
{
  tTbxMbServerResult result = TBX_MB_SERVER_ERR_ILLEGAL_DATA_ADDR;
    
  /* Request to write the coil at address 0? */
  if (addr == 0U)
  {
     if (value == TBX_ON)
     {
       BoardLedOn();    
     }
     else
     {
       BoardLedOff();  
     }
     result = TBX_MB_SERVER_OK;      
  }
  return result;
}
```

And voilà, you now have a fully functional Modbus server. You can extend it by adding support for:

* Writing holding registers.
* Reading discrete inputs.
* Reading input registers.

The process is the same: You implement the callback function and then register it with the channel using a `TbxMbServerSetCallbackXxx()` API function. Refer to the [API reference](apiref.md) for more details.

## Using FreeRTOS instead of a superloop

The previous example assumed a traditional superloop type application. Thanks to the ever increasing processing power and available RAM and ROM memory on modern microcontrollers, the use of a real-time operating system (RTOS) is more common. 

For this reason, MicroTBX-Modbus ships with an operating system abstraction layer (OSAL). In this section, we'll upgrade the previous superloop example to use FreeRTOS instead of a traditional superloop. 

### Select the correct OSAL source file

As a first step, re-configure your project to compile and link the correct OSAL source file:

* Remove `source/osal/tbxmb_superloop.c` from your project.
* Add `source/osal/tbxmb_freertos.c` to your project.

In case you use CMake to manage your project's build system, update its `target_link_libraries()`:

* Remove `microtbx-modbus-osal-superloop`.
* Add `microtbx-modbus-osal-freertos`.

### Create a new RTOS task for event handling

Instead of continuously calling `TbxMbEventTask()` in the superloop, create a new RTOS task and call `TbxMbEventTask()` in the task's infinite loop. You can assign it a priority of your liking that fits your application. Note that the MicroTBX-Modbus FreeRTOS OSAL source file automatically places the RTOS task in the waiting state, when no events are pending:

```c
void AppModbusTask(void * pvParameters)
{
  /* Enter infinite task loop. */
  for (;;)
  {
    /* Continuously call the Modbus stack event task function. */
    TbxMbEventTask();
  }
}

/* Create the Modbus task. */
xTaskCreate(AppModbusTask, "ModbusTask", configMINIMAL_STACK_SIZE, NULL, 4U, NULL);
```

Here follows to previous example application, upgraded for FreeRTOS:

```c
#include <microtbx.h>
#include <microtbxmodbus.h>
#include "board.h"
#include "FreeRTOS.h"
#include "task.h"

tTbxMbTp modbusTp;
tTbxMbServer modbusServer;

void AppModbusTask(void * pvParameters);
tTbxMbServerResult AppWriteCoil(tTbxMbServer channel, uint16_t addr, uint8_t value);

void main(void)
{
  /* Initialize the clock, enable peripherals and configure GPIO pins. */
  BoardInit();
  /* Construct a Modbus RTU transport layer object. */
  modbusTp = TbxMbRtuCreate(10, TBX_MB_UART_PORT1, TBX_MB_UART_19200BPS,
                            TBX_MB_UART_1_STOPBITS, TBX_MB_EVEN_PARITY);   
  /* Construct a Modbus server object. */
  modbusServer = TbxMbServerCreate(modbusTp); 
  /* Set the callback for accessing the coils in the Modbus data table. */
  TbxMbServerSetCallbackWriteCoil(modbusServer, AppWriteCoil);
  /* Create the Modbus task. */
  xTaskCreate(AppModbusTask, "ModbusTask", configMINIMAL_STACK_SIZE, NULL, 4U, NULL);    
  /* Start the RTOS scheduler. Note that this function does not return. */
  vTaskStartScheduler();
}

void AppModbusTask(void * pvParameters)
{
  /* Enter infinite task loop. */
  for (;;)
  {
    /* Continuously call the Modbus stack event task function. */
    TbxMbEventTask();
  }
}

tTbxMbServerResult AppWriteCoil(tTbxMbServer channel, uint16_t addr, uint8_t value)
{
  tTbxMbServerResult result = TBX_MB_SERVER_ERR_ILLEGAL_DATA_ADDR;
    
  /* Request to write the coil at address 0? */
  if (addr == 0U)
  {
     if (value == TBX_ON)
     {
       BoardLedOn();    
     }
     else
     {
       BoardLedOff();  
     }
     result = TBX_MB_SERVER_OK;      
  }
  return result;
}
```

## Next steps

After reading through this getting started section, you now have a basic understanding of how to set up a Modbus server. For ready-to-run examples, refer to the demo programs in the separate repository. It also includes example on how to set up a Modbus client, instead of a server:

* [https://github.com/feaser/microtbx-demos](https://github.com/feaser/microtbx-demos)

For more in-depth details on the API functions offered by MicroTBX-Modbus, head over to the [API reference](apiref.md) in this user manual. When your itchy to start adding MicroTBX-Modbus to your own embedded software program, continue with the [integration](integration.md) section of this user manual.
