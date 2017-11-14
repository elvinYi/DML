	/************************************************************************ *																		*
	*		Snail, Inc. Confidential and Proprietary, 2009				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		snOSFunc.cpp

	Contains:	component api cpp file

	Written by:	David

	Change History (most recent first):
	2009-04-01	David			Create file

*******************************************************************************/
#if defined _WIN32
#include <winsock2.h>
#include <windows.h>
#include <time.h>
#include "Ws2tcpip.h"
#include <stdio.h>
#ifdef WINCE
#pragma comment(lib, "mmtimer.lib")
#pragma comment(lib, "Ws2")
#else
#include <pdh.h>
#include <pdhmsg.h>
#pragma comment(lib, "pdh.lib")
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "Ws2_32.lib")
#endif //WINCE

#if (defined _WIN32 || defined _WIN64) && (!defined WINCE)
#include <intrin.h>
#endif	// (_WIN32 || _WIN64) && !WINCE

#elif defined _LINUX || defined (_IOS) || defined (_MAC_OS)
#include <stdio.h>
#include <unistd.h>
#include <time.h>      
#include <pthread.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h> 
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <assert.h>
#endif

#if defined (_IOS) || defined (_MAC_OS)
#import <sys/sysctl.h>
#import <Foundation/Foundation.h>
#import <mach/mach.h>
#endif

#if defined (_IOS)
#import <UIKit/UIKit.h>
#endif

#include "snOSFunc.h"

#define LOG_TAG "snOSFunc"
#ifndef HAVE_SYS_UIO_H
#define HAVE_SYS_UIO_H
#endif
#undef HAVE_SYS_UIO_H

#ifdef WINCE
const DOUBLE VT_SECOND_1970 = 2209161600.0;
const DOUBLE SECONDS_IN_ONE_DAY = 86400;
time_t __cdecl mktime(tm* pTM)
{
    SYSTEMTIME stToConvert = {
        pTM->tm_year+1900,
        pTM->tm_mon+1,
        pTM->tm_wday,
        pTM->tm_mday,
        pTM->tm_hour,
        pTM->tm_min,
        pTM->tm_sec,
        0};
    DOUBLE dToCal;
    SystemTimeToVariantTime(&stToConvert, &dToCal);
    return (time_t)(dToCal*SECONDS_IN_ONE_DAY - VT_SECOND_1970);
}
#endif //WINCE

void snOS_Sleep (SN_U32 nTime)
{
#ifdef _METRO
	Concurrency::wait(nTime);
#elif defined _WIN32
	Sleep (nTime);
#elif defined _LINUX
	usleep (1000 * nTime);
#elif defined __SYMBIAN32__
	User::After (nTime * 1000);
#elif defined _IOS || defined _MAC_OS
	usleep (1000 * nTime);
#endif // _WIN32
}

void snOS_NanoSleep (SN_U64 nTime)
{
#ifdef _METRO
	Concurrency::wait(nTime/1000000000);
#elif defined WIN32
	//HANDLE timer;	/* Timer handle */
	//LARGE_INTEGER li;	/* Time defintion */
	///* Create timer */
	//if(!(timer = CreateWaitableTimer(NULL, TRUE, NULL)))
	//	return;
	///* Set timer properties */
	//li.QuadPart = -nTime/100;
	//if(!SetWaitableTimer(timer, &li, 0, NULL, NULL, FALSE)){ 
	//	CloseHandle(timer);
	//	return;
	//}
	///* Start & wait for timer */
	//WaitForSingleObject(timer, INFINITE);
	///* Clean resources */
	//CloseHandle(timer);
#elif defined _LINUX || defined _IOS || defined _MAC_OS
	timespec	t, rem;
	t.tv_sec = 0;
	t.tv_nsec = (long)nTime;
	nanosleep( &t, &rem);
#endif // _WIN32
}

void snOS_SleepExitable(SN_U32 nTime, SN_BOOL * pbExit)
{
	SN_U64 nStart = snOS_GetSysTime();
	while(snOS_GetSysTime() < nStart + nTime)
	{
		if(pbExit && SN_TRUE == *pbExit)
			return;

		snOS_Sleep(5);
	}
}

SN_U64 snOS_GetSysTime (void)
{
	SN_U64	nTime = 0;

#ifdef _METRO
	nTime = GetTickCount64();
#elif defined _WIN32
	nTime = ::timeGetTime();
#elif defined _LINUX
    timespec tv;
	clock_gettime(CLOCK_MONOTONIC, &tv);

    static timespec stv = {0, 0};
    if ((0 == stv.tv_sec) && (0 == stv.tv_nsec))
	{
		stv.tv_sec = tv.tv_sec;
		stv.tv_nsec = tv.tv_nsec;
	}
    
    nTime = (SN_U64)((tv.tv_sec - stv.tv_sec) * 1000 + (tv.tv_nsec - stv.tv_nsec) / 1000000);

#elif defined _IOS || defined _MAC_OS
	static SN_U64 globalBeginTime = [[NSProcessInfo processInfo] systemUptime] * 1000;
	nTime = [[NSProcessInfo processInfo] systemUptime] * 1000 - globalBeginTime;
#endif // _WIN32

	return nTime;
}

SN_U64 snOS_GetNanoTime (void)
{
	SN_U64	nTime = 0;
#ifdef _METRO
	nTime = GetTickCount64() * 1000000;
#elif defined _WIN32
	nTime = ::timeGetTime() * 1000000;
#elif defined _LINUX
    timespec tv;
	clock_gettime(CLOCK_MONOTONIC, &tv);
	nTime = (SN_U64)tv.tv_sec * (1000 * 1000 * 1000) + tv.tv_nsec;
#elif defined _IOS || defined _MAC_OS
	
	nTime = [[NSProcessInfo processInfo] systemUptime] * (1000 * 1000 * 1000);
#endif
	return nTime;
}


SN_U64 snGetSysTime (void)
{
	SN_U64	nTime = 0;

#if defined _WIN32
	nTime = (SN_U64)::timeGetTime();
#elif defined _LINUX 
    timespec tv;
	clock_gettime(CLOCK_MONOTONIC, &tv);

    static timespec stv = {0, 0};
    if ((0 == stv.tv_sec) && (0 == stv.tv_nsec)) {
		stv.tv_sec = tv.tv_sec;
		stv.tv_nsec = tv.tv_nsec;
	}
    
    nTime = (SN_U64)((tv.tv_sec - stv.tv_sec) * 1000 + (tv.tv_nsec - stv.tv_nsec) / 1000000);

#elif defined _IOS || defined _MAC_OS
	
	nTime = [[NSProcessInfo processInfo] systemUptime] * 1000;
#endif // _WIN32

	return nTime;
}

void snOS_SetTimePeriod(SN_BOOL bStartOrEnd, SN_U32 nPeriod)
{
#ifdef _METRO
    return ;
#elif defined _WIN32
	if(bStartOrEnd)
		::timeBeginPeriod(nPeriod);
	else
		::timeEndPeriod(nPeriod);
#endif	// _WIN32
}

SN_U32  snOS_GetModuleFileName(SN_PTR pHandle, SN_PTCHAR buf,  SN_U32 size)
{
#if defined (_LINUX)
	int r = readlink("/proc/self/exe", buf, size);
		if (r<0 || r>=(int)size)
		return r;

	buf[r] = '\0';
	return r;
#elif defined(_METRO)
	Platform::String^ fullPath = Windows::ApplicationModel::Package::Current->InstalledLocation->Path;
	_tcscpy (buf , (const wchar_t*)fullPath->Data());
	return 0;
#elif defined(_WIN32)
#ifdef UNICODE
    return ::GetModuleFileName((HMODULE)pHandle, (LPWSTR)buf, size);
#else
    return ::GetModuleFileName((HMODULE)pHandle, buf, size);
#endif
#elif defined(_MAC_OS)
    CFURLRef appUrlRef = CFBundleCopyBundleURL(CFBundleGetMainBundle());
    CFStringRef macPath = CFURLCopyFileSystemPath(appUrlRef,
                                                  kCFURLPOSIXPathStyle);
    const char *pathPtr = CFStringGetCStringPtr(macPath,
                                                CFStringGetSystemEncoding());
    strcpy(buf, pathPtr);
    CFRelease(appUrlRef);
    CFRelease(macPath);
    
    return 0;
#endif // _LINUX

    return 0;
}

    SN_U32 snOS_GetAppResourcePath (SN_PTCHAR outPathAndName, SN_PTCHAR inResourceName, SN_U32 nSize)
    {
#ifdef _IOS
        if (NULL == inResourceName) {
            return 1;
        }
        
        NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
        NSString* strPath = [[NSBundle mainBundle] pathForResource:[NSString stringWithFormat:@"%s", inResourceName] ofType:nil];
        
        if (nil == strPath) {
            [pool release];
            return 1;
        }
        
        snstrncpy(outPathAndName, [strPath UTF8String], nSize);
        
        if (snstrlen([strPath UTF8String]) < nSize) {
            outPathAndName[snstrlen([strPath UTF8String])] = _T('\0');
        }
        else {
            outPathAndName[nSize] = _T('\0');
        }
        
        [pool release];
        return 0;
#else
        return 1;
#endif // _LINUX
    }
    
SN_U32 snOS_GetAppFolder (SN_PTCHAR pFolder, SN_U32 nSize)
{
#ifdef _METRO
	Platform::String^ fullPath = Windows::ApplicationModel::Package::Current->InstalledLocation->Path;
	_tcscpy (pFolder , (const wchar_t*)fullPath->Data());
	return 0;
#elif defined(_WIN32)
#ifdef UNICODE
	GetModuleFileName (NULL, (LPWSTR)pFolder, nSize);
#else
	GetModuleFileName (NULL, pFolder, nSize);
#endif
#elif defined _LINUX
	SN_S32 r = readlink("/proc/self/exe", pFolder, nSize);
	if (r < 0 || r >= (SN_S32)nSize)
		return r;
	pFolder[r] = '\0';
#elif defined _MAC_OS
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
	// tag:20110123
    NSString *filePath = [[[NSBundle mainBundle] bundlePath] stringByAppendingString:@"/"];
    if ([filePath hasSuffix:@"PluginProcess.app/"])
    {
        filePath = [NSHomeDirectory() stringByAppendingString:@"/"];
    }
    
	snstrcpy(pFolder, [filePath UTF8String]);
    [pool release];
	//snstrcpy(pFolder, "/Library/Internet Plug-Ins/");
#elif defined _IOS
	NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
	NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
	NSString *filePath = [[paths objectAtIndex:0] stringByAppendingString:@"/"];
	strcpy(pFolder, [filePath UTF8String]);
	[pool release];
	//NSString* dir = [[NSBundle mainBundle] bundlePath];
#endif // _LINUX

    SN_PTCHAR pPos = strrchr(pFolder, _T('/'));
	if (pPos == NULL)
		pPos = strrchr(pFolder, _T('\\'));
    SN_S32 nPos = pPos - pFolder;
    pFolder[nPos+1] = _T('\0');

	return 0;
}

SN_U32 snOS_GetPluginModuleFolder(SN_PTCHAR pFolder, SN_U32 nSize)
{
#ifdef _MAC_OS
@autoreleasepool {
    
	SN_TCHAR szFullAppName[256];
	snOS_GetModuleFileName(NULL, szFullAppName, strlen(szFullAppName));
	
	if(snstrstr(szFullAppName, _T("Safari")))
		snOS_GetBundleFolderByIdentifier((char*)"com.snail.safari", pFolder, nSize);
	else if(snstrstr(szFullAppName, _T("Firefox")))
		snOS_GetBundleFolderByIdentifier((char*)"com.snail.firefox", pFolder, nSize);
	
	SNLOGI("bundle path = %s\n", pFolder);
	
	snstrcat(pFolder, "/Contents/MacOS/");
	
	SNLOGI("Plugin module folder path = %s\n", pFolder);
}
#endif
	
	return 0;
}

SN_U32	snOS_GetBundleFolderByIdentifier(SN_PTCHAR pIdentifier, SN_PTCHAR pFolder, SN_U32 nSize)
{
#ifdef _MAC_OS
@autoreleasepool {
    if ((NULL == pIdentifier) || (NULL == pFolder)) {
        return 1;
    }
    
    NSBundle* bundle = [NSBundle bundleWithIdentifier:[NSString stringWithFormat:@"%s", pIdentifier]];
    if (nil == bundle) {
        return 1;
    }
    
	NSString* path = [bundle bundlePath];
    if (nil != path) {
        strcpy(pFolder, [path UTF8String]);
    }

    return 0;
}
#endif
	return 1;
}

SN_U32 snOS_GetThreadTime (SN_PTR	hThread)
{
	SN_U32 nTime = 0;

#ifdef _METRO
#elif defined _WIN32
	if(hThread == NULL)
		hThread = GetCurrentThread();

	if(!hThread)
		return 0;

	FILETIME ftCreationTime;
	FILETIME ftExitTime;
	FILETIME ftKernelTime;
	FILETIME ftUserTime;

	BOOL bRC = GetThreadTimes(hThread, &ftCreationTime, &ftExitTime, &ftKernelTime, &ftUserTime);
	if (!bRC)
		return -1;

	LONGLONG llKernelTime = ftKernelTime.dwHighDateTime;
	llKernelTime = llKernelTime << 32;
	llKernelTime += ftKernelTime.dwLowDateTime;

	LONGLONG llUserTime = ftUserTime.dwHighDateTime;
	llUserTime = llUserTime << 32;
	llUserTime += ftUserTime.dwLowDateTime;

	nTime = int((llKernelTime + llUserTime) / 10000);
#elif defined _LINUX
//	nTime = snOS_GetSysTime ();
    timespec tv;
	clock_gettime(CLOCK_THREAD_CPUTIME_ID, &tv);

    static timespec stvThread = {0, 0};
    if ((0 == stvThread.tv_sec) && (0 == stvThread.tv_nsec))
	{
		stvThread.tv_sec = tv.tv_sec;
		stvThread.tv_nsec = tv.tv_nsec;
	}
    
    nTime = (SN_U32)((tv.tv_sec - stvThread.tv_sec) * 1000 + (tv.tv_nsec - stvThread.tv_nsec) / 1000000);
#elif defined _IOS || defined _MAC_OS

	nTime = (SN_U32)snOS_GetSysTime();
#endif // _WIN32

	return nTime;
}

SN_S32 snOS_EnableDebugMode (int nShowLog)
{
#ifndef _SNNDBG
#if defined _IOS || defined _MAC_OS || defined _LINUX_ANDROID
    
    int 			nRC = 0;
    char 			szFile[1024];
    char			szBuff[256];
    struct stat 	stFile;
    time_t 			tNow;
    struct tm *		ptm;
	
#ifdef _LINUX_ANDROID
	strcpy (szFile,  "/data/local/voDebugFolder/vosystemtime.txt");
	nRC = stat (szFile, &stFile); 
	if (nRC != 0)
	{
		strcpy (szFile,  "/data/local/tmp/voDebugFolder/vosystemtime.txt");
	}
#else //_IOS _MAC_OS
    char szAppDir[1024];
    snOS_GetAppFolder(szAppDir, 1024);
    memset(szFile, 0, 1024);
    sprintf (szFile, "%s%s", szAppDir, "snDebugFolder/vosystemtime.txt");
#endif //_LINUX_ANDROID
    
    nRC = stat (szFile, &stFile);
    if (nRC != 0)
    {
        if (nShowLog > 0)
            SNLOGI ("It could not get file time from file %s", szFile);
        return 0;
    }
    
	time_t tFile = stFile.st_atime;
	ptm = localtime(&tFile);
	if (nShowLog > 0)
	{
		SNLOGI ("File   Time: %d  %d-%d-%d %d:%d:%d\n", (int)tFile, ptm->tm_year + 1900, ptm->tm_mon + 1,
		     ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec);   
	}
		        		
    tNow = time(NULL);		
    
	ptm = localtime(&tNow);
	if (nShowLog > 0)	
	{
		SNLOGI ("System Time: %d  %d-%d-%d %d:%d:%d   %d", (int)tNow, ptm->tm_year + 1900, ptm->tm_mon + 1,
		        ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec, abs (tNow - tFile)); 
	} 
		           
    if (abs (tNow - tFile) > 7200)
    	return 0;
    	
	FILE * hFile = fopen (szFile, "rb");
	if (hFile == NULL)
		return 0;
		
	fgets (szBuff, 256, hFile);
	fclose(hFile);
	
	int nYear = 0, nMonth = 0, nDay = 0, nHour = 0, nMin = 0, nSec = 0;
	sscanf (szBuff, "%d-%d-%d %d:%d:%d", &nYear, &nMonth, &nDay, &nHour, &nMin, &nSec);

	if (nShowLog > 0)
	{
		SNLOGI ("File: %s,  Time: %d-%d-%d  %d:%d:%d", szBuff, nYear, nMonth, nDay, nHour, nMin, nSec);
	}
	
	if (nYear - 1900 != ptm->tm_year || nMonth != ptm->tm_mon + 1 || nDay != ptm->tm_mday)
		return 0;
		
	int nFileTime = nHour * 3600 + nMin * 60 + nSec;
	int nSysteimTime = ptm->tm_hour * 3600 + ptm->tm_min * 60 + ptm->tm_sec;
	

	if (nShowLog > 0)
	{
		SNLOGI ("File Time %d, System time %d, Offset: %d", nFileTime, nSysteimTime, abs (nSysteimTime - nFileTime));
	}
 
    if (abs (nSysteimTime - nFileTime) > 7200)
    	return 0;		 	
 
 	SNLOGW ("The EnableDebugMode successful !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
	   
    return 1;
    
#else

	return 0;
#endif // _LINUX_ANDROID
    
#else
    return 0;
#endif // _SNNDBG
    
}

SN_U32 snOS_Log (char* pText)
{
#if defined _IOS || defined _MAC_OS
	printf("%s", pText);
#else
#endif // _IOS _MAC_OS

	return 0;
}

SN_U32 snOS_GetApplicationID(SN_PTCHAR pAppID, SN_U32 nSize)
{
	int nRet = -1;
	
#if defined _IOS || defined _MAC_OS
	NSString* pID = [[NSBundle mainBundle] bundleIdentifier];
	strcpy(pAppID, [pID UTF8String]);
	nRet = 0;
#endif // _IOS
	
	return nRet;
}

SN_U32	snOS_GetCPUNum ()
{
#if defined(_LINUX_ANDROID)
	int nTemps[] = {1, 3, 5, 7, 9, 11, 13, 15, 17, 19, 21};
	char cCpuName[512];
	memset(cCpuName, 0, sizeof(cCpuName));

	for(int i = (sizeof(nTemps)/sizeof(nTemps[0])) - 1 ; i >= 0; i--)
	{
		sprintf(cCpuName, "/sys/devices/system/cpu/cpu%d", nTemps[i]);
		int nOk = access(cCpuName, F_OK);
		if( nOk == 0)
		{
			return nTemps[i]+1;
		}
	}
	return 1;
#elif defined(_METRO)
    SYSTEM_INFO si;
    GetNativeSystemInfo(&si);
    return si.dwNumberOfProcessors;
#elif defined(_WIN32)
        SYSTEM_INFO si;
        GetSystemInfo(&si);
        return si.dwNumberOfProcessors;
#elif defined(_IOS) || defined (_MAC_OS)
    static unsigned int ncpu = 0;
    
    if (0 >= ncpu)
    {
        size_t len;
        len = sizeof(ncpu);
        sysctlbyname ("hw.physicalcpu",&ncpu,&len,NULL,0);
        
        if (0 >= ncpu) {
            sysctlbyname ("hw.ncpu",&ncpu,&len,NULL,0);
        }
    }
    
	return ncpu;
#else
	return 1;
#endif
}
    
    SN_U32	snOS_GetLogicalCPUNum ()
    {
#if defined(_IOS) || defined (_MAC_OS)
        static unsigned int ncpu = 0;
        
        if (0 >= ncpu)
        {
            size_t len;
            len = sizeof(ncpu);
            sysctlbyname ("hw.logicalcpu",&ncpu,&len,NULL,0);
            
            if (0 >= ncpu) {
                sysctlbyname ("hw.ncpu",&ncpu,&len,NULL,0);
            }
        }
        
        return ncpu;
#else
        return snOS_GetCPUNum();
#endif
    }

#if defined(_IOS) || defined(_MAC_OS)
    typedef void (* snSigPipe) (int);
    snSigPipe g_SIGPIPEHandler = NULL;
    
    void snReceiveSignal(int signal) {
        SNLOGI("snReceiveSignal signal %d", signal);
    }
    
    SN_S32 snMoreUNIXErrno(SN_S32 result)
    {
        int err;
        
        err = 0;
        if (result < 0) {
            err = errno;
        }
        return err;
    }
    
    SN_S32 snMoreUNIXIgnoreSIGPIPE(void)
    {
        int err;
        struct sigaction signalState;
        
        err = sigaction(SIGPIPE, NULL, &signalState);
        err = snMoreUNIXErrno(err);
        if (err == 0) {
            if (NULL != signalState.sa_handler) {
                g_SIGPIPEHandler = signalState.sa_handler;
            }
            
            SNLOGI("iOS snMoreUNIXIgnoreSIGPIPE Old sa_handler:%p", g_SIGPIPEHandler);
            signalState.sa_handler = snReceiveSignal;
            
            err = sigaction(SIGPIPE, &signalState, NULL);
            err = snMoreUNIXErrno(err);
        }
        
        return err;
    }
    
    SN_S32 snMoreUNIXRecoverySIGPIPE(void)
    {
        int err;
        struct sigaction signalState;
        
        err = sigaction(SIGPIPE, NULL, &signalState);
        err = snMoreUNIXErrno(err);
        if (err == 0) {
            if (NULL == signalState.sa_handler) {
                return 0;
            }
            else if (snReceiveSignal != signalState.sa_handler) {
                SNLOGI("iOS snMoreUNIXRecoverySIGPIPE return by sa_handler have be changed to:%p", signalState.sa_handler);
                return -1;
            }
            signalState.sa_handler = g_SIGPIPEHandler;
            SNLOGI("iOS snMoreUNIXRecoverySIGPIPE return OK by sa_handler:%p", signalState.sa_handler);
            
            err = sigaction(SIGPIPE, &signalState, NULL);
            err = snMoreUNIXErrno(err);
        }
        
        return err;
    }
    
    float snOS_GetDeviceScale()
    {
#ifdef _IOS
        static float scale = 0;
        
        if (scale > 0.9) {
            return scale;
        }
        
        float scaleRun = [[UIScreen mainScreen] scale];
        if ([[UIScreen mainScreen] respondsToSelector:@selector(nativeScale)]) {
            scaleRun = [UIScreen mainScreen].nativeScale;
            if (scaleRun > 2.0 && scaleRun <= 2.5)
            {
                scaleRun = 2.0;
                
            }
            else if (scaleRun > 2.5 && scaleRun < 3.0)
            {
                scaleRun = 3.0;
            }
        }
        
        scale = scaleRun;
        return scale;
#endif
        return 1;
    }
    
    float cpu_usage()
    {
        kern_return_t kr;
        task_info_data_t tinfo;
        mach_msg_type_number_t task_info_count;
        
        task_info_count = TASK_INFO_MAX;
        kr = task_info(mach_task_self(), TASK_BASIC_INFO, (task_info_t)tinfo, &task_info_count);
        if (kr != KERN_SUCCESS) {
            return -1;
        }
        
        task_basic_info_t      basic_info;
        thread_array_t         thread_list;
        mach_msg_type_number_t thread_count;
        
        thread_info_data_t     thinfo;
        mach_msg_type_number_t thread_info_count;
        
        thread_basic_info_t basic_info_th;
        uint32_t stat_thread = 0; // Mach threads
        
        basic_info = (task_basic_info_t)tinfo;
        
        // get threads in the task
        kr = task_threads(mach_task_self(), &thread_list, &thread_count);
        if (kr != KERN_SUCCESS) {
            return -1;
        }
        if (thread_count > 0)
            stat_thread += thread_count;
        
        long tot_sec = 0;
        long tot_usec = 0;
        float tot_cpu = 0;
        int j;
        
        for (j = 0; j < thread_count; j++)
        {
            thread_info_count = THREAD_INFO_MAX;
            kr = thread_info(thread_list[j], THREAD_BASIC_INFO,
                             (thread_info_t)thinfo, &thread_info_count);
            if (kr != KERN_SUCCESS) {
                return -1;
            }
            
            basic_info_th = (thread_basic_info_t)thinfo;
            
            if (!(basic_info_th->flags & TH_FLAGS_IDLE)) {
                tot_sec = tot_sec + basic_info_th->user_time.seconds + basic_info_th->system_time.seconds;
                tot_usec = tot_usec + basic_info_th->system_time.microseconds + basic_info_th->system_time.microseconds;
                tot_cpu = tot_cpu + basic_info_th->cpu_usage / (float)TH_USAGE_SCALE * 100.0;
            }
            
        } // for each thread
        
        kr = vm_deallocate(mach_task_self(), (vm_offset_t)thread_list, thread_count * sizeof(thread_t));
		if (kr != KERN_SUCCESS) {
			return -1;
		}
        
        return tot_cpu;
    }
#endif
    
SN_U32	snOS_GetCPUFrequency ()
{
#ifdef _WIN32
#ifndef WINCE
    LARGE_INTEGER f;            
    QueryPerformanceFrequency(&f);
	return (int)f.LowPart;
#endif
#endif
	return 0;
}

void snOS_GetCPUType(char *cpuString,int length)
{
#if defined _METRO
#elif (defined _WIN32 || defined _WIN64) && (!defined WINCE)
	int cpuinfo[4] = {-1};
	__cpuid(cpuinfo, 0);
	memset(cpuString, 0, length);
	const DWORD id = 0x80000002; 
	for(DWORD t = 0 ; t < 3; t++)
	{
		__cpuid(cpuinfo, id + t);
		memcpy(cpuString + 16 * t, cpuinfo, 16);
	}
#endif	// (_WIN32 || _WIN64) && !WINCE
}

#if defined(_LINUX) 
struct cpu_info {
    long unsigned utime, ntime, stime, itime;
    long unsigned iowtime, irqtime, sirqtime;
};

static unsigned int gCpuSys, gCpuUsr, gOsDelay, gOsGrid;
static SN_BOOL*     gCpuGoing;
static int  set_cpu_values(cpu_info& inInfo)
{
	FILE* pFile = NULL;
	pFile = fopen("/proc/stat", "r");
	if(!pFile)
	{
		SNLOGE("Can not open the /proc/stat and set the values\n");
		return -1;
	}
    int nRet = fscanf(pFile, "cpu  %lu %lu %lu %lu %lu %lu %lu", &inInfo.utime, &inInfo.ntime, &inInfo.stime,
            &inInfo.itime, &inInfo.iowtime, &inInfo.irqtime, &inInfo.sirqtime);
    fclose(pFile);

	if(!nRet)
	{
		SNLOGE("failed to fscanf");
		return -1;
	}

	return 0;
}
static void get_cpu()
{
	cpu_info old_cpu, new_cpu;
	long unsigned total_delta_time;
	memset(&old_cpu, 0, sizeof(cpu_info));
	memset(&new_cpu, 0, sizeof(cpu_info));
	int nOk = set_cpu_values(old_cpu);
	if(nOk < 0)
		return ;
	if(gOsDelay <=0)
		gOsDelay = 1;
	
	int nTimes = gOsDelay * 1000/ gOsGrid;
	for(int i=0; i < nTimes; i++)
	{
		if(*gCpuGoing == SN_FALSE)
			return;
		usleep(gOsGrid*1000);	
	}
	nOk = set_cpu_values(new_cpu);
	if(nOk < 0)
		return;
	
	total_delta_time = (new_cpu.utime + new_cpu.ntime + new_cpu.stime + new_cpu.itime
                        + new_cpu.iowtime + new_cpu.irqtime + new_cpu.sirqtime)
                     - (old_cpu.utime + old_cpu.ntime + old_cpu.stime + old_cpu.itime
                        + old_cpu.iowtime + old_cpu.irqtime + old_cpu.sirqtime);
	if(total_delta_time != 0)
	{
		gCpuUsr = ((new_cpu.utime + new_cpu.ntime) - (old_cpu.utime + old_cpu.ntime)) * 100  / total_delta_time;
		gCpuSys = ((new_cpu.stime ) - (old_cpu.stime)) * 100 / total_delta_time;
	}
	else
	{
		gCpuUsr = -1;
		gCpuSys = -1;
	}
}

static pthread_once_t           gCpuUThread = PTHREAD_ONCE_INIT;
#endif

#ifdef _WIN32
#ifndef _METRO
#ifndef WINCE
double m_fOldCPUIdleTime;
double m_fOldCPUKernelTime;
double m_fOldCPUUserTime;

double FileTimeToDouble(FILETIME &filetime)
{
	return (double)(filetime.dwHighDateTime * 4.294967296E9) + (double)filetime.dwLowDateTime;
}
BOOL Initialize() 
{
	FILETIME ftIdle, ftKernel, ftUser;
	BOOL flag = FALSE;
	if (flag = GetSystemTimes(&ftIdle, &ftKernel, &ftUser))
	{
		m_fOldCPUIdleTime = FileTimeToDouble(ftIdle);
		m_fOldCPUKernelTime = FileTimeToDouble(ftKernel);
		m_fOldCPUUserTime = FileTimeToDouble(ftUser);
	}
	return flag;
}
int GetCPUUseRate()
{
	Sleep(20);
	int nCPUUseRate = -1;
	FILETIME ftIdle, ftKernel, ftUser;
	if (GetSystemTimes(&ftIdle, &ftKernel, &ftUser))
	{
			double fCPUIdleTime = FileTimeToDouble(ftIdle);
			double fCPUKernelTime = FileTimeToDouble(ftKernel);
			double fCPUUserTime = FileTimeToDouble(ftUser);
			nCPUUseRate= (int)((fCPUKernelTime - m_fOldCPUKernelTime + fCPUUserTime - m_fOldCPUUserTime - fCPUIdleTime + m_fOldCPUIdleTime)*100 / (fCPUKernelTime - m_fOldCPUKernelTime + fCPUUserTime - m_fOldCPUUserTime));
			m_fOldCPUIdleTime = fCPUIdleTime;
			m_fOldCPUKernelTime = fCPUKernelTime;
			m_fOldCPUUserTime = fCPUUserTime;
	}
	return nCPUUseRate;
}

//------------------------------------------------------------------------------------------------------------------
int getcpuload()
{
  Sleep(500);
  static PDH_STATUS            status;
  static PDH_FMT_COUNTERVALUE  value;
  static HQUERY                query;
  static HCOUNTER              counter;
  static DWORD                 ret;
  static char                  runonce=1;
  int                         cput=0;

  if(runonce)
  {
    status = PdhOpenQuery(NULL, 0, &query);
    if(status != ERROR_SUCCESS)
    {
      printf("PdhOpenQuery() ***Error: 0x%X\n",status);
      return 0;
    }

    PdhAddCounter(query, TEXT("\\Processor(_Total)\\% Processor Time"),0,&counter); // A total of ALL CPU's in the system
    //PdhAddCounter(query, TEXT("\\Processor(0)\\% Processor Time"),0,&counter);    // For systems with more than one CPU (Cpu0)
    //PdhAddCounter(query, TEXT("\\Processor(1)\\% Processor Time"),0,&counter);    // For systems with more than one CPU (Cpu1)
    runonce=0;
    PdhCollectQueryData(query); // No error checking here
    return 0;
  }

  status = PdhCollectQueryData(query);
  if(status != ERROR_SUCCESS)
  {
    printf("PhdCollectQueryData() ***Error: 0x%X\n",status);
    if(status==PDH_INVALID_HANDLE) 
      printf("PDH_INVALID_HANDLE\n");
    else if(status==PDH_NO_DATA)
      printf("PDH_NO_DATA\n");
    else
      printf("Unknown error\n");
    return 0;
  }

  status = PdhGetFormattedCounterValue(counter, PDH_FMT_DOUBLE | PDH_FMT_NOCAP100 ,&ret, &value);
  if(status != ERROR_SUCCESS)
  {
    printf("PdhGetFormattedCounterValue() ***Error: 0x%X\n",status);
    return 0;
  }
  cput = (int)value.doubleValue;
  
  return cput;
}

#endif
#endif // _METRO
#endif

	int snOS_GetCpuUsage(SN_U32* pOutSys, SN_U32* pOutUsr, SN_BOOL* pInGoing, SN_U32 inDelay, SN_U32 inGrid)
{
	int nOk = 0;
#if defined(_LINUX)
	if(!pOutSys || !pOutUsr || !pInGoing)
		return -1;

	gOsDelay = inDelay;
	gOsGrid = inGrid;
	gCpuGoing = pInGoing;
	//nOk = pthread_once(&gCpuUThread, get_cpu);
        get_cpu();// the time two time insnke this me.hod must over 1 seconds
	gCpuUThread = PTHREAD_ONCE_INIT;
	*pOutSys = gCpuSys;
	*pOutUsr = gCpuUsr;
#endif

#if defined(_WIN32)
#ifndef _METRO
#ifndef WINCE
	if (!Initialize())
	{
		printf("Error! %d\n", GetLastError());
		return -1;
	}
	else
	{	
			*pOutSys = 0;
			*pOutUsr = getcpuload();
	}
#endif
#endif // _METRO
#endif
    
    
#if defined (_IOS) || defined (_MAC_OS)
    
    if (NULL == pOutSys || NULL == pOutUsr) {
        return -1;
    }
    
    int nLogicalCpu = snOS_GetLogicalCPUNum();
	int nCpu = snOS_GetCPUNum();
    
    if (0 == nCpu) {
        nCpu = nLogicalCpu = 1;
    }
    
    processor_info_array_t startCpuInfo = 0;
    natural_t numCPUsU = 0U;
    mach_msg_type_number_t numStartCpuInfo = 0;
    mach_msg_type_number_t numEndCpuInfo = 0;
    
    kern_return_t err = host_processor_info(mach_host_self(), PROCESSOR_CPU_LOAD_INFO, &numCPUsU, &startCpuInfo, &numStartCpuInfo);
    if((KERN_SUCCESS != err) || (0 == startCpuInfo)) {
        return -1;
    }
    
    useconds_t uTimeToSleep = 0;
    if (0 >= inDelay) {
        uTimeToSleep = 100; // at least 100ms
    }
    else {
        uTimeToSleep = inDelay * 1000; // millisecond
    }
    
    if ((0 >= inGrid) || (inGrid >= uTimeToSleep) || (NULL == pInGoing)) {
        usleep(uTimeToSleep * 1000);
    }
    else {
        int nTimes = uTimeToSleep / inGrid;
        for (int i=0; i < nTimes; ++i)
        {
            if (*pInGoing == SN_FALSE)
                break;
            
            usleep(inGrid * 1000);
        }
    }
    
    processor_info_array_t endCpuInfo = 0;
    err = host_processor_info(mach_host_self(), PROCESSOR_CPU_LOAD_INFO, &numCPUsU, &endCpuInfo, &numEndCpuInfo);
    if ((KERN_SUCCESS != err) || (0 == endCpuInfo)) {
        
        if (startCpuInfo) {
            size_t startCpuInfoSize = sizeof(integer_t) * numStartCpuInfo;
            vm_deallocate(mach_task_self(), (vm_address_t)startCpuInfo, startCpuInfoSize);
        }
        
        return -1;
    }
    
	float userUsed = 0;
    float systemUsed = 0;
    float totalCpu = 0;
    
	for (unsigned i = 0U; i < nLogicalCpu; ++i) {
        
        userUsed += endCpuInfo[(CPU_STATE_MAX * i) + CPU_STATE_USER] - startCpuInfo[(CPU_STATE_MAX * i) + CPU_STATE_USER];
        
        systemUsed += (endCpuInfo[(CPU_STATE_MAX * i) + CPU_STATE_SYSTEM] - startCpuInfo[(CPU_STATE_MAX * i) + CPU_STATE_SYSTEM])
        + (endCpuInfo[(CPU_STATE_MAX * i) + CPU_STATE_NICE] - startCpuInfo[(CPU_STATE_MAX * i) + CPU_STATE_NICE]);
        
        totalCpu += userUsed + systemUsed + (endCpuInfo[(CPU_STATE_MAX * i) + CPU_STATE_IDLE] - startCpuInfo[(CPU_STATE_MAX * i) + CPU_STATE_IDLE]);
	}
    
    if (startCpuInfo) {
        size_t startCpuInfoSize = sizeof(integer_t) * numStartCpuInfo;
        vm_deallocate(mach_task_self(), (vm_address_t)startCpuInfo, startCpuInfoSize);
    }
    
    if (endCpuInfo) {
        size_t endCpuInfoSize = sizeof(integer_t) * numEndCpuInfo;
        vm_deallocate(mach_task_self(), (vm_address_t)endCpuInfo, endCpuInfoSize);
    }
    
	*pOutSys = (systemUsed / totalCpu) * 100 * nLogicalCpu / nCpu;
	*pOutUsr = (userUsed / totalCpu) * 100 * nLogicalCpu / nCpu;
    
#endif

	return nOk;
}

#ifdef _LINUX_ANDROID
static long unsigned g_nMaxCpuFrequency = 0;
#endif	// _LINUX_ANDROID
SN_BOOL snOS_GetCurrentCpuFrequencyPercent(SN_U32 * pPercent)
{
#ifdef _LINUX_ANDROID
	FILE * pFile = NULL;
	if(g_nMaxCpuFrequency == 0)
	{
		pFile = fopen("/sys/devices/system/cpu/cpu0/cpufreq/cpuinfo_max_freq", "r");
		if(!pFile)
		{
			SNLOGE("failed to open the cpuinfo_max_freq");
			return SN_FALSE;
		}

		int nRet = fscanf(pFile, "%lu", &g_nMaxCpuFrequency);
		fclose(pFile);

		if(!nRet)
		{
			SNLOGE("failed to fscanf");
			return SN_FALSE;
		}

		pFile = NULL;
	}

	pFile = fopen("/sys/devices/system/cpu/cpu0/cpufreq/scaling_cur_freq", "r");
	if(!pFile)
	{
		SNLOGE("failed to open the scaling_cur_freq");
		return SN_FALSE;
	}

	long unsigned nCurrentCpuFrequency;
	int nRet = fscanf(pFile, "%lu", &nCurrentCpuFrequency);
	fclose(pFile);

	if(!nRet)
	{
		SNLOGE("failed to fscanf");
		return SN_FALSE;
	}

	SN_U64 ullCurrentCpuFrequency = nCurrentCpuFrequency;
	if(pPercent)
	{
		*pPercent = ullCurrentCpuFrequency * 10000 / g_nMaxCpuFrequency;
//		SNLOGI("percent %d, current freq %d, max freq %d", *pPercent, nCurrentCpuFrequency, g_nMaxCpuFrequency);
	}

	return SN_TRUE;
#else
	return SN_FALSE;
#endif	// _LINUX_ANDROID
}

//#if 1
SN_S64 snOS_GetUTC(void)
{
#ifdef _METRO
    time_t rawtime;

    time(&rawtime);

	return rawtime;
#else //_METRO
#if defined(_LINUX_ANDROID) || defined(_WIN32) || defined (_IOS) || defined (_MAC_OS)
#ifdef WINCE
	  SYSTEMTIME st;
	  GetSystemTime(&st);

	  long long t;
	  SystemTimeToFileTime(&st , (FILETIME *)&t);

	  return t;
#else 
    time_t curr = time(&curr);
    return curr;
#endif // WINCE
#else
  return -1;
#endif /// LINUX_WIN32
#endif //_METRO
}
SN_BOOL snOS_GetUTCFromNetwork(char *host, time_t * ptime, char*pPort)
{
#ifdef _METRO
	return SN_FALSE;
#else //_METRO
#ifdef _WIN32
  WORD wVersionRequested;
  WSADATA wsaData;
  int err;
  wVersionRequested = MAKEWORD( 2, 2 );
  err = WSAStartup( wVersionRequested, &wsaData );

  if( err != 0 )
    return SN_FALSE;
#endif
  int sockfd, numbytes;
  char buffer[101];
  struct addrinfo info;
  memset(&info, 0, sizeof(info));
  info.ai_family = AF_INET;
  info.ai_socktype = SOCK_STREAM;
  info.ai_protocol = 6;
  struct addrinfo * ptr_ret;
  //char * str_host = host;
  int ret = getaddrinfo(host , pPort , &info , &ptr_ret );
  if(ret != 0){
#ifdef _WIN32
  WSACleanup();
#endif // _WIN32
    return SN_FALSE;
  }
  struct addrinfo * ptr_entry = ptr_ret;
  if((sockfd = socket(ptr_entry->ai_family, ptr_entry->ai_socktype, ptr_entry->ai_protocol)) == -1){
#ifdef _WIN32
  WSACleanup();
#endif
    freeaddrinfo(ptr_ret);
    return SN_FALSE;
  }
  unsigned long ul = 1;
#ifdef _WIN32
  ioctlsocket(sockfd, FIONBIO, (unsigned long*)&ul);
#else
  ioctl(sockfd, FIONBIO, &ul);
#endif
  struct timeval timeout;
  int error, len = sizeof(int);
  fd_set   r;
  FD_ZERO(&r);
  FD_SET(sockfd,   &r);
  timeout.tv_sec = 2;
  timeout.tv_usec =0;
  if(connect(sockfd, ptr_entry->ai_addr, ptr_entry->ai_addrlen) == -1){
    if(select(sockfd + 1 , 0 , &r , 0 , &timeout ) > 0){
#ifdef _WIN32
      getsockopt(sockfd, SOL_SOCKET, SO_ERROR, (char*)&error, &len);
#else
      getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &error, (socklen_t *)&len);
#endif
      if(error != 0){
#ifdef _WIN32
        WSACleanup();
        closesocket(sockfd);
#else
        close(sockfd);
#endif
        freeaddrinfo(ptr_ret);
        return SN_FALSE;
      }
    }
  }
  ul = 0;
#ifdef _WIN32
  ioctlsocket(sockfd, FIONBIO, (unsigned long*)&ul);
#else
  ioctl(sockfd, FIONBIO, &ul);
#endif
  freeaddrinfo(ptr_ret);
  FD_ZERO(&r);
  FD_SET(sockfd,   &r);
  timeout.tv_sec = 2;
  timeout.tv_usec = 0;
  if(select(sockfd + 1 , 0 , &r , 0 , &timeout ) <= 0){
#ifdef _WIN32
    WSACleanup();
    closesocket(sockfd);
#else
    close(sockfd);
#endif
    return SN_FALSE;
  }
  timeout.tv_sec = 1;
  timeout.tv_usec = 0;
  setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout,sizeof(struct timeval));
  snOS_SendPacket(sockfd);
  if((numbytes = recv(sockfd, buffer, 100, 0)) == -1){
#ifdef _WIN32
    WSACleanup();
    closesocket(sockfd);
#else
    close(sockfd);
#endif
    return SN_FALSE;
  }
  if(numbytes < 20){
#ifdef _WIN32
    WSACleanup();
    closesocket(sockfd);
#else
    close(sockfd);
#endif
    return SN_FALSE;
  }else{
    char szTemp[100]="";
    char *pdest;
    pdest = strchr( buffer, ' ' );
    strcpy(szTemp, pdest+1);
    struct tm tm1;
    memset((void*)&tm1, 0, sizeof(struct tm));
    sscanf(szTemp, "%2d-%2d-%2d %2d:%2d:%2d",
        &tm1.tm_year, &tm1.tm_mon, &tm1.tm_mday, &tm1.tm_hour, &tm1.tm_min, &tm1.tm_sec);
    tm1.tm_year += (2000 - 1900);
    tm1.tm_mon --;
    tm1.tm_isdst=-1;
#ifdef WINCE
	*ptime = ::mktime(&tm1);
#else
	*ptime = mktime(&tm1);
#endif
  }
#ifdef _WIN32
  WSACleanup();
  closesocket(sockfd);
#else
  close(sockfd);
#endif
  return SN_TRUE;

#endif // _METRO 
}

SN_VOID snOS_SendPacket(int fd)
{
#ifdef _METRO
#else //_METRO
#define NTPFRAC(x) (4294 * (x) + ((1981 * (x))>>11))
#define USEC(x) (((x) >> 12) - 759 * ((((x) >> 10) + 32768) >> 16))
  unsigned int data[12];
  struct timeval now;
  if (sizeof(data) != 48)
    return;
  memset((char*)data, 0, sizeof(data));
  data[0] = htonl((0 << 30) | (3 << 27) | (3 << 24)
      | (0 << 16) | (4 << 8) | (-6 & 0xff));
  data[1] = htonl(1<<16);  /* Root Delay (seconds) */
  data[2] = htonl(1<<16);  /* Root Dispersion (seconds) */
#ifdef _WIN32
  struct tm tm;
  SYSTEMTIME wtm;
  GetLocalTime(&wtm);
  tm.tm_year = wtm.wYear - 1900;
  tm.tm_mon = wtm.wMonth - 1;
  tm.tm_mday = wtm.wDay;
  tm.tm_hour = wtm.wHour;
  tm.tm_min = wtm.wMinute;
  tm.tm_sec = wtm.wSecond;
  tm.tm_isdst = -1;
#ifdef WINCE
  now.tv_sec = (long)::mktime(&tm);
#else
  now.tv_sec = (long)mktime(&tm);
#endif
  now.tv_usec = wtm.wMilliseconds * 1000;
#else
  gettimeofday(&now, NULL);
#endif
  data[10] = htonl(now.tv_sec + 0x83aa7e80); /* Transmit Timestamp coarse */
  data[11] = htonl(NTPFRAC(now.tv_usec));  /* Transmit Timestamp fine   */
#ifdef _WIN32
  send(fd, (char*)data, 48, 0);
#else
  send(fd, data, 48, 0);
#endif
#endif //_METRO
}

SN_S32 snOS_GetUTCServer(SN_PCHAR server, SN_PCHAR sPort)
{
#ifdef _WIN32
  return 1;
#else	// _WIN32
#ifndef _SNNDBG
  SN_CHAR BufLine[256], Index = '0';
  SN_PCHAR target;

#if defined (_IOS) || defined (_MAC_OS)
    char szTmp[512];
    snOS_GetAppFolder(szTmp, 256);
    strcat(szTmp, "timeServer.cfg");
    
    FILE * cfgFD = fopen(szTmp, "r");
#else
    FILE * cfgFD = fopen("/data/local/tmp/timeServer.cfg", "r");
#endif
  if(cfgFD == NULL){
    return 1;
  }
  while((fgets(BufLine, 256, cfgFD)) != NULL){
    if(!strcmp(BufLine, "\n"))
      continue;
    BufLine[strlen(BufLine) - 1] = '\0';
    target = strstr(BufLine, "target=");
    if(target != NULL){
      Index=*(target + 7);
    }

    if(!strncmp((SN_PCHAR)&BufLine[0], (SN_PCHAR)&Index, 1)){
      char * portIndex = strstr(BufLine, ":");
      memcpy((void*)sPort, portIndex + 1, strlen(portIndex) - 1);
      *(sPort + strlen(portIndex) - 1) = '\0';
      memcpy((void*)server, BufLine + 2, strlen(BufLine) - 1 - strlen(portIndex));
      fclose(cfgFD);
      return 0;
    }
  }
  fclose(cfgFD);
#endif	// _SNNDBG

  return 1;
#endif // _WIN32
}

const char* snOS_GetOSName()
{
	const char* pName = NULL;
#if defined(_LINUX)
	pName = "Linux";
#   if defined(_LINUX_ANDROID) || defined(__SN_NDK__)
	pName = "Android";
#   endif // _LINUX_ANDROID || __SN_NDK__
#elif defined(_IOS)
	pName = "IPhone_OS";
#elif defined(_MAC_OS)
	pName="Mac_OS";
#elif defined(_WIN32)
	pName= "Windows_PC";
#   if defined(_WIN32_WCE)
	pName = "Windows_CE";
#   elif defined(_WIN32_MOBILE)
	pName= "Windows_MB";
#   elif defined(_METRO)
	pName = "Windows_Phone";
#   endif // _WIN32
#else
	pName = "Unkown"
#endif
   return pName;
}
    
    
SN_S32 snOS_GetAppResourceFolder (SN_PTCHAR pFolder, SN_U32 nSize)
{
    int nRet = -1;
    
    if(!pFolder)
        return nRet;
    
#if (defined _IOS) || (defined _MAC_OS)
    
    NSString *resourcePath = [[NSBundle mainBundle] resourcePath];
    //printf("resourcePath is  %s\n", [resourcePath UTF8String]);
    
    strcpy(pFolder, [resourcePath UTF8String]);
    
    nRet = 0;
    
#endif
    
    return nRet;
}

int snwcslen(const snwchar_t* str)
{   
	int len = 0;
	while (*str != '\0') {
		str++;
		len++;
	}
	return len;
} 

int SN_UnicodeToUTF8(const snwchar_t *szSrc, const int nSrcLen, char*strDst, const int nDstLen) 
{

	int is = 0, id = 0;
	const snwchar_t *ps = szSrc;
	unsigned char *pd = (unsigned char*)strDst;

	if (nDstLen <= 0)
		return 0;

	for (is = 0; is < nSrcLen; is++) {
		if (BYTE_1_REP > ps[is]) { /* 1 byte utf8 representation */
			if (id + 1 < nDstLen) {
				pd[id++] = (unsigned char)ps[is];
			} else {
				pd[id] = '\0'; /* Terminate string */
				return 0; /* ERROR_INSUFFICIENT_BUFFER */
			}
		} else if (BYTE_2_REP > ps[is]) {
			if (id + 2 < nDstLen) {
				pd[id++] = (unsigned char)(ps[is] >> 6 | 0xc0);
				pd[id++] = (unsigned char)((ps[is] & 0x3f) | 0x80);
			} else {
				pd[id] = '\0'; /* Terminate string */
				return 0; /* ERROR_INSUFFICIENT_BUFFER */
			}
		} else /*if (BYTE_3_REP > ps[is])*/ { /* 3 byte utf8 representation */
			if (id + 3 < nDstLen) {
				pd[id++] = (unsigned char)(ps[is] >> 12 | 0xe0);
				pd[id++] = (unsigned char)(((ps[is] >> 6)  & 0x3f) | 0x80);
				pd[id++] = (unsigned char)((ps[is] & 0x3f) | 0x80);
			} else {
				pd[id] = '\0'; /* Terminate string */
				return 0; /* ERROR_INSUFFICIENT_BUFFER */
			}
		//} else if (BYTE_4_REP > ps[is]) { /* 4 byte utf8 representation */
		//	if (id + 4 < nDstLen) {
		//		pd[id++] = 0x00;//(unsigned char)((SN_U32(ps[is]) >> 18) | 0xf0); modify for coverity by nick
		//		pd[id++] = (unsigned char)(((ps[is] >> 12)  & 0x3f) | 0x80);
		//		pd[id++] = (unsigned char)(((ps[is] >> 6)  & 0x3f) | 0x80);
		//		pd[id++] = (unsigned char)((ps[is] & 0x3f) | 0x80);
		//	} else {
		//		pd[id] = '\0'; /* Terminate string */
		//		return 0; /* ERROR_INSUFFICIENT_BUFFER */
		//	}

		} /* @todo Handle surrogate pairs */
	}

	pd[id] = '\0'; /* Terminate string */
	return id; /* This value is in bytes */
}

SN_VOID ConvertUnicodeToUTF8( SN_CHAR *pSourceBuffer,  SN_U32 uSize, SN_CHAR **ppDestBuffer, SN_U32 *pDestSize )
{
	
	if( *ppDestBuffer ){
		delete[] *ppDestBuffer;
		*ppDestBuffer = NULL;
	}
	
	SN_CHAR *pDBuf = *ppDestBuffer = new SN_CHAR[uSize+1];
	memcpy(pDBuf, (SN_CHAR*)pSourceBuffer, uSize);
	pDBuf[uSize] = '\0';

	if( pDBuf[0] == (SN_CHAR)0xff && pDBuf[1] == (SN_CHAR)0xfe)
	{
#if defined _WIN32

		int size = WideCharToMultiByte(CP_UTF8, 0, (LPCWSTR)pDBuf, -1, NULL, NULL, NULL, NULL );

		SN_CHAR *content_temp = NULL;
		content_temp = new SN_CHAR[ size + 1 ];
		memset( content_temp , 0 , size + 1 );

		WideCharToMultiByte(CP_UTF8, 0, (LPCWSTR)pDBuf, -1, content_temp, size, NULL, NULL );
		*pDestSize = size; 

#else
		SN_CHAR *content_temp = new SN_CHAR[ uSize + 1 ];
		memset( content_temp , 0 , uSize + 1 );

		int SrcLen = snwcslen((snwchar_t *)pDBuf);

		SN_UnicodeToUTF8((snwchar_t *)pDBuf, SrcLen, content_temp, uSize);
		*pDestSize = uSize; 

#endif
		delete []pDBuf;
		pDBuf = content_temp;
	}

}
