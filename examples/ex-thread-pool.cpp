//
// Created by liu on 05.01.2021.
//
#include "chrono"
#include <iostream>

#include "thread-pool.h"
#include "thread.h"

int main() {
	uint8_t  loop = 0;

//	basic::Thread thread([](){ std::cout << "Hello 1" << std::endl; });
//	thread.Start();
//	thread.Join();
//
//	thread = basic::Thread([](){ std::cout << "Hello 2" << std::endl; } );
//	thread.Start();
//	thread.Join();

	basic::ThreadPool thread_pool {"MyThreadPool", 4};
	thread_pool.Start(4);

	thread_pool.Run( [](){ std::cout << "Hello 3" << std::endl; } );
	thread_pool.Run( [](){ std::cout << "Hello 4" << std::endl; } );
	thread_pool.Run( [](){ std::cout << "Hello 1" << std::endl; } );
	thread_pool.Run( [](){ std::cout << "Hello 2" << std::endl; } );

	while(loop < 4)
	{
		using namespace std::chrono_literals;
		std::this_thread::sleep_for(1s);
		++loop;
	}

	thread_pool.Stop();
}