//
// Created by liu on 04.02.2021.
//

#include "count-down-latch.h"

using namespace basic;

CountDownLatch::CountDownLatch(int count)
		: m_count(count), m_mtx(), m_cond() {

}

int CountDownLatch::getCount() const {
	std::lock_guard<std::mutex> lk(m_mtx);

	return m_count;
}

void CountDownLatch::countDown() {
	std::lock_guard<std::mutex> lk(m_mtx);

	if (0 == m_count) {
		return;
	}

	--m_count;

	if (0 == m_count) {
		m_cond.notify_all();
	}
}

void CountDownLatch::wait(uint64_t miliseconds) {
	std::unique_lock<std::mutex> lk(m_mtx);

	if (0 == m_count) {
		return;
	}

	if (miliseconds > 0) {
		m_cond.wait_for(lk, std::chrono::milliseconds(miliseconds), [this] { return m_count <= 0; } );
	} else {
		m_cond.wait(lk, [this] { return m_count <= 0; });
	}

}