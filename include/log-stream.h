//
// Created by liu on 27.01.2021.
//

#ifndef BASIC_SERVICES_LOG_STREAM_H
#define BASIC_SERVICES_LOG_STREAM_H

#include "types.h"
#include "noncopyable.h"

namespace basic {

namespace detail {

const int kSmallBuffer = 4000;
const int kLargeBuffer = 4000*1000;

//! Class FixedBuffer
//! \tparam SIZE
//! \note Copy from muduo
template<int SIZE>
class BASIC_SERVICES_NO_EXPORT FixedBuffer : noncopyable
{
public:
	FixedBuffer()
		: m_cur(m_data)
	{
		setCookie(cookieStart);
	}

	~FixedBuffer()
	{
		setCookie(cookieEnd);
	}

	//! Append new data to the buffer
	size_t append(const char* buf, size_t len)
	{
		int available_space = avail();
		if (implicit_cast<size_t>(available_space) > len)
		{
			memcpy(m_cur, buf, len);
			m_cur += len;
			return len;
		} else {
			memcpy(m_cur, buf, available_space);
			m_cur += available_space;
			return available_space;
		}
	}

	//! Retrieve the internal data
	const char* data() const { return m_data; }

	//! Retrieve the length of usable data m_data
	int length() const { return static_cast<int>(m_cur - m_data); }

	//! Retrieve the current position of m_data
	char* current() { return m_cur; }

	//! Retrieve the available space of the buffer
	int avail() const { return static_cast<int>(end() - m_cur); }

	//! Increase the current position of the buffer
	void add(size_t len) { m_cur += len; }

	//! Reset the current position
	void reset() { m_cur = m_data; }

	//! Set all data to zero
	void bzero() { memZero(m_data, sizeof m_data); }

	//! Used for used by GDB
	const char* debugString();

	void setCookie(void (*cookie)()) { cookie_ = cookie; }

	// for used by unit test
	std::string toString() const { return std::string(m_data, length()); }


private:
	const char* end() const { return m_data + sizeof m_data; }
	// Must be outline function for cookies.
	static void cookieStart();
	static void cookieEnd();

	void (*cookie_)();
	char m_data[SIZE];  //!< Buffer data
	char* m_cur;        //!< Current position of buffer data
};

} // namespace detail

class BASIC_SERVICES_NO_EXPORT LogStream : noncopyable
{
	typedef LogStream self;
public:
	typedef detail::FixedBuffer<detail::kSmallBuffer> Buffer;

	self& operator<<(bool v)
	{
		buffer_.append(v ? "1" : "0", 1);
		return *this;
	}

	self& operator<<(short);
	self& operator<<(unsigned short);
	self& operator<<(int);
	self& operator<<(unsigned int);
	self& operator<<(long);
	self& operator<<(unsigned long);
	self& operator<<(long long);
	self& operator<<(unsigned long long);

	self& operator<<(const void*);

	self& operator<<(float v)
	{
		*this << static_cast<double>(v);
		return *this;
	}
	self& operator<<(double);
	// self& operator<<(long double);

	self& operator<<(char v)
	{
		buffer_.append(&v, 1);
		return *this;
	}

	// self& operator<<(signed char);
	// self& operator<<(unsigned char);

	self& operator<<(const char* str)
	{
		if (str)
		{
			buffer_.append(str, strlen(str));
		}
		else
		{
			buffer_.append("(null)", 6);
		}
		return *this;
	}

	self& operator<<(const unsigned char* str)
	{
		return operator<<(reinterpret_cast<const char*>(str));
	}

	self& operator<<(const std::string& v)
	{
		buffer_.append(v.c_str(), v.size());
		return *this;
	}

	void append(const char* data, int len) { buffer_.append(data, len); }
	const Buffer& buffer() const { return buffer_; }
	void resetBuffer() { buffer_.reset(); }

private:
	void staticCheck();

	template<typename T>
	void formatInteger(T);

	Buffer buffer_;

	static const int kMaxNumericSize = 32;
};

class Fmt // : noncopyable
{
public:
	template<typename T>
	Fmt(const char* fmt, T val);

	const char* data() const { return buf_; }
	int length() const { return length_; }

private:
	char buf_[32];
	int length_;
};

inline LogStream& operator<<(LogStream& s, const Fmt& fmt)
{
	s.append(fmt.data(), fmt.length());
	return s;
}

} // namespace basic

#endif //BASIC_SERVICES_LOG_STREAM_H
