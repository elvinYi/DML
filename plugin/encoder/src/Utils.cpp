#include <atlbase.h>
#include <atlstr.h>
#include <vector>

#include "Utils.h"

IED_ENTRY

static void GetCpuInfo(CString &chProcessorName, CString &chProcessorType, DWORD &dwNum, DWORD &dwMaxClockSpeed) {
	CString strPath = _T("HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0");//ע����Ӽ�·��
	CRegKey regkey;//����ע��������
	LONG lResult;//LONG�ͱ�������Ӧ���
	lResult = regkey.Open(HKEY_LOCAL_MACHINE, LPCTSTR(strPath), KEY_ALL_ACCESS); //��ע����
	if (lResult != ERROR_SUCCESS)
		return;

	WCHAR chCPUName[50] = { 0 };
	DWORD dwSize = 50;

	//��ȡProcessorNameString�ֶ�ֵ
	if (ERROR_SUCCESS == regkey.QueryStringValue(_T("ProcessorNameString"), chCPUName, &dwSize))
		chProcessorName = chCPUName;

	//��ѯCPU��Ƶ
	DWORD dwValue;
	if (ERROR_SUCCESS == regkey.QueryDWORDValue(_T("~MHz"), dwValue))
		dwMaxClockSpeed = dwValue;
	regkey.Close();//�ر�ע���
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
	if (nNeedChars > 0)//�ٴ��ж�һ��
	{
		std::vector<char> temp(nNeedChars);
		::WideCharToMultiByte(CP_ACP, 0, wide.c_str(), -1, &temp[0], nNeedChars, 0, 0);
		return std::string(&temp[0]);
	}

	return std::string();
}

/*
* ��ȡϵͳ�汾����
* @param [in] һ��OSVERSIONINFO�ṹ������
* @return true��ʾ�ɹ���false��ʾʧ��
*/
BOOL GetOsVersion(OSVERSIONINFO & osInfo)
{
	osInfo.dwOSVersionInfoSize = sizeof(osInfo);
	return GetVersionEx(&osInfo);
}

/*
* �жϵ�ǰϵͳ�Ƿ����ĳ���汾�����ĳ���汾
* @param [in] һ��OS_VERSIONö��ֵ
* @return true��ʾ��Ȼ���ڣ�false��ʾ���Ȼ򲻴���
*/
bool IsWindowsVersionEqualOrLater(OS_VERSION const osv)
{
	///��ŵ�ǰ����ϵͳ�汾
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