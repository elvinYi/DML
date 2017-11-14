#include "BaseBuffer.h"
#include "X264VideoEncoder.h"

#ifdef X264ENCODER_EXPORTS
#define LIBIX264_API   __declspec( dllexport ) 
#define LIBIX264_CLS   extern "C" __declspec( dllexport ) 
#else
#define LIBIX264_API   __declspec( dllimport ) 
#define LIBIX264_CLS   __declspec( dllexport ) 
#endif

typedef void* IX264Handle;

using namespace MediaPlugin;

LIBIX264_CLS IX264Handle IX264_Create() {
	auto p = new X264VideoEncoder();
	if (!p)
		return nullptr;

	return (IX264Handle)(p);
}

LIBIX264_CLS int IX264_Init(IX264Handle h, int codecType) {
	if (!h)
		return INVALID_OPERATION;

	auto p = static_cast<X264VideoEncoder*>(h);
	return p->initialize(codecType);
}

LIBIX264_CLS int IX264_Start(IX264Handle h) {
	if (!h)
		return INVALID_OPERATION;

	auto p = static_cast<X264VideoEncoder*>(h);
	return p->start();
}

LIBIX264_CLS int IX264_SetConfig(IX264Handle h, int key, void* config) {
	if (!h)
		return INVALID_OPERATION;

	auto p = static_cast<X264VideoEncoder*>(h);
	return p->internalSetParameter(key, config);
}

LIBIX264_CLS int IX264_GetParam(IX264Handle h, int key, void* param) {
	if (!h)
		return INVALID_OPERATION;

	auto p = static_cast<X264VideoEncoder*>(h);
	return p->internalGetParameter(key, param);
}

LIBIX264_CLS void IX264_Stop(IX264Handle h) {
	if (!h)
		return;

	auto p = static_cast<X264VideoEncoder*>(h);
	p->stop();
}

LIBIX264_CLS void IX264_Uninit(IX264Handle h) {
	if (!h)
		return;

	auto p = static_cast<X264VideoEncoder*>(h);
	p->uninitialize();
}

LIBIX264_CLS int IX264_Input(IX264Handle h, BaseBuffer* inputBuffer) {
	if (!h)
		return INVALID_OPERATION;

	auto p = static_cast<X264VideoEncoder*>(h);
	return p->processInput(inputBuffer);
}

LIBIX264_CLS int IX264_Output(IX264Handle h, BaseBuffer* outputBuffer) {
	if (!h)
		return INVALID_OPERATION;

	auto p = static_cast<X264VideoEncoder*>(h);
	return p->processOutput(outputBuffer);
}

LIBIX264_CLS void IX264_Destory(IX264Handle h) {
	if (h == nullptr)
		return;

	auto p = static_cast<X264VideoEncoder*>(h);
	delete p;
}
