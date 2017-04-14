//
// Created by 58 on 2016/4/27.
//

#ifndef WREDISCLIENT_LOG_H
#define WREDISCLIENT_LOG_H

#include <wedis/base/util/TSingleton.h>

namespace wRedis {
struct LogType {
	enum EM_LOG_TYPE {
		Debug = 0, Info = 1, Warn = 2, Error = 3, Num = 4
	};
};

class Log {
DECLARE_SINGLETON(Log);
public:
	Log();
	virtual ~Log();
public:
	// un thread safe
	void setLogFunc(LogType::EM_LOG_TYPE log_type, log_func_t func);
public:
	void debug(const I8* format_str, ...);
	void info(const I8* format_str, ...);
	void warn(const I8* format_str, ...);
	void error(const I8* format_str, ...);
private:
	void cleanUp();
	void initRaw();
private:
	log_func_t m_aLogs[LogType::Num];
};

#define Logger 		Log::getSingleton()
#define LoggerPtr 	Log::getSingletonPtr()
}

#endif //WREDISCLIENT_LOG_H
