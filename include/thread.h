//
// Created by liu on 05.01.2021.
//

#ifndef BASIC_SERVICES_THREAD_H
#define BASIC_SERVICES_THREAD_H

#include <thread>
#include <string>
#include <functional>

#include "noncopyable.h"

namespace basic {

//! Class Thread
class BASIC_SERVICES_EXPORT Thread : noncopyable {
public:
	//! Thread function type
	using Function = std::function<void()>;

	//! Constructor
	explicit Thread(Function);

	//! Move constructor
	Thread(Thread && rhs) noexcept;

	//! Move assignment
	Thread& operator = (Thread && rhs) noexcept;

	//! Destructor
	~Thread();

	//! Start thread function
	void Start();

	//! Join thread
	bool Join();

	//! Query thread is started
	bool isStarted() const;

	//! Query thread id
	std::thread::id id() const;

private:
	bool m_isStarted;		//!< thread started
	bool m_isJoined;    	//!< thread joined
	Function m_func;    	//!< thread function
	std::thread m_thread;   //!< thread instance
};

} // namespace basic

#endif //BASIC_SERVICES_THREAD_H
