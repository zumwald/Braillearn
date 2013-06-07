/*
 * Main.c
 *
 *  Created on: May 19, 2013
 *      Author: Dan Zumwalt
 */
/********************************************************************
* Project master header file
********************************************************************/
#include "includes.h"

/********************************************************************
* Module Defines
********************************************************************/
#define IDLE() while(!tickFlag);tickFlag=0

/********************************************************************
* Private Resources
********************************************************************/
volatile INT8U tickFlag = 0x00;

/********************************************************************
* Main() - System initialization routine and Timeslice Kernel.
********************************************************************/
int main(void) {
	INT32U ulPeriod;
	INT8U  keyOn = TRUE;

	/*	Initialize System Clock	*/
	SysCtlClockSet(
			SYSCTL_SYSDIV_8 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ
					| SYSCTL_OSC_MAIN);

	/*	Initialize Peripherals	*/
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);

	/*	Initialize GPIO States	*/
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE,
			GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);

	/*	Initialize Bit Toggles	*/
	GPIOPinTypeGPIOOutput(DB_PORT,DB_KEY|DB_UART|DB_SLICE);
	GPIOPinWrite(DB_PORT,DB_KEY|DB_UART|DB_SLICE,DB_KEY|DB_UART|DB_SLICE);

	/*	Initialize Timer Module	*/
	TimerConfigure(TIMER0_BASE, TIMER_CFG_32_BIT_PER);
	ulPeriod = 250000; //ulPeriod = (SysCtlClockGet() / 10) / 2;
	TimerLoadSet(TIMER0_BASE, TIMER_A, ulPeriod - 1);
	IntEnable(INT_TIMER0A);		// Enable timer interrupt
	TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

/*	Initialize Modules	*/
	KeyInit();
	CntlInit();
	UARTInit();
	SPIInit();
	DisplayInit();

/*	Enable Master Interrupts	*/
	IntMasterEnable();

	/*	Send welcome message	*/
	UART_Welcome();

	/*	Start Timeslice timer	*/
	TimerEnable(TIMER0_BASE, TIMER_A);

	while (1) {
#if defined(DB_SLICE) && defined(DB_PORT)
    GPIOPinWrite(DB_PORT, DB_SLICE, 0);
#endif
		IDLE();
#if defined(DB_SLICE) && defined(DB_PORT)
    GPIOPinWrite(DB_PORT, DB_SLICE, DB_SLICE);
#endif
    if(keyOn >= 3){
		KeyTask();
		keyOn = 0x00;
    }else{}
		CntlTask();
		UARTTask();
		DisplayTask();
		keyOn++;
	}
}

/********************************************************************
* Timer ISR - Generated on Timer0 tick, posts tickFlag for IDLE()
********************************************************************/
void Timer0IntHandler(void) {
	// Clear the timer interrupt
	TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
	// Read the current state of the GPIO pin and
	// write back the opposite state
	if (GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_2)) {
		GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3, 0);
	} else {
		GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, GPIO_PIN_2);
	}
	/*	Signal tickFlag	*/
	tickFlag = 0x01;
}
