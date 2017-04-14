//
// Created by 58 on 2016/5/4.
//

#include <wedis/base/thread/Lock.h>

namespace wRedis {
    RWLock::RWLock() {
        pthread_rwlock_init(&m_RWLock, NULLPTR);
    }

    RWLock::~RWLock() {
        pthread_rwlock_destroy(&m_RWLock);
    }

    BOOL RWLock::readLock() {
        return  pthread_rwlock_rdlock(&m_RWLock) == 0;
    }

    BOOL RWLock::writeLock(BOOL isPrio/* = FALSE*/) {
    	if(isPrio)
    		pthread_setschedprio(pthread_self( ), SCHED_FIFO);

        return pthread_rwlock_wrlock(&m_RWLock) == 0;
    }

    void RWLock::readUnLock() {
        pthread_rwlock_unlock(&m_RWLock);
    }

    void RWLock::writeUnLock() {
    	// 是否要重置 线程优先级 ？？？
    	pthread_rwlock_unlock(&m_RWLock);
    }

}
