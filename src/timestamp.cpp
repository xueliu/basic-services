//
// Created by clock on 15/01/2021.
//

#include <cinttypes>
#include <cstdio>
#include <ctime>

#include "timestamp.h"

namespace basic {

Timestamp Timestamp::Now()
{
	auto time_point = std::chrono::system_clock::now();
	return Timestamp(time_point);
}

Timestamp::Timestamp(std::chrono::system_clock::time_point value)
{
	m_timePoint = value;
}

std::string Timestamp::toString() const {
	char buf[32] = {0};

	int64_t seconds = milliseconds() / kMilliSecondsPerSecond;
	int64_t microseconds = milliseconds() % kMilliSecondsPerSecond;

	snprintf(buf, sizeof(buf)-1, "%" PRId64 ".%06" PRId64 "", seconds, microseconds);
	return buf;
}

std::string Timestamp::toFormattedString(bool show_milliseconds) const noexcept {
	char buf[64] = {0};
	auto time = std::chrono::system_clock::to_time_t(m_timePoint);
	std::tm tm_time = * std::localtime(& time);

	if (show_milliseconds)
	{
		int microseconds = static_cast<int>(milliseconds() % kMilliSecondsPerSecond);
		snprintf(buf, sizeof(buf), "%4d%02d%02d %02d:%02d:%02d.%03d",
		         tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
		         tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec,
		         microseconds);
	}
	else
	{
		snprintf(buf, sizeof(buf), "%4d%02d%02d %02d:%02d:%02d",
		         tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
		         tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec);
	}
	return buf;
}

} // namespace basic