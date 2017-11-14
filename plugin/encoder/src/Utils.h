#pragma once

#include <stdio.h>
#include <string>

#include <Windows.h>
#include <WinNT.h>

#include "PubConst.h"

IED_ENTRY

enum class CPUModel {
	UNKNOWN = 0,
	INTEL,
	AMD,
};

CPUModel GetCpuModel();

std::string UnicodeToAscii(const std::wstring& wide);

//�벻Ҫ����˳��
enum OS_VERSION
{
	OS_UNKNOWN = 0,
	//OS_WIN95 = 1,
	//OS_WIN98 = 2,
	//OS_WIN98SE = 3,
	//OS_WINME = 4,
	//OS_WINNT = 5,
	OS_WIN2000 = 6,
	OS_WINXP = 7,
	OS_WIN2003 = 8,
	//OS_WINXP64 = 9,
	OS_VISTA = 10,
	OS_WIN2008 = 11,
	OS_WIN2008R2 = 12,
	OS_WIN7 = 13,
	OS_WIN8 = 14,
	OS_WIN8_1 = 15,
	///Unknown high
	OS_UnKnownHigh,
};

/*
* ��ȡϵͳ�汾����
* @param [in] һ��OSVERSIONINFO�ṹ������
* @return TRUE��ʾ�ɹ���FALSE��ʾʧ��
*/
BOOL GetOsVersion(OSVERSIONINFO & osInfo);

/*
* �жϵ�ǰϵͳ�Ƿ����ĳ���汾�����ĳ���汾
* @param [in] һ��OS_VERSIONö��ֵ
* @return TRUE��ʾ��Ȼ���ڣ�FALSE��ʾ���Ȼ򲻴���
*/
bool IsWindowsVersionEqualOrLater(OS_VERSION const osv);

IED_EXIT