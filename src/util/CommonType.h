//
// Created by 58 on 2016/4/18.
//

#ifndef __COMMONTYPE_H__
#define __LISBASE_UTIL_COMMONTYPE_H__
#define __LISBASE_UTIL_COMMONTYPE_H__

#include <pthread.h>
#include "Atomic.h"

namespace lisbase {


typedef char I8;
typedef unsigned char UI8;
typedef short I16;
typedef unsigned short UI16;
typedef int I32;
typedef unsigned int UI32;
typedef long long I64;
typedef unsigned long long UI64;
typedef bool BOOL;
typedef float REAL32;
typedef double REAL64;

typedef I64 TIMET;

typedef pthread_t ThreadID;

#define TRUE true
#define FALSE false
#define INVALID_VALUE (-1)
#define NULLPTR (0)
#define EXTERN_C extern "C"
#define CRLF  "\r\n"
#define CR    '\r'
#define LF    '\n'


typedef	atomic_t		atword;
typedef	atomic64_t		atwordu64;

//#define initword(v, _Idx)		(((v).atomic) = (_Idx))
#define SYMBOL_DECLSPEC

#define IN
#define OUT

}

#endif 
