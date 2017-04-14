/*
 * EventMode.cpp
 *
 *  Created on: 2016年6月6日
 *      Author: 58
 */

#include <wedis/base/net/EventMode.h>
#include <wedis/base/util/Alloctor.h>
#include <wedis/base/util/System.h>

namespace wRedis {
EventMode::EventMode(EM_EVENT_MODE eMode, I32 initSize, I32 maxSize)
	: m_eMode(eMode), m_aEvents(NULLPTR), m_nEventSize(initSize), m_nMaxEventSize(maxSize) {
}

EventMode::~EventMode() {
	W_FREE(m_aEvents);
	m_nEventSize = 0;
}

BOOL EventMode::create() {
	m_aEvents = (Event**) W_CALLOC(m_nEventSize, sizeof(Event*));
	if(NULLPTR == m_aEvents) {
		return FALSE;
	}
	System::setRLimitFileSize(m_nMaxEventSize);
	return TRUE;
}

} /* namespace wRedis */
