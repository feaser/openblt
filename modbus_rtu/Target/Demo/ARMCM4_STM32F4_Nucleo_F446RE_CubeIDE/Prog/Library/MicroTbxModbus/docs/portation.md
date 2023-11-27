# Porting to your microcontroller platform

MicroTBX-Modbus comes with a build-in hardware abstraction layer. Essentially, this means that you only need to adjust the implementation of a few functions, to get the communication stack working on your specific microcontroller system. You can find a framework for all these functions at this location:

* `source/template/tbxmb_port.c`

Furthermore, the demo programs, located in a separate repository, already include ready-made ports:

* [https://github.com/feaser/microtbx-demos](https://github.com/feaser/microtbx-demos)

 When tasked with getting MicroTBX-Modbus running on your specific microcontroller system, follow these steps:

1. First check if demo programs exists for a microcontroller similar to yours. If so, grab that version of `tbxmb_port.c` and make whatever little tweaks needed to adjust it for your microcontroller system. Chances are that it already works, without making any changes.
2. Otherwise, grab the `tbxmb_port.c` file from the `source/template/` directory and use it as a starting point. Check the source code comments that lead with `TODO ##Port`. They contain hints about what you need to implement. 

The remainder of this section explains in more detail how to implement the port functions. Note that you can also outsource this effort to [Feaser](mailto:sales@feaser.com?subject=Portation%20request%20for%20MicroTBX-Modbus).

## Timer

MicroTBX-Modbus needs a time reference. For example to monitor the RTU communication's 1.5 and 3.5 character times. To get these timings right a free running counter, incrementing every 50 microseconds, provides a time reference.

### TbxMbPortTimerCount

```c
uint16_t TbxMbPortTimerCount(void)
```

This function obtains the current value of this counter and assumes that you already initialized a timer, during application initialization, to have its free running counter counting upwards at a 20 kHz frequency.

In contrast to most other Modbus communication stacks, MicroTBX-Modbus does not rely on an interrupt driven timer. It just needs the value of a timer peripheral's counter register, initialized to count upwards once every 50 microseconds. Benefits of this approach are that it has no interrupt overhead and that you can still reuse the timer for other purposes (input capture, PWM, output compare, etc.), as long as it can work with a 20 kHz base timer.

Timers are a scarce resource on microcontrollers. Therefore it is also possible to use the free running counter of a timer that runs at a different frequency. Just make sure to adjust the counter value in this function accordingly. For example, if you choose to reuse your RTOS' 1 millisecond system tick, you need to multiply its tick counter value by 20 to simulate a 20 kHz timer. This does of course have a negative impact on the accuracy of the RTU 1.5 and 3.5 character timings, so there's a trade-off involved.

| Return value                                                 |
| ------------------------------------------------------------ |
| Free running counter value as a 16-bit value. If your timer's counter value is more than 16-bit, simply<br>typecast it to `uint16_t`. |

## UART

The RTU and ASCII transport layers depend on a UART communication peripheral for the low-level data exchange. It is recommended to use a classical approach, where the transmission completion and reception of each byte triggers an interrupt. 

You could leverage the capability of a direct memory access (DMA) peripheral, in combination with the UART, as this lowers the interrupt overhead. However, this should not be used for data reception in combination with an RTU transport layer. The 1.5 character time between bytes can then not be properly monitored. DMA can be used for transmission, but the processing time of the byte transmit complete event is very short and therefore dedicating a DMA just for this is probably not worth it.

### TbxMbPortUartInit

```c
void TbxMbPortUartInit(tTbxMbUartPort     port, 
                       tTbxMbUartBaudrate baudrate,
                       tTbxMbUartDatabits databits, 
                       tTbxMbUartStopbits stopbits,
                       tTbxMbUartParity   parity)
```

Initialize the UART channel by performing the following steps:

* Enable the clock of the UART peripheral.
* Configure the UART Rx and Tx GPIO pins for UART communication.
* Switch the RS485 transceiver to reception mode (DE/NRE pins), if used.
* Configure the baudrate, number of databits, number of stopbits, and parity mode.
* Enable the UART transmitter and receiver.
* Enable the receive data register full (RXNE) interrupt.

Note that the actual meaning of the serial port number (`port`) is up to you. It typically maps to the UART peripheral number. E.g. `TBX_MB_UART_PORT1` = USART1 on an STM32. However, it doesn't have to. Let's say you only use two UART peripherals on your microcontroller system: USART2 and USART6. In this case it makes logical sense to map `TBX_MB_UART_PORT1` to USART2 and `TBX_MB_UART_PORT2` to USART6.

| Parameter  | Description                                    |
| ---------- | ---------------------------------------------- |
| `port`     | The serial port to use.                        |
| `baudrate` | The desired communication speed.               |
| `databits` | Number of databits for a character.            |
| `stopbits` | Number of stop bits at the end of a character. |
| `parity`   | Parity bit type to use.                        |

### TbxMbPortUartTransmit

```c
uint8_t TbxMbPortUartTransmit(tTbxMbUartPort         port, 
                              uint8_t        const * data, 
                              uint16_t               len)
```

Start the transfer of `len` bytes from the `data` array on the specified serial `port`:

* Switch the RS485 transceiver to transmission mode (DE/NRE pins), if used.
* Write the first byte (`data[0]`) to the UART transmit data register.
* Enable the transmit complete (TC) interrupt if the total length is just one byte (`len` is 1), otherwise enable the transmit data register empty (TXE) interrupt.

For managing the entire transfer, It is recommended to save transfer related information in a global (`volatile`)  variable. It can then be accessed and updated in the transmit interrupt handler ([TbxMbPortUartTxInterrupt()](#tbxbbportuarttxinterrupt)). This is what the template does with the `transmitInfo[]` array.

Note that you have mutual exclusive access to the bytes in the `data` array, until you call [TbxMbUartTransmitComplete()](apiref.md#tbxmbuarttransmitcomplete). This means that you do not need to copy the data bytes to a local buffer. This approach keeps RAM requirements low and benefits the run-time performance. Just make sure to call `TbxMbUartTransmitComplete()` once all bytes are transmitted or an error was detected, to release access to the `data` array.

| Parameter | Description                                    |
| --------- | ---------------------------------------------- |
| `port`    | The serial port to start the data transfer on. |
| `data`    | Byte array with data to transmit.              |
| `len`     | Number of bytes to transmit.                   |

| Return value                                    |
| ----------------------------------------------- |
| `TBX_OK` if successful, `TBX_ERROR ` otherwise. |

### TbxMbPortUartTxInterrupt

```c
void TbxMbPortUartTxInterrupt(tTbxMbUartPort port)
```

UART transmit complete and data register empty interrupt handler. Should be called from your UART interrupt handler, upon detection of this event, and do the following:

* If no more bytes are left to transmit:
    * Disable the transmit complete (TC) interrupt.
    * Switch the RS485 transceiver to reception mode (DE/NRE pins), if used.
    * Call [TbxMbUartTransmitComplete()](apiref.md#tbxmbuarttransmitcomplete) of the Modbus UART module to release mutual exclusive access to the transmit data buffer.
* Otherwise, write the next byte to the UART transmit data register.
    * If this is the last byte of the transfer:
        * Disable the transmit data register empty (TXE) interrupt.
        * Enable the transmit complete (TC) interrupt.

| Parameter | Description                                   |
| --------- | --------------------------------------------- |
| `port`    | The serial port that generated the interrupt. |

### TbxMbPortUartRxInterrupt

```c
void TbxMbPortUartRxInterrupt(tTbxMbUartPort port)
```

UART reception data register full interrupt handler. Should be called from your UART interrupt handler, upon detection of this event, and do the following:

* If a parity, framing or noise error was detected during the data reception, ignore the newly received byte.
* Clear all error reception related error flags: parity, framing, noise and also reception overrun.
* Read the newly received data from the UART reception data register.
* Clear the reception data register empty flag, if the UART peripheral does not automatically do this after a read from the UART reception data register.
* If the newly received byte should not be ignored, inform the Modbus UART module about the event, by calling [TbxMbUartDataReceived()](apiref.md#tbxmbuartdatareceived).

| Parameter | Description                                   |
| --------- | --------------------------------------------- |
| `port`    | The serial port that generated the interrupt. |
