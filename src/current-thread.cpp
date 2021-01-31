//
// Created by liu on 27.01.2021.
//

#include "current-thread.h"

namespace basic {

namespace CurrentThread {

thread_local int t_cachedTid = 0;
thread_local char t_tidString[32];
thread_local int t_tidStringLength = 6;
thread_local const char* t_threadName = "unknown";

} // namespace CurrentThread

} // namespace basic