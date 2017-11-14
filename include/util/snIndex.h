/************************************************************************
Snail Proprietary
Copyright (c) 2012, Snail Incorporated. All Rights Reserved

Snail, Inc., 4675 Stevens Creek Blvd, Santa Clara, CA 95051, USA

All data and information contained in or disclosed by this document are
confidential and proprietary information of Snail, and all rights
therein are expressly reserved. By accepting this material, the
recipient agrees that this material and the information contained
therein are held in confidence and in trust. The material may only be
used and/or disclosed as authorized in a license agreement controlling
such use and disclosure.
************************************************************************/

#ifndef __snIndex_H__
#define __snIndex_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "snType.h"

/* Define the module ID */
#define _MAKE_SOURCE_ID(id, name) \
SN_INDEX_SRC_##name = _SN_INDEX_SOURCE | id,

#define _MAKE_CODEC_ID(id, name) \
SN_INDEX_DEC_##name = _SN_INDEX_DEC | id, \
SN_INDEX_ENC_##name = _SN_INDEX_ENC | id,

#define _MAKE_EFFECT_ID(id, name) \
SN_INDEX_EFT_##name = _SN_INDEX_EFFECT | id,

#define _MAKE_SINK_ID(id, name) \
SN_INDEX_SNK_##name = _SN_INDEX_SINK | id,

#define _MAKE_FILTER_ID(id, name) \
SN_INDEX_FLT_##name = _SN_INDEX_FILTER | id,

#define _MAKE_PLUGIN_ID(id, name) \
SN_INDEX_PLUGIN_##name = _SN_INDEX_PLUGIN | id,
    
#define _MAKE_CUSTOM_ID(id, name) \
	SN_INDEX_CUSTOM_##name = _SN_INDEX_CUSTOM | id,

enum
{
	_SN_INDEX_SOURCE		= 0x01000000,
	_SN_INDEX_DEC			= 0x02000000,
	_SN_INDEX_ENC			= 0x03000000,
	_SN_INDEX_EFFECT		= 0x04000000,
	_SN_INDEX_SINK			= 0x05000000,
	_SN_INDEX_FILTER		= 0x06000000,
	_SN_INDEX_MFW			= 0x08000000,
    _SN_INDEX_PLUGIN		= 0x0B000000,
	_SN_INDEX_CUSTOM		= 0x0D000000,
    
	// define file parser modules
	_MAKE_SOURCE_ID (0x010000, MP4)
	_MAKE_SOURCE_ID (0x050000, AUDIO)
	_MAKE_SOURCE_ID (0x080000, MKV)
	_MAKE_SOURCE_ID (0x090000, MPEG)
	_MAKE_SOURCE_ID (0x0A0000, RAWDATA)
	_MAKE_SOURCE_ID (0x0B0000, SSP)     // refer to the SSP smooth streaming parser
	_MAKE_SOURCE_ID (0x0C0000, TSP)     // refer to TS parser
	_MAKE_SOURCE_ID (0x0D0000, VMAP)     // refer to VMAP parser
	_MAKE_SOURCE_ID (0x0E0000, SMIL)		//ADDED BY AIVEN
	_MAKE_SOURCE_ID (0x0F0000, XML) 		// ADDED BY AIVEN
	_MAKE_SOURCE_ID (0x001000, DOWNLOADER)	 // ADDED BY AIVEN

	
	// define network source modules
	_MAKE_SOURCE_ID (0x120000, HTTP)
	
	_MAKE_SOURCE_ID (0x121000, HLS)
	_MAKE_SOURCE_ID (0x122000, ISS)
	_MAKE_SOURCE_ID (0x123000, DASH)
	_MAKE_SOURCE_ID (0x124000, MSHTTP)
	_MAKE_SOURCE_ID (0x125000, PD)
	_MAKE_SOURCE_ID (0x126000, PREHTTP)
	_MAKE_SOURCE_ID (0x127000, SOURCEIO)
	_MAKE_SOURCE_ID (0x150000, CONTROLLER)
	_MAKE_SOURCE_ID (0x160000, ADSMANAGER)
	_MAKE_SOURCE_ID (0x170000, PUSHPDMANAGER)
	
	// define CMMB source modules
	_MAKE_SOURCE_ID (0x200000, CMMB)
	_MAKE_SOURCE_ID (0x210000, CMMB_INNO)
	_MAKE_SOURCE_ID (0x220000, CMMB_TELE)
	_MAKE_SOURCE_ID (0x230000, CMMB_SIANO)

	// define DVBT source modules
	_MAKE_SOURCE_ID (0x300000, DVBT)
	_MAKE_SOURCE_ID (0x310000, DVBT_DIBCOM)

	// define other source modules
	_MAKE_SOURCE_ID (0x400000, ID3)

	// define Caption source modules
	_MAKE_SOURCE_ID (0x500000, CLOSEDCAPTION)
	_MAKE_SOURCE_ID (0x510000, EXTSUBTITLE)
	_MAKE_SOURCE_ID (0x520000, DVBSUBTITLE)

	
	// define video codec modules
	_MAKE_CODEC_ID (0x010000, H264)
	_MAKE_CODEC_ID (0x020000, MPEG4)
	_MAKE_CODEC_ID (0x030000, H263)
	_MAKE_CODEC_ID (0x040000, S263)
	_MAKE_CODEC_ID (0x050000, RV)
	_MAKE_CODEC_ID (0x060000, WMV)
	_MAKE_CODEC_ID (0x070000, DIVX3)
	_MAKE_CODEC_ID (0x080000, MJPEG)
	_MAKE_CODEC_ID (0x090000, MPEG2)
	_MAKE_CODEC_ID (0x0A0000, VP6)
	_MAKE_CODEC_ID (0x0B0000, VP8)
	_MAKE_CODEC_ID (0x0C0000, VC1)
	_MAKE_CODEC_ID (0x0D0000, VIDEOPASER)
	_MAKE_CODEC_ID (0x0E0000, H265)
	_MAKE_CODEC_ID (0x100000, HDEC)                    // refer to the Hardware decoder
	_MAKE_CODEC_ID (0x110000, MCDEC)                   // refer to the MediaCodec Decoder


	// define audio codec modules
	_MAKE_CODEC_ID (0x210000, AAC)
	_MAKE_CODEC_ID (0x220000, MP3)
	_MAKE_CODEC_ID (0x230000, WMA)
	_MAKE_CODEC_ID (0x240000, RA)
	_MAKE_CODEC_ID (0x250000, AMRNB)
	_MAKE_CODEC_ID (0x260000, AMRWB)
	_MAKE_CODEC_ID (0x270000, AMRWBP)
	_MAKE_CODEC_ID (0x280000, QCELP)
	_MAKE_CODEC_ID (0x290000, EVRC)
	_MAKE_CODEC_ID (0x2A0000, ADPCM)
	_MAKE_CODEC_ID (0x2B0000, MIDI)
	_MAKE_CODEC_ID (0x2C0000, AC3)
	_MAKE_CODEC_ID (0x2D0000, FLAC)
	_MAKE_CODEC_ID (0x2E0000, DRA)
	_MAKE_CODEC_ID (0x2F0000, OGG)
	_MAKE_CODEC_ID (0x300000, G729)
	_MAKE_CODEC_ID (0x310000, APE)
	_MAKE_CODEC_ID (0x320000, ALAC)
	_MAKE_CODEC_ID (0x330000, EAC3)
	_MAKE_CODEC_ID (0x340000, DTS)
	_MAKE_CODEC_ID (0x350000, SBC)
	_MAKE_CODEC_ID (0x360000, G722)
	_MAKE_CODEC_ID (0x370000, G723)
	_MAKE_CODEC_ID (0x380000, G726)
	_MAKE_CODEC_ID (0x390000, G711)
	_MAKE_CODEC_ID (0x390000, AMC)
	_MAKE_CODEC_ID (0x3A0000, GSM610)
	_MAKE_CODEC_ID (0x3B0000, DSAPLUS)
	// define image codec modules
	_MAKE_CODEC_ID (0x410000, JPEG)
	_MAKE_CODEC_ID (0x420000, GIF)
	_MAKE_CODEC_ID (0x430000, PNG)
	_MAKE_CODEC_ID (0x440000, TIF)

	// define effect modules
	_MAKE_EFFECT_ID (0x010000, EQ)
	_MAKE_EFFECT_ID (0x020000, DOLBY)
	_MAKE_EFFECT_ID (0x030000, AudioSpeed)
	_MAKE_EFFECT_ID (0x040000, Resample)

	// define sink modules
	_MAKE_SINK_ID (0x010000, VIDEO)
	_MAKE_SINK_ID (0x020000, AUDIO)
	_MAKE_SINK_ID (0x030000, CCRRR)
	_MAKE_SINK_ID (0x040000, CCRRV)
	_MAKE_SINK_ID (0x050000, YUVR)						// refer to YUV renderer

	_MAKE_SINK_ID (0x110000, MP4)
	_MAKE_SINK_ID (0x120000, AVI)
	_MAKE_SINK_ID (0x130000, AFW)
	_MAKE_SINK_ID (0x140000, TS)
};


/* define the error ID */
#define SN_ERR_NONE						0x00000000
#define SN_ERR_FINISH					0x00000001
#define SN_ERR_EOS						SN_ERR_FINISH
#define SN_ERR_RETRY					0x00000002
#define SN_ERR_DROPPEDFRAME				0x00000003
#define SN_ERR_RECYCLE					0x00000005
#define SN_ERR_BASE						0X80000000
#define SN_ERR_FAILED					0x80000001
#define SN_ERR_OUTOF_MEMORY				0x80000002
#define SN_ERR_NOT_IMPLEMENT			0x80000003
#define SN_ERR_INVALID_ARG				0x80000004
#define SN_ERR_INPUT_BUFFER_SMALL		0x80000005
#define SN_ERR_OUTPUT_BUFFER_SMALL		0x80000006
#define SN_ERR_FAULT_STATUS				0x80000007
#define SN_ERR_FAULT_PARAMID			0x80000008
#define SN_ERR_CODEC_UNSUPPORTED        0x80000009
#define SN_ERR_FORCE_FLUSH				0x8000000A
#define	SN_ERR_NOW_NO_AUDIO				0x8000000B
#define	SN_ERR_NOW_NO_VIDEO				0x8000000C
#define SN_ERR_FRAME_SHOULD_BE_DROPPED  0x8000000D
#define SN_ERR_TIMESTAMP_RESET			0x8000000E
#define SN_ERR_FRAME_DROPPED			0x8000000F
#define SN_ERR_TIMEOUT					0x80000010
	
#define SN_ERR_LICENSE_ERROR		   (SN_INDEX_MFW_SNLCS|SN_ERR_BASE)

#define	SN_PID_COMMON_BASE				 0x40000000						/*!< The base param ID for common */
#define	SN_PID_COMMON_QUERYMEM			(SN_PID_COMMON_BASE | 0X0001)	/*!< Query the memory needed Reserved. */
#define	SN_PID_COMMON_ISLIVE			(SN_PID_COMMON_BASE | 0X0002)	/*!< Set or Get the input buffer type. SN_INPUT_TYPE */
#define	SN_PID_COMMON_JAVAVM			(SN_PID_COMMON_BASE | 0X0003)	/*!< Query it has resource to use. SN_U32 *, 1 have, 0 No */
#define	SN_PID_COMMON_HEADDATA			(SN_PID_COMMON_BASE | 0X0004)	/*!< The head data of decoder in track. SN_CODECBUFFER * */
#define	SN_PID_COMMON_FLUSH				(SN_PID_COMMON_BASE | 0X0005)	/*!< Flush the codec buffer.SN_U32 *, 1 Flush, 0 No * */
#define	SN_PID_COMMON_START				(SN_PID_COMMON_BASE | 0X0006)	/*!< Start. 0 */
#define	SN_PID_COMMON_PAUSE				(SN_PID_COMMON_BASE | 0X0007)	/*!< Pause  */
#define	SN_PID_COMMON_STOP				(SN_PID_COMMON_BASE | 0X0008)	/*!< Stop  * */
#define	SN_PID_COMMON_UPDATE_VIDEO_DATA	(SN_PID_COMMON_BASE | 0X0009)	/*!< update video data from MediaCodec to OpengGL ES * */
#define	SN_PID_COMMON_HeadInfo			(SN_PID_COMMON_BASE | 0X000B)	/*!< Get the head info desciption. SN_HEAD_INFO * */
#define	SN_PID_COMMON_Listener			(SN_PID_COMMON_BASE | 0X000C)	/*!< Set listener. SN_LISTENER */

#define	SN_PID_COMMON_CPUNUM			(SN_PID_COMMON_BASE | 0X0201)	/*!< Set the cpu number. int * */
#define	SN_PID_COMMON_CPUVERSION		(SN_PID_COMMON_BASE | 0X0202)	/*!< Set the cpu version. int * */
#define	SN_PID_COMMON_LOGFUNC			(SN_PID_COMMON_BASE | 0X0203)	/*!< Set the volog function callback. int * */
#define	SN_PID_COMMON_FRAME_BUF_EX		(SN_PID_COMMON_BASE | 0X0204)	/*!< Set the additional frame buffer num. SN_U32 * */
#define	SN_PID_COMMON_LIVE_STRAT_OPT    (SN_PID_COMMON_BASE | 0X0205)	/*!< Set the live stream start position option. SN_U32 * */
#define	SN_PID_COMMON_WORKPATH			(SN_PID_COMMON_BASE | 0X0207)	/*!< Set the working path. SN_TCHAR*     * */
#define	SN_PID_COMMON_EOF_FLUSH			(SN_PID_COMMON_BASE | 0X0208)	/*!< Flush the codec buffer when EOF.SN_U32 *, 1 Flush, 0 No * */

#define		SN_ENGN_BASE				0x766E6677
#define		SN_EVENT_LOAD_SOURCE				SN_ENGN_BASE + 0x01
#define		SN_EVENT_FORMAT_CHANGED				SN_ENGN_BASE + 0x02
#define		SN_EVENT_FORMAT_CHANGED_DONE		SN_ENGN_BASE + 0x03
#define		SN_EVENT_PLAYBACK_END				SN_ENGN_BASE + 0x04
#define		SN_EVENT_CREATE_VRENDER				SN_ENGN_BASE + 0x05
#define		SN_EVENT_DESTROY_VRENDER			SN_ENGN_BASE + 0x06
#define		SN_EVENT_DETECT_SENSOR				SN_ENGN_BASE + 0x07
#define		SN_EVENT_DECODER_PHASEIN			SN_ENGN_BASE + 0x08 
#define		SN_EVENT_DECODER_PHASEIN_DONE		SN_ENGN_BASE + 0X09
#define		SN_EVENT_ENABLE_VSYNC				SN_ENGN_BASE + 0X0A
#define		SN_EVENT_VSYNC						SN_ENGN_BASE + 0X0B
#define		SN_EVENT_CHECK_RENDER_TIME			SN_ENGN_BASE + 0x0C 
#define		SN_EVENT_FLUSH_VIDEO_DECODER		SN_ENGN_BASE + 0x0D 
#define		SN_EVENT_REQUEST_ADATA				SN_ENGN_BASE + 0x17
#define		SN_EVENT_REQUEST_VDATA				SN_ENGN_BASE + 0x18
#define		SN_EVENT_DECODE_AUDIO				SN_ENGN_BASE + 0x19
#define		SN_EVENT_RENDER_AUDIO				SN_ENGN_BASE + 0x1A
#define		SN_EVENT_DECODE_VIDEO				SN_ENGN_BASE + 0x1B 
#define		SN_EVENT_RENDER_VIDEO				SN_ENGN_BASE + 0x1C 
#define		SN_EVENT_NOW_NO_AUDIO				SN_ENGN_BASE + 0x1D
#define		SN_EVENT_SEEK						SN_ENGN_BASE + 0X1E
#define		SN_EVENT_IMAGE_DECODE_FINISH		SN_ENGN_BASE + 0X1F
#define		SN_EVENT_RENDER_IMAGE				SN_ENGN_BASE + 0X20
#define		SN_EVENT_RECONNECT_SERVER			SN_ENGN_BASE + 0X21

#define		SN_STREAM_BASE				0x866E6677
#define		SN_EVENT_SOCKET_ERR			SN_STREAM_BASE + 0x01
#define		SN_EVENT_BUFFERING_BEGIN	SN_STREAM_BASE + 0x02
#define		SN_EVENT_BUFFERING_END		SN_STREAM_BASE + 0x03
#define		SN_EVENT_BUFFERING_PERCENT	SN_STREAM_BASE + 0x04
#define		SN_EVENT_BUFFERING_FAIL		SN_STREAM_BASE + 0x05

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif // __snIndex_H__
