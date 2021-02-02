//
// Created by clock on 23/01/2021.
//

#include "serial-device.h"
#include "serial-buffer-device-impl.h"

namespace basic {


/* ******************************************************************************************* *
 *                             SerialBufferDevice implementation                             *
 * ******************************************************************************************* */

//! Constructor
//!
//! \param name - Name of the serial device
//! \param config - Configuration of the serial device
//! \param size - Size of rx buffer
SerialBufferDevice::SerialBufferDevice(char const *name, SerialDevice::Configuration const &config, size_t size)
		: m_impl{std::make_unique<Impl>(name, config, size)} {
	if (!m_impl->IsValid())
		m_impl.reset();
}

SerialBufferDevice::~SerialBufferDevice() = default;

//! Data retrieval from the device
//!
//! \param buffer - Reference to the received data chunk
//! \return - Size of the received data chunk
size_t SerialBufferDevice::ReadSome(uint8_t const *&buffer) {
	return !m_impl ? 0 : m_impl->ReadSome(buffer);
}

//! Data remove (acknowledging) a data chunk
//!
//! \brief This function acknowledges a data chunk from the device,
//!			removing it from the internal buffer.
//! \param size - Size of the data chunk to remove
void SerialBufferDevice::Remove(size_t size) noexcept {
	if (m_impl) m_impl->Remove(size);
}


//! Write data to the device
//!
//! \param data - Pointer to the data to be written
//! \param size - Size of the data
//! \return - Size of the written data
size_t SerialBufferDevice::WriteSome(uint8_t const *data, size_t size) {
	return !m_impl ? 0 : m_impl->WriteSome(data, size);
}

//! Close the device
//!
void SerialBufferDevice::Close() {
	if (m_impl) m_impl->Close();
}

//! Retrieve the status of the modem lines
//!
//! \return - Modem line status
SerialDevice::ModemLine SerialBufferDevice::GetLine() const {
	return !m_impl ? ModemLine::kNone : m_impl->GetLine();
}

//! Set the status of the modem lines
//!
//! \param mask -  Mask of modem lines to update
//! \param value - Value of modem lines to update
//! \return - Previous status of modem lines
SerialDevice::ModemLine SerialBufferDevice::SetLine(ModemLine mask, ModemLine value) {
	return !m_impl ? ModemLine::kNone : m_impl->SetLine(mask, value);
}


} // namespace basic