
// jw_OTADlg.h: 头文件
//

#pragma once


// CjwOTADlg 对话框
class CjwOTADlg : public CDialogEx
{
// 构造
public:
	CjwOTADlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_JW_OTA_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnOpenfile();
	CEdit m_Edit1;
	CString m_sEdit1;
	CEdit m_EditFilepath;
	CString m_sEditFilepath;
	afx_msg void OnBnClickedBtnPort();
	CComboBox m_comboPort;
	BOOL bRun;
	int iBaud;
	volatile int iRecvCount;
	void EnumCommPort(CComboBox* pComboBox);
protected:
	afx_msg LRESULT OnReceive(WPARAM wParam, LPARAM lParam);
public:
	CSliderCtrl m_Sleder1;
	int m_iSlider1;
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	CButton m_Btn_Ok;
	CButton m_Btn_Save;
	afx_msg void OnBnClickedBtnSave();
	CEdit m_EditAddrStart;
	CString m_sEditAddrStart;
	CEdit m_EditAddrStop;
	CString m_sEditAddrStop;
	CEdit m_Edit3;
	CString m_sEdit3;
	afx_msg void OnBnClickedBtnCalc();
	CEdit m_Edit4;
	CString m_sEdit4;
	CEdit m_Edit5;
	CString m_sEdit5;
};

extern CjwOTADlg *dlg;

