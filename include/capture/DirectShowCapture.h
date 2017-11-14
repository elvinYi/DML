#ifdef WIN32

#ifndef __DIRECT_SHOW_CAPTURE_H__
#define __DIRECT_SHOW_CAPTURE_H__
#include <windows.h>
#include <MMSystem.h>
#include <strmif.h>
#include <amvideo.h>
#include <stdint.h>
#include <string>
#include <vector>
#include <set>
#include <map>

struct IFilterGraph;
struct IGraphBuilder;		
struct ICaptureGraphBuilder2;
struct IMediaControl;			
struct IMediaEventEx;
struct IMediaSample;
struct IBaseFilter;
struct DirectShowStream;


//extern volatile bool b_destory_;

namespace MediaPlugin {

	class CaptureFilter;

	enum DeviceType
	{
		VIDEO_DEVICE,
		AUDIO_DEVICE
	};

	enum VideoOutputType
	{
		VIDEO_DATA_RGB24,
		VIDEO_DATA_YUV420P
	};

	enum AudioOutputType
	{
		AUDIO_DATA_PCM
	};

	enum AspectRatio
	{
		ASPECT_RATIO_4_3,
		ASPECT_RATIO_16_9
	};

	struct CaptureProperty
	{
		std::string image_type;
		int width;
		int height;
	};

	struct DirectShowStream
	{
		std::string		device_name;
		IBaseFilter     *device_filter;
		CaptureFilter   *capture_filter;
		AM_MEDIA_TYPE   mt;
		union
		{
			VIDEOINFOHEADER video;
			WAVEFORMATEX    audio;
		} header;
	};

	typedef void(*SampleCallback)(void *user_data, IMediaSample *sample, uint64_t time_stamp);

	class DirectShowCapture
	{
	public:
		DirectShowCapture(void);
		~DirectShowCapture(void);

		void Initialize();
		void CleanUp();
		void SetOutputVideoSize(uint32_t width, uint32_t height);
		void SuggestAspectRatio(AspectRatio aspect_ratio);
		void SuggestAvgTimePerFrame(uint32_t avg_time_per_frame);
		void GetVideoCaptureDeviceList(std::vector<std::wstring> &device_list);
		void GetAudioCaptureDeviceList(std::vector<std::wstring> &device_list);
		bool OpenVideoDevice(const std::wstring &video_device, SampleCallback callback_function,
			VideoOutputType output_type, void *user_data, const CaptureProperty *property = NULL);
		bool OpenAudioDevice(const std::wstring &audio_device, SampleCallback callback_function, AudioOutputType output_type, void *user_data);
		bool CloseVideoDevice();
		bool CloseAudioDevice();
		bool GetVideoProperity(uint32_t &width, uint32_t &height, uint32_t &frame_rate, uint32_t &frame_rate_base);
		bool GetAudioProperity(uint16_t &channels, uint32_t &sample_rate, uint16_t &bits_per_sample);
		bool StartCapture();
		bool StopCapture();
		void CreateDirectShowGraph();
		void DeleteDirectShowGraph();
		bool ShowVideoPropertyPage();
		bool ShowAudioPropertyPage();
		bool GetVideoPeripheralCaptureDeviceProperty(const std::wstring &video_device, std::vector<CaptureProperty> &supported_property, CaptureProperty &default_property);
		std::string MediaTypeToBasicString(const GUID &media_type);
		GUID BasicStringToGUID(const std::string &media_type);
	private:
		bool OpenDevice(const std::wstring &device_name, DeviceType device_type, SampleCallback callback_function,
			const void *output_type, void *user_data, const CaptureProperty *property = NULL);
		bool CloseDevice(DeviceType device_type);
		bool ShowPropertyPage(DeviceType device_type);
		IBaseFilter *FindCaptureDevice(const std::wstring &device_name, DeviceType device_type);

		//bool SetOutputFormat( IBaseFilter *filter, AM_MEDIA_TYPE *media_type);

		bool IntelligentConfigVideoDevice(IBaseFilter *device_filter, const CaptureProperty *property = NULL);

		uint32_t GetMediaFormatPriority(AM_MEDIA_TYPE *media_type, const CaptureProperty *property = NULL);

		bool ConnectFilters(IBaseFilter *base_filter, CaptureFilter *capture_filter);

		HRESULT	FindPinByIndex(IBaseFilter* pFilter, PIN_DIRECTION dir, IPin** ppPin, int nIndex, bool& bConected);
		bool IsPinConnected(IPin* pPin);

		IFilterGraph *				graph_;
		ICaptureGraphBuilder2*		capture_graph_builder_;
		IMediaControl *				media_control_;

		DirectShowStream			video_stream_;
		DirectShowStream			audio_stream_;

		bool						video_device_opened_;
		bool						audio_device_opened_;

		std::vector<std::wstring>		audio_device_list_;
		std::vector<std::wstring>		video_device_list_;

		uint32_t                    output_video_width_;
		uint32_t                    output_video_height_;
		uint32_t                    suggested_avg_time_per_frame_;

		double                      suggested_aspect_ratio_;

		std::map<std::string, GUID> video_type_table_;
	};

}
#endif

#endif