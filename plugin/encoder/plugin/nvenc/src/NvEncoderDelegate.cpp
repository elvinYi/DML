#include <stdlib.h>

#include "NvEncoderDelegate.h"
#include "NvEncoder.h"
#include "Errors.h"
#include "BaseBuffer.h"

NvEncoderDelegate::NvEncoderDelegate() {
	nvEncoder = nullptr;
}

NvEncoderDelegate::~NvEncoderDelegate() {
}

SInt32 NvEncoderDelegate::initialize(SInt32 codecType) {
	nvEncoder = new CNvEncoder();
	if (!nvEncoder) {
		return NO_MEMORY;
	}

	if (nvEncoder->Init(codecType)) {
		return INVALID_OPERATION;
	}

	return OK;
}

SInt32 NvEncoderDelegate::uninitialize() {
	if (nvEncoder) {
		nvEncoder->Uninit();
		delete nvEncoder;
	}
	nvEncoder = nullptr;
	return OK;
}

SInt32 NvEncoderDelegate::internalGetParameter(SInt32 index, Void* params) {
	if (!nvEncoder)
		return BAD_VALUE;

	switch (index)
	{
	case CODEC_FLAGS_HEADDATA:
	{
        BaseBuffer* headerBuffer = (BaseBuffer*)params;
		UInt8* data = headerBuffer->getData();
        UInt8* headerData = NULL;
		UInt32 headerSize = 0;
		nvEncoder->GetParam(CNvEncoderGetKey::SPS_PPS_SIZE, &headerSize);
		nvEncoder->GetParam(CNvEncoderGetKey::SPS_PPS_DATA, &headerData);
		memcpy(data, headerData, headerSize);
		headerBuffer->setLength(headerSize);
        headerBuffer->setFlags(CODEC_FLAGS_HEADDATA);
	}
	default:
		break;
	}
	return OK;
}

SInt32 NvEncoderDelegate::internalSetParameter(SInt32 index, Void* params) {
	if (!nvEncoder)
		return BAD_VALUE;

	switch (index)
	{
	case PID_VIDEO_WIDTH:
	{
		nvEncoder->SetParam(CNvEncoderSetkey::WIDTH, params);
		break;
	}
	case PID_VIDEO_HEIGHT:
	{
		nvEncoder->SetParam(CNvEncoderSetkey::HEIGHT, params);
		break;
	}
	case PID_VIDEO_BITRATE:
	{
		nvEncoder->SetParam(CNvEncoderSetkey::BITRATE, params);
		break;
	}
	case PID_VIDEO_ENC_I_FRAME_INTERTVAL:
	{
		nvEncoder->SetParam(CNvEncoderSetkey::GOPLENGTH, params);
		break;
	}
	case PID_VIDEO_ENC_QP:
	{
		nvEncoder->SetParam(CNvEncoderSetkey::QP, params);
		break;
	}
	case PID_VIDEO_ENC_QUALITY:
	{
		nvEncoder->SetParam(CNvEncoderSetkey::QUALITY, params);
		break;
	}
	case PID_VIDEO_ENC_BITRATE_CONTROL_MODE:
	{
		int rcMode = *(int*)params;
		if (rcMode == PID_VIDEO_BITRATE_CONTROL_CBR) {
			rcMode = NV_ENC_PARAMS_RC_CBR;
		}
		else if (rcMode == PID_VIDEO_BITRATE_CONTROL_VBR) {
			rcMode = NV_ENC_PARAMS_RC_VBR;
		}
		nvEncoder->SetParam(CNvEncoderSetkey::RCMODE, &rcMode);
		break;
	}
	case PID_VIDEO_ENC_B_FRAME_NUM:
	{
		nvEncoder->SetParam(CNvEncoderSetkey::NUMB, params);
		break;
	}
	case PID_VIDEO_ENC_PRESET:
	{
		enum NvencPreset {
			HQ = 0,         //"hq"
			LOW_LATENCY_HP, //"lowLatencyHP"
			HP,             //"hp"
			LOW_LATENCY_HQ, //"lowLatencyHQ"
			LOSSLESS,       //"lossless"
			DEFAULT,        //"default"
		};

		char* str_preset[] = {
			"hq",
			"lowLatencyHP",
			"hp",
			"lowLatencyHQ",
			"lossless",
			"default",
		};

		NvencPreset p = *(NvencPreset*)params;
		nvEncoder->SetParam(CNvEncoderSetkey::PRESET, str_preset[p]);
		break;
	}
	case PID_VIDEO_ENC_PROFILE:
	{
		nvEncoder->SetParam(CNvEncoderSetkey::PROFILE, params);
		break;
	}
	case PID_VIDEO_ENC_ENTROPY_CODING_MODE:
	{
		nvEncoder->SetParam(CNvEncoderSetkey::ENTROPY_CODING_MODE, params);
		break;
	}
	case CODEC_FLAGS_FLUSH:
	{
		nvEncoder->SetParam(CNvEncoderSetkey::FLUSH, params);
		break;
	}
	default:
		break;
	}
	return OK;
}

SInt32 NvEncoderDelegate::start() {
	if (nvEncoder)
		if (!nvEncoder->Start())
			return OK;

	return INVALID_OPERATION;
}

SInt32 NvEncoderDelegate::stop() {
	if (nvEncoder)
		nvEncoder->Stop();

	return OK;
}

SInt32 NvEncoderDelegate::processInput(BaseBuffer* inputBuffer) {
	VideoBuffer *p = (VideoBuffer*)inputBuffer->getData();
	if (nvEncoder->Encode(p->mData[0], p->mData[1], p->mData[2], p->mTime))
		return BAD_VALUE;
	return OK;
}

SInt32 NvEncoderDelegate::processOutput(BaseBuffer* outputBuffer) {
	uint8_t* out = outputBuffer->getData();
	int yuvSize = outputBuffer->getLength();
	unsigned int outSize;
	long long dts, pts;
	bool keyFrame;
	outputBuffer->setLength(0);
	if (nvEncoder->GetOutput(out, yuvSize, outSize, pts, dts, keyFrame))
		return NOT_ENOUGH_DATA;

	outputBuffer->setLength(outSize);
	outputBuffer->setDts(dts);
	outputBuffer->setTimestamp(pts);
	outputBuffer->setFlags(keyFrame);
	return OK;
}
