//
// Created by clock on 15/01/2021.
//



#include "timestamp.h"

namespace basic {

Timestamp Timestamp::Now()
{
	auto time_point = std::chrono::system_clock::now();
	auto duration =  std::chrono::duration_cast<std::chrono::microseconds>(time_point.time_since_epoch());

	return Timestamp(duration);
}

} // namespace basic