//
// Created by 58 on 2016/5/10.
//

#ifndef WREDISCLIENT_BLOCKINGQUEUE_H
#define WREDISCLIENT_BLOCKINGQUEUE_H

#include <list>
#include <errno.h>
#include <wedis/base/thread/Lock.h>
#include <wedis/base/util/Atomic.h>
#include <wedis/base/util/CommonType.h>
#include <wedis/base/util/Global.h>
#include <wedis/base/util/Log.h>
#include <wedis/base/util/Noncopyable.h>

namespace wRedis {
    template<typename T>
    class BlockingQueue : public Noncopyable{
    public:
        BlockingQueue() : m_Lock(), m_Cond(m_Lock), m_List() {
        }
        virtual ~BlockingQueue() {}

    public:
        void put(const T& node) {
            {
                LockGuard guard(m_Lock);
                m_List.push_back(node);
            }

            m_Cond.notify();
        }

        void putFront(const T& node) {
        	{
        		LockGuard guard(m_Lock);
        		m_List.push_front(node);
        	}

            m_Cond.notify();
        }

        T take() {
        	LockGuard guard(m_Lock);
            while(m_List.empty()) {
                if(W_UNLIKELY(0 != m_Cond.wait())) {
                    Logger.error("BlockingQueue take cond wait fialed. errno:%d.", errno);
                }
            }

            Assert(!m_List.empty());
#ifdef __GXX_CXX0X__
        	T outVal(std::move(m_List.front()));
#else
        	T outVal(m_List.front());
#endif
            m_List.pop_front();
            return outVal;
        }

        T takeWithTimeOut(TIMET timeout) {
        	{
        	LockGuard guard(m_Lock);
            while(m_List.empty()) {
            	if(timeout > 0) {
            		I32 ret = 0;
            		if(0 != (ret = m_Cond.wait(timeout))) {
            			if(ETIMEDOUT == ret) {
            				break;
            			}
            			Logger.error("BlockingQueue take cond wait fialed. errno:%d.", errno);
            		}
            	}
            	else {
            		if(W_UNLIKELY(0 != m_Cond.wait())) {
            			Logger.error("BlockingQueue take cond wait fialed. errno:%d.", errno);
            		}
            	}
            }

            if(!m_List.empty()) {
#ifdef __GXX_CXX0X__
        		T outVal(std::move(m_List.front()));
#else
        		T outVal(m_List.front());
#endif
            	m_List.pop_front();
            	return outVal;
            }

        	}
            throw 1;
        }

        T poll() {
        	{
        		LockGuard guard(m_Lock);
        		if(!m_List.empty()) {
#ifdef __GXX_CXX0X__
        			T outVal(std::move(m_List.front()));
#else
        			T outVal(m_List.front());
#endif
        			m_List.pop_front();
        			return outVal;
        		}
        	}

            throw 1;
        }

        void remove(T& rVal) {
        	LockGuard guard(m_Lock);
        	m_List.remove(rVal);
        }

        I32 size() {
        	LockGuard guard(m_Lock);
            return m_List.size();
        }

    private:
        mutable Lock m_Lock;
        Condition    m_Cond;
        std::list<T > m_List;
    };
}



#endif //WREDISCLIENT_BLOCKINGQUEUE_H
