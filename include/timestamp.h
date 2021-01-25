//
// Created by clock on 15/01/2021.
//

#ifndef BASIC_SERVICES_TIMESTAMP_H
#define BASIC_SERVICES_TIMESTAMP_H

#include <chrono>
#include <string>

#include "types.h"
#include "basic-services_export.h"

namespace basic {

class BASIC_SERVICES_EXPORT Timestamp {

public:

	Timestamp() : m_timePoint() {}

	explicit Timestamp(std::chrono::system_clock::time_point value);

	void Swap(Timestamp& rhs)
	{
		std::swap(m_timePoint, rhs.m_timePoint);
	}

	bool isValid() const {
		return m_timePoint > std::chrono::system_clock::time_point();
	}

	std::string toString() const;

	std::string toFormattedString(bool show_milliseconds = true) const noexcept;

	static Timestamp Now();

	static const int kMilliSecondsPerSecond = 1000;

private:
	constexpr std::chrono::milliseconds::rep milliseconds() const noexcept {
		return std::chrono::duration_cast<std::chrono::milliseconds>(m_timePoint.time_since_epoch()).count();
	}

	std::chrono::system_clock::time_point m_timePoint;
};

} // namespace basic

#endif //BASIC_SERVICES_TIMESTAMP_H
