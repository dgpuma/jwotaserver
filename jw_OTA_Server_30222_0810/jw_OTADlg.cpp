
// jw_OTADlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "jw_OTA.h"
#include "jw_OTADlg.h"
#include "afxdialogex.h"
#include "hex.h"
#include "Lion.h"
#include "crc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CFont fontEdit1;
CjwOTADlg* dlg;
// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)	
END_MESSAGE_MAP()


// CjwOTADlg 对话框



CjwOTADlg::CjwOTADlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_JW_OTA_DIALOG, pParent)
	, m_sEdit1(_T(""))
	, m_sEditFilepath(_T(""))
	, m_iSlider1(0)
	, m_sEditAddrStart(_T(""))
	, m_sEditAddrStop(_T(""))
	, m_sEdit3(_T(""))
	, m_sEdit4(_T(""))
	, m_sEdit5(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	dlg = this;
}

void CjwOTADlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_Edit1);
	DDX_Text(pDX, IDC_EDIT1, m_sEdit1);
	DDX_Control(pDX, IDC_EDITFILEPATH, m_EditFilepath);
	DDX_Text(pDX, IDC_EDITFILEPATH, m_sEditFilepath);
	DDX_Control(pDX, IDC_COMBO_PORT, m_comboPort);
	DDX_Control(pDX, IDC_SLIDER1, m_Sleder1);
	DDX_Slider(pDX, IDC_SLIDER1, m_iSlider1);
	DDV_MinMaxInt(pDX, m_iSlider1, 0, 28);
	DDX_Control(pDX, IDOK, m_Btn_Ok);
	DDX_Control(pDX, IDC_BTN_SAVE, m_Btn_Save);
	DDX_Control(pDX, IDC_EDIT_ADDRSTART, m_EditAddrStart);
	DDX_Text(pDX, IDC_EDIT_ADDRSTART, m_sEditAddrStart);
	DDX_Control(pDX, IDC_EDIT_ADDRSTOP, m_EditAddrStop);
	DDX_Text(pDX, IDC_EDIT_ADDRSTOP, m_sEditAddrStop);
	DDX_Control(pDX, IDC_EDIT3, m_Edit3);
	DDX_Text(pDX, IDC_EDIT3, m_sEdit3);
	DDX_Control(pDX, IDC_EDIT4, m_Edit4);
	DDX_Text(pDX, IDC_EDIT4, m_sEdit4);
	DDX_Control(pDX, IDC_EDIT5, m_Edit5);
	DDX_Text(pDX, IDC_EDIT5, m_sEdit5);
}

BEGIN_MESSAGE_MAP(CjwOTADlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_OPENFILE, &CjwOTADlg::OnBnClickedBtnOpenfile)
	ON_BN_CLICKED(IDC_BTN_PORT, &CjwOTADlg::OnBnClickedBtnPort)	
	ON_MESSAGE(UM_RECEIVE, &CjwOTADlg::OnReceive)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDOK, &CjwOTADlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CjwOTADlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BTN_SAVE, &CjwOTADlg::OnBnClickedBtnSave)
	ON_BN_CLICKED(IDC_BTN_CALC, &CjwOTADlg::OnBnClickedBtnCalc)
END_MESSAGE_MAP()


// CjwOTADlg 消息处理程序

BOOL CjwOTADlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	ShowWindow(SW_MINIMIZE);

	// TODO: 在此添加额外的初始化代码
	EnumCommPort(&m_comboPort);
	m_hComAsy = new ComAsy();

	{
		fontEdit1.DeleteObject();
		fontEdit1.CreatePointFont(-100, _T("新宋体"));//280,180,160   -220		
		GetDlgItem(IDC_EDIT1)->SetFont(&fontEdit1);
	}

	{	
		GetDlgItem(IDC_EDIT3)->SetFont(&fontEdit1);
		GetDlgItem(IDC_EDIT4)->SetFont(&fontEdit1);
		GetDlgItem(IDC_EDIT5)->SetFont(&fontEdit1);
	}
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CjwOTADlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CjwOTADlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CjwOTADlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CjwOTADlg::OnBnClickedBtnOpenfile()
{
	// TODO: 在此添加控件通知处理程序代码
	m_sEdit1 = L"0000\r\n0010\r\n";
	//CFileDialog log(TRUE, _T("文件"), NULL, OFN_HIDEREADONLY, _T("HEX文件(*.hex)|*.hex|所有文件(*.*)|*.*|"), NULL);
	CFileDialog log(TRUE, _T("文件"), NULL, OFN_HIDEREADONLY, _T("HEX文件(*.hex)|*.hex|BIN文件(*.bin)|*.bin|"), NULL);
	if (log.DoModal() == IDOK)
	{
		//pathname = log.GetPathName();
		//strname = log.GetFileName();
		//m_FileName.SetWindowText(pathname);
		//m_sEdit1 = log.GetPathName();
		CString str = log.GetPathName();
		
		LoadFile(str);
		//LoadFile(m_sEditFilepath);
		//LoadFile(log.GetPathName());
		//ShowFlash(flashbuffer, flashsize);
		m_sEditFilepath = log.GetPathName();
		//m_sEditFilepath = log.GetFileExt().MakeUpper();
	}
	//if (m_ComState == __COM_FREE)
	//{
	//	
	//	int len = 256;
	//	m_hComAsy->ComWrite((LPBYTE)flashbuffer, len);
	//}
	
	UpdateData(false);
}



void CjwOTADlg::EnumCommPort(CComboBox* pComboBox) //CListBox 
{
	HKEY hKey;
	if (ERROR_SUCCESS == ::RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"Hardware\\DeviceMap\\SerialComm", NULL, KEY_READ, &hKey))//打开串口注册表对应的键值
	{
		int i = 0;
		TCHAR RegKeyName[128], SerialPortName[128];
		DWORD dwLong, dwSize;
		pComboBox->Clear();
		while (TRUE)
		{
			dwLong = dwSize = sizeof(RegKeyName);
			if (ERROR_NO_MORE_ITEMS == ::RegEnumValue(hKey, i, RegKeyName, &dwLong, NULL, NULL, (PUCHAR)SerialPortName, &dwSize))//枚举串口 
			{
				break;
			}
			//if (!memcmp(RegKeyName, "\\Device\\", 8))//过滤虚拟串口
			{
				pComboBox->AddString(SerialPortName); //SerialPortName就是串口名字
			}
			i++;
		}
		if (pComboBox->GetCount() == 0)
		{
			MessageBox(L"在HKEY_LOCAL_MACHINE:Hardware\\DeviceMap\\SerialComm里找不到串口!!!");
		}
		RegCloseKey(hKey);
	}
	else
	{
		RegCloseKey(hKey);
		MessageBox(L"您的计算机的注册表上没有HKEY_LOCAL_MACHINE:Hardware\\DeviceMap\\SerialComm项");

	}

	if (pComboBox->GetCount() == 0)
	{
		for (int i = 1; i < 10; i++)
		{
			TCHAR SerialPortName[128];
			swprintf_s(SerialPortName, 128, L"COM%d", i);
			pComboBox->AddString(SerialPortName); //SerialPortName就是串口名字
			pComboBox->SetCurSel(0);
		}
	}
	pComboBox->SetCurSel(0);
}

void CjwOTADlg::OnBnClickedBtnPort()
{
	// TODO: 在此添加控件通知处理程序代码

	CString strPort;
	GetDlgItemText(IDC_COMBO_PORT, strPort);
	//::WritePrivateProfileStringW(_T("COM"), _T("PORT"), strPort, strFileSetting);
	strPort = L"\\\\.\\" + strPort;
	//AfxMessageBox(strPort);
	iBaud = 9600;
	if (__COM_DISABLE == m_ComState)
	{
		if (m_hComAsy->InitCOM(strPort, iBaud))
		{
			//MessageBox(strPort, L"提示", MB_OK);
			m_hComAsy->purge();
			
			m_comboPort.EnableWindow(FALSE);
			
			UpdateData(false);
			
			iRecvCount = 0;
			
			SetDlgItemText(IDC_BTN_PORT, L"ClosePort");
			m_Btn_Ok.EnableWindow(TRUE);
			m_ComState = __COM_FREE;
		}
		else AfxMessageBox(L"Open COM Port fail!");
	}
	else
	{
		bRun = false;
		m_hComAsy->UninitCOM();		
		m_comboPort.EnableWindow(TRUE);		
		UpdateData(false);
		SetDlgItemText(IDC_BTN_PORT, L"OpenPort");
		m_Btn_Ok.EnableWindow(FALSE);
		m_ComState = __COM_DISABLE;
	}
}

#define _CMD_GETVER 0xFF

afx_msg LRESULT CjwOTADlg::OnReceive(WPARAM wParam, LPARAM lParam)
{
	CString strTemp;
	int Len = (int)wParam;
	BYTE* Buf = (BYTE*)lParam;
	m_sEdit1 += L"\r\n";
	unsigned short cks = 0;
	unsigned char sendbuf[260] = { 0 };
	unsigned short Addr = 0;
	for (int i = 0; i < Len; i++) 
	{
		strTemp.Format(L"%02X ", Buf[i]);
		m_sEdit1 += strTemp;
	}
	if (Buf[0] == 0xAA)
	{
		cks = calcks(Buf, Len-2);
		strTemp.Format(L"=%02X %02x", cks>>8, cks&0xFF);
		m_sEdit1 += strTemp;
		if (m_ComState == __COM_FREE)
		{
			Addr = 0x100 * Buf[1];
			if (Buf[Len - 2] == (cks >> 8) && Buf[Len - 1] == (cks & 0xFF))
			{
				sendbuf[0] = 0xA5;
				sendbuf[1] = Buf[1];
				if (Buf[1] < 0x70)
				{
					for (int i = 0; i < 0x100; i++)
						sendbuf[2 + i] = flashbuffer[Addr++];
				}
				else
				{
					for (int i = 0; i < 0x100; i++)
					{
						sendbuf[2 + i] = 0xFF;
					}
					if (Buf[1] == _CMD_GETVER)
					{						
						sendbuf[2] = flashbuffer[0x6800];
						sendbuf[3] = flashbuffer[0x6801];
					}
				}
				cks = calcks(sendbuf, 258);
				sendbuf[258] = cks >> 8;
				sendbuf[259] = cks & 0xFF;
				Len = 260;
				m_hComAsy->ComWrite(sendbuf, Len);
			}
			else
			{
				sendbuf[0] = 0xA5;
				sendbuf[1] = Buf[Len - 2];
				sendbuf[2] = Buf[Len - 1];
				sendbuf[3] = cks>>8;
				sendbuf[4] = cks&0xFF;
				Len = 5;
				m_hComAsy->ComWrite(sendbuf, Len);
			}
		}
	}
	UpdateData(false);
	return 0;
}



void CjwOTADlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CDialogEx::OnTimer(nIDEvent);
}


void CjwOTADlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	unsigned char sendbuf[7] = {0xFC, 0x23, 0x03, 0xFF, 0x01, 0x01, 0x23 };
	int Len = 7;
	m_hComAsy->ComWrite(sendbuf, Len);
//CDialogEx::OnOK();
}


void CjwOTADlg::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	CDialogEx::OnCancel();
}


void CjwOTADlg::OnBnClickedBtnSave()
{
	// TODO: 
	if (m_sEditFilepath.GetLength() > 0)
	{
		UpdateData(true);
		m_Btn_Save.EnableWindow(FALSE);
		CString strFileName = m_sEditFilepath.Left(m_sEditFilepath.GetLength() - 3) + _T("OTA");
		long LAddrStart = wcstol(m_sEditAddrStart, NULL, 16);
		long LAddrStop = wcstol(m_sEditAddrStop, NULL, 16);
		TransFlash(flashbuffer, flashbufferT, 0x8000);
		SaveFile(strFileName, LAddrStart, LAddrStop);
		m_Btn_Save.EnableWindow(TRUE);

	}
	//MessageBox(strFileName);
}


void CjwOTADlg::OnBnClickedBtnCalc()
{
	// TODO: 在此添加控件通知处理程序代码
	CStringArray StrArry;
	//StrArry.Add(L"First");
	//StrArry.Add(L"Second");
	//StrArry.Add(L"Third");
	int i = 0;
	int j = 0;
	int Len = 0;
	int iCks = 0;
	CString strTemp, strTarget;
	unsigned char* tBuf;
	UpdateData(true);
	strTarget = m_sEdit1;
	strTarget = m_sEdit1.Trim().MakeUpper();
	strTarget.Replace(L"X", L"0");
	strTarget.Replace(L",", L" ");
	Len=strSplit(strTarget, _T(" "), StrArry);
	tBuf = new unsigned char[Len];
	strTarget = L"";
	i = Len;
	while (i--)
	{
		strTemp = StrArry[j].MakeUpper();
		const char* ss = wchar2char(strTemp);
		tBuf[j]= strtol(ss, NULL, 16);		
		strTemp.Format(L"%02X ", strtol(ss,NULL,16));
		strTarget += strTemp;
		j++;
	}
	
	iCks = calcks((unsigned char*)tBuf,Len);
	strTemp.Format(L"%02X", iCks & 0xFF);
	m_sEdit3 = strTarget + strTemp;

	iCks = calxor((unsigned char*)tBuf,Len);
	strTemp.Format(L"%02X", iCks);
	m_sEdit4 = strTarget + strTemp;

	iCks = Crc16Check((unsigned char*)tBuf, Len);
	strTemp.Format(L"%02X %02X", iCks>>8, iCks&0xFF);
	m_sEdit5 = strTarget + strTemp;
	UpdateData(false);
}
