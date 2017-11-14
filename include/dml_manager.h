#ifndef DML_MANAGER_H_
#define DML_MANAGER_H_
#include "pusher/dml_pusher.h"
#include "capture/dml_capture.h"
#include <stdlib.h>
#include <vector>

using namespace std;

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

typedef struct DMLVideoStream
{
	DMLVideoStream::DMLVideoStream() :bIsStop(true),
		pVideoCapture(NULL), pVideoPusher(NULL)
	{
		memset(&videoConfig, 0, sizeof(videoConfig));
	}
	bool bIsStop;
	DMLVideoCapture * pVideoCapture;
	DMLPusher * pVideoPusher;
	DMLVideoConfig videoConfig;
}DMLVideoStream;

class DMLManager
{
public:
	DMLManager();
	~DMLManager();
	static int16_t CreatDmlManager();
	static DMLManager* GetDmlManager();
	static void DestroyDmlManager();
	int16_t StartVideoStream(DMLVideoConfig * pVideoConfig);
	int16_t StopVideoStream(uint16_t nIndex);
	int16_t GetDeviceList(char source_list[][256]);

	
private:

	DMLVideoCapture * creatVideoCapture();
	DMLPusher * creatVideoPusher();
	int16_t creatVideoPreviewer();
	DMLVideoStream * creatVideoStream(DMLVideoConfig * pVideoConfig);

	vector<DMLVideoStream*>  vVideoStreamGroup;
	DMLVideoCapture * pVideoDevicePreviewer;



};





#endif