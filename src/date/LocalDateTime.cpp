#include "date/LocalDateTime.hpp"

#include <iomanip>

#include "util/ScopeGuard.hpp"

using namespace std;


namespace date {

LocalDateTime::LocalDateTime(uint64_t value): _value(value)
{
}


LocalDateTime::LocalDateTime(LocalDate const& date, Time const& time):
    _value(static_cast<uint32_t>(date) * 1000000UL + static_cast<uint32_t>(time))
{
}


LocalDateTime::LocalDateTime(uint32_t year, uint32_t month, uint32_t day, uint32_t hour, uint32_t minute, uint32_t second):
    _value(year * 10000000000UL + month * 100000000UL + day * 1000000UL + hour * 10000UL + minute * 100UL + second)
{
}


LocalDateTime::LocalDateTime(uint32_t year, Month month, uint32_t day, uint32_t hour, uint32_t minute, uint32_t second):
    LocalDateTime(year, static_cast<uint32_t>(month), day, hour, minute, second)
{
}


ostream& operator<<(ostream& out, const LocalDateTime& obj)
{
    out << setw(4) << setfill('0') << obj.year()
        << '-' << setw(2) << setfill('0') << static_cast<uint32_t>(obj.month())
        << '-' << setw(2) << setfill('0') << obj.day()
        << ' '
        << setw(2) << setfill('0') << obj.hour()
        << ':' << setw(2) << setfill('0') << obj.minute()
        << ':' << setw(2) << setfill('0') << obj.second();
    return out;
}

}
