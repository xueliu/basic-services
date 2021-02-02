//
// Created by liu on 01.02.2021.
//

#ifndef BASIC_SERVICES_SERIAL_BUFFER_DEVICE_IMPL_H
#define BASIC_SERVICES_SERIAL_BUFFER_DEVICE_IMPL_H

#include "serial-device-impl.h"

namespace basic {

/* ******************************************************************************************* *
 *                          SerialBufferDevice::Impl definition                                *
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

	size_t ReadSome(uint8_t const *&buffer) {
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

} // namespace basic

#endif //BASIC_SERVICES_SERIAL_BUFFER_DEVICE_IMPL_H
