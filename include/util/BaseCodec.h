#ifndef BASE_DECODER_H_
#define BASE_DECODER_H_

#include <BaseDataTypes.h>
#include <MediaFormat.h>
#include "Errors.h"

namespace MediaPlugin {

	enum CodecFlags
	{
		CODEC_FLAGS_BASE,
		CODEC_FLAGS_INPUTTYPE,
		CODEC_FLAGS_HEADDATA,
		CODEC_FLAGS_HEADDATA_LENGTH,
		CODEC_FLAGS_FLUSH,
		CODEC_FLAGS_START,
		CODEC_FLAGS_PAUSE,
		CODEC_FLAGS_STOP,
		CODEC_FLAGS_CPUNUM
	};

	class BaseBuffer;
	class BaseCodec
	{
	public:
		BaseCodec() {};
		virtual ~BaseCodec() {};

		virtual SInt32 initialize(SInt32 codecType) { return OK; }
		virtual SInt32 uninitialize() { return OK; }

		virtual SInt32 internalGetParameter(SInt32 index, Void* params) { return OK; }
		virtual SInt32 internalSetParameter(SInt32 index, Void* params) { return OK; }

		virtual SInt32 start() { return OK; }
		virtual SInt32 stop()  { return OK; }

		virtual SInt32 processInput(BaseBuffer* inputBuffer) { return OK; }
		virtual SInt32 processOutput(BaseBuffer* outputBuffer) { return OK; }

	protected:
		enum State
		{
			UNINITIALIZED,
			INITIALIZED,
			CONFIGURED,
			STARTED,
			FLUSHING,
			STOPPING,
			STOPPED
		};

	private:
		BaseCodec& operator=(BaseCodec& codec) {}
		BaseCodec(const BaseCodec &) {}
	};

}
#endif // BASE_DECODER_H_