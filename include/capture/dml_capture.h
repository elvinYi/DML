#ifndef   _DML_VIDEO_CAPTURE_
#define   _DML_VIDEO_CAPTURE_

#include "common_define.h"
#include "util/snBaseThread.h"
#include "capture/DirectShowCapture.h"
#include "capture/AVFrameQueue.h"
#include "SavePicture.h"

using namespace MediaCoreAVFrame;

extern "C"
{
#ifdef WIN32
#include "libavformat/avformat.h"
#endif
#include "libavutil/avutil.h"
#include "libswscale/swscale.h"
};


namespace MediaPlugin {

	class DirectShowCapture;
	struct CaptureProperty;

	class DMLVideoCapture 
	{

	public:
		DMLVideoCapture();
		~DMLVideoCapture();

		int16_t  start(const std::string &source);
		void  stop(void);

		int16_t  get_device_list(std::vector<std::string> &source_list);

		int16_t  set_fps(int16_t fps);

		int16_t  SetResolution(uint32_t nWidth, uint32_t nHeight);

		int16_t  SetConfig(UIConfig *pUiConfig,uint32_t nWidth, uint32_t nHeight, int16_t nFps, VideoOutputType videoType);

		int16_t  set_capture_property(const CaptureProperty &property);

		int16_t  show_property_page();

		int16_t  get_device_capture_property(const std::string &device_name, std::vector<CaptureProperty> &property, CaptureProperty &default_property);

		VideoFrameQueue * GetVideoQueue();

	private:
		DirectShowCapture *     capture_;
		VideoFrameQueue *       pVideoQueue;
		uint8_t *               pRotateBuffer;
		uint8_t *               pResizeBuffer;
		SavePicture *           pBMPSaver;

		AVFrame *               pSrcResizeFrame;
		AVFrame *               pDstResizeFrame;
		SwsContext *            pResizeContext;
		bool                    bNeedRotate;

		volatile bool           stop_;

		UIConfig               uiConfig;
		int64_t                direct_show_time_base_;
		uint16_t               nFps;
		uint32_t               nInputWidth;
		uint32_t               nInputHeight;
		uint32_t               nOutputWidth;
		uint32_t               nOutputHeight;
		VideoOutputType        videoType;
		uint32_t               nVideoFrameSize;
		uint16_t               nInputFps;
		uint64_t               fpsTime;

		uint64_t               prev_dshow_time_stamp_;
		int32_t                avg_time_per_frame_;
		CaptureProperty        capture_property_;

		std::string            last_camera_name_;
		bool                   camera_status_;

		void releaseFFmpegResizer();
		static void on_video_sample_receive(void *user_data, IMediaSample *sample, uint64_t time_stamp);
	};
}
















#endif