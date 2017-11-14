#include <atlbase.h>
#include <atlstr.h>
#include <vector>

#include "Utils.h"

IED_ENTRY

static void GetCpuInfo(CString &chProcessorName, CString &chProcessorType, DWORD &dwNum, DWORD &dwMaxClockSpeed) {
	CString strPath = _T("HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0");//注册表子键路径
	CRegKey regkey;//定义注册表类对象
	LONG lResult;//LONG型变量－反应结果
	lResult = regkey.Open(HKEY_LOCAL_MACHINE, LPCTSTR(strPath), KEY_ALL_ACCESS); //打开注册表键
	if (lResult != ERROR_SUCCESS)
		return;

	WCHAR chCPUName[50] = { 0 };
	DWORD dwSize = 50;

	//获取ProcessorNameString字段值
	if (ERROR_SUCCESS == regkey.QueryStringValue(_T("ProcessorNameString"), chCPUName, &dwSize))
		chProcessorName = chCPUName;

	//查询CPU主频
	DWORD dwValue;
	if (ERROR_SUCCESS == regkey.QueryDWORDValue(_T("~MHz"), dwValue))
		dwMaxClockSpeed = dwValue;
	regkey.Close();//关闭注册表
}

CPUModel GetCpuModel() {
	CString s1;
	CString s2;
	DWORD d1;
	DWORD d2;
	GetCpuInfo(s1, s2, d1, d2);

	if (-1 != s1.Find(L"Intel")) {
		// find intel cpu
		return CPUModel::INTEL;
	}

	if (-1 != s1.Find(L"AMD")) {
		return CPUModel::AMD;
	}

	return CPUModel::UNKNOWN;
}

std::string UnicodeToAscii(const std::wstring& wide)
{
	int nNeedChars = WideCharToMultiByte(CP_ACP, 0, wide.c_str(), -1, 0, 0, 0, 0);
	if (nNeedChars > 0)//再次判断一下
	{
		std::vector<char> temp(nNeedChars);
		::WideCharToMultiByte(CP_ACP, 0, wide.c_str(), -1, &temp[0], nNeedChars, 0, 0);
		return std::string(&temp[0]);
	}

	return std::string();
}

/*
* 获取系统版本参数
* @param [in] 一个OSVERSIONINFO结构的引用
* @return true表示成功，false表示失败
*/
BOOL GetOsVersion(OSVERSIONINFO & osInfo)
{
	osInfo.dwOSVersionInfoSize = sizeof(osInfo);
	return GetVersionEx(&osInfo);
}

/*
* 判断当前系统是否等于某个版本或大于某个版本
* @param [in] 一个OS_VERSION枚举值
* @return true表示相等或大于，false表示不等或不大于
*/
bool IsWindowsVersionEqualOrLater(OS_VERSION const osv)
{
	///存放当前操作系统版本
	OSVERSIONINFO osInfoCur;
	if (GetOsVersion(osInfoCur) == TRUE)
	{
		if (osInfoCur.dwMajorVersion >= 7)
		{
			return true;
		}
		else if (osInfoCur.dwMajorVersion == 6)
		{
			if ((osv == OS_WIN8_1) && (osInfoCur.dwMinorVersion >= 3))
			{
				return true;
			}
			else if ((osv == OS_WIN8) && (osInfoCur.dwMinorVersion >= 2))
			{
				return true;
			}
			else if ((osv == OS_WIN7 || osv == OS_WIN2008R2)
				&& osInfoCur.dwMinorVersion >= 1)
			{
				return true;
			}
			else if ((osv == OS_VISTA || osv == OS_WIN2008)
				&& osInfoCur.dwMinorVersion >= 0)
			{
				return true;
			}
			else
			{
				return false;
			}
		}
		else if (osInfoCur.dwMajorVersion == 5)
		{
			if (osv == OS_WIN2003
				&& osInfoCur.dwMinorVersion >= 2)
			{
				return true;
			}
			else if (osv == OS_WINXP
				&& osInfoCur.dwMinorVersion >= 1)
			{
				return true;
			}
			else if (osv == OS_WIN2000
				&& osInfoCur.dwMinorVersion >= 0)
			{
				return true;
			}
			else
			{
				return false;
			}
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}

IED_EXIT