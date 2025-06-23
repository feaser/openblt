# API reference

This section provides a full reference of all the functions, macros and types that MicroTBX-Modbus offers.

## Macros

### Version

| Macro                  | Description                              |
| :--------------------- | :--------------------------------------- |
| `TBX_MB_VERSION_MAIN`  | Main version number of MicroTBX-Modbus.  |
| `TBX_MB_VERSION_MINOR` | Minor version number of MicroTBX-Modbus. |
| `TBX_MB_VERSION_PATCH` | Patch number of MicroTBX-Modbus.         |

### Common

Function codes.

| Macro                                  | Description                                         |
| :------------------------------------- | :-------------------------------------------------- |
| `TBX_MB_FC01_READ_COILS`               | Modbus function code 01 - Read Coils.               |
| `TBX_MB_FC02_READ_DISCRETE_INPUTS`     | Modbus function code 02 - Read Discrete Inputs.     |
| `TBX_MB_FC03_READ_HOLDING_REGISTERS`   | Modbus function code 03 - Read Holding Registers.   |
| `TBX_MB_FC04_READ_INPUT_REGISTERS`     | Modbus function code 04 - Read Input Registers.     |
| `TBX_MB_FC05_WRITE_SINGLE_COIL`        | Modbus function code 05 - Write Single Coil.        |
| `TBX_MB_FC06_WRITE_SINGLE_REGISTER`    | Modbus function code 06 - Write Single Register.    |
| `TBX_MB_FC08_DIAGNOSTICS`              | Modbus function code 08 - Diagnostics.              |
| `TBX_MB_FC15_WRITE_MULTIPLE_COILS`     | Modbus function code 15 - Write Multiple Coils.     |
| `TBX_MB_FC16_WRITE_MULTIPLE_REGISTERS` | Modbus function code 16 - Write Multiple Registers. |

Exception codes.

| Macro                               | Description                                       |
| :---------------------------------- | :------------------------------------------------ |
| `TBX_MB_EC01_ILLEGAL_FUNCTION`      | Modbus exception code 01 - Illegal function.      |
| `TBX_MB_EC02_ILLEGAL_DATA_ADDRESS`  | Modbus exception code 02 - Illegal data address.  |
| `TBX_MB_EC03_ILLEGAL_DATA_VALUE`    | Modbus exception code 03 - Illegal data value.    |
| `TBX_MB_EC04_SERVER_DEVICE_FAILURE` | Modbus exception code 04 - Server device failure. |

Diagnostics sub function codes.

| Macro                                      | Description                                                  |
| :----------------------------------------- | :----------------------------------------------------------- |
| `TBX_MB_DIAG_SC_QUERY_DATA`                | Diagnostics sub-function code - Return Query Data.           |
| `TBX_MB_DIAG_SC_CLEAR_COUNTERS`            | Diagnostics sub-function code - Clear Counters.              |
| `TBX_MB_DIAG_SC_BUS_MESSAGE_COUNT`         | Diagnostics sub-function code - Return Bus Message<br>Count. |
| `TBX_MB_DIAG_SC_BUS_COMM_ERROR_COUNT`      | Diagnostics sub-function code - Return Bus<br>Communication Error Count. |
| `TBX_MB_DIAG_SC_BUS_EXCEPTION_ERROR_COUNT` | Diagnostics sub-function code - Return Bus Exception<br>Error Count. |
| `TBX_MB_DIAG_SC_SERVER_MESSAGE_COUNT`      | Diagnostics sub-function code - Return Server Message<br>Count. |
| `TBX_MB_DIAG_SC_SERVER_NO_RESPONSE_COUNT`  | Diagnostics sub-function code - Return Server No<br>Response Count. |

Miscellaneous.

| Macro                      | Description                                                  |
| :------------------------- | :----------------------------------------------------------- |
| `TBX_MB_FC_EXCEPTION_MASK` | Bit mask to OR to the function code to flag it as an exception response. |

### Transport layer

Node address.

| Macro                           | Description                                |
| :------------------------------ | :----------------------------------------- |
| `TBX_MB_TP_NODE_ADDR_BROADCAST` | Node address value for broadcast purposes. |
| `TBX_MB_TP_NODE_ADDR_MIN`       | Minimum value of a valid node address.     |
| `TBX_MB_TP_NODE_ADDR_MAX`       | Maximum value of a valid node address.     |

Protocol data unit (PDU).

| Macro                        | Description                                                  |
| :--------------------------- | :----------------------------------------------------------- |
| `TBX_MB_TP_PDU_CODE_LEN_MAX` | Maximum size of the "Function code" at the start of a PDU.   |
| `TBX_MB_TP_PDU_DATA_LEN_MAX` | Maximum number of data bytes inside a PDU. This excludes the<br>function code. |
| `TBX_MB_TP_PDU_MAX_LEN`      | Maximum length of a PDU.                                     |

## Types

### Server

#### tTbxMbServer

```c
typedef void * tTbxMbServer
```

Handle to a Modbus server channel object, in the format of an opaque pointer.

#### tTbxMbServerResult

```c
typedef enum
{
  TBX_MB_SERVER_OK = 0U,
  TBX_MB_SERVER_ERR_ILLEGAL_DATA_ADDR,
  TBX_MB_SERVER_ERR_DEVICE_FAILURE
} tTbxMbServerResult
```

numerated type with all supported return values for the callbacks.

#### tTbxMbServerReadInput

```c
typedef tTbxMbServerResult (* tTbxMbServerReadInput)(tTbxMbServer   channel, 
                                                     uint16_t       addr, 
                                                     uint8_t      * value)
```

Modbus server callback function for reading a discrete input.

| Parameter | Description                                                  |
| --------- | ------------------------------------------------------------ |
| `channel` | Handle to the Modbus server channel object that triggered the callback. |
| `addr`    | Element address (`0`..`65535`).                              |
| `value`   | Pointer to write the value of the input to. Use `TBX_ON` if the input is on, `TBX_OFF`<br>otherwise. |

| Return value                                                 |
| ------------------------------------------------------------ |
| `TBX_MB_SERVER_OK` if successful, `TBX_MB_SERVER_ERR_ILLEGAL_DATA_ADDR` if the specific data element<br>address is not supported by this server, `TBX_MB_SERVER_ERR_DEVICE_FAILURE` otherwise. |

#### tTbxMbServerReadCoil

```c
typedef tTbxMbServerResult (* tTbxMbServerReadCoil)(tTbxMbServer   channel, 
                                                    uint16_t       addr, 
                                                    uint8_t      * value)
```

Modbus server callback function for reading a coil.

| Parameter | Description                                                  |
| --------- | ------------------------------------------------------------ |
| `channel` | Handle to the Modbus server channel object that triggered the callback. |
| `addr`    | Element address (`0`..`65535`).                              |
| `value`   | Pointer to write the value of the coil to. Use `TBX_ON` if the coils is on, `TBX_OFF`<br>otherwise. |

| Return value                                                 |
| ------------------------------------------------------------ |
| `TBX_MB_SERVER_OK` if successful, `TBX_MB_SERVER_ERR_ILLEGAL_DATA_ADDR` if the specific data element<br>address is not supported by this server, `TBX_MB_SERVER_ERR_DEVICE_FAILURE` otherwise. |

#### tTbxMbServerWriteCoil

```c
typedef tTbxMbServerResult (* tTbxMbServerWriteCoil)(tTbxMbServer channel, 
                                                     uint16_t     addr, 
                                                     uint8_t      value)
```

Modbus server callback function for writing a coil.

| Parameter | Description                                                  |
| --------- | ------------------------------------------------------------ |
| `channel` | Handle to the Modbus server channel object that triggered the callback. |
| `addr`    | Element address (`0`..`65535`).                              |
| `value`   | Coil value. Use `TBX_ON` to activate the coil, `TBX_OFF` otherwise. |

| Return value                                                 |
| ------------------------------------------------------------ |
| `TBX_MB_SERVER_OK` if successful, `TBX_MB_SERVER_ERR_ILLEGAL_DATA_ADDR` if the specific data element<br>address is not supported by this server, `TBX_MB_SERVER_ERR_DEVICE_FAILURE` otherwise. |

#### tTbxMbServerReadInputReg

```c
typedef tTbxMbServerResult (* tTbxMbServerReadInputReg)(tTbxMbServer   channel, 
                                                        uint16_t       addr, 
                                                        uint16_t     * value)
```

Modbus server callback function for reading an input register.

| Parameter | Description                                                  |
| --------- | ------------------------------------------------------------ |
| `channel` | Handle to the Modbus server channel object that triggered the callback. |
| `addr`    | Element address (`0`..`65535`).                              |
| `value`   | Pointer to write the value of the input register to.         |

| Return value                                                 |
| ------------------------------------------------------------ |
| `TBX_MB_SERVER_OK` if successful, `TBX_MB_SERVER_ERR_ILLEGAL_DATA_ADDR` if the specific data element<br>address is not supported by this server, `TBX_MB_SERVER_ERR_DEVICE_FAILURE` otherwise. |

#### tTbxMbServerReadHoldingReg

```c
typedef tTbxMbServerResult (* tTbxMbServerReadHoldingReg)(tTbxMbServer   channel, 
                                                          uint16_t       addr, 
                                                          uint16_t     * value)
```

Modbus server callback function for reading a holding register.

| Parameter | Description                                                  |
| --------- | ------------------------------------------------------------ |
| `channel` | Handle to the Modbus server channel object that triggered the callback. |
| `addr`    | Element address (`0`..`65535`).                              |
| `value`   | Pointer to write the value of the holding register to.       |

| Return value                                                 |
| ------------------------------------------------------------ |
| `TBX_MB_SERVER_OK` if successful, `TBX_MB_SERVER_ERR_ILLEGAL_DATA_ADDR` if the specific data element<br>address is not supported by this server, `TBX_MB_SERVER_ERR_DEVICE_FAILURE` otherwise. |

#### tTbxMbServerWriteHoldingReg

```c
typedef tTbxMbServerResult (* tTbxMbServerWriteHoldingReg)(tTbxMbServer channel, 
                                                           uint16_t     addr, 
                                                           uint16_t     value)
```

Modbus server callback function for writing a holding register.

| Parameter | Description                                                  |
| --------- | ------------------------------------------------------------ |
| `channel` | Handle to the Modbus server channel object that triggered the callback. |
| `addr`    | Element address (`0`..`65535`).                              |
| `value`   | Value of the holding register.                               |

| Return value                                                 |
| ------------------------------------------------------------ |
| `TBX_MB_SERVER_OK` if successful, `TBX_MB_SERVER_ERR_ILLEGAL_DATA_ADDR` if the specific data element<br>address is not supported by this server, `TBX_MB_SERVER_ERR_DEVICE_FAILURE` otherwise. |

#### tTbxMbServerCustomFunction

```c
typedef uint8_t (* tTbxMbServerCustomFunction)(tTbxMbServer    channel,
                                               uint8_t const * rxPdu,
                                               uint8_t       * txPdu,
                                               uint8_t       * len)
```

Modbus server callback function for implementing custom function code handling. Thanks to this functionality, the user can support Modbus function codes that are either currently not supported or user defined extensions.

The `rxPdu` and `txPdu` parameters are pointers to the byte array of the PDU. The first byte (i.e. `rxPdu[0]`) contains the function code, followed by its data bytes. Upon calling the callback, the `len` parameter contains the length of `rxPdu`. When preparing the response, you can write the length of the `txPdu` response to `len` as well.

| Parameter | Description                                                  |
| --------- | ------------------------------------------------------------ |
| `channel` | Handle to the Modbus server channel object that triggered the callback. |
| `rxPdu`   | Pointer to a byte array for reading the received PDU.        |
| `txPdu`   | Pointer to a byte array for writing the response PDU.        |
| `len`     | Pointer to the PDU length, including the function code.      |

| Return value                                                 |
| ------------------------------------------------------------ |
| `TBX_TRUE` if the callback function handled the received function code and prepared a response PDU.<br>`TBX_FALSE` otherwise. |

### Client

#### tTbxMbClient

```c
typedef void * tTbxMbClient
```

Handle to a Modbus client channel object, in the format of an opaque pointer.

### Transport layer

#### tTbxMbTp

```c
typedef void * tTbxMbTp
```

Handle to a Modbus transport layer object, in the format of an opaque pointer.

### UART

#### tTbxMbUartPort

```c
typedef enum
{
  TBX_MB_UART_PORT1 = 0U,
  TBX_MB_UART_PORT2,
  TBX_MB_UART_PORT3,
  TBX_MB_UART_PORT4,
  TBX_MB_UART_PORT5,
  TBX_MB_UART_PORT6,
  TBX_MB_UART_PORT7,
  TBX_MB_UART_PORT8,
  TBX_MB_UART_NUM_PORT
} tTbxMbUartPort
```

Enumerated type with all supported UART ports.

#### tTbxMbUartBaudrate

```c
typedef enum
{
  TBX_MB_UART_1200BPS = 0U,
  TBX_MB_UART_2400BPS,
  TBX_MB_UART_4800BPS,
  TBX_MB_UART_9600BPS,
  TBX_MB_UART_19200BPS,
  TBX_MB_UART_38400BPS,
  TBX_MB_UART_57600BPS,
  TBX_MB_UART_115200BPS,
  TBX_MB_UART_NUM_BAUDRATE
} tTbxMbUartBaudrate
```

Enumerated type with all supported UART baudrates.

#### tTbxMbUartDatabits

```c
typedef enum
{
  TBX_MB_UART_7_DATABITS = 0U,
  TBX_MB_UART_8_DATABITS,
  TBX_MB_UART_NUM_DATABITS
} tTbxMbUartDatabits
```

Enumerated type with all supported UART data bits modes.

#### tTbxMbUartStopbits

```c
typedef enum
{
  TBX_MB_UART_1_STOPBITS = 0U,
  TBX_MB_UART_2_STOPBITS,
  TBX_MB_UART_NUM_STOPBITS
} tTbxMbUartStopbits
```

Enumerated type with all supported parity modes.

#### tTbxMbUartParity

```c
typedef enum
{
  TBX_MB_ODD_PARITY = 0U,
  TBX_MB_EVEN_PARITY,
  TBX_MB_NO_PARITY,
  TBX_MB_UART_NUM_PARITY
} tTbxMbUartParity
```

Enumerated type with all supported parity modes.

## Functions

### Server

#### TbxMbServerCreate

```c
tTbxMbServer TbxMbServerCreate(tTbxMbTp transport)
```

Creates a Modbus server channel object and assigns the specified Modbus transport layer to the channel for packet transmission and reception.

This example creates a Modbus RTU server channel object for a node with address `10`:

```c
/* Construct a Modbus RTU transport layer object. */
tTbxMbTp modbusTp = TbxMbRtuCreate(10U, TBX_MB_UART_PORT1, TBX_MB_UART_19200BPS,
                                   TBX_MB_UART_1_STOPBITS, TBX_MB_EVEN_PARITY); 
/* Construct a Modbus server object. */
tTbxMbServer modbusServer = TbxMbServerCreate(modbusTp);  
```

| Parameter   | Description                                                  |
| ----------- | ------------------------------------------------------------ |
| `transport` | Handle to a previously created Modbus transport layer object to assign to the channel. |

| Return value                                                 |
| ------------------------------------------------------------ |
| Handle to the newly created Modbus server channel object if successful, `NULL` otherwise. |

#### TbxMbServerFree

```c
void TbxMbServerFree(tTbxMbServer channel)
```

Releases a Modbus server channel object, previously created with [TbxMbServerCreate()](#tbxmbservercreate).

| Parameter | Description                                            |
| --------- | ------------------------------------------------------ |
| `channel` | Handle to the Modbus server channel object to release. |

#### TbxMbServerSetCallbackReadInput

```c
void TbxMbServerSetCallbackReadInput(tTbxMbServer          channel,
                                     tTbxMbServerReadInput callback)
```

Registers the callback function that this server calls, whenever a client requests the reading of a specific discrete input.

The example connects the state of two digital inputs to the Modbus discrete inputs at addresses `10000` to `10001`:

```c
tTbxMbServerResult AppReadInput(tTbxMbServer   channel,
                                uint16_t       addr,
                                uint8_t      * value)
{
  tTbxMbServerResult result = TBX_MB_SERVER_OK;

  /* Filter on the requested discrete input address. */
  switch (addr)
  {
  case 10000U:
    *value = BspDigitalIn(BSP_DIGITAL_IN1);
    break;
  
  case 10001U:
    *value = BspDigitalIn(BSP_DIGITAL_IN2);
    break;
  
  default:
    /* Unsupported discrete input address. */
    result = TBX_MB_SERVER_ERR_ILLEGAL_DATA_ADDR;
    break;
  }

  /* Give the result back to the caller. */
  return result;
}

/* Set the callback for reading the Modbus discrete inputs. */
TbxMbServerSetCallbackReadInput(modbusServer, AppReadInput);
```

| Parameter  | Description                                 |
| ---------- | ------------------------------------------- |
| `channel`  | Handle to the Modbus server channel object. |
| `callback` | Pointer to the callback function.           |

#### TbxMbServerSetCallbackReadCoil

```c
void TbxMbServerSetCallbackReadCoil(tTbxMbServer         channel,
                                    tTbxMbServerReadCoil callback)
```

Registers the callback function that this server calls, whenever a client requests the reading of a specific coil.

The example assumes the application stores the state of two coils in an array with name `appCoils[]` . Whenever a client requests the reading of the Modbus coils at addresses `0` to `1`, the currently stored values in the `appCoils[]` array are returned:

```c
uint8_t appCoils[2] = { TBX_ON, TBX_OFF };

tTbxMbServerResult AppReadCoil(tTbxMbServer   channel,
                               uint16_t       addr,
                               uint8_t      * value)
{
  tTbxMbServerResult result = TBX_MB_SERVER_ERR_ILLEGAL_DATA_ADDR;

  /* Supported coil address? */
  if (addr <= 1U)
  {
    /* Store the current coil state. */
    *value = appCoils[addr];
    result = TBX_MB_SERVER_OK;
  }    
  /* Give the result back to the caller. */
  return result;
}

/* Set the callback for reading the Modbus coils. */
TbxMbServerSetCallbackReadCoil(modbusServer, AppReadCoil);
```

| Parameter  | Description                                 |
| ---------- | ------------------------------------------- |
| `channel`  | Handle to the Modbus server channel object. |
| `callback` | Pointer to the callback function.           |

#### TbxMbServerSetCallbackWriteCoil

```c
void TbxMbServerSetCallbackWriteCoil(tTbxMbServer          channel,
                                     tTbxMbServerWriteCoil callback)
```

Registers the callback function that this server calls, whenever a client requests the writing of a specific coil.

The example connects the Modbus coil addresses `0` to `1` to the  state of two digital outputs:

```c
tTbxMbServerResult AppWriteCoil(tTbxMbServer   channel,
                                uint16_t       addr,
                                uint8_t        value)
{
  tTbxMbServerResult result = TBX_MB_SERVER_OK;

  /* Filter on the requested coil address. */
  switch (addr)
  {
  case 0U:
    BspDigitalOut(BSP_DIGITAL_OUT1, value);
    break;
  
  case 1U:
    BspDigitalOut(BSP_DIGITAL_OUT2, value);
    break;
  
  default:
    /* Unsupported coil address. */
    result = TBX_MB_SERVER_ERR_ILLEGAL_DATA_ADDR;
    break;
  }

  /* Give the result back to the caller. */
  return result;
}

/* Set the callback for writing the Modbus coils. */
TbxMbServerSetCallbackWriteCoil(modbusServer, AppWriteCoil);
```

| Parameter  | Description                                 |
| ---------- | ------------------------------------------- |
| `channel`  | Handle to the Modbus server channel object. |
| `callback` | Pointer to the callback function.           |

#### TbxMbServerSetCallbackReadInputReg

```c
void TbxMbServerSetCallbackReadInputReg(tTbxMbServer                channel,
                                        tTbxMbServerReadInputReg    callback)
```

Registers the callback function that this server calls, whenever a client requests the reading of a specific input register.

The example connects the state of two analog inputs to the Modbus input registers at addresses `30000` to `30001`:

```c
tTbxMbServerResult AppReadInputReg(tTbxMbServer  channel,
                                    uint16_t      addr, 
                                    uint16_t    * value)
{
  tTbxMbServerResult result = TBX_MB_SERVER_OK;

  /* Filter on the requested input register address. */
  switch (addr)
  {
  case 30000U:
    *value = BspAnalogIn(BSP_ANALOG_IN1);
    break;
  
  case 30001U:
    *value = BspAnalogIn(BSP_ANALOG_IN2);
    break;
  
  default:
    /* Unsupported input register address. */
    result = TBX_MB_SERVER_ERR_ILLEGAL_DATA_ADDR;
    break;
  }

  /* Give the result back to the caller. */
  return result;
}

/* Set the callback for reading the Modbus input registers. */
TbxMbServerSetCallbackReadInputReg(modbusServer, AppReadInputReg);
```

| Parameter  | Description                                 |
| ---------- | ------------------------------------------- |
| `channel`  | Handle to the Modbus server channel object. |
| `callback` | Pointer to the callback function.           |

#### TbxMbServerSetCallbackReadHoldingReg

```c
void TbxMbServerSetCallbackReadHoldingReg(tTbxMbServer                channel,
                                          tTbxMbServerReadHoldingReg  callback)
```

Registers the callback function that this server calls, whenever a client requests the reading of a specific holding register.

The example assumes the application stores the state of two holding registers in an array with name `appHoldingRegs[]` . Whenever a client requests the reading of the Modbus holding registers at addresses `40000` to `40001`, the currently stored values in the `appHoldingRegs[]` array are returned:

```c
uint16_t appHoldingRegs[2] = { 1234, 5678 };

tTbxMbServerResult AppReadHoldingReg(tTbxMbServer   channel,
                                     uint16_t       addr,
                                     uint16_t     * value)
{
  tTbxMbServerResult result = TBX_MB_SERVER_ERR_ILLEGAL_DATA_ADDR;

  /* Supported holding register address? */
  if ( (addr >= 40000) && (addr <= 40001U) )
  {
    /* Store the holding register state. */
    *value = appHoldingReg[addr - 40000U];
    result = TBX_MB_SERVER_OK;
  }    
  /* Give the result back to the caller. */
  return result;
}

/* Set the callback for reading the Modbus holding registers. */
TbxMbServerSetCallbackReadHoldingReg(modbusServer, AppReadHoldingReg);
```

| Parameter  | Description                                 |
| ---------- | ------------------------------------------- |
| `channel`  | Handle to the Modbus server channel object. |
| `callback` | Pointer to the callback function.           |

#### TbxMbServerSetCallbackWriteHoldingReg

```c
void TbxMbServerSetCallbackWriteHoldingReg(tTbxMbServer                channel,
                                           tTbxMbServerWriteHoldingReg callback)
```

Registers the callback function that this server calls, whenever a client requests the writing of a specific holding register.

The example connects the Modbus holding registers addresses `40000` to `40001` to two 8-bit PWM output signals:

```c
tTbxMbServerResult AppWriteHoldingReg(tTbxMbServer channel,
                                      uint16_t     addr, 
                                      uint16_t     value)
{
  tTbxMbServerResult result = TBX_MB_SERVER_OK;

  /* Filter on the requested holding register address. */
  switch (addr)
  {
  case 40000U:
    /* PWM supports 8-bit duty cycle. */
    if (value <= 255U)
    {
      BspPwmOut(BSP_PWM_OUT1, (uint8_t)value);
    }
    else
    {
      result = TBX_MB_SERVER_ERR_DEVICE_FAILURE;
    }
    break;
  
  case 40001U:
    /* PWM supports 8-bit duty cycle. */
    if (value <= 255U)
    {
      BspPwmOut(BSP_PWM_OUT2, (uint8_t)value);
    }
    else
    {
      result = TBX_MB_SERVER_ERR_DEVICE_FAILURE;
    }
    break;
  
  default:
    /* Unsupported holding register address. */
    result = TBX_MB_SERVER_ERR_ILLEGAL_DATA_ADDR;
    break;
  }

  /* Give the result back to the caller. */
  return result;
}

/* Set the callback for writing the Modbus holding registers. */
TbxMbServerSetCallbackWriteHoldingReg(modbusServer, AppWriteHoldingReg);
```

| Parameter  | Description                                 |
| ---------- | ------------------------------------------- |
| `channel`  | Handle to the Modbus server channel object. |
| `callback` | Pointer to the callback function.           |

#### TbxMbServerSetCallbackCustomFunction

```c
void TbxMbServerSetCallbackCustomFunction (tTbxMbServer                channel,
                                           tTbxMbServerCustomFunction  callback)
```

Registers the callback function that this server calls, whenever it received a PDU containing a function code not currently supported. With the aid of this callback function the user can implement support for new function codes.

The example shows how to add support for function code 17 (*Report Server ID*). It's the counter-part to the example for [TbxMbClientCustomFunction()](#tbxmbclientcustomfunction). According to the Modbus protocol, the response to the *Report Server ID* request is device specific. The device implementation decides the number of bytes for the Server ID and if additional data is added to the response. The following code snippet implements support for *Report Server ID*, where the actual server ID is 16-bits and the response contains no additional data:

```c
uint8_t AppReportServerIdCallback(tTbxMbServer    channel,
                                  uint8_t const * rxPdu,
                                  uint8_t       * txPdu,
                                  uint8_t       * len)
{
  uint8_t result = TBX_FALSE;

  /* Function code 17 - Report Server ID? */
  if (rxPdu[0] == 17U)
  {
    /* Check the expected request length. */
    if (*len == 1U)
    {
      /* Prepare the response. */
      txPdu[0] = 17U; /* Function code. */
      txPdu[1] = 3U;  /* Byte count. */
      TbxMbCommonStoreUInt16BE(0x1234U, &txPdu[2]); /* server ID. */
      txPdu[4] = 0xFFU; /* Run indicator status = ON. */
      *len = 5U;
      /* Function code handled. */
      result = TBX_TRUE;
    }
  }

  /* Give the result back to the caller. */
  return result;
}     

/* Set the callback for handling custom function codes. */
TbxMbServerSetCallbackCustomFunction(modbusServer, AppReportServerIdCallback);
```

| Parameter  | Description                                 |
| ---------- | ------------------------------------------- |
| `channel`  | Handle to the Modbus server channel object. |
| `callback` | Pointer to the callback function.           |

### Client

#### TbxMbClientCreate

```c
tTbxMbClient TbxMbClientCreate(tTbxMbTp transport,
                               uint16_t responseTimeout,
                               uint16_t turnaroundDelay)
```

Creates a Modbus client channel object and assigns the specified Modbus transport layer to the channel for packet transmission and reception.

This example creates a Modbus RTU client channel object. Note the the `nodeAddr` parameter of function [TbxMbRtuCreate()](#tbxmbrtucreate) is not applicable when used on a client and should simply be set to a value of `0`:

```c
/* Construct a Modbus RTU transport layer object. */
tTbxMbTp modbusTp = TbxMbRtuCreate(0U, TBX_MB_UART_PORT1, TBX_MB_UART_19200BPS,
                                   TBX_MB_UART_1_STOPBITS, TBX_MB_EVEN_PARITY); 
/* Construct a Modbus client object. */
tTbxMbClient modbusClient = TbxMbClientCreate(modbusTp, 1000U, 100U);
```

| Parameter         | Description                                                  |
| ----------------- | ------------------------------------------------------------ |
| `transport`       | Handle to a previously created Modbus transport layer object to assign to the<br>channel. |
| `responseTimeout` | Maximum time in milliseconds to wait for a response from the Modbus server,<br>after sending a PDU. |
| `turnaroundDelay` | Delay time in milliseconds after sending a broadcast PDU to give all recipients<br>sufficient time to process the PDU. |

| Return value                                                 |
| ------------------------------------------------------------ |
| Handle to the newly created Modbus client channel object if successful, `NULL` otherwise. |

#### TbxMbClientFree

```c
void TbxMbClientFree(tTbxMbClient channel)
```

Releases a Modbus client channel object, previously created with [TbxMbClientCreate()](#tbxmbclientcreate).

| Parameter | Description                                            |
| --------- | ------------------------------------------------------ |
| `channel` | Handle to the Modbus client channel object to release. |

#### TbxMbClientReadCoils

```c
uint8_t TbxMbClientReadCoils(tTbxMbClient   channel,
                             uint8_t        node,
                             uint16_t       addr,
                             uint16_t       num,
                             uint8_t      * coils)
```

Reads the coil(s) from the server with the specified node address.

The example reads the state of two coils at Modbus addresses `0` to `1`, from a Modbus server with node address `10`:

```c
uint8_t coils[2] = { 0 };

TbxMbClientReadCoils(modbusClient, 10U, 0U, 2U, coils);
```

| Parameter | Description                                                  |
| --------- | ------------------------------------------------------------ |
| `channel` | Handle to the Modbus client channel for the requested operation. |
| `node`    | The address of the server. This parameter is transport layer dependent. It is needed on<br>RTU/ASCII, yet don't care for TCP unless it is a gateway to an RTU network. If it's don't<br>care, set it to a value of `255`. |
| `addr`    | Starting element address (0..65535) in the Modbus data table for the coil read operation. |
| `num`     | Number of elements to read from the coils data table. Range can be `1`..`2000`. |
| `coils`   | Pointer to array with `TBX_ON` / `TBX_OFF` values where the coil state will be written to. |

| Return value                                   |
| ---------------------------------------------- |
| `TBX_OK` if successful, `TBX_ERROR` otherwise. |

#### TbxMbClientReadInputs

```c
uint8_t TbxMbClientReadInputs(tTbxMbClient   channel,
                              uint8_t        node,
                              uint16_t       addr,
                              uint16_t       num,
                              uint8_t      * inputs)
```

Reads the discrete input(s) from the server with the specified node address.

The example reads the state of two discrete inputs at Modbus addresses `10000` to `10001`, from a Modbus server with node address `10`:

```c
uint8_t inputs[2] = { 0 };

TbxMbClientReadInputs(modbusClient, 10U, 10000U, 2U, inputs);
```

| Parameter | Description                                                  |
| --------- | ------------------------------------------------------------ |
| `channel` | Handle to the Modbus client channel for the requested operation. |
| `node`    | The address of the server. This parameter is transport layer dependent. It is needed on<br>RTU/ASCII, yet don't care for TCP unless it is a gateway to an RTU network. If it's don't<br>care, set it to a value of `255`. |
| `addr`    | Starting element address (0..65535) in the Modbus data table for the discrete input<br>read operation. |
| `num`     | Number of elements to read from the discrete inputs data table. Range can be `1`..`2000`. |
| `inputs`  | Pointer to array with `TBX_ON` / `TBX_OFF` values where the discrete input state will be<br>written to. |

| Return value                                   |
| ---------------------------------------------- |
| `TBX_OK` if successful, `TBX_ERROR` otherwise. |

#### TbxMbClientReadInputRegs

```c
uint8_t TbxMbClientReadInputRegs(tTbxMbClient   channel,
                                 uint8_t        node,
                                 uint16_t       addr,
                                 uint8_t        num,
                                 uint16_t     * inputRegs)
```

Reads the input register(s) from the server with the specified node address.

The example reads two input registers at Modbus addresses `30000` to `30001`, from a Modbus server with node address `10`:

```c
uint16_t inputRegs[2] = { 0 };

TbxMbClientReadInputRegs(modbusClient, 10U, 30000U, 2U, inputRegs);
```

| Parameter   | Description                                                  |
| ----------- | ------------------------------------------------------------ |
| `channel`   | Handle to the Modbus client channel for the requested operation. |
| `node`      | The address of the server. This parameter is transport layer dependent. It is needed on<br>RTU/ASCII, yet don't care for TCP unless it is a gateway to an RTU network. If it's don't<br>care, set it to a value of `255`. |
| `addr`      | Starting element address (0..65535) in the Modbus data table for the input register<br>read operation. |
| `num`       | Number of elements to read from the input registers data table. Range can be `1`..`125`. |
| `inputRegs` | Pointer to array where the input register values will be written to. |

| Return value                                   |
| ---------------------------------------------- |
| `TBX_OK` if successful, `TBX_ERROR` otherwise. |

#### TbxMbClientReadHoldingRegs

```c
uint8_t TbxMbClientReadHoldingRegs(tTbxMbClient   channel,
                                   uint8_t        node,
                                   uint16_t       addr,
                                   uint8_t        num,
                                   uint16_t     * holdingRegs)
```

Reads the holding register(s) from the server with the specified node address.

The example reads two holding registers at Modbus addresses `40000` to `40001`, from a Modbus server with node address `10`:

```c
uint16_t holdingRegs[2] = { 0 };

TbxMbClientReadHoldingRegs(modbusClient, 10U, 40000U, 2U, holdingRegs);
```

| Parameter     | Description                                                  |
| ------------- | ------------------------------------------------------------ |
| `channel`     | Handle to the Modbus client channel for the requested operation. |
| `node`        | The address of the server. This parameter is transport layer dependent. It is needed on<br>RTU/ASCII, yet don't care for TCP unless it is a gateway to an RTU network. If it's don't<br>care, set it to a value of `255`. |
| `addr`        | Starting element address (0..65535) in the Modbus data table for the holding register<br>read operation. |
| `num`         | Number of elements to read from the holding registers data table. Range can be<br>`1`..`125`. |
| `holdingRegs` | Pointer to array where the holding register values will be written to. |

| Return value                                   |
| ---------------------------------------------- |
| `TBX_OK` if successful, `TBX_ERROR` otherwise. |

#### TbxMbClientWriteCoils

```c
uint8_t TbxMbClientWriteCoils(tTbxMbClient         channel,
                              uint8_t              node,
                              uint16_t             addr,
                              uint16_t             num,
                              uint8_t      const * coils)
```

Writes the coil(s) to the server with the specified node address.

The example writes the state of two coils at Modbus addresses `0` to `1`, to a Modbus server with node address `10`:

```c
uint8_t coils[2] = { TBX_OFF, TBX_OFF };

TbxMbClientWriteCoils(modbusClient, 10U, 0U, 2U, coils);
```

| Parameter | Description                                                  |
| --------- | ------------------------------------------------------------ |
| `channel` | Handle to the Modbus client channel for the requested operation. |
| `node`    | The address of the server. This parameter is transport layer dependent. It is needed on<br>RTU/ASCII, yet don't care for TCP unless it is a gateway to an RTU network. If it's don't<br>care, set it to a value of `255`. |
| `addr`    | Starting element address (0..65535) in the Modbus data table for the coil write operation. |
| `num`     | Number of elements to write to the coils data table. Range can be `1`..`1968`. |
| `coils`   | Pointer to array with the desired `TBX_ON` / `TBX_OFF` coil values. |

| Return value                                   |
| ---------------------------------------------- |
| `TBX_OK` if successful, `TBX_ERROR` otherwise. |

#### TbxMbClientWriteHoldingRegs

```c
uint8_t TbxMbClientWriteHoldingRegs(tTbxMbClient         channel,
                                    uint8_t              node,
                                    uint16_t             addr,
                                    uint8_t              num,
                                    uint16_t     const * holdingRegs)
```

Writes the holding register(s) to the server with the specified node address.

The example writes two holding registers at Modbus addresses `40000` to `40001`, to a Modbus server with node address `10`:

```c
uint16_t holdingRegs[2] = { 63U, 127U };

TbxMbClientWriteHoldingRegs(modbusClient, 10U, 40000U, 2U, holdingRegs);
```

| Parameter     | Description                                                  |
| ------------- | ------------------------------------------------------------ |
| `channel`     | Handle to the Modbus client channel for the requested operation. |
| `node`        | The address of the server. This parameter is transport layer dependent. It is needed on<br>RTU/ASCII, yet don't care for TCP unless it is a gateway to an RTU network. If it's don't<br>care, set it to a value of `255`. |
| `addr`        | Starting element address (0..65535) in the Modbus data table for the holding register<br>write operation. |
| `num`         | Number of elements to write to the holding registers data table. Range can be<br>`1`..`123`. |
| `holdingRegs` | Pointer to array with the desired holding register values.   |

| Return value                                   |
| ---------------------------------------------- |
| `TBX_OK` if successful, `TBX_ERROR` otherwise. |

#### TbxMbClientDiagnostics

```c
uint8_t TbxMbClientDiagnostics(tTbxMbClient         channel,
                               uint8_t              node,
                               uint16_t             subcode,
                               uint16_t           * count)
```

Perform diagnostic operation on the server for checking the communication system.

The example obtains the number of packets with a correct CRC, received by a Modbus server with node address `10`:

```c
uint16_t count = 0U;

TbxMbClientDiagnostics(modbusClient, 10U, TBX_MB_DIAG_SC_SERVER_MESSAGE_COUNT, &count);
```

| Parameter | Description                                                  |
| --------- | ------------------------------------------------------------ |
| `channel` | Handle to the Modbus client channel for the requested operation. |
| `node`    | The address of the server. This parameter is transport layer dependent. It is needed on<br>RTU/ASCII, yet don't care for TCP unless it is a gateway to an RTU network. If it's don't<br>care, set it to a value of `255`. |
| `subcode` | Sub-function code for specifying the diagnostic operation to perform. Currently<br>supported values:<br>- `TBX_MB_DIAG_SC_QUERY_DATA`<br>\- `TBX_MB_DIAG_SC_CLEAR_COUNTERS`<br>- `TBX_MB_DIAG_SC_BUS_MESSAGE_COUNT`<br>- `TBX_MB_DIAG_SC_BUS_COMM_ERROR_COUNT`<br>- `TBX_MB_DIAG_SC_BUS_EXCEPTION_ERROR_COUNT`<br>- `TBX_MB_DIAG_SC_SERVER_MESSAGE_COUNT`<br>\- `TBX_MB_DIAG_SC_SERVER_NO_RESPONSE_COUNT` |
| `count`   | Location where the retrieved count value will be written to. Only applicable for the<br>sub-function codes that end with `_COUNT`. |

| Return value                                   |
| ---------------------------------------------- |
| `TBX_OK` if successful, `TBX_ERROR` otherwise. |

#### TbxMbClientCustomFunction

```c
uint8_t TbxMbClientCustomFunction(tTbxMbClient         channel,
                                  uint8_t              node,
                                  uint8_t      const * txPdu,
                                  uint8_t            * rxPdu,
                                  uint8_t            * len)
```

Send a custom function code PDU to the server and receive its response PDU. Thanks to this functionality, the user can support Modbus function codes that are either currently not supported or user defined extensions. 

The `txPdu` and `rxPdu` parameters are pointers to the byte array of the PDU. The first byte (i.e. `txPdu[0]`) contains the function code, followed by its data bytes. When calling this function, set the `len` parameter to the length of the `txPdu`. This function updates the `len` parameter with the length of the received PDU, which it stores in `rxPdu`.

The example shows how to add support for function code 17 (*Report Server ID*). It's the counter-part to the example for [TbxMbServerSetCallbackCustomFunction()](#tbxmbserversetcallbackcustomfunction). According to the Modbus protocol, the response to the *Report Server ID* request is device specific. The device implementation decides the number of bytes for the Server ID and if additional data is added to the response. The following code snippet implements support for *Report Server ID*, where it reads out the 16-bit server ID of a Modbus server with node address `10`:

```c
uint16_t AppReportServerId(tTbxMbClient channel,
                           uint8_t      node)
{
  /* static to lower stack load. */
  static uint8_t response[TBX_MB_TP_PDU_MAX_LEN]; 
  uint8_t        request[1] = { 17U };
  uint8_t        len = 1U;
  uint16_t       result = 0U;

  /* Transceive function code 17 - Report Server ID. */
  if (TbxMbClientCustomFunction(channel, node, request, 
                                response, &len) == TBX_OK)
  {
    /* Response length as expected? */
    if (len == 5U)
    {
      /* Not an exception response and byte count correct? */
      if ((response[0] == 17U) && (response[1] == 3U))
      {
        /* Read out the received server ID. */
        result = TbxMbCommonExtractUInt16BE(&response[2]);
      }
    }
  }

  /* Give the result back to the caller. */
  return result;
}                          

/* Read the server ID. */
uint16_t serverId = AppReportServerId(modbusClient, 10U);
```

| Parameter | Description                                                  |
| --------- | ------------------------------------------------------------ |
| `channel` | Handle to the Modbus client channel for the requested operation. |
| `node`    | The address of the server. This parameter is transport layer dependent. It is needed on<br>RTU/ASCII, yet don't care for TCP unless it is a gateway to an RTU network. If it's don't<br>care, set it to a value of `255`. |
| `txPdu`   | Pointer to a byte array with the PDU to transmit.            |
| `rxPdu`   | Pointer to a byte array with the received response PDU.      |
| `len`     | Pointer to the PDU length, including the function code.      |

| Return value                                   |
| ---------------------------------------------- |
| `TBX_OK` if successful, `TBX_ERROR` otherwise. |

### Event

#### TbxMbEventTask

```c
void TbxMbEventTask(void)
```

Task function that drives the entire Modbus stack. It processes internally generated events. How to call this function depends on the selected operating system abstraction layer (OSAL), which you determine based on the `source/osal/tbxmb_XXX.c` source file you compile and link with your firmware.

In a traditional superloop application (`tbxmb_superloop.c`), call this function continuously in the infinite program loop:

```c
#include <microtbx.h>
#include <microtbxmodbus.h>

void main(void)
{
  /* TODO Initialize the clock, enable peripherals and configure GPIO pins. */
  /* TODO Construct a Modbus transport layer object. */
  /* TODO Construct a Modbus client or server object. */

  /* Enter the program's infinite loop. */  
  for(;;)
  {
    /* Continuously call the Modbus stack event task function. */
    TbxMbEventTask();    
  } 
}
```

When using an RTOS (e.g. `tbxmb_freertos.c`), create a new task during application initialization and call this function from this task's infinite loop:

```c
#include <microtbx.h>
#include <microtbxmodbus.h>
#include "FreeRTOS.h"
#include "task.h"

void AppModbusTask(void * pvParameters);

void main(void)
{
  /* TODO Initialize the clock, enable peripherals and configure GPIO pins. */
  /* TODO Construct a Modbus transport layer object. */
  /* TODO Construct a Modbus client or server object. */

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
```

There is one exception: When using a traditional super application in combination with just a Modbus client. In this case you can omit the call to this task function. With this combination, the communication with a Modbus server happens in a blocking manner and the event task is automatically called internally, while blocking. Convenient and easy, but not optimal from a run-time performance. For this reason it is recommended to use an RTOS in combination with a Modbus client.

### Common

#### TbxMbCommonExtractUInt16BE

```c
uint16_t TbxMbCommonExtractUInt16BE(uint8_t const * data)
```

Helper function to extract an unsigned 16-bit value from the data of a Modbus packet, where 16-bit values are always stored in the big endian format.

| Parameter | Description                                                  |
| --------- | ------------------------------------------------------------ |
| `data`    | Pointer to the byte array that holds the two bytes to extract, stored in the big endian<br>format. |

| Return value                       |
| ---------------------------------- |
| The 16-bit unsigned integer value. |

#### TbxMbCommonStoreUInt16BE

```c
void TbxMbCommonStoreUInt16BE(uint16_t   value,
                              uint8_t  * data)
```

Helper function to store an unsigned 16-bit value in the data of a Modbus packet, where 16-bit values are always stored in the big endian format.

| Parameter | Description                                                  |
| --------- | ------------------------------------------------------------ |
| `value`   | The unsigned 16-bit value to store.                          |
| `data`    | Pointer to the byte array where to store the value in the big endian format. |

### RTU

#### TbxMbRtuCreate

```c
tTbxMbTp TbxMbRtuCreate(uint8_t            nodeAddr, 
                        tTbxMbUartPort     port, 
                        tTbxMbUartBaudrate baudrate,
                        tTbxMbUartStopbits stopbits,
                        tTbxMbUartParity   parity)
```

Creates a Modbus RTU transport layer object, which can later on be linked to a Modbus client or server channel.

Example for the following communication settings:

* First serial port on the board.
* Baudrate 19200 bits/second.
* 8 data-bits (default and fixed for an RTU transport layer).
* even parity.
* 1 stop-bit.
* Node address 10.

```c
tTbxMbTp modbusTp = TbxMbRtuCreate(10U, TBX_MB_UART_PORT1, TBX_MB_UART_19200BPS,
                                   TBX_MB_UART_1_STOPBITS, TBX_MB_EVEN_PARITY);   
```

| Parameter  | Description                                                  |
| ---------- | ------------------------------------------------------------ |
| `nodeAddr` | The address of the node. Can be in the range `1`..`247` for a server node. Set it to `0` for<br>a client. |
| `port`     | The serial port to use. The actual meaning of the serial port is hardware dependent. It<br>typically maps to the UART peripheral number. E.g. `TBX_MB_UART_PORT1` = USART1 on<br>an STM32. |
| `baudrate` | The desired communication speed.                             |
| `stopbits` | Number of stop bits at the end of a character.               |
| `parity`   | Parity bit type to use.                                      |

| Return value                                                 |
| ------------------------------------------------------------ |
| Handle to the newly created RTU transport layer object if successful, `NULL` otherwise. |

#### TbxMbRtuFree

```c
void TbxMbRtuFree(tTbxMbTp transport)
```

Releases a Modbus RTU transport layer object, previously created with [TbxMbRtuCreate()](#tbxmbrtucreate).

| Parameter   | Description                                      |
| ----------- | ------------------------------------------------ |
| `transport` | Handle to RTU transport layer object to release. |

### UART

#### TbxMbUartTransmitComplete

```c
void TbxMbUartTransmitComplete(tTbxMbUartPort port)
```

Event function to signal to the UART module that the entire transfer, initiated by `TbxMbPortUartTransmit`, completed. This function should be called by the hardware specific UART port (located in `tbxmb_port.c`) at TX interrupt level.

| Parameter | Description                                     |
| --------- | ----------------------------------------------- |
| `port`    | The serial port that the transfer completed on. |

#### TbxMbUartDataReceived

```c
void TbxMbUartDataReceived(tTbxMbUartPort         port, 
                           uint8_t        const * data, 
                           uint8_t                len)
```

Event function to signal the reception of new data to the UART module. This function should be called by the hardware specific UART port (located in `tbxmb_port.c`) at RX interrupt level.

| Parameter | Description                                     |
| --------- | ----------------------------------------------- |
| `port`    | The serial port that the transfer completed on. |
| `data`    | Byte array with newly received data.            |
| `len`     | Number of newly received bytes.                 |
