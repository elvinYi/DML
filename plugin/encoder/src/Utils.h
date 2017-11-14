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

//请不要调整顺序
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
* 获取系统版本参数
* @param [in] 一个OSVERSIONINFO结构的引用
* @return TRUE表示成功，FALSE表示失败
*/
BOOL GetOsVersion(OSVERSIONINFO & osInfo);

/*
* 判断当前系统是否等于某个版本或大于某个版本
* @param [in] 一个OS_VERSION枚举值
* @return TRUE表示相等或大于，FALSE表示不等或不大于
*/
bool IsWindowsVersionEqualOrLater(OS_VERSION const osv);

IED_EXIT