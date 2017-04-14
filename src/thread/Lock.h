//
// Created by 58 on 2016/5/4.
//

#ifndef WREDISCLIENT_LOCK_H
#define WREDISCLIENT_LOCK_H

#include <sys/time.h>
#include <wedis/base/util/Atomic.h>
#include <wedis/base/util/CommonType.h>
#include <wedis/base/util/Noncopyable.h>
#include <wedis/base/util/SystemTime.h>

namespace wRedis {
    class Lock {
    public:
        Lock( ){ pthread_mutex_init( &m_Mutex , NULLPTR );} ;
        ~Lock( ){ pthread_mutex_destroy( &m_Mutex) ;} ;
    public:
        void	lock( ){ pthread_mutex_lock(&m_Mutex);} ;
        void	unlock( ){ pthread_mutex_unlock(&m_Mutex);} ;
        BOOL	trylock(){ if(pthread_mutex_trylock(&m_Mutex)==0) return TRUE; return FALSE; };

    public:
        pthread_mutex_t* getMutex() { return &m_Mutex; }
    private:
        pthread_mutex_t 	m_Mutex; //共享锁变量
    };

    class LockGuard : public Noncopyable {
    public:
    	explicit LockGuard(Lock& rLock) {
            m_pLock = &rLock;
            m_pLock->lock();
        }
        ~LockGuard() { m_pLock->unlock(); }
    private:
        Lock* m_pLock;
    };

    class Condition : public Noncopyable {
    public:
    	explicit Condition(Lock& lock, BOOL bRealTime = TRUE) : m_lock(lock), m_bRealTime(bRealTime) {
    		if(bRealTime) {
    			pthread_cond_init(&m_cond, NULLPTR);
    		}
    		else {
    			Assert(pthread_condattr_init(&m_cond_attr));
    			Assert(pthread_condattr_setclock(&m_cond_attr, CLOCK_MONOTONIC));
    			Assert(pthread_cond_init(&m_cond, &m_cond_attr));
    		}
    	}
        virtual ~Condition() {
        	pthread_condattr_destroy(&m_cond_attr);
        	pthread_cond_destroy(&m_cond);
        }

    public:
        I32 wait() { return pthread_cond_wait(&m_cond, m_lock.getMutex()); }
        I32 wait(const TIMET timeout) {
        	EM_CLOCK_TYPE eType = m_bRealTime ? EM_CLOCK_REALTIME : EM_CLOCK_MONOTONIC;
        	timespec ts = SystemTime::add(eType, timeout);
        	return pthread_cond_timedwait(&m_cond, m_lock.getMutex(), &ts);
        }
        void notifyAll() { pthread_cond_broadcast(&m_cond); }
        void notify() { pthread_cond_signal(&m_cond); }
    private:
        Lock& m_lock;
        BOOL  m_bRealTime;
        pthread_cond_t m_cond;
        pthread_condattr_t m_cond_attr;
    };

    class RWLock : public Noncopyable {
    public:
        RWLock();
        ~RWLock();
    public:
        BOOL readLock();
        BOOL writeLock(BOOL isPrio = FALSE);
        void readUnLock();
        void writeUnLock();
    private:
        pthread_rwlock_t m_RWLock; // the underlying platform mutex
    };

    class SpinLock : public Noncopyable {
    public:
      	SpinLock( ){ pthread_spin_init( &m_SpinLock , NULLPTR ); }
        ~SpinLock( ){ pthread_spin_destroy(&m_SpinLock); }
    public:
        void	lock( ){ pthread_spin_lock(&m_SpinLock);}
        void	unlock( ){ pthread_spin_unlock(&m_SpinLock);}
        BOOL	trylock(){ if(pthread_spin_trylock(&m_SpinLock)==0) return TRUE; return FALSE; }

    public:
        pthread_spinlock_t* getSpinLock() { return &m_SpinLock; }
    private:
        pthread_spinlock_t 	m_SpinLock; //共享锁变量
    };

    class ReadLockGuard : public Noncopyable {
    public:
      	explicit ReadLockGuard(RWLock& rLock) {
            m_pLock = &rLock;
            m_pLock->readLock();
        }
        ~ReadLockGuard() { m_pLock->readUnLock(); }
    private:
        RWLock* m_pLock;
    };

    class WriteLockGuard : public Noncopyable {
    public:
       	explicit WriteLockGuard(RWLock& rLock) {
            m_pLock = &rLock;
            m_pLock->writeLock();
        }
        ~WriteLockGuard() { m_pLock->writeLock(); }
    private:
        RWLock* m_pLock;
    };

    class RWFreeLock : public Noncopyable {
    public:
    	RWFreeLock();
    	~RWFreeLock();
    public:
        BOOL readLock();
        BOOL writeLock();
        void readUnLock();
        void writeUnLock();
    private:
        atomic_t m_nLockCnt;
        atomic_t m_nWriteWaitCnt;

    };

//    class SpinLock : public Noncopyable {
//    public:
//        SpinLock() {
//            atomic_set(m_Lock, 0);
//        }
//
//        BOOL trylock() {
//        	bool ret = !std::atomic_flag_test_and_set_explicit(&lck, std::memory_order_acquire);
//            auto currently_locked = locked_.load(std::memory_order_relaxed);
//            return !currently_locked &&
//                   locked_.compare_exchange_weak(currently_locked, true,
//                                                 std::memory_order_acquire,
//                                                 std::memory_order_relaxed);
//        }
//
//        void lock() {
//        	while (std::atomic_flag_test_and_set_explicit(&lck, std::memory_order_acquire)) ;
//            for (size_t tries = 0;; ++tries) {
//                if (try_lock()) {
//                    // success
//                    break;
//                }
//                port::AsmVolatilePause();
//                if (tries > 100) {
//                    std::this_thread::yield();
//                }
//            }
//        }
//
//        void unlock() {
//        	std::atomic_flag_clear_explicit(&lck, std::memory_order_release);
//        	locked_.store(false, std::memory_order_release);
//        }
//
//    private:
//        atomic_t m_Lock;
//    };


}



#endif //WREDISCLIENT_LOCK_H
