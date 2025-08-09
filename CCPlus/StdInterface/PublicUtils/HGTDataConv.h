/***************************************************************************
 *  HGTDataConv.h
 *
 *	Health Gene Technologies, All Rights Reserved
 *
 *  Created on  : Setp. 06, 2021 11:17:05 AM
 *  Author      : Justin Xia
 *
 *  Version Info: 1.0
 *
 ***************************************************************************/

#ifndef GTDATACONV_H_
#define GTDATACONV_H_


/* byte order type definition */
typedef enum
{
	BYTE_ENDIAN_BIG,
	BYTE_ENDIAN_LITTLE
} BYTE_ENDIAN_t;


/* transfer Motorola HEX bytes to unsigned long */
unsigned long hex_to_long(unsigned char *ucData, unsigned int nLen);

/* transfer Motorola HEX bytes to string */
DLL_EXPORT char *hex_to_string(unsigned char *ucData, unsigned int nLen, char *str);

/* transfer BCD bytes to integer */
DLL_EXPORT int bcd_to_int(unsigned char *ucData, unsigned int nLen);

/* get time stamp asc - 14*/
void get_timestamp(char *ts);

/* bcd-yyyymmddhhmmss - 07*/
DLL_EXPORT void get_bcd_timestamp(char *ts);

unsigned long get_tick_count();

/*
	1.
	ucHex = { 0x00, 0x1A, 0x2B, 0x30 }
	nHexLen = 4; Big Endian
	==>
	nBCDLen = 6;  // Right Align
	ucBCD = {0x00, 0x00, 0x01, 0x71, 0x49, 0x92}

	2.
	ucHex = { 0x00, 0x1A, 0x2B, 0x30 }
	nHexLen = 4; Little Endian
	==>
	nBCDLen = 6;  // Right Align
	ucBCD = {0x00, 0x08, 0x08, 0x13, 0x10, 0x72}
*/
DLL_EXPORT void hex_to_bcd(unsigned char *ucHex, unsigned int nHexLen,
				unsigned char *ucBCD, unsigned int nBCDLen, BYTE_ENDIAN_t endianType);

#endif /* GTDATACONV_H_ */

