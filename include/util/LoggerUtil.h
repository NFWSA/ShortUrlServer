#ifndef SN_SHORT_URL_SERVER_LOGGER_UTIL_H
#define SN_SHORT_URL_SERVER_LOGGER_UTIL_H

#include <iostream>

namespace sn {
namespace LoggerUtil {

enum LogLevel {
    LOG_LV_DEBUG = 0,
    LOG_LV_INFO,
    LOG_LV_WARN,
    LOG_LV_ERROR,
    LOG_LV_FATAL,
};

struct SingleLogMessage {
    SingleLogMessage(LogLevel lv, const std::string &file, int line);
    ~SingleLogMessage();
    std::ostream& stream();
    void *impl_;
};

extern void InitLogRotation(const std::string &name, const std::string &log_path, bool log_to_stdout = true);

extern SingleLogMessage Info(const std::string &file, int line);
extern SingleLogMessage Debug(const std::string &file, int line);
extern SingleLogMessage Warn(const std::string &file, int line);
extern SingleLogMessage Error(const std::string &file, int line);
extern SingleLogMessage Fatal(const std::string &file, int line);

} /* namespace LoggerUtil */
} /* namespace sn */

#define LOGUTIL_LOG_I() sn::LoggerUtil::Info(__FILE__, __LINE__).stream()
#define LOGUTIL_LOG_D() sn::LoggerUtil::Debug(__FILE__, __LINE__).stream()
#define LOGUTIL_LOG_W() sn::LoggerUtil::Warn(__FILE__, __LINE__).stream()
#define LOGUTIL_LOG_E() sn::LoggerUtil::Error(__FILE__, __LINE__).stream()
#define LOGUTIL_LOG_F() sn::LoggerUtil::Fatal(__FILE__, __LINE__).stream()

#endif // SN_SHORT_URL_SERVER_LOGGER_UTIL_H
