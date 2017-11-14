#include "Log.h"

#if defined(ANDROID)
#include <android/log.h>
#else
#include <Windows.h>
#include <time.h>
#endif

#include <stdio.h>
#include <stdarg.h>
#include <string>
#include <vector>
#include <tchar.h>

#if defined(ANDROID)
const char *Game_RR_Tag = "Game_RR";
#endif

static std::wstring AsciiToUnicode(const std::string& in_str)
{
	int nNeedWchars = MultiByteToWideChar(CP_ACP, 0, in_str.c_str(), -1, NULL, 0);
	if (nNeedWchars > 0)
	{
		std::vector<wchar_t> temp(nNeedWchars);
		::MultiByteToWideChar( CP_ACP, 0, in_str.c_str(), -1, &temp[0], nNeedWchars );
		return std::wstring(&temp[0]);
	}

	return std::wstring();
}

Log* Log::self = nullptr;

Log::Log() {
	fp = NULL;
}

Log::~Log() {
	if (fp) {
		fclose(fp);
		fp = NULL;
	}
}

BOOL FindFirstFileExists(LPCTSTR lpPath, DWORD dwFilter)
{
	WIN32_FIND_DATA fd;
	HANDLE hFind = FindFirstFile(lpPath, &fd);
	BOOL bFilter = (FALSE == dwFilter) ? TRUE : fd.dwFileAttributes & dwFilter;
	BOOL RetValue = ((hFind != INVALID_HANDLE_VALUE) && bFilter) ? TRUE : FALSE;
	FindClose(hFind);
	return RetValue;
}

bool FolderExists(LPCTSTR lpPath)
{
	if (FALSE == FindFirstFileExists(lpPath, FILE_ATTRIBUTE_DIRECTORY)) {
		if (!CreateDirectory(lpPath, NULL)) {
			// create log fold failed
			return false;
		}
	}
	return true;
}

void Log::createFile(char *logPath) {
	if (fp || !logPath)
		return;

	bool ret = FolderExists(_T("./log"));

	time_t t;  //秒时间
	tm* local; //本地时间
	char buf[256];

	t = time(NULL); //获取目前秒时间
	local = localtime(&t); //转为本地时间
	strftime(buf, 256, "%Y-%m-%d", local);

	char s[512];
	int count = 0;
	while (true)
	{
		if (ret) {
			sprintf_s(s, 512, "./log/%s-%s-%d.log", logPath, buf, count++);
		}
		else {
			sprintf_s(s, 512, "%s-%s-%d.log", logPath, buf, count++);
		}

		DWORD dwAttr = GetFileAttributes(AsciiToUnicode(s).c_str());
		if (INVALID_FILE_ATTRIBUTES == dwAttr)
			break;
	}

	fopen_s(&fp, s, "wb");
}

void Log::Init(char *logPath) {
	if (!self)
		self = new Log();

	if (self)
		self->createFile(logPath);
}

void Log::Release() {
	if (self)
		delete self;

	self = nullptr;
}

void Log::d(const char *tag, const char *fmt, ...) {
	va_list list;
	char fmtdata[1024] = { 0 };
	va_start(list, fmt);
#if defined(ANDROID)
	vsnprintf(fmtdata, sizeof(fmtdata), fmt, list);
#else
	vsnprintf_s(fmtdata, sizeof(fmtdata), fmt, list);
#endif
	va_end(list);
	self->Printf("DEBUG", tag, fmtdata);
}

void Log::w(const char *tag, const char *fmt, ...) {
	va_list list;
	char fmtdata[1024] = { 0 };
	va_start(list, fmt);
#if defined(ANDROID)
	vsnprintf(fmtdata, sizeof(fmtdata), fmt, list);
#else
	vsnprintf_s(fmtdata, sizeof(fmtdata), fmt, list);
#endif
	va_end(list);
	self->Printf("WARN", tag, fmtdata);
}

void Log::e(const char *tag, const char *fmt, ...) {
	va_list list;
	char fmtdata[1024] = { 0 };
	va_start(list, fmt);
#if defined(ANDROID)
	vsnprintf(fmtdata, sizeof(fmtdata), fmt, list);
#else
	vsnprintf_s(fmtdata, sizeof(fmtdata), fmt, list);
#endif
	va_end(list);
	self->Printf("ERROR", tag, fmtdata);
}

void Log::Printf(const char *level, const char *tag, const char *fmt) {
    char fmtdata[1024] = { 0 };
#if defined(ANDROID)
	// TODO : here is bug!!!!!
	sprintf(fmtdata, "%s:(%s):%s \n", level, tag, fmt);
#else
	SYSTEMTIME local_time = { 0 };
	GetLocalTime(&local_time);
	int len = sprintf_s(fmtdata, 1024, "[%02d:%02d:%02d.%03d]:%s:(%s):%s \n",
		local_time.wHour, local_time.wMinute, local_time.wSecond, local_time.wMilliseconds, level, tag, fmt);
#endif

#if defined(ANDROID)
    __android_log_print(ANDROID_LOG_DEBUG, Game_RR_Tag, "%s", fmtdata);
#else
	OutputDebugString(AsciiToUnicode(fmtdata).c_str());

	if (fp) {
		fwrite(fmtdata, 1, len, fp);
		fflush(fp);
	}
#endif
}
