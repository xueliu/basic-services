//
// Created by clock on 15/01/2021.
//

#include <cinttypes>

#include "timestamp.h"

namespace basic {

Timestamp Timestamp::Now()
{
	auto time_point = std::chrono::system_clock::now();
	auto duration =  std::chrono::duration_cast<std::chrono::microseconds>(time_point.time_since_epoch());

	return Timestamp(duration);
}

std::string Timestamp::toString() const {
	char buf[32] = {0};

	int64_t seconds = m_microSeconds.count() / kMicroSecondsPerSecond;
	int64_t microseconds = m_microSeconds.count() % kMicroSecondsPerSecond;

	snprintf(buf, sizeof(buf)-1, "%" PRId64 ".%06" PRId64 "", seconds, microseconds);
	return buf;
}

} // namespace basic