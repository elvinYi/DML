#ifndef  _DML_PUSHER_H_
#define  _DML_PUSHER_H_
#include "pusher/rtp_sender.h"
#include <string>
#include <stdint.h>
#include <vector>
#include <common_define.h>
#include "Render/RenderManager.h"
#include "capture/AVFrameQueue.h"
#include "encoder/interface/IEncoder.h"
#include "BaseBuffer.h"
#include "MediaFormat.h"
#include "SavePicture.h"
#include <thread>

#define  OUT_PUT_FRAMES 20
#define  OLNY_D3D_RANDER        1

//#define DEBUG 

#ifdef DEBUG
#define H264_PATH ".\\v.264"
#define YUV_PATH ".\\v.yuv"
static FILE *h264Fp = NULL;
static FILE *yuvFp = NULL;
#endif
using namespace MediaCoreAVFrame;
class RenderManager;
class Render;
namespace MediaPlugin {
	class BaseBuffer;

	class DMLPusher
	{
	public:
		DMLPusher(void);
		virtual ~DMLPusher(void);

		int16_t  SetConfig(UIConfig * pUiConfig, VideoFrameQueue * pVideoQueue,
			uint16_t nFps, DMLVideoEncodeInfo *pVideoEncodeConfig, RtpStreamConfig *pRtpConfig);
		int16_t  Start();
		int16_t  Stop();
	private:
		void            renderWork();
		void            pusherWork();
		void *          pParent;
		UIConfig        uiConfig;
		void            releaseThread(std::thread *pThread);

		static void sendVideoEncoderEvent_IF(IEncoderNotifyHandle h, IEncoderEvent e, void* param);
		void sendVideoEncoderEvent(IEncoderNotifyHandle h, IEncoderEvent e);
		int16_t createVideoEncoder();
#if OLNY_D3D_RANDER
		RenderManager *				pVideoRender;
#else
		Render        *             pVideoRender;
#endif 
		std::thread *               renderThread;
		std::thread *               pusherThread;
		VideoDataType               videoType;
		bool                        bStop;
		int16_t                     nFps;
		uint64_t                    nFrameNum;
		VideoFrameQueue *           pVideoQueue;
		SavePicture *               pPictureSaver;

		char *pVideoHeaderData;
		BaseBuffer videoHeadDataBuffer;
		IEncoderHandle videoEncoderHandle;

		char *pVideoOutputData;
		VideoBuffer videoInputBuffer;
		BaseBuffer videoBaseInputBuffer;
		BaseBuffer videoBaseOutputBuffer;

		uint8_t *pYUVBuffer;

		RtpSession * pRtpSession;

		DMLVideoEncodeInfo videoEncodeConfig;
		RtpStreamConfig rtpStreamConfig;
	};


}

#endif