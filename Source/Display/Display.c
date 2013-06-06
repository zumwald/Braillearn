/*
 * Display.c
 *
 *  Created on: May 27, 2013
 *      Author: Dan Zumwalt
 ***********************************************************************
 * Project master header file
 ********************************************************************/
#include "includes.h"

/********************************************************************
 * Module Defines
 ********************************************************************/

/********************************************************************
 * Public Resources
 ********************************************************************/
void DisplayInit(void);
void DisplayTask(void);
void DisplayUpdate(INT8U *data);
void DisplayClear(void);

/********************************************************************
 * Private Resources
 ********************************************************************/
INT8U DisplayBuffer[DISPLAYLEN];
INT8U BufferIndex;
volatile INT8U UpdateDisplay;
INT8U BrailleBuffer[DISPLAYLEN];

/********************************************************************
 * DisplayUpdate() - Sets DisplayBuffer to the ASCII data specified
 * 		- public
 ********************************************************************/
void DisplayUpdate(INT8U *data) {

	INT8U i;
	UpdateDisplay = TRUE;

	for (i = 0; i < DISPLAYLEN; i++) {
		if (data[i] == 0x00) {
			DisplayBuffer[i] = 0x00;
			break;
		} else {
			DisplayBuffer[i] = data[i];
		}
	}
	BufferIndex = i;
}

/********************************************************************
 * DisplayAppendChar() - Shifts given char into end of display
 * 		- public
 ********************************************************************/
void DisplayAppendChar(INT8U key) {

	INT8U i;
	//	Shift current buffer
	for (i = 0; i < DISPLAYLEN - 1; i++) {
		DisplayBuffer[i] = DisplayBuffer[i + 1];
	}
	//	Insert key
	DisplayBuffer[i] = key;
	UpdateDisplay = TRUE;
}

/********************************************************************
 * DisplayBlinkMiss() - sends command to follower to blink first
 * 						char on display, indicating a 'miss' to user
 * 						while in "LEARN" mode.
 * 		- public
 ********************************************************************/
void DisplayBlinkMiss(void) {
	DisplayBuffer[0] = 0x00;
	UpdateDisplay = TRUE;
}

/********************************************************************
 * DisplayClear() - Clears display.
 * 		- public
 ********************************************************************/
void DisplayClear(void) {

	DisplayBuffer[0] = 0x00;
	BufferIndex = 0;
}

/********************************************************************
 * DisplayInit() - Display Initialization. Must run before calling
 * 				   display functions.
 * 		- public
 ********************************************************************/
void DisplayInit(void) {

	int i;
	//	Initialize DisplayBuffer to be empty
	for (i = 0; i < DISPLAYLEN; i++) {
		DisplayBuffer[i] = 0xff;
	}
	//	clear display
	(void) SPISendLine(DisplayBuffer, 4);
	UpdateDisplay = FALSE;
	BufferIndex = 0;
}

/********************************************************************
 * DisplayTask() - Updates display according to UpdateDisplay
 * (Public)
 ********************************************************************/
void DisplayTask(void) {

	INT8U i;
	INT16U j;

#if defined(DB_DISP) && defined(DB_PORT)
	GPIOPinWrite(DB_PORT, DB_DISP, 0);
#endif
	if (UpdateDisplay) {
		/*	translate displaybuffer to braillebuffer	*/
		for (i = 0; i < DISPLAYLEN; i++) {
			if ((DisplayBuffer[i] == 0x00) || (i > BufferIndex) || (DisplayBuffer[i] == ' ')) {
				BrailleBuffer[i] = 0xff;
			} else {
				for (j = 0; j < 256; j++) {
					if (DisplayBuffer[i] == UBrailleLookup[j]) {
						BrailleBuffer[i] = (INT8U) ~j;
						break;
					} else {
					}
				}
			}
		}
		/*	Send Braille data to display	*/
		(void) SPISendLine(BrailleBuffer, 4);
	} else {
	}
#if defined(DB_DISP) && defined(DB_PORT)
	GPIOPinWrite(DB_PORT, DB_DISP, DB_DISP);
#endif
}
/*******************************************************************/
