/*
 * Control.c
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
typedef enum {
	MENU, NOTES, READ, LEARN, CHAT
} CNTLSTATES;

typedef enum {
	nNOTES, nNEW, nOLD, nSELECT
} NOTESTATES;

typedef enum {
	rREAD, rSELECT
} READSTATES;

/********************************************************************
 * Public Resources
 ********************************************************************/
void CntlTask(void);

/********************************************************************
 * TODO Private Resources
 ********************************************************************/
static CNTLSTATES progState;
static NOTESTATES noteState;
static READSTATES readState;
static INT32U sliceCnt;
static INT8U bMenuDisplayed;

//static INT8U displayBuffer[DISPLAYLEN];
//static INT8U displayIndex;

static INT8U chatRxBuffer[UARTBUFFERSIZE], chatTxBuffer[UARTBUFFERSIZE];
static INT8U chatRxIndex, chatTxIndex;

/*	file system resources	*/
static FILETABLESTRUCT fileLookupTable[NUMFILES];	// Index of files
static FILETABLESTRUCT tmpFile;	//	RAM copy of current filetable
static FILESTRUCT tmpFileData;
static INT32U tmpFIndex;

static const INT8U learnString[] =
		"the quick brown fox jumped over the lazy brown dog. THE QUICK BROWN FOX JUMPED OVER THE LAZY BROWN DOG!";

static void MenuState(INT8U key, INT8U cntlFlag, INT8U error, INT16U rawKey);
static void NotesState(INT8U key, INT8U cntlFlag, INT8U error, INT16U rawKey);
static void ReadState(INT8U key, INT8U cntlFlag, INT8U error, INT16U rawKey);
static void LearnState(INT8U key, INT8U cntlFlag, INT8U error, INT16U rawKey);
static void ChatState(INT8U key, INT8U cntlFlag, INT8U error, INT16U rawKey);

/********************************************************************
 * TODO InitTmpVars() - Initialization routine for between states.
 ********************************************************************/
void InitTmpVars(void) {

	sliceCnt = 0;
	bMenuDisplayed = FALSE;
	//displayIndex = 0;
	chatRxIndex = 0;
	chatTxIndex = 0;
	tmpFile.state = fUNUSED;
	tmpFile.tableIndex = 0;
	tmpFIndex = 0;
}

/********************************************************************
 * TODO FindNextFile() - returns true if success, false if no files exist.
 * 					next = TRUE, look forward. next = FALSE, look back.
 ********************************************************************/
INT8U FindNextFile(FILETABLESTRUCT *current, INT8U next, FILESTATES fStates) {

	INT16S i;
	INT8U curindex = current->tableIndex;

	if (next) {
		//	curindex + 1 to end
		for (i = curindex + 1; i < NUMFILES; i++) {
			if (fileLookupTable[i].state & fStates) {
				*current = fileLookupTable[i];
				return (INT8U) TRUE;
			} else {
			}
		}
		//	beginning to curindex - 1
		for (i = 0; i < curindex + 1; i++) {
			if (fileLookupTable[i].state & fStates) {
				*current = fileLookupTable[i];
				return (INT8U) TRUE;
			} else {
			}
		}
		return (INT8U) FALSE;
	} else {
		//	curindex to beginning
		for (i = curindex; i >= 0; i--) {
			if (fileLookupTable[i].state & fStates) {
				*current = fileLookupTable[i];
				return (INT8U) TRUE;
			} else {
			}
		}
		//	end to curindex + 1
		for (i = NUMFILES; i > curindex; i--) {
			if (fileLookupTable[i].state & fStates) {
				*current = fileLookupTable[i];
				return (INT8U) TRUE;
			} else {
			}
		}
		return (INT8U) FALSE;
	}
}

/********************************************************************
 * TODO GetNextFile() - returns pointer to next unused file table or null
 * 					if they are all full.
 ********************************************************************/
FILETABLESTRUCT GetNextFile(INT8U *err) {

	INT8U i;
	for (i = 0; i < NUMFILES; i++) {
		if (fileLookupTable[i].state == fUNUSED) {
			*err = FALSE;
			return fileLookupTable[i];
		} else {
		}
	}
	*err = TRUE;
	return fileLookupTable[0];
}

/********************************************************************
 * TODO CntlInit() - Initialization routine for the control module.
 ********************************************************************/
void CntlInit(void) {
	/*	Initialize resources	*/
	progState = MENU;
	noteState = nSELECT;
	readState = rSELECT;
	sliceCnt = 0;
	bMenuDisplayed = FALSE;

	//displayIndex = 0;

	chatRxIndex = 0;
	chatTxIndex = 0;

	FileInit((FILETABLESTRUCT *) &fileLookupTable);
	tmpFile.state = fUNUSED;
	tmpFile.tableIndex = 0;
	tmpFIndex = 0;
}

/********************************************************************
 * CntlTask() - Implements program flow according to Menu structure.
 * (Public)
 ********************************************************************/
void CntlTask(void) {
	INT8U key, cntlFlag, error;
	INT16U rawKey;

	key = GetKey(&cntlFlag, &rawKey);

	//	DEBUG
	if (key) {
		UARTSend("Key: ", (INT32U) 5);
		UARTSend(&key, (INT32U) 1);
		UARTSend("\r\n", (INT32U) 2);
		(void) SPISendChar((INT8U) rawKey);
	} else {
	}
	//	-----

	switch (progState) {
	/*		Menu State			*/	//TODO Menu State
	default:
	case MENU:
		MenuState(key, cntlFlag, error, rawKey);
		break;
		/*		Note State			*/	//TODO Notes State
	case NOTES:
		NotesState(key, cntlFlag, error, rawKey);
		break;
		/*		Read State			*/	//TODO Read state
	case READ:
		ReadState(key, cntlFlag, error, rawKey);
		break;
		/*		Learn State			*/	//TODO Learn state
	case LEARN:
		LearnState(key, cntlFlag, error, rawKey);
		break;
		/*		Chat State			*/	//TODO Chat state
	case CHAT:
		ChatState(key, cntlFlag, error, rawKey);
		break;
	}
	sliceCnt++;
}

/********************************************************************
 * TODO MenuState() - function called during menu state.
 ********************************************************************/
void MenuState(INT8U key, INT8U cntlFlag, INT8U error, INT16U rawKey) {

	switch (key) {
	case 'n':
	case 'N':
		progState = NOTES;
		noteState = nSELECT;
		UARTSend("\r\nNotes", (INT32U) 7);
		DisplayUpdate("Notes");
		InitTmpVars();
		break;
	case 'r':
	case 'R':
		progState = READ;
		readState = rSELECT;
		UARTSend("\r\nRead", (INT32U) 6);
		DisplayUpdate("Read");
		InitTmpVars();
		break;
	case 'l':
	case 'L':
		progState = LEARN;
		UARTSend("\r\nLearn", (INT32U) 7);
		DisplayUpdate("Learn");
		InitTmpVars();
		break;
	case 'c':
	case 'C':
		progState = CHAT;
		UARTSend("\r\nChat", (INT32U) 6);
		DisplayUpdate("Chat");
		InitTmpVars();
		break;
	default:
		//	Display Menu Message
		/*	"N: Notes?
		 * 	 R: Read?
		 * 	 L: Learn?
		 * 	 C: Chat?	"
		 */
		if (bMenuDisplayed == FALSE) {
			switch (sliceCnt) {
			case 0:
				UARTSend("Main Menu\r\n", (INT32U) 11);
				DisplayUpdate("Menu");
				break;
			case ONESECOND:
				UARTSend("[N]otes     ", (INT32U) 12);
				DisplayUpdate("[N]otes");
				break;
			case TWOSECONDS:
				UARTSend("[R]ead     ", (INT32U) 11);
				DisplayUpdate("[R]ead");
				break;
			case THREESECONDS:
				UARTSend("[L]earn     ", (INT32U) 12);
				DisplayUpdate("[L]earn");
				break;
			case FOURSECONDS:
				UARTSend("[C]hat\r\n", (INT32U) 8);
				DisplayUpdate("[C]hat");
				bMenuDisplayed = TRUE;
				break;
			default:
				break;
			}
		} else {
			if (sliceCnt >= ONEMINUTE) {
				sliceCnt = 0;
				bMenuDisplayed = FALSE;
			} else {
			}
		}
		break;
	}
}

/********************************************************************
 * TODO NotesState() - function called during menu state.
 ********************************************************************/
void NotesState(INT8U key, INT8U cntlFlag, INT8U error, INT16U rawKey) {

	static INT8U waitFlag = FALSE;

	/*	Allow message time to settle	*/
	if ((sliceCnt >= ONESECOND)&& (waitFlag == FALSE)){
	waitFlag = TRUE;
	sliceCnt = 0;
} else {}

	if (waitFlag) {
		switch (noteState) {
		default:
		case nSELECT:
			switch (key) {
			case 'n':
			case 'N':
				noteState = nNEW;
				InitTmpVars();
				break;
			case 'o':
			case 'O':
				noteState = nOLD;
				InitTmpVars();
				break;
			default:
				if ((cntlFlag == TRUE) && (key == 'M')) {
					progState = MENU;
					InitTmpVars();
				} else {
					//	Display
					/*	"N: New?
					 *   O: Old?	"
					 */
					if (bMenuDisplayed == FALSE) {
						switch (sliceCnt) {
						case 1:
							UARTSend("\r\n[N]ew", (INT32U) 7);
							DisplayUpdate("[N]ew");
							break;
						case TWOSECONDS:
							UARTSend("\r\n[O]ld", (INT32U) 7);
							DisplayUpdate("[O]ld");
							bMenuDisplayed = TRUE;
							break;
						default:
							break;
						}
					} else {
						if (sliceCnt >= ONEMINUTE) {
							sliceCnt = 0;
							bMenuDisplayed = FALSE;
						} else {
						}
					}
				}
				break;
			}
			break;
		case nNEW:
			if (!((cntlFlag == TRUE) && (key == 'M'))) {
				//	get note file name (slice counting)
				// 	when LF received, noteState = NOTES
				//	Ignore keys until menu displayed for user
				if (bMenuDisplayed == FALSE) {
					UARTSend("\r\nName: ", (INT32U) 8);
					DisplayUpdate("Name: ");
					/*	Get current file pointer	*/
					tmpFile = GetNextFile(&error);
					if (error == TRUE) {
						/*	Error, memory full	*/
						progState = MENU;
						InitTmpVars();
					} else {
						tmpFile.state = fNOTES;
					}
					bMenuDisplayed = TRUE;
				} else {
					/*	Menu has been displayed, looking for keys now	*/
					/*	by state mutex, we can use for tmpFIndex as our
					 *  index for the name field in the filetable entry.
					 */
					switch (key) {
					default:
						if (key == 0x00) {
							break;
						} else {
							tmpFile.name[tmpFIndex++] = key;
							if (tmpFIndex < BUFFERLEN) {
								break;
							} else {
							}	//	fall through
						}
					case '\r':
					case '\n':
						//	got name, moving on into NOTES mode
						if (tmpFIndex != 0) {
							tmpFIndex = 0;
							noteState = nNOTES;
						} else {
						}
						break;
					}
				}
			} else {
				noteState = nSELECT;
				InitTmpVars();
			}
			break;
		case nOLD:
			if (cntlFlag == TRUE) {
				error = FALSE;
				switch (key) {
				case 'M':
					noteState = nSELECT;
					InitTmpVars();
					break;
				case '<':
					error = FindNextFile(&tmpFile, BACK, fNOTES);
					break;
				case '>':
					error = FindNextFile(&tmpFile, FORWARD, fNOTES);
					break;
				}
				if (error == TRUE) {
					//	no old notes go back to select
					UARTSend("\r\nNo Old Notes", (INT32U) 14);
					DisplayUpdate("None");
					noteState = nSELECT;
					InitTmpVars();
				} else {
					UARTSend(tmpFile.name, (INT32U) BUFFERLEN);
					DisplayUpdate(tmpFile.name);
				}
			} else if ((key == ' ') || (key == '\r') || (key == '\n')) {
				if (tmpFile.state != fUNUSED) {
					noteState = nNOTES;
				} else {
				}
			} else {
			}
			break;
		case nNOTES:
			if (!(cntlFlag == TRUE && (key == 'M'))
					&& (tmpFIndex < FILEBLOCKSIZE)) {
				if (!cntlFlag && key != 0x00) {	// key is valid and not Nav
					if (key == '\b') {
						tmpFileData[tmpFIndex--] = 0x00;
					} else {
						tmpFileData[tmpFIndex++] = key;
						DisplayAppendChar(key);
					}
				} else {
				}
			} else {
				//	Save this note
				fileLookupTable[tmpFile.tableIndex] = tmpFile;
				(void) FileUpdate(&fileLookupTable[tmpFile.tableIndex],
						(FILESTRUCT *) tmpFileData);
				progState = MENU;
				InitTmpVars();
			}
			break;
		}
	} else {
	}
}

/********************************************************************
 * TODO ReadState() - function called during menu state.
 ********************************************************************/
void ReadState(INT8U key, INT8U cntlFlag, INT8U error, INT16U rawKey) {

	static INT8U waitFlag = FALSE;

	/*	Allow message time to settle	*/
	if ((sliceCnt >= ONESECOND)&& (waitFlag == FALSE)){
	waitFlag = TRUE;
	sliceCnt = 0;
} else {}

	if (waitFlag) {
		switch (readState) {
		default:
		case rSELECT:
			if (cntlFlag == TRUE) {
				if (key == 'M') {
					progState = MENU;
					InitTmpVars();
				} else {
					if (key == '<') {	// can read any type of file.
						error = FindNextFile(&tmpFile, BACK,
								(FILESTATES) (fNOTES | fLEARN | fREAD));
					} else {
						error = FindNextFile(&tmpFile, FORWARD,
								(FILESTATES) (fNOTES | fLEARN | fREAD));
					}
					if (error == TRUE) {
						//	no old notes go back to select
						UARTSend("\r\nNo Old Docs", (INT32U) 14);
						DisplayUpdate("No Docs");
						readState = rSELECT;
						InitTmpVars();
						break;
					} else {

					}
				}
			} else {
				switch (key) {
				case '\r':
				case '\n':
				case ' ':
					if (tmpFile.state != fUNUSED) {
						readState = rREAD;
					} else {
					}
					break;
				default:
					UARTSend(tmpFile.name, (INT32U) BUFFERLEN);
					DisplayUpdate(tmpFile.name);
				}
			}
			break;
		case rREAD:
			if ((cntlFlag == FALSE) && (key != 'M')) {
				if ((cntlFlag == TRUE) && (key == '<')) {
					if (tmpFIndex < DISPLAYLEN) {
						/*	ignore	*/
					} else {
						tmpFIndex -= DISPLAYLEN;
					}
				} else if ((cntlFlag == TRUE) && (key == '>')) {
					if (tmpFIndex > FILEBLOCKSIZE - DISPLAYLEN) {
						/*	ignore	*/
					} else {
						tmpFIndex += DISPLAYLEN;
					}
				}
				DisplayUpdate((INT8U *) (tmpFile.ptr + tmpFIndex));
			} else {
				readState = rSELECT;
				InitTmpVars();
			}
			break;
		}
	} else {
	}
}

/********************************************************************
 * TODO LearnState() - function called during menu state.
 ********************************************************************/
void LearnState(INT8U key, INT8U cntlFlag, INT8U error, INT16U rawKey) {

	static INT8U waitFlag = FALSE;
	static INT8U fMissed = FALSE;

	/*	Allow message time to settle	*/
	if ((sliceCnt >= ONESECOND)&& (waitFlag == FALSE)){
	waitFlag = TRUE;
	sliceCnt = 0;
} else {}

	if (waitFlag) {
		if (!((cntlFlag == TRUE) && (key == 'M'))) {
			if (key != learnString[tmpFIndex]) {
				if (sliceCnt & 0xA00) {
					if (fMissed == TRUE) {
						fMissed = FALSE;
						UARTSend("\r ", (INT32U) 2);
						UARTSend(learnString + tmpFIndex + 1,
								(INT32U) DISPLAYLEN - 1);
						//DisplayBlinkMiss();
						DisplayUpdate(" ");
						DisplayUpdate((INT8U *)(learnString + tmpFIndex + 1));
					} else {
						fMissed = TRUE;
						UARTSend("\r", (INT32U) 1);
						UARTSend(learnString + tmpFIndex, (INT32U) DISPLAYLEN);
						DisplayUpdate((INT8U *) (learnString + tmpFIndex));
					}
				} else {
				}
			} else {
				fMissed = FALSE;
				tmpFIndex++;
				if (learnString[tmpFIndex] == 0x00) {
					progState = MENU;
					InitTmpVars();
				} else {
					//	Shifted display to show next char as first
					UARTSend("\r", (INT32U) 1);
					UARTSend(learnString + tmpFIndex, (INT32U) DISPLAYLEN);
					DisplayUpdate((INT8U *) (learnString + tmpFIndex));
				}
			}
		} else {
			progState = MENU;
			InitTmpVars();
		}
	} else {
	}
}

/********************************************************************
 * TODO ChatState() - function called during menu state.
 ********************************************************************/
void ChatState(INT8U key, INT8U cntlFlag, INT8U error, INT16U rawKey) {

	if ((cntlFlag == FALSE) && (key != 'M')) {
		/*	Get Chat message	*/
		UARTGetBuffer(chatRxBuffer);
		if (chatRxBuffer[chatRxIndex] != 0x00) {
			//	New Message
			chatRxIndex = 0;
			DisplayUpdate(chatRxBuffer);
		} else {
		}

		/*	Send user key	*/
		if (!cntlFlag || key == ' ') {
			//UARTSendChar(key);
			switch (key) {
			case '\r':
			case '\n':
				UARTSend(chatTxBuffer, (INT32U) chatTxIndex);
				chatTxIndex = 0;
				break;
			default:
				chatTxBuffer[chatTxIndex++] = key;
				if (chatTxIndex == UARTBUFFERSIZE) {
					UARTSend(chatTxBuffer, (INT32U) chatTxIndex);
					chatTxIndex = 0;
				} else {
				}
			}
		} else {
			switch (key) {
			case '<':
				if (chatRxIndex > DISPLAYLEN) {
					chatRxIndex -= DISPLAYLEN;
				} else {
				}
				break;
			case '>':
				if (chatRxIndex < UARTBUFFERSIZE - DISPLAYLEN) {
					chatRxIndex += DISPLAYLEN;
				} else {
				}
			}
		}
	} else {
		progState = MENU;
		InitTmpVars();
	}
}
/********************************************************************/
