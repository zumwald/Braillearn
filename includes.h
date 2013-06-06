/*************************************************************************
* includes.h - Master header file template for star12 projects.
*              This version is for ETec454 Lab1 using the Metrowerks 
*              compiler. 
*
* Todd Morton 01/05/05
* Modified 01/31/13 by Dan Zumwalt
**************************************************************************
* General type definitions 
*************************************************************************/
#ifndef _INCLUDES_H
#define _INCLUDES_H

typedef unsigned char   INT8U;
typedef signed char     INT8S;
typedef unsigned short  INT16U;
typedef signed short    INT16S;
typedef unsigned long   INT32U;
typedef signed long     INT32S;

/*************************************************************************
* General Defined Constants 
*************************************************************************/
#define FALSE    0
#define TRUE     1

/*************************************************************************
* General defined macros 
*************************************************************************/
#define FOREVER()       while(1)
#define TRAP()          while(1){}

/*************************************************************************
* MCU specific definitions
* Move the comment to use the project MCU, or enter a new MCU header.
*************************************************************************/
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"
#include "driverlib/debug.h"
#include "driverlib/fpu.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/uart.h"
#include "driverlib/flash.h"

/*************************************************************************
* Project Constant and Macro Definitions
*************************************************************************/
#define BIT0            0x01
#define BIT1            0x02
#define BIT2            0x04
#define BIT3            0x08
#define BIT4            0x10
#define BIT5            0x20
#define BIT6            0x40
#define BIT7            0x80
#define GPIO_ALL 		0x000000ff

#define SLICE_PERMS		10
#define DB_PORT			GPIO_PORTE_BASE
#define DB_SLICE        BIT3
#define DB_UART         BIT4
#define DB_KEY          BIT5
#define DB_DISP			BIT6

/*		UART		*/
#define UARTBUFFERSIZE	16
#define CRYSTAL_16MHZ (INT32U)16000000

/*		SPI			*/
#define SPIBAUD			115200
#define TIMEOUTTICKS	10
#define DISPLAYLEN		8

/*		FILE IO		*/
#define BUFFERLEN		16
#define	NUMFILES		50
#define FILEBASEADDR	0x00004000
#define FILEBLOCKSIZE 	1024

/*		KEY			*/
#define KEY_SPACE 	0x00002000	// swapped with '<' for demo board 130603
#define KEY_LEFT  	0x00008000	// swapped with ' ' for demo board 130603
#define KEY_RIGHT 	0x00004000
#define KEYBITS		0xFF
#define NAVBITS		(BIT5|BIT6|BIT7)
#define KEYPORT_BASE	GPIO_PORTB_BASE
#define NAVPORT_BASE	GPIO_PORTC_BASE

/*		CONTROL		*/
#define	ONESECOND		(1000/SLICE_PERMS)
#define	TWOSECONDS		(2000/SLICE_PERMS)
#define THREESECONDS	(3000/SLICE_PERMS)
#define FOURSECONDS		(4000/SLICE_PERMS)
#define FIVESECONDS		(5000/SLICE_PERMS)
#define	SIXSECONDS		(6000/SLICE_PERMS)
#define ONEMINUTE		(60000/SLICE_PERMS)
#define FORWARD			TRUE
#define BACK			FALSE

/*************************************************************************
* System Header Files 
*************************************************************************/

/*************************************************************************
* Module Header Files or Declarations 
*************************************************************************/
#include "Key.h"
#include "Control.h"
#include "UART.h"
#include "SPI.h"
#include "FileIO.h"
#include "Display.h"
/************************************************************************/

#endif
