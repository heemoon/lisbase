// Copyright (c) 2017 The Lisbeth Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef __LISBASE_TSINGLETON_H__
#define __LISBASE_TSINGLETON_H__

#include <pthread.h>

#include "lis_alloctor.h"
#include "lis_noncopyable.h"

namespace lisbase {
// This doesn't detect inherited member functions!
// http://stackoverflow.com/questions/1966362/sfinae-to-check-for-inherited-member-functions
//template<typename T>
//struct has_no_destroy {
//	template<typename C> static char test(typeof(&C::no_destroy)); // or decltype in C++11
//	template<typename C> static I32 test(...);
//	const static BOOL value = sizeof(test<T>(0)) == 1;
//};

template<typename T> class TSingleton: public Noncopyable {
public:
	static T& getSingleton() {
		if (NULLPTR == sm_Singleton) {
			pthread_once(&sm_ponce, &TSingleton::init);
			Assert(sm_Singleton != NULLPTR);
		}

		return (*sm_Singleton);
	}

	static T* getSingletonPtr() {
		if (NULLPTR == sm_Singleton) {
			pthread_once(&sm_ponce, &TSingleton::init);
		}
		return sm_Singleton;
	}
protected:
	TSingleton();
	~TSingleton();
private:
	static void init() {
		sm_Singleton = new T();
//		if (!has_no_destroy<T>::value) {
//			::atexit(destroy);
//		}
	}

//	static void destroy() {
//		typedef char T_must_be_complete_type[sizeof(T) == 0 ? -1 : 1];
//		T_must_be_complete_type dummy;
//		(void) dummy;  // for 让编译器检查错误
//
//		W_DELETE(sm_Singleton);
//	}
private:
	static pthread_once_t sm_ponce;
	static T* sm_Singleton;
};

template<typename T>
pthread_once_t TSingleton<T>::sm_ponce = PTHREAD_ONCE_INIT;

template<typename T>
T* TSingleton<T>::sm_Singleton = NULLPTR;
}

#define DECLARE_SINGLETON(className) \
	public: \
	static className& getSingleton(); \
	static className* getSingletonPtr();

#define IMPLEMENT_SINGLETON(className) \
	className& className::getSingleton(){	static className m_Instance; return m_Instance; } \
	className* className::getSingletonPtr() { return &getSingleton(); }

#define IMPLEMENT_SINGLETON_T(className) \
	className& className::getSingleton(){	return TSingleton<className>::getSingleton(); } \
	className* className::getSingletonPtr() { return TSingleton<className>::getSingletonPtr(); }
#endif
