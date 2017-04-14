/*
 * EpollMode.h
 *
 *  Created on: 2016年6月6日
 *      Author: 58
 */

#ifndef WEDIS_BASE_NET_EPOLLMODE_H_
#define WEDIS_BASE_NET_EPOLLMODE_H_

#include "wedis/base/net/EventMode.h"
#include <sys/epoll.h>

// epoll 结构体
//struct epoll_event
//{
//  uint32_t events;  /* Epoll events */
//  epoll_data_t data;    /* User data variable */
//} __attribute__ ((__packed__));
//
//typedef union epoll_data
//{
//  void *ptr;
//  int fd;
//  uint32_t u32;
//  uint64_t u64;
//} epoll_data_t;

namespace wRedis {

// 通知内核需要监听size个fd
#define MAX_EPOLL_SIZE 32000

// 初始化 epoll_event 数组大小
#define INITIAL_EPOLL_NEVENT 1024

// epoll_event 最大扩容大小，就是同一时刻 epoll_wait 唤醒的fd最大数量
#define MAX_EPOLL_NEVENT 4096

class EpollMode : public EventMode {
public:
	EpollMode(I32 initSize, I32 maxSize);
	virtual ~EpollMode();
public:
	virtual BOOL create();
	virtual BOOL add(Event* e, I16 ev);
	virtual BOOL del(I32 fd);

	virtual BOOL ctl(I32 fd, I16 ev);
	virtual BOOL dispatch(I64 timeOut);
private:
	// epoll_ctl error 处理接口
	BOOL __dealErr(I32 op, I32 fd, struct epoll_event* pEe);
private:
	I32 m_Epfd;
	struct epoll_event* m_aEpollEvents;
};

} /* namespace wRedis */

#endif /* WEDIS_BASE_NET_EPOLLMODE_H_ */
