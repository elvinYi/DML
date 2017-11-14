	/************************************************************************
	*																		*
	*		Snail, Inc. Confidential and Proprietary, 2013				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		snMutex.h

	Contains:	snMutex.header file

	Written by:	David
*******************************************************************************/
#ifndef __snMutex_H__
#define __snMutex_H__

#ifdef _WIN32
#include <windows.h>
#elif defined LINUX
#include <pthread.h>
#elif defined(_IOS) || defined(_MAC_OS) 
#include <pthread.h>
#endif // _WIN32

#include "snType.h"

#undef EXPORT_CLS
#define EXPORT_CLS

// wrapper for whatever critical section we have
class EXPORT_CLS snMutex {
public:
    snMutex(void);
    virtual ~snMutex(void);

    void lock(void);
    void unlock(void);
	int	 trylock(void);

protected:
#ifdef _WIN32
    CRITICAL_SECTION	m_CritSec;
#elif defined LINUX
	pthread_mutex_t		m_hMutex;
#elif defined(_IOS) || defined(_MAC_OS)
	pthread_mutex_t		m_hMutex;
#endif // _WIN32
};

// locks a critical section, and unlocks it automatically
// when the lock goes out of scope
class snAutoLock {

public:
    snAutoLock(snMutex * plock) {
        m_pLock = plock;
        if (m_pLock) {
            m_pLock->lock();
        }
    };

    virtual ~snAutoLock() {
        if (m_pLock) {
            m_pLock->unlock();
        }
    };

protected:
    snMutex * m_pLock;
};
    
#endif //__snMutex_H__
