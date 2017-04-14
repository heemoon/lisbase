/*
 * Event.h
 *
 *  Created on: 2016年6月6日
 *      Author: 58
 */

#ifndef WEDIS_BASE_NET_EVENT_H_
#define WEDIS_BASE_NET_EVENT_H_

#include <wedis/base/util/CommonType.h>

namespace wRedis {

enum EM_EVENT_TYPE {
	EM_EV_IO,                // socket fd
	EM_EV_TIMER,             // timer  fd
	EM_EV_SIGNAL,            // signal fd
};

enum EM_EVENT_OP_TYPE {
	EM_EV_OP_NONE = 0,
	EM_EV_OP_READ = 1,
	EM_EV_OP_WRITE = 2,
};

enum EM_EVENT_RESULT_TYPE {
	EM_EV_RESULT_ERR = 0,
	EM_EV_RESULT_READ = 1,
	EM_EV_RESULT_WRITE = 2,
	EM_EV_RESULT_TIMEOUT = 4,
};

class Event {
public:
	Event(EM_EVENT_TYPE eType);
	virtual ~Event();
protected:
	virtual void onRead()    = 0;
	virtual void onWrite()   = 0;
	virtual void onErr()     = 0;
	virtual void onTimeout() = 0;
public:
	virtual I32  getFd()     = 0;
	void proc(I16 ev);
public:
	// event 类型
	EM_EVENT_TYPE m_eType;
	// mask
	I16 m_Mask;
};

} /* namespace wRedis */

#endif /* WEDIS_BASE_NET_EVENT_H_ */
