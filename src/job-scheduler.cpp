//
// Created by liu on 07.01.2021.
//

#include <ios>
#include <cstring>
#include <iostream>
#include "job-scheduler.h"

namespace basic {

std::string JobScheduler::error(int error_number, const std::string& module_name)
{
	std::ostringstream oss;
	oss << "Job Scheduler module : " << module_name << " (" << std::dec << error_number << ") :" << ::strerror(error_number);
	return oss.str();
}


} // namespace basic

//JobScheduler::JobScheduler()
//	: m_isRunning(false), m_isSkipped(false), m_isTerminated(false)
//	, m_retValue(), m_jobs(0)
//{
//
//}
//
//}
