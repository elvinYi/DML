#ifdef WIN32

#include "DirectShowCapture.h"
#include <strmif.h>
#include <stdint.h>

namespace MediaPlugin {
	void WINAPI FreeMediaType(AM_MEDIA_TYPE& mt);
	HRESULT WINAPI CopyMediaType(AM_MEDIA_TYPE *pmtTarget,
		const AM_MEDIA_TYPE *pmtSource);

	/****************************************************************************
	 * Declaration of our dummy directshow filter pin class
	 ****************************************************************************/
	class CaptureFilter;

	class CapturePin : public IPin, public IMemInputPin
	{
		friend class CaptureEnumMediaTypes;

		CaptureFilter  *p_filter;

		IPin *p_connected_pin;

		AM_MEDIA_TYPE *media_types;
		size_t media_type_count;

		AM_MEDIA_TYPE cx_media_type;

		//std::deque<MediaSample> samples_queue;

		long i_ref;

	public:
		CapturePin(CaptureFilter *_p_filter, AM_MEDIA_TYPE *mt, size_t mt_count);
		virtual ~CapturePin();

		/* IUnknown methods */
		STDMETHODIMP QueryInterface(REFIID riid, void **ppv);
		STDMETHODIMP_(ULONG) AddRef();
		STDMETHODIMP_(ULONG) Release();

		/* IPin methods */
		STDMETHODIMP Connect(IPin * pReceivePin, const AM_MEDIA_TYPE *pmt);
		STDMETHODIMP ReceiveConnection(IPin * pConnector,
			const AM_MEDIA_TYPE *pmt);
		STDMETHODIMP Disconnect();
		STDMETHODIMP ConnectedTo(IPin **pPin);
		STDMETHODIMP ConnectionMediaType(AM_MEDIA_TYPE *pmt);
		STDMETHODIMP QueryPinInfo(PIN_INFO * pInfo);
		STDMETHODIMP QueryDirection(PIN_DIRECTION * pPinDir);
		STDMETHODIMP QueryId(LPWSTR * Id);
		STDMETHODIMP QueryAccept(const AM_MEDIA_TYPE *pmt);
		STDMETHODIMP EnumMediaTypes(IEnumMediaTypes **ppEnum);
		STDMETHODIMP QueryInternalConnections(IPin* *apPin, ULONG *nPin);
		STDMETHODIMP EndOfStream(void);

		STDMETHODIMP BeginFlush(void);
		STDMETHODIMP EndFlush(void);
		STDMETHODIMP NewSegment(REFERENCE_TIME tStart, REFERENCE_TIME tStop,
			double dRate);

		/* IMemInputPin methods */
		STDMETHODIMP GetAllocator(IMemAllocator **ppAllocator);
		STDMETHODIMP NotifyAllocator(IMemAllocator *pAllocator, BOOL bReadOnly);
		STDMETHODIMP GetAllocatorRequirements(ALLOCATOR_PROPERTIES *pProps);
		STDMETHODIMP Receive(IMediaSample *pSample);
		STDMETHODIMP ReceiveMultiple(IMediaSample **pSamples, long nSamples,
			long *nSamplesProcessed);
		STDMETHODIMP ReceiveCanBlock(void);

		/* Custom methods */
		//HRESULT CustomGetSample( MediaSample * );
		//HRESULT CustomGetSamples( deque<MediaSample> &external_queue );

		AM_MEDIA_TYPE &CustomGetMediaType();
	};

	/****************************************************************************
	 * Declaration of our dummy directshow filter class
	 ****************************************************************************/
	class CaptureFilter : public IBaseFilter
	{
		friend class CapturePin;

		CapturePin     *p_pin;
		IFilterGraph   *p_graph;
		FILTER_STATE   state;

		long i_ref;

	public:
		CaptureFilter(AM_MEDIA_TYPE *mt, size_t mt_count, SampleCallback callback_function, void *user_data);

		virtual ~CaptureFilter();

		/* IUnknown methods */
		STDMETHODIMP QueryInterface(REFIID riid, void **ppv);
		STDMETHODIMP_(ULONG) AddRef();
		STDMETHODIMP_(ULONG) Release();

		/* IPersist method */
		STDMETHODIMP GetClassID(CLSID *pClsID);

		/* IMediaFilter methods */
		STDMETHODIMP GetState(DWORD dwMSecs, FILTER_STATE *State);
		STDMETHODIMP SetSyncSource(IReferenceClock *pClock);
		STDMETHODIMP GetSyncSource(IReferenceClock **pClock);
		STDMETHODIMP Stop();
		STDMETHODIMP Pause();
		STDMETHODIMP Run(REFERENCE_TIME tStart);

		/* IBaseFilter methods */
		STDMETHODIMP EnumPins(IEnumPins ** ppEnum);
		STDMETHODIMP FindPin(LPCWSTR Id, IPin ** ppPin);
		STDMETHODIMP QueryFilterInfo(FILTER_INFO * pInfo);
		STDMETHODIMP JoinFilterGraph(IFilterGraph * pGraph, LPCWSTR pName);
		STDMETHODIMP QueryVendorInfo(LPWSTR* pVendorInfo);

		/* Custom methods */
		CapturePin *CustomGetPin();

	private:
		//	boost::mutex				*lock_;
		//	boost::condition_variable	*wait_;
		SampleCallback	callback_function_;
		void *			callback_user_data_;
	};

	/****************************************************************************
	 * Declaration of our dummy directshow enumpins class
	 ****************************************************************************/
	class CaptureEnumPins : public IEnumPins
	{
		CaptureFilter  *p_filter;

		int i_position;
		long i_ref;

	public:
		CaptureEnumPins(CaptureFilter *_p_filter, CaptureEnumPins *pEnumPins);
		virtual ~CaptureEnumPins();

		// IUnknown
		STDMETHODIMP QueryInterface(REFIID riid, void **ppv);
		STDMETHODIMP_(ULONG) AddRef();
		STDMETHODIMP_(ULONG) Release();

		// IEnumPins
		STDMETHODIMP Next(ULONG cPins, IPin ** ppPins, ULONG * pcFetched);
		STDMETHODIMP Skip(ULONG cPins);
		STDMETHODIMP Reset();
		STDMETHODIMP Clone(IEnumPins **ppEnum);
	};

	/****************************************************************************
	 * Declaration of our dummy directshow enummediatypes class
	 ****************************************************************************/
	class CaptureEnumMediaTypes : public IEnumMediaTypes
	{
		CapturePin     *p_pin;
		AM_MEDIA_TYPE cx_media_type;

		size_t i_position;
		long i_ref;

	public:
		CaptureEnumMediaTypes(CapturePin *_p_pin,
			CaptureEnumMediaTypes *pEnumMediaTypes);

		virtual ~CaptureEnumMediaTypes();

		// IUnknown
		STDMETHODIMP QueryInterface(REFIID riid, void **ppv);
		STDMETHODIMP_(ULONG) AddRef();
		STDMETHODIMP_(ULONG) Release();

		// IEnumMediaTypes
		STDMETHODIMP Next(ULONG cMediaTypes, AM_MEDIA_TYPE ** ppMediaTypes,
			ULONG * pcFetched);
		STDMETHODIMP Skip(ULONG cMediaTypes);
		STDMETHODIMP Reset();
		STDMETHODIMP Clone(IEnumMediaTypes **ppEnum);
	};
}
#endif