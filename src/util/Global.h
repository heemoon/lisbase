//
// Created by 58 on 2016/4/18.
//

#ifndef WREDISCLIENT_GLOBAL_H
#define WREDISCLIENT_GLOBAL_H


#include <stdlib.h>
#include <stddef.h>
#include <wedis/base/util/CommonType.h>
#include <wedis/base/util/Assertx.h>

namespace wRedis {

//=====================================================
// 定义常量值
const static REAL32 Epsilon = 1e-06f;                               //REAL32 精度
const static REAL32 PositiveInfinity = 3.402823466e+38f;            //REAL32 最大值
const static I32 MAX_IP_SIZE = 64;                               //IPv4 字符串格式的最大长度 255.255.255.255\0
const static I32 MAX_FILE_NAME_LEN = 64;
const static I32 W_INT_MAX = 0x7fffffff;                            // int 最大值

// 定义无效返回值
const static UI8 INVALID_UI8 = (UI8)INVALID_VALUE;                 //无效的无符号I8型
const static UI32 INVALID_UI32 = (UI32)INVALID_VALUE;               //无效的无符号整形
const static TIMET INVALID_TIME = (TIMET)INVALID_VALUE;             //无效时间

// redis 基础回复定义
static const I8* QUEUED = "QUEUED";
static const I8* PONG = "PONG";
static const I8* OK = "OK";
static const I8* NIL = "nil";
static const I8* STRING_EMPTY = "";
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


//=====================================================
// default logger function
extern void log_debug(const I8* str);
extern void log_info(const I8* str);
extern void log_warn(const I8* str);
extern void log_error(const I8* str);
//=====================================================


}

#endif //WREDISCLIENT_GLOBAL_H
