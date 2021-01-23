//
// Created by clock on 23/01/2021.
//

#ifndef BASIC_SERVICES_SERIAL_DEVICE_WINS_IMPL_H
#define BASIC_SERVICES_SERIAL_DEVICE_WINS_IMPL_H

#include "serial-device.h"

namespace basic {

class BASIC_SERVICES_NO_EXPORT SerialDevice::Impl {

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
