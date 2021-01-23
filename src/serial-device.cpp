//
// Created by clock on 01/01/2021.
//


#include "serial-device.h"
#include "serial-device-impl.h"

namespace basic {

/* ******************************************************************************************* *
 *                                 SerialDevice implementation                                 *
 * ******************************************************************************************* */

SerialDevice::SerialDevice(char const* name, Configuration const& config) :
		m_impl{ std::make_unique<Impl>(name, config) }
{
	if (!m_impl->IsValid())
		m_impl.reset();
}

SerialDevice::~SerialDevice() = default;

size_t SerialDevice::ReadSome(uint8_t * data, size_t size)
{
	return !m_impl ? 0 : m_impl->ReadSome(data, size);
}

size_t SerialDevice::WriteSome(uint8_t const * data, size_t size)
{
	return !m_impl ? 0 : m_impl->WriteSome(data, size);
}

void SerialDevice::Close()
{
	if (m_impl) m_impl->Close();
}

SerialDevice::ModemLine SerialDevice::GetLine() const
{
	return !m_impl ? ModemLine::kNone : m_impl->GetLine();
}

SerialDevice::ModemLine SerialDevice::SetLine(ModemLine mask, ModemLine value)
{
	return !m_impl ? ModemLine::kNone : m_impl->SetLine(mask, value);
}

} // namespace basic