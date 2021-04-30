//
// Created by liu on 05.01.2021.
//

#ifndef BASIC_SERVICES_DATE_H
#define BASIC_SERVICES_DATE_H

#include <string>
#include <memory>

namespace basic {

class Date {
public:
	Date();

	Date(uint32_t year, uint32_t month, uint32_t day);

	Date(const Date & date);

	virtual ~Date();

	bool isValid() const;

	bool isZero() const;

	std::string toString() const;

	uint64_t toSeconds() const;

	uint16_t getYear() const;

	uint8_t getMonth() const;

	uint8_t getDay() const;

	bool isLeapYear() const;

	Date plusYears(int32_t) const;

	Date plusMonths(int32_t) const;

	Date plusDays(int32_t) const;

	static Date parse(const std::string&);

	static Date epochSeconds(uint64_t);

	static Date nowUTC();

	Date& operator=(const Date&);

	bool operator==(const Date&) const;

	bool operator!=(const Date&) const;

	bool operator>(const Date&) const;

	bool operator<(const Date&) const;

private:
	class Impl;
	std::unique_ptr<Impl> m_impl;
};

} // namespace basic

#endif //BASIC_SERVICES_DATE_H
