//
// Created by liu on 05.02.2021.
//

#include <cassert>

#include "circle-buffer.h"
#include "event.h"

using namespace basic;

auto main() -> int
{

	Event event_1(16);

	CircleBuffer<Event, std::uint32_t> event_queue(4);

	event_queue.post(&event_1);

	const Event * event_ret = event_queue.get( );

	assert(event_ret->signal == event_1.signal);

	return 0;
}