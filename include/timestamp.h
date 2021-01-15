//
// Created by clock on 15/01/2021.
//

#ifndef BASIC_SERVICES_TIMESTAMP_H
#define BASIC_SERVICES_TIMESTAMP_H

#include <chrono>

#include "types.h"

namespace basic {

class Timestamp {

public:

	Timestamp() : m_microSeconds{} {}
	explicit Timestamp(std::chrono::microseconds value) : m_microSeconds(value) {}

	void Swap(Timestamp& rhs)
	{
		std::swap(m_microSeconds, rhs.m_microSeconds);
	}

	static Timestamp Now();

private:
	std::chrono::microseconds m_microSeconds; //!< Micro seconds since Epoch
};

} // namespace basic

#endif //BASIC_SERVICES_TIMESTAMP_H
