#include "pch.h"
#include "hex.h"
#include <stdio.h>
#include <string.h>
//#include <stdlib.h>
#include "jw_OTADlg.h"


#define FLASHVOLUM 32


int  flashsize = 0x8000;                     //flash�Ĵ洢��Ԫ����
unsigned char buffer[256];                   //�洢hex�ļ���һ������
unsigned char trsbuffer[200];                //�洢hex�ļ������������
unsigned char flashbuffer[0x8000];           //�洢Flash�е�����
unsigned char flashbufferT[0x8000];           //�洢Flash�е�����
long cks = 0;
//int  flashsize = 2 * FLASHVOLUM * 1024 * sizeof(char);      //flash�Ĵ洢��Ԫ����
//char* buffer = malloc(sizeof(char) * 100);                  //�洢hex�ļ���һ������
//char* trsbuffer = malloc(sizeof(char) * 200);               //�洢hex�ļ������������
//char* flashbuffer = malloc(flashsize);                      //�洢Flash�е�����

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
    //flashsize = FLASHVOLUM * 1024 * sizeof(char);      //flash�Ĵ洢��Ԫ����
    //buffer = (char*)malloc(sizeof(char) * 100);                  //�洢hex�ļ���һ������
    //trsbuffer = (char*)malloc(sizeof(char) * 200);               //�洢hex�ļ������������
    //flashbuffer = (char*)malloc(flashsize);                      //�洢Flash�е�����
    memset(flashbuffer, 0xFF, flashsize);
    flashsize = 0;
    cks = 0;
    CString str;
    //str.Format(L"%d", 124);
    str = fileName.Right(3).MakeUpper();
   
    if (str.Compare(L"HEX")==0)
    {
        CStdioFile file;
        //���ļ�
        if (!file.Open(fileName, CFile::modeRead))
        {
            ::AfxMessageBox(_T("�ļ���ʧ�ܡ�"));
            return;
        }
        //���ļ�
        CString strText = _T("");

        while (file.ReadString(strText))
        {
            TranHexToBuffer(flashbuffer, strText);
        }

        ShowFlash(flashbuffer, 32 * 1024);
        //�ر��ļ�
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
    int startadd;                                           //formatcmdָʾ����ʼ��ַ
    int count;

    CString eofstring = L":00000001FF";
    if (formatcmd == eofstring) {//������־
        printf("End of file.\n");
        //AfxMessageBox(L"End of file.\n");
        return;
    }
    if (formatcmd.GetAt(0) != ':') {                              //������":"��ʼ
        printf("Format wrong!\n");
        ::AfxMessageBox(L"Format wrong!\n");
        return;
    }
    if (formatcmd.GetLength() < 11) {                           //�ַ�������С��11
        printf("Format wrong!\n");

        ::AfxMessageBox(L"Format wrong!\n");
        return;
    }

    count = wcstol(formatcmd.Mid(1, 2), NULL, 16);              //������ݵ�Ԫ����
    startadd = wcstol(formatcmd.Mid(3, 4), NULL, 16);           //������ݵ�Ԫ����
    if (flashsize < startadd + count)
        flashsize = startadd + count;
    if (formatcmd.GetLength() < 11 + count * 2) {               //�ַ����������𻵣���ʧ��
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
    //��formatcmd�ַ�����������hex�ļ���ʽ�޸�flashbuffer�еĴ洢��Ԫ
    //�� formatcmd=":03000000020030CB"
    //��flahsbuffer����"0000"��ʼ������"03"���ڴ浥Ԫ������Ϊ"020030"
    //flashsize:��ʾflash�Ĵ洢��Ԫ����
    char* eofstring = ":00000001FF";
    int startadd;                                           //formatcmdָʾ����ʼ��ַ
    int count;                                              //formatcmdָʾ�����ݵ�Ԫ����
    int p, q;

    CString str;//����ָ��
    if (strcmp(formatcmd, eofstring) == 0) {//������־
        printf("End of file.\n");
        AfxMessageBox(L"End of file.\n");
        return;
    }
    if (formatcmd[0] != ':') {                              //������":"��ʼ
        printf("Format wrong!\n");
        ::AfxMessageBox(L"Format wrong!\n");
        return;
    }
    if (size < 11) {                           //�ַ�������С��11
        printf("Format wrong!\n");
        str.Format(L"Format wrong! len=%d.\n %s", size, formatcmd);

        ::AfxMessageBox(str);
        return;
    }
    count = 0;// HexToDec((const char*)ExtraSubStr(formatcmd, 1, 2));         //������ݵ�Ԫ����
    startadd = 0;// HexToDec((const char*)ExtraSubStr(formatcmd, 3, 4));  //�����ʼ��ַ
    if (size < 11 + count * 2) {               //�ַ����������𻵣���ʧ��
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
    //��ʽ��������һ��Hex�ļ��ַ���src����Hex�ļ���֯��ʽ
    //���ֳ���ͬ�Ŀ飬��֮��ͨ���ո����
    // ":llaaaatt[dd]cc"
    // ":"�п�ʼ��־��"ll"���ݳ��ȣ�"aaaa"��ʼ��ַ��"tt"�������ͣ�"dd"���ݣ�"cc"У��
    //������Ϊ��": ll aaaa tt [dd] cc"
    int srclen = strlen(src);
    //printf("Length of src:%d",srclen);
    int p = 0, q = 0;//������pΪsrc��������qΪdst����
    int i;//ѭ����־
    while (p <= srclen) {//ѭ������src����ʽ����
        if (p == 0) {
            if (src[p] != ':') {//�Ƿ���":"��ʼ
                printf("Format wrong! Please check the format of the file.\n");
                break;
            }
            else {
                dst[q++] = ':'; dst[q++] = ' ';
                p++; continue;
            }
        }
        if (p == 1) {
            for (i = 0; i < 2; i++) {//��ȡ��λ��"ll"
                dst[q++] = src[p++];
            }
            dst[q++] = ' '; continue;
        }
        if (p == 3) {
            for (i = 0; i < 4; i++) {//��ȡ��λ��"aaaa"
                dst[q++] = src[p++];
            }
            dst[q++] = ' '; continue;
        }
        if (p == 7) {
            for (i = 0; i < 2; i++) {//��ȡ��λ��"tt"
                dst[q++] = src[p++];
            }
            dst[q++] = ' '; continue;
        }
        for (i = 0; i < 2; i++) {//��dst������λ������Ϊһ�飬�ÿո����
            dst[q++] = src[p++];
        }
        dst[q++] = ' ';
    }
    dst[q] = '\0';//������־
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
    //��Flash������д���ļ���
    //flashbuffer:�ڴ浥Ԫָ�룻filename:Ҫд����ļ��ļ�����
    //flashsize:д��ĳ��ȣ����ֽڼ�
    FILE* fp;
    int i, rowlen = 16;
    if ((fp = fopen(filename, "w")) == NULL) {
        printf("Open file error.\n");
        exit(0);
    }
    //��ʼд��
    for (i = 0; i < flashsize; i += 2) {
        if ((i / 2) % (rowlen) == 0) {
            if (i != 0) {//���ǵ�һ�У�����д��16����Ԫ����
                fprintf(fp, "\n");
            }
            fprintf(fp, "%06X\t", i / 2);//�ڴ浥Ԫ��ַ�����ʽ��6λʮ�����ƣ�A~F��д��
        }
        fprintf(fp, "%c%c ", flashbuffer[i], flashbuffer[i + 1]);
    }
    fclose(fp);

}
*/