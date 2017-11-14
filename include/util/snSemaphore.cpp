	/************************************************************************
	*																		*
	*		Snail, Inc. Confidential and Proprietary, 2013				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		snSemaphore.cpp

	Contains:	snSemaphore class file

	Written by:	David
*******************************************************************************/

#if defined(_WIN32)
#  include <tchar.h>
#endif

#ifndef WINCE
#include "errno.h"
#endif //WINCE

#if defined(LINUX)
#include <unistd.h>
#include <sys/time.h>
#include <jni.h>
#include <android/log.h>
#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h> 
#elif defined _IOS || defined _MAC_OS
#include <sys/time.h>
#include <time.h>
#include "sninfo.h"
#endif

#include "snThread.h"
#include "snSemaphore.h"

#if defined _IOS 
long snSemaphore::m_nTotalSemaIndex = 0;
#endif

snSemaphore::snSemaphore(const SN_U32 initial) : m_nprimary (initial) {

#if defined  _WIN32

	int max = 100;
	m_hsema = CreateSemaphore(NULL,
		m_nprimary,			// initial count
		max,				// maximum count
		NULL);				// unnamed semaphore

#elif defined LINUX || defined(_MAC_OS)
	sem_init(&m_hsema, 0, initial);
#elif defined _IOS
	m_hsema = NULL;
	memset(m_szSemaName, 0, SNSEM_NAME_MAXLEN);

	char szname[32];
	szname[0] = '\0';
	sprintf(szname, "/snsem-%04ld", m_nTotalSemaIndex);
	if (SNSEM_ERR == open(szname, O_CREAT | O_EXCL, 0644, initial)) {
		//SNINFO("sema name: %s, initial: %d", m_szSemaName, initial);
		//m_hsema = sem_open(m_szSemaName, O_CREAT, 0644, initial);
		//SNINFO("m_hsema: %p", m_hsema);
		SNINFO("create sema %s failed: %s", szname, strerror(errno));

		szname[0] = '\0';
		sprintf(szname, "/snsem-%04ld", random());
		if (SNSEM_ERR == open(szname, O_CREAT, 0644, initial)) {
			SNINFO("create sema %s failed: %s", szname, strerror(errno));
		}
	}
	m_nTotalSemaIndex++;
	//SNINFO("create sema %s with %d got %p", szname, initial, m_hsema);
#endif // _WIN32

}

snSemaphore::snSemaphore(const SN_U32 limit, const SN_U32 initial) : m_nprimary (initial)
{

#if defined  _WIN32

	int max = limit;
	m_hsema = CreateSemaphore(NULL,
		m_nprimary,			// initial count
		max,				// maximum count
		NULL);				// unnamed semaphore

#elif defined LINUX || defined(_MAC_OS)
	sem_init(&m_hsema, 0, initial);
#elif defined _IOS
	m_hsema = NULL;
	memset(m_szSemaName, 0, SNSEM_NAME_MAXLEN);

	char szname[32];
	szname[0] = '\0';
	sprintf(szname, "/snsem-%04ld", m_nTotalSemaIndex);
	if (SNSEM_ERR == open(szname, O_CREAT | O_EXCL, 0644, initial)) {
		//SNINFO("sema name: %s, initial: %d", m_szSemaName, initial);
		//m_hsema = sem_open(m_szSemaName, O_CREAT, 0644, initial);
		//SNINFO("m_hsema: %p", m_hsema);
		SNINFO("create sema %s failed: %s", szname, strerror(errno));

		szname[0] = '\0';
		sprintf(szname, "/snsem-%04ld", random());
		if (SNSEM_ERR == open(szname, O_CREAT, 0644, initial)) {
			SNINFO("create sema %s failed: %s", szname, strerror(errno));
		}
	}
	m_nTotalSemaIndex++;
	SNINFO("create sema %s with %d got %p", szname, initial, m_hsema);
#endif // _WIN32

}

snSemaphore::~snSemaphore()
{
#ifdef _WIN32
	CloseHandle (m_hsema);
#elif defined LINUX || defined(_MAC_OS)
	sem_destroy(&m_hsema);
	/*if (m_hsema) {
		if (sem_close(m_hsema) != 0) {
			//SNINFO("%s", strerror(errno));
		}
		if (sem_unlink(m_szSemaName) != 0) {
			//SNINFO("%s", strerror(errno));
		}
		m_hsema = NULL;
	}*/
#elif defined _IOS
	if (m_hsema) {
		if (sem_close(m_hsema) != 0) {
			SNINFO("%s", strerror(errno));
		}
		if (sem_unlink(m_szSemaName) != 0) {
			SNINFO("%s", strerror(errno));
		}
		SNINFO("closed %s %p", m_szSemaName, m_hsema);
		m_hsema = NULL;
	}
#endif // _WIN32
}

#if defined _IOS
SN_U32 snSemaphore::open(const char* const name, const int oflag) 
{

	if (name == NULL)
		return SNSEM_ERR;
	
	sem_unlink(name);
	m_hsema = sem_open(name, oflag);
	//SNINFO("m_hsema: %p", m_hsema);
	if (m_hsema == SEM_FAILED) {
		SNINFO("error: 0x%08x, %s", errno, strerror(errno));
		m_hsema = NULL;
		return SNSEM_ERR;
	}

	//SNINFO("sema name: %s, m_hsema: %p", name, m_hsema);
	int size = strlen(name);
	if (size >= SNSEM_NAME_MAXLEN)
		size = SNSEM_NAME_MAXLEN - 1;
	strncpy(m_szSemaName, name, size);
	m_szSemaName[size] = '\0';
	return 0;
}

SN_U32 snSemaphore::open(const char* const name, const int oflag, const unsigned int mode, const unsigned int value) 
{

	sem_unlink(name);
	m_hsema = sem_open(name, oflag, mode, value);
	//SNINFO("m_hsema: %p", m_hsema);
	if (m_hsema == SEM_FAILED) {
		SNINFO("error: 0x%08x, %s", errno, strerror(errno));
		m_hsema = NULL;
		return SNSEM_ERR;
	}

	//SNINFO("sema name: %s, m_hsema: %p", name, m_hsema);
	int size = strlen(name);
	if (size >= SNSEM_NAME_MAXLEN)
		size = SNSEM_NAME_MAXLEN - 1;
	strncpy(m_szSemaName, name, size);
	m_szSemaName[size] = '\0';
	return 0;
}
#endif

SN_U32 snSemaphore::getvalue() {

	SN_U32 val = 0;
#if defined _WIN32
#elif defined LINUX || defined(_MAC_OS)
	int nval = 0;
	int ret = sem_getvalue(&m_hsema, &nval);
	if (ret != 0)
		val = 0;
	else
		val = nval;
#endif
	return val;
}

SN_U32 snSemaphore::down(const SN_U32 step)
{
#ifdef _WIN32

	SN_U32 uRC = WaitForSingleObject(m_hsema, SNSEM_MAXTIME);
	if (uRC == WAIT_TIMEOUT)
		return SNSEM_TIMEOUT;

#elif defined LINUX || defined(_MAC_OS)
	//int ret = sem_wait(&m_hsema);
	sem_wait(&m_hsema);
	////SNINFO("sem_wait return %d", ret);
#elif defined _IOS
	sem_wait(m_hsema);
#endif // _WIN32

	return 0;
}

SN_U32 snSemaphore::up (const SN_U32 step)
{
#ifdef _WIN32

	ReleaseSemaphore(m_hsema, step, NULL);

#elif defined LINUX || defined(_MAC_OS)
	sem_post(&m_hsema);
#elif defined _IOS
	sem_post(m_hsema);
#endif // _WIN32

	return 0;
}


SN_U32 snSemaphore::wait(SN_U32 nWaitTime)
{
	SN_U32 ret = SNSEM_OK;

#ifdef _WIN32

	SN_U32 uRC = WaitForSingleObject (m_hsema, nWaitTime);
	if (uRC == WAIT_TIMEOUT)
		return SNSEM_TIMEOUT;

#elif defined LINUX || defined(_MAC_OS)

	/*ret = sem_trywait(&m_hsema);
	if (ret == EAGAIN) {

		struct timespec req = {0};
		time_t sec = (int)(nWaitTime / 1000);
		nWaitTime = nWaitTime - (sec * 1000);
		req.tv_sec = sec;   
		req.tv_nsec = nWaitTime * 1000000L; 
		while (nanosleep(&req, &req) == -1 && errno == EINTR) {
			////SNINFO("time remaining: %lds %ldns\n", req.tv_sec, req.tv_nsec);
			continue;
		}
	}*/
	struct timespec ts;  
	clock_gettime(CLOCK_REALTIME, &ts);  
	long secs = nWaitTime/1000;
	nWaitTime %= 1000;

	nWaitTime = nWaitTime*1000*1000 + ts.tv_nsec;  
	long add = nWaitTime / (1000*1000*1000);  
	ts.tv_sec += (add + secs);  
	ts.tv_nsec = nWaitTime % (1000*1000*1000);
	ret = sem_timedwait(&m_hsema, &ts);  

#elif defined _IOS
	ret = sem_trywait(m_hsema);
	if (ret == EAGAIN) {

		struct timespec req = {0};
		time_t sec = (int)(nWaitTime / 1000);
		nWaitTime = nWaitTime - (sec * 1000);
		req.tv_sec = sec;            /* s */
		req.tv_nsec = nWaitTime * 1000000L;    /* ns */
		while (nanosleep (&req, &req) == -1 && errno == EINTR) {
			////SNINFO("time remaining: %lds %ldns\n", req.tv_sec, req.tv_nsec);
			continue;
		}
	}
	
	ret = 0;
#endif

	return ret;
}

