//
// Created by liu on 05.01.2021.
//
#include <cassert>

#include "thread-pool.h"

namespace basic {

ThreadPool::ThreadPool(std::string name, uint16_t capacity,
                       uint16_t thread_num = std::thread::hardware_concurrency())
		: m_name(std::move(name)), m_capacity(capacity), m_threadNum(thread_num), m_isRunning(false) {

	m_threads.reserve(m_threadNum);
}

ThreadPool::~ThreadPool() {
	if (m_isRunning)
		Stop();
}

void ThreadPool::Start() {
	assert(m_threads.empty());
	m_isRunning = true;
//	m_threads.reserve(m_threadNum);
	for (int i = 0; i < m_threadNum; ++i) {
		m_threads.emplace_back(new basic::Thread([this] { runInThread(); }));
		m_threads[i]->Start();
	}
}

void ThreadPool::Stop() {
	{
		std::lock_guard<std::mutex> lk(m_mutex);
		m_isRunning = false;
		m_notEmpty.notify_all();
	}

	for (auto &thread : m_threads) {
		thread->Join();
	}
}

void ThreadPool::Post(Task task) {
	if (m_threads.empty()) {
		task();
	} else {
		std::unique_lock<std::mutex> lk(m_mutex);
		m_notFull.wait(lk, [this] { return not(m_capacity > 0 && m_tasks.size() >= m_capacity); });

		m_tasks.push_back(std::move(task));
		m_notEmpty.notify_one();
	}
}

ThreadPool::Task ThreadPool::take() {
	std::unique_lock<std::mutex> lk(m_mutex);
	m_notEmpty.wait(lk, [this] {return not (m_tasks.empty() && m_isRunning);} );

	Task task;
	if (!m_tasks.empty()) {
		task = m_tasks.front();
		m_tasks.pop_front();
		if (m_capacity > 0) {
			m_notFull.notify_one();
		}
	}

	return task;
}

void ThreadPool::runInThread() {
	while (m_isRunning) {
		Task task(take());
		if (task) {
			task();
		}
	}
}

const std::string &ThreadPool::Name() const {
	return m_name;
}

} // namespace basic