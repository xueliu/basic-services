//
// Copywrite Xue Liu <liuxuenetmail@gmail.com>
//

#ifndef BASIC_SERVICES_VERSION_H
#define BASIC_SERVICES_VERSION_H

#include <stdint.h>
#include <stdbool.h>

namespace basic {

//! Version definition
struct Version {
	uint8_t  const Major;  //!< major version number
	uint8_t  const Minor;  //!< minor version number
	uint16_t const Build;  //!< build version number

	Version(uint8_t major, uint8_t minor = 0, uint16_t build = 0) noexcept
		: Major(major)
		, Minor(minor)
		, Build(build)
	{ }

	virtual ~Version() = default;

	//! isSupported - Validate version
	//! \param version - Version to check
	//! \return true - version is supported
	//! \return false - version is not supported
	virtual bool isSupported(Version const& version) const noexcept {
		return (Major == version.Major)
			&& ( (Minor > version.Minor)
				|| ( (Minor == version.Minor)
					&& (Build >= version.Build)
					)
				);
	}

};

} // namespace basic

#endif //BASIC_SERVICES_VERSION_H
