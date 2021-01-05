//
// Created by liu on 05.01.2021.
//

#ifndef BASIC_SERVICES_THREAD_POOL_H
#define BASIC_SERVICES_THREAD_POOL_H

#include <mutex>
#include <vector>
#include <deque>
#include <condition_variable>
#include <string>

#include "noncopyable.h"
#include "thread.h"
#include "basic-services_export.h"

namespace basic {

//! Class Thread Pool
class BASIC_SERVICES_EXPORT ThreadPool : public noncopyable {
public:
	//! Thread function type
	using Task = basic::Thread::Function;

	//! Constructor
	explicit ThreadPool(std::string, uint16_t, uint16_t);

	//! Destructor
	~ThreadPool();

	//! Start thread pool
	void Start();

	//! Stop thread pool
	void Stop();

	//! Retrieve name of the thread pool
	const std::string& Name() const;

	//! Post task into the thread pool
	void Post(Task task);
private:

	//! Thread function
	void runInThread();

	//! Retrieve task from task queue
	Task take();

	mutable std::mutex m_mutex;
	std::condition_variable m_notEmpty;
	std::condition_variable m_notFull;

	std::string m_name;

	std::vector<std::unique_ptr<basic::Thread> > m_threads; //<! Thread list
	std::deque<Task> m_tasks;                               //<! Task list

	bool m_isRunning;

	uint16_t m_capacity;
	uint16_t m_threadNum;
};

} // namespace basic

#endif //BASIC_SERVICES_THREAD_POOL_H
