#include "dml_manager.h"
#include <Objbase.h>

#ifdef DML_SDK_EXPORTS
#define DML_SDK_API   __declspec( dllexport ) 
#define DML_SDK_CLS   extern "C" __declspec( dllexport ) 
#else
#define DML_SDK_API
#endif

DML_SDK_CLS int initialize()
{
	CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);

	return DMLManager::CreatDmlManager();
}

DML_SDK_CLS void release()
{
	if (DMLManager::GetDmlManager() == NULL){
		return;
	}

	DMLManager::GetDmlManager()->DestroyDmlManager();
}

DML_SDK_CLS int get_device_list(char device_list[][256])
{
	if (DMLManager::GetDmlManager() == NULL){
		return BAD_VALUE;
	}

	return DMLManager::GetDmlManager()->GetDeviceList(device_list);
}

DML_SDK_CLS int start_video_stream(DMLVideoConfig * pVideoConfig)
{
	if (DMLManager::GetDmlManager() == NULL){
		return BAD_VALUE;
	}

	return DMLManager::GetDmlManager()->StartVideoStream(pVideoConfig);
}

DML_SDK_CLS int stop_video_stream(int nIndex)
{
	if (DMLManager::GetDmlManager() == NULL){
		return BAD_VALUE;
	}

	return DMLManager::GetDmlManager()->StopVideoStream(nIndex);
}
