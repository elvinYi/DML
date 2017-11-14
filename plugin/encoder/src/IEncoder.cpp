#include "PubConst.h"
#include "Log.h"
#include "LoadEncoderDll.h"
#include "MediaFormat.h"

typedef void* IEncoderHandle;
typedef void* IEncoderNotifyHandle;
typedef void(*SendIEncoderEventProc)(IEncoderNotifyHandle h, IEncoder::IEncoderEvent e, void* param);

IED_ENTRY

static const char* tag = "IEncoderImpl";

#define CHECK_NULL(name) \
	if (name == nullptr) { \
		Log::e(tag, "the pointer is nullptr"); \
		return false; \
	} \


class IEncoderImpl {
public:
	IEncoderImpl(IEncoderNotifyHandle h, SendIEncoderEventProc s)
		: h(h),
		  s(s) {
		Log::Init();
		encoder = nullptr;
	}

	~IEncoderImpl() {
		Log::Release();
	}

	void SendMsgToOuter(IEncoderEvent e, void* param) {
		s(h, (IEncoder::IEncoderEvent)e, param);
	}


	bool Init(IEncoderType type, IEncoderIndex index, int codecType) {
		encoder = new IBaseCodec();
		CHECK_NULL(encoder);

		if (!eDll.LoadDll(type, index, codecType, *encoder))
			return false;

		encoder->h = encoder->create();
		if (!encoder->h)
			return false;

		return (encoder->init(encoder->h, codecType) == OK);
	}

	bool Start() {
		CHECK_NULL(encoder);
		return (encoder->start(encoder->h) == OK);
	}

	bool SetConfig(IEncoderSetKey key, void* config) {
		CHECK_NULL(encoder);
		int transformKey = 0;
		// the transform key, please see MediaFormat.h
		switch (key)
		{
		case IEncoder::IEncoderSetKey::AUDIO_SAMPLERATE:
			transformKey = PID_AUDIO_SAMPLEREATE;
			break;
		case IEncoder::IEncoderSetKey::AUDIO_CHANNEL:
			transformKey = PID_AUDIO_CHANNELS;
			break;
		case IEncoder::IEncoderSetKey::AUDIO_BITPERSAMPLE:
			transformKey = PID_AUDIO_SAMPLEBIT;
			break;
		case IEncoder::IEncoderSetKey::AUDIO_BITRATE:
			transformKey = PID_AUDIO_BITRATE;
			break;
		case IEncoder::IEncoderSetKey::AUDIO_PROFILE:
			transformKey = PID_AUDIO_AAC_PROFILE;
			break;
		case IEncoder::IEncoderSetKey::VIDEO_WIDTH:
			transformKey = PID_VIDEO_WIDTH;
			break;
		case IEncoder::IEncoderSetKey::VIDEO_HEIGHT:
			transformKey = PID_VIDEO_HEIGHT;
			break;
		case IEncoder::IEncoderSetKey::VIDEO_FPS:
			transformKey = PID_VIDEO_ENC_FPS;
			break;
		case IEncoder::IEncoderSetKey::VIDEO_BITRATE:
			transformKey = PID_VIDEO_BITRATE;
			break;
		case IEncoder::IEncoderSetKey::VIDEO_I_FRAME_INTERTVAL:
			transformKey = PID_VIDEO_ENC_I_FRAME_INTERTVAL;
			break;
		case IEncoder::IEncoderSetKey::VIDEO_REFFRAME:
			transformKey = PID_VIDEO_ENC_REFFRAME;
			break;
		case IEncoder::IEncoderSetKey::VIDEO_INPUT_FORMAT:
			transformKey = PID_VIDEO_ENC_INPUT_FORMAT;
			break;
		case IEncoder::IEncoderSetKey::FLUSH:
			transformKey = CODEC_FLAGS_FLUSH;
			break;
		case IEncoder::IEncoderSetKey::VIDEO_ENC_PRESET:
			transformKey = PID_VIDEO_ENC_PRESET;
			break;
		case IEncoder::IEncoderSetKey::VIDEO_ENC_PROFILE:
			transformKey = PID_VIDEO_ENC_PROFILE;
			break;
		case IEncoder::IEncoderSetKey::VIDEO_ENC_CRF:
			transformKey = PID_VIDEO_ENC_CRF;
			break;
		case IEncoder::IEncoderSetKey::VIDEO_ENC_BITRATE_CONTROL_MODE:
			transformKey = PID_VIDEO_ENC_BITRATE_CONTROL_MODE;
			break;
		case IEncoder::IEncoderSetKey::VIDEO_ENC_QUALITY:
			transformKey = PID_VIDEO_ENC_QUALITY;
			break;
		case IEncoder::IEncoderSetKey::VIDEO_ENC_B_FRAME_NUM:
			transformKey = PID_VIDEO_ENC_B_FRAME_NUM;
			break;
		case IEncoder::IEncoderSetKey::VIDEO_ENC_ENTROPY_CODING_MODE:
			transformKey = PID_VIDEO_ENC_ENTROPY_CODING_MODE;
			break;
		case IEncoder::IEncoderSetKey::VIDEO_CPU_COUNT:
			transformKey = PID_VIDEO_CPU_COUNT;
			break;
		case IEncoder::IEncoderSetKey::VIDEO_ENC_TUNE:
			transformKey = PID_VIDEO_ENC_TUNE;
			break;
		default:
			return false;
		}
		return (encoder->setConfig(encoder->h, transformKey, config) == OK);
	}

	bool GetParam(IEncoderGetKey key, void* param) {
		CHECK_NULL(encoder);
		int transformKey = 0;
		switch (key)
		{
		case IEncoder::IEncoderGetKey::HEAD_DATA:
			transformKey = CODEC_FLAGS_HEADDATA;
			break;
		default:
			return false;
		}
		return (encoder->getParam(encoder->h, transformKey, param) == OK);
	}

	int Input(BaseBuffer* inputBuffer) {
		if (!encoder)
			return INVALID_OPERATION;
		return encoder->input(encoder->h, inputBuffer);
	}

	int Output(BaseBuffer* outputBuffer) {
		if (!encoder)
			return INVALID_OPERATION;
		return encoder->output(encoder->h, outputBuffer);
	}

	void Stop(){
		if (!encoder)
			return;
		encoder->stop(encoder->h);
	}

	void Uninit() {
		if (!encoder)
			return;
		encoder->uninit(encoder->h);
		encoder->destory(encoder->h);
		delete encoder;
		encoder = nullptr;
	}

private:
	IBaseCodec* encoder;
	IEncoderNotifyHandle h;
	SendIEncoderEventProc s;
	LoadEncoderDll eDll;
};

IED_EXIT

IED_USING

#ifdef LIBIENCODER_EXPORTS
#define LIBIENCODER_API   __declspec( dllexport ) 
#define LIBIENCODER_CLS   __declspec( dllexport ) 
#else
//#define LIBIENCODER_API   __declspec( dllimport ) 
//#define LIBIENCODER_CLS   __declspec( dllexport ) 
#define LIBIENCODER_CLS extern "C" __declspec(dllexport)
#endif

LIBIENCODER_CLS IEncoderHandle IEncoder_Create(IEncoderNotifyHandle h, SendIEncoderEventProc s) {
	if (s == nullptr)
		return nullptr;

	auto p = new IEncoderImpl(h, s);
	if (!p)
		return nullptr;

	return (IEncoderHandle)(p);
}

LIBIENCODER_CLS bool IEncoder_Init(IEncoderHandle h, IEncoderType type, IEncoderIndex index, int codec) {
	if (!h)
		return false;

	auto p = static_cast<IEncoderImpl*>(h);
	return p->Init(type, index, codec);
}

LIBIENCODER_CLS bool IEncoder_Start(IEncoderHandle h) {
	if (!h)
		return false;

	auto p = static_cast<IEncoderImpl*>(h);
	return p->Start();
}

LIBIENCODER_CLS bool IEncoder_SetConfig(IEncoderHandle h, IEncoderSetKey key, void* config) {
	if (!h)
		return false;

	auto p = static_cast<IEncoderImpl*>(h);
	return p->SetConfig(key, config);
}

LIBIENCODER_CLS bool IEncoder_GetParam(IEncoderHandle h, IEncoderGetKey key, void* param) {
	if (!h)
		return false;

	auto p = static_cast<IEncoderImpl*>(h);
	return p->GetParam(key, param);
}

LIBIENCODER_CLS void IEncoder_Stop(IEncoderHandle h) {
	if (!h)
		return;

	auto p = static_cast<IEncoderImpl*>(h);
	p->Stop();
}

LIBIENCODER_CLS void IEncoder_Uninit(IEncoderHandle h) {
	if (!h)
		return;

	auto p = static_cast<IEncoderImpl*>(h);
	p->Uninit();
}

LIBIENCODER_CLS int IEncoder_Input(IEncoderHandle h, BaseBuffer* input) {
	if (!h)
		return INVALID_OPERATION;

	auto p = static_cast<IEncoderImpl*>(h);
	return p->Input(input);

}

LIBIENCODER_CLS int IEncoder_Output(IEncoderHandle h, BaseBuffer* output) {
	if (!h)
		return INVALID_OPERATION;

	auto p = static_cast<IEncoderImpl*>(h);
	return p->Output(output);

}

LIBIENCODER_CLS void IEncoder_Destory(IEncoderHandle h) {
	if (h == nullptr)
		return;

	auto p = static_cast<IEncoderImpl*>(h);
	delete p;
}
