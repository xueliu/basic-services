//
// Created by liu on 25.01.2021.
//

#include <iostream>

#include "timestamp.h"

auto main() -> int
{

	basic::Timestamp timestamp;

	std::cout << timestamp.isValid() << std::endl;

	timestamp = basic::Timestamp::Now();

	std::cout << timestamp.isValid() << std::endl;

	std::cout << timestamp.toFormattedString() << std::endl;

	return 0;
}