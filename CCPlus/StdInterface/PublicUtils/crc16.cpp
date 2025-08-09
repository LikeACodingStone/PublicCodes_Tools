
/***************************************************************************
 *  crc16.cpp
 *
 *	Health Gene Technologies, All Rights Reserved
 *
 *  Created on  : 2021-09-07 ионГ10:02:14
 *  Author      : Justin.Xia
 *
 *  Version Info: 1.0
 *
 ***************************************************************************/

#include "crc16.h"

unsigned int calc_crc16(unsigned char *ucData, unsigned int nLen, unsigned int uiInit)
{
	unsigned int i, nCRC16;
	unsigned char j;

	nCRC16 = uiInit;
	for (i = 0; i < nLen; i++)
	{
		nCRC16 ^= ucData[i];
		for (j = 0; j < 8; j ++)
		{
			if(nCRC16 & 0x0001)
			{
				nCRC16 >>= 1;
				nCRC16 ^= CRC16_POLYNOMIAL;
			} else {
				nCRC16 >>= 1;
			}
		}
	}
	return nCRC16;
}
 
