
#pragma once
#ifndef __HEX_H_
#define __HEX_H_

extern int  flashsize;                 //flash的存储单元个数

extern unsigned char buffer[256];                   //存储hex文件的一行内容
extern unsigned char trsbuffer[200];                //存储hex文件解析后的内容
extern unsigned char flashbuffer[0x8000];              //存储Flash中的内容
extern unsigned char flashbufferT[0x8000];              //存储Flash中的内容

//void TranHex(char* flashbuffer, char* formatcmd, int flashsize);
void ShowFlash(unsigned char* flashbuffer, int flashsize);
void LoadFile(CString fileName);
void SaveFile(CString fileName,long LAddrStart, long LAddrStop);
void TransFlash(unsigned char* buf, unsigned char* bufT, int iLen);
#endif
