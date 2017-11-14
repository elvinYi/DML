#include "RenderManager.h"

#include "D3DRender.h"
#include "GdiRender.h"

RenderManager::RenderManager(void)
    : render_(NULL)
{
}


RenderManager::~RenderManager(void)
{
    CleanUp();
}

bool RenderManager::Initialize(HWND window, VideoDataType &data_type)
{
     CleanUp();

     
     /*if (render_->Initialize(window, VIDEO_DATA_YUV420P))
     {
         data_type = VIDEO_DATA_YUV420P;
         return true;
     }
     else*/ 
     
     render_ = new D3DRender();
     if (render_->Initialize(window, VIDEO_DATA_RGB24))
     {
         data_type = VIDEO_DATA_RGB24;
         return true;
     }
     else
     {
         delete render_;
         render_ = NULL;
     }

     render_ = new GdiRender();
     if (render_->Initialize(window, VIDEO_DATA_RGB24))
     {
         data_type = VIDEO_DATA_RGB24;
         return true;
     }
     else
     {
         delete render_;
         render_ = NULL;
     }

     data_type = VIDEO_DATA_UNKNOWN;
     return false;
}

bool RenderManager::Display(const void *data, int width, int height)
{
    if (render_ == NULL)
    {
        return false;
    }
    return render_->Display(data, width, height);
}

void RenderManager::CleanUp()
{
    if (render_ != NULL)
    {
        render_->CleanUp();
        delete render_;
        render_ = NULL;
    }
}