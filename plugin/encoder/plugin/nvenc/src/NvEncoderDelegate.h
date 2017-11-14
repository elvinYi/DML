#ifndef _NV_ENCODER_DELEGATE_H_
#define _NV_ENCODER_DELEGATE_H_

#include "BaseCodec.h"

using namespace MediaPlugin;

class CNvEncoder;

class NvEncoderDelegate : public BaseCodec {
public:
	NvEncoderDelegate();
	~NvEncoderDelegate();

	virtual SInt32 initialize(SInt32 codecType);
	virtual SInt32 uninitialize();

	virtual SInt32 internalGetParameter(SInt32 index, Void* params);
	virtual SInt32 internalSetParameter(SInt32 index, Void* params);

	virtual SInt32 start();
	virtual SInt32 stop();

	virtual SInt32 processInput(BaseBuffer* inputBuffer);
	virtual SInt32 processOutput(BaseBuffer* outputBuffer);

private:
	CNvEncoder* nvEncoder;
};

#endif