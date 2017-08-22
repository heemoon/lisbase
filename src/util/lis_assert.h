// Copyright (c) 2017 The Lisbeth Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef __LISBASE_ASSERTX_H__
#define __LISBASE_ASSERTX_H__

#include <stdarg.h>  /* for va_list */
#include <stdint.h>  /* for uint32_t */
#include "lis_common_macro.h"

namespace lisbase {
/**
 * GCC(version>=2.9)
 * __builtin_expect
 * gcc need param: -fprofile-arcs
 */
#ifndef __builtin_expect
#define __builtin_expect(x, expected_value) (x)
#endif

#ifndef likely
#define likely(x)  __builtin_expect(!!(x), 1)
#endif

#ifndef unlikely
#define unlikely(x)  __builtin_expect(!!(x), 0)
#endif

#ifndef LIS_LIKELY
#define LIS_LIKELY(x) likely(x)
#endif

#ifndef LIS_UNLIKELY
#define LIS_UNLIKELY(x) unlikely(x)
#endif

/**
 * assert
 */
void __show__(const char* szTemp);
void __assert__(const char* file, uint32_t line, const char* func, const char* expr);
void __assertspecial__(const char* file, uint32_t line, const char* func, const char* expr, const char* msg);
void __assert__format__(const char* file, uint32_t line, const char* func, const char* expr, const char* format, ...);

#define Assert(expr) {if(!(expr)){__assert__(__FILE__,(uint32_t)__LINE__,__PRETTY_FUNCTION__,#expr);}}
#define AssertSpecial(expr,msg) {if(!(expr)){__assertspecial__(__FILE__,(uint32_t)__LINE__,__PRETTY_FUNCTION__,#expr,msg);}}
#define Assertf(expr, formatString, args...) if(!(expr)){__assert__format__(__FILE__,(uint32_t)__LINE__,__PRETTY_FUNCTION__,#expr, formatString, ##args);}
#define AssertRetFalse(expr)  do {Assert(expr);if(!(expr))return FALSE;} while(0);
#define AssertRetNull(expr) do {Assert(expr);if(!(expr))return NULLPTR;} while(0);
#define AssertRet(expr, ret) do {Assert(expr);if(!(expr))return (ret);} while(0);
#define AssertRetEmpty(expr) do {Assert(expr);if(!(expr))return;} while(0);
#define NOT_REACHED() Assert(0)


/**
 * try catch throw
 *
 * eg:
 * void func() {
 *     __ENTER_FUNCTION
 *     AssertWithThrow(0);
 *     //AssertSpecialWithThrow(0, "i want throw!!!");
 *     __LEAVE_FUNCTION
 * }
 */
void __show_throw__(const char* szTemp);
void __assert_throw__(const char* file, uint32_t line, const char* func, const char* expr);
void __assertspecial_throw__(const char* file, uint32_t line, const char* func, const char* expr, const char* msg);
#define AssertWithThrow(expr) {if(!(expr)){__assert_throw__(__FILE__,(uint32_t)__LINE__,__PRETTY_FUNCTION__,#expr);}}
#define AssertSpecialWithThrow(expr,msg) {if(!(expr)){__assertspecial_throw__(__FILE__,(uint32_t)__LINE__,__PRETTY_FUNCTION__,#expr,msg);}}

#define __ENTER_FUNCTION {try{
#define __LEAVE_FUNCTION }catch(...){AssertSpecialWithThrow(FALSE,__PRETTY_FUNCTION__);}}

}

#endif
