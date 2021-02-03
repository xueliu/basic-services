//
// Created by clock on 29/12/2020.
//

#ifndef BASIC_SERVICES_SERIAL_DEVICE_H
#define BASIC_SERVICES_SERIAL_DEVICE_H

#include <cstdint>
#include <memory>
#include <functional>
#include <initializer_list>
#include <cstring>


#include "basic-services_export.h"

namespace basic {

//! Class Serial Device
class BASIC_SERVICES_EXPORT SerialDevice {
public:
	//! Flow control type
	enum class FlowControl : uint8_t {
		kNone,        //!< no flow control
		kRtsCts,    //!<
		kXonXoff    //!<
	};

	//! Parity type
	enum class Parity : uint8_t {
		kNone,     //!< no parity
		kEven,
		kOdd,
		kSpace
	};

	//! Modem control lines
	enum class ModemLine : uint8_t {
		kNone = 0x00,
		kDsr = 0x01,
		kDtr = 0x02,
		kRts = 0x04,
		kCts = 0x08,
		kError = 0x80,   //!< invalid value indicator
	};

	//! Serial configuration
	struct Configuration {
		uint32_t baudrate;            //!< baud rate in bits/second
		uint8_t data_bits;            //!< number of data bits
		Parity parity;                //!< parity type
		uint8_t stop_bits;            //!< number of stop bits
		FlowControl flow_control;    //!< flow control type
		bool gap_sync;                //!< flag: operating in gap sync mode
		unsigned int timeout;        //!< byte timeout in milliseconds

		constexpr explicit Configuration(uint32_t baud, uint8_t data = 8, Parity par = Parity::kNone, uint8_t stop = 1,
		                                 FlowControl flow_control = FlowControl::kNone, unsigned int time = 0,
		                                 bool sync = false)
				: baudrate(baud), data_bits(data), parity(par), stop_bits(stop), flow_control(flow_control),
				  gap_sync(sync), timeout(time) {}
	};

	//! Constructor
	//!
	//! \param name - Name of the serial device
	//! \param config - Configuration of the serial device
	SerialDevice(char const *name, Configuration const &config);

	//! Destructor
	~SerialDevice();

	//! Move constructor
	SerialDevice(SerialDevice &&) = default;

	//! Move assigment
	SerialDevice &operator=(SerialDevice &&) = default;

	//! Check validity of the instance
	//!
	//! \retval true - Instance valid
	//! \retval false - Instance invalid
	explicit operator bool() const noexcept { return !!m_impl; }

	//! Close the serial device
	void Close();

	//! Read some data from the serial device
	//!
	//! \param data - Pointer to data buffer
	//! \param size - Size of the data buffer
	//! \return Number of read data bytes
	size_t ReadSome(uint8_t *data, size_t size);

	//! Write some data to the serial device
	//!
	//! \param data - Pointer of data to write
	//! \param size - Number of data to write
	//! \return Number of written data bytes
	size_t WriteSome(uint8_t const *data, size_t size);

	//! Retrieve the status of the modem lines
	//!
	//! \return Modem line status
	ModemLine GetLine() const;


	//! Set the status of the modem lines
	//!
	//! \param mask - Mask of modem lines to update
	//! \param value - Value of modem lines to update
	//! \return Previous status of modem lines
	ModemLine SetLine(ModemLine mask, ModemLine value);

public:
	class Impl;

private:
	std::unique_ptr<Impl> m_impl;
};

//! Combining ModemLine status
static inline SerialDevice::ModemLine operator|(SerialDevice::ModemLine a, SerialDevice::ModemLine b) {
	return SerialDevice::ModemLine(uint8_t(a) | uint8_t(b));
}

//! Checking ModemLine status
static inline bool operator&(SerialDevice::ModemLine a, SerialDevice::ModemLine b) {
	return 0 != (uint8_t(a) & uint8_t(b));
}

//! Class Serial Buffer Device
//!
//! \brief SerialBufferDevice implements a buffered serial device.
//! The buffer in the receive direction allows handling of received data chunks in a off-line way.
//!
class BASIC_SERVICES_EXPORT SerialBufferDevice {
public:
	using ModemLine = SerialDevice::ModemLine;

	//! Constructor
	SerialBufferDevice(char const *, SerialDevice::Configuration const &, size_t);

	//! Destructor
	~SerialBufferDevice();

	//! Move constructor
	SerialBufferDevice(SerialBufferDevice &&) = default;

	//! Move assignment
	SerialBufferDevice &operator=(SerialBufferDevice &&) = default;

	//! Instance validity check
	explicit operator bool() const noexcept { return !!m_impl; }

	//! Close the device
	void Close();

	//! Data retrieval from the device
	size_t ReadSome(uint8_t const *&);

	//! Data remove (acknowledging) a data chunk
	void Remove(size_t) noexcept;

	//! Write data to the device
	size_t WriteSome(uint8_t const *, size_t);

	//! Retrieve the status of the modem lines
	ModemLine GetLine() const;

	//! Set the status of the modem lines
	ModemLine SetLine(ModemLine mask, ModemLine value);

public:
	class Impl;

private:
	std::unique_ptr<Impl> m_impl;
};

//! Class SerialPacketDevice
//!
//! \brief SerialPacketDevice implements a packet based serial device.
//! Data packets are assembled from a serial data stream and passed on as
//! complete packets for processing.
class BASIC_SERVICES_EXPORT SerialPacketDevice {
public:
	//! Class for packet delimiter
	struct Delimiter {

		using size_type = uint8_t;					//!< Size type
		static constexpr size_type kMaxSize = 3;    //!< Maximum delimiter size
		size_type const size;						//!< Delimiter size
		uint8_t data[kMaxSize];                     //!< Delimiter data

		static constexpr size_type set_size(size_t sz) {
			return (sz <= kMaxSize) ? size_type(sz) : kMaxSize;
		}

		//! Default constructor
		Delimiter() : size(0), data{0} {}

		//! Constructor of arbitrary binary delimiter
		Delimiter(std::initializer_list<uint8_t> const &arr)
			: size(set_size(arr.size())), data{} {
			std::memcpy(data, arr.begin(), size);
		}

		//! Constructor from string
		explicit Delimiter(char const *str)
			: size(set_size(strlen(str))), data{} {
			std::memcpy(data, str, size);
		}
	}; // struct Delimiter

	using RxHandler = std::function<void(uint8_t const *, size_t)>;

	//! Constructor
	SerialPacketDevice(char const*, SerialDevice::Configuration const&, size_t,
	                   size_t, RxHandler&&,
	                   std::initializer_list<Delimiter> const&,
	                   Delimiter const&);

	//! Destructor
	~SerialPacketDevice();

	//! Move constructor
	SerialPacketDevice(SerialPacketDevice&&) = default;

	//! Move assignment
	SerialPacketDevice& operator=(SerialPacketDevice&&) = default;

	//! Instance validity check
	explicit operator bool() const noexcept { return !!m_impl; }

	//!
	size_t WriteSome(uint8_t const*, size_t);

	using ModemLine = SerialDevice::ModemLine;

	//! Retrieve the status of the modem lines
	ModemLine GetLine() const;

	//! Set the status of the modem lines
	ModemLine SetLine(ModemLine mask, ModemLine value);

public:
	class Impl;

private:
	std::unique_ptr<Impl> m_impl;
};

} // namespace basic

#endif //BASIC_SERVICES_SERIAL_DEVICE_H
