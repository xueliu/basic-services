//
// Created by liu on 27.01.2021.
//

#ifndef BASIC_SERVICES_CURRENT_THREAD_H
#define BASIC_SERVICES_CURRENT_THREAD_H

#include <thread>

namespace basic {

namespace CurrentThread {

extern thread_local int t_cachedTid;
extern thread_local char t_tidString[32];
extern thread_local int t_tidStringLength;
extern thread_local const char* t_threadName;
void cacheTid();

inline int tid()
{
	if (t_cachedTid == 0)
	{
		// cacheTid();
		t_cachedTid = 10;
	}
	return t_cachedTid;
}

inline const char* tidString() // for logging
{
	return t_tidString;
}

inline int tidStringLength() // for logging
{
	return t_tidStringLength;
}

inline const char* name()
{
	return t_threadName;
}

bool isMainThread();

} // namespace CurrentThread

} // namespace basic

#endif //BASIC_SERVICES_CURRENT_THREAD_H
