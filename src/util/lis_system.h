// Copyright (c) 2017 The Lisbeth Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef __LISBASE_UTIL_SYSTEM_H__
#define __LISBASE_UTIL_SYSTEM_H__

#include <stdint.h>
#include <sys/types.h>

namespace lisbase {
namespace System {
/**
 * 获取进程号
 */
pid_t pid();

/**
 * 当前线程id
 */
thread_t currentThreadId();

/**
 * cpu count
 */
int cpuCount();

/**
 * 内存大小
 */
int memorySize();

/**
 * sleep
 */
void sleep(time_t millionseconds = 0);

/**
 * dump stack
 */
void dumpStack(const char* reason);

/**
 * set rlimit file size
 */
void setRLimitFileSize(const int maxFd);

/**
 * check process is exist
 */
bool checkProcessExist(pid_t pid);
}

}
#endif
