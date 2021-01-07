//
// Created by liu on 06.01.2021.
//

#ifndef BASIC_SERVICES_QUEUE_H
#define BASIC_SERVICES_QUEUE_H

#include <memory>
#include <queue>
#include <mutex>
#include <condition_variable>

#include "noncopyable.h"
#include "basic-services_export.h"

namespace basic {

//! Template Class Queue
//!
//! \brief
//! A thread safe queue implementation
template<typename T>
class BASIC_SERVICES_EXPORT Queue : public noncopyable {
public:
	using value_type = T;
	using reference = T &;
	using point_type = T *;
	using const_reference = const T &;

public:
	//! Constructor
	//!
	//! \param cap - Capacity (Maximum size) of the queue (0 means unlimited)
	explicit Queue(std::size_t cap = 0)
			: m_capacity(cap), m_mutex{}, m_condPop{}, m_condPush{} {}

	//! Move Constructor
	Queue(Queue &&q) noexcept
			: m_capacity(q.m_capacity), m_mutex(std::move(q.m_mutex)), m_condPop(std::move(q.m_condPop)),
			  m_condPush(std::move(q.m_condPush)), m_queue(std::move(q.m_queue)) {

	}

	//! Destructor
	~Queue() = default;

	//! Query whether queue is empty
	//!
	//! \retval true - The queue is empty
	//! \retval false - The queue is no emtpy
	bool isEmpty() const {
		std::lock_guard<std::mutex> lk(m_mutex);
		return m_queue.empty();
	}

	//! Query whether queue is full
	//!
	//! \retval true - The queue is full
	//! \retval false - The queue is no full
	bool isFull() const {
		std::lock_guard<std::mutex> lk(m_mutex);
		return m_capacity > 0 && m_queue.size() >= m_capacity;
	}

	//! Query size of the queue
	std::size_t Size() const {
		std::lock_guard<std::mutex> lk(m_mutex);
		std::size_t size = m_queue.size();
		return size;
	}

	//! Clear the queue
	void Clear() {
		std::lock_guard<std::mutex> lk(m_mutex);
		m_queue.clear();
	}

	//! Set the capacity of the queue
	//!
	//! \param cap - Capacity to set
	void setCapacity(std::size_t cap) {
		std::lock_guard<std::mutex> lk(m_mutex);
		m_capacity = cap;
	}

	//! Query the capacity of the queue
	//! \return Capacity of the queue
	std::size_t getCapacity() const {
		std::lock_guard<std::mutex> lk(m_mutex);
		std::size_t cap = m_capacity;

		return cap;
	}

	//! Query the current amount of values in the queue
	//! \return Number of values
	std::size_t Size()
	{
		std::lock_guard<std::mutex> lk(m_mutex);
		std::size_t size = m_queue.size();
		return size;
	}

	//! Push value into the queue
	//!
	//!\brief
	//! Push one value in the queue
	//!
	//! \note
	//! If the capacity is reached the caller blocks until a value is popped from the queue.
	//! \param value - Value to be pushed
	void Push(const_reference value) {
		std::unique_lock<std::mutex> lk(m_mutex);
		while (m_capacity > 0 && m_queue.size() >= m_capacity) {
			m_condPop.wait(lk);
		}

		m_queue.push(value);
		m_condPush.notify_one();
	}

	//! Move value into the queue
	//!
	//!\brief
	//! Move one value in the queue
	//!
	//! \note
	//! If the capacity is reached the caller blocks until a value is popped from the queue.
	//! \param value - Value to be pushed
	void Push(value_type &&value) {
		std::unique_lock<std::mutex> lk(m_mutex);
		while (m_capacity > 0 && m_queue.size() >= m_capacity) {
			m_condPop.wait(lk);
		}

		m_queue.push(std::move(value));
		m_condPush.notify_one();
	}

	//! Pop value out of the queue
	//!
	//!\brief
	//! Pop one value out of the queue
	//!
	//! \note
	//! If the queue is empty the caller blocks until a value is pushed into the queue.
	//! \return Value be popped
	value_type Pop() {
		std::unique_lock<std::mutex> lk(m_mutex);
		while (m_queue.empty()) {
			m_condPush.wait(lk);
		}

		T value = std::move(m_queue.front());
		m_queue.pop();
		m_condPop.notify_one();

		return value;
	}

private:
	std::size_t m_capacity;
	std::queue<value_type> m_queue;
	mutable std::mutex m_mutex;
	std::condition_variable m_condPop;
	std::condition_variable m_condPush;
};

} // namespace basic

#endif //BASIC_SERVICES_QUEUE_H
