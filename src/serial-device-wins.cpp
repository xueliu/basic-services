//
// Created by clock on 23/01/2021.
//

#include "serial-device.h"
#include "serial-device-impl.h"

namespace basic {

namespace {
//! Baud rate conversion
//!
//! \param baud - Baud rate in bits per second
//! \return - Specific baud rate
constexpr DWORD Baudrate(uint32_t baud) {
	switch (baud) {
#ifdef CBR_110
		case 110 :
			return CBR_110;
#endif
#ifdef CBR_300
		case 300 :
			return CBR_300;
#endif
#ifdef CBR_600
		case 600 :
			return CBR_600;
#endif
#ifdef CBR_1200
		case 1200 :
			return CBR_1200;
#endif
#ifdef CBR_2400
		case 2400 :
			return CBR_2400;
#endif
#ifdef CBR_4800
		case 4800 :
			return CBR_4800;
#endif
#ifdef CBR_9600
		case 9600 :
			return CBR_9600;
#endif
#ifdef CBR_19200
		case 19200 :
			return CBR_19200;
#endif
#ifdef CBR_38400
		case 38400 :
			return CBR_38400;
#endif
#ifdef CBR_56000
		case 56000 :
			return CBR_56000;
#endif
#ifdef CBR_57600
		case 57600 :
			return CBR_57600;
#endif
#ifdef CBR_115200
		case 115200 :
			return CBR_115200;
#endif
#ifdef CBR_128000
		case 128000 :
			return CBR_128000;
#endif
#ifdef CBR_256000
		case 256000 :
			return CBR_256000;
#endif
		default:
			return CBR_115200;
	}
}

//! Serial configuration conversion
//!
//! \param[out] options - Target specific configuration
//! \param[in] config - Source serial configuration
//! \retval 0 - Configuration valid
//! \retval -1 - Invalid configuration
int SetConfig(struct _DCB &options, SerialDevice::Configuration const &config) noexcept {
	memset(&options, 0, sizeof(options));
	options.DCBlength = sizeof(options);

	options.BaudRate = Baudrate(config.baudrate);
	if (((config.stop_bits == 1) || (config.stop_bits == 2))
	    && (config.data_bits >= 5)
	    && (config.data_bits <= (config.parity == SerialDevice::Parity::kNone ? 8 : 7))
			) {

		static BYTE const data_config[] = {
				4, 5, 6, 7, 8
		};

		options.ByteSize = data_config[
				(config.parity == SerialDevice::Parity::kNone ? config.data_bits : config.data_bits + 1) - 4];

		switch (config.parity) {
			case SerialDevice::Parity::kNone:
				options.Parity = NOPARITY;
				options.fParity = 0;
				break;

			case SerialDevice::Parity::kEven:
				options.Parity = EVENPARITY;
				options.fParity = 1;
				break;

			case SerialDevice::Parity::kOdd:
				options.Parity = ODDPARITY;
				options.fParity = 1;
				break;

			case SerialDevice::Parity::kSpace:
				options.Parity = SPACEPARITY;
				options.fParity = 1;
				break;

			default:
				options.Parity = NOPARITY;
				options.fParity = 0;
		} // switch config.parity

		if (config.stop_bits == 1) {
			options.StopBits = 0;
		} else {
			options.StopBits = 2;
		}

		switch (config.flow_control) {
			case SerialDevice::FlowControl::kRtsCts:
				options.fRtsControl = RTS_CONTROL_ENABLE;
				options.fOutxCtsFlow = TRUE;
				options.fInX = FALSE;
				options.fOutX = FALSE;
				break;

			case SerialDevice::FlowControl::kXonXoff:
				options.fRtsControl = RTS_CONTROL_DISABLE;
				options.fOutxCtsFlow = FALSE;
				options.fInX = TRUE;
				options.fOutX = TRUE;
				break;

			default:
				options.fRtsControl = RTS_CONTROL_DISABLE;
				options.fOutxCtsFlow = FALSE;
				options.fInX = FALSE;
				options.fOutX = FALSE;
				break;
		}
	}

	return 0;
}

} // anonymous namespace

//! Initialisation of the serial device
//!
//! \param config - Configuration of the serial device
//! \retval 0 - Serial device initialised
//! \retval -1 - Failed to initialise serial device
int SerialDevice::Impl::Init(Configuration const &config)
{
	int result = -1;

	result = SetConfig(option_, config);
	if (result < 0) return result;

	/* misc parameters */
	option_.fErrorChar = FALSE;
	option_.fBinary = TRUE;
	option_.fNull = FALSE;
	option_.fAbortOnError = FALSE;
	option_.wReserved = 0;
	option_.XonLim = 2;
	option_.XoffLim = 4;
	option_.XonChar = 0x13;
	option_.XoffChar = 0x19;
	option_.EvtChar = 0;

	if (!SetCommState( fd_, &option_ ) )
	{
		result = -1;
		return result;
	}

	if(!SetCommMask( fd_, 0 ) )
	{
		result = -1;
		return result;
	}

	return result;
}

SerialDevice::Impl::Impl(char const * name, SerialDevice::Configuration const &config)
	: fd_ (CreateFile( name, GENERIC_READ | GENERIC_WRITE, 0, nullptr, OPEN_EXISTING, 0, nullptr ))
	, state_(!config.gap_sync ? State::kTimeout : State::kWaiting) {
	if (fd_ != INVALID_HANDLE_VALUE && ( Init(config) < 0) )
	{
		( void )CloseHandle( fd_ );
		fd_ = INVALID_HANDLE_VALUE;
	}
}

SerialDevice::Impl::~Impl() {

}

size_t SerialDevice::Impl::ReadSome(uint8_t *data, size_t size) {
	return 0;
}

size_t SerialDevice::Impl::WriteSome(uint8_t const *data, size_t size) {
	return 0;
}

void SerialDevice::Impl::Close() {
	( void )CloseHandle( fd_ );
	fd_ = INVALID_HANDLE_VALUE;
}

SerialDevice::ModemLine SerialDevice::Impl::GetLine() const {
	return SerialDevice::ModemLine();
}

SerialDevice::ModemLine SerialDevice::Impl::SetLine(ModemLine mask, ModemLine value) {
	return SerialDevice::ModemLine();
}


} // namespace basic