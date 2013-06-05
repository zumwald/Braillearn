/*
 * SPI.h
 *
 *  Created on: May 22, 2013
 *      Author: zumwald
 */

#ifndef SPI_H_
#define SPI_H_

/********************************************************************
* Public Resources
********************************************************************/

/********************************************************************
* SPIInit() - SPI Initialization. Must run before calling SPI funcs
********************************************************************/
extern void SPIInit(void);

/********************************************************************
* SPISendChar() - Sends char of data to specified address, returns
* 				  TRUE if ack is received from follower, otherwise FALSE.
********************************************************************/
extern INT8U SPISendChar(INT8U data);

/********************************************************************
* SPISendLine() - Sends 8 characters to display, returns TRUE if ack
* 				  is received from follower, otherwise FALSE.
********************************************************************/
extern INT8U SPISendLine(INT8U *data,INT8U len);


#endif /* SPI_H_ */
