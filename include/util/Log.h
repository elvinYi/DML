#ifndef _LOG_H_
#define _LOG_H_

#include <stdio.h>

class Log {
public:
	static void Init(char *logPath);
	static void Release();

	static void d(const char *tag, const char *fmt, ...);
	static void w(const char *tag, const char *fmt, ...);
	static void e(const char *tag, const char *fmt, ...);

protected:
	Log();
	~Log();

	void createFile(char *logPath);


private:
	void Printf(const char *level, const char *tag, const char *fmt);

private:
	static Log* self;
	FILE *fp;
};

#endif /* _LOG_H_ */
