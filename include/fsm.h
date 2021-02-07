//
// Created by clock on 07/02/2021.
//

#ifndef BASIC_SERVICES_FSM_H
#define BASIC_SERVICES_FSM_H

#include "types.h"
#include "event.h"
#include "basic-services_export.h"

namespace basic {

//!< State returned from  a state-handler function
typedef std::uint8_t State;

//!< State-handler function
typedef State (*StateHandler)(void * const me, EventConstPtr const e);

//! Designates a target for an initial or regular transition.
#define TRAN_(target_) (me->tran(reinterpret_cast<StateHandler>(target_)))

//!< helper macro to trigger internal event in an FSM
#define STATE_TRIG_(state_, sig_) \
    ((*(state_))(this, &basic::reservedEvents[sig_]))

//!< helper macro to trigger exit action in an FSM
#define STATE_EXIT_(state_) do { \
    STATE_TRIG_(state_, Fsm::EXIT_SIG); \
} while (false)

//!< helper macro to trigger entry action in an FSM
#define STATE_ENTER_(state_) do { \
    STATE_TRIG_(state_, Fsm::ENTRY_SIG); \
} while (false)

//! Class finite state machine
class BASIC_SERVICES_EXPORT Fsm {
public:
	StateHandler m_state;   //!< Current active state (state-variable)
	StateHandler m_temp;    //!< Temporary state: target of tran. or superstate

	enum : State {
		RET_HANDLED		= 0U,
		RET_IGNORED		= 1U,
		RET_TRAN		= 2U,
		RET_TOP 		= 3U,
		RET_UNHANDLED	= 4U,
	};
public:
	//! Virtual destructor */
	virtual ~Fsm() = default;

	//! Executes the top-most initial transition
	virtual void init(void const * const e);

	//! Overload init(nullptr)
	virtual void init() { this->init(nullptr); }

	/*! Dispatches an event to a FSM */
	virtual void dispatch(EventConstPtr const ev);

public:
	Fsm(StateHandler const initial)
			: m_state(nullptr)
			, m_temp(initial)
	{}

	StateHandler state( ) const {
		return m_state;
	}

	static State EVT_IGNORED()		{ return RET_IGNORED;	}
	static State EVT_HANDLED()		{ return RET_HANDLED;	}
	static State EVT_UNHANDLED()	{ return RET_UNHANDLED; }

	State tran(StateHandler const target) {
		m_temp = target;
		return RET_TRAN;
	}

protected:
	enum ReservedSignals : Signal {
		ENTRY_SIG = 1,	//!< signal for entry actions
		EXIT_SIG,		//!< signal for exit actions
		INIT_SIG		//!< signal for initial transitions
	};
};

} // namespace basic

#endif //BASIC_SERVICES_FSM_H
