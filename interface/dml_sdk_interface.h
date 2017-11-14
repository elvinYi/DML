#ifndef _DML_SDK_INTERFACE_H_
#define _DML_SDK_INTERFACE_H_

#ifdef WIN32
#ifdef DML_SDK_EXPORTS
#define DML_SDK_API extern "C" __declspec(dllexport)
#else
#define DML_SDK_API extern "C" __declspec(dllimport)
#endif
#else
#define DML_SDK_API 
#endif
#include <stdint.h>
#include <string>

#ifdef __cplusplus
extern "C" {
#endif
	typedef void(*CALLBACK_MSG)(uint32_t uType, uint32_t uMsg, void *param1, void *param2, void *pthis);//UI

	enum DMLEvent {
		// pusher
		DML_EVENT_PUSHER_STATE,
		DML_EVENT_PUSHER_START,
		DML_EVENT_PUSHER_STOP,

		//capture
		DML_EVENT_CAPTURE_OPEN_FAILED,

		//encoder
		DML_EVENT_PUSHER_OPEN_FAILED
	};


	typedef struct UIConfig
	{
		void* windHandle;
		void* pWind;
		CALLBACK_MSG windMsgCB;
	}UIConfig;

	typedef struct DMLVideoConfig
	{
		uint16_t nIndex;
		uint16_t nFps;
		uint16_t nWidth;
		uint16_t nHeight;
		uint32_t nSSRC;
		uint32_t nMemberID;
		uint32_t nSeverPort;
		std::string sSeverAddr;
		std::string sCameraName;
		UIConfig uiConfig;
	}DMLVideoConfig;

	DML_SDK_API int initialize();
	DML_SDK_API void release();
	DML_SDK_API int get_device_list(char device_list[][256]);
	DML_SDK_API int start_video_stream(DMLVideoConfig * pVideoConfig);
	DML_SDK_API int stop_video_stream(int nIndex);

#ifdef __cplusplus
}
#endif
#endif // !_DML_SDK_INTERFACE_H_
