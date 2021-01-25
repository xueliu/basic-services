//
// Created by liu on 07.01.2021.
//

#ifndef BASIC_SERVICES_JOB_SCHEDULER_H
#define BASIC_SERVICES_JOB_SCHEDULER_H

#include <any>
#include <mutex>
#include <atomic>
#include <memory>

#include "noncopyable.h"
#include "queue.h"

namespace basic {

class Job {
public:
	//! Default constructor
	Job() = default;

	//! Virtual destructor
	virtual ~Job() = default;

	//! Execute the job
	virtual void exec() = 0;

private:
	//! Job entry point
	static void * entry(void * arg);
};

//! Class JobScheduler
class JobScheduler : public noncopyable {
public:
	//! Constructor
//	JobScheduler();

	//! Destructor
	~JobScheduler() = default;

	//! Factory
	static auto CreateJobScheduler(size_t capacity = 0) -> std::unique_ptr<JobScheduler>;

	//! Stop Job
	static int Stop(JobScheduler* scheduler, bool wait_pending_jobs = false);

	void Start();

	int pushJob(std::unique_ptr<Job>);

	bool isRunning() const;

	bool isTerminated() const;
private:
	explicit JobScheduler(size_t capacity);

	int init();

	int create();

	void waitUntilRunning();

	int join();

	std::any run();

	std::string error(int error_code, const std::string& module_name);

	static std::any startRoutine(std::any arg);
private:
	basic::Queue<std::unique_ptr<Job>> m_jobQueue;
	std::any m_retValue;

	mutable std::mutex m_workerMutex;
	std::condition_variable m_workerStarted;
	std::atomic_bool m_isRunning;
	std::atomic_bool m_isTerminated;
	std::atomic_bool m_isSkipped;
	std::atomic_int32_t m_jobs;
};

} // namespace basic

#endif //BASIC_SERVICES_JOB_SCHEDULER_H
