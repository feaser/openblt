/************************************************************************************//**
* \file         main.c
* \brief        SerialBoot program source file.
* \ingroup      SerialBoot
* \internal
*----------------------------------------------------------------------------------------
*                          C O P Y R I G H T
*----------------------------------------------------------------------------------------
*   Copyright (c) 2017  by Feaser    http://www.feaser.com    All rights reserved
*
*----------------------------------------------------------------------------------------
*                            L I C E N S E
*----------------------------------------------------------------------------------------
* This file is part of OpenBLT. OpenBLT is free software: you can redistribute it and/or
* modify it under the terms of the GNU General Public License as published by the Free
* Software Foundation, either version 3 of the License, or (at your option) any later
* version.
*
* OpenBLT is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
* without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
* PURPOSE. See the GNU General Public License for more details.
*
* You have received a copy of the GNU General Public License along with OpenBLT. It
* should be located in ".\Doc\license.html". If not, contact Feaser to obtain a copy.
*
* \endinternal
****************************************************************************************/

/****************************************************************************************
* Include files
****************************************************************************************/
#include <stdio.h>                          /* standard I/O functions                  */
#include <string.h>                         /* for string library                      */
#include "xcploader.h"                      /* XCP loader module                       */
#include "xcptpuart.h"                      /* XCP transport layer for UART            */
#include "firmware.h"                       /* Firmware module                         */
#include "srecparser.h"                     /* S-record parser                         */
#include "timeutil.h"                       /* for time utilities module               */


/****************************************************************************************
* Macro definitions
****************************************************************************************/
/* Program return codes. */
#define RESULT_OK                           (0)
#define RESULT_COMMANDLINE_ERROR            (1)
#define RESULT_FIRMWARE_LOAD_ERROR          (2)
#define RESULT_PROGRAM_START_ERROR          (3)
#define RESULT_MEMORY_ERASE_ERROR           (4)
#define RESULT_PROGRAM_STOP_ERROR           (5)
#define RESULT_MEMORY_PROGRAM_ERROR         (6)


/****************************************************************************************
* Local data declarations
****************************************************************************************/
/** \brief The firmware filename that is specified at the command line. */
static char *firmwareFilename;

/** \brief XCP loader settings. */
static tXcpSettings xcpSettings =
{
  .timeoutT1 = 1000,
  .timeoutT3 = 2000,
  .timeoutT4 = 10000,
  .timeoutT5 = 1000,
  .timeoutT7 = 2000
};

/** \brief XCP UART transport layer settings. */
static tXcpTpUartSettings xcpTpUartSettings =
{
  .baudrate = SERIALPORT_BR57600,
  .portname = "/dev/ttyS0"
};


/****************************************************************************************
* Function prototypes
****************************************************************************************/
static void DisplayProgramInfo(void);
static void DisplayProgramUsage(void);
static bool ParseCommandLine(int argc, char *argv[]);


/************************************************************************************//**
** \brief     This is the program entry point.
** \param     argc Number of program arguments.
** \param     argv Array with program arguments.
** \return    Program return code. 0 for success, error code otherwise.
**
****************************************************************************************/
int main(int argc, char *argv[])
{
  int result = RESULT_OK;
  uint32_t fwBaseAddress;
  uint32_t fwTotalSize;
  uint32_t segmentIdx;
  const tFirmwareSegment *segment;

  /* -------------------- Display info ----------------------------------------------- */
  /* inform user about the program */
  DisplayProgramInfo();

  /* -------------------- Process command line --------------------------------------- */
  /* start out by making sure program was started with the correct parameters */
  if (!ParseCommandLine(argc, argv))
  {
    /* parameters invalid. inform user about how this program works */
    DisplayProgramUsage();
    return RESULT_COMMANDLINE_ERROR;
  }

  /* -------------------- Initialization --------------------------------------------- */
  /* initialize the XCP loader module using the UART transport layer. */
  XcpLoaderInit(&xcpSettings, XcpTpUartGetTransport(), &xcpTpUartSettings);
  /* initialize the firmware module and link the S-recorder parser */
  FirmwareInit(SRecParserGetParser());

  /* -------------------- Parse the firmware file ------------------------------------ */
  /* attempt to load the firmware file */
  printf("Loading firmware file..."); fflush(stdout);
  if (!FirmwareLoadFromFile(firmwareFilename))
  {
    /* set error code and abort */
    printf("ERROR\n");
    result = RESULT_FIRMWARE_LOAD_ERROR;
    goto finish;
  }
  printf("OK\n");
  /* determine firmware base address and total size */
  for (segmentIdx=0; segmentIdx<FirmwareGetSegmentCount(); segmentIdx++)
  {
    segment = FirmwareGetSegment(segmentIdx);
    /* is this the first segment? */
    if (segmentIdx == 0)
    {
      /* initialize */
      fwBaseAddress = segment->base;
      fwTotalSize = segment->length;
    }
    else
    {
      /* update */
      if (segment->base < fwBaseAddress)
      {
        fwBaseAddress = segment->base;
      }
      fwTotalSize += segment->length;
    }
  }
  /* display some firmware statistics */
  printf("-> Number of segments:  %u\n", FirmwareGetSegmentCount());
  printf("-> Base memory address: 0x%08x\n", fwBaseAddress);
  printf("-> Total data bytes:    %u\n", fwTotalSize);

  /* -------------------- Connect to target ------------------------------------------ */
  printf("Connecting to bootloader..."); fflush(stdout);
  if (!XcpLoaderConnect())
  {
    /* no response. prompt the user to reset the system */
    printf("TIMEOUT\nReset your microcontroller..."); fflush(stdout);
    /* now keep retrying until we get a response */
    while (!XcpLoaderConnect())
    {
      /* delay a bit to not pump up the CPU load */
      TimeUtilDelayMs(20);
    }
  }
  printf("OK\n");

  /* -------------------- Start the programming session ------------------------------ */
  /* attempt to start the programming session */
  printf("Starting programming session..."); fflush(stdout);
  if (!XcpLoaderStartProgrammingSession())
  {
    /* set error code and abort */
    printf("ERROR\n");
    result = RESULT_PROGRAM_START_ERROR;
    goto finish;
  }
  printf("OK\n");

  /* -------------------- Erase memory ----------------------------------------------- */
  /* erase each segment one at a time  */
  for (segmentIdx=0; segmentIdx<FirmwareGetSegmentCount(); segmentIdx++)
  {
    segment = FirmwareGetSegment(segmentIdx);
    /* attempt to erase memory */
    printf("Erasing %u bytes starting at 0x%08x...", segment->length, segment->base); fflush(stdout);
    if (!XcpLoaderClearMemory(segment->base, segment->length))
    {
      /* set error code and abort */
      printf("ERROR\n");
      result = RESULT_MEMORY_ERASE_ERROR;
      goto finish;
    }
    printf("OK\n");
  }

  /* -------------------- Program data ----------------------------------------------- */
  /* program each segment one at a time  */
  for (segmentIdx=0; segmentIdx<FirmwareGetSegmentCount(); segmentIdx++)
  {
    segment = FirmwareGetSegment(segmentIdx);
    /* attempt to program memory */
    printf("Programming %u bytes starting at 0x%08x...", segment->length, segment->base); fflush(stdout);
    if (!XcpLoaderProgramData(segment->base, segment->length, segment->data))
    {
      /* set error code and abort */
      printf("ERROR\n");
      result = RESULT_MEMORY_PROGRAM_ERROR;
      goto finish;
    }
    printf("OK\n");
  }

  /* -------------------- Stop the programming session ------------------------------- */
  /* attempt to stop the programming session */
  printf("Finishing programming session..."); fflush(stdout);
  if (!XcpLoaderStopProgrammingSession())
  {
    /* set error code and abort */
    printf("ERROR\n");
    result = RESULT_PROGRAM_STOP_ERROR;
    goto finish;
  }
  printf("OK\n");

  /* -------------------- Cleanup ---------------------------------------------------- */
finish:
  /* uninitialize the firmware module */
  FirmwareDeinit();
  /* uninitialize the XCP loader module. note that this automatically disconnects the
   * slave, if connected, by requesting it to perform a reset.
   */
  XcpLoaderDeinit();
  /* give result back */
  return result;
} /*** end of main ***/


/************************************************************************************//**
** \brief     Outputs information to the user about this program.
** \return    none.
**
****************************************************************************************/
static void DisplayProgramInfo(void)
{
  printf("-------------------------------------------------------------------------\n");
  printf("SerialBoot version 2.00. Performs firmware updates via the serial port\n");
  printf("for a microcontroller based system that runs the OpenBLT bootloader.\n\n");
  printf("Copyright (c) by Feaser  http://www.feaser.com\n");
  printf("-------------------------------------------------------------------------\n");
} /*** end of DisplayProgramInfo ***/


/************************************************************************************//**
** \brief     Outputs information to the user about how to use this program.
** \return    none.
**
****************************************************************************************/
static void DisplayProgramUsage(void)
{
  printf("Usage:    SerialBoot -d[device] -b[baudrate] [s-record file]\n\n");
#ifdef PLATFORM_WIN32
  printf("Example:  SerialBoot -dCOM4 -b57600 myfirmware.s19\n");
  printf("          -> Connects to COM4, configures a communication speed of 57600\n");
#else
  printf("Example:  SerialBoot -d/dev/ttyS0 -b57600 myfirmware.s19\n");
  printf("          -> Connects to ttyS0, configures a communication speed of 57600\n");
#endif
  printf("             bits/second and programs the myfirmware.s19 file in non-\n");
  printf("             volatile memory of the microcontroller using OpenBLT.\n");
  printf("             Supported baudrates are: 9600, 19200, 38400, 57600 and\n");
  printf("             115200 bits/second.\n");
  printf("-------------------------------------------------------------------------\n");
} /*** end of DisplayProgramUsage ***/


/************************************************************************************//**
** \brief     Parses the command line arguments. A fixed amount of arguments is expected.
**            The program should be called as:
**              SerialBoot -d[device] -b[baudrate] [s-record file]
** \param     argc Number of program parameters.
** \param     argv array to program parameter strings.
** \return    True if successful, false otherwise.
**
****************************************************************************************/
static bool ParseCommandLine(int argc, char *argv[])
{
  uint8_t paramIdx;
  bool firmwareFileFound = false;
  uint32_t baudrateValue;

  /* make sure that enough arguments were specified. this program needs at least 2. the
   * first one is always the program name and the second one is the s-record file.
   */
  if (argc < 2)
  {
    return false;
  }

  /* loop through all the command line parameters, just skip the 1st one because this
   * is the name of the program, which we are not interested in.
   */
  for (paramIdx=1; paramIdx<argc; paramIdx++)
  {
    /* is this the device name? */
    if ( (argv[paramIdx][0] == '-') && (argv[paramIdx][1] == 'd') )
    {
      /* set the device name */
      xcpTpUartSettings.portname = &argv[paramIdx][2];
      continue;
    }
    /* is this the baudrate? */
    if ( (argv[paramIdx][0] == '-') && (argv[paramIdx][1] == 'b') )
    {
      /* extract the baudrate */
      sscanf(&argv[paramIdx][2], "%u", &baudrateValue);
      /* convert to the baudrate type */
      switch (baudrateValue)
      {
        case 115200:
          xcpTpUartSettings.baudrate = SERIALPORT_BR115200;
          break;
        case 57600:
          xcpTpUartSettings.baudrate = SERIALPORT_BR57600;
          break;
        case 38400:
          xcpTpUartSettings.baudrate = SERIALPORT_BR38400;
          break;
        case 19200:
          xcpTpUartSettings.baudrate = SERIALPORT_BR19200;
          break;
        case 9600:
          xcpTpUartSettings.baudrate = SERIALPORT_BR9600;
          break;
        default:
          /* unsupported baudrate specified */
          return false;
      }
      continue;
    }
    /* still here so it must be the filename */
    else
    {
      /* set the file name and set flag that this parameter was found */
      firmwareFilename = &argv[paramIdx][0];
      firmwareFileFound = true;
    }
  }

  /* verify if all required parameters were found */
  if (!firmwareFileFound)
  {
    return false;
  }
  /* still here so the parsing was successful */
  return true;
} /*** end of ParseCommandLine ***/


/*********************************** end of main.c *************************************/

