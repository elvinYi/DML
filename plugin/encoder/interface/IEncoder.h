#ifndef _I_ENCODER_H_
#define _I_ENCODER_H_

#ifdef LIBIENCODER_EXPORTS
#define LIBIENCODER_API   __declspec( dllexport ) 
#define LIBIENCODER_CLS   __declspec( dllexport ) 
#else
#define LIBIENCODER_API
//#define LIBIENCODER_API   __declspec( dllimport ) 
//#define LIBIENCODER_CLS   __declspec( dllexport ) 
#endif

#ifdef __cplusplus
extern "C" {
#endif

	struct BBuffer;

	enum IEncoderEvent {
		IENCODER_EVENT_VIDEO_HEADER_ERROR = 0,
	};

	enum IEncoderNvencPreset {
		IENCODER_NVENC_PRESET_HQ = 0,         //"hq"
		IENCODER_NVENC_PRESET_LOW_LATENCY_HP, //"lowLatencyHP"
		IENCODER_NVENC_PRESET_HP,             //"hp"
		IENCODER_NVENC_PRESET_LOW_LATENCY_HQ, //"lowLatencyHQ"
		IENCODER_NVENC_PRESET_LOSSLESS,       //"lossless"
		IENCODER_NVENC_PRESET_DEFAULT,        //"default"
	};

	enum IEncoderX264Preset {
		IENCODER_X264_PRESET_ULTRA_FAST = 0,
		IENCODER_X264_PRESET_SUPER_FAST,
		IENCODER_X264_PRESET_VERY_FAST,
		IENCODER_X264_PRESET_FASTER,
		IENCODER_X264_PRESET_FAST,
		IENCODER_X264_PRESET_MEDIUM,
		IENCODER_X264_PRESET_SLOW,
		IENCODER_X264_PRESET_SLOWER,
	};

	enum IEncoderProfile {
		IENCODER_PROFILE_MAIN = 0,
		IENCODER_PROFILE_HIGH,
	};

	enum IEncoderEntropyCodingMode {
		IENCODER_ENTROPY_CODING_MODE_CAVLC = 0,
		IENCODER_ENTROPY_CODING_MODE_CABAC,
	};

	enum IEncoderSetKey {
		IENCODER_SET_KEY_AUDIO_SAMPLERATE = 0,
		IENCODER_SET_KEY_AUDIO_CHANNEL,
		IENCODER_SET_KEY_AUDIO_BITRATE,
		IENCODER_SET_KEY_AUDIO_BITPERSAMPLE,
		IENCODER_SET_KEY_AUDIO_PROFILE,

		IENCODER_SET_KEY_VIDEO_WIDTH,
		IENCODER_SET_KEY_VIDEO_HEIGHT,
		IENCODER_SET_KEY_VIDEO_FPS,
		IENCODER_SET_KEY_VIDEO_BITRATE,
		IENCODER_SET_KEY_VIDEO_CPU_COUNT,
		IENCODER_SET_KEY_VIDEO_I_FRAME_INTERTVAL,
		IENCODER_SET_KEY_VIDEO_REFFRAME,
		IENCODER_SET_KEY_VIDEO_INPUT_FORMAT,   // YUV420, NV12
		IENCODER_SET_KEY_VIDEO_ENC_PRESET,
		IENCODER_SET_KEY_VIDEO_ENC_PROFILE,
		IENCODER_SET_KEY_VIDEO_ENC_CRF,
		IENCODER_SET_KEY_VIDEO_ENC_BITRATE_CONTROL_MODE,
		IENCODER_SET_KEY_VIDEO_ENC_QUALITY,
		IENCODER_SET_KEY_VIDEO_ENC_B_FRAME_NUM,
		IENCODER_SET_KEY_VIDEO_ENC_ENTROPY_CODING_MODE,
		IENCODER_SET_KEY_VIDEO_ENC_TUNE,
		IENCODER_SET_KEY_FLUSH,
	};

	enum IEncoderInputYUVFormat {
		IENCODER_INPUT_YUV_FORMAT_YUV420 = 0,
		IENCODER_INPUT_YUV_FORMAT_NV12,
	};

	enum IEncoderGetKey {
		IENCODER_GET_KEY_HEAD_DATA = 0,
	};

	enum IEncoderType {
		IENCODER_AUDIO = 0,
		IENCODER_VIDEO,
	};

	enum IEncoderIndex {
		IENCODER_VIDEO_INDEX_DEFAULT = 0,   // select the best encoder
		IENCODER_VIDEO_INDEX_QUICKSYNC,
		IENCODER_VIDEO_INDEX_NVENCODER,
		IENCODER_VIDEO_INDEX_CUDA,
		IENCODER_VIDEO_INDEX_X264,
		IENCODER_AUDIO_INDEX_NVENCODER_265,

		IENCODER_AUDIO_INDEX_FAAC,
	};

	typedef void* IEncoderHandle;
	typedef void* IEncoderNotifyHandle;
	typedef void(*SendIEncoderEventProc)(IEncoderNotifyHandle h, IEncoderEvent e, void* param);

	LIBIENCODER_API IEncoderHandle IEncoder_Create(IEncoderNotifyHandle h, SendIEncoderEventProc s);
	
	LIBIENCODER_API bool IEncoder_Init(IEncoderHandle h, IEncoderType type, IEncoderIndex index, int codec);
	
	LIBIENCODER_API bool IEncoder_Start(IEncoderHandle h);

	LIBIENCODER_API bool IEncoder_SetConfig(IEncoderHandle h, IEncoderSetKey key, void* config);
	
	LIBIENCODER_API bool IEncoder_GetParam(IEncoderHandle h, IEncoderGetKey key, void* param);

	LIBIENCODER_API void IEncoder_Stop(IEncoderHandle h);
	
	LIBIENCODER_API void IEncoder_Uninit(IEncoderHandle h);

	LIBIENCODER_API int IEncoder_Input(IEncoderHandle h, BBuffer* input);

	LIBIENCODER_API int IEncoder_Output(IEncoderHandle h, BBuffer* output);

	LIBIENCODER_API void IEncoder_Destory(IEncoderHandle h);

#ifdef __cplusplus
}
#endif

#endif