/*
 * EventModeThread.cpp
 *
 *  Created on: 2016年6月7日
 *      Author: 58
 */

#include <wedis/base/net/EventModeThread.h>
#include <wedis/base/net/EpollMode.h>

namespace wRedis {

IMPLEMENT_SINGLETON(EventModeThread)

EventModeThread::EventModeThread()
	: CThread() {
	m_pEventMode = NULLPTR;
	Assert(init());
	Assert(create());
	Assert(start());
	m_pCoMgr = new CoroutineManager();
}

EventModeThread::~EventModeThread() {
	W_DELETE(m_pEventMode);
}

BOOL EventModeThread::init() {
	m_pEventMode = new EpollMode(1024, 32000);
	Assert(m_pEventMode);

	return m_pEventMode->create();
}

void EventModeThread::run() {
	Assert(m_pEventMode);

	while(1) {
		m_pEventMode->dispatch(30);
//		if(FALSE == m_pEventMode->dispatch(30)) {
//			break;
//		}
	}
}

} /* namespace wRedis */
