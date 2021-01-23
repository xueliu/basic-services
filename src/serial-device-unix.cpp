//
// Created by clock on 23/01/2021.
//

#include <fcntl.h>
#include <sys/file.h>

#include <mutex>

#include "serial-device.h"
#include "serial-device-impl.h"

namespace basic {

/* ******************************************************************************************* *
 *                                     local implementation                                    *
 * ******************************************************************************************* */

namespace {

//! Baud rate conversion
//!
//! \param baud - Baud rate in bits per second
//! \return - Specific baud rate
constexpr speed_t Baudrate(uint32_t baud) {
	switch (baud) {
#ifdef B50
		case 50 :
			return B50;
#endif
#ifdef B75
		case 75 :
			return B75;
#endif
#ifdef B110
		case 110 :
			return B110;
#endif
#ifdef B134
		case 134 :
			return B134;
#endif
#ifdef B150
		case 150 :
			return B150;
#endif
#ifdef B200
		case 200 :
			return B200;
#endif
#ifdef B300
		case 300 :
			return B300;
#endif
#ifdef B600
		case 600 :
			return B600;
#endif
#ifdef B1200
		case 1200 :
			return B1200;
#endif
#ifdef B1800
		case 1800 :
			return B1800;
#endif
#ifdef B2400
		case 2400 :
			return B2400;
#endif
#ifdef B4800
		case 4800 :
			return B4800;
#endif
#ifdef B9600
		case 9600 :
			return B9600;
#endif
#ifdef B19200
		case 19200 :
			return B19200;
#endif
#ifdef B38400
		case 38400 :
			return B38400;
#endif
#ifdef B57600
		case 57600 :
			return B57600;
#endif
#ifdef B115200
		case 115200 :
			return B115200;
#endif
#ifdef B230400
		case 230400 :
			return B230400;
#endif
#ifdef B460800
		case 460800 :
			return B460800;
#endif
#ifdef B500000
		case 500000 :
			return B500000;
#endif
#ifdef B576000
		case 576000 :
			return B576000;
#endif
#ifdef B921600
		case 921600 :
			return B921600;
#endif
#ifdef B1000000
		case 1000000 :
			return B1000000;
#endif
#ifdef B1152000
		case 1152000 :
			return B1152000;
#endif
#ifdef B1500000
		case 1500000 :
			return B1500000;
#endif
#ifdef B2000000
		case 2000000 :
			return B2000000;
#endif
#ifdef B2500000
		case 2500000 :
			return B2500000;
#endif
#ifdef B3000000
		case 3000000 :
			return B3000000;
#endif
#ifdef B3500000
		case 3500000 :
			return B3500000;
#endif
#ifdef B4000000
		case 4000000 :
			return B4000000;
#endif
		default:
			return B0;
	}
}

//! Disable a termios configuration flag
//! \param val - flag to be disabled
//! \return - inverted flag
constexpr tcflag_t Mask(tcflag_t val) { return tcflag_t(~val); }

//! Serial configuration conversion
//!
//! \param[out] options - Target specific configuration
//! \param[in] config - Source serial configuration
//! \retval 0 - Configuration valid
//! \retval -1 - Invalid configuration
int SetConfig(struct termios &options, SerialDevice::Configuration const &config) noexcept {
	speed_t const baudrate = Baudrate(config.baudrate);
	if (B0 != baudrate) // baud rate recognised
	{
		if (((config.stop_bits == 1) || (config.stop_bits == 2))
		    && (config.data_bits >= 5)
		    && (config.data_bits <= (config.parity == SerialDevice::Parity::kNone ? 8 : 7))
				) {
			// set up baud rate
			if ((0 <= cfsetispeed(&options, baudrate)) && (0 <= cfsetospeed(&options, baudrate))) {
				static tcflag_t const data_config[] =
						{
								CS5, CS6, CS7, CS8
						};
				options.c_cflag &= Mask(CSIZE);
				options.c_cflag |= data_config[
						(config.parity == SerialDevice::Parity::kNone ? config.data_bits : config.data_bits + 1) - 5];

				switch (config.parity) {
					case SerialDevice::Parity::kEven:
						options.c_cflag |= PARENB;
						options.c_cflag &= Mask(PARODD);
						break;

					case SerialDevice::Parity::kOdd:
						options.c_cflag |= (PARENB | PARODD);
						break;

					default:
						options.c_cflag &= Mask(PARENB);
				}

				if (options.c_cflag & PARENB) {
					options.c_iflag |= (INPCK | ISTRIP);
					options.c_iflag &= Mask(IGNPAR | PARMRK);
				} else {
					options.c_iflag &= Mask(INPCK | ISTRIP | IGNPAR | PARMRK);
				}

				if (config.stop_bits == 1) {
					options.c_cflag &= Mask(CSTOPB);
				} else {
					options.c_cflag |= CSTOPB;
				}

				switch (config.flow_control) {
					case SerialDevice::FlowControl::kRtsCts:
						options.c_cflag |= CRTSCTS;
						options.c_iflag &= Mask(IXON | IXOFF | IXANY);
						break;

					case SerialDevice::FlowControl::kXonXoff:
						options.c_iflag |= (IXON | IXOFF | IXANY);
						options.c_cflag &= Mask(CRTSCTS);
						break;

					default:
						options.c_cflag &= Mask(CRTSCTS);
						options.c_iflag &= Mask(IXON | IXOFF | IXANY);
						break;
				}

				return 0;
			}
		}
	}
	return -1;
}

} // anonymous namespace


/* ******************************************************************************************* *
 *                              SerialDevice::Impl implementation                              *
 * ******************************************************************************************* */

//! Constructor of SerialDevice::Impl
//!
//! \param name - Name of the serial device
//! \param config - Configuration of the serial device
SerialDevice::Impl::Impl(char const* name, SerialDevice::Configuration const& config)
		: fd_(open(name, O_RDWR | O_NOCTTY | O_NDELAY)) // non-blocking
		, state_(!config.gap_sync ? State::kTimeout : State::kWaiting) {
	if ( (fd_ >= 0)
	     && ((flock(fd_, LOCK_EX | LOCK_NB) < 0) || (Init(config) < 0)) )
	{
		close(fd_); fd_ = -1;
	}
}

//! Destructor
SerialDevice::Impl::~Impl() {
	if (fd_ >= 0)
	{
		Close();
		close(fd_); fd_ = -1;
	}
}

//! Initialisation of the serial device
//!
//! \param config - Configuration of the serial device
//! \retval 0 - Serial device initialised
//! \retval 1 - Failed to initialise serial device
int SerialDevice::Impl::Init(Configuration const& config)
{
	int result = -1;

	speed_t const baudrate = Baudrate(config.baudrate);

	// baud rate recognised
	if(B0 == baudrate) return result;

	// switch to blocking mode */
	result = fcntl(fd_, F_SETFL, 0);
	if(result < 0) return result;

	result = tcgetattr(fd_, &options_);
	if(result < 0) return result;

	result = SetConfig(options_, config);
	if(result < 0) return result;

	// select raw input
	options_.c_lflag &= Mask(ICANON | ECHO | ECHOE | ISIG);
	// enable receiver, ignore CD signal
	options_.c_cflag |= (CREAD | CLOCAL);
	// select raw output
	options_.c_oflag &= Mask(OPOST);
	// select RX timeout
	options_.c_cc[VMIN]  = config.gap_sync ? 1 : 0;
	options_.c_cc[VTIME] = cc_t((config.timeout + 99U) / 100U);

	result = tcsetattr(fd_, TCSANOW, &options_);

	return result;
}

//! Read some data from the serial device
//!
//! \param data - Pointer to data buffer
//! \param size - Size of the data buffer
//! \return Number of read data bytes
size_t SerialDevice::Impl::ReadSome(uint8_t* data, size_t size)
{
	if (state_ == State::kShutdown)
		return 0;

	ssize_t const sz = read(fd_, data, size);

	if (state_ != State::kTimeout)
	{
		// switch between blocking mode and time-out mode
		std::lock_guard<std::mutex> guard(mutex_);
		// sz = 0 indicates end of file
		// sz > 0 the number of bytes read is returned
		if ( ( (0 == sz) && (state_ == State::kReading) )   // switch to blocking mode
		     || ( (0  < sz) && (state_ == State::kWaiting)) )  // switch to timeout mode
		{
			// switch to blocking mode
			if (state_ == State::kReading)
			{
				options_.c_cc[VMIN] = 1;    // read() is blocked, until there is one character in the buffer
				// and the timer is starting counter at the same time. After that
				// when VTIME is expired OR VMIN characters are received,
				// the read will return.
				state_ = State::kWaiting;

				tcsetattr(fd_, TCSANOW, &options_);
			}
				// switch to timeout mode
			else if (0 != options_.c_cc[VTIME])
			{
				options_.c_cc[VMIN] = 0;    // read() is return immediately characters are received
				// otherwise read() is waiting for the next character for VTIME.
				state_ = State::kReading;

				tcsetattr(fd_, TCSANOW, &options_);
			}
		}
	}
	return (sz < 0) ? 0 : size_t(sz);
}

//! Write some data to the serial device
//!
//! \param data - Pointer of data to write
//! \param size - Number of data to write
//! \return Number of written data bytes
size_t SerialDevice::Impl::WriteSome(uint8_t const* data, size_t size)
{
	if (state_ == State::kShutdown)
		return 0;

	ssize_t const sz = write(fd_, data, size);
	return (sz < 0) ? 0 : size_t(sz);
}

//! Closing the device
//!
//! \note
//! Closes the serial device, unblocking any pending calls.
void SerialDevice::Impl::Close()
{
	std::lock_guard<std::mutex> guard(mutex_);

	state_ = State::kShutdown;

	// switch to unblocking mode
	options_.c_cc[VMIN]  = 0;
	options_.c_cc[VTIME] = 0;
	tcsetattr(fd_, TCSANOW, &options_);

	fcntl(fd_, F_SETFL, FNDELAY);
}

//! Retrieve the status of the modem lines
//!
//! \return Modem line status
SerialDevice::ModemLine SerialDevice::Impl::GetLine() const
{
	int status = 0;
	return (0 <= ioctl(fd_, TIOCMGET, &status)) ? LineStatus(status) : ModemLine::kError;
}

//! Set the status of the modem lines
//!
//! \param mask - Mask of modem lines to update
//! \param value - Value of modem lines to update
//! \return Previous status of modem lines
SerialDevice::ModemLine SerialDevice::Impl::SetLine(ModemLine mask, ModemLine value)
{
	std::lock_guard<std::mutex> guard(mutex_);

	int status = 0;
	if (0 <= ioctl(fd_, TIOCMGET, &status))
	{
		int new_status = status;

		if (!(options_.c_cflag & CRTSCTS))
		{
			if (mask & ModemLine::kRts)
			{
				if (value & ModemLine::kRts)
					new_status |= TIOCM_RTS;
				else
					new_status &= ~TIOCM_RTS;
			}
		}

		if (mask & ModemLine::kDtr)
		{
			if (value & ModemLine::kDtr)
				new_status |= TIOCM_DTR;
			else
				new_status &= ~TIOCM_DTR;
		}

		if (0 <= ioctl(fd_, TIOCMSET, &new_status))
			return LineStatus(status);
	}
	return ModemLine::kError;
}

} // namespace basic