//
// Created by 58 on 2016/5/4.
//

#include <stdio.h>
#include <wedis/base/thread/CThread.h>
#include <wedis/base/util/Global.h>
#include <wedis/base/util/Log.h>
#include <wedis/base/util/System.h>

namespace wRedis {

void* threadProcess(void* pParam) {
	CThread* pThread = (CThread*) pParam;
	if (NULLPTR == pThread)
		return NULLPTR;

	pThread->ready();

	pThread->run();

	pThread->status(CThread::ThreadStatus::INVALID);

	Logger.debug("[%d]thread done.", pThread->getID());

	return NULLPTR;
}

CThread::CThread() :
		m_Lock(), m_Cond(m_Lock) {
	m_eStatus = ThreadStatus::INVALID;
	m_ThreadID = (ThreadID) INVALID_VALUE;
	m_hThread = NULLPTR;
}

CThread::~CThread() {
}

BOOL CThread::create() {
	if (ThreadStatus::INVALID != m_eStatus) {
		return FALSE;
	}

	I32 nResult = pthread_create(&m_ThreadID, NULLPTR, threadProcess, this);
	if (nResult != 0)
		return FALSE;

	m_hThread = this;

	return TRUE;
}

void CThread::ready() {
	LockGuard guard(m_Lock);
	status(ThreadStatus::READY);
	m_Cond.wait();
}

BOOL CThread::start() {
	while (m_eStatus != ThreadStatus::READY) {
		System::sleep(1);
	}

	{
		LockGuard guard(m_Lock);
		m_Cond.notify();
	}

	m_eStatus = ThreadStatus::RUNNING;
	return TRUE;
}

void CThread::exit() {
	m_eStatus = ThreadStatus::EXITING;
	if((ThreadID)INVALID_VALUE != m_ThreadID)  {
		void* pResult = 0;
		pthread_join(m_ThreadID, &pResult);
	}
	m_ThreadID = (ThreadID) INVALID_VALUE;
	m_hThread = NULLPTR;
	m_eStatus = ThreadStatus::INVALID;
}

/**
 * 这个接口有问题暂时不可以使用
 */
void CThread::forceExit() {
	m_eStatus = ThreadStatus::EXITING;
	if((ThreadID)INVALID_VALUE != m_ThreadID)  {
		if(0 != pthread_cancel(m_ThreadID)) {
			Logger.error("CThread::forceExit failed, threadi:%d", m_ThreadID);
		}
	}
	if((ThreadID)INVALID_VALUE != m_ThreadID)  {
		void* pResult = 0;
		pthread_join(m_ThreadID, &pResult);
	}
	m_ThreadID = (ThreadID) INVALID_VALUE;
	m_hThread = NULLPTR;
	m_eStatus = ThreadStatus::INVALID;
}

void CThread::run() {
}

HANDLE CThread::handle() {
	return m_hThread;
}

}
