/*
 * ThreadLocal.h
 *
 *  Created on: 2016年6月1日
 *      Author: 58
 */

#ifndef BASE_THREAD_THREADLOCAL_H_
#define BASE_THREAD_THREADLOCAL_H_

#include <wedis/base/util/Noncopyable.h>
#include <pthread.h>

namespace wRedis {

template<typename T>
class ThreadLocal : Noncopyable {
public:
	ThreadLocal() {
		pthread_key_create(&m_Key, &ThreadLocal::destructor);
	}

	~ThreadLocal() {
		pthread_key_delete(m_Key);
	}

public:
	T& value() {
		T* perThreadValue = static_cast<T*>(pthread_getspecific(m_Key));
	    if(!perThreadValue) {
	    	T* newObj = new T();
	    	pthread_setspecific(m_Key, newObj);
	    	perThreadValue = newObj;
	    }
	    return *perThreadValue;
	}

private:
	static void destructor(void *x)	{
		T* obj = static_cast<T*>(x);
		typedef I8 T_must_be_complete_type[sizeof(T) == 0 ? -1 : 1];
	    T_must_be_complete_type dummy; (void) dummy;
	    W_DELETE(obj);
	}

private:
	pthread_key_t m_Key;
};

} /* namespace wRedis */

#endif /* BASE_THREAD_THREADLOCAL_H_ */
