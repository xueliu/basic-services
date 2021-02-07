//
// Created by clock on 07/02/2021.
//

#include "fsm.h"

namespace basic {

void Fsm::init(void const * const e) {
	assert(m_temp != nullptr);  // constructor must be executed
	assert(m_state == nullptr); // initial tran. NOT taken yet

	// execute the top-most initial transition
	// and the transition must be taken
	assert((*m_temp)(this, static_cast<EventConstPtr>(e)) == RET_TRAN);

	// enter the target
	(void)STATE_TRIG_(m_temp, Fsm::ENTRY_SIG);
	// record the new active state
	m_state = m_temp;
}

void Fsm::dispatch(EventConstPtr const ev)
{
	// state configuration must be stable
	assert(m_state == m_temp);

	// call the event handler
	State ret = (*m_state)(this, ev);

	// transition taken?
	if (ret == RET_TRAN) {
		// exit the source
		STATE_EXIT_(m_state);

		// enter the target
		STATE_ENTER_(m_temp);

		// record the new active state
		m_state = m_temp;
	}
}

} // namespace basic