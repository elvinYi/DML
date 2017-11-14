#ifndef __GDI_RENDER__H__
#define __GDI_RENDER__H__

#include <Windows.h>
#include "BaseRender.h"

class GdiRender : public BaseRender
{
public:
    GdiRender(void);
    ~GdiRender(void);
    bool Initialize(HWND window, VideoDataType video_data_type);
	bool Display(const void *data, int width, int height);
	void CleanUp();
private:
    bool GdiConfigure();
    int GetWindowWidth(HWND hwnd);
    int GetWindowHeight(HWND hwnd);
private:
    HWND        window_handle_;
    int         video_width_;
    int         video_height_;
    HBITMAP     off_bitmap_;
    HDC         off_dc_;
    void *      picture_buffer_;
};

#endif