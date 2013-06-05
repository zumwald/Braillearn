/*
 * FileIO.c
 *
 *  Created on: May 27, 2013
 *      Author: Dan Zumwalt
 ********************************************************************
* Project master header file
********************************************************************/
#include "includes.h"

/********************************************************************
* Module Defines
********************************************************************/

/********************************************************************
* Public Resources
********************************************************************/

/********************************************************************
* Private Resources
********************************************************************/
INT8U emptyName[] = "               ";

/********************************************************************
* FileInit() - FileIO Initialization. Must run before any fileIO
* 		- 	PUBLIC
********************************************************************/
void FileInit(FILETABLESTRUCT *fileTblStruct){

	INT8U i,j;

	for (i = 0; i < NUMFILES; i++){
		fileTblStruct[i].tableIndex = i;
		for (j = 0; j< BUFFERLEN; j++){
			fileTblStruct[i].name[j] = ' ';
		}
		fileTblStruct[i].state = fUNUSED;
		fileTblStruct[i].ptr = (FILESTRUCT *)(FILEBASEADDR + i*0x0400);
	}
}

/********************************************************************
* FileUpdate() - writes current file data to FLASH, returns true if
* 				 successful false if otherwise.
* 		-	PUBLIC
********************************************************************/
INT8U FileUpdate(FILETABLESTRUCT *fileTblStruct, FILESTRUCT *data){

		INT32S 	fpRet = 1;
		INT8U 	retval;

		FlashErase((INT32U)fileTblStruct->ptr);
		fpRet = FlashProgram((INT32U *)data, (INT32U)fileTblStruct->ptr, FILEBLOCKSIZE);

		switch (fpRet){
		case 0:
			//	Success!
			retval = TRUE;
			break;
		case -1:
		default:
			retval = FALSE;
			break;
		}
		return retval;
}
/*******************************************************************/
