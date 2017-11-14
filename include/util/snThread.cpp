	/************************************************************************
	*																		*
	*		Snail, Inc. Confidential and Proprietary, 2003 -2009			*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		snThread.c

	Contains:	memory operator implement code

	Written by:	David

	Change History (most recent first):
	2009-03-15		David			Create file

*******************************************************************************/
#if defined _WIN32
#include <windows.h>
#elif defined LINUX
#include <sys/mman.h>
#include <pthread.h>
#include <sys/prctl.h>
#elif defined __SYMBIAN32__
#include <e32std.h>
#elif defined _IOS || defined _MAC_OS
#include "snLog.h"
#include <pthread.h>
#include "snOSFunc.h"
#endif

#include "snThread.h"

#define SN_DEFAULT_STACKSIZE (128 * 1024)

#ifdef _WIN32
#define MS_VC_EXCEPTION 0x406D1388
#pragma pack(push,8)
typedef struct tagSNTHREADNAME_INFO
{
   SN_U32 dwType; // Must be 0x1000.
   char * szName; // Pointer to name (in user addr space).
   SN_U32 dwThreadID; // Thread ID (-1=caller thread).
   SN_U32 dwFlags; // Reserved for future use, must be zero.
} SNTHREADNAME_INFO;
#pragma pack(pop)
#endif // _WIN32

void snThreadSetName(SN_U32 uThreadID, const char* threadName)
{
#ifdef _WIN32
   //SNTHREADNAME_INFO info;
   //info.dwType = 0x1000;
   //info.szName = (char *)threadName;
   //info.dwThreadID = uThreadID;
   //info.dwFlags = 0;

   //__try
   //{
   //   RaiseException( MS_VC_EXCEPTION, 0, sizeof(info)/sizeof(ULONG_PTR), (ULONG_PTR*)&info );
   //}
   //__except(EXCEPTION_EXECUTE_HANDLER)
   //{
   //}
#elif defined LINUX 
	prctl(PR_SET_NAME, (unsigned long)threadName , 0 , 0 , 0);
#endif // _WIN32
}

void snThreadGetName(char* const threadName, const int buflen)
{
	if (threadName == NULL)
		return;
		
//#ifdef _WIN32
//	  char* pszName=NULL;
//    __asm{
//        mov eax, fs:[0x18]    //    Locate the caller's TIB
//        mov eax, [eax+0x14]    //    Read the pvArbitary field in the TIB
//        mov [pszName], eax    //    pszName = pTIB->pvArbitary
//    }
//
//	if (pszName)
//		strcpy_s(threadName, buflen, pszName);
//
//
//#elif defined LINUX
//	prctl(PR_GET_NAME, (unsigned long)threadName , 0 , 0 , 0);
//#endif // _WIN32

}

SN_U32 snThreadCreate (snThreadHandle * pHandle, SN_ULONG * pID,
								 snThreadProc fProc, SN_PTR pParam, SN_U32 uFlag)
{
	if (pHandle == NULL || pID == NULL)
		return SN_ERR_INVALID_ARG;

	*pHandle = NULL;
	*pID = 0;
#ifdef _METRO
    std::thread *psnThread = new std::Thread(fProc, pParam);
    *pHandle = psnThread->native.handle();
    
	if (*pHandle == NULL)
		return SN_ERR_OUTOF_MEMORY;

    *pID = (SN_ULONG)*pHandle;
#elif defined _WIN32
	DWORD threadID = 0;
	*pHandle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)fProc, pParam, 0, &threadID);
	*pID = threadID;
	if (*pHandle == NULL)
		return SN_ERR_OUTOF_MEMORY;
#elif defined LINUX
	pthread_t 		tt;

	pthread_attr_t	attr;
	pthread_attr_init(&attr);
#if !defined NNJ
	pthread_attr_setstacksize(&attr, SN_DEFAULT_STACKSIZE);
	pthread_attr_setguardsize(&attr, PAGE_SIZE);
	/*attr.flags = 0;
	attr.stack_base = NULL;
	attr.stack_size = SN_DEFAULT_STACKSIZE; //DEFAULT_STACKSIZE;
	attr.guard_size = PAGE_SIZE;*/

	if (uFlag == 0)
	{
		pthread_attr_setschedpolicy(&attr, SCHED_NORMAL);
		attr.sched_priority = 0;
	}
	else
	{
		pthread_attr_setschedpolicy(&attr, SCHED_RR);
		attr.sched_priority = uFlag;
	}
#else 

#define PAGE_SIZE   (1UL << 12)
	pthread_attr_setstacksize(&attr, SN_DEFAULT_STACKSIZE);
	pthread_attr_setguardsize(&attr, PAGE_SIZE);

	struct sched_param param;
	if (uFlag == 0) {
		pthread_attr_setschedpolicy(&attr, SCHED_OTHER);
		param.sched_priority = 0;
	} else {
		pthread_attr_setschedpolicy(&attr, SCHED_RR);
		param.sched_priority = uFlag;
	}
	pthread_attr_setschedparam(&attr, &param);
#endif

	int rs = pthread_create(&tt, &attr, (void*(*)(void*))fProc ,pParam);
	pthread_attr_destroy(&attr);

	if (rs != 0)
		return SN_ERR_OUTOF_MEMORY;

	rs = pthread_detach(tt);

	*pHandle = (snThreadHandle)tt;
	*pID = (SN_U32)tt;
#elif defined __SYMBIAN32__
	RThread thread;
	int nRC = thread.Create(_L("SNThreadName"), (TThreadFunction)fProc, 40960, KMinHeapSize, 256
			* KMinHeapSize, NULL);
	if (nRC == KErrNone)
		thread.Resume();
	
#elif defined _IOS || defined _MAC_OS
	pthread_attr_t  attr;
    pthread_t       posixThreadID;
    int             returnVal;
	/*
	attr.flags = 0;
	attr.stack_base = NULL;
	attr.stack_size = SN_DEFAULT_STACKSIZE; //DEFAULT_STACKSIZE;
	attr.guard_size = PAGE_SIZE;
	
	if (uFlag == 0)
	{
		attr.sched_policy = SCHED_NORMAL;
		attr.sched_priority = 0;
	}
	else
	{
		attr.sched_policy = SCHED_RR;
		attr.sched_priority = uFlag;
	}
 */
	
    returnVal = pthread_attr_init(&attr);
    returnVal = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    //int     threadError = 
    pthread_create(&posixThreadID, &attr, (void*(*)(void*))fProc, pParam);
	*pHandle	= (snThreadHandle)posixThreadID;
	*pID		= (SN_ULONG)posixThreadID;
	//pthread_detach(posixThreadID);
#endif

	return SN_ERR_NONE;
}

SN_U32 snThreadClose (snThreadHandle hHandle, SN_U32 uExitCode)
{
	if (hHandle == NULL)
		return SN_ERR_INVALID_ARG;
#ifdef _METRO
    delete(hHandle);
#elif defined _WIN32
	CloseHandle (hHandle);
#endif //_METRO

	return SN_ERR_NONE;
}

SN_U32 snThreadGetPriority (snThreadHandle hHandle, snThreadPriority * pPriority)
{
	if (hHandle == NULL)
		return SN_ERR_INVALID_ARG;

#ifdef _METRO
	return SN_ERR_NOT_IMPLEMENT;
#elif defined _WIN32
	SN_S32 nPriority = 0;
	nPriority = GetThreadPriority (hHandle);
	snThreadConvertPriority (pPriority, &nPriority, SN_FALSE);
#endif //_WIN32

#ifdef _IOS
	int policy = 0;
	struct sched_param param;
    SN_S32 nPriority = 0;
	
	int iRet = pthread_getschedparam((pthread_t)hHandle, &policy, &param);
	if (0 != iRet) {
		SNLOGE("pthread_getschedparam error :%d", iRet);
		return SN_ERR_FAILED;
	}
	nPriority = param.sched_priority;
	SNLOGI("get succ hHandle:%ld, policy:%d, param.sched_priority:%d", (SN_ULONG)hHandle, policy, param.sched_priority);
	snThreadConvertPriority (pPriority, &nPriority, SN_FALSE);
#endif

	return SN_ERR_NONE;
}


SN_U32 snThreadSetPriority (snThreadHandle hHandle, snThreadPriority uPriority)
{
	if (hHandle == NULL)
		return SN_ERR_INVALID_ARG;

#ifdef _METRO
	return SN_ERR_NOT_IMPLEMENT;
#elif defined _WIN32
	SN_S32 nPriority = 0;
	snThreadConvertPriority (&uPriority, &nPriority, SN_TRUE);
	SetThreadPriority (hHandle, nPriority);
#endif //_WIN32

#ifdef _IOS
	int policy = 0;
	struct sched_param param;
    SN_S32 nPriority = 0;
	
	int iRet = pthread_getschedparam((pthread_t)hHandle, &policy, &param);
	if (0 != iRet) {
		SNLOGE("pthread_getschedparam hHandle:%ld, error :%d", (SN_ULONG)hHandle, iRet);
		return SN_ERR_FAILED;
	}

	SNLOGI("get succ hHandle:%ld, policy:%d, param.sched_priority:%d", (SN_ULONG)hHandle, policy, param.sched_priority);
	
	snThreadConvertPriority (&uPriority, &nPriority, SN_TRUE);
	param.sched_priority = nPriority;
	iRet = pthread_setschedparam((pthread_t)hHandle, policy, &param);
	
	if (0 != iRet) {
		SNLOGE("pthread_attr_setschedparam hHandle:%ld, error :%d, param.sched_priority:%d", (SN_ULONG)hHandle, iRet, param.sched_priority);
		return SN_ERR_FAILED;
	}

	SNLOGI("set succ hHandle:%ld, policy:%d, param.sched_priority:%d", (SN_ULONG)hHandle, policy, param.sched_priority);
#endif
	
	return SN_ERR_NONE;
}

SN_U32 snThreadSuspend (snThreadHandle hHandle)
{
	if (hHandle == NULL)
		return SN_ERR_INVALID_ARG;

#ifdef _METRO
	return SN_ERR_NOT_IMPLEMENT;
#elif defined _WIN32
	SuspendThread (hHandle);
#endif //_WIN32

	return SN_ERR_NONE;
}

SN_U32 snThreadResume (snThreadHandle hHandle)
{
	if (hHandle == NULL)
		return SN_ERR_INVALID_ARG;

#ifdef _METRO
	return SN_ERR_NOT_IMPLEMENT;
#elif defined _WIN32
	ResumeThread (hHandle);
#endif //_WIN32

	return SN_ERR_NONE;
}

SN_U32 snThreadProtect (snThreadHandle hHandle)
{

	return SN_ERR_NONE;
}

SN_U32 snThreadConvertPriority (snThreadPriority * pPriority, SN_S32 * pPlatform, SN_BOOL bPlatform)
{
	if (bPlatform)
	{
		switch (*pPriority)
		{
#if defined _WIN32 || defined _IOS
		case SN_THREAD_PRIORITY_TIME_CRITICAL:
			*pPlatform = THREAD_PRIORITY_TIME_CRITICAL;
			break;

		case SN_THREAD_PRIORITY_HIGHEST:
			*pPlatform = THREAD_PRIORITY_HIGHEST;
			break;

		case SN_THREAD_PRIORITY_ABOVE_NORMAL:
			*pPlatform = THREAD_PRIORITY_ABOVE_NORMAL;
			break;

		case SN_THREAD_PRIORITY_NORMAL:
			*pPlatform = THREAD_PRIORITY_NORMAL;
			break;

		case SN_THREAD_PRIORITY_BELOW_NORMAL:
			*pPlatform = THREAD_PRIORITY_BELOW_NORMAL;
			break;

		case SN_THREAD_PRIORITY_LOWEST:
			*pPlatform = THREAD_PRIORITY_LOWEST;
			break;

		case SN_THREAD_PRIORITY_IDLE:
			*pPlatform = THREAD_PRIORITY_IDLE;
			break;
#endif // _WIN32
		default:
			break;
		}
	}
	else
	{
		switch (*pPlatform)
		{
#if defined _WIN32 || defined _IOS
		case THREAD_PRIORITY_TIME_CRITICAL:
			*pPriority = SN_THREAD_PRIORITY_TIME_CRITICAL;
			break;

		case THREAD_PRIORITY_HIGHEST:
			*pPriority = SN_THREAD_PRIORITY_HIGHEST;
			break;

		case THREAD_PRIORITY_ABOVE_NORMAL:
			*pPriority = SN_THREAD_PRIORITY_ABOVE_NORMAL;
			break;

		case THREAD_PRIORITY_NORMAL:
			*pPriority = SN_THREAD_PRIORITY_NORMAL;
			break;

		case THREAD_PRIORITY_BELOW_NORMAL:
			*pPriority = SN_THREAD_PRIORITY_BELOW_NORMAL;
			break;

		case THREAD_PRIORITY_LOWEST:
			*pPriority = SN_THREAD_PRIORITY_LOWEST;
			break;

		case THREAD_PRIORITY_IDLE:
			*pPriority = SN_THREAD_PRIORITY_IDLE;
			break;
#endif // _WIN32
		default:
			break;
		}
	}

	return SN_ERR_NONE;
}

SN_ULONG snThreadGetCurrentID (void)
{
#ifdef _WIN32
	return (SN_S32) GetCurrentThreadId ();
#elif defined _LINUX
	return (SN_S32) pthread_self ();
#elif defined _MAC_OS || defined _IOS
	return (SN_ULONG) pthread_self ();
#endif // _WIN32
}
    
