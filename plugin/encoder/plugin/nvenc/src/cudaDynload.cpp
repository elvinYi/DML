#include "cudaDynload.h"
#include <Windows.h>
#include <WinBase.h>

PCUINIT cuInit = NULL;
PCUDEVICEGETCOUNT cuDeviceGetCount = NULL;
PCUDEVICEGET cuDeviceGet = NULL;
PCUDEVICEGETNAME cuDeviceGetName = NULL;
PCUDEVICECOMPUTECAPABILITY cuDeviceComputeCapability = NULL;
PCUCTXCREATE cuCtxCreate = NULL;
PCUCTXPOPCURRENT cuCtxPopCurrent = NULL;
PCUCTXDESTROY cuCtxDestroy = NULL;

static HMODULE cudaLib = NULL;
static bool loadFailed = false;

#define CHECK_LOAD_FUNC(f, s) \
{ \
    f = (decltype(f))GetProcAddress(cudaLib, s); \
    if (f == NULL) \
    { \
        goto error; \
    } \
}

bool loadCuda()
{
    if (cudaLib != NULL)
        return true;

    if (loadFailed)
        return false;

    cudaLib = LoadLibrary(TEXT("nvcuda.dll"));
    if (cudaLib == NULL)
    {
        //Log(TEXT("Failed loading CUDA dll"));
        goto error;
    }

    CHECK_LOAD_FUNC(cuInit, "cuInit");
    CHECK_LOAD_FUNC(cuDeviceGetCount, "cuDeviceGetCount");
    CHECK_LOAD_FUNC(cuDeviceGet, "cuDeviceGet");
    CHECK_LOAD_FUNC(cuDeviceGetName, "cuDeviceGetName");
    CHECK_LOAD_FUNC(cuDeviceComputeCapability, "cuDeviceComputeCapability");
    CHECK_LOAD_FUNC(cuCtxCreate, "cuCtxCreate_v2");
    CHECK_LOAD_FUNC(cuCtxPopCurrent, "cuCtxPopCurrent_v2");
    CHECK_LOAD_FUNC(cuCtxDestroy, "cuCtxDestroy_v2");

    //Log(TEXT("CUDA loaded successfully"));

    return true;

error:

    if (cudaLib != NULL)
        FreeLibrary(cudaLib);
    cudaLib = NULL;

    loadFailed = true;

    return false;
}

void unLoadCuda() {
    if (cudaLib != NULL)
        FreeLibrary(cudaLib);
    cudaLib = NULL;

    loadFailed = false;
}
