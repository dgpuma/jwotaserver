#include "pch.h"
#include "Lion.h"
#include <fstream>
//#include "stdlib.h"
//#include "stdio.h"
#include "string.h"
//#include "io.h" 
#include<iostream>
#include <cstring>
#include "resource.h"
//#include "MFCApplication1Dlg.h"
using namespace std;
HANDLE hCom;  //ȫ�ֱ��������ھ��  
#define NOPARITY            0  
#define ODDPARITY           1  
#define EVENPARITY          2  
#define ONESTOPBIT          0  
#define ONE5STOPBITS        1  
#define TWOSTOPBITS         2  
#define CBR_110             110  
#define CBR_300             300  
#define CBR_600             600  
#define CBR_1200            1200  
#define CBR_2400            2400  
#define CBR_4800            4800  
#define CBR_9600            9600  
#define CBR_14400           14400  
#define CBR_19200           19200  
#define CBR_38400           38400  
#define CBR_56000           56000  
#define CBR_57600           57600  
#define CBR_115200          115200  
#define CBR_128000          128000  
#define CBR_256000          256000  

volatile char m_ComState;
DWORD WINAPI ThreadFunc(LPVOID);

unsigned char H2D(unsigned char Hex)
{
	unsigned char Dec = 0;
	if (Hex >= '0')
	{
		if (Hex < 'A') Dec = Hex - '0';
		else Dec = Hex + 10 - 'A';
	}
	return Dec;
}

wchar_t* c2w(const char* str)
{
	int length = strlen(str) + 1;
	wchar_t* t = (wchar_t*)malloc(sizeof(wchar_t) * length);
	memset(t, 0, length * sizeof(wchar_t));
	MultiByteToWideChar(CP_ACP, 0, str, strlen(str), t, length);
	return t;
}


char* wtoc(wchar_t* wText, char* psText)
{
	DWORD dwNum = WideCharToMultiByte(CP_ACP, NULL, wText, -1, NULL, 0, NULL, FALSE);//�ѵ�����������NULL�ĵ����ַ����ĳ��Ȱ�����β��
	//char* psText = NULL;
	psText = new char[dwNum];
	if (!psText)
	{
		delete[]psText;
		psText = NULL;
	}
	WideCharToMultiByte(CP_ACP, NULL, wText, -1, psText, dwNum, NULL, FALSE);
	return psText;
	//string ret = psText;
	//delete[]psText;
	//return ret;
}


wchar_t* ctow(char* sText)
{
	DWORD dwNum = MultiByteToWideChar(CP_ACP, 0, sText, -1, NULL, 0);//�ѵ�����������NULL�ĵ����ַ����ĳ��Ȱ�����β��

	wchar_t* pwText = NULL;
	pwText = new wchar_t[dwNum];
	if (!pwText)
	{
		delete[]pwText;
		pwText = NULL;
	}
	unsigned nLen = MultiByteToWideChar(CP_ACP, 0, sText, -1, pwText, dwNum + 10);
	if (nLen >= 0)
	{
		pwText[nLen] = 0;
	}
	return pwText;
	/*wstring ret = pwText;
	delete[]pwText;
	return ret;*/
}

void LoadFile(CString sFileName, unsigned char* buf, int& iLen)
{
	CString str, strTemp;
	str = sFileName.Right(3).MakeUpper();

	if (str == "BIN")
	{
		ifstream  fin1(sFileName, ios_base::binary);
		istream::pos_type current_pos1 = fin1.tellg();//��¼�µ�ǰλ�� 
		fin1.seekg(0, ios_base::end);//�ƶ����ļ�β
		int file_size1 = fin1.tellg();
		fin1.seekg(current_pos1);//�ƶ���ԭ����λ��
		memset(buf, 0, iLen);
		iLen = file_size1;
		fin1.read(reinterpret_cast<char*>(buf), file_size1 * sizeof(BYTE));
		fin1.close();
		//iLen = 2;
	}
	else if (str == "HEX" || str == "MOT")
	{
		memset(buf, 0x00, iLen);
		//��txt�ļ�
		ifstream infile;//�����ļ�����
		infile.open(sFileName, ios::in);//��txt
		if (!infile)
		{
			//AfxMessageBox("��ȡtxt�ļ�ʧ�ܣ�");
			return;
		}
		//string temp, temp2mat;
		char pixelTemp[1024];//�˴�ֻ��6���� 
		int i = 0;
		int iLen = 0;
		LONG32 BaseAddr = 0;
		LONG32 Addr = 0;

		while (infile.getline(pixelTemp, 1024)) //��ȡһ�У�ֱ�������ж���
		{
			CString str, strTemp;
			int iDataLen = 0;
			unsigned char cks = 0;
			if (pixelTemp[0] == ':')
			{

				iDataLen = strlen(pixelTemp);
				_strupr_s(pixelTemp, iDataLen + 1);
				int num = MultiByteToWideChar(0, 0, pixelTemp, -1, NULL, 0);
				wchar_t* wide = new wchar_t[num];
				MultiByteToWideChar(0, 0, pixelTemp, -1, wide, num);

				iLen = wcslen(wide);

				if (strcmp(pixelTemp, ":00000001FF") == 0) break;
				iDataLen = (H2D(pixelTemp[1]) << 4) + H2D(pixelTemp[2]);
				//iDataLen = H2D(pixelTemp[2])<<4;
				/*wsprintf(wide, L"iDataLen=%d, cks=%02X", iDataLen, H2D(pixelTemp[2]));
				MessageBox(NULL, wide, L"CheckSum", MB_OK);*/

				cks = iDataLen + (H2D(pixelTemp[3]) << 4) + H2D(pixelTemp[4])
					+ (H2D(pixelTemp[5]) << 4) + H2D(pixelTemp[6])
					+ (H2D(pixelTemp[7]) << 4) + H2D(pixelTemp[8]);
				if (pixelTemp[7] == '0' && pixelTemp[8] == '0')  //DATA
				{
					Addr = BaseAddr + (H2D(pixelTemp[3]) << 12) + (H2D(pixelTemp[4]) << 8)
						+ (H2D(pixelTemp[5]) << 4) + (H2D(pixelTemp[6]) << 0);
					/*wsprintf(wide, L"iDataLen=%d, cks=%02X", iDataLen, cks);
					MessageBox(NULL, wide, L"CheckSum", MB_OK);*/
					for (int i = 0; i < iDataLen; i++)
					{
						buf[Addr] = (H2D(pixelTemp[9 + i * 2]) << 4) + H2D(pixelTemp[10 + i * 2]);
						cks = (cks + buf[Addr]) & 0xFF;
						Addr++;
						/*	wsprintf(wide, L"i=%d,iVal=%02X, CKS=%d",i, buf[Addr]&0xFF, cks);
							MessageBox(NULL, wide, L"CheckSum", MB_OK);*/
					}
				}

				else if (pixelTemp[11] == '0' && pixelTemp[12] == '2')  //SEGMENT
				{
					BaseAddr = (H2D(pixelTemp[3]) << 12) + (H2D(pixelTemp[4]) << 8)
						+ (H2D(pixelTemp[5]) << 4) + (H2D(pixelTemp[6]) << 0);
					BaseAddr <<= 4;
					for (int i = 0; i < iDataLen; i++)
					{
						buf[Addr] = (H2D(pixelTemp[9 + i * 2]) << 4) + H2D(pixelTemp[10 + i * 2]);
						cks = (cks + buf[Addr]) & 0xFF;
						//cks = 0xFF;
					}
				}

				else if (pixelTemp[11] == '0' && pixelTemp[12] == '4')  //BASEADDR
				{
					BaseAddr = (H2D(pixelTemp[3]) << 12) + (H2D(pixelTemp[4]) << 8)
						+ (H2D(pixelTemp[5]) << 4) + (H2D(pixelTemp[6]) << 0);
					BaseAddr <<= 16;
					for (int i = 0; i < iDataLen; i++)
					{
						buf[Addr] = (H2D(pixelTemp[9 + i * 2]) << 4) + H2D(pixelTemp[10 + i * 2]);
						cks = (cks + buf[Addr]) & 0xFF;
						//cks &= 0xFF;
					}
				}

				//else
			/*	{
					wsprintf(wide, L"7=%C, 8=%C", pixelTemp[7], pixelTemp[8]);
					MessageBox(NULL, wide, L"CheckSum", MB_OK);
				}*/
				cks = 0x100 - cks;
				/*wsprintf(wide, L"CKS=%2X", cks);
				MessageBox(NULL, wide, L"CheckSum", MB_OK);*/
				if ((H2D(pixelTemp[9 + iDataLen * 2]) << 4) + H2D(pixelTemp[10 + iDataLen * 2]) != cks)
					break;
			}

			/*int num = MultiByteToWideChar(0, 0, pixelTemp, -1, NULL, 0);
			wchar_t *wide = new wchar_t[num];
			MultiByteToWideChar(0, 0, pixelTemp, -1, wide, num);

			MessageBox(NULL,wide, L"Prompt", MB_OK);
			iLen = wcslen(wide);
			wsprintf(wide,L"Length=%d",iLen);
			MessageBox(NULL, wide, L"Prompt", MB_OK);*/
		}
		infile.close();
	}
}

//

LionThread::LionThread()
{

}
LionThread::~LionThread()
{

}

int LionThread::ThreadFunc(LPVOID pParam)
{
	LionThread* pObj = (LionThread*)pParam;
	return pObj->ThreadFuncKernel();
}

int LionThread::ThreadFuncKernel()
{
	while (1)
	{
		// my thread
		// to control class non-static member
	}
	return 0;
}

DWORD WINAPI ThreadFunc(LPVOID param)
{
	CWnd* pMainDlg = (CWnd*)param;
	for (int i = 0; i < 10; i++)
	{

	}


	pMainDlg->MessageBoxW(L"ThreadMessageBox");

	Sleep(5000);



	//::SendMessage(pMainDlg->GetSafeHwnd(), WM_THREADOVER, 1UL, 2UL);


	return 0;
}

UINT ExecutingFunction(LPVOID pParam)
{
	return 0;
}

volatile BOOL m_bRun;
void ThreadFunc()
{
	CTime time;
	CString strTime;
	m_bRun = TRUE;
	while (m_bRun)
	{
		time = CTime::GetCurrentTime();
		strTime = time.Format("%H:%M:%S");
		strTime = "XXX";
		//::SetDlgItemText(AfxGetMainWnd()->m_hWnd, IDC_BTN_LOADFILE, strTime);

		Sleep(19000);
	}
}
//
//bool OpenPort(int iPort)
//{
//	HANDLE hCom;  //ȫ�ֱ��������ھ��  
//	CString PortName;
//	PortName.Format(L"\\\\.\\COM%d", iPort);
//	hCom = CreateFile(PortName,  //COM1��  
//		GENERIC_READ | GENERIC_WRITE, //�������д  
//		0,  //��ռ��ʽ  
//		NULL,
//		OPEN_EXISTING,  //�򿪶����Ǵ���  
//		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, //�ص���ʽ  
//		NULL);
//	if (hCom == INVALID_HANDLE_VALUE)
//	{
//		AfxMessageBox(L"��COMʧ��!");
//		return FALSE;
//	}
//	//LPDCB lpDCB;
//	//GetCommState(hCom, //��ʶͨѶ�˿ڵľ��  
//	//	lpDCB //ָ��һ���豸���ƿ飨DCB�ṹ����ָ��  
//	//);
//	//BOOL SetupComm(
//
////	HANDLE hFile,   // ͨ���豸�ľ��   
////	DWORD dwInQueue,    // ���뻺�����Ĵ�С���ֽ�����   
////	DWORD dwOutQueue    // ����������Ĵ�С���ֽ�����  
////);
//
//	SetupComm(hCom, 1024, 1024); //���뻺����������������Ĵ�С����1024  
//
//	//typedef struct _COMMTIMEOUTS {
//	//	DWORD ReadIntervalTimeout; //�������ʱ  
//	//	DWORD ReadTotalTimeoutMultiplier; //��ʱ��ϵ��  
//	//	DWORD ReadTotalTimeoutConstant; //��ʱ�䳣��  
//	//	DWORD WriteTotalTimeoutMultiplier; // дʱ��ϵ��  
//	//	DWORD WriteTotalTimeoutConstant; //дʱ�䳣��  
//	//} COMMTIMEOUTS, *LPCOMMTIMEOUTS;
//	COMMTIMEOUTS TimeOuts;
//	//�趨����ʱ  
//	TimeOuts.ReadIntervalTimeout = 1000;
//	TimeOuts.ReadTotalTimeoutMultiplier = 500;
//	TimeOuts.ReadTotalTimeoutConstant = 5000;
//	//�趨д��ʱ  
//	TimeOuts.WriteTotalTimeoutMultiplier = 500;
//	TimeOuts.WriteTotalTimeoutConstant = 2000;
//	SetCommTimeouts(hCom, &TimeOuts); //���ó�ʱ  
//
//	DCB dcb;
//	GetCommState(hCom, &dcb);
//	dcb.BaudRate = 9600; //������Ϊ9600  
//	dcb.ByteSize = 8; //ÿ���ֽ���8λ  
//	dcb.Parity = NOPARITY; //����żУ��λ  
//	dcb.StopBits = TWOSTOPBITS; //����ֹͣλ  
//	SetCommState(hCom, &dcb);
//
//	PurgeComm(hCom, PURGE_TXCLEAR | PURGE_RXCLEAR);
//
//	return TRUE;
//}
//
//void ClosePort()
//{
//	//BOOL CloseHandle(
//	//	HANDLE hObject; //handle to object to close   
//	//);
//	CloseHandle(hCom);
//}
//
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// COM��ͬ����д
//// ����ͬ������WaitCommEvent�ȴ����ݶ�ȡ�ķ���������ʵ�ü�ֵ�����Բ����ۡ�
//
//HANDLE InitCOM(LPCTSTR Port)
//{
//	HANDLE hCom = INVALID_HANDLE_VALUE;
//	hCom = CreateFile(Port, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING,
//		0/*ͬ����ʽ�򿪴���*/, NULL);
//	if (INVALID_HANDLE_VALUE == hCom)
//	{
//		return INVALID_HANDLE_VALUE;
//	}
//	SetupComm(hCom, 4096, 4096);//���û���
//
//	DCB dcb;
//
//	GetCommState(hCom, &dcb);//���ô���
//	dcb.DCBlength = sizeof(dcb);
//	dcb.BaudRate = CBR_9600;
//	dcb.StopBits = ONESTOPBIT;
//	SetCommState(hCom, &dcb);
//
//	PurgeComm(hCom, PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR | PURGE_TXABORT);//��ջ���
//
//	COMMTIMEOUTS ct;
//	//���ö�ȡ��ʱʱ�䣬��ReadFlie��ȴ�ʱ��
//	ct.ReadIntervalTimeout = 0;
//	ct.ReadTotalTimeoutConstant = 5000;
//	ct.ReadTotalTimeoutMultiplier = 500;
//
//	ct.WriteTotalTimeoutMultiplier = 500;
//	ct.WriteTotalTimeoutConstant = 5000;
//
//	SetCommTimeouts(hCom, &ct);//���ó�ʱ
//
//	return hCom;
//
//}
//
//
//bool ComRead(HANDLE hCom, LPBYTE buf, int& len)
//{
//	DWORD ReadSize = 0;
//	BOOL rtn = FALSE;
//
//	//���ö�ȡ1���ֽ����ݣ��������������ݵ���ʱ����������أ�����ֱ����ʱ
//	rtn = ReadFile(hCom, buf, 1, &ReadSize, NULL);
//
//	//����ǳ�ʱrtn=true����ReadSize=0����������ݵ�����ȡһ���ֽ�ReadSize=1
//	if (rtn == TRUE && 1 == ReadSize)
//	{
//		DWORD Error;
//		COMSTAT cs = { 0 };
//		int ReadLen = 0;
//		//��ѯʣ������ֽ�δ��ȡ���洢��cs.cbInQue��
//		ClearCommError(hCom, &Error, &cs);
//		ReadLen = (cs.cbInQue > len) ? len : cs.cbInQue;
//		if (ReadLen > 0)
//		{
//			//����֮ǰ�ȴ�ʱ�Զ�ȡһ���ֽڣ�����buf+1
//			rtn = ReadFile(hCom, buf + 1, ReadLen, &ReadSize, NULL);
//			len = 0;
//			if (rtn)
//			{
//				len = ReadLen + 1;
//			}
//		}
//	}
//	PurgeComm(hCom, PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR | PURGE_TXABORT);
//	return rtn != FALSE;
//}
//
//bool ComWrite(HANDLE hCom, LPBYTE buf, int& len)
//{
//	PurgeComm(hCom, PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR | PURGE_TXABORT);
//	BOOL rtn = FALSE;
//	DWORD WriteSize = 0;
//	rtn = WriteFile(hCom, buf, len, &WriteSize, NULL);
//
//	len = WriteSize;
//	return rtn != FALSE;
//
//}


ComAsy* m_hComAsy;

ComAsy::ComAsy() :
	m_hCom(INVALID_HANDLE_VALUE),
	m_IsOpen(false),
	m_Thread(NULL)
{
	memset(&m_ovWait, 0, sizeof(m_ovWait));
	memset(&m_ovWrite, 0, sizeof(m_ovWrite));
	memset(&m_ovRead, 0, sizeof(m_ovRead));
}

ComAsy::~ComAsy()
{
	UninitCOM();

}

//��ʼ�������ô���
bool ComAsy::InitCOM(LPCTSTR Port, int iBaud)
{
	//AfxMessageBox(Port);
	//return false;
	m_hCom = CreateFile(Port, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING,
		FILE_FLAG_OVERLAPPED | FILE_ATTRIBUTE_NORMAL,//�����첽��ʶ
		NULL);
	if (INVALID_HANDLE_VALUE == m_hCom)
	{
		return false;
	}


	DCB dcb;
	GetCommState(m_hCom, &dcb);
	dcb.DCBlength = sizeof(dcb);
	dcb.BaudRate = iBaud;// CBR_115200;// CBR_9600;
	dcb.ByteSize = 8;
	dcb.Parity = NOPARITY;
	dcb.StopBits = ONESTOPBIT;
	//dcb.fOutxCtsFlow = DTR_CONTROL_DISABLE;
	dcb.fDtrControl = DTR_CONTROL_DISABLE;
	dcb.XonLim = 512;
	dcb.XoffLim = 1;
	SetCommState(m_hCom, &dcb);//���ô���
	SetupComm(m_hCom, 2048, 2048);//���÷��ͽ��ջ���
	PurgeComm(m_hCom, PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR | PURGE_TXABORT);

	COMMTIMEOUTS ct;
	ct.ReadIntervalTimeout = MAXDWORD;//��ȡ����ʱ����Ϊ��WaitCommEvent�ȴ�����
	ct.ReadTotalTimeoutConstant = 0;  //
	ct.ReadTotalTimeoutMultiplier = 0;//

	ct.WriteTotalTimeoutMultiplier = 500;
	ct.WriteTotalTimeoutConstant = 5000;

	SetCommTimeouts(m_hCom, &ct);

	//�����¼�����
	m_ovRead.hEvent = CreateEvent(NULL, false, false, NULL);
	m_ovWrite.hEvent = CreateEvent(NULL, false, false, NULL);
	m_ovWait.hEvent = CreateEvent(NULL, false, false, NULL);

	SetCommMask(m_hCom, EV_ERR | EV_RXCHAR);//���ý����¼�

	//������ȡ�߳�
	m_Thread = (HANDLE)_beginthreadex(NULL, 0, &ComAsy::OnRecv, this, 0, NULL);
	m_IsOpen = true;
	return true;

}

//д�����ݣ�����д������һ�㲻����̫�ߵ�����Ҫ�������첽д��ʱ��������ں�̨д�룬���ȴ�д����ɺ����˳�����ʱ�൱��ͬ����д�롣
bool ComAsy::ComWrite(LPBYTE buf, int& len)
{
	BOOL rtn = FALSE;
	DWORD WriteSize = 0;

	PurgeComm(m_hCom, PURGE_TXCLEAR | PURGE_TXABORT);
	m_ovWait.Offset = 0;
	rtn = WriteFile(m_hCom, buf, len, &WriteSize, &m_ovWrite);

	len = 0;
	if (FALSE == rtn && GetLastError() == ERROR_IO_PENDING)//��̨��ȡ
	{
		//�ȴ�����д�����
		if (FALSE == ::GetOverlappedResult(m_hCom, &m_ovWrite, &WriteSize, TRUE))
		{
			return false;
		}
	}

	len = WriteSize;
	return rtn != FALSE;
}

//��ȡ����
int recvlen;
unsigned int __stdcall ComAsy::OnRecv(void* LPParam)
{
	ComAsy* obj = static_cast<ComAsy*>(LPParam);

	DWORD WaitEvent = 0, Bytes = 0;
	BOOL Status = FALSE;
	BYTE ReadBuf[4096];
	BYTE ReadBufTrans[4096];
	DWORD Error;
	COMSTAT cs = { 0 };

	while (obj->m_IsOpen)
	{
		WaitEvent = 0;
		obj->m_ovWait.Offset = 0;
		Status = WaitCommEvent(obj->m_hCom, &WaitEvent, &obj->m_ovWait);
		//WaitCommEventҲ��һ���첽���������Ҫ�ȴ�
		if (FALSE == Status && GetLastError() == ERROR_IO_PENDING)//
		{
			//����������������̻߳�ͣ�ڴˣ����hCom�رջ���������False
			Status = GetOverlappedResult(obj->m_hCom, &obj->m_ovWait, &Bytes, TRUE);
		}
		ClearCommError(obj->m_hCom, &Error, &cs);
		if (TRUE == Status //�ȴ��¼��ɹ�
			&& WaitEvent & EV_RXCHAR//�����������ݵ���
			&& cs.cbInQue > 0)//������
		{
			Bytes = 0;
			obj->m_ovRead.Offset = 0;
			memset(ReadBuf, 0, sizeof(ReadBuf));
			//�����Ѿ����ﻺ������ReadFile���ᵱ���첽�������������ȡ������True
			Status = ReadFile(obj->m_hCom, ReadBuf, sizeof(ReadBuf), &Bytes, &obj->m_ovRead);
			if (Status != FALSE)
			{
				for (int i = 0; i < Bytes; i++)
					ReadBufTrans[recvlen + i] = ReadBuf[i];
				recvlen += Bytes;
				if (recvlen >= 6)
				{

					CWnd* pWnd = AfxGetApp()->GetMainWnd();

					HWND* pDlg;
					pDlg = (HWND*)pWnd;
					::PostMessage(pWnd->GetSafeHwnd(), UM_RECEIVE, recvlen, (LPARAM)ReadBufTrans);
					recvlen = 0;

				}
				//cout << "Read:" << (LPCSTR)ReadBuf << "   Len:" << Bytes << endl;
				//CString str, strTemp;
				////str.Format(L"%d:", Bytes);
				//
				//for (int i = 0; i < Bytes; i++)
				//{
				//	strTemp.Format(L"%02X ", 0xFF & ReadBuf[i]);
				//	str += strTemp;
				//}
				//MessageBox(NULL, str, NULL, 0);
				//PostMessage(m_pMainWnd��UM_RECEIVE��&str��&Bytes);

				/*CWnd* pWnd = AfxGetApp()->GetMainWnd();

					HWND* pDlg;
				pDlg = (HWND*)pWnd;
				::PostMessage(pWnd->GetSafeHwnd(), UM_RECEIVE, Bytes, (LPARAM)ReadBuf);
				*/

			}
			//PurgeComm(obj->m_hCom, PURGE_RXCLEAR | PURGE_RXABORT);
		}

	}
	return 0;

}

void ComAsy::purge()
{
	recvlen = 0;
	PurgeComm(m_hCom, PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR | PURGE_TXABORT);
}

//�رմ���
void ComAsy::UninitCOM()
{
	m_IsOpen = false;
	if (INVALID_HANDLE_VALUE != m_hCom)
	{
		CloseHandle(m_hCom);
		m_hCom = INVALID_HANDLE_VALUE;
	}
	if (NULL != m_ovRead.hEvent)
	{
		CloseHandle(m_ovRead.hEvent);
		m_ovRead.hEvent = NULL;
	}
	if (NULL != m_ovWrite.hEvent)
	{
		CloseHandle(m_ovWrite.hEvent);
		m_ovWrite.hEvent = NULL;
	}
	if (NULL != m_ovWait.hEvent)
	{
		CloseHandle(m_ovWait.hEvent);
		m_ovWait.hEvent = NULL;
	}
	if (NULL != m_Thread)
	{
		WaitForSingleObject(m_Thread, 5000);//�ȴ��߳̽���
		CloseHandle(m_Thread);
		m_Thread = NULL;
	}

}

//BOOL WritePrivateProfileString(
//	LPCTSTR lpAppName,
//	LPCTSTR lpKeyName,
//	LPCTSTR lpString,
//	LPCTSTR lpFileName
//);

//DWORD GetPrivateProfileString(
//	LPCTSTR lpAppName,
//	LPCTSTR lpKeyName,
//	LPCTSTR lpDefault,
//	LPTSTR lpReturnedString,
//	DWORD nSize,
//	LPCTSTR lpFileName
//);



int LoadMyInfo(void* para, int n_column, char** column_value, char** column_name)
{
	//para������ sqlite3_exec �ﴫ��� void * ����
	//ͨ��para����������Դ���һЩ�����ָ�루������ָ�롢�ṹָ�룩��Ȼ����������ǿ��ת���ɶ�Ӧ�����ͣ���������void*���ͣ�����ǿ��ת����������Ͳſ��ã���Ȼ�������Щ����
	//n_column����һ����¼�ж��ٸ��ֶ� (��������¼�ж�����)
	// char ** column_value �Ǹ��ؼ�ֵ������������ݶ������������ʵ�����Ǹ�1ά���飨��Ҫ��Ϊ��2ά���飩��ÿһ��Ԫ�ض���һ�� char * ֵ����һ���ֶ����ݣ����ַ�������ʾ����/0��β��
	//char ** column_name �� column_value�Ƕ�Ӧ�ģ���ʾ����ֶε��ֶ�����
	//����Ҳ�ʹ�� para �������������Ĵ���.
	LPWSTR lpStr = new WCHAR[1024];
	int i;
	CString str;
	swprintf_s(lpStr, 80, L"��¼���� %d ���ֶ�\r\n", n_column);
	MessageBox(NULL, lpStr, L"PROMPT", MB_OK);
	for (i = 0; i < n_column; i++)
	{
		//swprintf_s(lpStr, 1000,L"�ֶ���:%s->�ֶ�ֵ:%s\r\n",column_name[i], column_value[i]);
		//str.Format(L"�ֶ���:%s->�ֶ�ֵ:%s\r\n", char2wchar(column_name[i]), char2wchar(column_value[i]));
		//UTF8ToUnicode(column_name[i], lpStr);
		//str = (CString)lpStr;
		str.Format(L"�ֶ���:%s->�ֶ�ֵ:%s\r\n", char2wchar(U2G(column_name[i])), U2T(column_value[i]));
		MessageBox(NULL, str, L"PROMPT", MB_OK);
	}
	//printf("------------------ / n");
	return 0;
}


void DrawLabel(CDC* pDC, int ix_offset, int iy_offset)
{
	int ix, iy;
	ix = 100 + ix_offset;
	iy = 100 + iy_offset;
	/*
	pDC->MoveTo(ix, iy);//���
	ix += 500;
	pDC->LineTo(ix, iy);//�յ�
	iy += 300;
	pDC->LineTo(ix, iy);//�յ�
	ix -= 500;
	pDC->LineTo(ix, iy);//�յ�
	iy -= 300;
	pDC->LineTo(ix, iy);//�յ�
	*/
	CDC MemDC;           //���ȶ���һ����ʾ�豸����
	HBITMAP tempBmp = (HBITMAP)LoadImage(AfxGetInstanceHandle(),
		L"h:\\\\123.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	MemDC.CreateCompatibleDC(NULL);
	MemDC.MoveTo(00, 00);
	MemDC.LineTo(1000, 400);
	SelectObject(MemDC, tempBmp);
	pDC->BitBlt(0, 700, 1000, 1000, &MemDC, 0, 0, SRCCOPY);
}


//void GetGuid()
//{
//	CString str, strTemp;
//	char* ss = new char[1024];
//	//str =
//	//str= char2wchar(GetCPUID());
//	//str = ss;
//	//str.Format(L"%s", GetCPUID());
//	//str.Format(L"%s", "12345");
//	//m_strEditBarcode = str;
//	//str =CA2T(ss);
//	//str = CA2W(GetCPUID());
//	//getLion(ss);
//	//str = getLion(ss);
//	//ss = getLion1();
//	//str = GetCPUID();
//	//MessageBox(str, 0, 0);
//	//str.Format(L"%s", ss);
//	/*str = "";
//	for (int i = 0; i < 4; i++)
//	{
//		strTemp.Format(L"%02X ", ss[i] & 0xFF);
//		str += strTemp;
//	}*/
//	//m_strEditBarcode = str;
//
//	CString chDriveInfo[100];
//	DWORD dwNum;
//	GetSysInfo* si = new GetSysInfo();
//	si->GetDiskInfo(dwNum, chDriveInfo);
//	for (int i = 0; i < dwNum; i++)
//	{
//		str.Format(L"No.:%d\r\n", i);
//		//MessageBox(str, 0, 0);
//
//		strTemp = chDriveInfo[i];
//		str += strTemp + L"\r\n";
//
//		//MessageBox(str, 0, 0);
//	}
//	int icount;
//	CWmiInfo* wmi = new CWmiInfo();
//	wmi->InitWmi();
//
//	/*wmi->GetSingleItemInfo(L"Win32_DiskDrive", L"Availability", str);
//	MessageBox(str, 0, 0);*/
//
//	wmi->GetSingleItemInfo(L"Win32_baseboard", L"Manufacturer", str);
//	//MessageBox(str, 0, 0);
//	wmi->GetSingleItemInfo(L"Win32_BaseBoard", L"SerialNumber", str);
//	//MessageBox(str, 0, 0);
//	wmi->GetSingleItemInfo(L"Win32_Processor", L"Name", str);
//	//MessageBox(str, 0, 0);
//
//	//GetGroupItemInfo(L"Win32_DiskDrive", chDriveInfo, iCount, CString&);
//	wmi->ReleaseWmi();
//	str = getUUID();
//	delete wmi;
//	//MessageBox(str, 0, 0);
//	int len = 128;
//	GetCpuByCmd(ss, len);
//	str = ss;
//	/*str = "";
//	for (int i = 0; i < len; i++)
//	{
//		strTemp.Format(L"%02X", ss[i] & 0xFF);
//		str += strTemp;
//	}*/
//	MessageBox(NULL, str, 0, 0);
//}