//
// Created by clock on 23/01/2021.
//

#include "serial-device.h"
#include "serial-device-impl.h"

namespace basic {

/* ******************************************************************************************* *
 *                          SerialBufferedDevice::Impl implementation                          *
 * ******************************************************************************************* */

class BASIC_SERVICES_NO_EXPORT SerialBufferDevice::Impl : public SerialDevice::Impl {
private:
	size_t const m_rx_size;                 //!< Size of internal rx buffer
	std::unique_ptr<uint8_t[]> m_rx_buffer; //!< Internal rx buffer
	size_t m_rx_index = 0;                  //!< Begin index of received data chunk
	size_t m_rx_data_len = 0;               //!< Length of received data chunk

	using SerialDevice::Impl::ReadSome;

public:
	//! Constructor
	Impl(char const *name, SerialDevice::Configuration const &config, size_t size)
			: SerialDevice::Impl(name, config), m_rx_size(size),
			  m_rx_buffer((0 != m_rx_size) ? std::make_unique<uint8_t[]>(m_rx_size) : nullptr) {}

	//! Destructor
	~Impl() = default;

	bool IsValid() const noexcept { return SerialDevice::Impl::IsValid() && m_rx_buffer; }

	size_t Read(uint8_t const *&buffer) {
		// no received data chunk
		if (0 == m_rx_data_len) {
			// synchronously read data from the serial device
			m_rx_index = 0;
			m_rx_data_len = ReadSome(&m_rx_buffer[m_rx_index], m_rx_size - m_rx_index);
		}
		// return the received data chunk
		buffer = m_rx_data_len ? &m_rx_buffer[m_rx_index] : nullptr;
		return m_rx_data_len;
	}

	void Remove(size_t size) noexcept {
		if (m_rx_data_len > size) {
			m_rx_data_len -= size;
			m_rx_index += size;
		} else {
			// remove all data chunk in the buffer
			m_rx_data_len = 0;
			m_rx_index = 0;
		}
	}
};


/* ******************************************************************************************* *
 *                             SerialBufferedDevice implementation                             *
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
size_t SerialBufferDevice::Read(uint8_t const *&buffer) {
	return !m_impl ? 0 : m_impl->Read(buffer);
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
size_t SerialBufferDevice::Write(uint8_t const *data, size_t size) {
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