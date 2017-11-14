#ifndef _PUB_CONST_H_
#define _PUB_CONST_H_

#include <crtdbg.h>

inline void EnableMemLeakCheck()
{
#ifdef _DEBUG
   _CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) | _CRTDBG_LEAK_CHECK_DF);
#endif
}

#ifdef _DEBUG
#define new   new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

#include "PubDef.h"

using namespace std;

#define IED_ENTRY namespace IEncoder {
#define IED_EXIT  };
#define IED_USING using namespace IEncoder;

//#define TTM_DEBUG 0

IED_ENTRY

enum class IEncoderEvent {
	VIDEO_HEADER_ERROR = 0,
};

enum class IEncoderSetKey {
	AUDIO_SAMPLERATE = 0,
	AUDIO_CHANNEL,
	AUDIO_BITRATE,
	AUDIO_BITPERSAMPLE,
	AUDIO_PROFILE,

	VIDEO_WIDTH,
	VIDEO_HEIGHT,
	VIDEO_FPS,
	VIDEO_BITRATE,
	VIDEO_CPU_COUNT,
	VIDEO_I_FRAME_INTERTVAL,
	VIDEO_REFFRAME,
	VIDEO_INPUT_FORMAT,   // YUV420, NV12
	VIDEO_ENC_PRESET,
	VIDEO_ENC_PROFILE,
	VIDEO_ENC_CRF,
	VIDEO_ENC_BITRATE_CONTROL_MODE,
	VIDEO_ENC_QUALITY,
	VIDEO_ENC_B_FRAME_NUM,
	VIDEO_ENC_ENTROPY_CODING_MODE,
	VIDEO_ENC_TUNE,
	FLUSH,
};

enum IEncoderNvencPreset {
	HQ = 0,         //"hq"
	LOW_LATENCY_HP, //"lowLatencyHP"
	HP,             //"hp"
	LOW_LATENCY_HQ, //"lowLatencyHQ"
	LOSSLESS,       //"lossless"
	DEFAULT,        //"default"
};

enum IEncoderX264Preset {
	ULTRA_FAST = 0,
	SUPER_FAST,
	VERY_FAST,
	FASTER,
	FAST,
	MEDIUM,
	SLOW,
	SLOWER,
};

enum IEncoderEntropyCodingMode {
	CAVLC = 0,
	CABAC,
};

enum IEncoderProfile {
	MAIN = 0,
	HIGH,
};

enum class IEncoderInputYUVFormat {
	YUV420 = 0,
	NV12,
};

enum class IEncoderGetKey {
	HEAD_DATA = 0,
};

enum class IEncoderType{
	AUDIO = 0,
	VIDEO,
};

enum class IEncoderIndex {
	DEFAULT = 0,   // audio aac; video select the best encoder
	QUICKSYNC,
	NVENCODER,
	CUDA,
	X264,
	NVENCODER_265,

	FAAC,
};

IED_EXIT

#endif /* _PUB_CONST_H_*/
