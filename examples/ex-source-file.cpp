//
// Created by clock on 26/01/2021.
//
#include <iostream>

#include "source-file.h"

auto main() -> int
{

	auto source_file = SourceFile(__FILE__);

	std::cout << "Current File Name:" << source_file.data_ << std::endl;
	std::cout << "Current File Name Length:" << source_file.size_ << std::endl;

	return 0;
}