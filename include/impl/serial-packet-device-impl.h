//
// Created by liu on 01.02.2021.
//

#ifndef BASIC_SERVICES_SERIAL_PACKET_DEVICE_IMPL_H
#define BASIC_SERVICES_SERIAL_PACKET_DEVICE_IMPL_H

#include <thread>

#include "serial-buffer-device-impl.h"

namespace basic {

/* ******************************************************************************************* *
*                           SerialPacketDevice::Impl definition                               *
* ******************************************************************************************* */

class BASIC_SERVICES_NO_EXPORT SerialPacketDevice::Impl : public SerialBufferDevice::Impl {
private:

	SerialPacketDevice::RxHandler const m_rxHandler;

	std::vector<Delimiter> const m_rxDelimiters;

	Delimiter const m_txDelimiter;

	typename Delimiter::size_type MAX_DELIMITER_SIZE;

	size_t m_maxPacketSize;
	std::unique_ptr<uint8_t[]> m_packet;

	std::thread m_packetizer;

	void Packetizer();

	static typename Delimiter::size_type MaxDelimiterSize(std::initializer_list<Delimiter> const &delimiters) {
		typename Delimiter::size_type sz = 0;
		for (Delimiter const &del : delimiters)
			if (sz < del.size)
				sz = del.size;
		return sz;
	}

	int FindDelimiter(uint8_t const *dp, size_t len, bool = false);

public:

	Impl(char const *name, SerialDevice::Configuration const &config, size_t size,
	     size_t packet_size, SerialPacketDevice::RxHandler && handler,
	     std::initializer_list<Delimiter> const &rx_delim,
	     Delimiter const &tx_delim)
			: SerialBufferDevice::Impl(name,SerialDevice::Configuration(
					                           config.baudrate,
					                           config.data_bits,
					                           config.parity,
					                           config.stop_bits,
					                           config.flow_control,
					                           config.timeout,
					                           true), size), m_rxHandler(std::move(handler)), m_rxDelimiters(rx_delim),
			  m_txDelimiter(tx_delim), MAX_DELIMITER_SIZE(MaxDelimiterSize(rx_delim)), m_maxPacketSize(packet_size),
			  m_packet(std::make_unique<uint8_t[]>(m_maxPacketSize + MAX_DELIMITER_SIZE)),
			  m_packetizer(&Impl::Packetizer, this) {}

/** Destructor */
	~Impl() {
		MAX_DELIMITER_SIZE = 0;
		Close();
		m_packetizer.join();
	}

/** Check for valid instance */
	bool IsValid() const noexcept {
		return SerialBufferDevice::Impl::IsValid()
		       && m_rxHandler
		       && MAX_DELIMITER_SIZE
		       && m_maxPacketSize
		       && m_packet;
	}

	size_t WriteSome(uint8_t const *, size_t);
};

} // namespace basic

#endif //BASIC_SERVICES_SERIAL_PACKET_DEVICE_IMPL_H
