#include "Log.h"

#if defined(ANDROID)
#include <android/log.h>
#else
#include <Windows.h>
#endif

#include <stdarg.h>

IED_ENTRY

#if defined(ANDROID)
const char *Game_RR_Tag = "Game_RR";
#endif

Log* Log::self = nullptr;

Log::Log() {
#if _TEST_LOG_FILE_
	fp = NULL;
	//time_t     now = time(0);
	//struct tm  tstruct;
	//char       buf[80];
	//tstruct = *localtime(&now);
	//strftime(buf, sizeof(buf), "%Y-%m-%d, %X", &tstruct);

	fopen_s(&fp, "C://Test.log", "ab+");
#endif
}

Log::~Log() {
#if _TEST_LOG_FILE_
	if (fp) {
		fclose(fp);
		fp = NULL;
	}
#endif
}

void Log::Init() {
	if (!self)
		self = new Log();
}

void Log::Release() {
	//if (self)
	//	delete self;
	//self = nullptr;
}

void Log::d(const char *tag, const char *fmt, ...) {
#ifdef _DEBUG
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
#endif
}

void Log::w(const char *tag, const char *fmt, ...) {
#ifdef _DEBUG
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
#endif
}

void Log::e(const char *tag, const char *fmt, ...) {
#ifdef _DEBUG
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
#endif
}

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

void Log::Printf(const char *level, const char *tag, const char *fmt) {
#ifdef _DEBUG
    char fmtdata[1024] = { 0 };
#if defined(ANDROID)
	// TODO : here is bug!!!!!
	sprintf(fmtdata, "%s:(%s):%s \n", level, tag, fmt);
#else
	int len = sprintf_s(fmtdata, 1024, "%s:(%s):%s \n", level, tag, fmt);
#endif

#if defined(ANDROID)
    __android_log_print(ANDROID_LOG_DEBUG, Game_RR_Tag, "%s", fmtdata);
#else
	OutputDebugString(AsciiToUnicode(fmtdata).c_str());

#if _TEST_LOG_FILE_
	if (fp) {
		fwrite(fmtdata, 1, len, fp);
	}
#endif
	//printf("%s", fmtdata);
#endif

#endif
}

IED_EXIT
