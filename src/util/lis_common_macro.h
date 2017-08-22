// Copyright (c) 2017 The Lisbeth Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef __LISBASE_COMMONMACRO_H__
#define __LISBASE_COMMONMACRO_H__


#ifndef TRUE
#define TRUE true
#endif

#ifndef FALSE	
#define FALSE false
#endif

#ifndef INVALID_VALUE	
#define INVALID_VALUE (-1)
#endif

#ifndef NULLPTR	
#define NULLPTR (0)
#endif

#ifndef EXTERN_C
#define EXTERN_C extern "C"
#endif

#ifndef CRLF	
#define CRLF  "\r\n"
#endif

#ifndef CR
#define CR    '\r'
#endif

#ifndef LF
#define LF    '\n'
#endif

#ifndef IN
#define IN
#endif

#ifndef OUT
#define OUT
#endif

//=====================================================
// 类不可赋值与拷贝
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
    TypeName(const TypeName&);\
    TypeName& operator=(const TypeName&)

// 类不可构造
#define DISALLOW_CONSTRUCTOR(TypeName) \
private: \
    TypeName();\
    ~TypeName()
//=====================================================

//=====================================================
// define inline
#if defined(__GNUC__) && (__GNUC__ > 3 ||(__GNUC__ == 3 && __GNUC_MINOR__ >= 1))
# define ALWAYS_INLINE __attribute__ ((always_inline))
#else
# define ALWAYS_INLINE inline
#endif
//=====================================================


//=====================================================
// how many
#if ! defined (howmany)
#define howmany(x, y)   (((x)+((y)-1))/(y))
#endif
//=====================================================


//=====================================================
// offsetof marcro
#ifndef offsetof
/**
 * s是一个结构名，它有一个名为m的成员（s和m 是宏offsetof的形参，它实际是返回结构s的成员m的偏移地址.
 * (s *)0 是骗编译器说有一个指向类（或结构）s的指针，其地址值0
 * &((s *)0)->m   是要取得类s中成员变量m的地址. 因基址为0，这时m的地址当然就是m在s中的偏移
 */
#define offsetof(s, m) (size_t)&(((s *)0)->m)
#endif

// 内部类定义开始
#define INNER_CLASS_BEGIN(className) class className

// 内部类定义结束
#define INNER_CLASS_END(className) m_##className;

#define GET_INNER_MEMBER(className) m_##className

// 获取Outter class pointer
#define GET_OUTTER_CLASS_POINTER(outerClass, outerPointer, innerClass) \
		outerClass* (outerPointer) = ((outerClass*)((char*)(this) - offsetof(outerClass, m_##innerClass)));

#define UPCAST(ptr, type, field) \
		((type *)(((char*)(ptr))- offsetof(type, field)))
//=====================================================

#endif 
