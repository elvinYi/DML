#ifndef _LOG_H_
#define _LOG_H_

#include "PubConst.h"

IED_ENTRY

//#define _TEST_LOG_FILE_ 1

class Log {
public:
	static void Init();
	static void Release();

	static void d(const char *tag, const char *fmt, ...);
	static void w(const char *tag, const char *fmt, ...);
	static void e(const char *tag, const char *fmt, ...);

protected:
	Log();
	~Log();

private:
	void Printf(const char *level, const char *tag, const char *fmt);

private:
	static Log* self;
#if _TEST_LOG_FILE_
	FILE* fp;
#endif

};

IED_EXIT

#endif /* _LOG_H_ */
