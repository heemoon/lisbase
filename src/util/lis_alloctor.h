// Copyright (c) 2017 The Lisbeth Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef __LISBASE_ALLOCTOR_H__
#define __LISBASE_ALLOCTOR_H__

#include <stdlib.h>

// 是否使用 tcmalloc 判断版本并定义获取 malloc size
#if defined(USE_TCMALLOC)
#define LIS_MALLOC_LIB ("tcmalloc-" #TC_VERSION_MAJOR "." #TC_VERSION_MINOR)
#include <google/tcmalloc.h>
#if (TC_VERSION_MAJOR == 1 && TC_VERSION_MINOR >= 6) || (TC_VERSION_MAJOR > 1)
#define HAVE_MALLOC_SIZE 1
#define lis_malloc_size(p) tc_malloc_size(p)
#else
#error "Newer version of tcmalloc required"
#endif

// 是否使用 jemalloc  判断版本并定义获取 malloc size
#elif defined(USE_JEMALLOC)
#define LIS_MALLOC_LIB ("jemalloc-" #JEMALLOC_VERSION_MAJOR "." #JEMALLOC_VERSION_MINOR "." #JEMALLOC_VERSION_BUGFIX)
#include <jemalloc/jemalloc.h>
#if (JEMALLOC_VERSION_MAJOR == 2 && JEMALLOC_VERSION_MINOR >= 1) || (JEMALLOC_VERSION_MAJOR > 2)
#define HAVE_MALLOC_SIZE 1
#define lis_malloc_size(p) je_malloc_usable_size(p)
#else
#error "Newer version of jemalloc required"
#error "Newer version of jemalloc required"
#endif
#endif

#ifndef LIS_MALLOC_LIB
#define LIS_MALLOC_LIB "libc"
#endif

// 覆盖基础的内存接口
#if defined(USE_TCMALLOC)
#define malloc(size) tc_malloc(size)
#define calloc(count,size) tc_calloc(count,size)
#define realloc(ptr,size) tc_realloc(ptr,size)
#define free(ptr) tc_free(ptr)
#elif defined(USE_JEMALLOC)
#define malloc(size) je_malloc(size)
#define calloc(count,size) je_calloc(count,size)
#define realloc(ptr,size) je_realloc(ptr,size)
#define free(ptr) je_free(ptr)
#endif

#ifndef LIS_MALLOC
#define LIS_MALLOC(size) malloc((size))
#endif

#ifndef LIS_CALLOC
#define LIS_CALLOC(count, size) calloc((count), (size))
#endif

#ifndef LIS_REALLOC
#define LIS_REALLOC(ptr, size) realloc((ptr), (size))
#endif

#ifndef LIS_FREE
#define LIS_FREE(x)	if( (x)!=NULLPTR ) { free (x); (x)=NULLPTR; }
#endif

//根据指针值删除内存(new分配空间)
#ifndef LIS_DELETE
#define LIS_DELETE(x)	if( (x)!=NULLPTR ) { delete (x); (x)=NULLPTR; }
#endif

//根据指针值删除数组类型内存(new分配块空间)
#ifndef LIS_DELETE_ARRAY
#define LIS_DELETE_ARRAY(x)	if( (x)!=NULLPTR ) { delete[] (x); (x)=NULLPTR; }
#endif

#endif
