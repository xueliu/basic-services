//
// Created by clock on 23/01/2021.
//

#ifndef BASIC_SERVICES_SERIAL_DEVICE_WINS_IMPL_H
#define BASIC_SERVICES_SERIAL_DEVICE_WINS_IMPL_H

#include <mutex>

#include <windows.h>

#include "serial-device.h"

namespace basic {

class BASIC_SERVICES_NO_EXPORT SerialDevice::Impl {
private:
	HANDLE fd_ = INVALID_HANDLE_VALUE;

	DCB option_;

	std::mutex mutex_;

	enum State : uint8_t {
		kTimeout,   //!< operating in time-out mode
		kWaiting,   //!< gap-sync waiting for data
		kReading,   //!< gap-sync reading data
		kShutdown   //!< device shutting down
	} state_ = kTimeout;

	int Init(Configuration const &config);

	// Translation of system specific line status to generic line status
//	static ModemLine LineStatus(int status) {
//		ModemLine result = ModemLine::kNone;
//
//		if (status & TIOCM_RTS)
//			result = result | ModemLine::kRts;
//		if (status & TIOCM_DTR)
//			result = result | ModemLine::kDtr;
//		if (status & TIOCM_CTS)
//			result = result | ModemLine::kCts;
//		if (status & TIOCM_DSR)
//			result = result | ModemLine::kDsr;
//
//		return result;
//	}
public:
	Impl(char const * name, Configuration const & config);

	~Impl();

public:
	bool IsValid() const noexcept { return true; }

	size_t ReadSome(uint8_t * data, size_t size);

	size_t WriteSome(uint8_t const * data, size_t size);

	void Close();

	ModemLine GetLine() const;

	ModemLine SetLine(ModemLine mask, ModemLine value);
};

} // namespace basic

#endif //BASIC_SERVICES_SERIAL_DEVICE_WINS_IMPL_H
