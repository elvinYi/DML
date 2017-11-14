#include "BaseBuffer.h"
#include "NvEncoderDelegate.h"

#ifdef NVENC_EXPORTS
#define LIBNVENC_API   __declspec( dllexport ) 
#define LIBNVENC_CLS   extern "C" __declspec( dllexport ) 
#else
#define LIBNVENC_API   __declspec( dllimport ) 
#define LIBNVENC_CLS   __declspec( dllexport ) 
#endif

typedef void* INvEncHandle;

LIBNVENC_CLS INvEncHandle INvEnc_Create() {
	auto p = new NvEncoderDelegate();
	if (!p)
		return nullptr;

	return (INvEncHandle)(p);
}

LIBNVENC_CLS int INvEnc_Init(INvEncHandle h, int codecType) {
	if (!h)
		return INVALID_OPERATION;

	auto p = static_cast<NvEncoderDelegate*>(h);
	return p->initialize(codecType);
}

LIBNVENC_CLS int INvEnc_Start(INvEncHandle h) {
	if (!h)
		return INVALID_OPERATION;

	auto p = static_cast<NvEncoderDelegate*>(h);
	return p->start();
}

LIBNVENC_CLS int INvEnc_SetConfig(INvEncHandle h, int key, void* config) {
	if (!h)
		return INVALID_OPERATION;

	auto p = static_cast<NvEncoderDelegate*>(h);
	return p->internalSetParameter(key, config);
}

LIBNVENC_CLS int INvEnc_GetParam(INvEncHandle h, int key, void* param) {
	if (!h)
		return INVALID_OPERATION;

	auto p = static_cast<NvEncoderDelegate*>(h);
	return p->internalGetParameter(key, param);
}

LIBNVENC_CLS void INvEnc_Stop(INvEncHandle h) {
	if (!h)
		return;

	auto p = static_cast<NvEncoderDelegate*>(h);
	p->stop();
}

LIBNVENC_CLS void INvEnc_Uninit(INvEncHandle h) {
	if (!h)
		return;

	auto p = static_cast<NvEncoderDelegate*>(h);
	p->uninitialize();
}

LIBNVENC_CLS int INvEnc_Input(INvEncHandle h, BaseBuffer* inputBuffer) {
	if (!h)
		return INVALID_OPERATION;

	auto p = static_cast<NvEncoderDelegate*>(h);
	return p->processInput(inputBuffer);
}

LIBNVENC_CLS int INvEnc_Output(INvEncHandle h, BaseBuffer* outputBuffer) {
	if (!h)
		return INVALID_OPERATION;

	auto p = static_cast<NvEncoderDelegate*>(h);
	return p->processOutput(outputBuffer);
}

LIBNVENC_CLS void INvEnc_Destory(INvEncHandle h) {
	if (h == nullptr)
		return;

	auto p = static_cast<NvEncoderDelegate*>(h);
	delete p;
}
