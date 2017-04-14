/*
 * EventMode.h
 *
 *  Created on: 2016年6月6日
 *      Author: 58
 */

#ifndef WEDIS_BASE_NET_EVENTMODE_H_
#define WEDIS_BASE_NET_EVENTMODE_H_

#include <wedis/base/net/Event.h>

namespace wRedis {

enum EM_EVENT_MODE {
	EM_EVENT_EPOLL,
	EM_EVENT_POLL,
	EM_EVENT_SELECT,  // 自带 socket 超时功能，可读取errfds 来判断超时
};

class EventMode {
public:
	EventMode(EM_EVENT_MODE eMode, I32 initSize, I32 maxSize);
	virtual ~EventMode();

public:
	virtual BOOL create();
	virtual BOOL add(Event* e, I16 ev) = 0;
	virtual BOOL del(I32 fd) = 0;
	virtual BOOL ctl(I32 fd, I16 ev) = 0;
	virtual BOOL dispatch(I64 timeOut) = 0;
public:
	inline EM_EVENT_MODE mode() { return m_eMode; }
protected:
	// 使用的模式
	EM_EVENT_MODE m_eMode;
	// events array, fd 直接索引
	Event** m_aEvents;
	// 当前 aEvents 大小
	I32 m_nEventSize;
	// 最大Events大小
	I32 m_nMaxEventSize;

	// Timer 管理器
	// fd --- 超时时间
	// 会有很多fd 的 超时， 在每次调用完后 getTimeout时会check一下 expired ！！！
	// 一旦超时则关闭连接对应的之前发送过的所有消息全都要返回超时错误。

};

} /* namespace wRedis */

#endif /* WEDIS_BASE_NET_EVENTMODE_H_ */
