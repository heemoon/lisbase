
#include <time.h>
#include "lis_system_time.h"
#include "lis_assert.h"

namespace lisbase {
	IMPLEMENT_SINGLETON(SystemTime)

	SystemTime::SystemTime() {
		m_DeltaMillTime = 0;
		struct timeval tv;
		::gettimeofday(&tv, &m_sTimeZone);
		m_BeginMillTime = tv.tv_sec * MILLISECONDS_PER_SECOND + tv.tv_usec / MICROSECONDS_PER_MILLSECOND;
	}

	SystemTime::~SystemTime() {
	}

	// Epoch 1970-1-1 00:00:00
	TIMET SystemTime::nowMillSec() {
		struct timeval tv;
		::gettimeofday(&tv, NULLPTR);
		return tv.tv_sec * MILLISECONDS_PER_SECOND + tv.tv_usec / MICROSECONDS_PER_MILLSECOND;
	}

	// Epoch 1970-1-1 00:00:00
	TIMET SystemTime::nowMicroSec() {
		struct timeval tv;
		::gettimeofday(&tv, NULLPTR);
		return tv.tv_sec * MICROSECONDS_PER_SECOND + tv.tv_usec;
	}

	// Epoch 1970-1-1 00:00:00
	TIMET SystemTime::nowSec() {
		time_t ttime;
		::time(&ttime);
		return (TIMET)ttime;
	}

	void SystemTime::local(TIMET sec, OUT struct tm* pTM) {
		Assert(pTM);
		::localtime_r((const time_t*)&sec, pTM);
	}

	void SystemTime::gmt(TIMET sec, OUT struct tm* pTM) {
		Assert(pTM);
		::gmtime_r((const time_t*)&sec, pTM);
	}

	TIMET SystemTime::diff(const struct timeval &tv1, const struct timeval &tv2) {
		return ((TIMET)(tv1.tv_sec - tv2.tv_sec - 1))*MILLISECONDS_PER_SECOND + (1000000 + tv1.tv_usec - tv2.tv_usec)/MICROSECONDS_PER_MILLSECOND;
	}

	timespec SystemTime::diff(timespec& ts1, timespec& ts2) {
		timespec ret;
		if((ts1.tv_nsec - ts2.tv_nsec) < 0) {
			ret.tv_sec = ts1.tv_sec - ts2.tv_sec - 1;
			ret.tv_nsec = NANOSECONDS_PER_SECOND + ts1.tv_nsec - ts2.tv_nsec;
		} else {
		    ret.tv_sec = ts1.tv_sec - ts2.tv_sec;
		    ret.tv_nsec = ts1.tv_nsec - ts2.tv_nsec;
		}

		return ret;
	}

	timespec SystemTime::add(timespec& ts1, timespec& ts2) {
		timespec ret;
		ret.tv_sec = ts1.tv_sec  + ts2.tv_sec;
		ret.tv_nsec = ts1.tv_nsec + ts2.tv_nsec;

		ret.tv_sec += ret.tv_nsec / NANOSECONDS_PER_SECOND;
		ret.tv_nsec = ret.tv_nsec % NANOSECONDS_PER_SECOND;

		return ret;
	}

	timespec SystemTime::add(EM_CLOCK_TYPE eType, TIMET millSeconds) {
		timespec ts;
		ts.tv_sec = millSeconds / SystemTime::MILLISECONDS_PER_SECOND;
		ts.tv_nsec = (millSeconds % SystemTime::MILLISECONDS_PER_SECOND) * SystemTime::NANOSECONDS_PER_MILLSECOND;

		timespec now = {0, 0};
		::clock_gettime((clockid_t) eType, &now);

		return add(ts, now);
	}

	timespec SystemTime::getCurTimespec(EM_CLOCK_TYPE eType) {
		timespec ts = {0, 0};
		::clock_gettime((clock_t) eType, &ts);
		return ts;
	}

	TIMET SystemTime::getSecFormatBySec(TIMET curSec) {
		struct tm v;
		SystemTime::local(curSec, &v);
		TIMET year = v.tm_year + 1900;
		int month = v.tm_mon + 1;
		int day = v.tm_mday;
		int hour = v.tm_hour;
		int min = v.tm_min;
		int sec = v.tm_sec;

		return year * 10000000000 + month * 100000000 + day * 1000000 + hour * 10000 + min * 100 + sec;
	}

	void SystemTime::local(struct tm* pTM) {
		TIMET curSec = getCurSec();
		local(curSec, pTM);
	}

	void SystemTime::gmt(struct tm* pTM) {
		TIMET curSec = getCurSec();
		gmt(curSec, pTM);
	}






}

