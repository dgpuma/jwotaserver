
#pragma once
#ifndef __HEX_H_
#define __HEX_H_

extern int  flashsize;                 //flash�Ĵ洢��Ԫ����

extern unsigned char buffer[256];                   //�洢hex�ļ���һ������
extern unsigned char trsbuffer[200];                //�洢hex�ļ������������
extern unsigned char flashbuffer[0x8000];              //�洢Flash�е�����
extern unsigned char flashbufferT[0x8000];              //�洢Flash�е�����

//void TranHex(char* flashbuffer, char* formatcmd, int flashsize);
void ShowFlash(unsigned char* flashbuffer, int flashsize);
void LoadFile(CString fileName);
void SaveFile(CString fileName,long LAddrStart, long LAddrStop);
void TransFlash(unsigned char* buf, unsigned char* bufT, int iLen);
#endif
