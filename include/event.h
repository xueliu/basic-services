//
// Created by clock on 15/01/2021.
//

#ifndef BASIC_SERVICES_EVENT_H
#define BASIC_SERVICES_EVENT_H

#include "basic-services_export.h"

namespace basic {


using Signal = std::uint16_t;

constexpr Signal NULL_SIG   {0};    //!< Signal for null
constexpr Signal ENTRY_SIG  {1};	//!< Signal for entry actions
constexpr Signal EXIT_SIG   {2};    //!< Signal for exit actions
constexpr Signal INIT_SIG   {3};    //!< Signal for initial actions
constexpr Signal USER_SIG   {4};    //!< Signal for staring user defined actions

class BASIC_SERVICES_EXPORT Event {
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

EventConst reservedEvents[4] = {
		Event(NULL_SIG),
		Event(ENTRY_SIG),
		Event(EXIT_SIG),
		Event(INIT_SIG)
};

} // namespace basic

#endif //BASIC_SERVICES_EVENT_H
