/*
 * EventModeThread.h
 *
 *  Created on: 2016年6月7日
 *      Author: 58
 */

#ifndef WEDIS_BASE_NET_EVENTMODETHREAD_H_
#define WEDIS_BASE_NET_EVENTMODETHREAD_H_

#include <wedis/base/thread/CThread.h>
#include <wedis/base/net/EventMode.h>
#include <wedis/base/util/TSingleton.h>
#include <wedis/base/coroutine/CoroutineManager.h>

namespace wRedis {

class EventModeThread : public CThread {
DECLARE_SINGLETON(EventModeThread);
public:
    BOOL init();
    inline EventMode* getMode() { return m_pEventMode; }
public:
    virtual void run();
    virtual BOOL canSafeExit() { return FALSE; }

public:
    CoroutineManager* coMgr() { return m_pCoMgr; }
private:
	EventModeThread();
	virtual ~EventModeThread();
private:
    EventMode* m_pEventMode;

    // test
    CoroutineManager* m_pCoMgr;
};

} /* namespace wRedis */

#endif /* WEDIS_BASE_NET_EVENTMODETHREAD_H_ */
