//
// Created by clock on 23/01/2021.
//

#include "serial-device.h"
#include "serial-device-impl.h"

namespace basic {

SerialDevice::Impl::Impl(char const *name, SerialDevice::Configuration const &config) {

}

SerialDevice::Impl::~Impl() {

}

size_t SerialDevice::Impl::ReadSome(uint8_t *data, size_t size) {
	return 0;
}

size_t SerialDevice::Impl::WriteSome(uint8_t const *data, size_t size) {
	return 0;
}

void SerialDevice::Impl::Close() {

}

SerialDevice::ModemLine SerialDevice::Impl::GetLine() const {
	return SerialDevice::ModemLine();
}

SerialDevice::ModemLine SerialDevice::Impl::SetLine(ModemLine mask, ModemLine value) {
	return SerialDevice::ModemLine();
}


} // namespace basic