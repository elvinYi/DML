#include "pusher/dml_pusher.h"
#include "util/snOSFunc.h"
#include "libyuv/libyuv.h"
#include <tchar.h>
#include <math.h>

#define VIDEO_HEADER_SIZE 128
#define MAX_VIDEO_ENCODE_SIZE  1280 * 1280 * 4
#define MAX_VIDEO_FRAME_SIZE 1920*1080*3
#define DEFAULT_FPS 15
#define DEFAULT_PT 97
#define THIS_FILE "dml_pusher.c"
namespace MediaPlugin {

	DMLPusher::DMLPusher()
        : pVideoRender (NULL)
		, bStop(true)
		, nFps(DEFAULT_FPS)
		, pVideoQueue(NULL)
		, videoEncoderHandle(NULL)
		, pVideoHeaderData(NULL)
		, pVideoOutputData(NULL)
		, pYUVBuffer(NULL)
		, pRtpSession(NULL)
		, nFrameNum(0)
		, renderThread(NULL)
		, pusherThread(NULL)

	{
		pPictureSaver = new SavePicture();
		memset(&uiConfig, 0, sizeof(UIConfig));
		memset(&videoEncodeConfig, 0, sizeof(videoEncodeConfig));
		memset(&rtpStreamConfig, 0, sizeof(RtpStreamConfig));
		pYUVBuffer = new uint8_t[MAX_VIDEO_FRAME_SIZE / 2];
		if (!pYUVBuffer){
			return;
		}
	}

	DMLPusher::~DMLPusher(void)
	{
		Stop();
		SAFE_DELETE(pPictureSaver);
		SAFE_DELETE(pYUVBuffer);
		SAFE_DELETE(pRtpSession);
	}

	int16_t DMLPusher::SetConfig(UIConfig * pUiConfig, VideoFrameQueue * pVideoQueue, 
		uint16_t nFps, DMLVideoEncodeInfo *pVideoEncodeConfig,RtpStreamConfig *pRtpConfig)
	{
		if (!pUiConfig || !pVideoQueue || nFps == 0 || !pVideoEncodeConfig || !pRtpConfig){
			return BAD_VALUE;
		}

		this->pVideoQueue = pVideoQueue;
		this->nFps = nFps;
		memcpy(&this->uiConfig, pUiConfig, sizeof(UIConfig));
		memcpy(&this->videoEncodeConfig, pVideoEncodeConfig, sizeof(DMLVideoEncodeInfo));
		this->rtpStreamConfig.memberid = pRtpConfig->memberid;
		this->rtpStreamConfig.port = pRtpConfig->port;
		this->rtpStreamConfig.sSeverAddr = pRtpConfig->sSeverAddr;
		this->rtpStreamConfig.ssrc = pRtpConfig->ssrc;

		return NO_ERROR;
	}

	int16_t DMLPusher::Start()
	{
		if (!bStop){
			return BAD_VALUE;
		}
#ifdef DEBUG
		remove(H264_PATH);
		remove(YUV_PATH);
		fopen_s(&h264Fp, H264_PATH, "ab+");
		fopen_s(&yuvFp, YUV_PATH, "ab+");
#endif
		pVideoHeaderData = new char[VIDEO_HEADER_SIZE];
		if (!pVideoHeaderData){
			return BAD_VALUE;
		}
		videoHeadDataBuffer.setData((const UInt8*)pVideoHeaderData);

		pVideoOutputData = new char[MAX_VIDEO_ENCODE_SIZE];
		if (!pVideoOutputData){
			return BAD_VALUE;
		}
		videoBaseOutputBuffer.setData((const UInt8*)pVideoOutputData);

#if OLNY_D3D_RANDER
		if (!pVideoRender)
			pVideoRender = new RenderManager();

		if (!pVideoRender)
			return BAD_VALUE;

		if (!pVideoRender->Initialize((HWND)uiConfig.windHandle, videoType))
		{
			pVideoRender->CleanUp();
			return BAD_VALUE;
		}
#else
		if (!pVideoRender)
			pVideoRender = new Rander();

		if (!pVideoRender->Init(CAPTURE_WIDTH, CAPTURE_HEIGHT_4X3, (HWND)handle_))
		{
			pVideoRender->Destroy();

		}
#endif
		pRtpSession = new RtpSession(rtpStreamConfig.sSeverAddr, rtpStreamConfig.port, rtpStreamConfig.ssrc, 2, 1, rtpStreamConfig.memberid);
		if (!pRtpSession){
			return BAD_VALUE;
		}

		bStop = false;
		nFrameNum = 0;

		videoEncoderHandle = IEncoder_Create(this, sendVideoEncoderEvent_IF);
		if (!videoEncoderHandle){
			return BAD_VALUE;
		}

		videoBaseInputBuffer.setData((const UInt8*)&videoInputBuffer);
		if (createVideoEncoder() != NO_ERROR){
			return BAD_VALUE;
		}

		if (uiConfig.windMsgCB && uiConfig.pWind){
			uiConfig.windMsgCB(1, DML_EVENT_PUSHER_START, NULL, NULL, uiConfig.pWind);
		}

		renderThread = new std::thread(&DMLPusher::renderWork, this);
		if (!renderThread) {
			return BAD_VALUE;
		}
		pusherThread = new std::thread(&DMLPusher::pusherWork, this);
		if (!pusherThread) {
			return BAD_VALUE;
		}
		return NO_ERROR;
	}

	int16_t DMLPusher::Stop()
	{
		bStop = true;
		snOS_Sleep(50);

		releaseThread(renderThread);
		renderThread = NULL;
		releaseThread(pusherThread);
		pusherThread = NULL;

#if OLNY_D3D_RANDER
		if (pVideoRender)
			pVideoRender->CleanUp();
#else
		if (pVideoRender)
			pVideoRender->Destroy();
#endif

		if (videoEncoderHandle){
			IEncoder_Stop(videoEncoderHandle);
			IEncoder_Uninit(videoEncoderHandle);
			IEncoder_Destory(videoEncoderHandle);
			videoEncoderHandle = NULL;
		}
		SAFE_DELETE(pVideoRender);
		SAFE_DELETE(pVideoHeaderData);
		SAFE_DELETE(pVideoOutputData);
		SAFE_DELETE(pRtpSession);
#ifdef DEBUG
		if (h264Fp)
			fclose(h264Fp);
#endif
		if (uiConfig.windMsgCB && uiConfig.pWind){
			uiConfig.windMsgCB(1, DML_EVENT_PUSHER_STOP, NULL, NULL, uiConfig.pWind);
		}
		return NO_ERROR;
	}

	void DMLPusher::pusherWork()
	{
		uint32_t intervalTime = 0;
		uint32_t elapsedTime = 0;
		uint64_t sysTime = snOS_GetSysTime();
		uint64_t fpsTime = snOS_GetSysTime();
		uint32_t pusherFPS = 0;
		uint32_t nPusherSize = 0;

		while (!bStop){
			intervalTime = (1000 / nFps);
			VideoFrame * pVideoFrame;
			sysTime = snOS_GetSysTime();
			if (pVideoQueue && pVideoQueue->peekData(&pVideoFrame) == OK){
				pusherFPS++;
				unsigned char* pDstY = pYUVBuffer;
				unsigned char* pDstU = pYUVBuffer + pVideoFrame->getWidth() * pVideoFrame->getHeight();
				int nDstUStride = pVideoFrame->getWidth() / 2;
				unsigned char* pDstV = pDstU + pVideoFrame->getWidth() * pVideoFrame->getHeight() / 4;

				libyuv::RGB24ToI420(pVideoFrame->getBuffer(), pVideoFrame->getWidth() * 3,
					pDstY, pVideoFrame->getWidth(),
					pDstU, nDstUStride, pDstV, nDstUStride, pVideoFrame->getWidth(), pVideoFrame->getHeight());

				if (videoEncoderHandle){
					// send data to encoder
					videoInputBuffer.mData[0] = (UInt8*)pDstY;
					videoInputBuffer.mData[1] = (UInt8*)pDstU;
					videoInputBuffer.mData[2] = (UInt8*)pDstV;
					videoInputBuffer.mStride[0] = pVideoFrame->getWidth();
					videoInputBuffer.mStride[1] = nDstUStride;
					videoInputBuffer.mStride[2] = nDstUStride;
					videoInputBuffer.mTime = pVideoFrame->getTimestamp();
					videoInputBuffer.mColorType = (ColorFormat)videoEncodeConfig.colorFormat;
#ifdef DEBUG
					if (yuvFp) {
						fwrite(pDstY, 1, pVideoFrame->getWidth() * pVideoFrame->getHeight() * 3 / 2, yuvFp);
						fflush(yuvFp);
					}
#endif
					Log::d(THIS_FILE, "encoder input frame ");
					int ret = IEncoder_Input(videoEncoderHandle, (BBuffer*)&videoBaseInputBuffer);
					if (ret == OK){
						// get the encode data
						videoBaseOutputBuffer.setLength(MAX_VIDEO_ENCODE_SIZE);
						ret = IEncoder_Output(videoEncoderHandle, (BBuffer*)&videoBaseOutputBuffer);
						
						if (videoBaseOutputBuffer.getLength() > 0){
							if (pRtpSession){
								nFrameNum++;
								nPusherSize += videoBaseOutputBuffer.getLength();
								Log::d(THIS_FILE, "ssrc:%d member:%d encoder output frame size:%d type:%d delay:%d",pRtpSession->getSSRC(),pRtpSession->getMemberid(),
									videoBaseOutputBuffer.getLength(), videoBaseOutputBuffer.getFlags(), snOS_GetSysTime() - videoBaseOutputBuffer.getTimestamp());
								pRtpSession->sendVideoFrame(videoBaseOutputBuffer.getData(), videoBaseOutputBuffer.getLength(),
									videoBaseOutputBuffer.getFlags(), nFrameNum, DEFAULT_PT, 0, 90000 / nFps);
							}
#ifdef DEBUG
							if (h264Fp) {
								fwrite(pVideoOutputData, 1, videoBaseOutputBuffer.getLength(), h264Fp);
								fflush(h264Fp);
							}
#endif
						}
					}
				}

				pVideoQueue->enqueueUsedFrame();
			}

			elapsedTime = snOS_GetSysTime() - sysTime;
			if (elapsedTime < intervalTime)
			{
				snOS_Sleep(intervalTime - elapsedTime);
				//snOS_Sleep(10);
			}

			if (snOS_GetSysTime() - fpsTime >= TIME_ONE_SECOND){
				Log::d(THIS_FILE, "push fps:%d ", pusherFPS);
				if (uiConfig.windMsgCB && uiConfig.pWind){
					nPusherSize /= 1024;
					uiConfig.windMsgCB(1, DML_EVENT_PUSHER_STATE, &pusherFPS, &nPusherSize, uiConfig.pWind);
					//uiConfig.windMsgCB(1, DML_EVENT_PUSHER_BITRATE, &nPusherSize, NULL, uiConfig.pWind);
				}
				pusherFPS = 0;
				nPusherSize = 0;
				fpsTime = snOS_GetSysTime();
			}
		}
	}

	void DMLPusher::renderWork()
	{
		uint32_t intervalTime;

		while (!bStop){
			intervalTime = 1000 / nFps;
			VideoFrame * pVideoFrame;

			if (pVideoQueue && pVideoQueue->peekData(&pVideoFrame) == OK ){
#if OLNY_D3D_RANDER
				if (videoType == VIDEO_DATA_RGB24){
					if (pVideoRender ){
						pVideoRender ->Display(pVideoFrame->getBuffer(), pVideoFrame->getWidth(), pVideoFrame->getHeight());
						//pPictureSaver->SaveRGBAsBmp_ex(pVideoFrame->getBuffer(), "c:\\picture", pVideoFrame->getWidth(), pVideoFrame->getHeight(), "1");
					}
				}
#else
				pVideoRender ->Display(sample_vector[0].codec_buffer, sample_vector[0].time);
#endif
			}
			snOS_Sleep(intervalTime);
		}
	}

	void DMLPusher::releaseThread(std::thread *pThread)
	{
		if (pThread) {
			if (pThread->joinable())
				pThread->join();

			delete pThread;
		}
	}

	void DMLPusher::sendVideoEncoderEvent_IF(IEncoderNotifyHandle h, IEncoderEvent e, void* param)
	{

	}

	int16_t DMLPusher::createVideoEncoder()
	{
		int codec = VIDEO_CODEC_H264;
		if (videoEncodeConfig.videoEncoderType == DMLEncoderType::NVENCODER_265)
			codec = VIDEO_CODEC_H265;

		if (!IEncoder_Init(videoEncoderHandle, IENCODER_VIDEO, (IEncoderIndex)videoEncodeConfig.videoEncoderType, codec)) {
			Log::w(THIS_FILE, "Encoder Init Failed");
			IEncoder_Uninit(videoEncoderHandle);
			IEncoder_Destory(videoEncoderHandle);
			videoEncoderHandle = NULL;
			return BAD_VALUE;
		}

		if (!IEncoder_SetConfig(videoEncoderHandle, IENCODER_SET_KEY_VIDEO_WIDTH, &videoEncodeConfig.width)) {
			Log::w(THIS_FILE, "Encoder Set Config Width Failed ");
			return BAD_VALUE;
		}

		if (!IEncoder_SetConfig(videoEncoderHandle, IENCODER_SET_KEY_VIDEO_HEIGHT, &videoEncodeConfig.height)) {
			Log::w(THIS_FILE, "Encoder Set Config Height Failed ");
			return BAD_VALUE;
		}

		if (!IEncoder_SetConfig(videoEncoderHandle, IENCODER_SET_KEY_VIDEO_BITRATE, &videoEncodeConfig.bitrate)) {
			Log::w(THIS_FILE, "Encoder Set Bitrate Failed ");
			return BAD_VALUE;
		}

		if (!IEncoder_SetConfig(videoEncoderHandle, IENCODER_SET_KEY_VIDEO_FPS, &videoEncodeConfig.fps)) {
			Log::w(THIS_FILE, "Encoder Set FPS Failed ");
			return BAD_VALUE;
		}

		if (!IEncoder_SetConfig(videoEncoderHandle, IENCODER_SET_KEY_VIDEO_I_FRAME_INTERTVAL, &videoEncodeConfig.keyFrameInterval)) {
			Log::w(THIS_FILE, "Encoder Set I Frame Interval Failed ");
			return BAD_VALUE;
		}

		if (!IEncoder_SetConfig(videoEncoderHandle, IENCODER_SET_KEY_VIDEO_CPU_COUNT, &videoEncodeConfig.cpu)) {
			Log::w(THIS_FILE, "Encoder Set Cpu Cnt Failed ");
			return BAD_VALUE;
		}

		if (!IEncoder_SetConfig(videoEncoderHandle, IENCODER_SET_KEY_VIDEO_REFFRAME, &videoEncodeConfig.reFrame)) {
			Log::w(THIS_FILE, "Encoder Set Refframe Failed ");
			return BAD_VALUE;
		}

		if (!IEncoder_SetConfig(videoEncoderHandle, IENCODER_SET_KEY_VIDEO_INPUT_FORMAT, &videoEncodeConfig.colorFormat)) {
			Log::w(THIS_FILE, "Encoder Set Input Format Failed ");
			return BAD_VALUE;
		}

		if (!IEncoder_SetConfig(videoEncoderHandle, IENCODER_SET_KEY_VIDEO_ENC_QUALITY, &videoEncodeConfig.quality)) {
			Log::w(THIS_FILE, "Encoder Set Quality Failed ");
			return BAD_VALUE;
		}

		int bitrateControl = PID_VIDEO_BITRATE_CONTROL_CBR;
		if (videoEncodeConfig.bitrateControl == DMLVideoEncBitrateControl::VBR) {
			bitrateControl = PID_VIDEO_BITRATE_CONTROL_VBR;
		}

		if (!IEncoder_SetConfig(videoEncoderHandle, IENCODER_SET_KEY_VIDEO_ENC_BITRATE_CONTROL_MODE, &bitrateControl)) {
			Log::w(THIS_FILE, "Encoder Set Bitrate Control Failed ");
			return BAD_VALUE;
		}

		if (!IEncoder_SetConfig(videoEncoderHandle, IENCODER_SET_KEY_VIDEO_ENC_PRESET, &videoEncodeConfig.preset)) {
			Log::w(THIS_FILE, "Encoder Set Preset Failed ");
			return BAD_VALUE;
		}

		if (!IEncoder_SetConfig(videoEncoderHandle, IENCODER_SET_KEY_VIDEO_ENC_PROFILE, &videoEncodeConfig.profile)) {
			Log::w(THIS_FILE, "Encoder Set ProFile Failed ");
			return BAD_VALUE;
		}

		if (!IEncoder_SetConfig(videoEncoderHandle, IENCODER_SET_KEY_VIDEO_ENC_B_FRAME_NUM, &videoEncodeConfig.bFrameNum)) {
			Log::w(THIS_FILE, "Encoder Set B Frame Num Failed ");
			return BAD_VALUE;
		}

		if (!IEncoder_SetConfig(videoEncoderHandle, IENCODER_SET_KEY_VIDEO_ENC_ENTROPY_CODING_MODE, &videoEncodeConfig.entropyCodingMode)) {
			Log::w(THIS_FILE, "Encoder Set Coding Mode Failed ");
			return BAD_VALUE;
		}

		if (!IEncoder_SetConfig(videoEncoderHandle, IENCODER_SET_KEY_VIDEO_ENC_CRF, &videoEncodeConfig.crf)) {
			Log::w(THIS_FILE, "Encoder Set CRF Failed ");
			return BAD_VALUE;
		}

		int tune = 7;
		if (!IEncoder_SetConfig(videoEncoderHandle, IENCODER_SET_KEY_VIDEO_ENC_TUNE, &tune)) {
			Log::w(THIS_FILE, "Encoder Set Tune Failed ");
			return BAD_VALUE;
		}
		if (!IEncoder_Start(videoEncoderHandle)) {
			Log::w(THIS_FILE, "Encoder Start Failed ");
			return BAD_VALUE;
		}

		if (!IEncoder_GetParam(videoEncoderHandle, IEncoderGetKey::IENCODER_GET_KEY_HEAD_DATA, &videoHeadDataBuffer)) {
			Log::w(THIS_FILE, "Encoder Set Head Data Failed ");
			return BAD_VALUE;
		}

		videoInputBuffer.mWidth = videoEncodeConfig.width;
		videoInputBuffer.mHeight = videoEncodeConfig.height;

		return NO_ERROR;
	}

}
