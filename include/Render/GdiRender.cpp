#include "GdiRender.h"


GdiRender::GdiRender(void)
    :  window_handle_(NULL), video_width_(640), video_height_(480),
    picture_buffer_(NULL)
{
}


GdiRender::~GdiRender(void)
{
    CleanUp();
}


bool GdiRender::Initialize(HWND window, VideoDataType video_data_type)
{
    if (video_data_type != VIDEO_DATA_RGB24)
    {
        return false;
    }
    if (::IsWindow(window) == FALSE)
    {
        return false;
    }
    window_handle_ = window;
    if (!GdiConfigure())
    {
        window_handle_ = NULL;
        return false;
    }
    return true;
}


bool GdiRender::Display(const void *data, int width, int height)
{
    if (::IsWindow(window_handle_) == FALSE)
    {
        return false;
    }
    if ( (video_width_ != width) || (video_height_ != height) )
    {
        video_width_ = width;
        video_height_ = height;
        if (!GdiConfigure())
        {
            return false;
        }
    }

    HDC hdc = GetDC(window_handle_);
    if (hdc == NULL)
    {
        return false;
    }
    if (picture_buffer_ != NULL)
    {
        size_t picture_buffer_size = video_width_ * video_height_ * 3;
        memcpy(picture_buffer_, data, picture_buffer_size);
        char temp_component = 0;
        for (char *picture_iter = static_cast<char *>(picture_buffer_); 
            picture_iter < static_cast<char *>(picture_buffer_) + picture_buffer_size; picture_iter += 3)
        {
            temp_component = *picture_iter;
            *picture_iter = *(picture_iter + 2);
            *(picture_iter + 2) = temp_component;
        }
    }

    RECT rect_dst = {0};
    GetClientRect(window_handle_, &rect_dst);
    int render_x = 0, render_y=0, render_w = 0, render_h = 0;
	if ( GetWindowWidth(window_handle_) * height <= GetWindowHeight(window_handle_) * width)
	{
		render_x = 0;
		render_y = (GetWindowHeight(window_handle_) - GetWindowWidth(window_handle_) * height / width ) >> 1;
		render_w = GetWindowWidth(window_handle_);
		render_h = GetWindowWidth(window_handle_) * height / width;
	}
	else
	{
		render_x = (GetWindowWidth(window_handle_) - GetWindowHeight(window_handle_) * width / height) >> 1;
		render_y = 0;
		render_w = GetWindowHeight(window_handle_) * width / height;
		render_h = GetWindowHeight(window_handle_);
	}

    bool success = false;
    if ( (video_width_ != render_w) || (video_height_ != render_h) )
    {
        ::SetStretchBltMode(hdc, MAXSTRETCHBLTMODE);
        success = (::StretchBlt(hdc, render_x, render_y, render_w, render_h, off_dc_, 0,
            0, video_width_, video_height_, SRCCOPY) != FALSE);
    }
    else
    {
        success = (::BitBlt(hdc, render_x, render_y, render_w, render_h, off_dc_, 0,
            0, SRCCOPY) != FALSE);
    }
    ReleaseDC(window_handle_, hdc);
    return success;
}


void GdiRender::CleanUp()
{
    if (off_dc_)
    {
        DeleteDC(off_dc_);
        off_dc_ = NULL;
    }

    if (off_bitmap_)
    {
        DeleteObject(off_bitmap_);
        off_bitmap_ = NULL;
    }

    picture_buffer_ = NULL;
}

bool GdiRender::GdiConfigure()
{
    CleanUp();
    if (window_handle_ == NULL)
    {
        return false;
    }

    HDC hdc = GetDC(window_handle_);
    if (hdc == NULL)
    {
        return false;
    }

    BITMAPINFOHEADER bih = {0};
    bih.biSize = sizeof(BITMAPINFOHEADER);
    bih.biSizeImage     = 0;
    bih.biPlanes        = 1;
    bih.biCompression   = BI_RGB;
    bih.biBitCount      = 24;
    bih.biWidth         = video_width_;
    bih.biHeight        = -video_height_;
    bih.biClrImportant  = 0;
    bih.biClrUsed       = 0;
    bih.biXPelsPerMeter = 0;

    bool success = true;
    do
    {
        off_bitmap_ = CreateDIBSection(hdc,(BITMAPINFO *)&bih, DIB_RGB_COLORS, &picture_buffer_, NULL, 0);
        if (off_bitmap_ == NULL)
        {
            success = false;
            break;
        }
        off_dc_ = CreateCompatibleDC(hdc);
        if (off_dc_ == NULL)
        {
            success = false;
            break;
        }
        HGDIOBJ old_bitmap = SelectObject(off_dc_, off_bitmap_);
        if (old_bitmap == NULL)
        {
            success = false;
            break;
        }
    }while(0);

    ReleaseDC(window_handle_, hdc);

    if (!success)
    {
        CleanUp();
    }
    return success;
}

int GdiRender::GetWindowWidth(HWND hwnd)
{
	RECT rc = {0};
	if (!::IsWindow(hwnd))
	{
		return 0;
	}
	else
	{
		::GetClientRect(hwnd, &rc);
		return (rc.right - rc.left);
	}
}

int GdiRender::GetWindowHeight(HWND hwnd)
{
	RECT rc = {0};
	if (!::IsWindow(hwnd))
	{
		return 0;
	}
	else
	{
		::GetClientRect(hwnd, &rc);
		return (rc.bottom - rc.top);
	}
}
