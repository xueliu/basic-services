//
// Created by liu on 04.02.2021.
//

#include "count-down-latch.h"

using namespace basic;

CountDownLatch::CountDownLatch(int count)
		: m_count(count), m_mtx(), m_cond() {

}

int CountDownLatch::getCount() const {
	return m_count.load();
}

void CountDownLatch::countDown() {
	if (0 == m_count) {
		return;
	}

	--m_count;

	if (0 == m_count) {
		m_cond.notify_all();
	}
}

void CountDownLatch::wait(uint64_t milliseconds) {

	if (0 == m_count) {
		return;
	}

	std::unique_lock<std::mutex> lk(m_mtx);
	if (milliseconds > 0) {
		m_cond.wait_for(lk, std::chrono::milliseconds(milliseconds), [this] { return m_count <= 0; } );
	} else {
		m_cond.wait(lk, [this] { return m_count <= 0; });
	}

}