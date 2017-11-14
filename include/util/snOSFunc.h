	/************************************************************************
	*																		*
	*		Snail, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		snOSFunc.h

	Contains:	snOSFunc.header file

	Written by:	David

	Change History (most recent first):
	2009-04-01	David			Create file

*******************************************************************************/

#ifndef __snOSFunc_H__
#define __snOSFunc_H__

#include "snType.h"

#ifdef __cplusplus
    extern "C" {
#endif /* __cplusplus */

EXPORT void			snOS_Sleep (SN_U32 nTime);
EXPORT void			snOS_NanoSleep (SN_U64 nTime);
// this function is exit-able but not accurate, please use it carefully
EXPORT void			snOS_SleepExitable (SN_U32 nTime, SN_BOOL * pbExit);
EXPORT SN_U64			snOS_GetSysTime (void);
EXPORT SN_U64			snOS_GetNanoTime (void);
EXPORT SN_U64			snGetSysTime (void);
EXPORT void			snOS_SetTimePeriod(SN_BOOL bStartOrEnd, SN_U32 nPeriod);

EXPORT SN_S64			snOS_GetUTC (void);
#if defined(_LINUX_ANDROID) || defined(_WIN32)
EXPORT SN_BOOL     snOS_GetUTCFromNetwork(char *host, time_t * ptime, char*port);
#endif
EXPORT SN_VOID     snOS_SendPacket(int fd);
EXPORT SN_S32      snOS_GetUTCServer(SN_PCHAR server, SN_PCHAR sPort);

EXPORT SN_U32			snOS_GetThreadTime (SN_PTR	hThread);

EXPORT SN_U32      snOS_GetModuleFileName(SN_PTR, SN_PTCHAR,  SN_U32);

EXPORT SN_U32          snOS_GetAppResourcePath (SN_PTCHAR outPathAndName, SN_PTCHAR inResourceName, SN_U32 nSize);
EXPORT SN_U32			snOS_GetAppFolder (SN_PTCHAR pFolder, SN_U32 nSize);
	
EXPORT SN_U32			snOS_GetPluginModuleFolder(SN_PTCHAR pFolder, SN_U32 nSize);
EXPORT SN_U32			snOS_GetBundleFolderByIdentifier(SN_PTCHAR pIdentifier, SN_PTCHAR pFolder, SN_U32 nSize);
EXPORT SN_U32			snOS_GetApplicationID(SN_PTCHAR pAppID, SN_U32 nSize);

EXPORT SN_U32          snOS_GetLogicalCPUNum();
EXPORT SN_U32			snOS_GetCPUNum(void);
EXPORT SN_U32			snOS_GetCPUFrequency (void);
EXPORT void			snOS_GetCPUType(char *cpuString,int length);

EXPORT SN_S32 			snOS_EnableDebugMode (int nshowLog);

EXPORT SN_U32			snOS_Log (char* pText);

EXPORT const char*	    snOS_GetOSName();
		
// outSys means the system cpu usage, outUsr means the user cpu usage, generally speaking cpu usage should use
// outSys+outUsr. These output values have been x100. inDelay use second. InDelay suggestion value is 1
// in order to solve exiting this function at least inDelay second problem, I have add the pInGoing parameters, the user can
// set it with pInGoing = false to return from this function with at least 	inGrid (ms)	
EXPORT int				snOS_GetCpuUsage(SN_U32* pOutSys, SN_U32* pOutUsr, SN_BOOL* pInGoing, SN_U32 inDelay, SN_U32 inGrid);
// *pPercent: percent * 100, for example 8512 means 85.12%
EXPORT SN_BOOL			snOS_GetCurrentCpuFrequencyPercent(SN_U32 * pPercent);
    
EXPORT SN_S32			snOS_GetAppResourceFolder (SN_PTCHAR pFolder, SN_U32 nSize);


#if !defined __SN_WCHAR_T__
#define __SN_WCHAR_T__
typedef unsigned short snwchar_t;
#endif
#define BYTE_1_REP          0x80   
#define BYTE_2_REP          0x800 
#define BYTE_3_REP          0x10000 
#define BYTE_4_REP          0x200000 

#define SIGMASK_1_1         0x80
#define SIGMASK_2_1         0xe0
#define SIGMASK_3_1         0xf0
#define SIGMASK_4_1         0xf8

EXPORT SN_S32			snwcslen(const snwchar_t* str);
EXPORT SN_S32			SN_UnicodeToUTF8(const snwchar_t *szSrc, const int nSrcLen, char*strDst, const int nDstLen);
EXPORT SN_VOID			ConvertUnicodeToUTF8(SN_CHAR *pSourceBuffer,  SN_U32 uSize, SN_CHAR **ppDestBuffer, SN_U32 *pDestSize);
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif //__snOSFunc_H__
