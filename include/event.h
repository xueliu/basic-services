//
// Created by clock on 15/01/2021.
//

#ifndef BASIC_SERVICES_EVENT_H
#define BASIC_SERVICES_EVENT_H

namespace basic {


//! Class Event
class Event {
public:
	//! Constructor
	Event() = default;

	//! Destructor
	virtual ~Event() = default;

protected:
	virtual void Process() = 0;
};

} // namespace basic

#endif //BASIC_SERVICES_EVENT_H
