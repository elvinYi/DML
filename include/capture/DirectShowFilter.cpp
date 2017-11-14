#ifdef WIN32
#include <dshow.h>
#include <vfwmsgs.h>
#include "DirectShowFilter.h"

#include <initguid.h>

#define FILTER_NAME  L"Capture Filter"
#define PIN_NAME     L"Capture"
#define msg_Dbg

namespace MediaPlugin {
	void WINAPI FreeMediaType(AM_MEDIA_TYPE& mt)
	{
		if (mt.cbFormat != 0)
		{
			CoTaskMemFree((PVOID)mt.pbFormat);
			mt.cbFormat = 0;
			mt.pbFormat = NULL;
		}
		if (mt.pUnk != NULL)
		{
			mt.pUnk->Release();
			mt.pUnk = NULL;
		}
	}

	HRESULT WINAPI CopyMediaType(AM_MEDIA_TYPE *pmtTarget,
		const AM_MEDIA_TYPE *pmtSource)
	{
		*pmtTarget = *pmtSource;

		if (!pmtSource || !pmtTarget) return S_FALSE;

		if (pmtSource->cbFormat && pmtSource->pbFormat)
		{
			pmtTarget->pbFormat = (PBYTE)CoTaskMemAlloc(pmtSource->cbFormat);
			if (pmtTarget->pbFormat == NULL)
			{
				pmtTarget->cbFormat = 0;
				return E_OUTOFMEMORY;
			}
			else
			{
				CopyMemory((PVOID)pmtTarget->pbFormat, (PVOID)pmtSource->pbFormat,
					pmtTarget->cbFormat);
			}
		}
		if (pmtTarget->pUnk != NULL)
		{
			pmtTarget->pUnk->AddRef();
		}

		return S_OK;
	}

	/****************************************************************************
	 * Implementation of our dummy directshow filter pin class
	 ****************************************************************************/

	CapturePin::CapturePin(CaptureFilter *_p_filter,
		AM_MEDIA_TYPE *mt, size_t mt_count)
		: p_filter(_p_filter),
		p_connected_pin(NULL), media_types(mt), media_type_count(mt_count),
		i_ref(1)
	{
		cx_media_type.majortype = mt[0].majortype;
		cx_media_type.subtype = GUID_NULL;
		cx_media_type.pbFormat = NULL;
		cx_media_type.cbFormat = 0;
		cx_media_type.pUnk = NULL;
	}

	CapturePin::~CapturePin()
	{
		for (size_t c = 0; c < media_type_count; c++)
		{
			FreeMediaType(media_types[c]);
			CoTaskMemFree((LPVOID)&media_types[c]);
		}
		FreeMediaType(cx_media_type);
	}

	AM_MEDIA_TYPE &CapturePin::CustomGetMediaType()
	{
		return cx_media_type;
	}

	/* IUnknown methods */
	STDMETHODIMP CapturePin::QueryInterface(REFIID riid, void **ppv)
	{
		if (riid == IID_IUnknown ||
			riid == IID_IPin)
		{
			AddRef();
			*ppv = (IPin *)this;
			return NOERROR;
		}
		if (riid == IID_IMemInputPin)
		{
			AddRef();
			*ppv = (IMemInputPin *)this;
			return NOERROR;
		}
		else
		{
			*ppv = NULL;
			return E_NOINTERFACE;
		}
	}

	STDMETHODIMP_(ULONG) CapturePin::AddRef()
	{
		return i_ref++;
	}

	STDMETHODIMP_(ULONG) CapturePin::Release()
	{
		if (!InterlockedDecrement(&i_ref))
		{
			delete this;
		}
		return 0;
	}

	/* IPin methods */
	STDMETHODIMP CapturePin::Connect(IPin *,
		const AM_MEDIA_TYPE *pmt)
	{
		if (State_Running == p_filter->state)
		{
			msg_Dbg("CapturePin::Connect [not stopped]");
			return VFW_E_NOT_STOPPED;
		}

		if (p_connected_pin)
		{
			msg_Dbg("CapturePin::Connect [already connected]");
			return VFW_E_ALREADY_CONNECTED;
		}

		if (!pmt) return S_OK;

		if (GUID_NULL != pmt->majortype &&
			media_types[0].majortype != pmt->majortype)
		{
			msg_Dbg("CapturePin::Connect [media major type mismatch]");
			return S_FALSE;
		}

		if (GUID_NULL != pmt->subtype /*&& !GetFourCCFromMediaType(*pmt)*/)
		{
			msg_Dbg("CapturePin::Connect [media subtype type not supported]");
			return S_FALSE;
		}

		if (pmt->pbFormat && pmt->majortype == MEDIATYPE_Video)
		{
			if (!((VIDEOINFOHEADER *)pmt->pbFormat)->bmiHeader.biHeight ||
				!((VIDEOINFOHEADER *)pmt->pbFormat)->bmiHeader.biWidth)
			{
				msg_Dbg("CapturePin::Connect[video width/height == 0 ]");
				return S_FALSE;
			}
		}
		msg_Dbg("CapturePin::Connect [OK]");
		return S_OK;
	}

	STDMETHODIMP CapturePin::ReceiveConnection(IPin * pConnector,
		const AM_MEDIA_TYPE *pmt)
	{
		if (State_Stopped != p_filter->state)
		{
			msg_Dbg("CapturePin::ReceiveConnection [not stopped]");
			return VFW_E_NOT_STOPPED;
		}

		if (!pConnector || !pmt)
		{
			msg_Dbg("CapturePin::ReceiveConnection [null pointer]");
			return E_POINTER;
		}

		if (p_connected_pin)
		{
			msg_Dbg("CapturePin::ReceiveConnection [already connected]");
			return VFW_E_ALREADY_CONNECTED;
		}

		if (S_OK != QueryAccept(pmt))
		{
			msg_Dbg("CapturePin::ReceiveConnection [media type not accepted]");
			return VFW_E_TYPE_NOT_ACCEPTED;
		}

		msg_Dbg("CapturePin::ReceiveConnection [OK]");

		p_connected_pin = pConnector;
		p_connected_pin->AddRef();

		FreeMediaType(cx_media_type);
		return CopyMediaType(&cx_media_type, pmt);
	}

	STDMETHODIMP CapturePin::Disconnect()
	{
		if (!p_connected_pin)
		{
			msg_Dbg("CapturePin::Disconnect [not connected]");
			return S_FALSE;
		}

		msg_Dbg("CapturePin::Disconnect [OK]");

		/* samples_queue was already flushed in EndFlush() */

		p_connected_pin->Release();
		p_connected_pin = NULL;
		//FreeMediaType( cx_media_type );
		//cx_media_type.subtype = GUID_NULL;

		return S_OK;
	}
	STDMETHODIMP CapturePin::ConnectedTo(IPin **pPin)
	{
		if (!p_connected_pin)
		{
			msg_Dbg("CapturePin::ConnectedTo [not connected]");
			return VFW_E_NOT_CONNECTED;
		}

		p_connected_pin->AddRef();
		*pPin = p_connected_pin;

		msg_Dbg("CapturePin::ConnectedTo [OK]");

		return S_OK;
	}

	STDMETHODIMP CapturePin::ConnectionMediaType(AM_MEDIA_TYPE *pmt)
	{
		if (!p_connected_pin)
		{
			msg_Dbg("CapturePin::ConnectionMediaType [not connected]");
			return VFW_E_NOT_CONNECTED;
		}
		return CopyMediaType(pmt, &cx_media_type);
	}

	STDMETHODIMP CapturePin::QueryPinInfo(PIN_INFO * pInfo)
	{
		pInfo->pFilter = p_filter;
		if (p_filter) p_filter->AddRef();

		memcpy(pInfo->achName, PIN_NAME, sizeof(PIN_NAME));
		pInfo->dir = PINDIR_INPUT;

		return NOERROR;
	}

	STDMETHODIMP CapturePin::QueryDirection(PIN_DIRECTION * pPinDir)
	{
		*pPinDir = PINDIR_INPUT;
		return NOERROR;
	}

	STDMETHODIMP CapturePin::QueryId(LPWSTR * Id)
	{
		*Id = (LPWSTR)L"iQiyi Capture Pin";

		return S_OK;
	}
	STDMETHODIMP CapturePin::QueryAccept(const AM_MEDIA_TYPE *pmt)
	{
		if (State_Stopped != p_filter->state)
		{
			msg_Dbg("CapturePin::QueryAccept [not stopped]");
			return S_FALSE;
		}

		if (media_types[0].majortype != pmt->majortype)
		{
			msg_Dbg("CapturePin::QueryAccept [media type mismatch]");
			return S_FALSE;
		}

		/*if( ( pmt->majortype == MEDIATYPE_Video && pmt->subtype != MEDIASUBTYPE_RGB24) ||
			( pmt->majortype == MEDIATYPE_Audio && pmt->subtype != MEDIASUBTYPE_PCM) )*/
		if (pmt->subtype != media_types[0].subtype)
		{
			msg_Dbg("CapturePin::QueryAccept [media type not supported]");
			return S_FALSE;
		}

		if (pmt->majortype == MEDIATYPE_Video)
		{
			if ((pmt->pbFormat &&
				((((VIDEOINFOHEADER *)pmt->pbFormat)->bmiHeader.biHeight < 10) ||
				(((VIDEOINFOHEADER *)pmt->pbFormat)->bmiHeader.biWidth < 10)))
				|| (!pmt->pbFormat))
			{
				msg_Dbg("CapturePin::QueryAccept [video size wxh == 0]");
				return S_FALSE;
			}

			msg_Dbg("CapturePin::QueryAccept [OK] (width=%ld, height=%ld, fps=%f)",
				((VIDEOINFOHEADER *)pmt->pbFormat)->bmiHeader.biWidth,
				((VIDEOINFOHEADER *)pmt->pbFormat)->bmiHeader.biHeight,
				10000000.0f / ((float)((VIDEOINFOHEADER *)pmt->pbFormat)->AvgTimePerFrame));
		}
		else if (pmt->majortype == MEDIATYPE_Audio)
		{
			msg_Dbg("CapturePin::QueryAccept [OK] (channels=%d, samples/sec=%lu, bits/samples=%d)",
				((WAVEFORMATEX *)pmt->pbFormat)->nChannels,
				((WAVEFORMATEX *)pmt->pbFormat)->nSamplesPerSec,
				((WAVEFORMATEX *)pmt->pbFormat)->wBitsPerSample);
		}
		else
		{
			msg_Dbg("CapturePin::QueryAccept [OK]");
		}

		if (p_connected_pin)
		{
			FreeMediaType(cx_media_type);
			CopyMediaType(&cx_media_type, pmt);
		}

		return S_OK;
	}

	STDMETHODIMP CapturePin::EnumMediaTypes(IEnumMediaTypes **ppEnum)
	{
		*ppEnum = new CaptureEnumMediaTypes(this, NULL);
		if (*ppEnum == NULL)
		{
			return E_OUTOFMEMORY;
		}
		return NOERROR;
	}

	STDMETHODIMP CapturePin::QueryInternalConnections(IPin**, ULONG *)
	{
		return E_NOTIMPL;
	}

	STDMETHODIMP CapturePin::EndOfStream(void)
	{
		return S_OK;
	}

	STDMETHODIMP CapturePin::BeginFlush(void)
	{
		return S_OK;
	}

	STDMETHODIMP CapturePin::EndFlush(void)
	{
		return S_OK;
	}
	STDMETHODIMP CapturePin::NewSegment(REFERENCE_TIME, REFERENCE_TIME, double)
	{
		return S_OK;
	}

	/* IMemInputPin methods */
	STDMETHODIMP CapturePin::GetAllocator(IMemAllocator **)
	{
		return VFW_E_NO_ALLOCATOR;
	}
	STDMETHODIMP CapturePin::NotifyAllocator(IMemAllocator *, BOOL)
	{
		return S_OK;
	}
	STDMETHODIMP CapturePin::GetAllocatorRequirements(ALLOCATOR_PROPERTIES *)
	{
		return E_NOTIMPL;
	}

	STDMETHODIMP CapturePin::Receive(IMediaSample *pSample)
	{
		if (p_filter != NULL && p_filter->callback_function_ != NULL)
		{
			p_filter->callback_function_(p_filter->callback_user_data_, pSample, 0);
		}
		//pSample->Release();
		return S_OK;
	}

	STDMETHODIMP CapturePin::ReceiveMultiple(IMediaSample **pSamples,
		long nSamples,
		long *nSamplesProcessed)
	{
		HRESULT hr = S_OK;

		*nSamplesProcessed = 0;
		while (nSamples-- > 0)
		{
			hr = Receive(pSamples[*nSamplesProcessed]);
			if (hr != S_OK) break;
			(*nSamplesProcessed)++;
		}
		return hr;
	}

	STDMETHODIMP CapturePin::ReceiveCanBlock(void)
	{
		return S_FALSE; /* Thou shalt not block */
	}



	CaptureFilter::CaptureFilter(AM_MEDIA_TYPE *mt, size_t mt_count, SampleCallback callback_function, void *user_data)
		:
		p_pin(new CapturePin(this, mt, mt_count)),
		state(State_Stopped), i_ref(1), callback_function_(callback_function), callback_user_data_(user_data)
	{
	}

	CaptureFilter::~CaptureFilter()
	{
		p_pin->Release();
	}

	/* IUnknown methods */
	STDMETHODIMP CaptureFilter::QueryInterface(REFIID riid, void **ppv)
	{
		if (riid == IID_IUnknown)
		{
			AddRef();
			*ppv = (IUnknown *)this;
			return NOERROR;
		}
		if (riid == IID_IPersist)
		{
			AddRef();
			*ppv = (IPersist *)this;
			return NOERROR;
		}
		if (riid == IID_IMediaFilter)
		{
			AddRef();
			*ppv = (IMediaFilter *)this;
			return NOERROR;
		}
		if (riid == IID_IBaseFilter)
		{
			AddRef();
			*ppv = (IBaseFilter *)this;
			return NOERROR;
		}
		else
		{
			*ppv = NULL;
			return E_NOINTERFACE;
		}
	}

	STDMETHODIMP_(ULONG) CaptureFilter::AddRef()
	{
		return i_ref++;
	}

	STDMETHODIMP_(ULONG) CaptureFilter::Release()
	{
		if (!InterlockedDecrement(&i_ref))
		{
			delete this;
		}
		return 0;
	}

	STDMETHODIMP CaptureFilter::GetClassID(CLSID *)
	{
		return E_NOTIMPL;
	}

	STDMETHODIMP CaptureFilter::GetState(DWORD, FILTER_STATE *State)
	{
		*State = state;
		return S_OK;
	}

	STDMETHODIMP CaptureFilter::SetSyncSource(IReferenceClock *)
	{
		return S_OK;
	}

	STDMETHODIMP CaptureFilter::GetSyncSource(IReferenceClock **pClock)
	{
		*pClock = NULL;
		return NOERROR;
	}

	STDMETHODIMP CaptureFilter::Stop()
	{
		p_pin->EndFlush();

		state = State_Stopped;
		return S_OK;
	};
	STDMETHODIMP CaptureFilter::Pause()
	{
		state = State_Paused;
		return S_OK;
	}

	STDMETHODIMP CaptureFilter::Run(REFERENCE_TIME)
	{
		state = State_Running;
		return S_OK;
	}

	/* IBaseFilter methods */
	STDMETHODIMP CaptureFilter::EnumPins(IEnumPins ** ppEnum)
	{
		/* Create a new ref counted enumerator */
		*ppEnum = new CaptureEnumPins(this, NULL);
		return *ppEnum == NULL ? E_OUTOFMEMORY : NOERROR;
	}

	STDMETHODIMP CaptureFilter::FindPin(LPCWSTR, IPin **)
	{
		return E_NOTIMPL;
	}

	STDMETHODIMP CaptureFilter::QueryFilterInfo(FILTER_INFO * pInfo)
	{
		memcpy(pInfo->achName, FILTER_NAME, sizeof(FILTER_NAME));

		pInfo->pGraph = p_graph;
		if (p_graph) p_graph->AddRef();

		return NOERROR;
	}

	STDMETHODIMP CaptureFilter::JoinFilterGraph(IFilterGraph * pGraph,
		LPCWSTR)
	{
		p_graph = pGraph;
		return NOERROR;
	}

	STDMETHODIMP CaptureFilter::QueryVendorInfo(LPWSTR*)
	{
		return E_NOTIMPL;
	}

	/* Custom methods */
	CapturePin *CaptureFilter::CustomGetPin()
	{
		return p_pin;
	}

	/****************************************************************************
	 * Implementation of our dummy directshow enumpins class
	 ****************************************************************************/

	CaptureEnumPins::CaptureEnumPins(CaptureFilter *_p_filter,
		CaptureEnumPins *pEnumPins)
		: p_filter(_p_filter), i_ref(1)
	{
		/* Hold a reference count on our filter */
		p_filter->AddRef();

		/* Are we creating a new enumerator */
		if (pEnumPins == NULL)
		{
			i_position = 0;
		}
		else
		{
			i_position = pEnumPins->i_position;
		}
	}

	CaptureEnumPins::~CaptureEnumPins()
	{
		p_filter->Release();
	}

	/* IUnknown methods */
	STDMETHODIMP CaptureEnumPins::QueryInterface(REFIID riid, void **ppv)
	{
		if (riid == IID_IUnknown ||
			riid == IID_IEnumPins)
		{
			AddRef();
			*ppv = (IEnumPins *)this;
			return NOERROR;
		}
		else
		{
			*ppv = NULL;
			return E_NOINTERFACE;
		}
	}

	STDMETHODIMP_(ULONG) CaptureEnumPins::AddRef()
	{
		return i_ref++;
	}

	STDMETHODIMP_(ULONG) CaptureEnumPins::Release()
	{
		if (!InterlockedDecrement(&i_ref))
		{
			delete this;
		}
		return 0;
	}

	/* IEnumPins */
	STDMETHODIMP CaptureEnumPins::Next(ULONG cPins, IPin ** ppPins,
		ULONG * pcFetched)
	{
		unsigned int i_fetched = 0;

		if (i_position < 1 && cPins > 0)
		{
			IPin *pPin = p_filter->CustomGetPin();
			*ppPins = pPin;
			pPin->AddRef();
			i_fetched = 1;
			i_position++;
		}

		if (pcFetched)
		{
			*pcFetched = i_fetched;
		}
		return (i_fetched == cPins) ? S_OK : S_FALSE;
	}

	STDMETHODIMP CaptureEnumPins::Skip(ULONG cPins)
	{
		i_position += cPins;

		if (i_position > 1)
		{
			return S_FALSE;
		}
		return S_OK;
	}

	STDMETHODIMP CaptureEnumPins::Reset()
	{
		i_position = 0;
		return S_OK;
	}

	STDMETHODIMP CaptureEnumPins::Clone(IEnumPins **ppEnum)
	{
		*ppEnum = new CaptureEnumPins(p_filter, this);
		if (*ppEnum == NULL) return E_OUTOFMEMORY;

		return NOERROR;
	}


	CaptureEnumMediaTypes::CaptureEnumMediaTypes(CapturePin *_p_pin, CaptureEnumMediaTypes *pEnumMediaTypes)
		: p_pin(_p_pin), i_ref(1)
	{
		/* Hold a reference count on our filter */
		p_pin->AddRef();

		/* Are we creating a new enumerator */
		if (pEnumMediaTypes == NULL)
		{
			CopyMediaType(&cx_media_type, &p_pin->cx_media_type);
			i_position = 0;
		}
		else
		{
			CopyMediaType(&cx_media_type, &pEnumMediaTypes->cx_media_type);
			i_position = pEnumMediaTypes->i_position;
		}
	}

	CaptureEnumMediaTypes::~CaptureEnumMediaTypes()
	{
		FreeMediaType(cx_media_type);
		p_pin->Release();
	}

	/* IUnknown methods */
	STDMETHODIMP CaptureEnumMediaTypes::QueryInterface(REFIID riid, void **ppv)
	{
		if (riid == IID_IUnknown ||
			riid == IID_IEnumMediaTypes)
		{
			AddRef();
			*ppv = (IEnumMediaTypes *)this;
			return NOERROR;
		}
		else
		{
			*ppv = NULL;
			return E_NOINTERFACE;
		}
	}

	STDMETHODIMP_(ULONG) CaptureEnumMediaTypes::AddRef()
	{
		return i_ref++;
	}

	STDMETHODIMP_(ULONG) CaptureEnumMediaTypes::Release()
	{
		if (!InterlockedDecrement(&i_ref))
		{
			delete this;
		}

		return 0;
	}

	/* IEnumMediaTypes */
	STDMETHODIMP CaptureEnumMediaTypes::Next(ULONG cMediaTypes,
		AM_MEDIA_TYPE ** ppMediaTypes,
		ULONG * pcFetched)
	{
		ULONG copied = 0;
		ULONG offset = 0;
		ULONG max = p_pin->media_type_count;

		if (!ppMediaTypes)
			return E_POINTER;

		if ((!pcFetched) && (cMediaTypes > 1))
			return E_POINTER;

		/*
		** use connection media type as first entry in iterator if it exists
		*/
		copied = 0;
		if (cx_media_type.subtype != GUID_NULL)
		{
			++max;
			if (i_position == 0)
			{
				ppMediaTypes[copied] =
					(AM_MEDIA_TYPE *)CoTaskMemAlloc(sizeof(AM_MEDIA_TYPE));
				if (CopyMediaType(ppMediaTypes[copied], &cx_media_type) != S_OK)
					return E_OUTOFMEMORY;
				++i_position;
				++copied;
			}
		}

		while ((copied < cMediaTypes) && (i_position < max))
		{
			ppMediaTypes[copied] =
				(AM_MEDIA_TYPE *)CoTaskMemAlloc(sizeof(AM_MEDIA_TYPE));
			if (CopyMediaType(ppMediaTypes[copied],
				&p_pin->media_types[i_position - offset]) != S_OK)
				return E_OUTOFMEMORY;

			++copied;
			++i_position;
		}

		if (pcFetched)  *pcFetched = copied;

		return (copied == cMediaTypes) ? S_OK : S_FALSE;
	}

	STDMETHODIMP CaptureEnumMediaTypes::Skip(ULONG cMediaTypes)
	{
		ULONG max = p_pin->media_type_count;
		if (cx_media_type.subtype != GUID_NULL)
		{
			max = 1;
		}
		i_position += cMediaTypes;
		return (i_position < max) ? S_OK : S_FALSE;
	}

	STDMETHODIMP CaptureEnumMediaTypes::Reset()
	{
		FreeMediaType(cx_media_type);
		CopyMediaType(&cx_media_type, &p_pin->cx_media_type);
		i_position = 0;
		return S_OK;
	}

	STDMETHODIMP CaptureEnumMediaTypes::Clone(IEnumMediaTypes **ppEnum)
	{
		*ppEnum = new CaptureEnumMediaTypes(p_pin, this);
		if (*ppEnum == NULL)
		{
			return E_OUTOFMEMORY;
		}
		return NOERROR;
	}

}
#endif