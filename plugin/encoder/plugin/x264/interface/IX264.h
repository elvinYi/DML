#ifndef _IX264_H_
#define _IX264_H_

#ifdef X264ENCODER_EXPORTS
#define LIBIX264_API   __declspec( dllexport ) 
#define LIBIX264_CLS   __declspec( dllexport ) 
#else
#define LIBIX264_API   __declspec( dllimport ) 
#define LIBIX264_CLS   __declspec( dllexport ) 
#endif

#ifdef __cplusplus
extern "C" {
#endif

	struct BaseBuffer;
	typedef void* IX264Handle;

	LIBIX264_API IX264Handle IX264_Create();

	LIBIX264_API int IX264_Init(IX264Handle h, int codecType); // codectype => VideoCodecType in MediaFormat.h 
	
	LIBIX264_API int IX264_Start(IX264Handle h);

	LIBIX264_API int IX264_SetConfig(IX264Handle h, int key, void* config); // key => VideoMetaIndex in MediaFormat.h & CodecFlags in BaseCodec.h
	
	LIBIX264_API int IX264_GetParam(IX264Handle h, int key, void* param);

	LIBIX264_API void IX264_Stop(IX264Handle h);
	
	LIBIX264_API void IX264_Uninit(IX264Handle h);

	LIBIX264_API int IX264_Input(IX264Handle h, BaseBuffer* inputBuffer);

	LIBIX264_API int IX264_Output(IX264Handle h, BaseBuffer* outputBuffer);

	LIBIX264_API void IX264_Destory(IX264Handle h);

#ifdef __cplusplus
}
#endif

#endif