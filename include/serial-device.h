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
		kNone,		//!< no flow control
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
		uint32_t baudrate;			//!< baud rate in bits/second
		uint8_t data_bits;			//!< number of data bits
		Parity parity;				//!< parity type
		uint8_t stop_bits;			//!< number of stop bits
		FlowControl flow_control;	//!< flow control type
		bool gap_sync;				//!< flag: operating in gap sync mode
		unsigned int timeout;		//!< byte timeout in milliseconds

		constexpr explicit Configuration(uint32_t baud, uint8_t data = 8, Parity par = Parity::kNone, uint8_t stop = 1,
		                                 FlowControl flow_control = FlowControl::kNone, unsigned int time = 0,
		                                 bool sync = false)
				: baudrate(baud)
				, data_bits(data)
				, parity(par)
				, stop_bits(stop)
				, flow_control(flow_control)
				, gap_sync(sync)
				, timeout(time) {}
	};

	//! Constructor
	//!
	//! \param name - Name of the serial device
	//! \param config - Configuration of the serial device
	SerialDevice(char const * name, Configuration const & config);

	//! Destructor
	~SerialDevice();

	//! Move constructor
	SerialDevice(SerialDevice&&) = default;

	//! Move assigment
	SerialDevice& operator=(SerialDevice&&) = default;

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
	size_t ReadSome(uint8_t * data, size_t size);

    //! Write some data to the serial device
    //!
    //! \param data - Pointer of data to write
    //! \param size - Number of data to write
    //! \return Number of written data bytes
	size_t WriteSome(uint8_t const* data, size_t size);

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
static inline SerialDevice::ModemLine operator| (SerialDevice::ModemLine a, SerialDevice::ModemLine b)
{
	return SerialDevice::ModemLine(uint8_t(a)|uint8_t(b));
}

//! Checking ModemLine status
static inline bool operator& (SerialDevice::ModemLine a, SerialDevice::ModemLine b)
{
	return 0 != (uint8_t(a) & uint8_t(b));
}

} // namespace basic

#endif //BASIC_SERVICES_SERIAL_DEVICE_H
