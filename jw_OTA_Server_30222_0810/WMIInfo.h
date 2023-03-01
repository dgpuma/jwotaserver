#pragma once

#include <atlbase.h>  
#include <afxpriv.h>  
#include <WbemIdl.h>  
#pragma comment(lib,"WbemUuid.lib")  

class CWmiInfo
{
public:
	CWmiInfo(void);
	~CWmiInfo(void);

public:
	HRESULT InitWmi();    //初始化WMI  
	HRESULT ReleaseWmi(); //释放  


	BOOL GetSingleItemInfo(CString, CString, CString&);
	BOOL GetGroupItemInfo(CString, CString[], int, CString&);

private:
	void VariantToString(const LPVARIANT, CString&) const;//将Variant类型的变量转换为CString  
private:
	IEnumWbemClassObject* m_pEnumClsObj;
	IWbemClassObject* m_pWbemClsObj;
	IWbemServices* m_pWbemSvc;
	IWbemLocator* m_pWbemLoc;
};

//上面就是一个WMI的封装，返回参数均以CString类型返回调用顺序：
//
//1.实例化一个CWmiInfo类对象
//
//2.调用InitWmi()函数初始化COM组件等。。。函数中有说明必须调用InitWmi初始化WMI组件，否则所有调用都无意义。
//
//3.调用GetXXX方法获取你要获取的字段值。
//
//4.最后使用完CWmiInfo类对象之后一定要记得调用ReleaseWmi()函数来释放资源

/*GetSingleItemInfo()
第一个参数为要查询的类名如"Win32_Processor"表示CPU类,
第二个参数表示要查的类的成员,如"Caption"表示查询CPU的名称
第三个参数表示返回值.故可以这样调用
CString strRetValue;
GetSingleItemInfo(_T("Win32_Processor"),_T("Caption"),strRetValue);
一样调用成功后即可从strRetValue参数中读取出CPU的名称*/
/*GetGroupItemInfo 函数与GetSingleItemInfo类似,不同的是GetSingleItemInfo一次只能查一个类成员而GetGroupItemInfo一次可以查询一个类的多个成员.
GetGroupItemInfo的第三个参数表示要查询的类成员的个数,即:第二个参数CString数组的大小
可以这样调用：CString strRetValue;
CString [] strClassMem = {_T("Caption"),
	_T("CurrentClockSpeed"),
	_T("DeviceID"),
	_T("Manufacturer"),
	_T("Manufacturer")
};

GetGroupItemInfo(_T("Win32_Processor"),strClassMem,5,strRetValue);*/



//// 硬件信息类名  
//Win32_Processor, // CPU 处理器  
//Win32_PhysicalMemory, // 物理内存条  
//Win32_Keyboard, // 键盘  
//Win32_PointingDevice, // 点输入设备，包括鼠标。  
//Win32_FloppyDrive, // 软盘驱动器  
//Win32_DiskDrive, // 硬盘驱动器  
//Win32_CDROMDrive, // 光盘驱动器  
//Win32_BaseBoard, // 主板  
//Win32_BIOS, // BIOS 芯片  
//Win32_ParallelPort, // 并口  
//Win32_SerialPort, // 串口  
//Win32_SerialPortConfiguration, // 串口配置  
//Win32_SoundDevice, // 多媒体设置，一般指声卡。  
//Win32_SystemSlot, // 主板插槽 (ISA & PCI & AGP)  
//Win32_USBController, // USB 控制器  
//Win32_NetworkAdapter, // 网络适配器  
//Win32_NetworkAdapterConfiguration, // 网络适配器设置  
//Win32_Printer, // 打印机  
//Win32_PrinterConfiguration, // 打印机设置  
//Win32_PrintJob, // 打印机任务  
//Win32_TCPIPPrinterPort, // 打印机端口  
//Win32_POTSModem, // MODEM  
//Win32_POTSModemToSerialPort, // MODEM 端口  
//Win32_DesktopMonitor, // 显示器  
//Win32_DisplayConfiguration, // 显卡  
//Win32_DisplayControllerConfiguration, // 显卡设置  
//Win32_VideoController, // 显卡细节。  
//Win32_VideoSettings, // 显卡支持的显示模式。  
//
//// 操作系统  
//Win32_TimeZone, // 时区  
//Win32_SystemDriver, // 驱动程序  
//Win32_DiskPartition, // 磁盘分区  
//Win32_LogicalDisk, // 逻辑磁盘  
//Win32_LogicalDiskToPartition, // 逻辑磁盘所在分区及始末位置。  
//Win32_LogicalMemoryConfiguration, // 逻辑内存配置  
//Win32_PageFile, // 系统页文件信息  
//Win32_PageFileSetting, // 页文件设置  
//Win32_BootConfiguration, // 系统启动配置  
//Win32_ComputerSystem, // 计算机信息简要  
//Win32_OperatingSystem, // 操作系统信息  
//Win32_StartupCommand, // 系统自动启动程序  
//Win32_Service, // 系统安装的服务  
//Win32_Group, // 系统管理组  
//Win32_GroupUser, // 系统组帐号  
//Win32_UserAccount, // 用户帐号  
//Win32_Process, // 系统进程  
//Win32_Thread, // 系统线程  
//Win32_Share, // 共享  
//Win32_NetworkClient, // 已安装的网络客户端  
//Win32_NetworkProtocol, // 已安装的网络协议  
CString getUUID();
CString getGUID();



BOOL GetCpuByCmd(char* lpszCpu, int len = 128);
