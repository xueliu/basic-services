//
// Created by clock on 26/01/2021.
//

#include "logging.h"

auto main() -> int
{
	LOG_TRACE << "This is a trace message";
	LOG_DEBUG << "This is a debug message";
	LOG_INFO << "This a a info message";
	LOG_WARN << "This a warning message";
	LOG_ERROR << "This is en error message!";
	LOG_SYSERR << "This is a system error message!";

	return 0;
}