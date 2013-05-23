/*
 * UART.c
 *	Modifed ~~ See Below.
 *  Created on: May 22, 2013
 *      Author: zumwald
 */


//*****************************************************************************
//
// uart_echo.c - Example for reading data from and writing data to the UART in
//               an interrupt driven fashion.
//
// Copyright (c) 2012 Texas Instruments Incorporated.  All rights reserved.
// Software License Agreement
//
// Texas Instruments (TI) is supplying this software for use solely and
// exclusively on TI's microcontroller products. The software is owned by
// TI and/or its suppliers, and is protected under applicable copyright
// laws. You may not combine this software with "viral" open-source
// software in order to form a larger program.
//
// THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
// NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
// NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. TI SHALL NOT, UNDER ANY
// CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
// DAMAGES, FOR ANY REASON WHATSOEVER.
//
// This is part of revision 9453 of the EK-LM4F120XL Firmware Package.
//
//*****************************************************************************

/********************************************************************
* Project master header file
********************************************************************/
#include "includes.h"

/********************************************************************
* Module Defines
********************************************************************/
#define BAUD	115200

/********************************************************************
* Public Resources
********************************************************************/
void UARTSend(const unsigned char *pucBuffer, unsigned long ulCount);

/********************************************************************
* Private Resources
********************************************************************/
const INT8U menuStr[36][10] = {"\r _       ","        _ ","_ _      _","_____     ","     _____","  _   _ \r\n",
						   "\r| |      ","       (_)"," | |    | "," ____|   /","\\   |  __ ","\\| \\ | |\r\n",
						   "\r| |__  _ ","__ __ _ _|"," | |    | ","|__     / "," \\  | |__)"," |  \\| |\r\n",
						   "\r| '_ \\| '","__/ _` | |"," | |    | "," __|   / /","\\ \\ |  _  ","/| . ` |\r\n",
						   "\r| |_) | |"," | (_| | |"," | |____| ","|____ / __","__ \\| | \\ ","\\| |\\  |\r\n",
						   "\r|_.__/|_|","  \\__,_|_|","_|______|_","_____/_/  ","  \\_\\_|  \\","_\\_| \\_|\r\n"};

//*****************************************************************************
//
// The UART interrupt handler.
//
//*****************************************************************************
void
UARTIntHandler(void)
{
    unsigned long ulStatus;

    // Get the interrupt status.
    ulStatus = ROM_UARTIntStatus(UART0_BASE, true);

    // Clear the asserted interrupts.
    ROM_UARTIntClear(UART0_BASE, ulStatus);

    // Loop while there are characters in the receive FIFO.
    while(ROM_UARTCharsAvail(UART0_BASE))
    {
        // Read the next character from the UART and write it back to the UART.
        ROM_UARTCharPutNonBlocking(UART0_BASE,
                                   ROM_UARTCharGetNonBlocking(UART0_BASE));
    }
}

//*****************************************************************************
//
// Send a string to the UART.	-	Public, Non-blocking
//
//*****************************************************************************
void
UARTSend(const unsigned char *pucBuffer, unsigned long ulCount)
{
    //
    // Loop while there are more characters to send.
    //
#if defined(DB_UART) && defined(DB_PORT)
    GPIOPinWrite(DB_PORT, DB_UART, 0);
#endif
    while(ulCount--)
    {
        //
        // Write the next character to the UART.
        //
        ROM_UARTCharPutNonBlocking(UART0_BASE, *pucBuffer++);
    }
#if defined(DB_UART) && defined(DB_PORT)
    GPIOPinWrite(DB_PORT, DB_UART, DB_UART);
#endif
}

/********************************************************************
* UARTInit() - Initialization routine for the UART module.
********************************************************************/
void
UARTInit(void)
{
    //
    // Enable lazy stacking for interrupt handlers.  This allows floating-point
    // instructions to be used within interrupt handlers, but at the expense of
    // extra stack usage.
    //
    FPUEnable();
    FPULazyStackingEnable();

    // Enable the peripherals used by this example.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    // Set GPIO A0 and A1 as UART pins.
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    //	Set UART Clock source as 16MHZ crystal
    UARTClockSourceSet(UART0_BASE,UART_CLOCK_PIOSC);

    // Configure the UART for 115,200, 8-N-1 operation.
    UARTConfigSetExpClk(UART0_BASE, CRYSTAL_16MHZ, (INT32U)BAUD,
                            (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
                             UART_CONFIG_PAR_NONE));

    /*	Enable FIFOs	*/
    UARTFIFOEnable(UART0_BASE);
    UARTFIFOLevelSet(UART0_BASE,UART_FIFO_TX4_8,UART_FIFO_RX4_8);

    // Enable the UART interrupt.
    IntEnable(INT_UART0);
    UARTIntEnable(UART0_BASE, UART_INT_RX | UART_INT_RT);
}

//*****************************************************************************
//
// Send the welcome string to the UART.	-	Public, BLOCKING 30mS
//
//*****************************************************************************
void UART_Welcome(void){

	int i, j;
#if defined(DB_UART) && defined(DB_PORT)
    GPIOPinWrite(DB_PORT, DB_UART, 0);
#endif
	for (i = 0; i<36; i++){
		for (j = 0; j<10; j++){
			UARTCharPut(UART0_BASE,(INT8U)menuStr[i][j]);
		}
	}
#if defined(DB_UART) && defined(DB_PORT)
    GPIOPinWrite(DB_PORT, DB_UART, DB_UART);
#endif
}

