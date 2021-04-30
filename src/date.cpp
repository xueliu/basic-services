//
// Created by liu on 05.01.2021.
//

#include "date.h"

namespace basic {

namespace {
constexpr static uint32_t DaysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
}

class Date::Impl {
private:
	uint16_t m_year;
	uint8_t m_month;
	uint8_t m_day;
	bool m_isValid;
public:
	Impl();

	Impl(uint16_t year, uint8_t month, uint8_t day);

	bool correctDate(int32_t &r_year, int32_t &r_month, int32_t &r_day) const;

	uint8_t daysInMonth(uint32_t month, uint32_t year);

	bool isLeapYear(uint32_t year) const;

	Date minusMonths(int months) const;

	Date minusDays(int32_t days) const;
};

Date::Impl::Impl()
		: m_year(0), m_month(0), m_day(0), m_isValid(false) {

}

Date::Impl::Impl(uint16_t year, uint8_t month, uint8_t day)
		: m_year(0), m_month(0), m_day(0), m_isValid(false) {
//	if (year >=1 && )
}

} // namespace basic