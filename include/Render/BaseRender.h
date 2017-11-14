#ifndef __BASE_RENDER__H__
#define __BASE_RENDER__H__

#include <Windows.h>

enum VideoDataType
{
    VIDEO_DATA_YUV420P,
    VIDEO_DATA_RGB24,
    VIDEO_DATA_UNKNOWN
};

class BaseRender
{
public:
    virtual ~BaseRender(void) {}
    virtual bool Initialize(HWND window, VideoDataType data_type) = 0;
	virtual bool Display(const void *data, int width, int height) = 0;
	virtual void CleanUp() = 0;
};

#endif