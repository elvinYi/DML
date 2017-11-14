#include "LoadEncoderDll.h"
#include "Log.h"
#include "Utils.h"

#include <tchar.h>

IED_ENTRY

static const char* tag = "LoadEncoderDll";

#ifdef _DEBUG
static const wchar_t* g_szQuicksyncDll = L"QuickSyncEncoderd.dll";   // dll name
static const wchar_t* g_szNvEncDll = L"NvEncd.dll";   // dll name
static const wchar_t* g_szFaacDll = L"FaacEncoderd.dll";   // dll name
static const wchar_t* g_szX264Dll = L"x264Encoderd.dll";   // dll name
#else
static const wchar_t* g_szQuicksyncDll = L"QuickSyncEncoder.dll";   // dll name
static const wchar_t* g_szNvEncDll = L"NvEnc.dll";   // dll name
static const wchar_t* g_szFaacDll = L"FaacEncoder.dll";   // dll name
static const wchar_t* g_szX264Dll = L"x264Encoder.dll";   // dll name
#endif

#define LoadAddress(fname, fproc, index) \
	iBase.fname = (fproc)GetProcAddress(hDll, address[index]); \
	if (!iBase.fname) \
		return false;

static const char* QuicksyncStr[10] = {
	"IQuicksync_Create",
	"IQuicksync_Init",
	"IQuicksync_Uninit",
	"IQuicksync_GetParam",
	"IQuicksync_SetConfig",
	"IQuicksync_Start",
	"IQuicksync_Stop",
	"IQuicksync_Input",
	"IQuicksync_Output",
	"IQuicksync_Destory"
};

static const char* NvEncStr[10] = {
	"INvEnc_Create",
	"INvEnc_Init",
	"INvEnc_Uninit",
	"INvEnc_GetParam",
	"INvEnc_SetConfig",
	"INvEnc_Start",
	"INvEnc_Stop",
	"INvEnc_Input",
	"INvEnc_Output",
	"INvEnc_Destory"
};

static const char* X264Str[10] = {
	"IX264_Create",
	"IX264_Init",
	"IX264_Uninit",
	"IX264_GetParam",
	"IX264_SetConfig",
	"IX264_Start",
	"IX264_Stop",
	"IX264_Input",
	"IX264_Output",
	"IX264_Destory"
};

static const char* FaacStr[10] = {
	"IFaac_Create",
	"IFaac_Init",
	"IFaac_Uninit",
	"IFaac_GetParam",
	"IFaac_SetConfig",
	"IFaac_Start",
	"IFaac_Stop",
	"IFaac_Input",
	"IFaac_Output",
	"IFaac_Destory"
};

bool LoadEncoderDll::LoadInternalDll(const wchar_t* dll, const char** address, IBaseCodec& iBase) {
	Log::d(tag, "LoadEncoderDll, dll:%s", dll);
	printf("LoadEncoderDll, begin, dll:%s", dll);
	hDll = LoadLibrary(dll);
	printf("LoadEncoderDll, 111 dll:%s", dll);
	if (hDll == nullptr)
		return false;

	LoadAddress(create, CreateProc, 0);
	LoadAddress(init, InitializeProc, 1);
	LoadAddress(uninit, UninitializePorc, 2);
	LoadAddress(getParam, InternalGetParameterProc, 3);
	LoadAddress(setConfig, InternalSetConfigProc, 4);
	LoadAddress(start, StartProc, 5);
	LoadAddress(stop, StopProc, 6);
	LoadAddress(input, ProcessInputProc, 7);
	LoadAddress(output, ProcessOutputProc, 8);
	LoadAddress(destory, DestoryProc, 9);

	Log::d(tag, "LoadEncoderDll, end, dll:%s", dll);
	return true;
}

bool LoadEncoderDll::LoadDll(IEncoderType type, IEncoderIndex index, int codec, IBaseCodec& iBase) {
	bool ret = false;
	if (type == IEncoderType::VIDEO) {
		if (codec == VIDEO_CODEC_H264) {
			switch (index)
			{
			case IEncoder::IEncoderIndex::DEFAULT:
				break;
			case IEncoder::IEncoderIndex::QUICKSYNC:
				if (!IsWindowsVersionEqualOrLater(OS_WIN7)) {
					Log::w(tag, "can not use quicksync, the os is not gt win7!!!");
					break;
				}
				ret = LoadInternalDll(g_szQuicksyncDll, QuicksyncStr, iBase);
				break;
			case IEncoder::IEncoderIndex::NVENCODER:
				ret = LoadInternalDll(g_szNvEncDll, NvEncStr, iBase);
				break;
			case IEncoder::IEncoderIndex::CUDA:
				break;
			case IEncoder::IEncoderIndex::X264:
#ifdef HAVE_X264
				ret = LoadInternalDll(g_szX264Dll, X264Str, iBase);
#endif
				break;
			default:
				break;
			}
		}

		if (codec == VIDEO_CODEC_H265) {
			switch (index)
			{
			case IEncoder::IEncoderIndex::NVENCODER_265:
				ret = LoadInternalDll(g_szNvEncDll, NvEncStr, iBase);
				break;
			default:
				break;
			}
		}
	}
	else if (type == IEncoderType::AUDIO) {
		if (AUDIO_CODEC_AAC == codec) {
			switch (index)
			{
			case IEncoder::IEncoderIndex::FAAC:
				ret = LoadInternalDll(g_szFaacDll, FaacStr, iBase);
				break;
			default:
				break;
			}
		}
	}

	return ret;
}

void LoadEncoderDll::UnloadDll() {
	if (hDll)
		FreeLibrary(hDll);
	hDll = nullptr;
}

IED_EXIT