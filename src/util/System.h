/*
 * System.h
 *
 *  Created on: 2016年5月13日
 *      Author: 58
 */

#ifndef UTIL_SYSTEM_H_
#define UTIL_SYSTEM_H_

#include <sys/types.h>
#include <wedis/base/util/CommonType.h>

namespace wRedis {
namespace System {
/**
 * 获取进程号
 */
pid_t pid();

/**
 * 当前线程id
 */
ThreadID currentThreadId();

/**
 * cpu count
 */
I32 cpuCount();

/**
 * 内存大小
 */
I32 memorySize();

/**
 * sleep
 */
void sleep(TIMET millionseconds = 0);

/**
 * dump stack
 */
void dumpStack(const I8* reason);

/**
 * set rlimit file size
 */
void setRLimitFileSize(const I32 maxFd);

/**
 * get ip
 */
UI32 getIP(const I8* ifstr);

/**
 * get eth0 ip
 */
I8* getEth0IP();

/**
 * check process is exist
 */
BOOL checkProcessExist(pid_t pid);



}

}
#endif /* UTIL_SYSTEM_H_ */
