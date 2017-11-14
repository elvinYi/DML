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
#ifndef __snBaseread_H__
#define __snBaseread_H__

#include "snIndex.h"
#include "snThread.h"
#include "snMutex.h"
#include "snSemaphore.h"

#ifdef _LINUX_ANDROID
#include "jni.h"
#endif	// _LINUX_ANDROID


struct EXPORT_CLS snBaseEventItem {

	//SN_U32				type;
	SN_U32				id;
	SN_ULONG			p1;
	SN_ULONG			p2;
	SN_U64				order;
	SN_U64				time;
	snBaseEventItem		*pnext;

	snBaseEventItem(const SN_U32 uid, const SN_ULONG up1 = 0, const SN_ULONG up2 = 0, const SN_U64 ullOrder = 0) {
		//type = 0;
		id = uid;
		p1 = up1;
		p2 = up2;
		order = ullOrder;
		time = 0LL;
		pnext = NULL;
	}

	snBaseEventItem(const snBaseEventItem &r) {
		//type = r.type;
		id = r.id;
		p1 = r.p1;
		p2 = r.p2;
		order = r.order;
		time = r.time;
		pnext = NULL;
	}

	snBaseEventItem& operator= (const snBaseEventItem &r) {

		//type = r.type;
		id = r.id;
		p1 = r.p1;
		p2 = r.p2;
		order = r.order;
		time = r.time;
	
		return *this;
	}
};

class EXPORT_CLS snBaseThread {

public:
	snBaseThread(void* const trigger, const char*pname = NULL, void* pUserData = NULL);
	virtual ~snBaseThread(void);

	//int					start(SNTDThreadCreate fThreadCreate = NULL);
	int					start(void);
	int					pause(void);
	int					stop(void);
	int					enqueueEvent(snBaseEventItem &r);
	int					enqueueEvent(const SN_U32 id, const SN_ULONG p1 = 0, const SN_ULONG p2 = 0, const SN_S32 delay = 0); 
	int					enqueueEventByOrder(const SN_U32 id, const SN_U64 order, const SN_ULONG p1 = 0, const SN_ULONG p2 = 0, const SN_S32 delay = 0); 
	void				setRecycleInterval(const SN_U32 interval = 0) { m_uInterval = interval; }
	void				cancelAllEvent(void);
	void				cancelEventByP2(const SN_ULONG p2);
	void				cancelEventByOrder(const SN_ULONG order);
	SN_STATUS			getStatus (void);

	int					setJavaVM(SN_PTR pJavaVM);
	void*				getContext(void);

	void				printall(const SN_ULONG p2);
private:

	snBaseEventItem*	dequeue(void);
	int					returnEvent(snBaseEventItem* const);
	int					returnEventByOrder(snBaseEventItem* const, const SN_BOOL bAutoIncrease = SN_FALSE);
	int					returnBuffer(snBaseEventItem* const);

	bool				isEmpty(void);
	int					freeAllEvent(void);
	int					process(void);
	static	int			snEventHandler(SN_PTR pParam);

protected:
	SN_STATUS			m_nstat;
	snThreadHandle		m_handle;
	SN_ULONG			m_nID;
	SN_U32				m_uInterval;
	char*				m_pszName;

	snMutex				m_mutStatus;
	snMutex				m_mutMsgQueue;
	snMutex				m_mutCancelEvent;

	snSemaphore			m_semaPause;
	snSemaphore			m_semaStop;
	snSemaphore			m_semaEventQueue;
#ifdef _LINUX_ANDROID
	snSemaphore			m_semaContext;
#endif

	snBaseEventItem*	m_phead;
	snBaseEventItem*	m_pmiddle;
	snBaseEventItem*	m_ptail;
	int					m_nfilled;
	int					m_nfree;

	void*				m_pagent;
	unsigned int		(*m_ffire) (const SN_U32 evt, const SN_ULONG p1, const SN_ULONG p2, void* agent);
#ifdef _LINUX_ANDROID
	JavaVM*				m_pJavaVM;
	JNIEnv*				m_pJniEnv;
#endif	// _LINUX_ANDROID
};
    
#endif // __snBaseread_H__

