#ifdef WIN32
#include "D3DRender.h"

#include <stdint.h>
#include <stdio.h>

extern "C"
{
#ifndef __STDC_CONSTANT_MACROS
#  define __STDC_CONSTANT_MACROS
#endif

#ifdef WIN32
#include "libavformat/avformat.h"
#endif
#include "libavutil/avutil.h"
#include "libswscale/swscale.h"
};

D3DRender::D3DRender()
	:m_d3d9_dll(NULL), m_d3d_interface(NULL), m_window(NULL), m_device(NULL), m_surface(NULL),
    m_texture(NULL), m_vertex_buffer(NULL), m_width(180), m_height(320), m_data_type_(VIDEO_DATA_YUV420P),
    src_frame_(NULL), dst_frame_(NULL), sws_context_(NULL)
{
}

bool D3DRender::Initialize(HWND window,  VideoDataType data_type)
{
	if (!Direct3DAttachWindow(window))
	{
		//Log::Error("<D3DRender>Direct3DCreate failed.");
		return false;
	}
	if (!Direct3DCreate())
	{
		//Log::Error("<D3DRender>Direct3DCreate failed.");
		return false;
	}
	if (!Direct3DOpen())
	{
		//Log::Error("<D3DRender>Direct3DOpen failed.");
		CleanUp();
		return false;
	}
	Direct3DSetRenderState();

	if (!Direct3DCreateSurface(data_type))
	{
		//Log::Error("<D3DRender>Direct3DCreateSurface failed.");
		CleanUp();
		return false;
	}
	if (!Direct3DCreateScene())
	{
		//Log::Error("<D3DRender>Direct3DCreateScene failed.");
		CleanUp();
	}
    if (data_type == VIDEO_DATA_RGB24)
    {
        if (!FFmpegConfigImageConverter(m_width, m_height))
        {
            FFmpegSafeDestroyImageConverter();
            return false;
        }
    }
	
	return true;
}

void D3DRender::CleanUp()
{
    FFmpegSafeDestroyImageConverter();
	Direct3DDestroyScene();
	Direct3DDestroySurface();
	Direct3DClose();
	Direct3DDestroy();
}

bool D3DRender::Display(const void *data, int width, int height)
{
    if (width == 0 || height == 0)
    {
        return false;
    }

	if (width != m_width || height != m_height)
	{
		m_width = width;
		m_height = height;
		Direct3DReconfigure();
        FFmpegConfigImageConverter(width, height);
	}
	int x = 0, y=0, w = 0, h = 0;
	if ( GetWindowWidth(m_window) * height <= GetWindowHeight(m_window) * width)
	{
		x = 0;
		y = (GetWindowHeight(m_window) - GetWindowWidth(m_window) * height / width ) >> 1;
		w = GetWindowWidth(m_window);
		h = GetWindowWidth(m_window) * height / width;
	}
	else
	{
		x = (GetWindowWidth(m_window) - GetWindowHeight(m_window) * width / height) >> 1;
		y = 0;
		w = GetWindowHeight(m_window) * width / height;
		h = GetWindowHeight(m_window);
	}
	Rect source = {0,0, width, height};
	Rect destination = { x, y, w, h };

	return Direct3DDrawSlice(data, source, destination);
}

bool D3DRender::Direct3DAttachWindow(HWND window)
{
	m_window = window;
	if (m_window)
	{
		return true;
	}
	else
	{
//		Log::Error("<D3DRender>AttachWindow failed.");
		return false;
	}
}

bool D3DRender::Direct3DDrawSlice(const void *data, const Rect &source_rect, const Rect &destination_rect)
{
	HRESULT hr = 0;
	D3DMATRIX matrix;
	D3DVIEWPORT9 viewport;
	if (!Direct3DEnsureDeviceAvailable())
	{
		return false;
	}
	Direct3DClear();
    /* Set the viewport */
    viewport.X = 0;
    viewport.Y = 0;
	viewport.Width = GetWindowWidth(m_window);
	viewport.Height = GetWindowHeight(m_window);
    viewport.MinZ = 0.0f;
    viewport.MaxZ = 1.0f;
    IDirect3DDevice9_SetViewport(m_device, &viewport);
	if (viewport.Width && viewport.Height) 
	{
        matrix.m[0][0] = 2.0f / viewport.Width;
        matrix.m[0][1] = 0.0f;
        matrix.m[0][2] = 0.0f;
        matrix.m[0][3] = 0.0f;
        matrix.m[1][0] = 0.0f;
        matrix.m[1][1] = -2.0f / viewport.Height;
        matrix.m[1][2] = 0.0f;
        matrix.m[1][3] = 0.0f;
        matrix.m[2][0] = 0.0f;
        matrix.m[2][1] = 0.0f;
        matrix.m[2][2] = 1.0f;
        matrix.m[2][3] = 0.0f;
        matrix.m[3][0] = -1.0f;
        matrix.m[3][1] = 1.0f;
        matrix.m[3][2] = 0.0f;
        matrix.m[3][3] = 1.0f;
       IDirect3DDevice9_SetTransform(m_device, D3DTS_PROJECTION, &matrix);
    }

	Direct3DImportPicture(data);
	Direct3DDrawPrimitive(source_rect, destination_rect);
	
	if (FAILED(IDirect3DDevice9_Present(m_device, NULL, NULL, 0, 0)))
	{
//		Log::Error("<vo_direct3d>IDirect3DDevice9_Present failed.\n");
		return false;
	}

   return true;
}

D3DRender::~D3DRender(void)
{
    CleanUp();
}

bool D3DRender::Direct3DCreate()
{
	m_d3d9_dll = ::LoadLibraryA("d3d9.dll");
	if (m_d3d9_dll == NULL)
	{
//		Log::Error("Load d3d9.dll failed, GLE=%d.", ::GetLastError());
		return false;
	}
	IDirect3D9 *(WINAPI * D3DCreate9) (UINT SDKVersion) = 
		(IDirect3D9 * (WINAPI *) (UINT))::GetProcAddress(m_d3d9_dll, "Direct3DCreate9");
	if (D3DCreate9 == NULL)
	{
//		Log::Error("Unable to find entry point of Direct3DCreate9, GLE=%d.", ::GetLastError());
		return false;
	}
	m_d3d_interface = D3DCreate9(D3D_SDK_VERSION);
	if (m_d3d_interface == NULL)
	{
		::FreeLibrary(m_d3d9_dll);
		m_d3d9_dll = NULL;
//		Log::Error("Direct3DCreate9 failed.");
		return false;
	}
	if (FAILED(IDirect3D9_GetDeviceCaps(m_d3d_interface, D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &m_display_caps)))
	{
//		Log::Error("Reading display capabilities failed.");
		return false;
	}
	if (!(m_display_caps.DevCaps2 & D3DDEVCAPS2_CAN_STRETCHRECT_FROM_TEXTURES) ||
        !(m_display_caps.TextureFilterCaps & (D3DPTFILTERCAPS_MAGFLINEAR)) ||
        !(m_display_caps.TextureFilterCaps & (D3DPTFILTERCAPS_MINFLINEAR))) 
	{
//        Log::Error("Device does not support stretching from textures.");
        return false;
    }
	return true;
}

void D3DRender::Direct3DDestroy()
{
	if (m_d3d_interface)
	{
		IDirect3D9_Release(m_d3d_interface);
		m_d3d_interface = NULL;
	}
	if (m_d3d9_dll)
	{
		::FreeLibrary(m_d3d9_dll);
		m_d3d9_dll = NULL;
	}
}

bool D3DRender::Direct3DOpen()
{
    if (!Direct3DFillPresentationParameters())
	{
//		Log::Error("<D3DRender>Direct3DFillPresentationParameters failed.");
        return false;
	}

    D3DADAPTER_IDENTIFIER9 d3dai;
    if (FAILED(IDirect3D9_GetAdapterIdentifier(m_d3d_interface, D3DADAPTER_DEFAULT, 0, &d3dai))) 
	{
//		Log::Information("IDirect3D9_GetAdapterIdentifier failed.");
    } 
	else
	{
//		Log::Information("Direct3d Device: %s %lu %lu %lu", d3dai.Description,
//                d3dai.VendorId, d3dai.DeviceId, d3dai.Revision );
    }

	HRESULT hr = IDirect3D9_CreateDevice(m_d3d_interface, D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL, m_window, D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED,
		&m_present_params, &m_device);
	if (FAILED(hr)) 
	{
		//Log::Error("Could not create the D3D device! (hr=0x%lX)", hr);
		return false;
    }

	InvalidateRect(m_window, NULL, TRUE);
    
	//Log::Information("Direct3D device adapter successfully initialized.");
    return true;
}

void D3DRender::Direct3DClose()
{
	if (m_device != NULL)
	{
		IDirect3D9_Release(m_device);
        m_device = NULL;
	}
}

bool D3DRender::Direct3DFillPresentationParameters()
{
	D3DDISPLAYMODE d3ddm;
	HRESULT hr = IDirect3D9_GetAdapterDisplayMode(m_d3d_interface,
					D3DADAPTER_DEFAULT, &d3ddm);
    if (FAILED(hr)) 
	{
//		Log::Error("<D3DRender>Could not read adapter display mode. (hr=0x%lX)", hr);
		return false;
	}
    /* Set up the structure used to create the D3DDevice. */
    D3DPRESENT_PARAMETERS *d3dpp = &m_present_params;
    ZeroMemory(d3dpp, sizeof(D3DPRESENT_PARAMETERS));
    d3dpp->Flags                  = D3DPRESENTFLAG_VIDEO;
    d3dpp->Windowed               = TRUE;
	d3dpp->hDeviceWindow          = m_window;
	d3dpp->BackBufferWidth        = GetWindowWidth(m_window);
	d3dpp->BackBufferHeight       = GetWindowHeight(m_window);
    d3dpp->SwapEffect             = D3DSWAPEFFECT_COPY;
    d3dpp->MultiSampleType        = D3DMULTISAMPLE_NONE;
    d3dpp->PresentationInterval   = D3DPRESENT_INTERVAL_DEFAULT;
    d3dpp->BackBufferFormat       = d3ddm.Format;
    d3dpp->BackBufferCount        = 1;
    d3dpp->EnableAutoDepthStencil = FALSE;
	return true;
}

bool D3DRender::Direct3DCreateSurface(VideoDataType video_data_type)
{
	/* Create a surface */
    HRESULT hr = 0;
    if (video_data_type == VIDEO_DATA_YUV420P)
    {
        hr = IDirect3DDevice9_CreateOffscreenPlainSurface(m_device,
		    m_width,
		    m_height, (D3DFORMAT)MAKEFOURCC('Y', 'V', '1', '2'),
		    D3DPOOL_DEFAULT, &m_surface, NULL);
    }
    else if (video_data_type == VIDEO_DATA_RGB24)
    {
        hr = IDirect3DDevice9_CreateOffscreenPlainSurface(m_device,
		    m_width,
		    m_height, D3DFMT_X8R8G8B8,
		    D3DPOOL_DEFAULT, &m_surface, NULL);
    }
    else
    {
        return false;
    }
    if (FAILED(hr)) 
	{
//        Log::Error("Failed to create picture surface. (hr=0x%lx)", hr);
        return false;
    }
    m_data_type_ = video_data_type;

    IDirect3DDevice9_ColorFill(m_device, m_surface, NULL, D3DCOLOR_XRGB(0, 0, 0));

    return true;
}

void D3DRender::Direct3DDestroySurface()
{
	if (m_surface != NULL)
	{
		IDirect3DSurface9_Release(m_surface);
		m_surface = NULL;
	}
}

bool D3DRender::Direct3DCreateScene()
{
    HRESULT hr = 0;
    /*
     * Create a texture for use when rendering a scene
     * for performance reason, texture format is identical to backbuffer
     * which would usually be a RGB format
     */
    hr = IDirect3DDevice9_CreateTexture(m_device,
		m_width, m_height, 
		1, D3DUSAGE_RENDERTARGET,
		m_present_params.BackBufferFormat,
		D3DPOOL_DEFAULT, &m_texture, NULL);
	
    if (FAILED(hr))
	{
//		Log::Error("Failed to create texture. (hr=0x%lx)", hr);
        return false;
    }
//	Log::Information("Direct3D scene created successfully");
    return true;
}

void D3DRender::Direct3DDestroyScene()
{
	if (m_texture)
	{
        IDirect3DTexture9_Release(m_texture);
		m_texture = NULL;
	}
}

bool D3DRender::Direct3DClear()
{
	HRESULT hr = IDirect3DDevice9_Clear(m_device, 0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0,0,0), 0.0f, 0);
	if (FAILED(hr)) 
	{
        //Log::Error("Failed to create picture surface. (hr=0x%lx)", hr);
		return false;
    }
	return true;
}

void D3DRender::Direct3DSetRenderState()
{
	IDirect3DDevice9_SetSamplerState(m_device, 0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
    IDirect3DDevice9_SetSamplerState(m_device, 0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);

    if (m_display_caps.TextureFilterCaps & D3DPTFILTERCAPS_MINFLINEAR)
	{
        IDirect3DDevice9_SetSamplerState(m_device, 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
    }
	else
	{
        IDirect3DDevice9_SetSamplerState(m_device, 0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
    }
    if (m_display_caps.TextureFilterCaps & D3DPTFILTERCAPS_MAGFLINEAR)
	{
        IDirect3DDevice9_SetSamplerState(m_device, 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
    }
    else 
    {
        IDirect3DDevice9_SetSamplerState(m_device, 0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
    }
	IDirect3DDevice9_SetVertexShader(m_device, NULL);
	IDirect3DDevice9_SetRenderState(m_device, D3DRS_AMBIENT, D3DCOLOR_XRGB(255,255,255));
	IDirect3DDevice9_SetRenderState(m_device, D3DRS_CULLMODE, D3DCULL_NONE);
	IDirect3DDevice9_SetRenderState(m_device, D3DRS_ZENABLE, D3DZB_FALSE);
	IDirect3DDevice9_SetRenderState(m_device, D3DRS_LIGHTING, FALSE);
	IDirect3DDevice9_SetRenderState(m_device, D3DRS_DITHERENABLE, TRUE);
	IDirect3DDevice9_SetRenderState(m_device, D3DRS_STENCILENABLE, FALSE);
	IDirect3DDevice9_SetRenderState(m_device, D3DRS_ALPHABLENDENABLE, FALSE);
	IDirect3DDevice9_SetRenderState(m_device, D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
	IDirect3DDevice9_SetRenderState(m_device, D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);

	if (m_display_caps.AlphaCmpCaps & D3DPCMPCAPS_GREATER) 
	{
		IDirect3DDevice9_SetRenderState(m_device, D3DRS_ALPHATESTENABLE,TRUE);
		IDirect3DDevice9_SetRenderState(m_device, D3DRS_ALPHAREF, 0x00);
		IDirect3DDevice9_SetRenderState(m_device, D3DRS_ALPHAFUNC,D3DCMP_GREATER);
    }

    IDirect3DDevice9_SetTextureStageState(m_device, 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
    IDirect3DDevice9_SetTextureStageState(m_device, 0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    IDirect3DDevice9_SetTextureStageState(m_device, 0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
    IDirect3DDevice9_SetTextureStageState(m_device, 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
    IDirect3DDevice9_SetTextureStageState(m_device, 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
	
	D3DMATRIX matrix;
    matrix.m[0][0] = 1.0f;
    matrix.m[0][1] = 0.0f;
    matrix.m[0][2] = 0.0f;
    matrix.m[0][3] = 0.0f;
    matrix.m[1][0] = 0.0f;
    matrix.m[1][1] = 1.0f;
    matrix.m[1][2] = 0.0f;
    matrix.m[1][3] = 0.0f;
    matrix.m[2][0] = 0.0f;
    matrix.m[2][1] = 0.0f;
    matrix.m[2][2] = 1.0f;
    matrix.m[2][3] = 0.0f;
    matrix.m[3][0] = 0.0f;
    matrix.m[3][1] = 0.0f;
    matrix.m[3][2] = 0.0f;
    matrix.m[3][3] = 1.0f;
    IDirect3DDevice9_SetTransform(m_device, D3DTS_WORLD, &matrix);
    IDirect3DDevice9_SetTransform(m_device, D3DTS_VIEW, &matrix);
}

bool D3DRender::Direct3DImportPicture(const void *video_data)
{
	HRESULT hr = 0;
	D3DLOCKED_RECT d3drect = {0};
	//Lock surface
	hr = IDirect3DSurface9_LockRect(m_surface, &d3drect, NULL, 0);
    if (FAILED(hr))
	{
		return false;
    }
    if (m_data_type_ == VIDEO_DATA_YUV420P)
    {
	    uint8_t *yuv[3] = {0};
	    yuv[0] = (uint8_t *)d3drect.pBits;
	    yuv[2] = yuv[0] + d3drect.Pitch * m_height;
	    yuv[1] = yuv[2] + d3drect.Pitch * m_height / 4;
	    for (int i = 0; i < (m_height >> 1); ++i)
	    {
		    memcpy(yuv[0], (uint8_t *)video_data + 2 * i * m_width, m_width);
		    yuv[0] += d3drect.Pitch;
		    memcpy(yuv[0], (uint8_t *)video_data + (2 * i + 1) * m_width, m_width);
		    yuv[0] += d3drect.Pitch;
		    memcpy(yuv[1], (uint8_t *)video_data + m_width * m_height + i * (m_width >> 1), m_width >> 1);
		    yuv[1] += (d3drect.Pitch >> 1);
		    memcpy(yuv[2], (uint8_t *)video_data + m_width * m_height + m_width * m_height / 4 + i * (m_width >> 1), m_width >> 1);
		    yuv[2] += (d3drect.Pitch >> 1);
	    }
    }
    else if (m_data_type_ == VIDEO_DATA_RGB24)
    {
#if 0
        uint8_t *d3d_buffer = (uint8_t *)d3drect.pBits;
        int video_buffer_size = m_width * m_height * 3;
        int pixel_counter = 0;
        for (int i = 0; i < video_buffer_size; i+=3)
        {
            *(d3d_buffer + 2) = *((uint8_t *)video_data + i + 0);
            *(d3d_buffer + 1) = *((uint8_t *)video_data + i + 1);
            *(d3d_buffer + 0) = *((uint8_t *)video_data + i + 2);
            d3d_buffer += 4;
            
            ++pixel_counter;
            if (pixel_counter == m_width)
            {
                d3d_buffer += (d3drect.Pitch - (m_width << 2));
                pixel_counter = 0;
            }
        }
#else
        FFmpegConvertBitmap(static_cast<const uint8_t *>(video_data), static_cast<uint8_t *>(d3drect.pBits), d3drect.Pitch);
#endif
    }
	/* Unlock the Surface */
	hr = IDirect3DSurface9_UnlockRect(m_surface);
	if (FAILED(hr))
	{
//		Log::Error("IDirect3DSurface9_UnlockRect failed.");
		return false;
	}
    /* retrieve texture top-level surface */
    LPDIRECT3DSURFACE9 destination;
    hr = IDirect3DTexture9_GetSurfaceLevel(m_texture, 0, &destination);
    if (FAILED(hr))
	{
//        Log::Error("IDirect3DTexture9_GetSurfaceLevel Failed.(hr=0x%0lX)", hr);
        return false;
    }


	hr = IDirect3DDevice9_StretchRect(m_device, m_surface, NULL, destination, NULL, D3DTEXF_LINEAR);
    IDirect3DSurface9_Release(destination);
    if (FAILED(hr)) 
	{
//        Log::Error("IDirect3DDevice9_StretchRect Failed.(hr=0x%0lX)", hr);
        return false;
    }
    return true;
}

bool D3DRender::Direct3DDrawPrimitive(const Rect &source_rect, const Rect &destination_rect)
{
	HRESULT hr = 0;
	float minx, miny, maxx, maxy;
    float minu, maxu, minv, maxv;
    DWORD color;
	float rhw;
	
	minx = destination_rect.x - 0.5f;
    miny = destination_rect.y - 0.5f;
	maxx = destination_rect.x + destination_rect.width - 0.5f;
	maxy = destination_rect.y + destination_rect.height - 0.5f;
	rhw = 1.0f;

    minu = (float) source_rect.x / m_width;
    maxu = (float) (source_rect.x + source_rect.width) / m_width;
	minv = (float) source_rect.y / m_height;
    maxv = (float) (source_rect.y + source_rect.height) / m_height;

    color = D3DCOLOR_XRGB(255, 255, 255);

    m_vertices[0].x = minx;
    m_vertices[0].y = miny;
    m_vertices[0].z = 0.0f;
	m_vertices[0].rhw = rhw;
    m_vertices[0].color = color;
    m_vertices[0].u = minu;
    m_vertices[0].v = minv;

    m_vertices[1].x = maxx;
    m_vertices[1].y = miny;
    m_vertices[1].z = 0.0f;
	m_vertices[1].rhw = rhw;
    m_vertices[1].color = color;
    m_vertices[1].u = maxu;
    m_vertices[1].v = minv;

    m_vertices[2].x = maxx;
    m_vertices[2].y = maxy;
    m_vertices[2].z = 0.0f;
	m_vertices[2].rhw = rhw;
    m_vertices[2].color = color;
    m_vertices[2].u = maxu;
    m_vertices[2].v = maxv;

    m_vertices[3].x = minx;
    m_vertices[3].y = maxy;
    m_vertices[3].z = 0.0f;
	m_vertices[3].rhw = rhw;
    m_vertices[3].color = color;
    m_vertices[3].u = minu;
    m_vertices[3].v = maxv;
	
	hr = IDirect3DDevice9_SetFVF(m_device, D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1);
    if (FAILED(hr)) 
	{
//        Log::Error("IDirect3DDevice9_SetFVF Failed.(hr=0x%0lX)", hr);
        return false;
    }
    // Setup our texture. Using textures introduces the texture stage states,
    // which govern how textures get blended together (in the case of multiple
    // textures) and lighting information. In this case, we are modulating
    // (blending) our texture with the diffuse color of the vertices.
	hr = IDirect3DDevice9_SetTexture(m_device, 0, (LPDIRECT3DBASETEXTURE9)m_texture);
    if (FAILED(hr)) 
	{
//		Log::Error("IDirect3DDevice9_SetTexture Failed.(hr=0x%0lX)", hr);
        return false;
    }
    Direct3DSetRenderState();
	if (FAILED(IDirect3DDevice9_BeginScene(m_device)))
	{
//		Log::Error("BeginScene failed.\n");
		return false;
	}
	// we use FVF instead of vertex shader
    // draw rectangle
    hr = IDirect3DDevice9_DrawPrimitiveUP(m_device, D3DPT_TRIANGLEFAN, 2, m_vertices, sizeof(*m_vertices));
    if (FAILED(hr)) 
	{
//		Log::Error("IDirect3DDevice9_DrawPrimitiveUP Failed.(hr=0x%0lX)", hr);
		return false;
	}
	if (FAILED(IDirect3DDevice9_EndScene(m_device)))
	{
//		Log::Error("IDirect3DDevice9_EndScene failed.\n");
		return false;
	}
	return true;
}

bool D3DRender::Direct3DEnsureDeviceAvailable()
{
	static uint32_t window_width = GetWindowWidth(m_window);
	static uint32_t window_height = GetWindowHeight(m_window);
	HRESULT hr = IDirect3DDevice9_TestCooperativeLevel(m_device);
	bool	window_size_changed = ((window_width != GetWindowWidth(m_window)) || (window_height != GetWindowHeight(m_window))) 
		 && (GetWindowWidth(m_window) != 0) && (GetWindowHeight(m_window) != 0);

	if (window_size_changed)
	{
		window_width = GetWindowWidth(m_window);
		window_height = GetWindowHeight(m_window);
		return Direct3DReconfigure();
	}
	if (SUCCEEDED(hr))
	{
		return true;
	}
	else if ( FAILED(hr) && (hr == D3DERR_DEVICENOTRESET))
	{
		return Direct3DReconfigure();
	}
	else
	{
		return false;
	}
}

bool D3DRender::Direct3DReconfigure()
{
	HRESULT hr = 0;
	Direct3DDestroyScene();
	Direct3DDestroySurface();
	if (!Direct3DFillPresentationParameters())
	{
		return false;
	}
	hr = IDirect3DDevice9_Reset(m_device,  &m_present_params);
	//if (FAILED(hr))
	//{
	//	//Log::Error("IDirect3DDevice9_Reset Failed.(hr=0x%0lX)", hr);
	//	printf("hr=0x%0lX", hr);
	//	return false;
	//}
	return (Direct3DCreateSurface(m_data_type_) && Direct3DCreateScene());
}

int D3DRender::GetWindowWidth(HWND hwnd)
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

int D3DRender::GetWindowHeight(HWND hwnd)
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

bool D3DRender::FFmpegConfigImageConverter(int width, int height)
{
    FFmpegSafeDestroyImageConverter();
	src_frame_ = av_frame_alloc();//avcodec_alloc_frame();
	dst_frame_ = av_frame_alloc();//avcodec_alloc_frame();
	sws_context_ = sws_getContext(width, height, PIX_FMT_BGR24, width, height, PIX_FMT_BGRA, SWS_POINT, NULL, NULL, NULL);
    return (sws_context_!=NULL);
}

void D3DRender::FFmpegSafeDestroyImageConverter()
{
    if (sws_context_)
    {
        sws_freeContext(sws_context_);
        sws_context_ = NULL;
    }
    if(src_frame_)
    {
        av_free(src_frame_);
        src_frame_ = NULL;
    }
    if (dst_frame_)
    {
        av_free(dst_frame_);
        dst_frame_ = NULL;
    }
}

void D3DRender::FFmpegConvertBitmap(const uint8_t *src, uint8_t *dst, int pitch)
{
    avpicture_fill( reinterpret_cast<AVPicture *>(src_frame_), const_cast<uint8_t *>(src), PIX_FMT_BGR24, m_width, m_height );
    avpicture_fill( reinterpret_cast<AVPicture *>(dst_frame_), dst, PIX_FMT_BGRA, m_width, m_height);
    src_frame_->linesize[0] = m_width * 3;
    dst_frame_->linesize[0] = pitch;

	//src_frame_->data[0] += src_frame_->linesize[0] * (m_height - 1);
	//src_frame_->linesize[0] *= -1;

    sws_scale(sws_context_, src_frame_->data, src_frame_->linesize, 0, m_height, dst_frame_->data, dst_frame_->linesize);
}

#endif