/* ******************************************************************************************* *
 *                                         header files                                        *
 * ******************************************************************************************* */

#include <list>
#include <algorithm>
#include <chrono>
#include <mutex>
#include <thread>
#include <condition_variable>

#include "timer.h"

using namespace std;
using namespace chrono;

namespace basic {

/* ******************************************************************************************* *
 *                                    Timer::Impl definition                                   *
 * ******************************************************************************************* */

//! Timer::Impl - Timer implementation
//! \brief This class implements the timer functionality.
//! \note
//! The associated timer manager implementation (TimerManager::Impl)
//! manages a list of Timer::Impl instances, thus avoiding an additional
//! indirection level while at the same time providing an efficient
//! relationship between timer and timer manager.
class BASIC_SERVICES_NO_EXPORT Timer::Impl
{
private:
	Timer * m_inst;								//!< associated timer interface
	TimerManager::Impl & m_manager;				//!< associated timer manager
	Timer::Callback m_callbackHandler;	//!< time-out callback handler
	steady_clock::time_point m_expiration;		//!< time point of expiration

	enum class State : uint8_t					//!< timer state
	{
		kIdle,									//!< timer is inactive
		kActive,								//!< timer is active
		kExpired								//!< timer is expired
	} m_state = State::kIdle;

	enum class Access : uint8_t                 //!< timer access control
	{
		kDirect,								//!< direct access to timer
		kLocked,								//!< timer is locked, i.e. must not be destroyed
		kDestruct								//!< timer was locked and must be destroyed
	} m_access = Access::kDirect;

	//! getDuration - Retrieval of remaining running time duration (internal)
	//! \param now - Optional reference time point
	//! \return Remaining running time in milliseconds
	Duration getDuration(steady_clock::time_point const &now = steady_clock::now()) const noexcept {
		milliseconds::rep const remaining_ms = (m_state != State::kActive) ? 0U :
		                                       duration_cast<milliseconds>(m_expiration - now).count();
		return remaining_ms < 0U ? 0U : Duration(remaining_ms);
	}

	//! setDuration - Re-starting a timer (internal)
	//! \param duration - Running time to be set up. 0 stops the timer.
	//! \return - true Running time updated
	//! \return - false Running time could not be updated
	bool setDuration(Timer::Duration duration) noexcept {
		if (m_access != Access::kDestruct) {
			if (0 == duration) {
				m_state = State::kIdle;
				return true;
			}

			if (m_callbackHandler) {
				m_expiration = steady_clock::now() + milliseconds(duration);
				m_state = State::kActive;
				return true;
			}
		}
		return false;
	}

private:
	friend TimerManager::Impl;

public:

	//! Constructor
	//! \param inst - Timer instance (API interface)
	//! \param manager - Associated timer manager
	//! \param callback - Timeout callback function
	Impl(Timer &inst, TimerManager::Impl &manager, Timer::Callback &&callback)
			: m_inst(&inst)
			, m_manager(manager)
			, m_callbackHandler(std::move(callback))
			{ }

	//! Destructor
	~Impl() = default;

	//! create - Create timer instance
	//! \param timer - Timer instance (API interface)
	//! \param manager - Associated timer manager
	//! \param callback - Timeout callback function
	//! \return Impl* - Pointer of created timer instance
	//! \return nullptr - Unable to create timer instance
	static Impl *create(Timer & timer, TimerManager::Impl * manager, Callback && callback);

	//! destroy - Destroy timer instance
	//! \brief
	//! Destroys an instance via direct disassociation from a timer manager
	//! \param timer_impl - Pointer to instance do destroy
	static void destroy(Impl * timer_impl);

	//! Assignment of a new timeout callback
	//! \param callback - Timeout callback function to assign
	//! \return true - Timeout handler assigned
	//! \return false - Failed to assign timeout handler
	bool operator=(Timer::Callback &&callback)
	{
		if ((m_access != Access::kDestruct) && (m_state != State::kActive))
		{
			m_callbackHandler = std::move(callback);
			return true;
		}
		return false;
	}

	//! Retrieval of remaining running time duration
	//! \return Remaining running time in milliseconds
	explicit operator Timer::Duration() const noexcept
	{
		return getDuration();
	}

	//! Re-start a timer
	//!
	//! \param duration - Running duration time. 0 stops the timer.
	//! \return true - Duration time updated
	//! \return false - Duration time could not be updated
	//!
	bool operator=(Timer::Duration duration);

	//! Sorting operator
	//!
	//! \param rhs - the other timer
	//!
	//! \note
	//! The timer list is sorted into 3 sections:
	//! 1. active timers, sorted in ascending order of their expiration time point
	//! 2. inactive timers that do not need servicing
	//! 3. inactive timers that need servicing
	bool operator < (Impl const & rhs) const noexcept
	{
		return ((m_state == State::kActive) && (rhs.m_state != State::kActive))
		       || ((m_state == State::kActive) && (rhs.m_state == State::kActive) && (m_expiration < rhs.m_expiration))
		       || ((m_state == State::kIdle) && (rhs.m_state != State::kActive));
	}

	//! Matching operator
	//!
	//! \brief
	//! identification of instances
	//!
	//! \param rhs - Instance to identify
	//! \return true - two instances are the same.
	//! \return false - two instance are not the same.
	bool operator==(Impl const * rhs) const noexcept {
		return this == rhs;
	}
};

/* ******************************************************************************************* *
 *                                TimerManager::Impl definition                                *
 * ******************************************************************************************* */

class BASIC_SERVICES_NO_EXPORT TimerManager::Impl {
private:
	std::list<Timer::Impl> m_timerList; //!< list of associated timer instances
	std::mutex m_lock;					//!< access lock
	std::condition_variable m_sync;		//!< thread synchronisation
	bool m_termination = false;			//!< flag: thread to be terminated
	std::thread m_worker;				//!< instance of worker thread

	//! function of work thread
	void workThread();

	Timer::Duration updateTimers();
public:
	//! constructor
	Impl();

	//! destructor
	~Impl();

	//! create timer instance
	Timer::Impl *createTimer(Timer &, Timer::Callback &&);

	//! destroy timer instance
	void destroyTimer(Timer::Impl *);

	//! update timer duration
	bool updateTimer(Timer::Impl *, Timer::Duration);
};

/* ******************************************************************************************* *
 *                              TimerManager::Impl implementation                              *
 * ******************************************************************************************* */

//! Constructor of timer manager implementation
//! \brief Creates an empty timer manager.
TimerManager::Impl::Impl()
	: m_worker(&Impl::workThread, this)
	{ }

//! Destructor of timer manager implementation
//! \brief Destroys a timer manager thread and all active timers.
TimerManager::Impl::~Impl() {
	// TODO: using lock_guard
	//std::lock_guard<std::mutex> lck(m_lock);

	m_lock.lock();
	m_termination = true;
	m_sync.notify_one();
	m_lock.unlock();

	// wait for the termination of the thread
	m_worker.join();

	// make sure to wait for any access methods to return
	m_lock.lock();
	m_lock.unlock();
}

//! createTimer - Create timer instance
//!
//! \brief
//! Creates a timer instance via direct association with the timer manager
//!
//! \param timer - Timer instance (API interface)
//! \param callback - Timeout callback function
//! \return Timer::Impl * - Pointer of created instance
//! \retval nullptr - Unable to create timer instance
Timer::Impl *TimerManager::Impl::createTimer(Timer & timer, Timer::Callback &&callback) {
	std::lock_guard<mutex> guard(m_lock);

	Timer::Impl *timer_ = nullptr;
	if (!m_termination) {
		// create a temp timer list with only one timer instance
		std::list<Timer::Impl> list_;
		list_.emplace_back(timer, *this, std::move(callback));
		timer_ = &list_.back();

		// merge the new timer list with the private one (maintaining a sorted list)
		m_timerList.merge(list_);

		// perform an update of the timer list, if necessary (if timer should be active)
		m_sync.notify_one();
	}
	return timer_;
}

//! destroyTimer - Destroy timer instance
//! \brief Destroys a timer instance via direct disassociation from the timer manager
//! \param timer - Pointer to instance do destroy
void TimerManager::Impl::destroyTimer(Timer::Impl *timer) {
	std::lock_guard<mutex> guard(m_lock);

	auto it = std::find(m_timerList.begin(), m_timerList.end(), timer);
	if (it != m_timerList.end()) {
		it->m_inst = nullptr;

		if (it->m_access == Timer::Impl::Access::kDirect) {
			m_timerList.erase(it);

			// wake up the manager thread if waiting for destruction
			if (m_termination && m_timerList.empty())
				m_sync.notify_one();
		}
		// the timer is currently being handled
		else {
			it->m_access = Timer::Impl::Access::kDestruct;
		}
	}
}

//! updateTimer - Updating a timer with a new running time
//! \param timer - Timer to update
//! \param duration - New duration time to be set up
//! \return true - Duration time updated
//! \return false - Duration time could not be updated
bool TimerManager::Impl::updateTimer(Timer::Impl *timer, Timer::Duration duration) {
	std::lock_guard<mutex> guard(m_lock);

	if (!m_termination || (0 == duration)) {
		// determine timer position in the list
		auto it = std::find(m_timerList.begin(), m_timerList.end(), timer);
		// the time is found
		if (it != m_timerList.end()) {
			// update the timer
			if (it->setDuration(duration)) {
				// re-insert the timer into the list to maintain a correctly sorted list
				std::list<Timer::Impl> entry;
				entry.splice(entry.end(), m_timerList, it);
				m_timerList.merge(entry);

				if (0 != duration)
					m_sync.notify_one();

				return true;
			}
		}
	}
	return false;
}

//! Timer manager worker thread
//! \brief
//! This is the body of the timer manager worker processor.
//! It sleeps until notified or until the timer list needs to be processed.
void TimerManager::Impl::workThread() {
	std::unique_lock<mutex> lock(m_lock);

	while (!m_termination) {
		// process the active timers and obtain the next sleep interval
		Timer::Duration const duration = updateTimers();

		// processing required after time-out
		if (0 != duration) {
			m_sync.wait_for(lock, milliseconds(duration));
		}
		// no active timers, wait for event
		else {
			m_sync.wait(lock);
		}
	}
	// wait until the timer list is empty
	m_sync.wait(lock, [this] { return m_timerList.empty(); });
}

//! Update timer list
//! \brief
//! This function processes all active timers by checking their timeout,
//! calling the respective timeout callback function.
//!
//! \return Timeout in milliseconds for the next update
//! \retval 0 - Timer list is empty
Timer::Duration TimerManager::Impl::updateTimers() {
	Timer::Duration duration = 0;

	/* use a consistent time-stamp per cycle */
	steady_clock::time_point now = steady_clock::now();
	for (bool run = true; run;) {
		run = false;

		// 1. check if active timers have expired
		// Process the timer list from the beginning until the end of the segment of active timers
		auto it = m_timerList.begin();
		for (; it != m_timerList.end() && (it->m_state == Timer::Impl::State::kActive) &&
		       (0 == it->getDuration(now)); ++it) {
			it->m_state = Timer::Impl::State::kExpired;

			// schedule another run
			run = true;
		}

		// 2. move all expired timers to the end of the list
		if (it != m_timerList.begin()) {
			// move the segment to the end of the list
			m_timerList.splice(m_timerList.end(), m_timerList, m_timerList.begin(), it);

			// 3. process all expired timers
			while (m_timerList.end() != (it = std::find_if(m_timerList.begin(), m_timerList.end(),
			                                               [](Timer::Impl const &timer) {
				                                               return timer.m_state == Timer::Impl::State::kExpired;
			                                               }))) {
				it->m_state = Timer::Impl::State::kIdle;

				// mark the timer as locked to prevent its destruction
				it->m_access = Timer::Impl::Access::kLocked;

				if (it->m_inst && it->m_callbackHandler) {
					// invoke the timer call-back with released lock
					m_lock.unlock();
					it->m_callbackHandler(*it->m_inst);
					m_lock.lock();
				}

				if (it->m_access != Timer::Impl::Access::kDestruct) {
					it->m_access = Timer::Impl::Access::kDirect;
				} else // timer to be destroyed
				{
					m_timerList.erase(it);
				}
			}
		}

		// update the current time stamp to compensate for runtime delays of the loop
		now = steady_clock::now();

		// the first list entry determines the next invocation interval
		duration = m_timerList.empty() ? 0 : m_timerList.front().getDuration(now);
	}
	return duration;
}


/* ******************************************************************************************* *
 *                                  Timer::Impl implementation                                 *
 * ******************************************************************************************* */

//! create - Create Timer Implementation
//! \param timer - Timer instance (API interface)
//! \param manager - Associated timer manager
//! \param callback - Timeout callback function
//! \return Pointer to created timer instance
//! \retval nullptr - Unable to create timer instance
Timer::Impl *Timer::Impl::create(Timer &timer, TimerManager::Impl * manager, Callback &&callback)
{
	return manager ? manager->createTimer(timer, std::move(callback)) : nullptr;
}

//! destroy - destroy a timer implementation
//! \brief
//! Destroys an instance via direct disassociation from a timer manager
//!
//! \param timer - Pointer to timer instance to be destroyed
void Timer::Impl::destroy(Impl *timer) {
	if (timer)
		timer->m_manager.destroyTimer(timer);
}

//! Restart timer
//!
//! \param duration - Timer running duration to be set up. 0 stops the timer.
//! \retval true - Timer running duration updated.
//! \retval false - Timer running duration could not be updated.
bool Timer::Impl::operator=(Timer::Duration duration) {
	return m_manager.updateTimer(this, duration);
}

/* ******************************************************************************************* *
 *                                 TimerManager implementation                                 *
 * ******************************************************************************************* */

//! Constructor
//! Creates an empty timer manager thread.
TimerManager::TimerManager()
	: m_impl(new Impl)
	{ }

//! Move Constructor
TimerManager::TimerManager(TimerManager &&mov) noexcept
	: m_impl(std::move(mov.m_impl))
	{ }

//! Destructor
//! \note
//! Destroys the timer manager including its thread.
//! Blocks the caller until the thread has terminated.
//!
//! If there are still timer instances associated with the
//! timer manager, the destructor will block the caller until
//! all timer instances have been destroyed.
TimerManager::~TimerManager() = default;

TimerManager &TimerManager::operator=(TimerManager &&mov)  noexcept {
	m_impl = std::move(mov.m_impl);
	return *this;
}


/* ******************************************************************************************* *
 *                                     Timer implementation                                    *
 * ******************************************************************************************* */

//! Constructor - Timer constructor
//! Creates a timer and associates it with a timer manager.
//! The timer remains idle until started. \see Start
//!
//! \param manager - Timer manager to associate the timer with
//! \param callback - Timeout callback function
//!
//! \note
//! It is allowed to modify the timer instance from inside the call-back callback, e.g. to restart the timer.
//! It is also allowed to modify the Duration of timers associated with the \em same timer manager.
//! It is also allowed to delete the timer instance or other timers associated with the \em same timer manager from
//! inside the call-back callback, as well as create additional timers.
Timer::Timer(TimerManager &manager, Callback &&callback)
	: m_impl(Timer::Impl::create(*this, manager.m_impl.get(), std::move(callback)))
	{}

//! Destructor
Timer::~Timer() {
	Timer::Impl::destroy(m_impl);
}

Timer::operator Timer::Duration() const {
	return m_impl ? static_cast<Timer::Duration>(*m_impl) : 0U;
}

bool Timer::Start(Duration duration) {
	return m_impl && (*m_impl = duration);
}

bool Timer::operator=(Callback &&handler) {
	return m_impl && (*m_impl = std::move(handler));
}

} //namespace basic