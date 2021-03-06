#ifndef MEDIA_FORMAT_H_
#define MEDIA_FORMAT_H_

#include <stdint.h>
#include <BaseDataTypes.h>

#ifdef __cplusplus
namespace MediaPlugin {
extern "C" {
#endif

#define MAX_STREAM_COUNT    5

typedef enum StreamFormat
{
    MEDIA_FORMAT_BASE   = 0,
    MEDIA_FORMAT_AVI    = 1,
    MEDIA_FORMAT_FLV,
    MEDIA_FORMAT_MKV,
    MEDIA_FORMAT_MPEG,
    MEDIA_FORMAT_MPEG4,
    MEDIA_FORMAT_TS,
    MEDIA_FORMAT_WMV,
    MEDIA_FORMAT_END
}StreamFormat;

typedef enum StreamTrack
{
    MEDIA_TRACK_BASE    = 0,
    MEDIA_TRACK_AUDIO   = 1,
    MEDIA_TRACK_VIDEO,
    MEDIA_TRACK_SUBTITLE,
    MEDIA_TRACK_END
}StreamTrack;

typedef enum VideoCodecType
{
    VIDEO_CODEC_BASE    = 0,
    VIDEO_CODEC_H263    = 1,
    VIDEO_CODEC_H264,
    VIDEO_CODEC_H265,
    VIDEO_CODEC_MPEG2,
    VIDEO_CODEC_MPEG4,
    VIDEO_CODEC_VC1,
    VIDEO_CODEC_VP8,
    VIDEO_CODEC_VP9,
    VIDEO_CODEC_END
}VideoCodecType;

typedef enum AudioCodecType
{
    AUDIO_CODEC_BASE = 0,
    AUDIO_CODEC_MP3LAME = 1,
    AUDIO_CODEC_AAC,
    AUDIO_CODEC_AC3,
    AUDIO_CODEC_DTS,
    AUDIO_CODEC_WAV,
    AUDIO_CODEC_END
}AudioCodecType;

typedef enum AudioMetaIndex
{
	PID_AUDIO_BASE = 0x40000000,

	PID_AUDIO_FORMAT = (PID_AUDIO_BASE | 0X0001),
	PID_AUDIO_SAMPLEREATE = (PID_AUDIO_BASE | 0X0002),
	PID_AUDIO_SAMPLEBIT = (PID_AUDIO_BASE | 0X0003),
	PID_AUDIO_CHANNELS = (PID_AUDIO_BASE | 0X0004),
	PID_AUDIO_BITRATE = (PID_AUDIO_BASE | 0X0005),
	PID_AUDIO_CHANNELMODE = (PID_AUDIO_BASE | 0X0006),
	PID_AUDIO_CHANNELCONFIG = (PID_AUDIO_BASE | 0X0007),
	PID_AUDIO_USE_OUT_PTS = (PID_AUDIO_BASE | 0X0008),
	PID_AUDIO_DELAY_PER_FRAME = (PID_AUDIO_BASE | 0X0009),
	PID_AUDIO_AAC_PROFILE = (PID_AUDIO_BASE | 0X000A),
	PID_AUDIO_PCM_OUT_CHANNELS = (PID_AUDIO_BASE | 0X000B),
} AudioMetaIndex;

typedef enum VideoMetaIndex
{
    PID_VIDEO_BASE          = 0x41000000,

    PID_VIDEO_WIDTH         = (PID_VIDEO_BASE | 0X0001),
    PID_VIDEO_HEIGHT        = (PID_VIDEO_BASE | 0X0002),
    PID_VIDEO_BITRATE       = (PID_VIDEO_BASE | 0X0003),
    PID_VIDEO_FORMAT        = (PID_VIDEO_BASE | 0X0004),
    PID_VIDEO_ASPECTRATIO   = (PID_VIDEO_BASE | 0X0005),
    PID_VIDEO_DISP_ORIENTATION = (PID_VIDEO_BASE | 0X0006),
    PID_VIDEO_ENC_QUALITY   = (PID_VIDEO_BASE | 0x0007),
    PID_VIDEO_ENC_QP        = (PID_VIDEO_BASE | 0X0008),
    PID_VIDEO_ENC_REFFRAME  = (PID_VIDEO_BASE | 0X0009),
    PID_VIDEO_ENC_I_FRAME_INTERTVAL = (PID_VIDEO_BASE | 0X000A),
    PID_VIDEO_ENC_FPS       = (PID_VIDEO_BASE | 0X000B),
	PID_VIDEO_ENC_INPUT_FORMAT = (PID_VIDEO_BASE | 0X000C),

	PID_VIDEO_CPU_COUNT = (PID_VIDEO_BASE | 0X000D),
	PID_VIDEO_HARDWARE = (PID_VIDEO_BASE | 0X000E),
	PID_VIDEO_ENC_OPTARG = (PID_VIDEO_BASE | 0X000F),
	PID_VIDEO_ENC_PROFILE = (PID_VIDEO_BASE | 0X0010),
	PID_VIDEO_ENC_CRF = (PID_VIDEO_BASE | 0X0011),
	PID_VIDEO_ENC_BUFSIZE = (PID_VIDEO_BASE | 0X0012),
	PID_VIDEO_ENC_PRESET = (PID_VIDEO_BASE | 0X0013),
	PID_VIDEO_ENC_TUNE = (PID_VIDEO_BASE | 0X0014),
	PID_VIDEO_JNI_ENV = (PID_VIDEO_BASE | 0X0015),
	PID_VIDEO_APP_CONTEXT = (PID_VIDEO_BASE | 0X0016),
	PID_VIDEO_APP_HOLDER = (PID_VIDEO_BASE | 0X0017),
	PID_VIDEO_APP_URI = (PID_VIDEO_BASE | 0X0018),
	PID_VIDEO_ENC_ENTROPY_CODING_MODE = (PID_VIDEO_BASE | 0X0019),
	PID_VIDEO_ENC_B_FRAME_NUM = (PID_VIDEO_BASE | 0X001A),
	PID_VIDEO_ENC_BITRATE_CONTROL_MODE = (PID_VIDEO_BASE | 0X001B),
}VideoMetaIndex;

typedef enum VideoEncBitrateControl
{
    PID_VIDEO_BITRATE_CONTROL_BASE          = 0x50000000,
	PID_VIDEO_BITRATE_CONTROL_VBR = (PID_VIDEO_BITRATE_CONTROL_BASE | 0X0001),
	PID_VIDEO_BITRATE_CONTROL_CBR = (PID_VIDEO_BITRATE_CONTROL_BASE | 0X0002),
};

typedef enum VideoEncEntropyCodingMode
{
    PID_VIDEO_ENTROPY_CODING_MODE_BASE          = 0x51000000,
	PID_VIDEO_ENTROPY_CODING_MODE_CAVLC = (PID_VIDEO_ENTROPY_CODING_MODE_BASE | 0X0001),
	PID_VIDEO_ENTROPY_CODING_MODE_CABAC = (PID_VIDEO_ENTROPY_CODING_MODE_BASE | 0X0002),
};

typedef enum VideoEncNvencPreset {
	PID_VIDEO_NVENC_PRESET_BASE = 0x52000000,
	PID_VIDEO_NVENC_PRESET_HQ = (PID_VIDEO_NVENC_PRESET_BASE | 0x0001),             //"hq"
	PID_VIDEO_NVENC_PRESET_LOW_LATENCY_HP = (PID_VIDEO_NVENC_PRESET_BASE | 0x0002), //"lowLatencyHP"
	PID_VIDEO_NVENC_PRESET_HP = (PID_VIDEO_NVENC_PRESET_BASE | 0x0003),             //"hp"
	PID_VIDEO_NVENC_PRESET_LOW_LATENCY_HQ = (PID_VIDEO_NVENC_PRESET_BASE | 0x0004), //"lowLatencyHQ"
	PID_VIDEO_NVENC_PRESET_LOSSLESS = (PID_VIDEO_NVENC_PRESET_BASE | 0x0005),       //"lossless"
	PID_VIDEO_NVENC_PRESET_DEFAULT = (PID_VIDEO_NVENC_PRESET_BASE | 0x0006),        //"default"
};

typedef enum VideoEncX264Preset {
	PID_VIDEO_X264_PRESET_BASE = 0x53000000,
	PID_VIDEO_X264_PRESET_ULTRA_FAST = (PID_VIDEO_X264_PRESET_BASE | 0x0001),
	PID_VIDEO_X264_PRESET_SUPER_FAST = (PID_VIDEO_X264_PRESET_BASE | 0x0002),
	PID_VIDEO_X264_PRESET_VERY_FAST = (PID_VIDEO_X264_PRESET_BASE | 0x0003),
	PID_VIDEO_X264_PRESET_FASTER = (PID_VIDEO_X264_PRESET_BASE | 0x0004),
	PID_VIDEO_X264_PRESET_FAST = (PID_VIDEO_X264_PRESET_BASE | 0x0005),
	PID_VIDEO_X264_PRESET_MEDIUM = (PID_VIDEO_X264_PRESET_BASE | 0x0006),
	PID_VIDEO_X264_PRESET_SLOW = (PID_VIDEO_X264_PRESET_BASE | 0x0007),
	PID_VIDEO_X264_PRESET_SLOWER = (PID_VIDEO_X264_PRESET_BASE | 0x0008),
};

typedef enum VideoEncProfile {
	PID_VIDEO_PROFILE_BASE = 0x54000000,
	PID_VIDEO_PROFILE_MAIN = (PID_VIDEO_PROFILE_BASE | 0x0001),
	PID_VIDEO_PROFILE_HIGH = (PID_VIDEO_PROFILE_BASE | 0x0002),
};

typedef enum ColorFormat
{
    VIDEO_COLOR_FORMAT_BASE     = 0x60000000,
    VIDEO_COLOR_FORMAT_YUV420P  = (VIDEO_COLOR_FORMAT_BASE | 0X0001),
    VIDEO_COLOR_FORMAT_YUV422   = (VIDEO_COLOR_FORMAT_BASE | 0X0002),
    VIDEO_COLOR_FORMAT_YUV444   = (VIDEO_COLOR_FORMAT_BASE | 0X0003),
    VIDEO_COLOR_FORMAT_YV12     = (VIDEO_COLOR_FORMAT_BASE | 0X0004),
    VIDEO_COLOR_FORMAT_NV12     = (VIDEO_COLOR_FORMAT_BASE | 0X0005),
    VIDEO_COLOR_FORMAT_NV21     = (VIDEO_COLOR_FORMAT_BASE | 0X0006),
    VIDEO_COLOR_FORMAT_YVYU     = (VIDEO_COLOR_FORMAT_BASE | 0X0007),
    VIDEO_COLOR_FORMAT_YUY2     = (VIDEO_COLOR_FORMAT_BASE | 0x0008),
    VIDEO_COLOR_FORMAT_UYVY     = (VIDEO_COLOR_FORMAT_BASE | 0x0009),
    VIDEO_COLOR_FORMAT_RGB565   = (VIDEO_COLOR_FORMAT_BASE | 0X0100),   // R:5 G:6 B:5
    VIDEO_COLOR_FORMAT_RGB888   = (VIDEO_COLOR_FORMAT_BASE | 0X0101),   // R:8 G:8 B:8
	VIDEO_COLOR_FORMAT_BGR888   = (VIDEO_COLOR_FORMAT_BASE | 0X0102),
    VIDEO_COLOR_FORMAT_ARGB8888 = (VIDEO_COLOR_FORMAT_BASE | 0X0103),   // A:8 R:8 G:8 B:8
    VIDEO_COLOR_FORMAT_ABGR8888 = (VIDEO_COLOR_FORMAT_BASE | 0X0104),
    VIDEO_COLOR_FORMAT_BGRA8888 = (VIDEO_COLOR_FORMAT_BASE | 0X0105),
	VIDEO_COLOR_FORMAT_RGBA8888 = (VIDEO_COLOR_FORMAT_BASE | 0X0106)
}ColorFormat;

typedef enum VideoColorSpace
{
    VIDEO_CS_DEFAULT,
    VIDEO_CS_SRGB,
    VIDEO_CS_601,
    VIDEO_CS_709,
}VideoColorSpace;

typedef enum VideoRangeType 
{
    VIDEO_RANGE_DEFAULT,
    VIDEO_RANGE_PARTIAL,
    VIDEO_RANGE_FULL
}VideoRangeType;

typedef enum PCMFormat
{
    AUDIO_PCM_FORMAT_BASE       = 0x61000000,
    AUDIO_PCM_FORMAT_8BIT       = (AUDIO_PCM_FORMAT_BASE | 0X0001),
    AUDIO_PCM_FORMAT_16BIT      = (AUDIO_PCM_FORMAT_BASE | 0X0002),
    AUDIO_PCM_FORMAT_32BIT      = (AUDIO_PCM_FORMAT_BASE | 0X0003),
    AUDIO_PCM_FORMAT_FLOAT      = (AUDIO_PCM_FORMAT_BASE | 0X0004)
}PCMFormat;

typedef enum PCMChannel
{
    AUDIO_CHANNEL_BASE          = 0x62000000,
    AUDIO_CHANNEL_MONO          = (AUDIO_CHANNEL_BASE | 0x0001),
    AUDIO_CHANNEL_STEREO        = (AUDIO_CHANNEL_BASE | 0x0002),
    AUDIO_CHANNEL_2POINT1       = (AUDIO_CHANNEL_BASE | 0x0003),
    AUDIO_CHANNEL_QUAD          = (AUDIO_CHANNEL_BASE | 0x0004),
    AUDIO_CHANNEL_4POINT1       = (AUDIO_CHANNEL_BASE | 0x0005),
    AUDIO_CHANNEL_5POINT1       = (AUDIO_CHANNEL_BASE | 0x0006),
    AUDIO_CHANNEL_5POINT1_SURROUND = (AUDIO_CHANNEL_BASE | 0x0007),
    AUDIO_CHANNEL_7POINT1       = (AUDIO_CHANNEL_BASE | 0x0008),
    AUDIO_CHANNEL_7POINT1_SURROUND = (AUDIO_CHANNEL_BASE | 0x0009),
    AUDIO_CHANNEL_SURROUND      = (AUDIO_CHANNEL_BASE | 0x000A)
}PCMChannel;

// Video data buffer, usually used as input pin(encoder) or output pin(decoder) of video codec.
typedef struct VideoBuffer
{
    UInt8*      mData[3];       // Buffer pointer
    SInt32      mStride[3];     // Buffer stride
    SInt32      mWidth;
    SInt32      mHeight;
    SInt64      mTime;          // The time of the buffer
    ColorFormat mColorType;     // Color Type
}VideoBuffer;

// Audio data buffer, usually used as input pin(encoder) or output pin(decoder) of audio codec. 
typedef struct AudioBuffer
{
    UInt8*      mData;
    SInt32      mStride;
    SInt32      mSampleRate;  /*!< Sample rate */
    SInt32      mChannels;    /*!< Channel count */
    SInt32	    mSampleBits;  /*!< Bits per sample */
    SInt64      mTime;      
}AudioBuffer;

enum
{
    MEDIA_ERROR_BASE        = -1000,

    ERROR_ALREADY_CONNECTED = MEDIA_ERROR_BASE - 1,
/*    ERROR_NOT_CONNECTED     = MEDIA_ERROR_BASE - 2,*/
    ERROR_UNKNOWN_HOST      = MEDIA_ERROR_BASE - 3,
    ERROR_CANNOT_CONNECT    = MEDIA_ERROR_BASE - 4,
    ERROR_IO                = MEDIA_ERROR_BASE - 5,
    ERROR_CONNECTION_LOST   = MEDIA_ERROR_BASE - 6,
    ERROR_MALFORMED         = MEDIA_ERROR_BASE - 7,
    ERROR_OUT_OF_RANGE      = MEDIA_ERROR_BASE - 8,
    ERROR_BUFFER_TOO_SMALL  = MEDIA_ERROR_BASE - 9,
    ERROR_UNSUPPORTED       = MEDIA_ERROR_BASE - 10,
    ERROR_END_OF_STREAM     = MEDIA_ERROR_BASE - 11
};

typedef struct VideoStreamInfo
{
    SInt32 mWidth;
    SInt32 mHeight;
    SInt32 mFramerate;
    SInt32 mBitrate;
    VideoCodecType mCodecType;
    ColorFormat mColorType; // capture source use it.
    UInt8* mHeaderData;
    SInt32 mHeaderLength;
}VideoStreamInfo;

typedef struct AudioStreamInfo
{
    SInt32 mSampleRate;
    SInt32 mChannel;
    SInt32 mBitsPerSample;
    SInt32 mBitrate;
    AudioCodecType mCodecType;
    UInt8* mHeaderData;
    SInt32 mHeaderLength;
}AudioStreamInfo;

#ifdef __cplusplus
}  // end extern "C"
} // end namespace MediaPlugin
#endif

#endif  //MEDIA_FORMAT_H_