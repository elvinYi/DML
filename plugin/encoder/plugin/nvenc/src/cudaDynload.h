#ifndef H_NVENC_CUDADYNLOAD__H
#define H_NVENC_CUDADYNLOAD__H

typedef enum cudaError_enum {
    CUDA_SUCCESS = 0
} CUresult;
typedef int CUdevice;
typedef void* CUcontext;

#define CUDAAPI __stdcall

typedef CUresult(CUDAAPI *PCUINIT)(unsigned int Flags);
typedef CUresult(CUDAAPI *PCUDEVICEGETCOUNT)(int *count);
typedef CUresult(CUDAAPI *PCUDEVICEGET)(CUdevice *device, int ordinal);
typedef CUresult(CUDAAPI *PCUDEVICEGETNAME)(char *name, int len, CUdevice dev);
typedef CUresult(CUDAAPI *PCUDEVICECOMPUTECAPABILITY)(int *major, int *minor, CUdevice dev);
typedef CUresult(CUDAAPI *PCUCTXCREATE)(CUcontext *pctx, unsigned int flags, CUdevice dev);
typedef CUresult(CUDAAPI *PCUCTXPOPCURRENT)(CUcontext *pctx);
typedef CUresult(CUDAAPI *PCUCTXDESTROY)(CUcontext ctx);

extern PCUINIT cuInit;
extern PCUDEVICEGETCOUNT cuDeviceGetCount;
extern PCUDEVICEGET cuDeviceGet;
extern PCUDEVICEGETNAME cuDeviceGetName;
extern PCUDEVICECOMPUTECAPABILITY cuDeviceComputeCapability;
extern PCUCTXCREATE cuCtxCreate;
extern PCUCTXPOPCURRENT cuCtxPopCurrent;
extern PCUCTXDESTROY cuCtxDestroy;

bool loadCuda();
void unLoadCuda();

#endif
