#ifndef __LISBASE_SYSTEMTIME_H__
#define __LISBASE_SYSTEMTIME_H__

#include <sys/time.h>
#include "lis_singleton.h"
#include "lis_common_macro.h"

namespace lisbase {
	typedef uint64_t TIMET;

	enum EM_CLOCK_TYPE {
		EM_CLOCK_REALTIME        = 1,	//CLOCK_REALTIME  系统实时时间,随系统实时时间改变而改变,即从UTC1970-1-1 0:0:0开始计时, 中间时刻如果系统时间被用户改成其他,则对应的时间相应改变
		EM_CLOCK_PROCESS_CPUTIME = 2,   // if defined (_POSIX_CPUTIME)         CLOCK_PROCESS_CPUTIME_ID  本进程到当前代码系统CPU花费的时间
		EM_CLOCK_THREAD_CPUTIME  = 3,   // if defined (_POSIX_THREAD_CPUTIME)  CLOCK_THREAD_CPUTIME_ID   本线程到当前代码系统CPU花费的时间
		EM_CLOCK_MONOTONIC       = 4    // if defined (_POSIX_MONOTONIC_CLOCK) CLOCK_MONOTONIC           从系统启动这一刻起开始计时,不受系统时间被用户改变的影响
	};

	class SystemTime {
	DECLARE_SINGLETON(SystemTime)
	public:
		const static TIMET DAYS_PER_WEEK = 7;                                         //每周日期数
		const static TIMET MAX_DAYS_PER_MONTH = 31;                                   //每月日期数
		const static TIMET SECONDS_PER_MIN = 60;                                      //每分秒数
		const static TIMET SECONDS_PER_HOUR = SECONDS_PER_MIN * 60;                   //每小时秒数
		const static TIMET SECONDS_PER_DAY = SECONDS_PER_HOUR * 24;                   //每天秒数
		const static TIMET SECONDS_PER_WEEK = SECONDS_PER_DAY * DAYS_PER_WEEK;        //每周秒数
		const static TIMET MILLISECONDS_PER_SECOND = 1000;                            //每秒毫秒数
		const static TIMET MICROSECONDS_PER_MILLSECOND = 1000;  					  //每毫秒微秒数
		const static TIMET NANOSECONDS_PER_MILLSECOND = 1000;  						  //每毫秒纳秒数
		const static TIMET MICROSECONDS_PER_SECOND = MICROSECONDS_PER_MILLSECOND * MILLISECONDS_PER_SECOND;
		const static TIMET NANOSECONDS_PER_SECOND = NANOSECONDS_PER_MILLSECOND * MILLISECONDS_PER_SECOND;
		const static TIMET TICKS_PER_SECOND = 10 * 1000 * 1000;                       //每秒Tick数

	private:
		SystemTime();
		~SystemTime();
	public:
		// UTC 标准时间 1970.1.1 0.0.0 到此时的毫秒数
		static TIMET nowMillSec();
		// UTC 标准时间 1970.1.1 0.0.0 到此时的微秒数
		static TIMET nowMicroSec();
		// UTC 标准时间 1970.1.1 0.0.0 到此时的秒数
		static TIMET nowSec();
		// 转换成本地时间
		static void local(TIMET sec, OUT struct tm* pTM);
		// 格林威治
		static void gmt(TIMET sec, OUT struct tm* pTM);
		// diff 时间 毫秒
		static TIMET diff(const struct timeval &tv1, const struct timeval &tv2);
		// diff timespec
		static timespec diff(timespec& ts1, timespec& ts2);
		// add timespec
		static timespec add(timespec& ts1, timespec& ts2);
		// addwithnow timespec
		static timespec add(EM_CLOCK_TYPE eType, TIMET millSeconds);
		// get timespec
		static timespec getCurTimespec(EM_CLOCK_TYPE eType);
		// 根据秒数算出当前格式化的时间 eg: 111 return: 20161014123001
		static TIMET getSecFormatBySec(TIMET sec);
	public:
		// 获取当前毫秒数 加上了 delta
		inline TIMET getCurMillSec() { return nowMillSec() + m_DeltaMillTime; }
		inline TIMET getCurSec()  { return nowSec() + m_DeltaMillTime / MILLISECONDS_PER_SECOND; }
		inline TIMET getCurMicroSec() { return nowMicroSec() + m_DeltaMillTime * MICROSECONDS_PER_MILLSECOND; }
		// 获取已经运行了多少毫秒
		inline TIMET getCurRunTime() { return nowMillSec() + m_DeltaMillTime - m_BeginMillTime; }
		inline void setDeltaMillSec(TIMET delta) { m_DeltaMillTime = delta; }

	public:
		// 根据时分秒算出时间
		TIMET getSecByFormat(int year, int mon, int day, int hour = 0, int min = 0, int sec = 0);
		// 获取当天的时间
		TIMET getSecByFormat(int hour = 0, int min = 0, int sec = 0);
		// 转换成本地当前时间
		void local(struct tm* pTM);
		// gmt
		void gmt(struct tm* pTM);
	private:
		// 与标准时间相差的毫秒数
		TIMET m_DeltaMillTime;
		// 单例初始化的毫秒数
		TIMET m_BeginMillTime;
		// 时区
		struct timezone m_sTimeZone;
	};
}


#endif
