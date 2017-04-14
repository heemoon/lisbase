//
// Created by 58 on 2016/5/4.
//

#include <wedis/base/thread/ThreadManager.h>
#include <wedis/base/util/Alloctor.h>

namespace wRedis {
ThreadManager::ThreadManager(BOOL bNeedDelete) {
	for (I32 i = 0; i < MAX_THREAD_NUM; ++i) {
		m_aThreads[i] = NULLPTR;
	}
	m_uCount = 0;
	m_bNeedDelete = bNeedDelete;
}

ThreadManager::~ThreadManager() {
	m_uCount = 0;
	for (I32 i = 0; i < MAX_THREAD_NUM; ++i) {
		if (TRUE == m_bNeedDelete) {
			W_FREE(m_aThreads[i]);
		} else {
			m_aThreads[i] = NULLPTR;
		}
	}
}

BOOL ThreadManager::addThread(CThread* pThread) {
	if ((m_uCount >= MAX_THREAD_NUM) || (pThread == NULLPTR)) {
		return FALSE;
	}

	for (I32 i = 0; i < MAX_THREAD_NUM; ++i) {
		if (NULLPTR == m_aThreads[i]) {
			m_aThreads[i] = pThread;
			++m_uCount;
			return TRUE;
		}
	}

	return FALSE;
}

BOOL ThreadManager::delThread(ThreadID tid) {
	for (I32 i = 0; i < MAX_THREAD_NUM; ++i) {
		if (NULLPTR != m_aThreads[i]) {
			if (tid == m_aThreads[i]->getID()) {
				m_aThreads[i] = NULLPTR;
				--m_uCount;
				return TRUE;
			}
		} // end of if NULLPTR != m_aThreads[i]
	} // end of for
	return FALSE;
}

BOOL ThreadManager::startAllThread() {
	if (m_uCount == 0)
		return TRUE;

	for (I32 i = 0; i < MAX_THREAD_NUM; ++i) {
		if (m_aThreads[i]) {
			if (!m_aThreads[i]->start()) {
				return FALSE;
			}
		}
	}

	return TRUE;
}

BOOL ThreadManager::stopAllThread() {
	for (I32 i = 0; i < MAX_THREAD_NUM; ++i) {
		if (NULLPTR == m_aThreads[i])
			continue;
		if (!m_aThreads[i]->canSafeExit())
			return FALSE;

		m_aThreads[i]->exit();
	}

	return TRUE;
}

CThread* ThreadManager::getThread(ThreadID tid) {
	for (I32 i = 0; i < MAX_THREAD_NUM; ++i) {
		if (NULLPTR == m_aThreads[i])
			continue;

		if (tid == m_aThreads[i]->getID()) {
			return m_aThreads[i];
		}
	}

	return NULLPTR;
}

CThread* ThreadManager::getThreadByIndex(UI32 uIndex) {
	if (uIndex >= MAX_THREAD_NUM)
		return NULLPTR;
	return m_aThreads[uIndex];
}

UI32 ThreadManager::getThreadCount() {
	return m_uCount;
}
}

