#include "capture/dml_capture.h"
#include <capture/StringConverter.h>
#include "util/snOSFunc.h"
#include "ImgRotater.h"
#include <stdlib.h>

#define LOG_FILE "./capture.log"
#define THIS_FILE "dml_capture.c"
#define MAX_VIDEO_FRAME 2

namespace MediaPlugin {

	DMLVideoCapture::DMLVideoCapture()
		: capture_(NULL), direct_show_time_base_(INT64_MAX), nFps(30), nOutputWidth(DEFAULT_WIDTH), nOutputHeight(DEFAULT_HEIGHT),
		nInputWidth(0), nInputHeight(0),videoType(VIDEO_DATA_RGB24),pVideoQueue(NULL),stop_(true), prev_dshow_time_stamp_(0),
		avg_time_per_frame_(10000000 / nFps), camera_status_(false), pRotateBuffer(NULL), pSrcResizeFrame(NULL), pDstResizeFrame(NULL),
		pResizeContext(NULL), bNeedRotate(false), nInputFps(0)
	{
		nVideoFrameSize = nOutputWidth * nOutputHeight * 3;
		memset(&uiConfig, 0, sizeof(UIConfig));
		fpsTime = snOS_GetSysTime();
		capture_ = new DirectShowCapture();
		capture_->Initialize();
		capture_->SetOutputVideoSize(nOutputWidth, nOutputWidth);
		capture_->SuggestAvgTimePerFrame(avg_time_per_frame_);
		pRotateBuffer = (uint8_t*) _aligned_malloc(nOutputWidth * nOutputHeight * 3,32);
		pResizeBuffer = (uint8_t*)_aligned_malloc(nOutputWidth * nOutputHeight * 3, 32);
		pBMPSaver = new SavePicture();
		//AllocConsole(); //debug console
	}

	DMLVideoCapture::~DMLVideoCapture()
	{
		//FreeConsole();
		stop();

		if (capture_ != NULL){
			capture_->CleanUp();
			delete capture_;
			capture_ = NULL;
		}

		if (pRotateBuffer){
			_aligned_free(pRotateBuffer);
			pRotateBuffer = NULL;
		}

		if (pResizeBuffer){
			_aligned_free(pResizeBuffer);
			pResizeBuffer = NULL;
		}

		SAFE_DELETE(pBMPSaver);
	}

	int16_t DMLVideoCapture::start(const std::string &source)
	{
		if (capture_ != NULL)
		{
			//set_fps(60);
			pVideoQueue = new VideoFrameQueue(0, MAX_VIDEO_FRAME);

			if (!pVideoQueue){
				//log
				return BAD_VALUE;
			}

			pVideoQueue->allocateMemory(nVideoFrameSize);
			pVideoQueue->clearData();

			if (nOutputWidth < nOutputHeight){
				capture_->SetOutputVideoSize(800, 600);
			}
			else {
				capture_->SetOutputVideoSize(nOutputWidth, nOutputHeight);
			}
			capture_->SuggestAvgTimePerFrame(avg_time_per_frame_);
			capture_->CloseVideoDevice();
			capture_->DeleteDirectShowGraph();
			capture_->CreateDirectShowGraph();

			CaptureProperty *property = NULL;
			if ((capture_property_.image_type != "") && (capture_property_.width != 0) && (capture_property_.height != 0)){
				property = &capture_property_;
			}
			if (capture_->OpenVideoDevice(StringConverter::utf8_to_unicode(source),
				on_video_sample_receive, VIDEO_DATA_RGB24, this, property) && capture_->StartCapture()){
				uint32_t frame_rate = 0;
				uint32_t frame_rate_base = 0;
				capture_->GetVideoProperity(nInputWidth, nInputHeight, frame_rate, frame_rate_base);
				if (nInputWidth <= 0 || nInputHeight <= 0 || frame_rate <= 0)
					return BAD_VALUE;

				if ((nInputWidth > nInputHeight && nOutputWidth < nOutputHeight) ||
					(nInputWidth < nInputHeight && nOutputWidth > nOutputHeight)){
					bNeedRotate = true;
				}

				releaseFFmpegResizer();
				pSrcResizeFrame = av_frame_alloc();//avcodec_alloc_frame();
				pDstResizeFrame = av_frame_alloc();//avcodec_alloc_frame();
				if (bNeedRotate){
					pResizeContext = sws_getContext(nInputWidth, nInputHeight, PIX_FMT_BGR24,
						nOutputHeight, nOutputWidth, PIX_FMT_BGR24, SWS_BICUBIC, NULL, NULL, NULL);
				}else{
					pResizeContext = sws_getContext(nInputWidth, nInputHeight, PIX_FMT_BGR24,
						nOutputWidth, nOutputHeight, PIX_FMT_BGR24, SWS_BICUBIC, NULL, NULL, NULL);
				}

				prev_dshow_time_stamp_ = 0;
				stop_ = false;
				//last_camera_name_ = source;

				camera_status_ = true;
				return NO_ERROR;
			}
		}
		Log::w(THIS_FILE, "Open Camera Failed!Camera:%s Input Width:%d Input Height:%d Output Width:%d Output Height:%d",
			source.c_str(), nInputWidth, nInputHeight, nOutputWidth, nOutputHeight);
		if (uiConfig.windMsgCB && uiConfig.pWind){
			uiConfig.windMsgCB(1, DML_EVENT_CAPTURE_OPEN_FAILED, NULL, NULL, uiConfig.pWind);
		}
		camera_status_ = false;
		return BAD_VALUE;
	}

	void DMLVideoCapture::stop(void)
	{
		stop_ = true;

		if (capture_ != NULL){
			capture_->StopCapture();
			capture_->CloseVideoDevice();
			capture_->DeleteDirectShowGraph();
		}

		if (pVideoQueue){
			delete pVideoQueue;
			pVideoQueue = NULL;
		}

		releaseFFmpegResizer();
		direct_show_time_base_ = INT64_MAX;

		camera_status_ = false;
	}

	int16_t DMLVideoCapture::get_device_list(std::vector<std::string> &device_list)
	{
		std::vector<std::wstring> device_unicode_list;

		if (capture_ != NULL){
			capture_->GetVideoCaptureDeviceList(device_unicode_list);
			device_list.clear();
			for (std::vector<std::wstring>::const_iterator iter = device_unicode_list.begin();
				iter != device_unicode_list.end(); ++iter){
				device_list.push_back(StringConverter::unicode_to_utf8(*iter));
			}
			return NO_ERROR;
		}
		return BAD_VALUE;
	}

	int16_t DMLVideoCapture::set_capture_property(const CaptureProperty &property)
	{
		capture_property_ = property;
		if (camera_status_){
			stop();
			start(last_camera_name_);
		}
		return NO_ERROR;
	}

	int16_t DMLVideoCapture::show_property_page()
	{
		return capture_->ShowVideoPropertyPage() ? NO_ERROR : BAD_VALUE;
	}

	int16_t DMLVideoCapture::get_device_capture_property(const std::string &device_name,
		std::vector<CaptureProperty> &property, CaptureProperty &default_property)
	{
		if (capture_->GetVideoPeripheralCaptureDeviceProperty(StringConverter::utf8_to_unicode(device_name), property, default_property)){
			return NO_ERROR;
		}
		return BAD_VALUE;
	}

	void DMLVideoCapture::on_video_sample_receive(void *user_data, IMediaSample *sample, uint64_t time_stamp)
	{
		DMLVideoCapture *context = static_cast<DMLVideoCapture *>(user_data);

		uint8_t *dshow_sample_data_pointer = NULL;
		int dshow_sample_data_size = 0;

		REFERENCE_TIME pts, end_date;
		uint32_t video_width = 0;
		uint32_t video_height = 0;
		uint32_t frame_rate = 0;
		uint32_t frame_rate_base = 0;
		uint64_t inputTime = snOS_GetSysTime();
		sample->GetPointer(&dshow_sample_data_pointer);
		if (!dshow_sample_data_pointer || !context->pResizeContext){
			return;
		}
		context->nInputFps++;
		dshow_sample_data_size = sample->GetActualDataLength();
		sample->GetTime(&pts, &end_date);
		if (context->direct_show_time_base_ == INT64_MAX){
			context->direct_show_time_base_ = pts;
		}

		//context->capture_->GetVideoProperity(context->nInputWidth, context->nInputHeight, frame_rate, frame_rate_base);
		//context->pBMPSaver->SaveRGBAsBmp_ex(const_cast<uint8_t *>(dshow_sample_data_pointer), "c:\\picture", context->nInputWidth, context->nInputHeight, "1");
		if (context->bNeedRotate){
			memcpy(context->pRotateBuffer, dshow_sample_data_pointer, dshow_sample_data_size);
			avpicture_fill(reinterpret_cast<AVPicture *>(context->pSrcResizeFrame), 
				const_cast<uint8_t *>(context->pRotateBuffer), PIX_FMT_BGR24, context->nInputWidth, context->nInputHeight);
			avpicture_fill(reinterpret_cast<AVPicture *>(context->pDstResizeFrame), 
				context->pResizeBuffer, PIX_FMT_BGR24, context->nOutputHeight, context->nOutputWidth);
			context->pSrcResizeFrame->linesize[0] = context->nInputWidth * 3;
			context->pSrcResizeFrame->data[0] += context->pSrcResizeFrame->linesize[0] * (context->nInputHeight - 1);
			context->pSrcResizeFrame->linesize[0] *= -1;
			context->pDstResizeFrame->linesize[0] = context->nOutputHeight * 3;

			sws_scale(context->pResizeContext, context->pSrcResizeFrame->data, context->pSrcResizeFrame->linesize,
				0, context->nInputHeight, context->pDstResizeFrame->data, context->pDstResizeFrame->linesize);
		}else{
			memcpy(context->pRotateBuffer, dshow_sample_data_pointer, dshow_sample_data_size);
			avpicture_fill(reinterpret_cast<AVPicture *>(context->pSrcResizeFrame),
				const_cast<uint8_t *>(context->pRotateBuffer), PIX_FMT_BGR24, context->nInputWidth, context->nInputHeight);
			avpicture_fill(reinterpret_cast<AVPicture *>(context->pDstResizeFrame),
				context->pResizeBuffer, PIX_FMT_BGR24, context->nOutputWidth, context->nOutputHeight);
			context->pSrcResizeFrame->linesize[0] = context->nInputWidth * 3;
			context->pSrcResizeFrame->data[0] += context->pSrcResizeFrame->linesize[0] * (context->nInputHeight - 1);
			context->pSrcResizeFrame->linesize[0] *= -1;
			context->pDstResizeFrame->linesize[0] = context->nOutputWidth * 3;

			sws_scale(context->pResizeContext, context->pSrcResizeFrame->data, context->pSrcResizeFrame->linesize,
				0, context->nInputHeight, context->pDstResizeFrame->data, context->pDstResizeFrame->linesize);
		}
		//context->pBMPSaver->SaveRGBAsBmp_ex(context->pResizeBuffer, "c:\\picture", context->nOutputHeight, context->nOutputWidth, "2");
		if (context->bNeedRotate){
			RGBRotate(context->pResizeBuffer, context->pRotateBuffer, context->nOutputHeight, context->nOutputWidth, 3, Angle_270);
		}else{
			RGBRotate(context->pResizeBuffer, context->pRotateBuffer, context->nOutputWidth, context->nOutputHeight, 3, Angle_0);
		}
		//context->pBMPSaver->SaveRGBAsBmp_ex(context->pRotateBuffer, "c:\\picture", context->nOutputWidth, context->nOutputHeight, "3");
		if (snOS_GetSysTime() - context->fpsTime >= TIME_ONE_SECOND){
			Log::d(THIS_FILE, "Camera Fps:%d", context->nInputFps);
			context->nInputFps = 0;
			context->fpsTime = snOS_GetSysTime();
		}
		VideoFrame videoFrame(context->pRotateBuffer, context->nVideoFrameSize, inputTime,
			0, context->nOutputWidth, context->nOutputHeight, VIDEO_COLOR_FORMAT_RGB888);
		context->pVideoQueue->putData(&videoFrame);
	
	}

	int16_t DMLVideoCapture::SetConfig(UIConfig *pUiConfig,uint32_t nWidth, uint32_t nHeight, int16_t nFps, VideoOutputType videoType)
	{
		if (!nWidth || !nHeight || !nFps || !pUiConfig){
			return BAD_VALUE;
		}

		this->nOutputWidth = nWidth;
		this->nOutputHeight = nHeight;
		this->nFps = 30;
		this->avg_time_per_frame_ = 10000000 / 30;
		this->videoType = videoType;

		if (this->videoType == VIDEO_DATA_RGB24){
			this->nVideoFrameSize = nWidth * nHeight * 3;
		}else{
			this->nVideoFrameSize = nWidth * nHeight * 3 / 2;
		}

		memcpy(&this->uiConfig, pUiConfig, sizeof(UIConfig));
		return NO_ERROR;
	}

	VideoFrameQueue * DMLVideoCapture::GetVideoQueue()
	{
		return this->pVideoQueue;
	}

	void DMLVideoCapture::releaseFFmpegResizer()
	{
		if (pSrcResizeFrame){
			av_free(pSrcResizeFrame);
			pSrcResizeFrame = NULL;
		}

		if (pDstResizeFrame){
			av_free(pDstResizeFrame);
			pDstResizeFrame = NULL;
		}

		if (pResizeContext){
			sws_freeContext(pResizeContext);
			pResizeContext = NULL;
		}
	}

}