#include "pch.h"  
#include "WmiInfo.h"  
#include <string>        // 为用string这个必须
using namespace std; // 为用string这个必须
#pragma comment(lib, "rpcrt4.lib")  // UuidCreate - Minimum supported OS Win 2000

CWmiInfo::CWmiInfo(void)
{
	m_pWbemSvc = NULL;
	m_pWbemLoc = NULL;
	m_pEnumClsObj = NULL;
}

CWmiInfo::~CWmiInfo(void)
{
	m_pWbemSvc = NULL;
	m_pWbemLoc = NULL;
	m_pEnumClsObj = NULL;
}

HRESULT CWmiInfo::InitWmi()
{
	HRESULT hr;

	//一、初始化COM组件  
		//初始化COM  
	hr = ::CoInitializeEx(0, COINIT_MULTITHREADED);
	if (SUCCEEDED(hr) || RPC_E_CHANGED_MODE == hr)
	{
		//设置进程的安全级别，（调用COM组件时在初始化COM之后要调用CoInitializeSecurity设置进程安全级别，否则会被系统识别为病毒）  
		hr = CoInitializeSecurity(NULL,
			-1,
			NULL,
			NULL,
			RPC_C_AUTHN_LEVEL_PKT,
			RPC_C_IMP_LEVEL_IMPERSONATE,
			NULL,
			EOAC_NONE,
			NULL);
		//VERIFY(SUCCEEDED(hr));  

		//二、创建一个WMI命名空间连接  
		//创建一个CLSID_WbemLocator对象  
		hr = CoCreateInstance(CLSID_WbemLocator,
			0,
			CLSCTX_INPROC_SERVER,
			IID_IWbemLocator,
			(LPVOID*)&m_pWbemLoc);
		VERIFY(SUCCEEDED(hr));

		//使用m_pWbemLoc连接到"root\cimv2"并设置m_pWbemSvc的指针  
		hr = m_pWbemLoc->ConnectServer(CComBSTR(L"ROOT\\CIMV2"),
			NULL,
			NULL,
			0,
			NULL,
			0,
			0,
			&m_pWbemSvc);
		VERIFY(SUCCEEDED(hr));

		//三、设置WMI连接的安全性  
		hr = CoSetProxyBlanket(m_pWbemSvc,
			RPC_C_AUTHN_WINNT,
			RPC_C_AUTHZ_NONE,
			NULL,
			RPC_C_AUTHN_LEVEL_CALL,
			RPC_C_IMP_LEVEL_IMPERSONATE,
			NULL,
			EOAC_NONE);
		VERIFY(SUCCEEDED(hr));

	}
	return(hr);
}

HRESULT CWmiInfo::ReleaseWmi()
{
	HRESULT hr;

	if (NULL != m_pWbemSvc)
	{
		hr = m_pWbemSvc->Release();
	}
	if (NULL != m_pWbemLoc)
	{
		hr = m_pWbemLoc->Release();
	}
	if (NULL != m_pEnumClsObj)
	{
		hr = m_pEnumClsObj->Release();
	}

	::CoUninitialize();

	return(hr);
}

BOOL CWmiInfo::GetSingleItemInfo(CString ClassName, CString ClassMember, CString& chRetValue)
{
	USES_CONVERSION;

	CComBSTR query("SELECT * FROM ");
	VARIANT vtProp;
	ULONG uReturn;
	HRESULT hr;
	BOOL bRet = FALSE;

	if (NULL != m_pWbemSvc)
	{
		//查询类ClassName中的所有字段,保存到m_pEnumClsObj中  
		query += CComBSTR(ClassName);
		hr = m_pWbemSvc->ExecQuery(CComBSTR("WQL"), query, WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
			0, &m_pEnumClsObj);
		chRetValue = L"";
		if (SUCCEEDED(hr))
		{
			//初始化vtProp值  
			VariantInit(&vtProp);
			uReturn = 0;

			//返回从当前位置起的第一个对象到m_pWbemClsObj中  
			do {
				CString temp;
				hr = m_pEnumClsObj->Next(WBEM_INFINITE, 1, &m_pWbemClsObj, &uReturn);
				if (SUCCEEDED(hr) && uReturn > 0)
				{
					//从m_pWbemClsObj中找出ClassMember标识的成员属性值,并保存到vtProp变量中  
					hr = m_pWbemClsObj->Get(CComBSTR(ClassMember), 0, &vtProp, 0, 0);
					if (SUCCEEDED(hr))
					{
						//VariantToString(&vtProp, chRetValue);
						VariantToString(&vtProp, temp);
						VariantClear(&vtProp);//清空vtProp  
						bRet = TRUE;
						chRetValue += temp + L";";
					}
				}
			} while (uReturn > 0);
		}
	}
	if (NULL != m_pEnumClsObj)
	{
		hr = m_pEnumClsObj->Release();
		m_pEnumClsObj = NULL;
	}
	if (NULL != m_pWbemClsObj)
	{
		hr = m_pWbemClsObj->Release();
		m_pWbemClsObj = NULL;
	}
	return bRet;
}

BOOL CWmiInfo::GetGroupItemInfo(CString ClassName, CString ClassMember[], int n, CString& chRetValue)
{
	USES_CONVERSION;

	CComBSTR query("SELECT * FROM ");
	CString result, info;
	VARIANT vtProp;
	ULONG uReturn;
	HRESULT hr;
	int i;
	BOOL bRet = FALSE;
	if (NULL != m_pWbemSvc)
	{
		query += CComBSTR(ClassName);
		hr = m_pWbemSvc->ExecQuery(CComBSTR("WQL"), query, WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, 0, &m_pEnumClsObj);
		if (SUCCEEDED(hr))
		{
			VariantInit(&vtProp); //初始化vtProp变量  
			if (m_pEnumClsObj)
			{
				Sleep(10);
				uReturn = 0;
				hr = m_pEnumClsObj->Next(WBEM_INFINITE, 1, &m_pWbemClsObj, &uReturn);
				if (SUCCEEDED(hr) && uReturn > 0)
				{
					for (i = 0; i < n; ++i)
					{
						hr = m_pWbemClsObj->Get(CComBSTR(ClassMember[i]), 0, &vtProp, 0, 0);
						if (SUCCEEDED(hr))
						{
							VariantToString(&vtProp, info);
							chRetValue += info + _T("\t");
							VariantClear(&vtProp);
							bRet = TRUE;
						}
					}
					chRetValue += _T("\r\n");
				}
			}
		}
	}

	if (NULL != m_pEnumClsObj)
	{
		hr = m_pEnumClsObj->Release();
		m_pEnumClsObj = NULL;
	}
	if (NULL != m_pWbemClsObj)
	{
		hr = m_pWbemClsObj->Release();
		m_pWbemClsObj = NULL;
	}
	return bRet;
}

void CWmiInfo::VariantToString(const LPVARIANT pVar, CString& chRetValue) const
{
	USES_CONVERSION;

	CComBSTR HUGEP* pBstr;
	BYTE HUGEP* pBuf;
	LONG low, high, i;
	HRESULT hr;

	switch (pVar->vt)
	{
	case VT_BSTR:
	{
		chRetValue = W2T(pVar->bstrVal);
	}
	break;
	case VT_BOOL:
	{
		if (VARIANT_TRUE == pVar->boolVal)
			chRetValue = "是";
		else
			chRetValue = "否";
	}
	break;
	case VT_I4:
	{
		chRetValue.Format(_T("%d"), pVar->lVal);
	}
	break;
	case VT_UI1:
	{
		chRetValue.Format(_T("%d"), pVar->bVal);
	}
	break;
	case VT_UI4:
	{
		chRetValue.Format(_T("%d"), pVar->ulVal);
	}
	break;

	case VT_BSTR | VT_ARRAY:
	{
		hr = SafeArrayAccessData(pVar->parray, (void HUGEP**) & pBstr);
		hr = SafeArrayUnaccessData(pVar->parray);
		chRetValue = W2T(pBstr->m_str);
	}
	break;

	case VT_I4 | VT_ARRAY:
	{
		SafeArrayGetLBound(pVar->parray, 1, &low);
		SafeArrayGetUBound(pVar->parray, 1, &high);

		hr = SafeArrayAccessData(pVar->parray, (void HUGEP**) & pBuf);
		hr = SafeArrayUnaccessData(pVar->parray);
		CString strTmp;
		high = min(high, MAX_PATH * 2 - 1);
		for (i = low; i <= high; ++i)
		{
			strTmp.Format(_T("%02X"), pBuf[i]);
			chRetValue += strTmp;
		}
	}
	break;
	default:
		break;
	}
}

CString getGUID()
{
	GUID   guid;
	CString   szGUID;

	if (S_OK == ::CoCreateGuid(&guid))
	{
		szGUID.Format(L"{%08X-%04X-%04x-%02X%02X-%02X%02X%02X%02X%02X%02X} "
			, guid.Data1
			, guid.Data2
			, guid.Data3
			, guid.Data4[0], guid.Data4[1]
			, guid.Data4[2], guid.Data4[3], guid.Data4[4], guid.Data4[5]
			, guid.Data4[6], guid.Data4[7]
		);
	}
	return szGUID;
}

CString getUUID()
{
	UUID uuidRoute;
	RPC_STATUS st;

	CString str = L"12345";
	BSTR bstrName;
	bstrName = SysAllocString(L"JingWei Lion");
	str = bstrName;
	UuidCreate(&uuidRoute);
	//str.Format(L"{%08X-%04X-%04x-%02X%02X-%02X%02X%02X%02X%02X%02X} "
	//	, uuidRoute.Data1
	//	, uuidRoute.Data2
	//	, uuidRoute.Data3
	//	, uuidRoute.Data4[0], uuidRoute.Data4[1]
	//	, uuidRoute.Data4[2], uuidRoute.Data4[3], uuidRoute.Data4[4], uuidRoute.Data4[5]
	//	, uuidRoute.Data4[6], uuidRoute.Data4[7]
	//);
	//st = ::UuidCreate(&uuidRoute);
	//if (st != RPC_S_OK)
	//{
	//	//continue;
	//}


	//wchar_t * pszGuid = new wchar_t [200];
	//UuidToString(&uuidRoute, (RPC_WSTR*)pszGuid);
	char* pszGuid = new char[200];
	UuidToStringA(&uuidRoute, (RPC_CSTR*)&pszGuid);
	//_bstr_t bstrUUID = _bstr_t((char*)pszGuid);
	//CString strTemp;
	//str = L"";
	//for (int i = 0; i < 12; i++)
	//{
	//	strTemp.Format(L"%02X", 0xFF & pszGuid[i]);
	//	str += strTemp;
	//}
	str = pszGuid;
	//RpcStringFree((RPC_WSTR*)pszGuid);

	return str;
}


BOOL GetCpuByCmd(char* lpszCpu, int len/*=128*/)
{
	const long MAX_COMMAND_SIZE = 10000; // 命令行输出缓冲大小	
	//WCHAR szFetCmd[] = L"wmic cpu get processorid"; // 获取CPU序列号命令行	
	//const string strEnSearch = "ProcessorId"; // CPU序列号的前导信息
	WCHAR szFetCmd[] = L"wmic CSPRODUCT GET UUID"; // 获取CPU序列号命令行	
	const string strEnSearch = "UUID"; // CPU序列号的前导信息

	BOOL   bret = FALSE;
	HANDLE hReadPipe = NULL; //读取管道
	HANDLE hWritePipe = NULL; //写入管道	
	PROCESS_INFORMATION pi;   //进程信息	
	STARTUPINFO			si;	  //控制命令行窗口信息
	SECURITY_ATTRIBUTES sa;   //安全属性
	memset(lpszCpu, 0, len);
	char			szBuffer[MAX_COMMAND_SIZE + 1] = { 0 }; // 放置命令行结果的输出缓冲区
	string			strBuffer;
	unsigned long	count = 0;
	long			ipos = 0;

	memset(&pi, 0, sizeof(pi));
	memset(&si, 0, sizeof(si));
	memset(&sa, 0, sizeof(sa));

	pi.hProcess = NULL;
	pi.hThread = NULL;
	si.cb = sizeof(STARTUPINFO);
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.lpSecurityDescriptor = NULL;
	sa.bInheritHandle = TRUE;

	//1.0 创建管道
	bret = CreatePipe(&hReadPipe, &hWritePipe, &sa, 0);
	if (!bret)
	{
		goto END;
	}

	//2.0 设置命令行窗口的信息为指定的读写管道
	GetStartupInfo(&si);
	si.hStdError = hWritePipe;
	si.hStdOutput = hWritePipe;
	si.wShowWindow = SW_HIDE; //隐藏命令行窗口
	si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;

	//3.0 创建获取命令行的进程
	bret = CreateProcess(NULL, szFetCmd, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi);
	if (!bret)
	{
		goto END;
	}

	//4.0 读取返回的数据
	WaitForSingleObject(pi.hProcess, 500/*INFINITE*/);
	bret = ReadFile(hReadPipe, szBuffer, MAX_COMMAND_SIZE, &count, 0);
	if (!bret)
	{
		goto END;
	}

	//5.0 查找CPU序列号
	bret = FALSE;
	strBuffer = szBuffer;
	ipos = strBuffer.find(strEnSearch);

	if (ipos < 0) // 没有找到
	{
		goto END;
	}
	else
	{
		strBuffer = strBuffer.substr(ipos + strEnSearch.length());
	}

	memset(szBuffer, 0x00, sizeof(szBuffer));
	strcpy_s(szBuffer, strBuffer.c_str());

	//去掉中间的空格 \r \n
	int j = 0;
	for (int i = 0; i < strlen(szBuffer); i++)
	{
		if (szBuffer[i] != ' ' && szBuffer[i] != '\n' && szBuffer[i] != '\r')
		{
			lpszCpu[j] = szBuffer[i] & 0xFF;
			j++;
		}
	}

	bret = TRUE;

END:
	//关闭所有的句柄
	CloseHandle(hWritePipe);
	CloseHandle(hReadPipe);
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);

	return(bret);


}


