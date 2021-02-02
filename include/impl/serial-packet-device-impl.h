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

	std::function<void(uint8_t const *, size_t)> const rx_handler_;

	std::vector<Delimiter> const rx_delimiters_;

	Delimiter const tx_delimiter_;

	typename Delimiter::size_type max_delimiter_size_;

	size_t max_packet_size_;
	std::unique_ptr<uint8_t[]> packet_;

	std::thread packetizer_;

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
	     size_t packet_size, std::function<void(uint8_t const *, size_t)> &&handler,
	     std::initializer_list<Delimiter> const &rx_delim,
	     Delimiter const &tx_delim)
			: SerialBufferDevice::Impl(name,SerialDevice::Configuration(
					                           config.baudrate,
					                           config.data_bits,
					                           config.parity,
					                           config.stop_bits,
					                           config.flow_control,
					                           config.timeout,
					                           true), size), rx_handler_(std::move(handler)), rx_delimiters_(rx_delim),
			  tx_delimiter_(tx_delim), max_delimiter_size_(MaxDelimiterSize(rx_delim)), max_packet_size_(packet_size),
			  packet_(std::make_unique<uint8_t[]>(max_packet_size_ + max_delimiter_size_)),
			  packetizer_(&Impl::Packetizer, this) {}

/** Destructor */
	~Impl() {
		max_delimiter_size_ = 0;
		Close();
		packetizer_.join();
	}

/** Check for valid instance */
	bool IsValid() const noexcept {
		return SerialBufferDevice::Impl::IsValid()
		       && rx_handler_
		       && max_delimiter_size_
		       && max_packet_size_
		       && packet_;
	}

	size_t WriteSome(uint8_t const *, size_t);
};

} // namespace basic

#endif //BASIC_SERVICES_SERIAL_PACKET_DEVICE_IMPL_H
