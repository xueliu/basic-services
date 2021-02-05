//
// Created by liu on 05.02.2021.
//

#include <cassert>

#include "circle-buffer.h"

using namespace basic;

auto main() -> int
{
	using Signal = std::uint16_t;

	class Event {
	public:
		explicit Event(Signal sig) noexcept
				: signal(sig) {}
		Signal signal;

		bool operator == (const Event & rhs) const
		{
			return this->signal == rhs.signal;
		}

		bool operator != (const Event & rhs) const
		{
			return this->signal != rhs.signal;
		}
	};

	//! typedef for non-const pointer type Event
	typedef Event * EventPtr;

	//! typedef for const type Event
	typedef const Event EventConst;

	//! typedef for const pointer type Event
	typedef const Event * EventConstPtr;

	Event event_1(16);

	CircleBuffer<Event, std::uint32_t> event_queue(4);

	event_queue.post(&event_1);

	const Event * event_ret = event_queue.get( );

	assert(event_ret->signal == event_1.signal);

	return 0;
}