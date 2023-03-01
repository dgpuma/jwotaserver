
#include "LionString.h"
#include "pch.h"
#include <fstream>
//#include "stdlib.h"
//#include "stdio.h"
#include "string.h"
//#include "io.h" 
#include<iostream>
#include <cstring>
#include "resource.h"
//#include "atlconv.h"

#define EXAMPLE_MACRO_NAME
#define _CRT_SECURE_NO_WARNINGS

using namespace std;

char* CString2Char(CString cstr)
{
	//USES_CONVERSION
	//char* p = W2A(cstr.GetBuffer(0));
	//cstr.ReleaseBuffer();
	//return p;

	//wchar_t* wCharString = cstr.GetBuffer(cstr.GetLength() + 1);
	//size_t origsize = wcslen(wCharString) + 1;
	//size_t convertedChars = 0;
	//char* CharString;
	//CharString = new char(origsize);
	//wcstombs_s(&convertedChars, CharString, origsize, wCharString, _TRUNCATE);
	//return CharString;

	int n = cstr.GetLength();
	int len = WideCharToMultiByte(CP_ACP, 0, cstr, n, NULL, 0, NULL, NULL);
	char* pChar = new char[len + 1];
	WideCharToMultiByte(CP_ACP, 0, cstr, n, pChar, len, NULL, NULL);
	pChar[len] = '\0';
	return pChar;
}

char* wchar2char(const wchar_t* wchar)
{
	char* m_char;
	int len = WideCharToMultiByte(CP_ACP, 0, wchar, wcslen(wchar), NULL, 0, NULL, NULL);
	m_char = new char[len + 1];
	WideCharToMultiByte(CP_ACP, 0, wchar, wcslen(wchar), m_char, len, NULL, NULL);
	m_char[len] = '\0';
	return m_char;
}


wchar_t* char2wchar(const char* cchar)
{
	wchar_t* m_wchar;
	int len = MultiByteToWideChar(CP_ACP, 0, cchar, strlen(cchar), NULL, 0);
	m_wchar = new wchar_t[len + 1];
	MultiByteToWideChar(CP_ACP, 0, cchar, strlen(cchar), m_wchar, len);
	m_wchar[len] = '\0';
	return m_wchar;
}



//<pre class = "cpp" name = "code">//将ASCII码转换为UTF8编码
void ASCIIToUTF8(char cACSII[], char cUTF8[])
{
#ifndef SQL_MAX_LENTH
#define SQL_MAX_LENTH 1024
#endif
	//先将ASCII码转换为Unicode编码
	int nlen = MultiByteToWideChar(CP_ACP, 0, cACSII, -1, NULL, NULL);
	wchar_t* pUnicode = new wchar_t[SQL_MAX_LENTH];
	memset(pUnicode, 0, nlen * sizeof(wchar_t));
	MultiByteToWideChar(CP_ACP, 0, cACSII, -1, (LPWSTR)pUnicode, nlen);
	wstring wsUnicode = pUnicode;
	//将Unicode编码转换为UTF-8编码
	nlen = WideCharToMultiByte(CP_UTF8, 0, wsUnicode.c_str(), -1, NULL, 0, NULL, NULL);
	WideCharToMultiByte(CP_UTF8, 0, wsUnicode.c_str(), -1, cUTF8, nlen, NULL, NULL);
}

void UTF8ToASCII(char* cUTF8, char ASCII[])
{
	string str = cUTF8;
	//先将UTF8编码转换为Unicode编码
	int nLen = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);
	wchar_t* pwcUnicode = new wchar_t[nLen];
	memset(pwcUnicode, 0, nLen * sizeof(wchar_t));
	MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, (LPWSTR)pwcUnicode, nLen);
	//将Unicode编码转换为ASCII编码
	nLen = WideCharToMultiByte(CP_ACP, 0, pwcUnicode, -1, NULL, 0, NULL, NULL);
	WideCharToMultiByte(CP_ACP, 0, pwcUnicode, -1, ASCII, nLen, NULL, NULL);
}


void ASCIIToUnicode(char cASCII[], wchar_t wcUnicode[])
{
	int nlen = MultiByteToWideChar(CP_ACP, 0, cASCII, -1, NULL, NULL);
	MultiByteToWideChar(CP_ACP, 0, cASCII, -1, (LPWSTR)wcUnicode, nlen);
}

//将Unicode编码转换为UTF-8编码
void UnicodeToUTF8(wchar_t wcUnicode[], char cUTF8[])
{
	wstring wsUnicode = wcUnicode;
	int nLen = WideCharToMultiByte(CP_UTF8, 0, wsUnicode.c_str(), -1, NULL, 0, NULL, NULL);
	WideCharToMultiByte(CP_UTF8, 0, wsUnicode.c_str(), -1, cUTF8, nLen, NULL, NULL);
}



//将UTF-8编码转换为Unicode编码
void UTF8ToUnicode(char* cUTF8, wchar_t wcUnicode[])
{
	string sUTF8 = cUTF8;
	int nLen = MultiByteToWideChar(CP_UTF8, 0, sUTF8.c_str(), -1, NULL, 0);
	MultiByteToWideChar(CP_UTF8, 0, sUTF8.c_str(), -1, (LPWSTR)wcUnicode, nLen);
}



//将Unicode编码转换为ASCII编码
void UnicodeToUTF8A(wchar_t wcUnicode[], char cASCII[])
{
	int nLen = WideCharToMultiByte(CP_ACP, 0, wcUnicode, -1, NULL, 0, NULL, NULL);
	WideCharToMultiByte(CP_ACP, 0, wcUnicode, -1, cASCII, nLen, NULL, NULL);
}

//UTF-8到GB2312的转换
char* U2G(const char* utf8)
{
	int len = MultiByteToWideChar(CP_UTF8, 0, utf8, -1, NULL, 0);
	wchar_t* wstr = new wchar_t[len + 1];
	memset(wstr, 0, len + 1);
	MultiByteToWideChar(CP_UTF8, 0, utf8, -1, wstr, len);
	len = WideCharToMultiByte(CP_ACP, 0, wstr, -1, NULL, 0, NULL, NULL);
	char* str = new char[len + 1];
	memset(str, 0, len + 1);
	WideCharToMultiByte(CP_ACP, 0, wstr, -1, str, len, NULL, NULL);
	if (wstr) delete[] wstr;
	return str;
}

//GB2312到UTF-8的转换
char* G2U(const char* gb2312)
{
	int len = MultiByteToWideChar(CP_ACP, 0, gb2312, -1, NULL, 0);
	wchar_t* wstr = new wchar_t[len + 1];
	memset(wstr, 0, len + 1);
	MultiByteToWideChar(CP_ACP, 0, gb2312, -1, wstr, len);
	len = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
	char* str = new char[len + 1];
	memset(str, 0, len + 1);
	WideCharToMultiByte(CP_UTF8, 0, wstr, -1, str, len, NULL, NULL);
	if (wstr) delete[] wstr;
	return str;
}

TCHAR* U2T(const char* utf8)
{
	int len = MultiByteToWideChar(CP_UTF8, 0, utf8, -1, NULL, 0);
	wchar_t* wstr = new wchar_t[len + 1];
	memset(wstr, 0, len + 1);
	MultiByteToWideChar(CP_UTF8, 0, utf8, -1, wstr, len);
	len = WideCharToMultiByte(CP_ACP, 0, wstr, -1, NULL, 0, NULL, NULL);

	return wstr;
}



void SplitCString(const CString& _cstr, const CString& _flag, CStringArray& _resultArray)
{
	CString strSrc(_cstr);

	CStringArray& strResult = _resultArray;
	CString strLeft = _T("");

	int nPos = strSrc.Find(_flag);
	while (0 <= nPos)
	{
		strLeft = strSrc.Left(nPos);
		if (!strLeft.IsEmpty())
		{
			strResult.Add(strLeft);
		}
		strSrc = strSrc.Right(strSrc.GetLength() - nPos - 1);
		nPos = strSrc.Find(_flag);
	}

	if (!strSrc.IsEmpty()) {
		strResult.Add(strSrc);
	}
}

int StrToHex(CString str, LPBYTE buff)
{
	//1 CString m_str;
	//2 m_str = "1F"; //16进制字符为1F
	//3 m_str = "0x" + m_str;
	//4 char *p = (char*)((LPCTSTR)m_str); //将CString的字符m_str转化成char*型，并赋值给指针p
	//5 char *str;
	//6 int m_Speed = (int)strtol(p, &str, 16); //十六进制转化成10进制，并赋值给整形数据m_Speed

	CStringArray _resultArray;
	//CString _flag = L" ";
	CString strA;
	int iLen = 0;
	CString str1 = L"123";
	str1 = str;
	wchar_t* t1 = str1.GetBuffer(str1.GetLength());
	str1.ReleaseBuffer();

	//SplitCString(L"12 34 56   78 9", L" ", _resultArray);
	SplitCString(str, L" ", _resultArray);
	iLen = _resultArray.GetCount();
	strA.Format(L"%d", iLen);
	::MessageBox(NULL, strA, NULL, 0);
	for (int i = 0; i < iLen; i++)
	{
		::MessageBox(NULL, _resultArray[i], NULL, 0);
		CString strT0 = L"0x" + _resultArray[i];
		LPWSTR s = (strT0.GetBuffer());
	}
	return iLen;
}

int StrToHexA(CString str, LPBYTE buff)
{
	int len = 0;
	int index = 0;
	bool bAct = false;
	char buf[2048];
	str.Trim();
	str.MakeUpper();
	len = WideCharToMultiByte(CP_ACP, 0, str, str.GetLength(), NULL, 0, NULL, NULL);
	WideCharToMultiByte(CP_ACP, 0, str, str.GetLength(), (char*)buf, len, NULL, NULL);
	for (int i = 0; i < len; i++)
	{
		if (buf[i] >= '0' && buf[i] <= '9')
		{
			bAct = true;
			buff[index] <<= 4;
			buff[index] += buf[i] - '0';
		}
		else if (buf[i] >= 'A' && buf[i] <= 'F')
		{
			bAct = true;
			buff[index] <<= 4;
			buff[index] += buf[i] - 'A' + 10;
		}
		else
		{
			if (bAct) index++;
			bAct = false;
		}
	}
	if (bAct) index++;
	return index;
}

int charSplit(char *pstr, char* seps, CStringArray& p_strA)
{
	//char string[] = "A string\tof ,,tokens\nand some  more tokens";
	//char seps[] = " ,\t\n";
	char* token;
	int count = 0;
	token = strtok(pstr, seps); // C4996
   // Note: strtok is deprecated; consider using strtok_s instead
	while (token != NULL)
	{
		// While there are tokens in "string"
		printf(" %s\n", token);
		p_strA.Add(CString(token));
		count++;
		// Get next token:
		token = strtok(NULL, seps); // C4996
	}
	return count;
}

int strSplit(CString str, CString Seperator, CStringArray& splitstr)
{
	//str.MakeUpper();
	splitstr.RemoveAll();
	int count = 0;
	//CString Seperator = _T(" ");
	int Position = 0;
	CString token;
	for (int i = 0;; i++)
	{
		token = str.Tokenize(Seperator, Position);
		if (token.IsEmpty())
		{
			count = i;
			break;
		}
		splitstr.Add(token);
	}
	return count;
}