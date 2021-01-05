//
// Created by liu on 05.01.2021.
//

#ifndef BASIC_SERVICES_NONCOPYABLE_H
#define BASIC_SERVICES_NONCOPYABLE_H

#include "basic-services_export.h"

namespace basic {

class BASIC_SERVICES_NO_EXPORT noncopyable {
public:
	noncopyable(const noncopyable&) = delete;
	void operator=(const noncopyable&) = delete;

protected:
	noncopyable() = default;
	~noncopyable() = default;
};

} // namespace basic

#endif //BASIC_SERVICES_NONCOPYABLE_H
