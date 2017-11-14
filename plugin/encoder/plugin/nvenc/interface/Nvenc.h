#ifndef _NVENC_H_
#define _NVENC_H_

#ifdef NVENC_EXPORTS
#define LIBNVENC_API   __declspec( dllexport ) 
#define LIBNVENC_CLS   __declspec( dllexport ) 
#else
#define LIBNVENC_API   __declspec( dllimport ) 
#define LIBNVENC_CLS   __declspec( dllexport ) 
#endif

#ifdef __cplusplus
extern "C" {
#endif

	struct BaseBuffer;
	typedef void* INvEncHandle;

	LIBNVENC_API INvEncHandle INvEnc_Create();

	LIBNVENC_API int INvEnc_Init(INvEncHandle h, int codecType); // codectype => VideoCodecType in MediaFormat.h 
	
	LIBNVENC_API int INvEnc_Start(INvEncHandle h);

	LIBNVENC_API int INvEnc_SetConfig(INvEncHandle h, int key, void* config); // key => VideoMetaIndex in MediaFormat.h & CodecFlags in BaseCodec.h
	
	LIBNVENC_API int INvEnc_GetParam(INvEncHandle h, int key, void* param);

	LIBNVENC_API void INvEnc_Stop(INvEncHandle h);
	
	LIBNVENC_API void INvEnc_Uninit(INvEncHandle h);

	LIBNVENC_API int INvEnc_Input(INvEncHandle h, BaseBuffer* inputBuffer);

	LIBNVENC_API int INvEnc_Output(INvEncHandle h, BaseBuffer* outputBuffer);

	LIBNVENC_API void INvEnc_Destory(INvEncHandle h);

#ifdef __cplusplus
}
#endif

#endif