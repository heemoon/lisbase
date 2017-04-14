/*
 * Event.cpp
 *
 *  Created on: 2016年6月6日
 *      Author: 58
 */

#include <wedis/base/net/Event.h>

namespace wRedis {

Event::Event(EM_EVENT_TYPE eType)
	: m_eType(eType) {
	m_Mask = 0;
}

Event::~Event() {
}

void Event::proc(I16 ev) {
	if(ev & EM_EV_RESULT_ERR) {
		// proc err
		onErr();
		return;
	}
	else if(ev & EM_EV_RESULT_TIMEOUT) {
		onTimeout();
		return;
	}

	if(ev & EM_EV_OP_READ) {
		onRead();
	}

	if(ev & EM_EV_OP_WRITE) {
		onWrite();
	}
}


} /* namespace wRedis */
