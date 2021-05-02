//
// Created by liu on 02.05.2021.
//

#ifndef BASIC_SERVICES_STATIC_ONLY_H
#define BASIC_SERVICES_STATIC_ONLY_H

namespace basic {

class BASIC_SERVICES_NO_EXPORT StaticOnly
{
private:
	// prevent constructor
	StaticOnly() = delete;

	// prevent copy constructor
	StaticOnly(const StaticOnly&) = delete;

	// prevent copy assignment constructor
	StaticOnly& operator=(const StaticOnly&) = delete;
};

} // namespace basic

#endif //BASIC_SERVICES_STATIC_ONLY_H
