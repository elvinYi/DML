#pragma once

#include "PubConst.h"
#include "BaseCodec.h"

#include <Windows.h>
#include <WinDef.h>

using namespace MediaPlugin;

IED_ENTRY

typedef void* IBaseCodecHandle;

typedef IBaseCodecHandle(*CreateProc)();
typedef SInt32(*InitializeProc)(IBaseCodecHandle h, SInt32 codecType);
typedef SInt32(*UninitializePorc)(IBaseCodecHandle h);
typedef SInt32(*InternalGetParameterProc)(IBaseCodecHandle h, SInt32 index, Void* params);
typedef SInt32(*InternalSetConfigProc)(IBaseCodecHandle h, SInt32 index, Void* params);
typedef SInt32(*StartProc)(IBaseCodecHandle h);
typedef SInt32(*StopProc)(IBaseCodecHandle h);
typedef SInt32(*ProcessInputProc)(IBaseCodecHandle h, BaseBuffer* inputBuffer);
typedef SInt32(*ProcessOutputProc)(IBaseCodecHandle h, BaseBuffer* outputBuffer);
typedef void(*DestoryProc)(IBaseCodecHandle h);

struct IBaseCodec {
	CreateProc create = nullptr;
	InitializeProc init = nullptr;
	UninitializePorc uninit = nullptr;
	InternalGetParameterProc getParam = nullptr;
	InternalSetConfigProc setConfig = nullptr;
	StartProc start = nullptr;
	StopProc stop = nullptr;
	ProcessInputProc input = nullptr;
	ProcessOutputProc output = nullptr;
	DestoryProc destory = nullptr;
	IBaseCodecHandle h = nullptr;
};

class LoadEncoderDll {
public:
	LoadEncoderDll() {
		hDll = nullptr;
	}
	
	~LoadEncoderDll() {

	}

	bool LoadDll(IEncoderType type, IEncoderIndex index, int codec, IBaseCodec& iBase);
	void UnloadDll();

private:
	bool LoadInternalDll(const wchar_t* dll, const char** address, IBaseCodec& iBase);

private:
	HINSTANCE hDll;
};

IED_EXIT
