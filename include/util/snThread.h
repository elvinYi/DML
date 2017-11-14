	/************************************************************************
	*																		*
	*		Snail, Inc. Confidential and Proprietary, 2003-2009			*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		snThread.h

	Contains:	memory operator function define header file

	Written by:	David

	Change History (most recent first):
	2009-03-10		David			Create file

*******************************************************************************/

#ifndef __snread_H__
#define __snread_H__

#include "snIndex.h"
#include "snType.h"

#ifdef __cplusplus
    extern "C" {
#endif /* __cplusplus */

typedef void * snThreadHandle;

typedef SN_U32 (* snThreadProc) (SN_PTR pParam);

/**
 * Thread create function
 * \param pHandle [in/out] the handle of thread
 * \param nID [in] the id of thread
 * \param fProc [in] the function entry pointer
 * \param pParam [in] the parameter in call function.
 */
typedef int (* SNTDThreadCreate) (void ** pHandle, int * pID, void * fProc, void * pParam);

typedef enum snThreadPriority
{
	SN_THREAD_PRIORITY_TIME_CRITICAL	= 0x0,
	SN_THREAD_PRIORITY_HIGHEST			= 0x1,
	SN_THREAD_PRIORITY_ABOVE_NORMAL		= 0x2,
	SN_THREAD_PRIORITY_NORMAL			= 0x3,
	SN_THREAD_PRIORITY_BELOW_NORMAL		= 0x4,
	SN_THREAD_PRIORITY_LOWEST			= 0x5,
	SN_THREAD_PRIORITY_IDLE				= 0x6,
} snThreadPriority;

#ifdef _IOS

#ifndef THREAD_PRIORITY_IDLE
#define THREAD_PRIORITY_IDLE 15
#endif
	
#ifndef THREAD_PRIORITY_LOWEST
#define THREAD_PRIORITY_LOWEST 21
#endif

#ifndef THREAD_PRIORITY_BELOW_NORMAL
#define THREAD_PRIORITY_BELOW_NORMAL 26
#endif

#ifndef THREAD_PRIORITY_NORMAL
#define THREAD_PRIORITY_NORMAL 31
#endif
	
#ifndef THREAD_PRIORITY_ABOVE_NORMAL
#define THREAD_PRIORITY_ABOVE_NORMAL 36
#endif
	
#ifndef THREAD_PRIORITY_HIGHEST
#define THREAD_PRIORITY_HIGHEST 41
#endif
	
#ifndef THREAD_PRIORITY_TIME_CRITICAL
#define THREAD_PRIORITY_TIME_CRITICAL 47
#endif

#endif

/**
 * Create the thread
 * \param pHandle [out] the thread handle
 * \param pID [out] the thread id
 * \param fProc [in] the thread start address
 * \param pParam [in] the thread call back parameter
 * \param uFlag [in] the thread was create with the flagr
 * \return value SN_ErrorNone
 */
EXPORT SN_U32	snThreadCreate (snThreadHandle * pHandle, SN_ULONG * pID, snThreadProc fProc, SN_PTR pParam, SN_U32 uFlag);


/**
 * close the thread
 * \param pHandle [in] the thread handle was created by snThreadCreate
 * \param uExitCode [in] the return code after exit the thread
 * \return value SN_ErrorNone
 */
EXPORT SN_U32	snThreadClose (snThreadHandle hHandle, SN_U32 uExitCode);


/**
 * Get the thread priority
 * \param pHandle [in] the thread handle was created by snThreadCreate
 * \param pPriority [out] the priority to get
 * \return value SN_ErrorNone
 */
EXPORT SN_U32	snThreadGetPriority (snThreadHandle hHandle, snThreadPriority * pPriority);


/**
 * Set the thread priority
 * \param pHandle [in] the thread handle was created by snThreadCreate
 * \param uPriority [in] the priority to set
 * \return value SN_ErrorNone
 */
EXPORT SN_U32	snThreadSetPriority (snThreadHandle hHandle, snThreadPriority uPriority);


/**
 * Suspend the thread
 * \param pHandle [in] the thread handle was created by snThreadCreate
 * \return value SN_ErrorNone
 */
EXPORT SN_U32	snThreadSuspend (snThreadHandle hHandle);


/**
 * Resume the thread
 * \param pHandle [in] the thread handle was created by snThreadCreate
 * \return value SN_ErrorNone
 */
EXPORT SN_U32	snThreadResume (snThreadHandle hHandle);

/**
 * Protect the thread
 * \param pHandle [in] the thread handle was created by snThreadCreate
 * \return value SN_ErrorNone
 */
EXPORT SN_U32	snThreadProtect (snThreadHandle hHandle);

/**
 * set the thread name
 * \param uThreadID [in] the thread id was created by snThreadCreate
 * \return value SN_ErrorNone
 */
EXPORT void snThreadSetName(SN_U32 uThreadID, const char* threadName);

EXPORT void snThreadGetName(char* const threadName, const int buflen);
/**
 * convert the thread priority
 * \param uPriority [in] the thread priority
 * \return value 
 */
EXPORT SN_U32	snThreadConvertPriority (snThreadPriority * pPriority, SN_S32 * pPlatform, SN_BOOL bPlatform);

/**
 * Get the current thread ID
 * \return current thread ID 
 */
EXPORT SN_ULONG snThreadGetCurrentID (void);
        
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif // __snread_H__
