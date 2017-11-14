	/************************************************************************
	*																		*
	*		Snail, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		snMutex.cpp

	Contains:	snMutex class file

	Written by:	David

*******************************************************************************/
#include "snMutex.h"

snMutex::snMutex() {

#if defined  _WIN32
    InitializeCriticalSection(&m_CritSec);
#elif defined NNJ
	pthread_mutexattr_t attr;
	pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE_NP);
	pthread_mutex_init (&m_hMutex, &attr);
#elif defined _LINUX_ANDROID
	pthread_mutexattr_t attr = PTHREAD_MUTEX_RECURSIVE_NP;
	pthread_mutex_init (&m_hMutex, &attr);
#elif defined(_IOS) || defined(_MAC_OS)
	int                   rc=0;
	pthread_mutexattr_t   mta;
	rc = pthread_mutexattr_init(&mta);
	rc = pthread_mutexattr_settype(&mta, PTHREAD_MUTEX_RECURSIVE);
	
	rc = pthread_mutex_init(&m_hMutex, &mta);
	rc = pthread_mutexattr_destroy(&mta);
	
	//pthread_mutex_init (&m_hMutex, NULL);	
#endif // _WIN32
}

snMutex::~snMutex() {
#ifdef _WIN32
    DeleteCriticalSection(&m_CritSec);
#elif defined LINUX
	pthread_mutex_destroy (&m_hMutex);
#elif defined(_IOS) || defined(_MAC_OS)
	pthread_mutex_destroy (&m_hMutex);
#endif // _WIN32
}

void snMutex::lock(void) {
#ifdef _WIN32
    EnterCriticalSection(&m_CritSec);
#elif defined LINUX// _WIN32
	pthread_mutex_lock (&m_hMutex);
#elif defined(_IOS) || defined(_MAC_OS)
	//SNLOGI("lock mutex = %d", m_hMutex);
	pthread_mutex_lock (&m_hMutex);	
#endif
}

void snMutex::unlock(void) {

#ifdef _WIN32
    LeaveCriticalSection(&m_CritSec);
#elif defined LINUX
	pthread_mutex_unlock (&m_hMutex);
#elif defined(_IOS) || defined(_MAC_OS)
	pthread_mutex_unlock (&m_hMutex);	
#endif // _WIN32
}

int snMutex::trylock(void) {

	int ret = 0;
#ifdef _WIN32
    LeaveCriticalSection(&m_CritSec);
#elif defined LINUX
	ret = pthread_mutex_trylock (&m_hMutex);
#elif defined(_IOS) || defined(_MAC_OS)
	ret = pthread_mutex_trylock (&m_hMutex);	
#endif // _WIN32

	return ret;
}

