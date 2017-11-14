	/************************************************************************
	*																		*
	*		Snail, Inc. Confidential and Proprietary, 2013				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		snSemaphore.h

	Contains:	snSemaphore.header file

	Written by:	 David 

*******************************************************************************/
#ifndef __snSemaore_H__
#define __snSemaore_H__

#ifdef _WIN32
#include <windows.h>
#elif defined LINUX
#include <pthread.h>
#include <semaphore.h>
#elif defined(_IOS) || defined(_MAC_OS)
#include <semaphore.h>
#include <pthread.h>
#endif // _WIN32

#include "snType.h"

#define SNSEM_NAME_MAXLEN 32

#define SNSEM_TIMEOUT	0X80000001
#define SNSEM_MAXTIME	0XFFFFFFFF
#define SNSEM_OK		0
#define SNSEM_ERR		-1

#undef EXPORT_CLS
#define EXPORT_CLS

// wrapper for whatever critical section we have
class EXPORT_CLS snSemaphore {

public:
	snSemaphore(const SN_U32 initial = 0);
	snSemaphore(const SN_U32 limit, const SN_U32 initial = 0);
	virtual ~snSemaphore(void);

#if defined _IOS
	SN_U32		open(const char* const name, const int oflag);
	SN_U32      open(const char* const name, const int oflag, const unsigned int mode, const unsigned int value);
#endif

	SN_U32		getvalue();
	SN_U32		down(const SN_U32 step = 1);
	SN_U32		up(const SN_U32 step = 1);
	SN_U32		wait(const SN_U32 nWaitTime = SNSEM_MAXTIME);

protected:

#ifdef _WIN32
	HANDLE		m_hsema;
#elif defined LINUX || defined _MAC_OS
	sem_t		m_hsema;
#elif defined _IOS 
	sem_t*		m_hsema;
	char		m_szSemaName[SNSEM_NAME_MAXLEN];
	static long m_nTotalSemaIndex;
#endif // _WIN32
	SN_U32		m_nprimary;
};

#endif //__snSemaore_H__

