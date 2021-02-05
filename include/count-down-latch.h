//
// Created by liu on 04.02.2021.
//

#ifndef BASIC_SERVICES_COUNT_DOWN_LATCH_H
#define BASIC_SERVICES_COUNT_DOWN_LATCH_H

#include <condition_variable>
#include <mutex>
#include <atomic>

#include "noncopyable.h"
#include "basic-services_export.h"

namespace basic {

// Class CountDownLatch
class CountDownLatch : public noncopyable {
private:
	mutable std::mutex m_mtx;
	std::condition_variable m_cond;
	std::atomic_int32_t m_count;

public:
	//! Constructor
	explicit CountDownLatch(int count);

	//! Count down by 1
	void countDown();

	//! Get current count
	int getCount() const;

	//! Wait for count to zero
	void wait(uint64_t milliseconds = 0);
};

} // namespace basic

#endif //BASIC_SERVICES_COUNT_DOWN_LATCH_H
