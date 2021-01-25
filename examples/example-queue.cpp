//
// Created by liu on 06.01.2021.
//

#include "queue.h"

auto mains() -> int
{
	basic::Queue<int> queue(4);

	queue.Push(1);
	queue.Push(2);

	return 0;
}