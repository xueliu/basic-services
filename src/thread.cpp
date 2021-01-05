//
// Created by liu on 05.01.2021.
//
#include <cassert>

#include "thread.h"

namespace basic {

Thread::Thread(Function func)
		: m_isStarted(false), m_isJoined(false), m_func(std::move(func)) {}

Thread::Thread(Thread &&rhs) noexcept
		: m_isJoined(rhs.m_isJoined), m_isStarted(rhs.m_isStarted), m_thread(std::move(rhs.m_thread)),
		  m_func(std::move(rhs.m_func)) {

}

Thread &Thread::operator=(Thread &&rhs) noexcept {
	if (this != &rhs) {
		m_isJoined = rhs.m_isJoined;
		m_isStarted = rhs.m_isStarted;
		m_thread = std::move(rhs.m_thread);
		m_func = std::move(rhs.m_func);
	}

	return *this;
}

Thread::~Thread() {
	if (m_isStarted && not m_isJoined) {
		m_thread.detach(); // no wait
	}
}

void Thread::Start() {
	assert(!m_isStarted);
	m_isStarted = true;

	m_thread = std::thread(m_func);
}

bool Thread::isStarted() const {
	return m_isStarted;
}

bool Thread::Join() {
	assert(m_isStarted);
	assert(!m_isJoined);
	if (m_thread.joinable()) {
		m_isJoined = true;
		m_thread.join();
	} else {
		m_isJoined = false;
	}

	return m_isJoined;
}

std::thread::id Thread::id() const {
	return m_thread.get_id();
}

} // namespace basic