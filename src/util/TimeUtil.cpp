#include "util/TimeUtil.h"
#include "util/StringUtil.h"

using namespace sn::TimeUtil;

Timestamp::Timestamp(): begin_tm_(std::chrono::high_resolution_clock::now()) {
}

void Timestamp::Reset() {
    begin_tm_ = std::chrono::high_resolution_clock::now();
}

std::string Timestamp::DurationStringShort() const {
    std::int64_t e = MillisecondsCount();
    std::int64_t S = e / 1000;
    e = e % 1000;
    std::int64_t M = S / 60;
    S = S % 60;
    std::int64_t H = M / 60;
    M = M % 60;
    return StringUtil::Format("%:%:%.%", { std::to_string(H), std::to_string(M), std::to_string(S), std::to_string(e) });
}

std::string Timestamp::DurationString() const {
    std::int64_t e = MillisecondsCount();
    std::int64_t S = e / 1000;
    e = e % 1000;
    std::int64_t M = S / 60;
    S = S % 60;
    std::int64_t H = M / 60;
    M = M % 60;
    return StringUtil::Format("% h % min % sec % mill", { std::to_string(H), std::to_string(M), std::to_string(S), std::to_string(e) });
}

std::int64_t Timestamp::MillisecondsCount() const {
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - begin_tm_).count();
}

double Timestamp::Seconds() const {
    return std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::high_resolution_clock::now() - begin_tm_).count();
}

std::int64_t Timestamp::MicrosecondsCount() const {
    return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - begin_tm_).count();
}

std::int64_t Timestamp::NanosecondsCount() const {
    return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - begin_tm_).count();
}

std::int64_t Timestamp::SecondsCount() const {
    return std::chrono::duration_cast<std::chrono::seconds>(std::chrono::high_resolution_clock::now() - begin_tm_).count();
}

std::int64_t Timestamp::MinutesCount() const {
    return std::chrono::duration_cast<std::chrono::minutes>(std::chrono::high_resolution_clock::now() - begin_tm_).count();
}

std::int64_t Timestamp::HoursCount() const {
    return std::chrono::duration_cast<std::chrono::hours>(std::chrono::high_resolution_clock::now() - begin_tm_).count();
}
