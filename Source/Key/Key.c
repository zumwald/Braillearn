/********************************************************************
* Key.c - A keypad module for a 4x4 matrix keypad.
* Revised to avoid shorts when two keys are pressed. This version
* allows for multiple column keys to be pressed and mapped to a
* different code by changing ColTable[] in KeyScan(). Multiple rows 
* can not be resolved. The topmost button will be used.
* The KeyCodeTable[] is currently set to generate ASCII codes.
* 02/20/2001 Todd Morton
*
* Rev.1
* Changed the ASCII codes for the function keys, A-D, to DC1-DC4.
* This is more appropriate for applications that use Multitap alpha
* entry using the numeric keys.
* 02/08/07 Todd Morton 
*
* Rev.2
* Removed all ASCII codes for Braillearn application.
* 05/19/2013 Dan Zumwalt
*
*********************************************************************
* Project master header file
********************************************************************/
#include "includes.h"

/********************************************************************
* Module Defines
********************************************************************/
typedef enum{KEY_OFF,KEY_EDGE,KEY_VERF} KEYSTATES;

/********************************************************************
* Public Resources
********************************************************************/
INT8U GetKey(INT8U *cntlFlag, INT16U *rawKey); /* Returns current value of KeyBuffer*/
void KeyInit(void);             /* Keypad Initialization           */
void KeyTask(void);             /* Main keypad read task           */
const INT8U UBrailleLookup[256] = {0,',','!','\t',0,';',':','\n',
									'.','\'',')',']',0,'"','?','0',
							/*16*/	'a','d','c',0,'b',0,0,0,
									'A','D','C',0,'B','@',0,'8',
							/*32*/	'e','h','g','`','f','<',0,0,
									'E','H','G',0,'F','~',0,'4',
							/*48*/	0,0,0,0,0,0,0,0,
									0,0,0,0,0,0,0,'x',
							/*64*/	'i','n','m','k','l',0,'j',0,
									'I','N','M','K','L',0,'J',0,
							/*80*/	'(',0,0,0,0,0,0,0,
									0,'2',0,0,0,0,0,'+',
							/*96*/	'u','x','w',0,'v',0,0,0,
									'U','X','W',0,'V',0,0,'6',
							/*112*/	0,0,0,0,0,0,0,0,
									0,0,0,0,0,0,0,'=',
							/*128*/	'o','t','s','q','r',0,'p',0,
									'O','T','S','Q','R','}','P','1',
							/*144*/	'y',0,0,0,'z',0,0,0,
									'Y',0,0,0,'Z',0,0,'9',
							/*160*/	0,0,0,0,'>',0,0,0,
									0,0,0,0,0,0,0,'5',
							/*176*/	0,'{',0,0,0,0,0,0,
									0,0,0,0,0,0,0,0,
							/*192*/	0,'*',0,'#',0,0,0,0,
									0,0,'$',0,0,0,0,'3',
							/*208*/	'[',0,0,0,0,0,0,0,
									0,0,0,0,0,0,0,'-',
							/*224*/	'\b','/','|',0,'\\',0,0,0,
									'_','&',0,0,0,0,0,'7',
							/*240*/	0,0,0,0,0,0,0,0,
							/*248*/	0,0,0,0,0,0,0,0};/*255*/

/********************************************************************
* Private Resources
********************************************************************/
static INT16U KeyScan(void);         /* Makes a single keypad scan  */
static INT16U KeyBuffer;             /* Holds the ASCII code for key*/
INT8U IsCntlKey(INT16U key);

/********************************************************************
* GetKey() - A function to provide access to KeyBuffer. This function
*            provides public access to KeyBuffer. It clears KeyBuffer
*            after for read-once handshaking.
*    - Public
********************************************************************/
INT8U GetKey(INT8U *cntlFlag, INT16U *rawKey){
    INT8U key;

    *rawKey = KeyBuffer;
    key = IsCntlKey(KeyBuffer);
    if (key != 0){
    	*cntlFlag = TRUE;
    }else{
    	key = UBrailleLookup[(INT8U)KeyBuffer];
    	*cntlFlag = FALSE;
    }
    KeyBuffer = 0;
    return key;
}

/********************************************************************
* IsCntlKey() - A function to return false if key is a character,
* 				otherwise the value of the control key is returned.
* 				Ex: '<', '>', or ' '.
*    - Private
********************************************************************/
INT8U IsCntlKey(INT16U key){

	if ((key & KEY_LEFT) && (key & KEY_RIGHT)){
		return 'M';	//	menu control
	}else if (key & KEY_LEFT){
		return '<';
	}else if (key & KEY_RIGHT){
		return '>';
	}else if (key & KEY_SPACE){
		return ' ';
	}else{
		return false;
	}
}

/********************************************************************
* KeyInit() - Initialization routine for the keypad module
*             The columns are normally set as inputs and, since they 
*             are pulled high, they are one. Then to pull a row low
*             during scanning, the direction for that pin is changed
*             to an output.
********************************************************************/
void KeyInit(void){

	/*	Configure Keyboard GPIO ports as Inputs	*/
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
	GPIOPinTypeGPIOInput(KEYPORT_BASE,GPIO_ALL);
	GPIOPinTypeGPIOInput(NAVPORT_BASE,BIT5|BIT6|BIT7);
	GPIOPadConfigSet(KEYPORT_BASE,KEYBITS,GPIO_STRENGTH_2MA,GPIO_PIN_TYPE_STD_WPU);
	GPIOPadConfigSet(NAVPORT_BASE,NAVBITS,GPIO_STRENGTH_2MA,GPIO_PIN_TYPE_STD_WPU);
    KeyBuffer = 0;           /* Init KeyBuffer      */
}

/********************************************************************
* KeyTask() - Read the keypad and updates KeyBuffer. 
*             A task decomposed into states for detecting and
*             verifying keypresses. This task should be called
*             periodically with a period greater than the worst case
*             switch bounce time and less than the shortest switch
*             activation time minus the bounce time. The switch must 
*             be released to have multiple acknowledged presses.
* (Public)
********************************************************************/
void KeyTask(void) {

    INT16U cur_key;
    static INT16U last_key = 0;
    static KEYSTATES KeyState = KEY_OFF;

#if defined(DB_KEY) && defined(DB_PORT)
    GPIOPinWrite(DB_PORT, DB_KEY, 0);
#endif

    cur_key = KeyScan();
    if(KeyState == KEY_OFF){    /* Key released state */
        if(cur_key != 0){
            KeyState = KEY_EDGE;
        }else{} /* wait for key press */
    }else if(KeyState == KEY_EDGE){     /* Keypress detected state*/
        if(cur_key == last_key){        /* Keypress verified */
            KeyState = KEY_VERF;
            KeyBuffer = cur_key; 		/*update buffer */
        }else if( cur_key == 0){        /* Unvalidated, start over */
            KeyState = KEY_OFF;
        }else{                          /*Unvalidated, diff key edge*/
        }
    }else if(KeyState == KEY_VERF){     /* Keypress verified state */
        if((cur_key == 0) || (cur_key != last_key)){
            KeyState = KEY_OFF;
        }else{ /* wait for release or key change */
        }
    }else{ /* In case of error */
        KeyState = KEY_OFF;             /* Should never get here */
    }
    last_key = cur_key;                 /* Save key for next time */

#if defined DB_KEY && defined DB_PORT
    GPIOPinWrite(DB_PORT, DB_KEY, DB_KEY);
#endif
}

/********************************************************************
* KeyScan() - Scans the keypad and returns a keycode. Prioritizes
* 			  navigation and space keys over keyboard keys.
* (Private)
********************************************************************/
static INT16U KeyScan(void) {

	volatile INT8U keyboard = 0, navkeys = 0;
	volatile INT16U returnval = 0;

	keyboard = (INT8U)GPIOPinRead(GPIO_PORTB_BASE,KEYBITS);
	keyboard ^= KEYBITS;
	navkeys  = (INT8U)GPIOPinRead(GPIO_PORTC_BASE,NAVBITS);
	navkeys = navkeys ^ NAVBITS;
	if (navkeys != 0){
		returnval = (INT16U)navkeys;
		returnval = returnval << 8;	// shift to next byte
		return returnval;
	}else{
		returnval = (INT16U)keyboard;
		return returnval;
	}
}
/********************************************************************/
