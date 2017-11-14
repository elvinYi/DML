#ifdef WIN32
#include "DirectShowCapture.h"
#include "DirectShowFilter.h"

#include <limits.h>
#include <tchar.h>
#include <strsafe.h>
#include <atlstr.h>
#include <stdio.h>
#include <dshow.h>
#include <strmif.h>
#include <amvideo.h>
#include <comutil.h>
#include <algorithm>

#ifdef _DEBUG
#	pragma comment(lib, "strmbasd.lib")
#else 
#	pragma comment(lib, "strmbase.lib")
#endif // _DEBUG
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "dmoguids.lib")
#pragma comment(lib, "comsuppw.lib")

#define msg_Dbg
#define DIRECT_SHOW_STRING_INTERRUPT 32

//extern volatile bool b_destory_;


namespace MediaPlugin {

	DirectShowCapture::DirectShowCapture(void)
		: graph_(NULL), capture_graph_builder_(NULL), media_control_(NULL),
		video_device_opened_(false), audio_device_opened_(false),
		output_video_width_(0), output_video_height_(0), suggested_avg_time_per_frame_(0),
		suggested_aspect_ratio_(4.0 / 3)
	{
		ZeroMemory(&video_stream_, sizeof(video_stream_));
		ZeroMemory(&audio_stream_, sizeof(audio_stream_));
		static const GUID MY_MEDIASUBTYPE_I420 = { 0x30323449, 0x0000, 0x0010, { 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71 } };

#define ADD_MEDIA_TYPE_TO_TABLE(media_type_string, media_type_guid) \
	DirectShowCapture::video_type_table_[media_type_string] = media_type_guid;

		ADD_MEDIA_TYPE_TO_TABLE("RGB24", MEDIASUBTYPE_RGB24);
		ADD_MEDIA_TYPE_TO_TABLE("RGB32", MEDIASUBTYPE_RGB32);
		ADD_MEDIA_TYPE_TO_TABLE("ARGB32", MEDIASUBTYPE_ARGB32);

		ADD_MEDIA_TYPE_TO_TABLE("YVYU", MEDIASUBTYPE_YVYU);
		ADD_MEDIA_TYPE_TO_TABLE("YUYV", MEDIASUBTYPE_YUYV);
		ADD_MEDIA_TYPE_TO_TABLE("Y411", MEDIASUBTYPE_Y411);
		ADD_MEDIA_TYPE_TO_TABLE("Y211", MEDIASUBTYPE_Y211);
		ADD_MEDIA_TYPE_TO_TABLE("YUY2", MEDIASUBTYPE_YUY2);
		ADD_MEDIA_TYPE_TO_TABLE("UYVY", MEDIASUBTYPE_UYVY);

		ADD_MEDIA_TYPE_TO_TABLE("I420", MY_MEDIASUBTYPE_I420);
		ADD_MEDIA_TYPE_TO_TABLE("Y41P", MEDIASUBTYPE_Y41P);
		ADD_MEDIA_TYPE_TO_TABLE("YV12", MEDIASUBTYPE_YV12);
		ADD_MEDIA_TYPE_TO_TABLE("IYUV", MEDIASUBTYPE_IYUV);
		ADD_MEDIA_TYPE_TO_TABLE("YVU9", MEDIASUBTYPE_YVU9);

		ADD_MEDIA_TYPE_TO_TABLE("MJPG", MEDIASUBTYPE_MJPG);
	}


	DirectShowCapture::~DirectShowCapture(void)
	{
		CleanUp();
	}


	void DirectShowCapture::CreateDirectShowGraph()
	{
		//CoInitialize(NULL);
		/* Create directshow filter graph */
		if (SUCCEEDED(CoCreateInstance(CLSID_FilterGraph, 0, CLSCTX_INPROC,
			(REFIID)IID_IFilterGraph, (void **)&graph_)))
		{
			/* Create directshow capture graph builder if available */
			if (SUCCEEDED(CoCreateInstance(CLSID_CaptureGraphBuilder2, 0,
				CLSCTX_INPROC, (REFIID)IID_ICaptureGraphBuilder2,
				(void **)&capture_graph_builder_)))
			{
				capture_graph_builder_->SetFiltergraph((IGraphBuilder *)graph_);
			}

			graph_->QueryInterface(IID_IMediaControl, (void **)&media_control_);
		}
		//CoUninitialize();
	}

	void DirectShowCapture::DeleteDirectShowGraph()
	{
		msg_Dbg("DeleteDirectShowGraph: Removing capture filter.");
		if (video_stream_.capture_filter != NULL)
		{
			graph_->RemoveFilter((IBaseFilter *)video_stream_.capture_filter);
			video_stream_.capture_filter->Release();
			video_stream_.capture_filter = NULL;
		}
		if (audio_stream_.capture_filter != NULL)
		{
			graph_->RemoveFilter((IBaseFilter *)audio_stream_.capture_filter);
			audio_stream_.capture_filter->Release();
			audio_stream_.capture_filter = NULL;
		}
		msg_Dbg("DeleteDirectShowGraph: Removing device filter.");
		if (video_stream_.device_filter != NULL)
		{
			graph_->RemoveFilter((IBaseFilter *)video_stream_.device_filter);
			video_stream_.device_filter = NULL;
		}
		if (audio_stream_.device_filter != NULL)
		{
			graph_->RemoveFilter((IBaseFilter *)audio_stream_.device_filter);
			audio_stream_.device_filter = NULL;
		}

		if (media_control_)
		{
			media_control_->Release();
			media_control_ = NULL;
		}
		if (capture_graph_builder_)
		{
			capture_graph_builder_->Release();
			capture_graph_builder_ = NULL;
		}

		if (graph_)
		{
			__try
			{
				if (!0/*b_destory_*/)//临时解决方案，软件退出时不release
				{
					graph_->Release();
				}
			}
			__except (EXCEPTION_EXECUTE_HANDLER)
			{

			}
			graph_ = NULL;
		}
	}

	void DirectShowCapture::Initialize()
	{
		CreateDirectShowGraph();
	}

	void DirectShowCapture::CleanUp()
	{
		DeleteDirectShowGraph();
	}

	bool DirectShowCapture::OpenVideoDevice(const std::wstring &video_device, SampleCallback callback_function,
		VideoOutputType output_type, void *user_data, const CaptureProperty *property)
	{
		return OpenDevice(video_device, VIDEO_DEVICE, callback_function, &output_type, user_data, property);
	}

	bool DirectShowCapture::OpenAudioDevice(const std::wstring &audio_device, SampleCallback callback_function, AudioOutputType output_type, void *user_data)
	{
		return OpenDevice(audio_device, AUDIO_DEVICE, callback_function, &output_type, user_data);
	}

	bool DirectShowCapture::OpenDevice(const std::wstring &device_name, DeviceType device_type, SampleCallback callback_function,
		const void *output_type, void *user_data, const CaptureProperty *property)
	{
		DirectShowStream *stream = NULL;
		HRESULT hr = 0;
		IBaseFilter *device_filter = NULL;
		CaptureFilter *capture_filter = NULL;
		bool *device_opened = NULL;
		AM_MEDIA_TYPE *mt = (AM_MEDIA_TYPE *)CoTaskMemAlloc(sizeof(AM_MEDIA_TYPE));
		ZeroMemory(mt, sizeof(AM_MEDIA_TYPE));
		static const GUID MY_MEDIASUBTYPE_I420 = { 0x30323449, 0x0000, 0x0010, { 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71 } };
		if (device_type == VIDEO_DEVICE)
		{
			stream = &video_stream_;
			mt->majortype = MEDIATYPE_Video;
			if ((*reinterpret_cast<const VideoOutputType *>(output_type)) == VIDEO_DATA_RGB24)
			{
				mt->subtype = MEDIASUBTYPE_RGB24;
			}
			else if ((*reinterpret_cast<const VideoOutputType *>(output_type)) == VIDEO_DATA_YUV420P)
			{
				mt->subtype = MY_MEDIASUBTYPE_I420;
			}
			device_opened = &video_device_opened_;
		}
		else if (device_type = AUDIO_DEVICE)
		{
			stream = &audio_stream_;
			mt->majortype = MEDIATYPE_Audio;
			mt->subtype = MEDIASUBTYPE_PCM;
			device_opened = &audio_device_opened_;
		}
		else
		{
			return false;
		}
		device_filter = FindCaptureDevice(device_name, device_type);
		if (device_filter)
		{
			msg_Dbg("using device: %s", device_name.c_str());
			if (device_type == VIDEO_DEVICE)
			{
				IntelligentConfigVideoDevice(device_filter, property);
			}
		}
		else
		{
			msg_Dbg("can't open device: %s", device_name.c_str());
			return false;
		}

		if (device_type == AUDIO_DEVICE)
		{
			CComPtr<IPin> pCapturePin;
			bool bConnected;
			FindPinByIndex(device_filter, PINDIR_OUTPUT, &pCapturePin, 0, bConnected);
			if (pCapturePin)
			{
				CComPtr<IAMBufferNegotiation> pNeg;
				pCapturePin->QueryInterface(IID_IAMBufferNegotiation, (void**)&pNeg);
				if (pNeg)
				{
					ALLOCATOR_PROPERTIES prop = { 0 };
					prop.cbBuffer = 4096;
					prop.cBuffers = 4;
					prop.cbAlign = 1;
					hr = pNeg->SuggestAllocatorProperties(&prop);
				}

			}
		}
		capture_filter = new CaptureFilter(mt, 1, callback_function, user_data);
		graph_->AddFilter(capture_filter, 0);
		graph_->AddFilter(device_filter, 0);

		if (ConnectFilters(device_filter, capture_filter))
		{
			msg_Dbg("filters connected successfully !");
			stream->mt = capture_filter->CustomGetPin()->CustomGetMediaType();

			if (stream->mt.majortype == MEDIATYPE_Video)
			{
				stream->header.video = *(VIDEOINFOHEADER *)stream->mt.pbFormat;
				msg_Dbg("MEDIATYPE_Video");
			}
			else if (stream->mt.majortype == MEDIATYPE_Audio)
			{
				stream->header.audio = *(WAVEFORMATEX *)stream->mt.pbFormat;
				msg_Dbg("MEDIATYPE_Audio");
			}
			else if (stream->mt.majortype == MEDIATYPE_Stream)
			{
				msg_Dbg("MEDIATYPE_Stream");
			}
			else
			{
				msg_Dbg("unknown stream majortype");
				goto fail;
			}

			stream->device_filter = device_filter;
			stream->capture_filter = capture_filter;
			*device_opened = true;
			return true;
		}
	fail:
		msg_Dbg("OpenDevice: Removing filters");
		if (device_filter != NULL)
		{
			graph_->RemoveFilter(device_filter);
		}
		if (capture_filter != NULL)
		{
			graph_->RemoveFilter(capture_filter);
		}

		device_filter = NULL;
		delete capture_filter;
		capture_filter = NULL;

		return false;
	}

	IBaseFilter *DirectShowCapture::FindCaptureDevice(const std::wstring &device_name, DeviceType device_type)
	{
		IBaseFilter *base_filter = NULL;
		IMoniker *moniker = NULL;
		ULONG fetched = 0;
		HRESULT hr = 0;
		std::vector<std::wstring> *device_list = NULL;

		ICreateDevEnum *device_enum = NULL;

		hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC,
			IID_ICreateDevEnum, (void **)&device_enum);
		if (FAILED(hr))
		{
			msg_Dbg("failed to create the device enumerator (0x%lx)", hr);
			return NULL;
		}

		IEnumMoniker *class_enum = NULL;
		if (device_type == VIDEO_DEVICE)
		{
			hr = device_enum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory,
				&class_enum, 0);
			device_list = &video_device_list_;
		}
		else if (device_type == AUDIO_DEVICE)
		{
			hr = device_enum->CreateClassEnumerator(CLSID_AudioInputDeviceCategory,
				&class_enum, 0);
			device_list = &audio_device_list_;
		}
		else
		{
			msg_Dbg("device type not supported.");
			return NULL;
		}

		device_enum->Release();
		if (FAILED(hr))
		{
			msg_Dbg("failed to create the class enumerator (0x%lx)", hr);
			return NULL;
		}

		if (class_enum == NULL)
		{
			msg_Dbg("no capture device was detected");
			return NULL;
		}

		device_list->clear();
		while (class_enum->Next(1, &moniker, &fetched) == S_OK)
		{
			IPropertyBag *property_bag;
			hr = moniker->BindToStorage(0, 0, IID_IPropertyBag,
				(void **)&property_bag);
			if (SUCCEEDED(hr))
			{
				if (device_type == VIDEO_DEVICE)
				{
					VARIANT var_device_path;
					VariantInit(&var_device_path);
					hr = property_bag->Read(L"DevicePath", &var_device_path, NULL);
					if (SUCCEEDED(hr))
					{
						std::wstring current_device_path = var_device_path.bstrVal;
						VariantClear(&var_device_path);
						//if (!boost::algorithm::contains(boost::algorithm::to_lower_copy(current_device_path), L"usb#"))
						//{
						//    property_bag->Release();
						//    continue;
						//}
					}
					else
					{
						property_bag->Release();
						continue;
					}
				}

				VARIANT var_friendly_name;
				VariantInit(&var_friendly_name);
				hr = property_bag->Read(L"FriendlyName", &var_friendly_name, NULL);

				if (SUCCEEDED(hr))
				{
					std::wstring current_device_name = var_friendly_name.bstrVal;
					VariantClear(&var_friendly_name);

					int dup = 0;
					std::vector<std::wstring>::const_iterator iter = device_list->begin();
					std::vector<std::wstring>::const_iterator end = device_list->end();
					while (iter != end)
					{
						if (0 == (*iter).compare(current_device_name))
						{
							current_device_name += L"#";
							current_device_name += ((++dup) + L'0');
							iter = device_list->begin();
						}
						else
						{
							++iter;
						}
					}
					device_list->push_back(current_device_name);

					if (device_name.substr(0, DIRECT_SHOW_STRING_INTERRUPT - 1) == current_device_name.substr(0, DIRECT_SHOW_STRING_INTERRUPT - 1))
					{
						msg_Dbg("asked for %s, binding to %s", device_name.c_str(), current_device_name.c_str());
						hr = moniker->BindToObject(NULL, 0, IID_IBaseFilter,
							(void **)&base_filter);
						if (FAILED(hr))
						{
							msg_Dbg("couldn't bind moniker to filter object (0x%lx)", hr);
							moniker->Release();
							class_enum->Release();
							return NULL;
						}
						moniker->Release();
						property_bag->Release();
						class_enum->Release();

						return base_filter;
					}
				}
			}
			property_bag->Release();
			moniker->Release();
		}
		class_enum->Release();
		sort(device_list->begin(), device_list->end());
		return NULL;
	}

	void DirectShowCapture::GetVideoCaptureDeviceList(std::vector<std::wstring> &device_list)
	{
		FindCaptureDevice(L"", VIDEO_DEVICE);
		device_list = video_device_list_;
	}

	void DirectShowCapture::GetAudioCaptureDeviceList(std::vector<std::wstring> &device_list)
	{
		FindCaptureDevice(L"", AUDIO_DEVICE);
		device_list = audio_device_list_;
	}

	/*bool DirectShowCapture::SetOutputFormat( IBaseFilter *filter, AM_MEDIA_TYPE *media_type)
	{
	IEnumPins *enum_pins = NULL;
	IPin *output_pin = NULL;
	IEnumMediaTypes *enum_media_type = NULL;
	bool result = false;
	if (filter == NULL)
	{
	return false;
	}
	if (FAILED(filter->EnumPins( &enum_pins )) )
	{
	msg_Dbg( "EnumDeviceCaps failed: no pin enumeration !");
	return false;
	}
	while (S_OK == enum_pins->Next(1, &output_pin, NULL))
	{
	PIN_INFO info;
	if( S_OK == output_pin->QueryPinInfo( &info ) )
	{
	if( info.pFilter )
	{
	info.pFilter->Release();
	}
	if( info.dir == PINDIR_INPUT )
	{
	output_pin->Release();
	continue;
	}
	}
	IAMStreamConfig *stream_config = NULL;
	if( SUCCEEDED(output_pin->QueryInterface( IID_IAMStreamConfig,
	(void **)&stream_config )) )
	{
	if( SUCCEEDED( stream_config->SetFormat(media_type) ) )
	{
	result = true;
	}
	stream_config->Release();
	}
	output_pin->Release();
	}
	enum_pins->Reset();
	return result;
	}*/

	bool DirectShowCapture::ConnectFilters(IBaseFilter *base_filter, CaptureFilter *capture_filter)
	{
		CapturePin *input_pin = capture_filter->CustomGetPin();
		AM_MEDIA_TYPE media_type = input_pin->CustomGetMediaType();

		if (capture_graph_builder_)
		{
			if (FAILED(capture_graph_builder_->RenderStream(&PIN_CATEGORY_CAPTURE, &media_type.majortype,
				base_filter, 0, (IBaseFilter *)capture_filter)))
			{
				return false;
			}
			return true;
		}
		else
		{
			IEnumPins *enum_pins = NULL;
			IPin *pin = NULL;

			if (S_OK != base_filter->EnumPins(&enum_pins))
			{
				return false;
			}
			while (S_OK == enum_pins->Next(1, &pin, NULL))
			{
				PIN_DIRECTION pin_dir;
				pin->QueryDirection(&pin_dir);

				if (pin_dir == PINDIR_OUTPUT && graph_->ConnectDirect(pin, (IPin *)input_pin, 0) == S_OK)
				{
					pin->Release();
					enum_pins->Release();
					return true;
				}
				pin->Release();
			}
			enum_pins->Release();
			return false;
		}
	}

	bool DirectShowCapture::StartCapture()
	{
		if (media_control_ != NULL)
		{
			FILTER_STATE fs;
			if (SUCCEEDED(media_control_->GetState(INFINITE, (OAFilterState*)&fs)))
			{
				if (fs == State_Running)
				{
					return true;
				}
			}
			else
			{
				return false;
			}
			if (SUCCEEDED(media_control_->Run()))
			{
				return true;
			}
		}
		return false;
	}

	bool DirectShowCapture::StopCapture()
	{
		if (media_control_ != NULL)
		{
			FILTER_STATE fs;
			if (SUCCEEDED(media_control_->GetState(INFINITE, (OAFilterState*)&fs)))
			{
				if (fs == State_Stopped)
				{
					return true;
				}
			}
			else
			{
				return false;
			}
			if (SUCCEEDED(media_control_->Stop()))
			{
				return true;
			}
		}
		return false;
	}

	bool DirectShowCapture::GetVideoProperity(uint32_t &width, uint32_t &height, uint32_t &frame_rate, uint32_t &frame_rate_base)
	{
		if (!video_device_opened_)
		{
			return false;
		}
		width = video_stream_.header.video.bmiHeader.biWidth;
		height = video_stream_.header.video.bmiHeader.biHeight;
		frame_rate = 10000000;
		frame_rate_base = static_cast<uint32_t>(video_stream_.header.video.AvgTimePerFrame);
		return true;
	}

	bool DirectShowCapture::GetAudioProperity(uint16_t &channels, uint32_t &sample_rate, uint16_t &bits_per_sample)
	{
		if (!audio_device_opened_)
		{
			return false;
		}
		channels = audio_stream_.header.audio.nChannels;
		sample_rate = audio_stream_.header.audio.nSamplesPerSec;
		bits_per_sample = audio_stream_.header.audio.wBitsPerSample;
		return true;
	}

	bool DirectShowCapture::CloseVideoDevice()
	{
		return CloseDevice(VIDEO_DEVICE);
	}

	bool DirectShowCapture::CloseAudioDevice()
	{
		return CloseDevice(AUDIO_DEVICE);
	}

	bool DirectShowCapture::CloseDevice(DeviceType device_type)
	{
		DirectShowStream *stream = NULL;
		HRESULT hr = 0;
		IBaseFilter *device_filter = NULL;
		CaptureFilter *capture_filter = NULL;
		bool *device_opened = NULL;
		if (device_type == VIDEO_DEVICE)
		{
			stream = &video_stream_;
			device_opened = &video_device_opened_;
		}
		else if (device_type = AUDIO_DEVICE)
		{
			stream = &audio_stream_;
			device_opened = &audio_device_opened_;
		}
		else
		{
			return false;
		}
		if (!StopCapture())
		{
			return false;
		}

		if (stream != NULL && stream->device_filter != NULL)
		{
			graph_->RemoveFilter(stream->device_filter);
		}
		if (stream != NULL && stream->capture_filter != NULL)
		{
			graph_->RemoveFilter(stream->capture_filter);
			stream->capture_filter->Release();
		}

		stream->device_filter = NULL;
		stream->capture_filter = NULL;
		return true;
	}

	HRESULT DirectShowCapture::FindPinByIndex(IBaseFilter* pFilter, PIN_DIRECTION dir, IPin** ppPin, int nIndex, bool& bConected)
	{
		HRESULT hr = S_OK;
		CComPtr<IEnumPins> spPins;
		hr = pFilter->EnumPins(&spPins);
		int nCount = 0;
		if (SUCCEEDED(hr))
		{
			ULONG ulFetched = 0;
			CComPtr<IPin> spPin;
			while (spPin.Release(), spPins->Next(1, &spPin, &ulFetched) == S_OK)
			{
				PIN_DIRECTION eTmpDir;
				hr = spPin->QueryDirection(&eTmpDir);
				if (SUCCEEDED(hr) && (eTmpDir == dir) && nCount++ == nIndex)
				{
					bConected = IsPinConnected(spPin);
					*ppPin = spPin;
					(*ppPin)->AddRef();
					break;
				}
			}
		}

		return hr;
	}

	bool DirectShowCapture::IsPinConnected(IPin* pPin)
	{
		CComPtr<IPin> spPeerPin;
		pPin->ConnectedTo(&spPeerPin);
		return spPeerPin != NULL;
	}

	bool DirectShowCapture::ShowVideoPropertyPage()
	{
		return ShowPropertyPage(VIDEO_DEVICE);
	}

	bool DirectShowCapture::ShowAudioPropertyPage()
	{
		return ShowPropertyPage(AUDIO_DEVICE);
	}

	bool DirectShowCapture::ShowPropertyPage(DeviceType device_type)
	{
		DirectShowStream *stream = NULL;
		bool *device_opened = NULL;
		if (device_type == VIDEO_DEVICE)
		{
			stream = &video_stream_;
			device_opened = &video_device_opened_;
		}
		else if (device_type = AUDIO_DEVICE)
		{
			stream = &audio_stream_;
			device_opened = &audio_device_opened_;
		}
		else
		{
			return false;
		}
		ISpecifyPropertyPages *spec = NULL;
		CAUUID cauuid;

		HRESULT hr = stream->device_filter->QueryInterface(IID_ISpecifyPropertyPages,
			(void **)&spec);
		if (FAILED(hr))
		{
			return false;
		}

		if (SUCCEEDED(spec->GetPages(&cauuid)))
		{
			if (cauuid.cElems > 0)
			{
				HWND hwnd_desktop = ::GetDesktopWindow();

				OleCreatePropertyFrame(hwnd_desktop, 30, 30, NULL, 1, reinterpret_cast<LPUNKNOWN *> (&stream->device_filter),
					cauuid.cElems, cauuid.pElems, 0, 0, NULL);

				CoTaskMemFree(cauuid.pElems);
			}
			spec->Release();
		}
		return true;
	}

	void DirectShowCapture::SetOutputVideoSize(uint32_t width, uint32_t height)
	{
		output_video_width_ = width;
		output_video_height_ = height;
	}

	void DirectShowCapture::SuggestAvgTimePerFrame(uint32_t avg_time_per_frame)
	{
		suggested_avg_time_per_frame_ = avg_time_per_frame;
	}

	bool DirectShowCapture::IntelligentConfigVideoDevice(IBaseFilter *device_filter, const CaptureProperty *property)
	{
		IEnumPins *enum_pins = NULL;
		IPin *output_pin = NULL;
		IEnumMediaTypes *enum_mt = NULL;

		if (device_filter == NULL)
		{
			return false;
		}

		if (FAILED(device_filter->EnumPins(&enum_pins)))
		{
			msg_Dbg("EnumDeviceCaps failed: no pin enumeration !");
			return false;
		}

		while (S_OK == enum_pins->Next(1, &output_pin, NULL))
		{
			PIN_INFO info;

			if (S_OK == output_pin->QueryPinInfo(&info))
			{
				if (info.pFilter)
				{
					info.pFilter->Release();
				}
			}
			output_pin->Release();
		}
		enum_pins->Reset();

		while (enum_pins->Next(1, &output_pin, NULL) == S_OK)
		{
			uint32_t max_priority = 0;
			PIN_INFO info = { 0 };

			if (S_OK == output_pin->QueryPinInfo(&info))
			{
				if (info.pFilter)
				{
					info.pFilter->Release();
				}
				if (info.dir == PINDIR_INPUT)
				{
					output_pin->Release();
					continue;
				}
				msg_Dbg("EnumDeviceCaps: trying pin %S", info.achName);
			}

			AM_MEDIA_TYPE *media_type = NULL;
			IAMStreamConfig *scream_config = NULL;

			if (SUCCEEDED(output_pin->QueryInterface(IID_IAMStreamConfig, (void **)&scream_config)))
			{
				int piCount = 0;
				int piSize = 0;
				if (SUCCEEDED(scream_config->GetNumberOfCapabilities(&piCount, &piSize)))
				{
					BYTE *pSCC = (BYTE *)CoTaskMemAlloc(piSize);
					if (NULL != pSCC)
					{

						for (int i = 0; i < piCount; ++i)
						{
							if (SUCCEEDED(scream_config->GetStreamCaps(i, &media_type, pSCC)))
							{
								if (MEDIATYPE_Video == media_type->majortype && FORMAT_VideoInfo == media_type->formattype)
								{
									VIDEO_STREAM_CONFIG_CAPS *pVSCC = reinterpret_cast<VIDEO_STREAM_CONFIG_CAPS*>(pSCC);
									VIDEOINFOHEADER *pVih = reinterpret_cast<VIDEOINFOHEADER*>(media_type->pbFormat);

									if (suggested_avg_time_per_frame_ != 0
										&& pVSCC->MinFrameInterval <= suggested_avg_time_per_frame_
										&& suggested_avg_time_per_frame_ <= pVSCC->MaxFrameInterval)
									{
										pVih->AvgTimePerFrame = suggested_avg_time_per_frame_;
									}

									uint32_t current_priority = GetMediaFormatPriority(media_type, property);

									if (current_priority > max_priority)
									{
										if (SUCCEEDED(scream_config->SetFormat(media_type)))
										{
											max_priority = current_priority;
										}
									}
								}
								FreeMediaType(*media_type);
								CoTaskMemFree((PVOID)media_type);
							}
						}
						CoTaskMemFree((LPVOID)pSCC);
					}
				}
				scream_config->Release();
			}

			if (FAILED(output_pin->EnumMediaTypes(&enum_mt)))
			{
				output_pin->Release();
				continue;
			}

			enum_mt->Release();
			output_pin->Release();
		}

		enum_pins->Release();
		return true;
	}

	uint32_t DirectShowCapture::GetMediaFormatPriority(AM_MEDIA_TYPE *media_type, const CaptureProperty *property)
	{
		static const GUID MY_MEDIASUBTYPE_I420 = { 0x30323449, 0x0000, 0x0010, { 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71 } };
		uint32_t priority = 0;
		if (!media_type)
		{
			return 0;
		}


		if (media_type->majortype == MEDIATYPE_Video)
		{
			VIDEOINFOHEADER *video_info = reinterpret_cast<VIDEOINFOHEADER*>(media_type->pbFormat);
			LONG media_width = video_info->bmiHeader.biWidth;
			LONG media_height = video_info->bmiHeader.biHeight;

			if ((property != NULL) && (property->image_type == MediaTypeToBasicString(media_type->subtype))
				&& (media_width == property->width) && (media_height == property->height))
			{
				priority = 0xffffffff;
			}
			else
			{
				if (output_video_width_ != 0 && output_video_height_ != 0)
				{

					double b = 1.0 * video_info->bmiHeader.biWidth / video_info->bmiHeader.biHeight;
					//if (media_width == 1280 && media_height == 720)
					//{
					//    return 0xFFFFFFFF;
					//}
					DOUBLE width_rate = (media_width > static_cast<LONG>(output_video_width_))
						? (1.0 * output_video_width_ / media_width) : (1.0 * media_width / output_video_width_);

					DOUBLE height_rate = (media_height > static_cast<LONG>(output_video_height_))
						? (1.0 * output_video_height_ / media_height) : (1.0 * media_height / output_video_height_);

					uint32_t global_rate = static_cast<uint32_t> (width_rate * height_rate * (1 << 14));

					priority = priority | global_rate;

					//if (media_width * suggested_video_height_ != media_height * suggested_video_width_)
					if (fabs(1.0 * media_width / media_height - 1.0 * output_video_width_ / output_video_height_) > 0.25)
						//if ( fabs( 1.0 * media_width / media_height - suggested_aspect_ratio_ ) > 0.25)
					{
						if (
							((output_video_width_ * media_height > output_video_height_ * media_width) && (media_width >= output_video_width_))
							||
							((output_video_width_ * media_height < output_video_height_ * media_width) && (media_height >= output_video_height_))
							)
						{
							priority = priority | (1 << 15);
						}
					}
					else
					{
						priority = priority | (1 << 16);
					}
					if (media_width >= static_cast<LONG>(output_video_width_) && media_height >= static_cast<LONG>(output_video_height_))
					{
						priority = priority | (1 << 17);
					}
				}
				if (media_type->subtype == MEDIASUBTYPE_RGB24
					|| media_type->subtype == MEDIASUBTYPE_RGB32
					|| media_type->subtype == MEDIASUBTYPE_ARGB32)
				{
					priority = priority | (1 << 21);
				}
				else if (media_type->subtype == MEDIASUBTYPE_YVYU
					|| media_type->subtype == MEDIASUBTYPE_YUYV
					|| media_type->subtype == MEDIASUBTYPE_Y411
					|| media_type->subtype == MEDIASUBTYPE_Y211
					|| media_type->subtype == MEDIASUBTYPE_YUY2
					|| media_type->subtype == MEDIASUBTYPE_UYVY)
				{
					priority = priority | (1 << 20);
				}
				else if (media_type->subtype == MY_MEDIASUBTYPE_I420
					|| media_type->subtype == MEDIASUBTYPE_Y41P
					|| media_type->subtype == MEDIASUBTYPE_YV12
					|| media_type->subtype == MEDIASUBTYPE_IYUV
					|| media_type->subtype == MEDIASUBTYPE_YVU9)
				{
					priority = priority | (1 << 19);
				}
				else
				{
					priority = priority | (1 << 18);
				}
			}
		}
		return priority;
	}

	void DirectShowCapture::SuggestAspectRatio(AspectRatio aspect_ratio)
	{
		if (aspect_ratio == ASPECT_RATIO_4_3)
		{
			suggested_aspect_ratio_ = 4.0 / 3;

		}
		else if (aspect_ratio == ASPECT_RATIO_16_9)
		{
			suggested_aspect_ratio_ = 16.0 / 9;
		}
	}

	bool DirectShowCapture::GetVideoPeripheralCaptureDeviceProperty(const std::wstring &video_device, std::vector<CaptureProperty> &supported_property,
		CaptureProperty &default_property)
	{
		IBaseFilter *device_filter = FindCaptureDevice(video_device, VIDEO_DEVICE);

		IEnumPins *enum_pins = NULL;
		IPin *output_pin = NULL;
		IEnumMediaTypes *enum_mt = NULL;

		uint32_t max_priority = 0;

		if (device_filter == NULL)
		{
			return false;
		}

		if (FAILED(device_filter->EnumPins(&enum_pins)))
		{
			msg_Dbg("EnumDeviceCaps failed: no pin enumeration !");
			return false;
		}

		while (S_OK == enum_pins->Next(1, &output_pin, NULL))
		{
			PIN_INFO info;

			if (S_OK == output_pin->QueryPinInfo(&info))
			{
				if (info.pFilter)
				{
					info.pFilter->Release();
				}
			}
			output_pin->Release();
		}
		enum_pins->Reset();

		supported_property.clear();

		bool has_got_output_format = false;
		while ((enum_pins->Next(1, &output_pin, NULL) == S_OK) && (!has_got_output_format))
		{
			uint32_t max_priority = 0;
			PIN_INFO info = { 0 };

			if (S_OK == output_pin->QueryPinInfo(&info))
			{
				if (info.pFilter)
				{
					info.pFilter->Release();
				}
				if (info.dir == PINDIR_INPUT)
				{
					output_pin->Release();
					continue;
				}
			}

			AM_MEDIA_TYPE *media_type = NULL;
			IAMStreamConfig *scream_config = NULL;

			if (SUCCEEDED(output_pin->QueryInterface(IID_IAMStreamConfig, (void **)&scream_config)))
			{
				int piCount = 0;
				int piSize = 0;
				if (SUCCEEDED(scream_config->GetNumberOfCapabilities(&piCount, &piSize)))
				{
					BYTE *pSCC = (BYTE *)CoTaskMemAlloc(piSize);
					if (NULL != pSCC)
					{

						for (int i = 0; i < piCount; ++i)
						{
							if (SUCCEEDED(scream_config->GetStreamCaps(i, &media_type, pSCC)))
							{
								if (MEDIATYPE_Video == media_type->majortype && FORMAT_VideoInfo == media_type->formattype)
								{
									VIDEO_STREAM_CONFIG_CAPS *pVSCC = reinterpret_cast<VIDEO_STREAM_CONFIG_CAPS*>(pSCC);
									VIDEOINFOHEADER *pVih = reinterpret_cast<VIDEOINFOHEADER*>(media_type->pbFormat);

									CaptureProperty property;
									property.width = pVih->bmiHeader.biWidth;
									property.height = pVih->bmiHeader.biHeight;
									property.image_type = MediaTypeToBasicString(media_type->subtype);
									supported_property.push_back(property);

									uint32_t current_priority = GetMediaFormatPriority(media_type, NULL);
									if (current_priority > max_priority)
									{
										max_priority = current_priority;
										default_property = property;
									}

								}
								FreeMediaType(*media_type);
								CoTaskMemFree((PVOID)media_type);
							}
						}
						has_got_output_format = true;

						CoTaskMemFree((LPVOID)pSCC);
					}
				}
				scream_config->Release();
			}

			if (FAILED(output_pin->EnumMediaTypes(&enum_mt)))
			{
				output_pin->Release();
				continue;
			}
			enum_mt->Release();
			output_pin->Release();
		}
		if (enum_pins != NULL)
		{
			enum_pins->Release();
			enum_pins = NULL;
		}

		if (device_filter != NULL)
		{
			device_filter->Release();
			device_filter = NULL;
		}
		return true;
	}

	std::string DirectShowCapture::MediaTypeToBasicString(const GUID &media_type)
	{
		for (std::map<std::string, GUID>::const_iterator iter = video_type_table_.begin();
			iter != video_type_table_.end(); ++iter)
		{
			if (iter->second == media_type)
			{
				return iter->first;
			}
		}
		return "UNKNOWN";
	}

	GUID DirectShowCapture::BasicStringToGUID(const std::string &media_type)
	{
		std::map<std::string, GUID>::const_iterator iter = video_type_table_.find(media_type);
		if (iter != video_type_table_.end())
		{
			return iter->second;
		}
		else
		{
			return GUID_NULL;
		}
	}
}
#endif