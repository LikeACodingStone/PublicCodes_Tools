/***************************************************************************
 *  HGTDataConv.cpp
 *
 *	Health Gene Technologies, All Rights Reserved
 *
 *  Created on  : Sept. 06, 2021 11:17:05 AM
 *  Author      : Justin Xia
 *
 *  Version Info: 1.0
 *
 ***************************************************************************/

#include "HGTDataConv.h"
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/timeb.h>

unsigned long hex_to_long(unsigned char *ucData, unsigned int nLen)
{
	unsigned long value = 0;

	for (unsigned int i = 0; i < nLen; i++)
	{
		value += ucData[i];
		if (i != nLen - 1)
			value *= 0x100;
	}

	return value;
}


char *hex_to_string(unsigned char *ucData, unsigned int nLen, char *str)
{
	for (unsigned int i = 0; i < nLen; i++)
	{
		sprintf(&str[i * 2], "%02X", ucData[i]);
	}
	str[2 * nLen] = '\0';
	return str;
}

int bcd_to_int(unsigned char *ucData, unsigned int nLen)
{
	unsigned int r = 0;
	unsigned int i = nLen;
	char ucBCD[4];

	for (i = 0; i < nLen; i++)
	{
		//check BCD format
		if (((ucData[i] & 0x0f) > 0x09) || ((ucData[i] & 0xF0) > 0x90))
			return -1;

		r *= 100;
		sprintf(&ucBCD[0], "%2x", ucData[i]);
		r += atoi(ucBCD);
	}

	return r;
}

void hex_to_bcd(unsigned char *ucHex, unsigned int nHexLen,
				unsigned char *ucBCD, unsigned int nBCDLen, BYTE_ENDIAN_t endianType)
{
	unsigned long value = 0;
	unsigned char ucDecValue, ucHigh, ucLow;
	if (endianType == BYTE_ENDIAN_BIG) //for PowerPC and Motorola
	{
		for (unsigned int i = 0; i < nHexLen; i++)
		{
			value += ucHex[i];
			if (i != nHexLen - 1)
				value *= 0x100;
		}
	} else { //Little Endian for Intel
		for (int i = nHexLen - 1; i >= 0; i--)
		{
			value += ucHex[i];
			if (i != 0)
				value *= 0x100;
		}
	}

	memset(ucBCD, 0x00, nBCDLen);
	for (int i = nBCDLen - 1; i >= 0; i--)
	{
		ucDecValue = (unsigned char)(value % 100);
		ucHigh = (ucDecValue / 10);
		ucLow = (ucDecValue % 10);
		ucBCD[i] = ucHigh * 0x10 + ucLow;

		value = value / 100;
		if (!value)
			break;
	}
}

void get_bcd_timestamp(char *ts)
{
	char szTime[16];
	memset(szTime, 0x00, 16);
	time_t ti;
	time(&ti);
	struct tm *pLocalTime = localtime(&ti);
	memset(ts, 0x00, 7);
	sprintf(szTime, "%04d%02d%02d%02d%02d%02d",
		pLocalTime->tm_year + 1900,pLocalTime->tm_mon + 1, pLocalTime->tm_mday,
		pLocalTime->tm_hour, pLocalTime->tm_min, pLocalTime->tm_sec);

	for (int i = 0; i < 7; i++)
	{
		ts[i] = (szTime[2 * i] - '0') * 0x10 + (szTime[2 * i + 1] - '0');
	}
}

void get_timestamp(char *ts)
{
	time_t ti;
	time(&ti);
	struct tm *pLocalTime = localtime(&ti);
	memset(ts, 0x00, 15);
	sprintf(ts, "%04d%02d%02d%02d%02d%02d",
		pLocalTime->tm_year + 1900,pLocalTime->tm_mon + 1, pLocalTime->tm_mday,
		pLocalTime->tm_hour, pLocalTime->tm_min, pLocalTime->tm_sec);
}

unsigned long get_tick_count()
{
	struct timeb tp;
	ftime(&tp);

	return (unsigned long)tp.time;
}

