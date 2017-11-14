////////////////////////////////////////////////////////////////////////////
//
// Copyright 1993-2014 NVIDIA Corporation.  All rights reserved.
//
// Please refer to the NVIDIA end user license agreement (EULA) associated
// with this source code for terms and conditions that govern your use of
// this software. Any use, reproduction, disclosure, or distribution of
// this software and related documentation outside the terms of the EULA
// is strictly prohibited.
//
////////////////////////////////////////////////////////////////////////////

#if defined(NV_WINDOWS)
    //#include <d3d9.h>
    //#include <d3d10_1.h>
    //#include <d3d11.h>
#pragma warning(disable : 4996)
#endif

#include "NvHWEncoder.h"

typedef void* CUcontext;

#define MAX_ENCODE_QUEUE 32

#define SET_VER(configStruct, type) {configStruct.version = type##_VER;}

template<class T>
class CNvQueue {
    T** m_pBuffer;
    unsigned int m_uSize;
    unsigned int m_uPendingCount;
    unsigned int m_uAvailableIdx;
    unsigned int m_uPendingndex;
public:
    CNvQueue(): m_pBuffer(NULL), m_uSize(0), m_uPendingCount(0), m_uAvailableIdx(0),
                m_uPendingndex(0)
    {
    }

    ~CNvQueue()
    {
        delete[] m_pBuffer;
    }

    bool Initialize(T *pItems, unsigned int uSize)
    {
        m_uSize = uSize;
        m_uPendingCount = 0;
        m_uAvailableIdx = 0;
        m_uPendingndex = 0;
        m_pBuffer = new T *[m_uSize];
        for (unsigned int i = 0; i < m_uSize; i++)
        {
            m_pBuffer[i] = &pItems[i];
        }
        return true;
    }


    T * GetAvailable()
    {
        T *pItem = NULL;
        if (m_uPendingCount == m_uSize)
        {
            return NULL;
        }
        pItem = m_pBuffer[m_uAvailableIdx];
        m_uAvailableIdx = (m_uAvailableIdx+1)%m_uSize;
        m_uPendingCount += 1;
        return pItem;
    }

    T* GetPending()
    {
        if (m_uPendingCount == 0) 
        {
            return NULL;
        }

        T *pItem = m_pBuffer[m_uPendingndex];
        m_uPendingndex = (m_uPendingndex+1)%m_uSize;
        m_uPendingCount -= 1;
        return pItem;
    }
};

typedef struct _EncodeFrameConfig
{
    uint8_t  *yuv[3];
    uint32_t stride[3];
    uint32_t width;
    uint32_t height;
}EncodeFrameConfig;

typedef enum 
{
    NV_ENC_DX9 = 0,
    NV_ENC_DX11 = 1,
    NV_ENC_CUDA = 2,
    NV_ENC_DX10 = 3,
} NvEncodeDeviceType;

enum class CNvEncoderSetkey {
	HEIGHT = 0,
	WIDTH,
	BITRATE,
	VBV_NAX_BITRATE,
	VBV_SIZE,
	FPS,
	RCMODE,
	GOPLENGTH,
	NUMB,
	QP,
	PRESET,   // not implemented, "hq", "lowLatencyHP", "hp", "lowLatencyHQ", "lossless"
	CODEC,
	INTRA_REFRESH,
	INTRA_REFRESH_PERIOD,
	INTRA_REFRESH_DURATION,
	PIC_STRUCT,
	YUV444,          // not implemented
	QUALITY,
	ENTROPY_CODING_MODE,
	PROFILE,
	FLUSH,
};

enum class CNvEncoderGetKey {
	SPS_PPS_SIZE = 0,  // int
	SPS_PPS_DATA,      // uint8_t**
};

class CNvEncoder
{
public:
    CNvEncoder();
    virtual ~CNvEncoder();

	int                                                  Init(int codec);
	void                                                 Uninit();
	int                                                  Encode(uint8_t* y, uint8_t* u, uint8_t* v, long long pts);
	int                                                  GetOutput(uint8_t* out, unsigned int maxSize, unsigned int& outSize, long long& pts, long long& dts, bool& keyFrame);
	int                                                  SetParam(CNvEncoderSetkey key, void* param);
	void                                                 GetParam(CNvEncoderGetKey key, void* param);
	int                                                  Start();
	void                                                 Stop();

    //int                                                  EncodeMain(int argc, char **argv);

protected:
    CNvHWEncoder                                        *m_pNvHWEncoder;
    uint32_t                                             m_uEncodeBufferCount;
    uint32_t                                             m_uPicStruct;
    void*                                                m_pDevice;
	unsigned char*                                       m_pOutData;
	unsigned int                                         m_iOutSize;
#if defined(NV_WINDOWS)
    //IDirect3D9                                          *m_pD3D;
#endif

    //EncodeConfig                                         m_stEncoderInput;
    EncodeBuffer                                         m_stEncodeBuffer[MAX_ENCODE_QUEUE];
    CNvQueue<EncodeBuffer>                               m_EncodeBufferQueue;
    EncodeOutputBuffer                                   m_stEOSOutputBfr; 
	EncodeConfig encodeConfig;

	int64_t vPts;
	int64_t vDts;
	int iFrameIntervalCount;
	bool isStart;

protected:
    NVENCSTATUS                                          Deinitialize(uint32_t devicetype);
    NVENCSTATUS                                          EncodeFrame(EncodeFrameConfig *pEncodeFrame, bool bFlush=false, uint32_t width=0, uint32_t height=0, long long pts = 0);
    //NVENCSTATUS                                          InitD3D9(uint32_t deviceID = 0);
    //NVENCSTATUS                                          InitD3D11(uint32_t deviceID = 0);
    //NVENCSTATUS                                          InitD3D10(uint32_t deviceID = 0);
    NVENCSTATUS                                          InitCuda(uint32_t deviceID = 0);
    NVENCSTATUS                                          AllocateIOBuffers(uint32_t uInputWidth, uint32_t uInputHeight, uint32_t isYuv444);
    NVENCSTATUS                                          ReleaseIOBuffers();
    unsigned char*                                       LockInputBuffer(void * hInputSurface, uint32_t *pLockedPitch);
    NVENCSTATUS                                          FlushEncoder();
};

// NVEncodeAPI entry point
typedef NVENCSTATUS (NVENCAPI *MYPROC)(NV_ENCODE_API_FUNCTION_LIST*); 
