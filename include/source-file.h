//
// Created by clock on 26/01/2021.
//

#ifndef BASIC_SERVICES_SOURCE_FILE_H
#define BASIC_SERVICES_SOURCE_FILE_H

#include <filesystem>

#include <string.h>

namespace basic {


//! Class SourceFile
//!
//! \note Copy from muduo
class SourceFile
{
public:
	template<int N>
	SourceFile(const char (&arr)[N])
			: data_(arr),
			  size_(N-1)
	{
		const char* slash = ::strrchr(data_, std::filesystem::path::preferred_separator );
		if (slash)
		{
			data_ = slash + 1;
			size_ -= static_cast<int>(data_ - arr);
		}
	}

	explicit SourceFile(const char* filename)
			: data_(filename)
	{
		const char* slash = ::strrchr(filename, std::filesystem::path::preferred_separator);
		if (slash)
		{
			data_ = slash + 1;
		}
		size_ = static_cast<int>(::strlen(data_));
	}

	const char* data_;
	int size_;
};

} // namespace basic

#endif //BASIC_SERVICES_SOURCE_FILE_H
