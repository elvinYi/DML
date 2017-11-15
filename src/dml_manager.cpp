#include "dml_manager.h"
#include "common_define.h"

static DMLManager * gDmlManager = NULL;
#define LOG_FILE "./dml"

DMLManager::DMLManager() :
pVideoDevicePreviewer(NULL)
{
	vVideoStreamGroup.clear();
	creatVideoPreviewer();
	Log::Init(LOG_FILE);
}

DMLManager::~DMLManager()
{
	if (pVideoDevicePreviewer){
		delete pVideoDevicePreviewer;
		pVideoDevicePreviewer = NULL;
	}
	vVideoStreamGroup.clear();
	Log::Release();
}

int16_t DMLManager::creatVideoPreviewer()
{
	if (!pVideoDevicePreviewer){
		pVideoDevicePreviewer = new DMLVideoCapture();
		return NO_ERROR;
	}
	return BAD_VALUE;
}

DMLVideoStream * DMLManager::creatVideoStream(DMLVideoConfig * pVideoConfig)
{
	if (!pVideoConfig){
		return NULL;
	}
	DMLVideoCapture * pVideoCapture = new DMLVideoCapture();
	if (!pVideoCapture){
		return NULL;
	}

	DMLPusher * pVideoPusher = new DMLPusher();
	if (!pVideoPusher){
		return NULL;
	}

	DMLVideoStream * pVideoStream = new DMLVideoStream();
	if (!pVideoStream){
		return NULL;
	}

	pVideoStream->pVideoCapture = pVideoCapture;
	pVideoStream->pVideoPusher = pVideoPusher;
	memcpy(&pVideoStream->videoConfig, pVideoConfig, sizeof(DMLVideoConfig));

	return pVideoStream;
}

int16_t DMLManager::GetDeviceList(char sourceList[][256])
{
	if (!pVideoDevicePreviewer){
		return BAD_VALUE;
	}

	std::vector<std::string> device;

	pVideoDevicePreviewer->get_device_list(device);

	for (int16_t i = 0; i < device.size(); i++)
	{
		strcpy(sourceList[i], device[i].c_str());
		sourceList[i][device[i].size()] = '\0';
	}

	return NO_ERROR;
}

int16_t DMLManager::StartVideoStream(DMLVideoConfig * pVideoConfig)
{
	DMLVideoStream * pVideoStream = creatVideoStream(pVideoConfig);
	if (!pVideoStream){
		return BAD_VALUE;
	}

	pVideoStream->pVideoCapture->SetConfig(&pVideoConfig->uiConfig, pVideoConfig->nWidth, pVideoConfig->nHeight,
		pVideoConfig->nFps, MediaPlugin::VIDEO_DATA_RGB24);

	if (pVideoStream->pVideoCapture->start(pVideoConfig->sCameraName) != NO_ERROR){
		//delete pVideoStream;
		if (pVideoConfig->uiConfig.windMsgCB && pVideoConfig->uiConfig.pWind){
			pVideoConfig->uiConfig.windMsgCB(1, DML_EVENT_CAPTURE_OPEN_FAILED, NULL, NULL, pVideoConfig->uiConfig.pWind);
		}
		return BAD_VALUE;
	}

	DMLVideoEncodeInfo videoEncodeConfig;
	videoEncodeConfig.bFrameNum = 0;
	videoEncodeConfig.bitrate = 1000000;
	videoEncodeConfig.bitrateControl = DMLVideoEncBitrateControl::VBR;
	videoEncodeConfig.colorFormat = 1610612737;
	videoEncodeConfig.cpu = 0;
	videoEncodeConfig.crf = 12;
	videoEncodeConfig.entropyCodingMode = (DMLVideoEntropyCodingMode) 0;
	videoEncodeConfig.fps = pVideoConfig->nFps;
	videoEncodeConfig.haveVideo = 1;
	videoEncodeConfig.height = pVideoConfig->nHeight;
	videoEncodeConfig.width = pVideoConfig->nWidth;
	videoEncodeConfig.keyFrameInterval = 15;
	videoEncodeConfig.preset = 2;
	videoEncodeConfig.profile = DMLVideoEncProfile::MAIN;
	videoEncodeConfig.quality = 6;
	videoEncodeConfig.reFrame = 4;
	videoEncodeConfig.videoEncoderType = X264;
 
	RtpStreamConfig rtpConfig;
	rtpConfig.memberid = pVideoConfig->nMemberID;
	rtpConfig.port = pVideoConfig->nSeverPort;
	rtpConfig.sSeverAddr = pVideoConfig->sSeverAddr;
	rtpConfig.ssrc = pVideoConfig->nSSRC;

	pVideoStream->pVideoPusher->SetConfig(&pVideoConfig->uiConfig, 
		pVideoStream->pVideoCapture->GetVideoQueue(), pVideoConfig->nFps,&videoEncodeConfig,&rtpConfig);

	if (pVideoStream->pVideoPusher->Start() != NO_ERROR){
		delete pVideoStream;
		if (pVideoConfig->uiConfig.windMsgCB && pVideoConfig->uiConfig.pWind){
			pVideoConfig->uiConfig.windMsgCB(1, DML_EVENT_PUSHER_OPEN_FAILED, NULL, NULL, pVideoConfig->uiConfig.pWind);
		}
		return BAD_VALUE;
	}

	pVideoStream->bIsStop = false;
	vVideoStreamGroup.push_back(*pVideoStream);

	return NO_ERROR;
}

int16_t DMLManager::StopVideoStream(uint16_t nIndex)
{
	vector<DMLVideoStream>::iterator it;

	for (it = vVideoStreamGroup.begin(); it != vVideoStreamGroup.end();){
		if ((*it).videoConfig.nIndex == nIndex &&
			(*it).bIsStop == false){
			(*it).pVideoPusher->Stop();
			(*it).pVideoCapture->stop();
			(*it).bIsStop = true;

			it = vVideoStreamGroup.erase(it);
		}else{
			++it;
		}
	}

	return NO_ERROR;
}

int16_t DMLManager::CreatDmlManager()
{
	if (gDmlManager){
		return BAD_VALUE;
	}

	gDmlManager = new DMLManager();

	return NO_ERROR;
}

DMLManager* DMLManager::GetDmlManager()
{
	return gDmlManager;
}

void DMLManager::DestroyDmlManager()
{
	if (gDmlManager){
		delete gDmlManager;
		gDmlManager = NULL;
	}
}




