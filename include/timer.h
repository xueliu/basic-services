//
// Copyright Xue Liu <liuxuenetmail@mail.com>
//

#ifndef BASIC_SERVICES_TIMER_H
#define BASIC_SERVICES_TIMER_H

#include <cstdint>
#include <memory>
#include <functional>

#include "basic-services_export.h"

namespace basic {

class Timer;

class BASIC_SERVICES_EXPORT TimerManager {
public:
	//! Constructor
	//! \brief
	//! Creates a timer manager
	TimerManager();

	//! Destructor
	~TimerManager();

	//! Move constructor
	TimerManager(TimerManager &&) noexcept;

	//! Move assignment
	TimerManager &operator=(TimerManager &&) noexcept;

private:
	friend Timer;
private:
	// PImpl Class
	class Impl;

	std::unique_ptr<Impl> m_impl;
};

//! Timer class
class BASIC_SERVICES_EXPORT Timer {
public:
	class Impl;

	//! Timer duration type
	using Duration = uint32_t;

	//! Timer callback handler type */
	using Callback = std::function<void(Timer &)>;

	//! Constructor
	explicit Timer(TimerManager &, Callback && = Callback());

	//! Destructor
	//!
	//! \brief
	//! Destroys the timer instance and removes it from the associated timer manager
	virtual ~Timer();

	Timer(Timer const &) = delete;

	Timer &operator=(Timer const &) = delete;

	//! Start timer
	//!
	//! \brief
	//! Starts or restarts the timer with the specified duration time in milliseconds.
	//!
	//! \param duration - Timer duration in milliseconds. A value of 0 stops a running timer.
	//! \retval true - Successful operation
	//! \retval false - Failed to modify timer duration
	bool Start(Duration duration);

	//! Stop timer
	//!
	//! \brief
	//! Stops the timer. No time-out callback will occur once this function returns.
	void Stop() { Start(0); }

	//! Retrieval of the remaining running-time of the timer
	//!
	//! \return Remaining running time in milliseconds.
	//! \retval 0 - Timer not running
	operator Duration() const;

	//! Timer call-back handler reassignment
	//!
	//! \brief
	//! Assigns a new call-back handler. The timer must not be running. \see Stop
	//!
	//! \param callback - Timer callback handler to assign
	//! \return true - Timer callback handler assigned
	//! \return false -  Failed to assign call-back handler
	bool operator = (Callback && callback);

private:
	Impl * m_impl;
};

} // namespace basic

#endif //BASIC_SERVICES_TIMER_H
