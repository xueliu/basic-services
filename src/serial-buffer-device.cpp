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
	size_t const m_rx_size;
	std::unique_ptr<uint8_t[]> m_rx_buffer;

	size_t m_rx_index = 0;
	size_t m_rx_data_len = 0;

	using SerialDevice::Impl::ReadSome;

public:
	Impl(char const *name, SerialDevice::Configuration const &config, size_t size)
			: SerialDevice::Impl(name, config), m_rx_size(size)
			, m_rx_buffer((0 != m_rx_size) ? std::make_unique<uint8_t[]>(m_rx_size) : nullptr) {}

	~Impl() = default;

	bool IsValid() const noexcept { return SerialDevice::Impl::IsValid() && m_rx_buffer; }

	size_t Read(uint8_t const *&buffer) {
		if (0 == m_rx_data_len) /* no active chunk */
		{
			/* try to read a new chunk from the coprocessor */
			m_rx_index = 0;
			m_rx_data_len = SerialDevice::Impl::ReadSome(&m_rx_buffer[m_rx_index], m_rx_size - m_rx_index);
		}
		/* return the current data chunk */
		buffer = m_rx_data_len ? &m_rx_buffer[m_rx_index] : nullptr;
		return m_rx_data_len;
	}
	
	void Remove(size_t size) noexcept {
		if (m_rx_data_len > size) {
			m_rx_data_len -= size;
			m_rx_index += size;
		} else {
			m_rx_data_len = 0;
			m_rx_index = 0;
		}
	}
};


/* ******************************************************************************************* *
 *                             SerialBufferedDevice implementation                             *
 * ******************************************************************************************* */

SerialBufferDevice::SerialBufferDevice(char const *name, SerialDevice::Configuration const &config, size_t size)
		: m_impl{std::make_unique<Impl>(name, config, size)} {
	if (!m_impl->IsValid())
		m_impl.reset();
}

SerialBufferDevice::~SerialBufferDevice() {}

size_t SerialBufferDevice::Read(uint8_t const *&buffer) {
	return !m_impl ? 0 : m_impl->Read(buffer);
}

void SerialBufferDevice::Remove(size_t size) noexcept {
	if (m_impl) m_impl->Remove(size);
}

size_t SerialBufferDevice::Write(uint8_t const *data, size_t size) {
	return !m_impl ? 0 : m_impl->WriteSome(data, size);
}

void SerialBufferDevice::Close() {
	if (m_impl) m_impl->Close();
}

SerialDevice::ModemLine SerialBufferDevice::GetLine() const {
	return !m_impl ? ModemLine::kNone : m_impl->GetLine();
}

SerialDevice::ModemLine SerialBufferDevice::SetLine(ModemLine mask, ModemLine value) {
	return !m_impl ? ModemLine::kNone : m_impl->SetLine(mask, value);
}


} // namespace basic