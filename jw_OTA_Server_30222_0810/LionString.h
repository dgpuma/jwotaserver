#pragma once

#ifndef _LIONSTRING_H_
#define _LIONSTRING_H_

char* U2G(const char* utf8);
char* G2U(const char* gb2312);
TCHAR* U2T(const char* utf8);
char* wchar2char(const wchar_t* wchar);
wchar_t* char2wchar(const char* cchar);

char* wtoc(wchar_t* wText, char* psText);
wchar_t* ctow(char* sText);

unsigned char H2D(unsigned char Hex);
void LoadFile(CString sFileName, unsigned char* buf, int& iLen);
int StrToHex(CString str, LPBYTE buff);
int StrToHexA(CString str, LPBYTE buff);
void SplitCString(const CString& _cstr, const CString& _flag, CStringArray& _resultArray);
int strSplit(CString str, CString Seperator, CStringArray& splitstr);
int charSplit(char* pstr, char* seps, CStringArray& p_strA);

#endif
