#pragma once
#ifndef _LION_H
#define _LION_H

#define __COM_DISABLE	0
#define __COM_FREE		1
#define __COM_ISDATA	2
#define __COM_READ		3
#define __COM_WRITE		4

#define UM_RECEIVE WM_USER + 100

#include "strcon.h"
#include "WMIInfo.h"
#include "LionString.h"
class LionThread :public CWinThread
{
public:
	LionThread();
	~LionThread();
	HANDLE	handle;
	DWORD	ThreadId;
	//void TestFunc();
	static int ThreadFunc(LPVOID pParam);
protected:
	int ThreadFuncKernel();
private:
};


class ComAsy
{
public:
	ComAsy();
	~ComAsy();
	bool InitCOM(LPCTSTR Port, int iBaud);//打开串口
	void UninitCOM(); //关闭串口并清理

	//写入数据
	bool ComWrite(LPBYTE buf, int& len);

	//读取线程
	static unsigned int __stdcall OnRecv(void*);
	void purge();

private:

	HANDLE m_hCom;
	OVERLAPPED m_ovWrite;//用于写入数据
	OVERLAPPED m_ovRead;//用于读取数据
	OVERLAPPED m_ovWait;//用于等待数据
	volatile bool m_IsOpen;//串口是否打开
	HANDLE m_Thread;//读取线程句柄

};
//int recvlen;
extern HANDLE hCom;
extern volatile char m_ComState;
extern ComAsy* m_hComAsy;

//HANDLE InitCOM(LPCTSTR Port);
void ThreadFunc();
//bool ComRead(HANDLE hCom, LPBYTE buf, int& len);
//bool ComWrite(HANDLE hCom, LPBYTE buf, int& len);

void DrawLabel(CDC* DC, int ix_offset, int iy_offset);
int LoadMyInfo(void* para, int n_column, char** column_value, char** column_name);
void GetGuid();
#endif
