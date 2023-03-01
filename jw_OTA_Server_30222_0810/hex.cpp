#include "pch.h"
#include "hex.h"
#include <stdio.h>
#include <string.h>
//#include <stdlib.h>
#include "jw_OTADlg.h"


#define FLASHVOLUM 32


int  flashsize = 0x8000;                     //flash的存储单元个数
unsigned char buffer[256];                   //存储hex文件的一行内容
unsigned char trsbuffer[200];                //存储hex文件解析后的内容
unsigned char flashbuffer[0x8000];           //存储Flash中的内容
unsigned char flashbufferT[0x8000];           //存储Flash中的内容
long cks = 0;
//int  flashsize = 2 * FLASHVOLUM * 1024 * sizeof(char);      //flash的存储单元个数
//char* buffer = malloc(sizeof(char) * 100);                  //存储hex文件的一行内容
//char* trsbuffer = malloc(sizeof(char) * 200);               //存储hex文件解析后的内容
//char* flashbuffer = malloc(flashsize);                      //存储Flash中的内容

void TranHexToBuffer(unsigned char* flashbuffer, CString strText);

void Trans(void)
{
    long Len = 0;
    unsigned char* pBuf = flashbuffer;
    while (Len < 0x8000)
    {
        *pBuf = (*pBuf) ^ (Len & 0xFF);
        pBuf++;
        Len++;
    }
}

void LoadFile(CString fileName)
{
    //flashsize = FLASHVOLUM * 1024 * sizeof(char);      //flash的存储单元个数
    //buffer = (char*)malloc(sizeof(char) * 100);                  //存储hex文件的一行内容
    //trsbuffer = (char*)malloc(sizeof(char) * 200);               //存储hex文件解析后的内容
    //flashbuffer = (char*)malloc(flashsize);                      //存储Flash中的内容
    memset(flashbuffer, 0xFF, flashsize);
    flashsize = 0;
    cks = 0;
    CString str;
    //str.Format(L"%d", 124);
    str = fileName.Right(3).MakeUpper();
   
    if (str.Compare(L"HEX")==0)
    {
        CStdioFile file;
        //打开文件
        if (!file.Open(fileName, CFile::modeRead))
        {
            ::AfxMessageBox(_T("文件打开失败。"));
            return;
        }
        //读文件
        CString strText = _T("");

        while (file.ReadString(strText))
        {
            TranHexToBuffer(flashbuffer, strText);
        }

        ShowFlash(flashbuffer, 32 * 1024);
        //关闭文件
        file.Close();
    }
    else if (str.Compare(L"BIN")==0)
    {
        CFile file;
        file.Open(fileName, CFile::modeRead| CFile::typeBinary);
        int Len = file.GetLength();
        file.Read(flashbuffer, Len);
        ShowFlash(flashbuffer, 32 * 1024);
        //str.Format(L"%d", file.GetLength());
       // AfxMessageBox(str);
        file.Close();
    }
}

void SaveFile(CString fileName, long LAddrStart, long LAddrStop)
{
    //Trans();

    CFile file;
    file.Open(fileName, CFile::modeWrite| CFile::modeCreate | CFile::typeBinary);
    int Len = LAddrStop - LAddrStart + 1;
    //file.Read(flashbuffer, Len);
    file.Write(flashbufferT+ LAddrStart, Len);
    //ShowFlash(flashbuffer, 32 * 1024);
    //str.Format(L"%d", file.GetLength());
   // AfxMessageBox(str);
    file.Close();
}

void TransFlash(unsigned char* buf, unsigned char* bufT, int iLen)
{
    unsigned char index = 0;
    unsigned char* buf_s = buf;
    unsigned char* buf_t = bufT;
    
    for (int i = 0; i < iLen; i++)
    {
        index = i % 0x100;
        //bufT[i + 0xFF - index - index] = buf[i] ^ index;
        bufT[i + 0xFF - index - index] = (unsigned char)(buf[i] ^ index);
    }
}

void TranHexToBuffer(unsigned char* flashbuffer, CString formatcmd)
{
    int startadd;                                           //formatcmd指示的起始地址
    int count;

    CString eofstring = L":00000001FF";
    if (formatcmd == eofstring) {//结束标志
        printf("End of file.\n");
        //AfxMessageBox(L"End of file.\n");
        return;
    }
    if (formatcmd.GetAt(0) != ':') {                              //不是以":"开始
        printf("Format wrong!\n");
        ::AfxMessageBox(L"Format wrong!\n");
        return;
    }
    if (formatcmd.GetLength() < 11) {                           //字符串长度小于11
        printf("Format wrong!\n");

        ::AfxMessageBox(L"Format wrong!\n");
        return;
    }

    count = wcstol(formatcmd.Mid(1, 2), NULL, 16);              //获得数据单元长度
    startadd = wcstol(formatcmd.Mid(3, 4), NULL, 16);           //获得数据单元长度
    if (flashsize < startadd + count)
        flashsize = startadd + count;
    if (formatcmd.GetLength() < 11 + count * 2) {               //字符串中数据损坏（丢失）
        printf("File has been damaged!\n");
        ::AfxMessageBox(L"File has been damaged!\n");
        return;
    }
    unsigned char ch = 0;
    for (int p = 0; p < count; p++) {
        ch = (unsigned char)wcstol(formatcmd.Mid(9 + 2 * p, 2), NULL, 16);
        flashbuffer[startadd + p] = ch;
    }
}


void TranHex(char* flashbuffer, char* formatcmd, int size) {
    //按formatcmd字符串描述，以hex文件格式修改flashbuffer中的存储单元
    //如 formatcmd=":03000000020030CB"
    //则将flahsbuffer中以"0000"开始的连续"03"个内存单元，内容为"020030"
    //flashsize:表示flash的存储单元个数
    char* eofstring = ":00000001FF";
    int startadd;                                           //formatcmd指示的起始地址
    int count;                                              //formatcmd指示的数据单元个数
    int p, q;

    CString str;//索引指针
    if (strcmp(formatcmd, eofstring) == 0) {//结束标志
        printf("End of file.\n");
        AfxMessageBox(L"End of file.\n");
        return;
    }
    if (formatcmd[0] != ':') {                              //不是以":"开始
        printf("Format wrong!\n");
        ::AfxMessageBox(L"Format wrong!\n");
        return;
    }
    if (size < 11) {                           //字符串长度小于11
        printf("Format wrong!\n");
        str.Format(L"Format wrong! len=%d.\n %s", size, formatcmd);

        ::AfxMessageBox(str);
        return;
    }
    count = 0;// HexToDec((const char*)ExtraSubStr(formatcmd, 1, 2));         //获得数据单元长度
    startadd = 0;// HexToDec((const char*)ExtraSubStr(formatcmd, 3, 4));  //获得起始地址
    if (size < 11 + count * 2) {               //字符串中数据损坏（丢失）
        printf("File has been damaged!\n");
        ::AfxMessageBox(L"File has been damaged!\n");
        return;
    }
    for (p = 0; p < count * 2; p++) {
        flashbuffer[startadd + p] = formatcmd[9 + p];
    }

    str.Format(L"%s", formatcmd);
    ::AfxMessageBox(str);
}

void FormatTran(char* dst, const char* src) {
    //格式解析，将一行Hex文件字符串src，按Hex文件组织格式
    //划分出不同的块，块之间通过空格隔开
    // ":llaaaatt[dd]cc"
    // ":"行开始标志；"ll"数据长度；"aaaa"起始地址；"tt"数据类型；"dd"数据；"cc"校验
    //解析后为：": ll aaaa tt [dd] cc"
    int srclen = strlen(src);
    //printf("Length of src:%d",srclen);
    int p = 0, q = 0;//索引，p为src的索引，q为dst索引
    int i;//循环标志
    while (p <= srclen) {//循环，对src按格式解析
        if (p == 0) {
            if (src[p] != ':') {//是否以":"开始
                printf("Format wrong! Please check the format of the file.\n");
                break;
            }
            else {
                dst[q++] = ':'; dst[q++] = ' ';
                p++; continue;
            }
        }
        if (p == 1) {
            for (i = 0; i < 2; i++) {//提取两位的"ll"
                dst[q++] = src[p++];
            }
            dst[q++] = ' '; continue;
        }
        if (p == 3) {
            for (i = 0; i < 4; i++) {//提取四位的"aaaa"
                dst[q++] = src[p++];
            }
            dst[q++] = ' '; continue;
        }
        if (p == 7) {
            for (i = 0; i < 2; i++) {//提取两位的"tt"
                dst[q++] = src[p++];
            }
            dst[q++] = ' '; continue;
        }
        for (i = 0; i < 2; i++) {//将dst的数据位以两个为一组，用空格隔开
            dst[q++] = src[p++];
        }
        dst[q++] = ' ';
    }
    dst[q] = '\0';//结束标志
}





//extern CString m_sEdit1;
void ShowFlash(unsigned char* flashbuffer, int flashsize)
{
    CString strTemp;
    unsigned char* pflash = flashbuffer;
    cks = 0;
    for (int i = 0; i < flashsize; i++)
    {
        if (i % 0x10 == 8)
        {
            (*dlg).m_sEdit1 += L" -";
        }
        if (i % 0x10 == 0)
        {
            if (i != 0)
            {
                (*dlg).m_sEdit1 += L"\r\n";
            }
            else
            {
                (*dlg).m_sEdit1 = L"";
            }
            strTemp.Format(L"%04X: ", i);
            (*dlg).m_sEdit1 += strTemp;
        }
        cks += (unsigned char)*(pflash) ;
        //cks &= 0xFFFF;
        strTemp.Format(L" %02X", (unsigned char)*(pflash++));

        (*dlg).m_sEdit1 += strTemp;
    }

    strTemp.Format(L"Check Sum= %08X\r\n", cks);
    (*dlg).m_sEdit1 = strTemp + (*dlg).m_sEdit1;
}

/*
void WriteFlashToFile(char* flashbuffer, char* filename, int flashsize) {
    //将Flash的内容写入文件中
    //flashbuffer:内存单元指针；filename:要写入的文件文件名；
    //flashsize:写入的长度，以字节计
    FILE* fp;
    int i, rowlen = 16;
    if ((fp = fopen(filename, "w")) == NULL) {
        printf("Open file error.\n");
        exit(0);
    }
    //开始写入
    for (i = 0; i < flashsize; i += 2) {
        if ((i / 2) % (rowlen) == 0) {
            if (i != 0) {//不是第一行，遇到写满16个单元换行
                fprintf(fp, "\n");
            }
            fprintf(fp, "%06X\t", i / 2);//内存单元地址输出格式：6位十六进制（A~F大写）
        }
        fprintf(fp, "%c%c ", flashbuffer[i], flashbuffer[i + 1]);
    }
    fclose(fp);

}
*/