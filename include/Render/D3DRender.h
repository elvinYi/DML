#ifdef WIN32
#ifndef __D3DRENDER_H__
#define __D3DRENDER_H__

#include <d3d9.h>

extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavutil/pixfmt.h"
#include "libswscale/swscale.h"
};


#include "BaseRender.h"


class D3DRender : public BaseRender
{
public:
	D3DRender();
	virtual bool Initialize(HWND window, VideoDataType data_type);
	virtual bool Display(const void *data, int width, int height);
	virtual void CleanUp();
	~D3DRender(void);
private:
	struct Vertex
	{
		float x, y, z;
		float rhw;
		DWORD color;
		float u, v;
	};

	struct Rect
	{
		int x;
		int y;
		int width;
		int height;
	};

	bool Direct3DCreate();
	void Direct3DDestroy();

	bool Direct3DOpen();
	void Direct3DClose();

	bool Direct3DAttachWindow(HWND window);
	bool Direct3DFillPresentationParameters();
	void Direct3DSetRenderState();

	bool Direct3DCreateSurface(VideoDataType video_data_type);
	void Direct3DDestroySurface();

	bool Direct3DCreateScene();
	void Direct3DDestroyScene();
	bool Direct3DEnsureDeviceAvailable();
	bool Direct3DReconfigure();

	bool Direct3DImportPicture(const void *video_data);
	bool Direct3DDrawPrimitive(const Rect &source_rect, const Rect &destination_rect);
	bool Direct3DClear();
	bool Direct3DSetupVertices(const Rect &source_rect, const Rect &destination_rect);
	bool Direct3DDrawSlice(const void *data, const Rect &source_rect, const Rect &destination_rect);

	int GetWindowWidth(HWND hwnd);
	int GetWindowHeight(HWND hwnd);

    bool FFmpegConfigImageConverter(int width, int height);
    void FFmpegSafeDestroyImageConverter();
    void FFmpegConvertBitmap(const uint8_t *src, uint8_t *dst, int pitch);

private:
    VideoDataType           m_data_type_;
	Vertex					m_vertices[4];
	HWND					m_window;
	HMODULE					m_d3d9_dll;
	IDirect3D9 *			m_d3d_interface;
	D3DPRESENT_PARAMETERS	m_present_params;
	D3DCAPS9				m_display_caps;
	LPDIRECT3DDEVICE9		m_device;
	LPDIRECT3DSURFACE9		m_surface;
	LPDIRECT3DTEXTURE9		m_texture;
	LPDIRECT3DVERTEXBUFFER9 m_vertex_buffer;
	int						m_width;
	int						m_height;

    AVFrame *               src_frame_;
    AVFrame *               dst_frame_;
    SwsContext *            sws_context_;
};

#endif
#endif