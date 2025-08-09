
/***************************************************************************
 *  crc16.h
 *
 *	Health Gene Technologies, All Rights Reserved
 *
 *  Created on  : 2021-09-07 ÉÏÎç10:02:12
 *  Author      : Justin.Xia
 *
 *  Version Info: 1.0
 *
 ***************************************************************************/
#ifndef __CRC16_H
#define __CRC16_H

#define CRC16_POLYNOMIAL 0xA001	// CCITT CRC16 POLYNOMIAL

/**** Routine Guide ********************************************************
 Description: Calculate CCITT CRC16 with specific intial value.
             
 Parameters : ucData
                The data buffer to calculate CRC16.
              nLen
                The data length of buffer.
			: uiInit
				The data of the initial value to calculate the crc.
 Returns    : CRC16 result.
****************************************************************************/
DLL_EXPORT unsigned int calc_crc16(unsigned char *ucData, unsigned int nLen, unsigned int uiInit = 0);


#endif /* __CRC16_H */

