#ifndef __RENDER_MANAGER__H__
#define __RENDER_MANAGER__H__

#include "BaseRender.h"

class RenderManager
{
public:
    RenderManager(void);
    ~RenderManager(void);
    bool Initialize(HWND window, VideoDataType &data_type);
	bool Display(const void *data, int width, int height);
	void CleanUp();
private:
    BaseRender *render_;
};

#endif