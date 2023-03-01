#pragma once
#ifndef _CRC_H
#define _CRC_H

typedef union
{
	unsigned char uc[2];
	unsigned short ui;
} u16_type;


unsigned short Crc16Check(unsigned char* puchMsg, unsigned short usDataLen);
unsigned short Crc16CheckRealTime(unsigned short vByte);
unsigned short Crc16(unsigned short puchMsgp, unsigned short crc_init);
unsigned short calcks(unsigned char* sBuf, unsigned short iLen);
unsigned char calxor(unsigned char* sBuf, unsigned short iLen);
#endif
