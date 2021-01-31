//
// Created by liu on 27.01.2021.
//

#include <thread>

#include "timestamp.h"
#include "logging.h"
#include "log-stream.h"
#include "current-thread.h"

namespace basic {

class Logger::Impl
{
public:
//	typedef Logger::Level LogLevel;
	Impl(Logger::Level level, int savedErrno, const SourceFile& file, int line);
	void formatTime();
	void finish();

	Timestamp m_time;
	LogStream m_stream;
	Logger::Level m_level;
	int m_line;
	SourceFile m_basename;
};

thread_local char t_errnobuf[512];
thread_local char t_time[64];
thread_local time_t t_lastSecond;

const char* strerror_tl(int savedErrno)
{
	//return strerror_r(savedErrno, t_errnobuf, sizeof t_errnobuf);
	return ""; // TODO:
}


Logger::Level initLogLevel() noexcept
{
	if (::getenv("BASIC_LOG_TRACE"))
		return Logger::Level::TRACE;
	else if (::getenv("BASIC_LOG_DEBUG"))
		return Logger::Level::DEBUG;
	else
		return Logger::Level::INFO;
}

Logger::Level g_logLevel = initLogLevel();

const char* LevelName[Logger::Level::NUM_LOG_LEVELS] =
{
	"FATAL ",
	"ERROR ",
	"WARN  ",
	"INFO  ",
	"DEBUG ",
	"TRACE ",
};

#ifdef _WIN32

// Currently no color for Windows
const char* LevelColor[Logger::Level::NUM_LOG_LEVELS] =
{
	"",
	"",
	"",
	"",
	"",
	""
};

const char* LevelColorReset = "";

const unsigned int LevelColorLength = 0;

#else

const char* LevelColor[Logger::Level::NUM_LOG_LEVELS] =
{
	"\x1B[31m ", /* red */
	"\x1B[31m ", /* red */
	"\x1B[33m ", /* orange */
	"\x1B[36m ", /* cyan */
	"\x1B[32m ", /* green */
	"\x1B[37m "  /* white */
};

const unsigned int LevelColorLength = 6;

const char* LevelColorReset = " \x1B[0m ";

#endif

// helper class for known string length at compile time
class T
{
public:
	T(const char* str, unsigned len)
			:str_(str),
			 len_(len)
	{
		assert(strlen(str) == len_);
	}

	const char* str_;
	const unsigned len_;
};

inline LogStream& operator<<(LogStream& s, T v)
{
	s.append(v.str_, v.len_);
	return s;
}

inline LogStream& operator<<(LogStream& s, const SourceFile& v)
{
	s.append(v.data_, v.size_);
	return s;
}

void defaultOutput(const char* msg, int len)
{
	size_t n = fwrite(msg, 1, len, stdout);
	//FIXME check n
	(void)n;
}

void defaultFlush()
{
	fflush(stdout);
}

Logger::OutputFunc g_output = defaultOutput;
Logger::FlushFunc g_flush = defaultFlush;

Logger::Impl::Impl(Logger::Level level, int savedErrno, const SourceFile& file, int line)
		: m_time(Timestamp::Now()),
		  m_stream(),
		  m_level(level),
		  m_line(line),
		  m_basename(file)
{
	m_stream << T(LevelColor[level], LevelColorLength);
	formatTime();
	CurrentThread::tid();
//	m_stream << T(CurrentThread::tidString(), CurrentThread::tidStringLength());
	m_stream << T(LevelName[level], 6);
	if (savedErrno != 0)
	{
		m_stream << strerror_tl(savedErrno) << " (errno=" << savedErrno << ") ";
	}
}

void Logger::Impl::formatTime()
{
	auto seconds = static_cast<time_t>(m_time.milliseconds() / Timestamp::kMilliSecondsPerSecond);
	int64_t microseconds = m_time.milliseconds() % Timestamp::kMilliSecondsPerSecond;

	if (seconds != t_lastSecond)
	{
		t_lastSecond = seconds;
		std::tm tm_time = * std::localtime(& seconds);

		int len = snprintf(t_time, sizeof(t_time), "%4d%02d%02d %02d:%02d:%02d",
		                   tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
		                   tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec);
		assert(len == 17); (void)len;
	}

	Fmt us(".%03d ", microseconds);
	assert(us.length() == 5);
	m_stream << T(t_time, 17) << T(us.data(), 5);
}

void Logger::Impl::finish()
{
	m_stream << " - " << m_basename << ':' << m_line << LevelColorReset << '\n';
}

Logger::Logger(SourceFile file, int line)
	: m_impl(std::make_unique<Impl>(Logger::Level::INFO, 0, file, line) )
{
}

Logger::Logger(SourceFile file, int line, Logger::Level level, const char* func)
	: m_impl(std::make_unique<Impl>(level, 0, file, line) )
{
	m_impl->m_stream << func << ' ';
}

Logger::Logger(SourceFile file, int line, Logger::Level level)
	: m_impl(std::make_unique<Impl>(level, 0, file, line) )
{
}

Logger::Logger(SourceFile file, int line, bool toAbort)
	: m_impl(std::make_unique<Impl>(toAbort?FATAL:ERROR, errno, file, line) )
{
}

Logger::~Logger()
{
	m_impl->finish();
	const LogStream::Buffer& buf(stream().buffer());
	g_output(buf.data(), buf.length());
	if (m_impl->m_level == FATAL)
	{
		g_flush();
		abort();
	}
}

LogStream& Logger::stream()
{
	return m_impl->m_stream;
}

void Logger::setLevel(Logger::Level level)
{
	g_logLevel = level;
}

Logger::Level Logger::getLevel()
{
	return g_logLevel;
}

void Logger::setOutput(OutputFunc out)
{
	g_output = out;
}

void Logger::setFlush(FlushFunc flush)
{
	g_flush = flush;
}


} // namespace basic