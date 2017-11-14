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

#include "NvHWEncoder.h"

#include <algorithm>

//extern "C"
//{
//#include <stdint.h>
//#include "x264.h"
//}

NVENCSTATUS CNvHWEncoder::NvEncOpenEncodeSession(void* device, uint32_t deviceType)
{
    NVENCSTATUS nvStatus = NV_ENC_SUCCESS;

    nvStatus = m_pEncodeAPI->nvEncOpenEncodeSession(device, deviceType, &m_hEncoder);
    if (nvStatus != NV_ENC_SUCCESS)
    {
        //assert(0);
    }

    return nvStatus;
}

NVENCSTATUS CNvHWEncoder::NvEncGetEncodeGUIDCount(uint32_t* encodeGUIDCount)
{
    NVENCSTATUS nvStatus = NV_ENC_SUCCESS;

    nvStatus = m_pEncodeAPI->nvEncGetEncodeGUIDCount(m_hEncoder, encodeGUIDCount);
    if (nvStatus != NV_ENC_SUCCESS)
    {
        //assert(0);
    }

    return nvStatus;
}

NVENCSTATUS CNvHWEncoder::NvEncGetEncodeProfileGUIDCount(GUID encodeGUID, uint32_t* encodeProfileGUIDCount)
{
    NVENCSTATUS nvStatus = NV_ENC_SUCCESS;

    nvStatus = m_pEncodeAPI->nvEncGetEncodeProfileGUIDCount(m_hEncoder, encodeGUID, encodeProfileGUIDCount);
    if (nvStatus != NV_ENC_SUCCESS)
    {
        //assert(0);
    }

    return nvStatus;
}

NVENCSTATUS CNvHWEncoder::NvEncGetEncodeProfileGUIDs(GUID encodeGUID, GUID* profileGUIDs, uint32_t guidArraySize, uint32_t* GUIDCount)
{
    NVENCSTATUS nvStatus = NV_ENC_SUCCESS;

    nvStatus = m_pEncodeAPI->nvEncGetEncodeProfileGUIDs(m_hEncoder, encodeGUID, profileGUIDs, guidArraySize, GUIDCount);
    if (nvStatus != NV_ENC_SUCCESS)
    {
        //assert(0);
    }

    return nvStatus;
}

NVENCSTATUS CNvHWEncoder::NvEncGetEncodeGUIDs(GUID* GUIDs, uint32_t guidArraySize, uint32_t* GUIDCount)
{
    NVENCSTATUS nvStatus = NV_ENC_SUCCESS;

    nvStatus = m_pEncodeAPI->nvEncGetEncodeGUIDs(m_hEncoder, GUIDs, guidArraySize, GUIDCount);
    if (nvStatus != NV_ENC_SUCCESS)
    {
        //assert(0);
    }

    return nvStatus;
}

NVENCSTATUS CNvHWEncoder::NvEncGetInputFormatCount(GUID encodeGUID, uint32_t* inputFmtCount)
{
    NVENCSTATUS nvStatus = NV_ENC_SUCCESS;

    nvStatus = m_pEncodeAPI->nvEncGetInputFormatCount(m_hEncoder, encodeGUID, inputFmtCount);
    if (nvStatus != NV_ENC_SUCCESS)
    {
        //assert(0);
    }

    return nvStatus;
}

NVENCSTATUS CNvHWEncoder::NvEncGetInputFormats(GUID encodeGUID, NV_ENC_BUFFER_FORMAT* inputFmts, uint32_t inputFmtArraySize, uint32_t* inputFmtCount)
{
    NVENCSTATUS nvStatus = NV_ENC_SUCCESS;

    nvStatus = m_pEncodeAPI->nvEncGetInputFormats(m_hEncoder, encodeGUID, inputFmts, inputFmtArraySize, inputFmtCount);
    if (nvStatus != NV_ENC_SUCCESS)
    {
        //assert(0);
    }

    return nvStatus;
}

NVENCSTATUS CNvHWEncoder::NvEncGetEncodeCaps(GUID encodeGUID, NV_ENC_CAPS_PARAM* capsParam, int* capsVal)
{
    NVENCSTATUS nvStatus = NV_ENC_SUCCESS;

    nvStatus = m_pEncodeAPI->nvEncGetEncodeCaps(m_hEncoder, encodeGUID, capsParam, capsVal);
    if (nvStatus != NV_ENC_SUCCESS)
    {
        //assert(0);
    }

    return nvStatus;
}

NVENCSTATUS CNvHWEncoder::NvEncGetEncodePresetCount(GUID encodeGUID, uint32_t* encodePresetGUIDCount)
{
    NVENCSTATUS nvStatus = NV_ENC_SUCCESS;

    nvStatus = m_pEncodeAPI->nvEncGetEncodePresetCount(m_hEncoder, encodeGUID, encodePresetGUIDCount);
    if (nvStatus != NV_ENC_SUCCESS)
    {
        //assert(0);
    }

    return nvStatus;
}

NVENCSTATUS CNvHWEncoder::NvEncGetEncodePresetGUIDs(GUID encodeGUID, GUID* presetGUIDs, uint32_t guidArraySize, uint32_t* encodePresetGUIDCount)
{
    NVENCSTATUS nvStatus = NV_ENC_SUCCESS;

    nvStatus = m_pEncodeAPI->nvEncGetEncodePresetGUIDs(m_hEncoder, encodeGUID, presetGUIDs, guidArraySize, encodePresetGUIDCount);
    if (nvStatus != NV_ENC_SUCCESS)
    {
        //assert(0);
    }

    return nvStatus;
}

NVENCSTATUS CNvHWEncoder::NvEncGetEncodePresetConfig(GUID encodeGUID, GUID  presetGUID, NV_ENC_PRESET_CONFIG* presetConfig)
{
    NVENCSTATUS nvStatus = NV_ENC_SUCCESS;

    nvStatus = m_pEncodeAPI->nvEncGetEncodePresetConfig(m_hEncoder, encodeGUID, presetGUID, presetConfig);
    if (nvStatus != NV_ENC_SUCCESS)
    {
        //assert(0);
    }

    return nvStatus;
}

NVENCSTATUS CNvHWEncoder::NvEncCreateInputBuffer(uint32_t width, uint32_t height, void** inputBuffer, uint32_t isYuv444)
{
    NVENCSTATUS nvStatus = NV_ENC_SUCCESS;
    NV_ENC_CREATE_INPUT_BUFFER createInputBufferParams;

    memset(&createInputBufferParams, 0, sizeof(createInputBufferParams));
    SET_VER(createInputBufferParams, NV_ENC_CREATE_INPUT_BUFFER);

    createInputBufferParams.width = width;
    createInputBufferParams.height = height;
    createInputBufferParams.memoryHeap = NV_ENC_MEMORY_HEAP_SYSMEM_CACHED;
    createInputBufferParams.bufferFmt = isYuv444 ? NV_ENC_BUFFER_FORMAT_YUV444_PL : NV_ENC_BUFFER_FORMAT_NV12_PL;

    nvStatus = m_pEncodeAPI->nvEncCreateInputBuffer(m_hEncoder, &createInputBufferParams);
    if (nvStatus != NV_ENC_SUCCESS)
    {
        //assert(0);
    }

    *inputBuffer = createInputBufferParams.inputBuffer;

    return nvStatus;
}

NVENCSTATUS CNvHWEncoder::NvEncDestroyInputBuffer(NV_ENC_INPUT_PTR inputBuffer)
{
    NVENCSTATUS nvStatus = NV_ENC_SUCCESS;

    if (inputBuffer)
    {
        nvStatus = m_pEncodeAPI->nvEncDestroyInputBuffer(m_hEncoder, inputBuffer);
        if (nvStatus != NV_ENC_SUCCESS)
        {
            //assert(0);
        }
    }

    return nvStatus;
}

NVENCSTATUS CNvHWEncoder::NvEncCreateBitstreamBuffer(uint32_t size, void** bitstreamBuffer)
{
    NVENCSTATUS nvStatus = NV_ENC_SUCCESS;
    NV_ENC_CREATE_BITSTREAM_BUFFER createBitstreamBufferParams;

    memset(&createBitstreamBufferParams, 0, sizeof(createBitstreamBufferParams));
    SET_VER(createBitstreamBufferParams, NV_ENC_CREATE_BITSTREAM_BUFFER);

    createBitstreamBufferParams.size = size;
    createBitstreamBufferParams.memoryHeap = NV_ENC_MEMORY_HEAP_SYSMEM_CACHED;

    nvStatus = m_pEncodeAPI->nvEncCreateBitstreamBuffer(m_hEncoder, &createBitstreamBufferParams);
    if (nvStatus != NV_ENC_SUCCESS)
    {
        //assert(0);
    }

    *bitstreamBuffer = createBitstreamBufferParams.bitstreamBuffer;

    return nvStatus;
}

NVENCSTATUS CNvHWEncoder::NvEncDestroyBitstreamBuffer(NV_ENC_OUTPUT_PTR bitstreamBuffer)
{
    NVENCSTATUS nvStatus = NV_ENC_SUCCESS;

    if (bitstreamBuffer)
    {
        nvStatus = m_pEncodeAPI->nvEncDestroyBitstreamBuffer(m_hEncoder, bitstreamBuffer);
        if (nvStatus != NV_ENC_SUCCESS)
        {
            //assert(0);
        }
    }

    return nvStatus;
}

NVENCSTATUS CNvHWEncoder::NvEncLockBitstream(NV_ENC_LOCK_BITSTREAM* lockBitstreamBufferParams)
{
    NVENCSTATUS nvStatus = NV_ENC_SUCCESS;

    nvStatus = m_pEncodeAPI->nvEncLockBitstream(m_hEncoder, lockBitstreamBufferParams);
    if (nvStatus != NV_ENC_SUCCESS)
    {
        //assert(0);
    }

    return nvStatus;
}

NVENCSTATUS CNvHWEncoder::NvEncUnlockBitstream(NV_ENC_OUTPUT_PTR bitstreamBuffer)
{
    NVENCSTATUS nvStatus = NV_ENC_SUCCESS;

    nvStatus = m_pEncodeAPI->nvEncUnlockBitstream(m_hEncoder, bitstreamBuffer);
    if (nvStatus != NV_ENC_SUCCESS)
    {
        //assert(0);
    }

    return nvStatus;
}

NVENCSTATUS CNvHWEncoder::NvEncLockInputBuffer(void* inputBuffer, void** bufferDataPtr, uint32_t* pitch)
{
    NVENCSTATUS nvStatus = NV_ENC_SUCCESS;
    NV_ENC_LOCK_INPUT_BUFFER lockInputBufferParams;

    memset(&lockInputBufferParams, 0, sizeof(lockInputBufferParams));
    SET_VER(lockInputBufferParams, NV_ENC_LOCK_INPUT_BUFFER);

    lockInputBufferParams.inputBuffer = inputBuffer;
    nvStatus = m_pEncodeAPI->nvEncLockInputBuffer(m_hEncoder, &lockInputBufferParams);
    if (nvStatus != NV_ENC_SUCCESS)
    {
        //assert(0);
    }

    *bufferDataPtr = lockInputBufferParams.bufferDataPtr;
    *pitch = lockInputBufferParams.pitch;

    return nvStatus;
}

NVENCSTATUS CNvHWEncoder::NvEncUnlockInputBuffer(NV_ENC_INPUT_PTR inputBuffer)
{
    NVENCSTATUS nvStatus = NV_ENC_SUCCESS;

    nvStatus = m_pEncodeAPI->nvEncUnlockInputBuffer(m_hEncoder, inputBuffer);
    if (nvStatus != NV_ENC_SUCCESS)
    {
        //assert(0);
    }

    return nvStatus;
}

NVENCSTATUS CNvHWEncoder::NvEncGetEncodeStats(NV_ENC_STAT* encodeStats)
{
    NVENCSTATUS nvStatus = NV_ENC_SUCCESS;

    nvStatus = m_pEncodeAPI->nvEncGetEncodeStats(m_hEncoder, encodeStats);
    if (nvStatus != NV_ENC_SUCCESS)
    {
        //assert(0);
    }

    return nvStatus;
}

NVENCSTATUS CNvHWEncoder::NvEncGetSequenceParams(NV_ENC_SEQUENCE_PARAM_PAYLOAD* sequenceParamPayload)
{
    NVENCSTATUS nvStatus = NV_ENC_SUCCESS;

    nvStatus = m_pEncodeAPI->nvEncGetSequenceParams(m_hEncoder, sequenceParamPayload);
    if (nvStatus != NV_ENC_SUCCESS)
    {
        //assert(0);
    }

    return nvStatus;
}

NVENCSTATUS CNvHWEncoder::NvEncRegisterAsyncEvent(void** completionEvent)
{
    NVENCSTATUS nvStatus = NV_ENC_SUCCESS;
    NV_ENC_EVENT_PARAMS eventParams;

    memset(&eventParams, 0, sizeof(eventParams));
    SET_VER(eventParams, NV_ENC_EVENT_PARAMS);

#if defined (NV_WINDOWS)
    eventParams.completionEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
#else
    eventParams.completionEvent = NULL;
#endif
    nvStatus = m_pEncodeAPI->nvEncRegisterAsyncEvent(m_hEncoder, &eventParams);
    if (nvStatus != NV_ENC_SUCCESS)
    {
        //assert(0);
    }

    *completionEvent = eventParams.completionEvent;

    return nvStatus;
}

NVENCSTATUS CNvHWEncoder::NvEncUnregisterAsyncEvent(void* completionEvent)
{
    NVENCSTATUS nvStatus = NV_ENC_SUCCESS;
    NV_ENC_EVENT_PARAMS eventParams;

    if (completionEvent)
    {
        memset(&eventParams, 0, sizeof(eventParams));
        SET_VER(eventParams, NV_ENC_EVENT_PARAMS);

        eventParams.completionEvent = completionEvent;

        nvStatus = m_pEncodeAPI->nvEncUnregisterAsyncEvent(m_hEncoder, &eventParams);
        if (nvStatus != NV_ENC_SUCCESS)
        {
            //assert(0);
        }
    }

    return nvStatus;
}

NVENCSTATUS CNvHWEncoder::NvEncMapInputResource(void* registeredResource, void** mappedResource)
{
    NVENCSTATUS nvStatus = NV_ENC_SUCCESS;
    NV_ENC_MAP_INPUT_RESOURCE mapInputResParams;

    memset(&mapInputResParams, 0, sizeof(mapInputResParams));
    SET_VER(mapInputResParams, NV_ENC_MAP_INPUT_RESOURCE);

    mapInputResParams.registeredResource = registeredResource;

    nvStatus = m_pEncodeAPI->nvEncMapInputResource(m_hEncoder, &mapInputResParams);
    if (nvStatus != NV_ENC_SUCCESS)
    {
        //assert(0);
    }

    *mappedResource = mapInputResParams.mappedResource;

    return nvStatus;
}

NVENCSTATUS CNvHWEncoder::NvEncUnmapInputResource(NV_ENC_INPUT_PTR mappedInputBuffer)
{
    NVENCSTATUS nvStatus = NV_ENC_SUCCESS;
    
    if (mappedInputBuffer)
    {
        nvStatus = m_pEncodeAPI->nvEncUnmapInputResource(m_hEncoder, mappedInputBuffer);
        if (nvStatus != NV_ENC_SUCCESS)
        {
            //assert(0);
        }
    }

    return nvStatus;
}

NVENCSTATUS CNvHWEncoder::NvEncDestroyEncoder()
{
    NVENCSTATUS nvStatus = NV_ENC_SUCCESS;

    if (m_bEncoderInitialized)
    {
        nvStatus = m_pEncodeAPI->nvEncDestroyEncoder(m_hEncoder);

        m_bEncoderInitialized = false;
    }

    return nvStatus;
}

NVENCSTATUS CNvHWEncoder::NvEncInvalidateRefFrames(const NvEncPictureCommand *pEncPicCommand)
{
    NVENCSTATUS nvStatus = NV_ENC_SUCCESS;

    for (uint32_t i = 0; i < pEncPicCommand->numRefFramesToInvalidate; i++)
    {
        nvStatus = m_pEncodeAPI->nvEncInvalidateRefFrames(m_hEncoder, pEncPicCommand->refFrameNumbers[i]);
    }

    return nvStatus;
}

NVENCSTATUS CNvHWEncoder::NvEncOpenEncodeSessionEx(void* device, NV_ENC_DEVICE_TYPE deviceType)
{
    NVENCSTATUS nvStatus = NV_ENC_SUCCESS;
    NV_ENC_OPEN_ENCODE_SESSION_EX_PARAMS openSessionExParams;

    memset(&openSessionExParams, 0, sizeof(openSessionExParams));
    SET_VER(openSessionExParams, NV_ENC_OPEN_ENCODE_SESSION_EX_PARAMS);

    openSessionExParams.device = device;
    openSessionExParams.deviceType = deviceType;
    openSessionExParams.reserved = NULL;
    openSessionExParams.apiVersion = NVENCAPI_VERSION;

    nvStatus = m_pEncodeAPI->nvEncOpenEncodeSessionEx(&openSessionExParams, &m_hEncoder);
    if (nvStatus != NV_ENC_SUCCESS)
    {
        //assert(0);
    }

    return nvStatus;
}

NVENCSTATUS CNvHWEncoder::NvEncRegisterResource(NV_ENC_INPUT_RESOURCE_TYPE resourceType, void* resourceToRegister, uint32_t width, uint32_t height, uint32_t pitch, void** registeredResource)
{
    NVENCSTATUS nvStatus = NV_ENC_SUCCESS;
    NV_ENC_REGISTER_RESOURCE registerResParams;

    memset(&registerResParams, 0, sizeof(registerResParams));
    SET_VER(registerResParams, NV_ENC_REGISTER_RESOURCE);

    registerResParams.resourceType = resourceType;
    registerResParams.resourceToRegister = resourceToRegister;
    registerResParams.width = width;
    registerResParams.height = height;
    registerResParams.pitch = pitch;

    nvStatus = m_pEncodeAPI->nvEncRegisterResource(m_hEncoder, &registerResParams);
    if (nvStatus != NV_ENC_SUCCESS)
    {
        //assert(0);
    }

    *registeredResource = registerResParams.registeredResource;

    return nvStatus;
}

NVENCSTATUS CNvHWEncoder::NvEncUnregisterResource(NV_ENC_REGISTERED_PTR registeredRes)
{
    NVENCSTATUS nvStatus = NV_ENC_SUCCESS;

    nvStatus = m_pEncodeAPI->nvEncUnregisterResource(m_hEncoder, registeredRes);
    if (nvStatus != NV_ENC_SUCCESS)
    {
        //assert(0);
    }

    return nvStatus;
}

NVENCSTATUS CNvHWEncoder::NvEncReconfigureEncoder(const NvEncPictureCommand *pEncPicCommand)
{
    NVENCSTATUS nvStatus = NV_ENC_SUCCESS;

    if (pEncPicCommand->bBitrateChangePending || pEncPicCommand->bResolutionChangePending)
    {
        if (pEncPicCommand->bResolutionChangePending)
        {
            m_uCurWidth = pEncPicCommand->newWidth;
            m_uCurHeight = pEncPicCommand->newHeight;
            if ((m_uCurWidth > m_uMaxWidth) || (m_uCurHeight > m_uMaxHeight))
            {
                return NV_ENC_ERR_INVALID_PARAM;
            }
            m_stCreateEncodeParams.encodeWidth = m_uCurWidth;
            m_stCreateEncodeParams.encodeHeight = m_uCurHeight;
            m_stCreateEncodeParams.darWidth = m_uCurWidth;
            m_stCreateEncodeParams.darHeight = m_uCurHeight;
        }

        if (pEncPicCommand->bBitrateChangePending)
        {
            m_stEncodeConfig.rcParams.averageBitRate = pEncPicCommand->newBitrate;
            m_stEncodeConfig.rcParams.maxBitRate = pEncPicCommand->newBitrate;
            m_stEncodeConfig.rcParams.vbvBufferSize = pEncPicCommand->newVBVSize != 0 ? pEncPicCommand->newVBVSize : (pEncPicCommand->newBitrate * m_stCreateEncodeParams.frameRateDen) / m_stCreateEncodeParams.frameRateNum;
            m_stEncodeConfig.rcParams.vbvInitialDelay = m_stEncodeConfig.rcParams.vbvBufferSize;
        }

        NV_ENC_RECONFIGURE_PARAMS stReconfigParams;
        memset(&stReconfigParams, 0, sizeof(stReconfigParams));
        memcpy(&stReconfigParams.reInitEncodeParams, &m_stCreateEncodeParams, sizeof(m_stCreateEncodeParams));
        stReconfigParams.version = NV_ENC_RECONFIGURE_PARAMS_VER;
        stReconfigParams.forceIDR = pEncPicCommand->bResolutionChangePending ? 1 : 0;

        nvStatus = m_pEncodeAPI->nvEncReconfigureEncoder(m_hEncoder, &stReconfigParams);
        if (nvStatus != NV_ENC_SUCCESS)
        {
            //assert(0);
        }
    }

    return nvStatus;
}

CNvHWEncoder::CNvHWEncoder()
{
    m_hEncoder = NULL;
    m_bEncoderInitialized = false;
    m_pEncodeAPI = NULL;
    m_hinstLib = NULL;
#ifdef _WRITE_FILE_
    m_fOutput = NULL;
#endif
    m_EncodeIdx = 0;
    m_uCurWidth = 0;
    m_uCurHeight = 0;
    m_uMaxWidth = 0;
    m_uMaxHeight = 0;

    memset(&m_stCreateEncodeParams, 0, sizeof(m_stCreateEncodeParams));
    SET_VER(m_stCreateEncodeParams, NV_ENC_INITIALIZE_PARAMS);

    memset(&m_stEncodeConfig, 0, sizeof(m_stEncodeConfig));
    SET_VER(m_stEncodeConfig, NV_ENC_CONFIG);

	spsppsData = NULL;
}

CNvHWEncoder::~CNvHWEncoder()
{
    // clean up encode API resources here
    if (m_pEncodeAPI)
    {
        delete m_pEncodeAPI;
        m_pEncodeAPI = NULL;
    }

	if (spsppsData) {
		delete[] spsppsData;
		spsppsData = NULL;
	}

    if (m_hinstLib)
    {
#if defined (NV_WINDOWS)
        FreeLibrary(m_hinstLib);
#else
        dlclose(m_hinstLib);
#endif

        m_hinstLib = NULL;
    }
}

NVENCSTATUS CNvHWEncoder::ValidateEncodeGUID (GUID inputCodecGuid)
{
    unsigned int i, codecFound, encodeGUIDCount, encodeGUIDArraySize;
    NVENCSTATUS nvStatus;
    GUID *encodeGUIDArray;

    nvStatus = m_pEncodeAPI->nvEncGetEncodeGUIDCount(m_hEncoder, &encodeGUIDCount);
    if (nvStatus != NV_ENC_SUCCESS)
    {
        //assert(0);
        return nvStatus;
    }

    encodeGUIDArray = new GUID[encodeGUIDCount];
    memset(encodeGUIDArray, 0, sizeof(GUID)* encodeGUIDCount);

    encodeGUIDArraySize = 0;
    nvStatus = m_pEncodeAPI->nvEncGetEncodeGUIDs(m_hEncoder, encodeGUIDArray, encodeGUIDCount, &encodeGUIDArraySize);
    if (nvStatus != NV_ENC_SUCCESS)
    {
        delete[] encodeGUIDArray;
        //assert(0);
        return nvStatus;
    }

    //assert(encodeGUIDArraySize <= encodeGUIDCount);

    codecFound = 0;
    for (i = 0; i < encodeGUIDArraySize; i++)
    {
        if (inputCodecGuid == encodeGUIDArray[i])
        {
            codecFound = 1;
            break;
        }
    }

    delete[] encodeGUIDArray;

    if (codecFound)
        return NV_ENC_SUCCESS;
    else
        return NV_ENC_ERR_INVALID_PARAM;
}

NVENCSTATUS CNvHWEncoder::ValidatePresetGUID(GUID inputPresetGuid, GUID inputCodecGuid)
{
    uint32_t i, presetFound, presetGUIDCount, presetGUIDArraySize;
    NVENCSTATUS nvStatus;
    GUID *presetGUIDArray;

    nvStatus = m_pEncodeAPI->nvEncGetEncodePresetCount(m_hEncoder, inputCodecGuid, &presetGUIDCount);
    if (nvStatus != NV_ENC_SUCCESS)
    {
        //assert(0);
        return nvStatus;
    }

    presetGUIDArray = new GUID[presetGUIDCount];
    memset(presetGUIDArray, 0, sizeof(GUID)* presetGUIDCount);

    presetGUIDArraySize = 0;
    nvStatus = m_pEncodeAPI->nvEncGetEncodePresetGUIDs(m_hEncoder, inputCodecGuid, presetGUIDArray, presetGUIDCount, &presetGUIDArraySize);
    if (nvStatus != NV_ENC_SUCCESS)
    {
        //assert(0);
        delete[] presetGUIDArray;
        return nvStatus;
    }

    //assert(presetGUIDArraySize <= presetGUIDCount);

    presetFound = 0;
    for (i = 0; i < presetGUIDArraySize; i++)
    {
        if (inputPresetGuid == presetGUIDArray[i])
        {
            presetFound = 1;
            break;
        }
    }

    delete[] presetGUIDArray;

    if (presetFound)
        return NV_ENC_SUCCESS;
    else
        return NV_ENC_ERR_INVALID_PARAM;
}

//----------------------------

enum ColorPrimaries
{
	ColorPrimaries_BT709 = 1,
	ColorPrimaries_Unspecified,
	ColorPrimaries_BT470M = 4,
	ColorPrimaries_BT470BG,
	ColorPrimaries_SMPTE170M,
	ColorPrimaries_SMPTE240M,
	ColorPrimaries_Film,
	ColorPrimaries_BT2020
};

enum ColorTransfer
{
	ColorTransfer_BT709 = 1,
	ColorTransfer_Unspecified,
	ColorTransfer_BT470M = 4,
	ColorTransfer_BT470BG,
	ColorTransfer_SMPTE170M,
	ColorTransfer_SMPTE240M,
	ColorTransfer_Linear,
	ColorTransfer_Log100,
	ColorTransfer_Log316,
	ColorTransfer_IEC6196624,
	ColorTransfer_BT1361,
	ColorTransfer_IEC6196621,
	ColorTransfer_BT202010,
	ColorTransfer_BT202012
};

enum ColorMatrix
{
	ColorMatrix_GBR = 0,
	ColorMatrix_BT709,
	ColorMatrix_Unspecified,
	ColorMatrix_BT470M = 4,
	ColorMatrix_BT470BG,
	ColorMatrix_SMPTE170M,
	ColorMatrix_SMPTE240M,
	ColorMatrix_YCgCo,
	ColorMatrix_BT2020NCL,
	ColorMatrix_BT2020CL
};

struct ColorDescription
{
	int fullRange;
	int primaries;
	int transfer;
	int matrix;
};

ColorDescription colorDesc;

NVENCSTATUS CNvHWEncoder::CreateEncoder(const EncodeConfig *pEncCfg)
{
    NVENCSTATUS nvStatus = NV_ENC_SUCCESS;

    if (pEncCfg == NULL)
    {
        return NV_ENC_ERR_INVALID_PARAM;
    }

    m_uCurWidth = pEncCfg->width;
    m_uCurHeight = pEncCfg->height;

    m_uMaxWidth = (pEncCfg->maxWidth > 0 ? pEncCfg->maxWidth : pEncCfg->width);
    m_uMaxHeight = (pEncCfg->maxHeight > 0 ? pEncCfg->maxHeight : pEncCfg->height);

    if ((m_uCurWidth > m_uMaxWidth) || (m_uCurHeight > m_uMaxHeight)) {
        return NV_ENC_ERR_INVALID_PARAM;
    }

#ifdef _WRITE_FILE_
    m_fOutput = pEncCfg->fOutput;
#endif

    if (!pEncCfg->width || !pEncCfg->height)
    {
        return NV_ENC_ERR_INVALID_PARAM;
    }

    GUID inputCodecGUID = pEncCfg->codec == NV_ENC_H264 ? NV_ENC_CODEC_H264_GUID : NV_ENC_CODEC_HEVC_GUID;
    nvStatus = ValidateEncodeGUID(inputCodecGUID);
    if (nvStatus != NV_ENC_SUCCESS)
    {
        PRINTERR("codec not supported \n");
        return nvStatus;
    }

    codecGUID = inputCodecGUID;

    m_stCreateEncodeParams.encodeGUID = inputCodecGUID;
    m_stCreateEncodeParams.presetGUID = pEncCfg->presetGUID;
    m_stCreateEncodeParams.encodeWidth = pEncCfg->width;
    m_stCreateEncodeParams.encodeHeight = pEncCfg->height;

    m_stCreateEncodeParams.darWidth = pEncCfg->width;
    m_stCreateEncodeParams.darHeight = pEncCfg->height;
    m_stCreateEncodeParams.frameRateNum = pEncCfg->fps;
    m_stCreateEncodeParams.frameRateDen = 1;
#if defined(NV_WINDOWS)
    m_stCreateEncodeParams.enableEncodeAsync = 1;
#else
    m_stCreateEncodeParams.enableEncodeAsync = 0;
#endif
    m_stCreateEncodeParams.enablePTD = 1;
    m_stCreateEncodeParams.reportSliceOffsets = 0;
    m_stCreateEncodeParams.enableSubFrameWrite = 0;
    m_stCreateEncodeParams.encodeConfig = &m_stEncodeConfig;
    m_stCreateEncodeParams.maxEncodeWidth = m_uMaxWidth;
    m_stCreateEncodeParams.maxEncodeHeight = m_uMaxHeight;

    // apply preset
    NV_ENC_PRESET_CONFIG stPresetCfg;
    memset(&stPresetCfg, 0, sizeof(NV_ENC_PRESET_CONFIG));
    SET_VER(stPresetCfg, NV_ENC_PRESET_CONFIG);
    SET_VER(stPresetCfg.presetCfg, NV_ENC_CONFIG);

    nvStatus = m_pEncodeAPI->nvEncGetEncodePresetConfig(m_hEncoder, m_stCreateEncodeParams.encodeGUID, m_stCreateEncodeParams.presetGUID, &stPresetCfg);
    if (nvStatus != NV_ENC_SUCCESS)
    {
        //assert(0);
        return nvStatus;
    }
    memcpy(&m_stEncodeConfig, &stPresetCfg.presetCfg, sizeof(NV_ENC_CONFIG));

	if (pEncCfg->bitrate != -1)
	{
		m_stEncodeConfig.rcParams.averageBitRate = pEncCfg->bitrate * 1000;
		m_stEncodeConfig.rcParams.maxBitRate = pEncCfg->bitrate * 1000;
		m_stEncodeConfig.rcParams.vbvBufferSize = pEncCfg->bitrate * 1000;
		m_stEncodeConfig.rcParams.vbvInitialDelay = pEncCfg->bitrate * 1000;
	}

	//m_stEncodeConfig.rcParams.rateControlMode = (NV_ENC_PARAMS_RC_MODE)pEncCfg->rcMode;

	//m_stEncodeConfig.frameIntervalP = 1;  // no B frame
	//m_stEncodeConfig.frameIntervalP = 3;  // 2 B frame
	m_stEncodeConfig.frameIntervalP = pEncCfg->numB + 1;

	if (pEncCfg->codec == NV_ENC_HEVC) {
		m_stEncodeConfig.encodeCodecConfig.hevcConfig.outputPictureTimingSEI = 0;
		m_stEncodeConfig.encodeCodecConfig.hevcConfig.disableSPSPPS = 1;

		m_stEncodeConfig.encodeCodecConfig.hevcConfig.disableDeblockAcrossSliceBoundary = 0;
		m_stEncodeConfig.encodeCodecConfig.hevcConfig.outputBufferingPeriodSEI = 0;
		
		// one slice one pic
		m_stEncodeConfig.encodeCodecConfig.hevcConfig.sliceMode = 0;
		m_stEncodeConfig.encodeCodecConfig.hevcConfig.sliceModeData = 0;
	}
	else {
		m_stEncodeConfig.encodeCodecConfig.h264Config.outputPictureTimingSEI = 0;
		m_stEncodeConfig.encodeCodecConfig.h264Config.disableSPSPPS = 1;

		bool bUseCFR = false;
		m_stEncodeConfig.encodeCodecConfig.h264Config.enableVFR = bUseCFR ? 0 : 1;
	}

	m_stEncodeConfig.frameFieldMode = NV_ENC_PARAMS_FRAME_FIELD_MODE_FRAME;
	
	// two pass, vbr
	bool is2PassRC = false;
	if ((NV_ENC_PARAMS_RC_MODE)pEncCfg->rcMode == NV_ENC_PARAMS_RC_CBR)
	{
		m_stEncodeConfig.rcParams.rateControlMode = NV_ENC_PARAMS_RC_CBR;
		if (is2PassRC)
			m_stEncodeConfig.rcParams.rateControlMode = NV_ENC_PARAMS_RC_2_PASS_QUALITY;

		if (pEncCfg->codec == NV_ENC_HEVC) {
			// do nothing
		}
		else {
			m_stEncodeConfig.encodeCodecConfig.h264Config.adaptiveTransformMode = NV_ENC_H264_ADAPTIVE_TRANSFORM_ENABLE;
			m_stEncodeConfig.encodeCodecConfig.h264Config.fmoMode = NV_ENC_H264_FMO_DISABLE;
		}
	}
	else
	{
		m_stEncodeConfig.rcParams.rateControlMode = NV_ENC_PARAMS_RC_VBR_MINQP;
		if (is2PassRC)
			m_stEncodeConfig.rcParams.rateControlMode = NV_ENC_PARAMS_RC_2_PASS_VBR;

		// vbr
		m_stEncodeConfig.rcParams.enableMinQP = 1;
		m_stEncodeConfig.rcParams.minQP.qpInterP = 32 - pEncCfg->quality;
		m_stEncodeConfig.rcParams.minQP.qpIntra = 32 - pEncCfg->quality;

		m_stEncodeConfig.rcParams.enableMaxQP = 0;
		m_stEncodeConfig.rcParams.maxQP.qpIntra = 25;
		m_stEncodeConfig.rcParams.maxQP.qpInterP = 25;
	}

	// high profile
	m_stEncodeConfig.profileGUID = pEncCfg->profileGUID;

	m_stEncodeConfig.encodeCodecConfig.h264Config.bdirectMode = m_stEncodeConfig.frameIntervalP > 1 ? NV_ENC_H264_BDIRECT_MODE_TEMPORAL : NV_ENC_H264_BDIRECT_MODE_DISABLE;
	m_stEncodeConfig.gopLength = pEncCfg->gopLength;

	colorDesc.fullRange = 0;
	colorDesc.matrix = 1;
	colorDesc.transfer = 13;
	colorDesc.primaries = 1;

	m_stEncodeConfig.encodeCodecConfig.h264Config.h264VUIParameters.videoSignalTypePresentFlag = 1;
	m_stEncodeConfig.encodeCodecConfig.h264Config.h264VUIParameters.colourDescriptionPresentFlag = 1;
	m_stEncodeConfig.encodeCodecConfig.h264Config.h264VUIParameters.colourMatrix = colorDesc.matrix;
	m_stEncodeConfig.encodeCodecConfig.h264Config.h264VUIParameters.videoFullRangeFlag = colorDesc.fullRange;
	m_stEncodeConfig.encodeCodecConfig.h264Config.h264VUIParameters.videoFormat = 5;
	m_stEncodeConfig.encodeCodecConfig.h264Config.h264VUIParameters.colourPrimaries = colorDesc.primaries;
	m_stEncodeConfig.encodeCodecConfig.h264Config.h264VUIParameters.transferCharacteristics = colorDesc.transfer;

	// one slice one pic
	m_stEncodeConfig.encodeCodecConfig.h264Config.sliceMode = 0;
	m_stEncodeConfig.encodeCodecConfig.h264Config.sliceModeData = 0;

	m_stEncodeConfig.mvPrecision = NV_ENC_MV_PRECISION_QUARTER_PEL;

	if (pEncCfg->isYuv444)
	{
		if (pEncCfg->codec == NV_ENC_HEVC) {
			m_stEncodeConfig.encodeCodecConfig.hevcConfig.chromaFormatIDC = 3;
		}
		else {
			m_stEncodeConfig.encodeCodecConfig.h264Config.chromaFormatIDC = 3;
		}
	}
	else
	{
		if (pEncCfg->codec == NV_ENC_HEVC) {
			m_stEncodeConfig.encodeCodecConfig.hevcConfig.chromaFormatIDC = 1;
		}
		else {
			m_stEncodeConfig.encodeCodecConfig.h264Config.chromaFormatIDC = 1;
		}
    }

    if (pEncCfg->intraRefreshEnableFlag)
    {
        if (pEncCfg->codec == NV_ENC_HEVC)
        {
            m_stEncodeConfig.encodeCodecConfig.hevcConfig.enableIntraRefresh = 1;
            m_stEncodeConfig.encodeCodecConfig.hevcConfig.intraRefreshPeriod = pEncCfg->intraRefreshPeriod;
            m_stEncodeConfig.encodeCodecConfig.hevcConfig.intraRefreshCnt = pEncCfg->intraRefreshDuration;
        }
        else
        {
            m_stEncodeConfig.encodeCodecConfig.h264Config.enableIntraRefresh = 1;
            m_stEncodeConfig.encodeCodecConfig.h264Config.intraRefreshPeriod = pEncCfg->intraRefreshPeriod;
            m_stEncodeConfig.encodeCodecConfig.h264Config.intraRefreshCnt = pEncCfg->intraRefreshDuration;
        }
    }

	m_stEncodeConfig.encodeCodecConfig.h264Config.entropyCodingMode = (NV_ENC_H264_ENTROPY_CODING_MODE)pEncCfg->entropyCodingMode;
    if (pEncCfg->invalidateRefFramesEnableFlag)
    {
        if (pEncCfg->codec == NV_ENC_HEVC)
        {
            m_stEncodeConfig.encodeCodecConfig.hevcConfig.maxNumRefFramesInDPB = 16;
        }
        else
        {
            m_stEncodeConfig.encodeCodecConfig.h264Config.maxNumRefFrames = 16;
        }
    }

    if (pEncCfg->qpDeltaMapFile)
    {
        m_stEncodeConfig.rcParams.enableExtQPDeltaMap = 1;
    }
    if (pEncCfg->codec == NV_ENC_H264)
    {
        m_stEncodeConfig.encodeCodecConfig.h264Config.idrPeriod = pEncCfg->gopLength;
    }
    else if (pEncCfg->codec == NV_ENC_HEVC)
    {
        m_stEncodeConfig.encodeCodecConfig.hevcConfig.idrPeriod = pEncCfg->gopLength;
    }

	// set lookahead, no have B frames
	m_stEncodeConfig.rcParams.enableLookahead = 1;
	//m_stEncodeConfig.rcParams.disableBadapt = 1;
	m_stEncodeConfig.rcParams.lookaheadDepth = 1;

	m_stEncodeConfig.rcParams.enableTemporalAQ = 1;

    nvStatus = m_pEncodeAPI->nvEncInitializeEncoder(m_hEncoder, &m_stCreateEncodeParams);
    if (nvStatus != NV_ENC_SUCCESS)
        return nvStatus;

    m_bEncoderInitialized = true;

	// get sps and pps
	NV_ENC_SEQUENCE_PARAM_PAYLOAD sequenceParams;
	memset(&sequenceParams, 0, sizeof(NV_ENC_SEQUENCE_PARAM_PAYLOAD));
	sequenceParams.version = NV_ENC_SEQUENCE_PARAM_PAYLOAD_VER;
	if (!spsppsData)
		spsppsData = new uint8_t[100];

	sequenceParams.inBufferSize = 100;
	sequenceParams.spsppsBuffer = spsppsData;
	spsppsoutSize = 0;
	sequenceParams.outSPSPPSPayloadSize = &spsppsoutSize;
	NvEncGetSequenceParams(&sequenceParams);

    return nvStatus;
}

GUID CNvHWEncoder::GetPresetGUID(char* encoderPreset, int codec)
{
    NVENCSTATUS nvStatus = NV_ENC_SUCCESS;
    GUID presetGUID = NV_ENC_PRESET_DEFAULT_GUID;

    if (encoderPreset && (stricmp(encoderPreset, "hq") == 0))
    {
        presetGUID = NV_ENC_PRESET_HQ_GUID;
    }
    else if (encoderPreset && (stricmp(encoderPreset, "lowLatencyHP") == 0))
    {
        presetGUID = NV_ENC_PRESET_LOW_LATENCY_HP_GUID;
    }
    else if (encoderPreset && (stricmp(encoderPreset, "hp") == 0))
    {
        presetGUID = NV_ENC_PRESET_HP_GUID;
    }
    else if (encoderPreset && (stricmp(encoderPreset, "lowLatencyHQ") == 0))
    {
        presetGUID = NV_ENC_PRESET_LOW_LATENCY_HQ_GUID;
    }
    else if (encoderPreset && (stricmp(encoderPreset, "lossless") == 0))
    {
        presetGUID = NV_ENC_PRESET_LOSSLESS_HP_GUID;
    }
    else
    {
        presetGUID = NV_ENC_PRESET_DEFAULT_GUID;
    }

    GUID inputCodecGUID = codec == NV_ENC_H264 ? NV_ENC_CODEC_H264_GUID : NV_ENC_CODEC_HEVC_GUID;
    nvStatus = ValidatePresetGUID(presetGUID, inputCodecGUID);
    if (nvStatus != NV_ENC_SUCCESS)
    {
        presetGUID = NV_ENC_PRESET_DEFAULT_GUID;
        PRINTERR("Unsupported preset guid %s\n", encoderPreset);
    }

    return presetGUID;
}

int getNalUnit(unsigned char *input, int inputLength, unsigned char** output, int* outputLength, int* startCodeLength, int *nalTyle)
{
#define NEXT32BITS(x) (x[0]<<3)|(x[1]<<2)|(x[2]<<1)|(x[3])
#define NEXT24BITS(x) (x[0]<<2)|(x[1]<<1)|(x[2])
	/*find 0x000001 */
	unsigned char *head0 = input, *end = input + inputLength;
	unsigned char *head1 = NULL;

	if (!input || inputLength <= 0)
	{
		if (output) { *output = NULL; }
		if (outputLength) { *outputLength = 0; }
		if (startCodeLength) { *startCodeLength = 0; }
		return -1;
	}

	while ((NEXT24BITS(head0) != 0x000001) && (NEXT32BITS(head0) != 0x00000001))
	{
		head0++;
		if (head0 == end)
			break;
	}

	if (end == head0)
	{
		if (output) { *output = NULL; }
		if (outputLength) { *outputLength = 0; }
		if (startCodeLength) { *startCodeLength = 0; }
		return -1;//no sc
	}

	// sc find.
	head1 = head0;
	if (NEXT24BITS(head0) == 0x000001)
	{
		head1 += 3;
		if (startCodeLength) { *startCodeLength = 3; }
	}
	else if (NEXT32BITS(head0) == 0x00000001)
	{
		head1 += 4;
		if (startCodeLength) { *startCodeLength = 4; }
	}

	*nalTyle = (head1[0] & 0x1f);//((head[0] >> 3) & 0x1f);
	//printf("head[0] = 0x%x\n", head[0]);

	// scan to buffer end or find next nal with start code.
	while ((end > head1) && (NEXT24BITS(head1) != 0x000001) && (NEXT32BITS(head1) != 0x00000001))
	{
		head1++;
	}

	if (output) { *output = head0; }
	if (outputLength) { *outputLength = head1 - head0; }

	return 0;

}

//h264 nalu type
#define NALU_TYPE_SLICE 						1	//P帧或者B帧
#define NALU_TYPE_DPA							2
#define NALU_TYPE_DPB							3
#define NALU_TYPE_DPC							4
#define NALU_TYPE_IDR							5	//I帧，关键帧
#define NALU_TYPE_SEI							6 	//增强帧
#define NALU_TYPE_SPS							7	//SPS
#define NALU_TYPE_PPS							8	//PPS
#define NALU_TYPE_AUD							9	//分隔符，解析时调过该类型和后面1个字节
#define NALU_TYPE_EOSEQ 						10
#define NALU_TYPE_EOSTREAM						11
#define NALU_TYPE_FILL							12

NVENCSTATUS CNvHWEncoder::ProcessOutput(const EncodeBuffer *pEncodeBuffer, unsigned char** outData, unsigned int& outSize, int64_t& pts, int64_t& dts)
{
    NVENCSTATUS nvStatus = NV_ENC_SUCCESS;

    if (pEncodeBuffer->stOutputBfr.hBitstreamBuffer == NULL && pEncodeBuffer->stOutputBfr.bEOSFlag == FALSE)
    {
        return NV_ENC_ERR_INVALID_PARAM;
    }

    if (pEncodeBuffer->stOutputBfr.bWaitOnEvent == TRUE)
    {
        if (!pEncodeBuffer->stOutputBfr.hOutputEvent)
        {
            return NV_ENC_ERR_INVALID_PARAM;
        }
#if defined(NV_WINDOWS)
        WaitForSingleObject(pEncodeBuffer->stOutputBfr.hOutputEvent, INFINITE);
#endif
    }

    if (pEncodeBuffer->stOutputBfr.bEOSFlag)
        return NV_ENC_SUCCESS;

    nvStatus = NV_ENC_SUCCESS;
    NV_ENC_LOCK_BITSTREAM lockBitstreamData;
    memset(&lockBitstreamData, 0, sizeof(lockBitstreamData));
    SET_VER(lockBitstreamData, NV_ENC_LOCK_BITSTREAM);
    lockBitstreamData.outputBitstream = pEncodeBuffer->stOutputBfr.hBitstreamBuffer;
    lockBitstreamData.doNotWait = false;

	outSize = 0;
    nvStatus = m_pEncodeAPI->nvEncLockBitstream(m_hEncoder, &lockBitstreamData);
    if (nvStatus == NV_ENC_SUCCESS)
    {
#ifdef _WRITE_FILE_
        fwrite(lockBitstreamData.bitstreamBufferPtr, 1, lockBitstreamData.bitstreamSizeInBytes, m_fOutput);
#endif
		if (10 * 1024 * 1024 < lockBitstreamData.bitstreamSizeInBytes) {
			if (*outData)
				delete[](*outData);

			*outData = new unsigned char[lockBitstreamData.bitstreamSizeInBytes];
		}

		//uint8_t *start = (uint8_t *)lockBitstreamData.bitstreamBufferPtr;
		//uint8_t *end = start + lockBitstreamData.bitstreamSizeInBytes;
		//const static uint8_t start_seq[] = { 0, 0, 1 };
		//start = std::search(start, end, start_seq, start_seq + 3);

		//x264_nal_t nalOut;
		//while (start != end)
		//{
		//	decltype(start) next = std::search(start + 1, end, start_seq, start_seq + 3);

		//	x264_nal_t nal;

		//	nal.i_ref_idc = (start[3] >> 5) & 3;
		//	nal.i_type = start[3] & 0x1f;

		//	nal.p_payload = start;
		//	nal.i_payload = int(next - start);
		//	//nalOut << nal;
		//	start = next;
		//}

		//unsigned char *nalStart;
		//int nalSize, nalType, scLength;

		//// filter extra sps pps sei payload.
		//unsigned char* payload = (unsigned char*)lockBitstreamData.bitstreamBufferPtr;
		//int payloadSize = lockBitstreamData.bitstreamSizeInBytes;
		//while (getNalUnit(payload, payloadSize, &nalStart, &nalSize, &scLength, &nalType) == 0)
		//{
		//	payload += nalSize;
		//	payloadSize -= nalSize;
		//	if (nalType == NALU_TYPE_SPS || nalType == NALU_TYPE_PPS || nalType == NALU_TYPE_SEI || nalType == NALU_TYPE_AUD)
		//		continue;
		//	else
		//		break;
		//}

		//nalStart += scLength; // skip start code.
		//nalSize -= scLength;

		//char pSize[4];
		//pSize[0] = nalSize >> 24;
		//pSize[1] = (nalSize << 8) >> 24;
		//pSize[2] = (nalSize << 16) >> 24;
		//pSize[3] = (nalSize << 24) >> 24;

		pts = lockBitstreamData.outputTimeStamp;
		dts = dtsList.front();
		dtsList.pop_front();
		//memcpy((*outData + outSize), pSize, 4);
		//outSize += 4;
		//memcpy((*outData + outSize), nalStart, nalSize);
		//outSize += nalSize;
		memcpy(*outData, lockBitstreamData.bitstreamBufferPtr, lockBitstreamData.bitstreamSizeInBytes);
		outSize = lockBitstreamData.bitstreamSizeInBytes;
        nvStatus = m_pEncodeAPI->nvEncUnlockBitstream(m_hEncoder, pEncodeBuffer->stOutputBfr.hBitstreamBuffer);
    }
    else
    {
        PRINTERR("lock bitstream function failed \n");
    }

    return nvStatus;
}

NVENCSTATUS CNvHWEncoder::Initialize(void* device, NV_ENC_DEVICE_TYPE deviceType)
{
    NVENCSTATUS nvStatus = NV_ENC_SUCCESS;
    MYPROC nvEncodeAPICreateInstance; // function pointer to create instance in nvEncodeAPI

#if defined(NV_WINDOWS)
#if defined (_WIN64)
    m_hinstLib = LoadLibrary(TEXT("nvEncodeAPI64.dll"));
#else
    m_hinstLib = LoadLibrary(TEXT("nvEncodeAPI.dll"));
#endif
#else
    m_hinstLib = dlopen("libnvidia-encode.so.1", RTLD_LAZY);
#endif
    if (m_hinstLib == NULL)
        return NV_ENC_ERR_OUT_OF_MEMORY;

#if defined(NV_WINDOWS)
    nvEncodeAPICreateInstance = (MYPROC)GetProcAddress(m_hinstLib, "NvEncodeAPICreateInstance");
#else
    nvEncodeAPICreateInstance = (MYPROC)dlsym(m_hinstLib, "NvEncodeAPICreateInstance");
#endif

    if (nvEncodeAPICreateInstance == NULL)
        return NV_ENC_ERR_OUT_OF_MEMORY;

    m_pEncodeAPI = new NV_ENCODE_API_FUNCTION_LIST;
    if (m_pEncodeAPI == NULL)
        return NV_ENC_ERR_OUT_OF_MEMORY;

    memset(m_pEncodeAPI, 0, sizeof(NV_ENCODE_API_FUNCTION_LIST));
    m_pEncodeAPI->version = NV_ENCODE_API_FUNCTION_LIST_VER;
    nvStatus = nvEncodeAPICreateInstance(m_pEncodeAPI);
    if (nvStatus != NV_ENC_SUCCESS)
        return nvStatus;

    nvStatus = NvEncOpenEncodeSessionEx(device, deviceType);
    if (nvStatus != NV_ENC_SUCCESS)
        return nvStatus;

    return NV_ENC_SUCCESS;
}

NVENCSTATUS CNvHWEncoder::NvEncEncodeFrame(EncodeBuffer *pEncodeBuffer, NvEncPictureCommand *encPicCommand,
	uint32_t width, uint32_t height, int64_t pts, NV_ENC_PIC_STRUCT ePicStruct,
	int8_t *qpDeltaMapArray, uint32_t qpDeltaMapArraySize)
{
	NVENCSTATUS nvStatus = NV_ENC_SUCCESS;
	NV_ENC_PIC_PARAMS encPicParams;

	memset(&encPicParams, 0, sizeof(encPicParams));
	SET_VER(encPicParams, NV_ENC_PIC_PARAMS);

	encPicParams.inputBuffer = pEncodeBuffer->stInputBfr.hInputSurface;
	encPicParams.bufferFmt = pEncodeBuffer->stInputBfr.bufferFmt;
	encPicParams.inputWidth = width;
	encPicParams.inputHeight = height;
	encPicParams.outputBitstream = pEncodeBuffer->stOutputBfr.hBitstreamBuffer;
	encPicParams.completionEvent = pEncodeBuffer->stOutputBfr.hOutputEvent;
	encPicParams.inputTimeStamp = pts;// m_EncodeIdx;
	encPicParams.pictureStruct = ePicStruct;
	encPicParams.qpDeltaMap = qpDeltaMapArray;
	encPicParams.qpDeltaMapSize = qpDeltaMapArraySize;
	encPicParams.encodePicFlags = 0;
	encPicParams.inputDuration = 0;

	if (codecGUID == NV_ENC_CODEC_HEVC_GUID) {
		encPicParams.codecPicParams.hevcPicParams.sliceMode = m_stEncodeConfig.encodeCodecConfig.hevcConfig.sliceMode;
		encPicParams.codecPicParams.hevcPicParams.sliceModeData = m_stEncodeConfig.encodeCodecConfig.hevcConfig.sliceModeData;
	}
	else {
		encPicParams.codecPicParams.h264PicParams.sliceMode = m_stEncodeConfig.encodeCodecConfig.h264Config.sliceMode;
		encPicParams.codecPicParams.h264PicParams.sliceModeData = m_stEncodeConfig.encodeCodecConfig.h264Config.sliceModeData;
	}

	if (encPicCommand)
	{
		if (encPicCommand->bForceIDR)
		{
			encPicParams.encodePicFlags |= NV_ENC_PIC_FLAG_FORCEIDR;
		}

		if (encPicCommand->bForceIntraRefresh)
		{
			if (codecGUID == NV_ENC_CODEC_HEVC_GUID)
			{
				encPicParams.codecPicParams.hevcPicParams.forceIntraRefreshWithFrameCnt = encPicCommand->intraRefreshDuration;
			}
			else
			{
				encPicParams.codecPicParams.h264PicParams.forceIntraRefreshWithFrameCnt = encPicCommand->intraRefreshDuration;
			}
		}
	}

    nvStatus = m_pEncodeAPI->nvEncEncodePicture(m_hEncoder, &encPicParams);
    if (nvStatus != NV_ENC_SUCCESS && nvStatus != NV_ENC_ERR_NEED_MORE_INPUT)
    {
        //assert(0);
        return nvStatus;
    }

	dtsList.push_back(pts);
    m_EncodeIdx++;

    return NV_ENC_SUCCESS;
}

NVENCSTATUS CNvHWEncoder::NvEncFlushEncoderQueue(void *hEOSEvent)
{
    NVENCSTATUS nvStatus = NV_ENC_SUCCESS;
    NV_ENC_PIC_PARAMS encPicParams;
    memset(&encPicParams, 0, sizeof(encPicParams));
    SET_VER(encPicParams, NV_ENC_PIC_PARAMS);
    encPicParams.encodePicFlags = NV_ENC_PIC_FLAG_EOS;
    encPicParams.completionEvent = hEOSEvent;
    nvStatus = m_pEncodeAPI->nvEncEncodePicture(m_hEncoder, &encPicParams);
    if (nvStatus != NV_ENC_SUCCESS)
    {
        //assert(0);
    }
    return nvStatus;
}

NVENCSTATUS CNvHWEncoder::ParseArguments(EncodeConfig *encodeConfig, int argc, char *argv[])
{
    for (int i = 1; i < argc; i++)
    {
        //if (stricmp(argv[i], "-bmpfilePath") == 0)
        //{
        //    if (++i >= argc)
        //    {
        //        PRINTERR("invalid parameter for %s\n", argv[i - 1]);
        //        return NV_ENC_ERR_INVALID_PARAM;
        //    }
        //    encodeConfig->inputFilePath = argv[i];
        //}
        //else if (stricmp(argv[i], "-i") == 0)
        //{
        //    if (++i >= argc)
        //    {
        //        PRINTERR("invalid parameter for %s\n", argv[i - 1]);
        //        return NV_ENC_ERR_INVALID_PARAM;
        //    }
        //    encodeConfig->inputFileName = argv[i];
        //}
        //else if (stricmp(argv[i], "-o") == 0)
        //{
        //    if (++i >= argc)
        //    {
        //        PRINTERR("invalid parameter for %s\n", argv[i - 1]);
        //        return NV_ENC_ERR_INVALID_PARAM;
        //    }
        //    encodeConfig->outputFileName = argv[i];
        //}
        if (stricmp(argv[i], "-size") == 0)
        {
            if (++i >= argc || sscanf(argv[i], "%d", &encodeConfig->width) != 1)
            {
                PRINTERR("invalid parameter for %s\n", argv[i - 1]);
                return NV_ENC_ERR_INVALID_PARAM;
            }

            if (++i >= argc || sscanf(argv[i], "%d", &encodeConfig->height) != 1)
            {
                PRINTERR("invalid parameter for %s\n", argv[i - 2]);
                return NV_ENC_ERR_INVALID_PARAM;
            }
        }
        else if (stricmp(argv[i], "-maxSize") == 0)
        {
            if (++i >= argc || sscanf(argv[i], "%d", &encodeConfig->maxWidth) != 1)
            {
                PRINTERR("invalid parameter for %s\n", argv[i - 1]);
                return NV_ENC_ERR_INVALID_PARAM;
            }

            if (++i >= argc || sscanf(argv[i], "%d", &encodeConfig->maxHeight) != 1)
            {
                PRINTERR("invalid parameter for %s\n", argv[i - 2]);
                return NV_ENC_ERR_INVALID_PARAM;
            }
        }
        else if (stricmp(argv[i], "-bitrate") == 0)
        {
            if (++i >= argc || sscanf(argv[i], "%d", &encodeConfig->bitrate) != 1)
            {
                PRINTERR("invalid parameter for %s\n", argv[i - 1]);
                return NV_ENC_ERR_INVALID_PARAM;
            }
        }
        else if (stricmp(argv[i], "-vbvMaxBitrate") == 0)
        {
            if (++i >= argc || sscanf(argv[i], "%d", &encodeConfig->vbvMaxBitrate) != 1)
            {
                PRINTERR("invalid parameter for %s\n", argv[i - 1]);
                return NV_ENC_ERR_INVALID_PARAM;
            }
        }
        else if (stricmp(argv[i], "-vbvSize") == 0)
        {
            if (++i >= argc || sscanf(argv[i], "%d", &encodeConfig->vbvSize) != 1)
            {
                PRINTERR("invalid parameter for %s\n", argv[i - 1]);
                return NV_ENC_ERR_INVALID_PARAM;
            }
        }
        else if (stricmp(argv[i], "-fps") == 0)
        {
            if (++i >= argc || sscanf(argv[i], "%d", &encodeConfig->fps) != 1)
            {
                PRINTERR("invalid parameter for %s\n", argv[i - 1]);
                return NV_ENC_ERR_INVALID_PARAM;
            }
        }
        //else if (stricmp(argv[i], "-startf") == 0)
        //{
        //    if (++i >= argc || sscanf(argv[i], "%d", &encodeConfig->startFrameIdx) != 1)
        //    {
        //        PRINTERR("invalid parameter for %s\n", argv[i - 1]);
        //        return NV_ENC_ERR_INVALID_PARAM;
        //    }
        //}
        //else if (stricmp(argv[i], "-endf") == 0)
        //{
        //    if (++i >= argc || sscanf(argv[i], "%d", &encodeConfig->endFrameIdx) != 1)
        //    {
        //        PRINTERR("invalid parameter for %s\n", argv[i - 1]);
        //        return NV_ENC_ERR_INVALID_PARAM;
        //    }
        //}
        else if (stricmp(argv[i], "-rcmode") == 0)
        {
            if (++i >= argc || sscanf(argv[i], "%d", &encodeConfig->rcMode) != 1)
            {
                PRINTERR("invalid parameter for %s\n", argv[i - 1]);
                return NV_ENC_ERR_INVALID_PARAM;
            }
        }
        else if (stricmp(argv[i], "-goplength") == 0)
        {
            if (++i >= argc || sscanf(argv[i], "%d", &encodeConfig->gopLength) != 1)
            {
                PRINTERR("invalid parameter for %s\n", argv[i - 1]);
                return NV_ENC_ERR_INVALID_PARAM;
            }
        }
        else if (stricmp(argv[i], "-numB") == 0)
        {
            if (++i >= argc || sscanf(argv[i], "%d", &encodeConfig->numB) != 1)
            {
                PRINTERR("invalid parameter for %s\n", argv[i - 1]);
                return NV_ENC_ERR_INVALID_PARAM;
            }
        }
        else if (stricmp(argv[i], "-qp") == 0)
        {
            if (++i >= argc || sscanf(argv[i], "%d", &encodeConfig->qp) != 1)
            {
                PRINTERR("invalid parameter for %s\n", argv[i - 1]);
                return NV_ENC_ERR_INVALID_PARAM;
            }
        }
        else if (stricmp(argv[i], "-preset") == 0)
        {
            if (++i >= argc)
            {
                PRINTERR("invalid parameter for %s\n", argv[i - 1]);
                return NV_ENC_ERR_INVALID_PARAM;
            }
            encodeConfig->encoderPreset = argv[i];
        }
        else if (stricmp(argv[i], "-devicetype") == 0)
        {
            if (++i >= argc || sscanf(argv[i], "%d", &encodeConfig->deviceType) != 1)
            {
                PRINTERR("invalid parameter for %s\n", argv[i - 1]);
                return NV_ENC_ERR_INVALID_PARAM;
            }
        }
        else if (stricmp(argv[i], "-codec") == 0)
        {
            if (++i >= argc || sscanf(argv[i], "%d", &encodeConfig->codec) != 1)
            {
                PRINTERR("invalid parameter for %s\n", argv[i - 1]);
                return NV_ENC_ERR_INVALID_PARAM;
            }
        }
        else if (stricmp(argv[i], "-encCmdFile") == 0)
        {
            if (++i >= argc)
            {
                PRINTERR("invalid parameter for %s\n", argv[i - 1]);
                return NV_ENC_ERR_INVALID_PARAM;
            }
            encodeConfig->encCmdFileName = argv[i];
        }
        else if (stricmp(argv[i], "-intraRefresh") == 0)
        {
            if (++i >= argc || sscanf(argv[i], "%d", &encodeConfig->intraRefreshEnableFlag) != 1)
            {
                PRINTERR("invalid parameter for %s\n", argv[i - 1]);
                return NV_ENC_ERR_INVALID_PARAM;
            }
        }
        else if (stricmp(argv[i], "-intraRefreshPeriod") == 0)
        {
            if (++i >= argc || sscanf(argv[i], "%d", &encodeConfig->intraRefreshPeriod) != 1)
            {
                PRINTERR("invalid parameter for %s\n", argv[i - 1]);
                return NV_ENC_ERR_INVALID_PARAM;
            }
        }
        else if (stricmp(argv[i], "-intraRefreshDuration") == 0)
        {
            if (++i >= argc || sscanf(argv[i], "%d", &encodeConfig->intraRefreshDuration) != 1)
            {
                PRINTERR("invalid parameter for %s\n", argv[i - 1]);
                return NV_ENC_ERR_INVALID_PARAM;
            }
        }
        else if (stricmp(argv[i], "-picStruct") == 0)
        {
            if (++i >= argc || sscanf(argv[i], "%d", &encodeConfig->pictureStruct) != 1)
            {
                PRINTERR("invalid parameter for %s\n", argv[i - 1]);
                return NV_ENC_ERR_INVALID_PARAM;
            }
        }
        else if (stricmp(argv[i], "-deviceID") == 0)
        {
            if (++i >= argc || sscanf(argv[i], "%d", &encodeConfig->deviceID) != 1)
            {
                PRINTERR("invalid parameter for %s\n", argv[i - 1]);
                return NV_ENC_ERR_INVALID_PARAM;
            }
        }
        else if (stricmp(argv[i], "-yuv444") == 0)
        {
            if (++i >= argc || sscanf(argv[i], "%d", &encodeConfig->isYuv444) != 1)
            {
                fprintf(stderr, "invalid parameter for %s\n", argv[i - 1]);
                return NV_ENC_ERR_INVALID_PARAM;
            }
        }
        else if (stricmp(argv[i], "-qpDeltaMapFile") == 0)
        {
            if (++i >= argc)
            {
                PRINTERR("invalid parameter for %s\n", argv[i - 1]);
                return NV_ENC_ERR_INVALID_PARAM;
            }
            encodeConfig->qpDeltaMapFile = argv[i];
        }
        else if (stricmp(argv[i], "-help") == 0)
        {
            return NV_ENC_ERR_INVALID_PARAM;
        }
        else
        {
            PRINTERR("invalid parameter  %s\n", argv[i++]);
            return NV_ENC_ERR_INVALID_PARAM;
        }
    }

    return NV_ENC_SUCCESS;
}
