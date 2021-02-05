//
// Created by liu on 05.02.2021.
//

#ifndef BASIC_SERVICES_CIRCLE_BUFFER_H
#define BASIC_SERVICES_CIRCLE_BUFFER_H

#include <memory>

#include "noncopyable.h"
#include "basic-services_export.h"

namespace basic {

//! A simple template circle buffer class
template< typename T, typename SIZE_TYPE>
class BASIC_SERVICES_EXPORT CircleBuffer : public noncopyable {
public:
	using value_type        = T;
	using size_type         = SIZE_TYPE;
	using pointer           = T*;
	using const_pointer     = const T *;
	using reference         = T&;
	using const_reference   = const T&;
public:
	//! Default constructor
	explicit CircleBuffer(size_type size)
		: m_front(), m_end(size), m_head(0U), m_tail(0U), m_free(size + 1U)
		, m_ring(std::make_unique<const_pointer[]>(size))
	{}

	//! Get one value from the circle buffer
	const_pointer get() noexcept {
		// always remove the value from the front location
		const_pointer value = m_front;

		// when the queue is not empty
		if (nullptr != value) {
			// update the number of fee value
			++m_free;
			// value in the array ?
			if (m_free <= m_end) {
				// remove from the tail
				m_front = m_ring[m_tail];
				if (0U == m_tail) {
					// wrap around
					m_tail = m_end;
				}
				--m_tail;
			} else {
				// queue becomes empty
				m_front = nullptr;
			}
		}

		return value;
	}

	//! Post one value into the circle buffer
	bool post(const_pointer const value) noexcept {
		bool status;
		if (m_free) {
			--m_free;
			// if queue is empty
			if (nullptr == m_front) {
				m_front = value;
			} else {
				m_ring[m_head] = value;

				if(m_head == 0U) {
					// wrap around
					m_head = m_end;
				}

				--m_head;
			}
			status = true; // value posted sucessfully
		} else {
			status = false;
		}

		return status;
	}

	//!
	bool empty() const noexcept
	{
		return nullptr == pointer(0);
	}
private:
	const_pointer m_front;	//!< Value at the front of the queue
	size_type m_end;		//!< Value at the end of the queue
	size_type m_head;       //!< Head index where value will be inserted
	size_type m_tail;       //!< End index where value will be extracted
	size_type m_free;       //!< Number of free places

	std::unique_ptr<const_pointer[]> m_ring;    //!< Instance of ring buffer

};

} // namespace basic

#endif //BASIC_SERVICES_CIRCLE_BUFFER_H
