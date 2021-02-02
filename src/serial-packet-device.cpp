//
// Created by liu on 01.02.2021.
//

#include "serial-device.h"
#include "serial-packet-device-impl.h"

namespace basic {

/* ******************************************************************************************* *
*                           SerialPacketDevice::Impl implementation                           *
* ******************************************************************************************* */

int SerialPacketDevice::Impl::FindDelimiter(uint8_t const *dp, size_t len, bool best_match) {
	int match = 0;
	if (len)
		for (Delimiter const &del : m_rxDelimiters) {
			if (len < del.size) {
				if (0 == std::memcmp(del.data, dp, len)) {
					match = int(-len);

					if (!best_match)
						break;
				}
			} else {
				if (0 == std::memcmp(dp, del.data, del.size)) {
					match = int(del.size);
					break;
				}
			}
		}
	return match;
}


size_t SerialPacketDevice::Impl::WriteSome(uint8_t const *data, size_t size) {
	size_t sz = SerialBufferDevice::Impl::WriteSome(data, size);
	if ((sz >= size) && m_txDelimiter.size)
		sz += SerialBufferDevice::Impl::WriteSome(m_txDelimiter.data, m_txDelimiter.size);
	return sz;
}


void SerialPacketDevice::Impl::Packetizer() {
	if (IsValid()) {
		bool discard = false;
		size_t packet_len = 0;
		size_t packet_end = 0;

		while (MAX_DELIMITER_SIZE) {

			uint8_t const *data;
			size_t const size = SerialBufferDevice::Impl::ReadSome(data);
			if (0 == size) {
				if (FindDelimiter(&m_packet[packet_len], packet_end - packet_len, true) > 0) {
					if (!discard) {
						m_rxHandler(&m_packet[0], packet_len);
					}

					discard = false;
					packet_len = 0;
					packet_end = 0;
				}
			} else {
				for (size_t i = 0; i < size; ++i) {
					m_packet[packet_end++] = *data++;

					int const match = FindDelimiter(&m_packet[packet_len], packet_end - packet_len);

					if (match == 0) {
						if (!discard && ((packet_len = packet_end) > m_maxPacketSize)) {
							discard = true;
							packet_len = 0;
							packet_end = 0;
						}
					} else if (match > 0) {
						if (!discard)
							m_rxHandler(&m_packet[0], packet_len);

						if ((packet_end - packet_len) > size_t(match)) {
							--data;
							--i;
						}

						discard = false;
						packet_len = 0;
						packet_end = 0;
					}
				}

				Remove(size);
			}
		}
	}
}


/* ******************************************************************************************* *
 *                              SerialPacketDevice implementation                              *
 * ******************************************************************************************* */

SerialPacketDevice::SerialPacketDevice(char const *name, SerialDevice::Configuration const &config, size_t size,
                                       size_t packet_size, std::function<void(uint8_t const *, size_t)> &&handler,
                                       std::initializer_list<Delimiter> const &rx_delim,
                                       Delimiter const &tx_delim)
		: m_impl(new Impl(name, config, size, packet_size, std::move(handler), rx_delim, tx_delim)) {
	if (!m_impl->IsValid())
		m_impl.reset();
}

SerialPacketDevice::~SerialPacketDevice() = default;

size_t SerialPacketDevice::WriteSome(uint8_t const *data, size_t size) {
	return !m_impl ? 0 : m_impl->WriteSome(data, size);
}

SerialDevice::ModemLine SerialPacketDevice::GetLine() const {
	return !m_impl ? ModemLine::kNone : m_impl->GetLine();
}

SerialDevice::ModemLine SerialPacketDevice::SetLine(ModemLine mask, ModemLine value) {
	return !m_impl ? ModemLine::kNone : m_impl->SetLine(mask, value);
}


} // namespace basic