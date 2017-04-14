//
// Created by 58 on 2016/4/27.
//

#include <string.h>
#include <wedis/base/util/Log.h>
#include <cstdio>
#include <stdarg.h>

namespace wRedis {
IMPLEMENT_SINGLETON_T(Log)

Log::Log() {
	initRaw();
}

Log::~Log() {
	cleanUp();
}

void Log::initRaw() {
	m_aLogs[LogType::Debug] = log_debug;
	m_aLogs[LogType::Info] = log_info;
	m_aLogs[LogType::Warn] = log_warn;
	m_aLogs[LogType::Error] = log_error;
}

void Log::cleanUp() {
	for (I32 i = 0; i < LogType::Num; ++i) {
		m_aLogs[i] = NULLPTR;
	}
}

void Log::setLogFunc(LogType::EM_LOG_TYPE log_type, log_func_t func) {
	if (NULLPTR != func && LogType::Num != log_type) {
		m_aLogs[log_type] = func;
	}
}

void Log::debug(const I8* format, ...) {
	Assert(m_aLogs[LogType::Debug]);

	I8 out_buf[2048] = { 0 };

	va_list args;
	va_start(args, format);
	vsnprintf(out_buf, sizeof(out_buf) - 1, format, args);
	m_aLogs[LogType::Debug](out_buf);
	va_end(args);
}

void Log::info(const I8* format, ...) {
	Assert(m_aLogs[LogType::Info]);

	I8 out_buf[2048] = { 0 };

	va_list args;
	va_start(args, format);
	vsnprintf(out_buf, sizeof(out_buf) - 1, format, args);
	m_aLogs[LogType::Info](out_buf);
	va_end(args);
}

void Log::warn(const I8* format, ...) {
	Assert(m_aLogs[LogType::Warn]);

	I8 out_buf[2048] = { 0 };

	va_list args;
	va_start(args, format);
	vsnprintf(out_buf, sizeof(out_buf) - 1, format, args);
	m_aLogs[LogType::Warn](out_buf);
	va_end(args);
}

void Log::error(const I8* format, ...) {
	Assert(m_aLogs[LogType::Error]);

	I8 out_buf[2048] = { 0 };

	va_list args;
	va_start(args, format);
	vsnprintf(out_buf, sizeof(out_buf) - 1, format, args);
	m_aLogs[LogType::Error](out_buf);
	va_end(args);
}
}
