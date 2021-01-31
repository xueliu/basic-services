//
// Created by liu on 27.01.2021.
//

#ifndef BASIC_SERVICES_LOGGING_H
#define BASIC_SERVICES_LOGGING_H

#include <memory>

#include "source-file.h"
#include "log-stream.h"
#include "basic-services_export.h"

namespace basic {

class BASIC_SERVICES_EXPORT Logger {
public:
	//! Logging Levels
	enum Level {
		FATAL = 0,
		ERROR,
		WARN,
		INFO,
		DEBUG,
		TRACE,
		NUM_LOG_LEVELS
	};

	Logger(SourceFile file, int line);
	Logger(SourceFile file, int line, Logger::Level level);
	Logger(SourceFile file, int line, Logger::Level level, const char* func);
	Logger(SourceFile file, int line, bool toAbort);
	~Logger();

	LogStream& stream();

	static Logger::Level getLevel();
	static void setLevel(Logger::Level level);


	typedef void (*OutputFunc)(const char* msg, int len);
	typedef void (*FlushFunc)();
	static void setOutput(OutputFunc);
	static void setFlush(FlushFunc);
private:
	class Impl;
	std::unique_ptr<Impl> m_impl;
};

extern Logger::Level g_logLevel; // global log level

#define LOG_TRACE if (basic::Logger::getLevel() <= basic::Logger::TRACE) \
		basic::Logger(__FILE__, __LINE__, basic::Logger::TRACE, __func__).stream()
#define LOG_DEBUG if (basic::Logger::getLevel() <= basic::Logger::DEBUG) \
		basic::Logger(__FILE__, __LINE__, basic::Logger::DEBUG, __func__).stream()
#define LOG_INFO if (basic::Logger::getLevel() <= basic::Logger::INFO) \
		basic::Logger(__FILE__, __LINE__).stream()
#define LOG_WARN basic::Logger(__FILE__, __LINE__, basic::Logger::WARN).stream()
#define LOG_ERROR basic::Logger(__FILE__, __LINE__, basic::Logger::ERROR).stream()
#define LOG_FATAL basic::Logger(__FILE__, __LINE__, basic::Logger::FATAL).stream()
#define LOG_SYSERR basic::Logger(__FILE__, __LINE__, false).stream()
#define LOG_SYSFATAL basic::Logger(__FILE__, __LINE__, true).stream()

const char* strerror_tl(int savedErrno);

} // namespace basic

#endif //BASIC_SERVICES_LOGGING_H
