#ifndef SN_SHORT_URL_SERVER_TIME_UTIL_H
#define SN_SHORT_URL_SERVER_TIME_UTIL_H

#include <chrono>
#include <string>

namespace sn {
namespace TimeUtil {

struct Timestamp {
    Timestamp();

    void Reset();
    std::string DurationStringShort() const;
    std::string DurationString() const;

    double Seconds() const;

    std::int64_t MillisecondsCount() const;
    std::int64_t MicrosecondsCount() const;
    std::int64_t NanosecondsCount() const;
    std::int64_t SecondsCount() const;
    std::int64_t MinutesCount() const;
    std::int64_t HoursCount() const;

private:
    std::chrono::time_point<std::chrono::high_resolution_clock> begin_tm_;
};

} /* namespace TimeUtil */
} /* namespace sn */

#endif // SN_SHORT_URL_SERVER_TIME_UTIL_H
