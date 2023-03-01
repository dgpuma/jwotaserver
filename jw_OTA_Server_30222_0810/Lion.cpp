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
HANDLE hCom;  //全局变量，串口句柄  
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
	DWORD dwNum = WideCharToMultiByte(CP_ACP, NULL, wText, -1, NULL, 0, NULL, FALSE);//把第五个参数设成NULL的到宽字符串的长度包括结尾符
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
	DWORD dwNum = MultiByteToWideChar(CP_ACP, 0, sText, -1, NULL, 0);//把第五个参数设成NULL的到宽字符串的长度包括结尾符

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
		istream::pos_type current_pos1 = fin1.tellg();//记录下当前位置 
		fin1.seekg(0, ios_base::end);//移动到文件尾
		int file_size1 = fin1.tellg();
		fin1.seekg(current_pos1);//移动到原来的位置
		memset(buf, 0, iLen);
		iLen = file_size1;
		fin1.read(reinterpret_cast<char*>(buf), file_size1 * sizeof(BYTE));
		fin1.close();
		//iLen = 2;
	}
	else if (str == "HEX" || str == "MOT")
	{
		memset(buf, 0x00, iLen);
		//读txt文件
		ifstream infile;//定义文件变量
		infile.open(sFileName, ios::in);//打开txt
		if (!infile)
		{
			//AfxMessageBox("读取txt文件失败！");
			return;
		}
		//string temp, temp2mat;
		char pixelTemp[1024];//此处只读6个数 
		int i = 0;
		int iLen = 0;
		LONG32 BaseAddr = 0;
		LONG32 Addr = 0;

		while (infile.getline(pixelTemp, 1024)) //读取一行，直到所有行读完
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
//	HANDLE hCom;  //全局变量，串口句柄  
//	CString PortName;
//	PortName.Format(L"\\\\.\\COM%d", iPort);
//	hCom = CreateFile(PortName,  //COM1口  
//		GENERIC_READ | GENERIC_WRITE, //允许读和写  
//		0,  //独占方式  
//		NULL,
//		OPEN_EXISTING,  //打开而不是创建  
//		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, //重叠方式  
//		NULL);
//	if (hCom == INVALID_HANDLE_VALUE)
//	{
//		AfxMessageBox(L"打开COM失败!");
//		return FALSE;
//	}
//	//LPDCB lpDCB;
//	//GetCommState(hCom, //标识通讯端口的句柄  
//	//	lpDCB //指向一个设备控制块（DCB结构）的指针  
//	//);
//	//BOOL SetupComm(
//
////	HANDLE hFile,   // 通信设备的句柄   
////	DWORD dwInQueue,    // 输入缓冲区的大小（字节数）   
////	DWORD dwOutQueue    // 输出缓冲区的大小（字节数）  
////);
//
//	SetupComm(hCom, 1024, 1024); //输入缓冲区和输出缓冲区的大小都是1024  
//
//	//typedef struct _COMMTIMEOUTS {
//	//	DWORD ReadIntervalTimeout; //读间隔超时  
//	//	DWORD ReadTotalTimeoutMultiplier; //读时间系数  
//	//	DWORD ReadTotalTimeoutConstant; //读时间常量  
//	//	DWORD WriteTotalTimeoutMultiplier; // 写时间系数  
//	//	DWORD WriteTotalTimeoutConstant; //写时间常量  
//	//} COMMTIMEOUTS, *LPCOMMTIMEOUTS;
//	COMMTIMEOUTS TimeOuts;
//	//设定读超时  
//	TimeOuts.ReadIntervalTimeout = 1000;
//	TimeOuts.ReadTotalTimeoutMultiplier = 500;
//	TimeOuts.ReadTotalTimeoutConstant = 5000;
//	//设定写超时  
//	TimeOuts.WriteTotalTimeoutMultiplier = 500;
//	TimeOuts.WriteTotalTimeoutConstant = 2000;
//	SetCommTimeouts(hCom, &TimeOuts); //设置超时  
//
//	DCB dcb;
//	GetCommState(hCom, &dcb);
//	dcb.BaudRate = 9600; //波特率为9600  
//	dcb.ByteSize = 8; //每个字节有8位  
//	dcb.Parity = NOPARITY; //无奇偶校验位  
//	dcb.StopBits = TWOSTOPBITS; //两个停止位  
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
//// COM口同步读写
//// 由于同步串口WaitCommEvent等待数据读取的方法基本无实用价值，所以不讨论。
//
//HANDLE InitCOM(LPCTSTR Port)
//{
//	HANDLE hCom = INVALID_HANDLE_VALUE;
//	hCom = CreateFile(Port, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING,
//		0/*同步方式打开串口*/, NULL);
//	if (INVALID_HANDLE_VALUE == hCom)
//	{
//		return INVALID_HANDLE_VALUE;
//	}
//	SetupComm(hCom, 4096, 4096);//设置缓存
//
//	DCB dcb;
//
//	GetCommState(hCom, &dcb);//设置串口
//	dcb.DCBlength = sizeof(dcb);
//	dcb.BaudRate = CBR_9600;
//	dcb.StopBits = ONESTOPBIT;
//	SetCommState(hCom, &dcb);
//
//	PurgeComm(hCom, PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR | PURGE_TXABORT);//清空缓存
//
//	COMMTIMEOUTS ct;
//	//设置读取超时时间，及ReadFlie最长等待时间
//	ct.ReadIntervalTimeout = 0;
//	ct.ReadTotalTimeoutConstant = 5000;
//	ct.ReadTotalTimeoutMultiplier = 500;
//
//	ct.WriteTotalTimeoutMultiplier = 500;
//	ct.WriteTotalTimeoutConstant = 5000;
//
//	SetCommTimeouts(hCom, &ct);//设置超时
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
//	//设置读取1个字节数据，当缓存中有数据到达时则会立即返回，否则直到超时
//	rtn = ReadFile(hCom, buf, 1, &ReadSize, NULL);
//
//	//如果是超时rtn=true但是ReadSize=0，如果有数据到达，会读取一个字节ReadSize=1
//	if (rtn == TRUE && 1 == ReadSize)
//	{
//		DWORD Error;
//		COMSTAT cs = { 0 };
//		int ReadLen = 0;
//		//查询剩余多少字节未读取，存储于cs.cbInQue中
//		ClearCommError(hCom, &Error, &cs);
//		ReadLen = (cs.cbInQue > len) ? len : cs.cbInQue;
//		if (ReadLen > 0)
//		{
//			//由于之前等待时以读取一个字节，所欲buf+1
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

//初始化并配置串口
bool ComAsy::InitCOM(LPCTSTR Port, int iBaud)
{
	//AfxMessageBox(Port);
	//return false;
	m_hCom = CreateFile(Port, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING,
		FILE_FLAG_OVERLAPPED | FILE_ATTRIBUTE_NORMAL,//设置异步标识
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
	SetCommState(m_hCom, &dcb);//配置串口
	SetupComm(m_hCom, 2048, 2048);//设置发送接收缓存
	PurgeComm(m_hCom, PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR | PURGE_TXABORT);

	COMMTIMEOUTS ct;
	ct.ReadIntervalTimeout = MAXDWORD;//读取无延时，因为有WaitCommEvent等待数据
	ct.ReadTotalTimeoutConstant = 0;  //
	ct.ReadTotalTimeoutMultiplier = 0;//

	ct.WriteTotalTimeoutMultiplier = 500;
	ct.WriteTotalTimeoutConstant = 5000;

	SetCommTimeouts(m_hCom, &ct);

	//创建事件对象
	m_ovRead.hEvent = CreateEvent(NULL, false, false, NULL);
	m_ovWrite.hEvent = CreateEvent(NULL, false, false, NULL);
	m_ovWait.hEvent = CreateEvent(NULL, false, false, NULL);

	SetCommMask(m_hCom, EV_ERR | EV_RXCHAR);//设置接受事件

	//创建读取线程
	m_Thread = (HANDLE)_beginthreadex(NULL, 0, &ComAsy::OnRecv, this, 0, NULL);
	m_IsOpen = true;
	return true;

}

//写入数据，由于写入数据一般不会有太高的性能要求，所以异步写入时如果数据在后台写入，则会等待写入完成后再退出，此时相当于同步的写入。
bool ComAsy::ComWrite(LPBYTE buf, int& len)
{
	BOOL rtn = FALSE;
	DWORD WriteSize = 0;

	PurgeComm(m_hCom, PURGE_TXCLEAR | PURGE_TXABORT);
	m_ovWait.Offset = 0;
	rtn = WriteFile(m_hCom, buf, len, &WriteSize, &m_ovWrite);

	len = 0;
	if (FALSE == rtn && GetLastError() == ERROR_IO_PENDING)//后台读取
	{
		//等待数据写入完成
		if (FALSE == ::GetOverlappedResult(m_hCom, &m_ovWrite, &WriteSize, TRUE))
		{
			return false;
		}
	}

	len = WriteSize;
	return rtn != FALSE;
}

//读取数据
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
		//WaitCommEvent也是一个异步命令，所以需要等待
		if (FALSE == Status && GetLastError() == ERROR_IO_PENDING)//
		{
			//如果缓存中无数据线程会停在此，如果hCom关闭会立即返回False
			Status = GetOverlappedResult(obj->m_hCom, &obj->m_ovWait, &Bytes, TRUE);
		}
		ClearCommError(obj->m_hCom, &Error, &cs);
		if (TRUE == Status //等待事件成功
			&& WaitEvent & EV_RXCHAR//缓存中有数据到达
			&& cs.cbInQue > 0)//有数据
		{
			Bytes = 0;
			obj->m_ovRead.Offset = 0;
			memset(ReadBuf, 0, sizeof(ReadBuf));
			//数据已经到达缓存区，ReadFile不会当成异步命令，而是立即读取并返回True
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
				//PostMessage(m_pMainWnd，UM_RECEIVE，&str，&Bytes);

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

//关闭串口
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
		WaitForSingleObject(m_Thread, 5000);//等待线程结束
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
	//para是你在 sqlite3_exec 里传入的 void * 参数
	//通过para参数，你可以传入一些特殊的指针（比如类指针、结构指针），然后在这里面强制转换成对应的类型（这里面是void*类型，必须强制转换成你的类型才可用）。然后操作这些数据
	//n_column是这一条记录有多少个字段 (即这条记录有多少列)
	// char ** column_value 是个关键值，查出来的数据都保存在这里，它实际上是个1维数组（不要以为是2维数组），每一个元素都是一个 char * 值，是一个字段内容（用字符串来表示，以/0结尾）
	//char ** column_name 跟 column_value是对应的，表示这个字段的字段名称
	//这里，我不使用 para 参数。忽略它的存在.
	LPWSTR lpStr = new WCHAR[1024];
	int i;
	CString str;
	swprintf_s(lpStr, 80, L"记录包含 %d 个字段\r\n", n_column);
	MessageBox(NULL, lpStr, L"PROMPT", MB_OK);
	for (i = 0; i < n_column; i++)
	{
		//swprintf_s(lpStr, 1000,L"字段名:%s->字段值:%s\r\n",column_name[i], column_value[i]);
		//str.Format(L"字段名:%s->字段值:%s\r\n", char2wchar(column_name[i]), char2wchar(column_value[i]));
		//UTF8ToUnicode(column_name[i], lpStr);
		//str = (CString)lpStr;
		str.Format(L"字段名:%s->字段值:%s\r\n", char2wchar(U2G(column_name[i])), U2T(column_value[i]));
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
	pDC->MoveTo(ix, iy);//起点
	ix += 500;
	pDC->LineTo(ix, iy);//终点
	iy += 300;
	pDC->LineTo(ix, iy);//终点
	ix -= 500;
	pDC->LineTo(ix, iy);//终点
	iy -= 300;
	pDC->LineTo(ix, iy);//终点
	*/
	CDC MemDC;           //首先定义一个显示设备对象
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