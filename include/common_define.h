#ifndef DML_COMMON_DEFINE_H_
#define DML_COMMON_DEFINE_H_

#ifdef WIN32
#include "stdint.h"
#else
#include <stdint.h>
#endif
#include <cstdlib>
#include <string>
#include "util/Errors.h"
#include "util/Log.h"

#define TIME_ONE_SECOND 1000
#define DEFAULT_WIDTH 720
#define DEFAULT_HEIGHT 1280

#define SAFE_DELETE(p) { if(p) { delete (p); (p)=NULL; } }

typedef void(*CALLBACK_MSG)(uint32_t uType, uint32_t uMsg, void *param1, void *param2, void *pthis);//UI

enum DMLEvent {
	// pusher
	DML_EVENT_PUSHER_STATE,
	DML_EVENT_PUSHER_START,
	DML_EVENT_PUSHER_STOP,

	//capture
	DML_EVENT_CAPTURE_OPEN_FAILED,

	//encoder
	DML_EVENT_PUSHER_OPEN_FAILED
};


enum E_SOURCE_TYPE
{
	CAMERA = 0,     //video
	MIC,            //audio
};

enum DMLEncoderType {
	//video
	DEFAULT = 0,// select the best encoder
	QUICKSYNC,
	NVENCODER,
	CUDA,
	X264,
	NVENCODER_265,
	//audio
	FAAC,
};

enum class DMLVideoEncBitrateControl
{
	CBR = 0,
	VBR,
};

enum class DMLVideoEncProfile
{
	MAIN = 0,
	HIGH,
};

enum DMLVideoNvencPreset {
	NVENC_HQ = 0,         //"hq"
	NVENC_LOW_LATENCY_HP, //"lowLatencyHP"
	NVENC_HP,             //"hp"
	NVENC_LOW_LATENCY_HQ, //"lowLatencyHQ"
	NVENC_LOSSLESS,       //"lossless"
	NVENC_DEFAULT,        //"default"
};

enum DMLVideoX264Preset {
	X264_ULTRA_FAST = 0,
	X264_SUPER_FAST,
	X264_VERY_FAST,
	X264_FASTER,
	X264_FAST,
	X264_MEDIUM,
	X264_SLOW,
	X264_SLOWER,
};

enum DMLVideoEntropyCodingMode {
	CAVLC = 0,
	CABAC,
};

typedef struct UIConfig
{
	void* windHandle;
	void* pWind;
	CALLBACK_MSG windMsgCB;
}UIConfig;
typedef struct  RtpStreamConfig
{
	std::string sSeverAddr;
	uint32_t port;
	uint32_t ssrc;
	uint32_t memberid;
}RtpStreamConfig;
typedef struct DMLVideoEncodeInfo {
	int width;
	int height;
	int bitrate;
	int colorFormat;
	int reFrame;     // default 4
	int keyFrameInterval;
	int fps;
	int crf;         // default 12
	bool haveVideo;
	int bFrameNum;   // 0 ~ 2
	int preset;      // see VRVideoNVEncPreset or VRVideoX264Preset
	int quality;     // 0 ~ 10. 0 - low quality; 10 - high quality
	int cpu;
	DMLVideoEncProfile profile;
	DMLVideoEntropyCodingMode entropyCodingMode;
	DMLVideoEncBitrateControl bitrateControl;
	DMLEncoderType videoEncoderType;
} DMLVideoEncodeInfo;


#endif // DML_COMMON_DEFINE_H_
