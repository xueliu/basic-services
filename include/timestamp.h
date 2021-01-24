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

	Timestamp() : m_microSeconds{} {}
	explicit Timestamp(std::chrono::microseconds value) : m_microSeconds(value) {}

	void Swap(Timestamp& rhs)
	{
		std::swap(m_microSeconds, rhs.m_microSeconds);
	}

	std::string toString() const;

	std::string toFormattedString(bool show_microseconds = true) const noexcept;

	static Timestamp Now();

	static const int kMicroSecondsPerSecond = 1000 * 1000;

private:
	std::chrono::microseconds m_microSeconds; //!< Micro seconds since Epoch
};

} // namespace basic

#endif //BASIC_SERVICES_TIMESTAMP_H
