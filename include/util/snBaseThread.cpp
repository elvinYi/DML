	/************************************************************************
	*																		*
	*		Snail, Inc. Confidential and Proprietary, 2003 - 2014		*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		snBaseThread.h

	Contains:	snBaseThread.header file

	Written by:	david
	
	Created: 2014/09/10

*******************************************************************************/
#ifdef __SYMBIAN32__
#include <stdlib.h>
#elif defined _IOS || defined _MAC_OS
#include <stdlib.h>
#else
#include <malloc.h>
#endif // __SYMBIAN32__

#include <string.h>
#include <stdio.h>
#include "snBaseThread.h"
#include "snOSFunc.h"

snBaseThread::snBaseThread(void* const trigger, const char* pname, void* pUserData)
		 : m_nstat(SN_STAT_INITED)
		 , m_handle(NULL)
		 , m_nID(0)
		 , m_uInterval(0)
		 , m_pszName(NULL)
		 , m_phead(NULL)
		 , m_pmiddle(NULL)
		 , m_ptail(NULL)
		 , m_nfilled(0)
		 , m_nfree(0)
		 , m_pagent(pUserData)
#ifdef _LINUX_ANDROID
		 , m_pJavaVM(NULL)
		 , m_pJniEnv(NULL)
#endif	// _LINUX_ANDROID
{
	if (pname != NULL) {
		int len = strlen(pname);
		if (len) {
			m_pszName = new char[len + 1];
			strncpy(m_pszName, pname, len);
			*(m_pszName + len) = '\0';
		}
	}

	*((void**)(&m_ffire)) = trigger;
}

snBaseThread::~snBaseThread(void) {

	stop();
	cancelAllEvent();
	freeAllEvent();

	delete m_pszName;
	m_pszName = NULL;
}

int snBaseThread::start(void) {


	{
		snAutoLock l(&m_mutStatus);

		//SNINFO("%s start with stat: %d", m_pszName, m_nstat);
		if (m_nstat == SN_STAT_RUNNING)
			return SN_ERR_NONE;

		m_nstat = SN_STAT_RUNNING;
	}

	// start from paused-status
	if (m_handle != NULL) {
		m_semaPause.up();
		return SN_ERR_NONE;
	}

	snThreadCreate(&m_handle, &m_nID, (snThreadProc)snEventHandler, this, 3);
	if (m_handle == NULL)
		return SN_ERR_FAILED;

#ifdef _LINUX_ANDROID
	if (m_pJavaVM) {
		// syncing context
		m_semaContext.down();
		SNINFO("JNIENV: %p", m_pJniEnv);
		// till now, context shouldn't be null
	}
#endif

	// for pause
	m_semaPause.up();
	return SN_ERR_NONE;
}

int snBaseThread::pause(void) {


	//SNINFO("pause with status: %d", m_nstat);
	if (m_nstat != SN_STAT_RUNNING)
		return SN_ERR_FAILED;

	{
		snAutoLock l(&m_mutStatus);
		m_nstat = SN_STAT_PAUSED;
	}

	////SNINFO("before pause semaphore: %d", m_semaPause.getvalue());
	m_semaPause.down();
	////SNINFO("after pause semaphore: %d", m_semaPause.getvalue());

	return SN_ERR_NONE;
}

int snBaseThread::stop(void) {

	if (m_nstat == SN_STAT_INITED || m_nstat == SN_STAT_STOPPED)
		return SN_ERR_NONE;

	if (m_nstat == SN_STAT_PAUSED)
		m_semaPause.up();

	{
		snAutoLock l(&m_mutStatus);
		m_nstat = SN_STAT_STOPPED;
	}

	// maybe the routine being suspended on this semaphore, so signal it
	m_semaEventQueue.up();
	//SNINFO("%s waiting routine exit...", m_pszName);
	m_semaStop.down(); // make sure the trhead-routine must stop before exit
	//SNINFO("%s thread stopped!", m_pszName);

	return SN_ERR_NONE;
}

SN_STATUS snBaseThread::getStatus(void) {

	snAutoLock l(&m_mutStatus);
	return m_nstat;
}

int	snBaseThread::enqueueEvent(snBaseEventItem &r) {

	{
		snAutoLock l(&m_mutMsgQueue);

		if (m_nfilled) { // have valid data

			if (m_pmiddle->pnext) { // there're free buffer(s)
				*(m_pmiddle->pnext) = r;
				m_nfree--;
			} else { // no free buffer
				m_pmiddle->pnext = new snBaseEventItem(r);
			}

			if (m_pmiddle->pnext) {
				m_pmiddle = m_pmiddle->pnext;
				m_nfilled++;
			}

		} else {  // no valid data

			if (m_pmiddle) { // there're free buffers
				*m_pmiddle = r;
				m_nfilled++;
				m_nfree--;
			} else { // no free buffer
				m_pmiddle = new snBaseEventItem(r);
				if (m_pmiddle) {
					m_phead = m_pmiddle;
					m_nfilled++;
				}
			}
		}

		// if no free buffer any more
		if (m_nfree == 0)
			m_ptail = NULL;

		//printall();
	}

	m_semaEventQueue.up();
	return m_nfilled;
}

int	snBaseThread::enqueueEvent(const SN_U32 id, const SN_ULONG p1, const SN_ULONG p2, const SN_S32 delay) {

	snBaseEventItem eve(id, p1, p2);
	if (delay > 0)
		eve.time = snOS_GetSysTime() + delay;

	enqueueEvent(eve);
	return m_nfilled;
}

int snBaseThread::enqueueEventByOrder(const SN_U32 id, const SN_U64 ullOrder, const SN_ULONG p1, const SN_ULONG p2, const SN_S32 delay)
{
	snBaseEventItem r(id, p1, p2, ullOrder);
	if (delay > 0)
		r.time = snOS_GetSysTime() + delay;
	else 
		r.time = snOS_GetSysTime();

	{
		snAutoLock l(&m_mutMsgQueue);

		//SNINFO("head: %p, mid: %p, tail: %p", m_phead, m_pmiddle, m_ptail);
		if (m_nfilled) { // have valid data

			snBaseEventItem* pprev = NULL;
			snBaseEventItem* pcur = m_phead;
			bool found = false;
			for (int i = 0; i < m_nfilled; ++i) {

				if (pcur == NULL) break;

				if (ullOrder <= pcur->order) {
					found = true;
					break;
				}

				pprev = pcur;
				pcur = pcur->pnext;
			}

			if (!found) {

				if (m_pmiddle->pnext) { // there're free buffer(s)
					*(m_pmiddle->pnext) = r;
					m_nfree--;
				} else { // no free buffer
					m_pmiddle->pnext = new snBaseEventItem(r);
				}

				if (m_pmiddle->pnext) {
					m_pmiddle = m_pmiddle->pnext;
					m_nfilled++;
				}

			} else {

				// circularly utilize the unoccupied buffer
				snBaseEventItem* pUnoccupied = m_pmiddle->pnext;
				if (pUnoccupied) {

					m_nfree--;
					if (pUnoccupied == m_ptail)
						m_ptail = NULL;

					m_pmiddle->pnext = pUnoccupied->pnext;
					*pUnoccupied = r;
					if (pprev) {
						pprev->pnext = pUnoccupied;
						pprev = pUnoccupied;
					} else {
						pprev = pUnoccupied;
						m_phead = pprev;
					}

				} else {

					if (pprev) {
						pprev->pnext = new snBaseEventItem(r);
						pprev = pprev->pnext;
					} else {
						pprev = new snBaseEventItem(r);
						if (pprev)
							m_phead = pprev;
					}
				}

				if (pprev) {
					pprev->pnext = pcur;
					m_nfilled++;
				}
			}

		} else {  // no valid data

			if (m_pmiddle) { // there're free buffers
				*m_pmiddle = r;
				m_nfilled++;
				m_nfree--;
			} else { // no free buffer
				m_pmiddle = new snBaseEventItem(r);
				if (m_pmiddle) {
					m_phead = m_pmiddle;
					m_nfilled++;
				}
			}
		}

		// if no free buffer any more
		if (m_nfree == 0)
			m_ptail = NULL;

		//printall();
	}

	m_semaEventQueue.up();
	return m_nfilled;
}

void snBaseThread::cancelAllEvent(void) {

	//SNINFO("%s to cancel all events.", m_pszName);
	snAutoLock l0(&m_mutCancelEvent);
	//SNINFO("%s cancelled all events.", m_pszName);
	{
		snAutoLock l(&m_mutMsgQueue);

		if (m_phead == NULL)
			return;

		m_nfree += m_nfilled;
		m_nfilled = 0;
		m_pmiddle = m_phead;
	}
}

void snBaseThread::cancelEventByP2(const SN_ULONG p2)
{
	snAutoLock l0(&m_mutCancelEvent);
	snAutoLock l(&m_mutMsgQueue);
    if (m_phead == NULL || m_nfilled == 0)
        return;

	bool found = false;
	do {

		found = false;
		snBaseEventItem* pprev = NULL;
		snBaseEventItem* pcur = m_phead;
		for (int i = 0; i < m_nfilled; ++i) {

			if (pcur == NULL) break;

			if (pcur->p2 == p2) {

				found = true;
				break;

			} else {

				pprev = pcur;
				pcur = pcur->pnext;
			}
		}
		//SNINFO("%s %02u would be canceled.", m_pszName, p2);

		if (!found) break;

		if (pprev)
			pprev->pnext = pcur->pnext;
		else
			m_phead = m_phead->pnext;

		if (m_pmiddle == pcur)
			m_pmiddle = pprev;

		m_nfilled--;
		if (m_nfilled == 0)
			m_pmiddle = m_phead;

		returnBuffer(pcur);

	} while (m_nfilled && found);
}

void snBaseThread::cancelEventByOrder(const SN_ULONG order)
{

	snAutoLock l(&m_mutMsgQueue);
    if (m_phead == NULL || m_nfilled == 0)
        return;

	bool found = false;
	do {

		found = false;
		snBaseEventItem* pprev = NULL;
		snBaseEventItem* pcur = m_phead;
		for (int i = 0; i < m_nfilled; ++i) {

			if (pcur == NULL) break;

			if (pcur->order == order) {

				found = true;
				break;

			} else {

				pprev = pcur;
				pcur = pcur->pnext;
			}
		}
		//SNINFO("%s %02u would be canceled.", m_pszName, p2);

		if (!found) break;

		if (pprev)
			pprev->pnext = pcur->pnext;
		else
			m_phead = m_phead->pnext;

		if (m_pmiddle == pcur)
			m_pmiddle = pprev;

		m_nfilled--;
		if (m_nfilled == 0)
			m_pmiddle = m_phead;

		returnBuffer(pcur);

	} while (m_nfilled && found);
}

snBaseEventItem* snBaseThread::dequeue(void) {

	snAutoLock l(&m_mutMsgQueue);

	if (isEmpty()) return NULL;

	snBaseEventItem* pe = m_phead;
	if (m_phead) {
		m_phead = m_phead->pnext;
		pe->pnext = NULL;
	}

	// it's the last one valid item
	if (pe == m_pmiddle) {
		if (NULL == m_phead) { //no valid buffer
			m_pmiddle = NULL;
			m_ptail = NULL;
		} else { // both head & middle all point to the 1st free buffer
			m_pmiddle = m_phead;
		}
	} 

	m_nfilled--;
	//SNINFO("dequeued %p time %llu", pe, pe->time);
	//printall();
	return pe;
}

int snBaseThread::returnEvent(snBaseEventItem* const pe) 
{

	if (NULL == pe) 
		return m_nfilled;

	{
		snAutoLock l(&m_mutMsgQueue);

		//SNINFO("event %p, id: %08x, time: %llu", pe, pe->id, pe->time);
		if (m_nfilled) { // have valid data

			pe->pnext = m_pmiddle->pnext;
			m_pmiddle->pnext = pe;
			m_pmiddle = pe;

		} else {  // no valid data

			pe->pnext = m_pmiddle;
			m_phead = m_pmiddle = pe;
		}

		m_nfilled++;
		//printall();
	}

	m_semaEventQueue.up();
	return m_nfilled;
}

int snBaseThread::returnEventByOrder(snBaseEventItem* const pe, const SN_BOOL bAutoIncrease) 
{

	if (NULL == pe) 
		return m_nfilled;

	if (bAutoIncrease) {
		if (pe->time == 0)
			pe->time = snOS_GetSysTime() + m_uInterval;
		else 
			pe->time += m_uInterval;
		//SNINFO("%s execute time: %llu, interval: %u, systime: %llu", m_pszName, pe->time, m_uInterval, snOS_GetSysTime());
	}

	{
		snAutoLock l(&m_mutMsgQueue);

		//SNINFO("event %p, id: %08x, order: %llu, mid: %p, tail: %p", pe, pe->id, pe->order, m_pmiddle, m_ptail);
		if (m_nfilled) { // have valid data

			snBaseEventItem* pprev = NULL;
			snBaseEventItem* pcur = m_phead;
			bool found = false;
			for (int i = 0; i < m_nfilled; ++i) {

				if (pcur == NULL) break;

				if (pe->order < pcur->order) {
					found = true;
					break;
				}

				pprev = pcur;
				pcur = pcur->pnext;
			}

			if (!found) {

				pe->pnext = m_pmiddle->pnext;
				m_pmiddle->pnext = pe;
				m_pmiddle = pe;

			} else {

				if (pprev) {
					pprev->pnext = pe;
					pprev = pe;
				} else {
					pprev = pe;
					m_phead = pprev;
				}

				if (pprev)
					pprev->pnext = pcur;
			}

		} else {  // no valid data

			pe->pnext = m_pmiddle;
			m_phead = m_pmiddle = pe;
		}
		m_nfilled++;
		//printall();
	}

	m_semaEventQueue.up();
	return m_nfilled;
}

int snBaseThread::returnBuffer(snBaseEventItem* const pe) {

	if (NULL == pe) 
		return m_nfree;

	snAutoLock l(&m_mutMsgQueue);

	//SNINFO("event %p, id: %08x, time: %llu", pe, pe->id, pe->time);
	pe->id = 0;
	pe->time = 0LL;
	pe->order = 0LL;
	pe->pnext = NULL;
	if (m_ptail) {
		m_ptail->pnext = pe;
	} else {
		if (m_pmiddle)
			m_pmiddle->pnext = pe;
		else
			m_phead = m_pmiddle = pe;
	}

	m_ptail = pe;
	m_nfree++;
	//printall();
	return m_nfree;
}

bool snBaseThread::isEmpty(void) {

	return (m_nfilled == 0 ? true : false);
}

int snBaseThread::freeAllEvent(void) {

	snAutoLock l(&m_mutMsgQueue);

	snBaseEventItem* ph = m_phead;
	while (ph) {
		m_phead = ph->pnext;
		delete ph;
		ph = m_phead;
	}

	m_phead = m_pmiddle = m_ptail = NULL;
	return SN_ERR_NONE;
}

int snBaseThread::snEventHandler(SN_PTR pParam) {

	snBaseThread * pt = (snBaseThread *)pParam;
	if (NULL == pt) return SN_ERR_FAILED;

	int ret = pt->process();
	return ret;
}
	
int snBaseThread::process(void) {

	snThreadSetName(m_nID, m_pszName);

#ifdef _LINUX_ANDROID
	if (m_pJavaVM) {

		int nStatus = m_pJavaVM->GetEnv((void **)&m_pJniEnv, JNI_VERSION_1_4);
		if (JNI_EDETACHED == nStatus)
			nStatus = m_pJavaVM->GetEnv((void **)&m_pJniEnv, JNI_VERSION_1_6);

		if (JNI_EDETACHED == nStatus) {
			m_pJavaVM->AttachCurrentThread(&m_pJniEnv, NULL);
			SNINFO("Attach thread %s to env-%p of JM-%p", m_pszName, m_pJniEnv, m_pJavaVM);
		}

		// for syncing context
		SNINFO("signal caller-thread going-forward.");
		m_semaContext.up();
	}
#endif	// _LINUX_ANDROID

	while (true) {

		// here to avoid dead-lock when it is changing status 
		SN_STATUS curstat = getStatus();
		if (curstat == SN_STAT_STOPPED)
			break;

		if (curstat == SN_STAT_PAUSED) {

			//SNINFO("%s really gonna pause... semaphore: %d", m_pszName, m_semaPause.getvalue());
			m_semaPause.down();

			//SNINFO("here already exit paused-status, so signal again for next pause. semaphore: %d", m_semaPause.getvalue());
			m_semaPause.up();
		}

		//SNINFO("status: %d, filled: %d, semahore: %d", curstat, m_nfilled, m_semaEventQueue.getvalue());
		// check if there's event, if no, suspend
		m_semaEventQueue.down();
		
		snBaseEventItem* pe = NULL;
		{
			snAutoLock l0(&m_mutCancelEvent);
			pe = dequeue();
			if (NULL == pe) continue;

			SN_U64 now = snOS_GetSysTime();
			//SNINFO("[event] %s id %08x, time %llu, systime: %llu", m_pszName, pe->id, pe->time, now);
			if (pe->time > 2 && now < (pe->time - 2)) {

				//SN_U64 begin = snOS_GetSysTime();
				//SNINFO("gonna sleep 2ms @ %llu for pe->time: %llu", begin, pe->time);
				snOS_Sleep(2);
				//SN_U64 end = snOS_GetSysTime();
				//SNINFO("truly slept %llu", end - begin);

				returnEventByOrder(pe);
				//SNINFO("[event] id %08x, time %llu, returned @ systime: %llu ", pe->id, pe->time, end);
				continue;
			}

			unsigned int ret = SN_ERR_NONE;
			if (m_ffire) {
				//if (pe->id == 0) {
				  //SNINFO("[%s fire] id %08x, p1 %08lx, p2 %08lx @ %llu", m_pszName, pe->id, pe->p1, pe->p2, now);
				  //}
				ret = m_ffire(pe->id, pe->p1, pe->p2, m_pagent);
			}

			if (ret == SN_ERR_RECYCLE) {
				//SNINFO("%s ret: %08x", m_pszName, ret);
				returnEventByOrder(pe, SN_TRUE);
				continue;
			}
		}

		returnBuffer(pe);
	}

#ifdef _LINUX_ANDROID
	if(m_pJavaVM) {
		SNINFO("Detach thread %s from env-%p JM-%p.", m_pszName, m_pJniEnv, m_pJavaVM);
		m_pJavaVM->DetachCurrentThread();
	}
#endif	// _LINUX_ANDROID

	m_handle = NULL;

	//SNINFO("thread %s exit.", m_pszName);
	m_semaStop.up(); // signal up to notify the calling-stop thread moving on.
	return 0;
}


int snBaseThread::setJavaVM(SN_PTR pJavaVM) {

#ifdef _LINUX_ANDROID
	m_pJavaVM = (JavaVM *)pJavaVM;
#endif	// _LINUX_ANDROID

	return 0;
}

void* snBaseThread::getContext(void) { 
	
	void* env = NULL;
#if defined _LINUX_ANDROID
	env = (void*)m_pJniEnv;
#endif

	return env;
}

void snBaseThread::printall(const SN_ULONG p2) 
{

	snAutoLock l(&m_mutMsgQueue);
	snBaseEventItem* ph = m_phead;
	int i = 0, cnt = 0;
	//SNINFO("---------%s filled: %d, free: %d, head: %p, mid: %p, tail: %p. find %d %02lu---------", m_pszName, m_nfilled, m_nfree, m_phead, m_pmiddle, m_ptail, cnt, p2);
	while (ph && i < m_nfilled) {

		cnt++;
		++i;

		//SNINFO("%s [%d] dec-%02lu  %p->order = %llu, time = %llu pnext = %p", m_pszName, i, ph->p2, ph, ph->order, ph->time, ph->pnext);
		//SNINFO("%s [%d] %p->order = %llu, id = %08x, time = %llu pnext = %p", m_pszName, i, ph, ph->order, ph->id, ph->time, ph->pnext);

		ph = ph->pnext;
	}
	//SNINFO("---------------------over!------------------------\n");
}

