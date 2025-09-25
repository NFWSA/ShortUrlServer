#include "util/LoggerUtil.h"
#include <ostream>

// #define USE_GLOG_RATHER_THAN_STL

#ifdef USE_GLOG_RATHER_THAN_STL
    #include "util/FileUtil.h"
    #define GLOG_USE_GLOG_EXPORT
    // #define GLOG_USE_GLOG_NO_EXPORT
    #include <glog/logging.h>
    #include <glog/log_severity.h>
#else
    #include <ctime>
    #include <thread>
    #include <chrono>
    #include <iomanip>
#endif // USE_GLOG_RATHER_THAN_STL

static std::string g_log_name;
static bool g_inited = false;
static bool g_log_to_stderr = true;

class NullStream : public std::ostream {
    // streambuffer doing nothing
    struct NullBuffer : public std::streambuf {
        int overflow(int c) noexcept override { return c; }
    } null_buffer;

public:
    NullStream() : std::ostream(&null_buffer) {}
    NullStream(const NullStream&) = delete;
    NullStream& operator=(const NullStream&) = delete;
} g_null_stream;

inline std::string LevelToString(sn::LoggerUtil::LogLevel lv) {
    switch (lv) {
        case sn::LoggerUtil::LogLevel::LOG_LV_DEBUG:
            return "DEBUG";
        case sn::LoggerUtil::LogLevel::LOG_LV_INFO:
            return "INFO ";
        case sn::LoggerUtil::LogLevel::LOG_LV_WARN:
            return "WARN ";
        case sn::LoggerUtil::LogLevel::LOG_LV_ERROR:
            return "ERROR";
        case sn::LoggerUtil::LogLevel::LOG_LV_FATAL:
            return "FATAL";
    }
    return "UNKNO";
}

namespace sn {
namespace LoggerUtil {

#ifdef USE_GLOG_RATHER_THAN_STL
    inline google::LogSeverity ToGoogleLevel(sn::LoggerUtil::LogLevel lv) {
        switch (lv) {
            case sn::LoggerUtil::LogLevel::LOG_LV_DEBUG:
                return google::GLOG_INFO;
            case sn::LoggerUtil::LogLevel::LOG_LV_INFO:
                return google::GLOG_INFO;
            case sn::LoggerUtil::LogLevel::LOG_LV_WARN:
                return google::GLOG_WARNING;
            case sn::LoggerUtil::LogLevel::LOG_LV_ERROR:
                return google::GLOG_ERROR;
            case sn::LoggerUtil::LogLevel::LOG_LV_FATAL:
                return google::GLOG_FATAL;
        }
        return google::GLOG_INFO;
    }
    SingleLogMessage::SingleLogMessage(LogLevel lv, const std::string &file, int line) :
        impl_(new google::LogMessage(file.c_str(), line, ToGoogleLevel(lv)))
    {
    }
    SingleLogMessage::~SingleLogMessage() {
        delete static_cast<google::LogMessage*>(impl_);
    }
    std::ostream& SingleLogMessage::stream() {
        return static_cast<google::LogMessage*>(impl_)->stream();
    }
#else
    SingleLogMessage::SingleLogMessage(LogLevel lv, const std::string &file, int line): impl_(nullptr) {
        if (!g_log_to_stderr)
            return;
        using namespace std::chrono_literals;
        auto end_pos = file.rfind('/');
        auto sub_file = file.substr(end_pos == file.npos ? 0 : end_pos + 1);
        auto now = std::chrono::system_clock::now();
        auto t_c = std::chrono::system_clock::to_time_t(now - 24h);
        auto duration_since_epoch = now.time_since_epoch();
        auto micros = std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch()).count() % 1000000;
        std::cerr
            << std::put_time(std::localtime(&t_c), "%F %T") << "."
                << std::setw(6) << std::setfill('0') << micros << std::setw(0) << std::setfill(' ')
            << " " << std::this_thread::get_id()
            << " [" << LevelToString(lv) << "] "
            << "[" << sub_file << ":" << line << "] > ";
    }
    SingleLogMessage::~SingleLogMessage() {
        if (!g_log_to_stderr)
            return;
        std::cerr << "\n";
        std::flush(std::cerr);
    }
    std::ostream& SingleLogMessage::stream() {
        if (!g_log_to_stderr)
            return g_null_stream;
        return std::cerr;
    }
#endif // USE_GLOG_RATHER_THAN_STL

void InitLogRotation(const std::string &name, const std::string &log_path, bool log_to_stdout) {
    if (g_inited)
        return;
    g_log_to_stderr = log_to_stdout;
    if (log_path.empty())
        return;
    g_inited = true;
    g_log_name = name;
#ifdef USE_GLOG_RATHER_THAN_STL
    google::InitGoogleLogging(name.c_str());
    if (log_to_stdout)
        google::LogToStderr();
    sn::FileUtil::CreateFolder(log_path);
    google::SetLogDestination(google::GLOG_INFO, log_path.c_str());
    google::SetLogFilenameExtension(".log");
#else
    std::ios::sync_with_stdio(false);
#endif // USE_GLOG_RATHER_THAN_STL
}

SingleLogMessage Info(const std::string &file, int line) {
    return SingleLogMessage(LogLevel::LOG_LV_INFO, file, line);
}
SingleLogMessage Debug(const std::string &file, int line) {
    return SingleLogMessage(LogLevel::LOG_LV_DEBUG, file, line);
}
SingleLogMessage Warn(const std::string &file, int line) {
    return SingleLogMessage(LogLevel::LOG_LV_WARN, file, line);
}
SingleLogMessage Error(const std::string &file, int line) {
    return SingleLogMessage(LogLevel::LOG_LV_ERROR, file, line);
}
SingleLogMessage Fatal(const std::string &file, int line) {
    return SingleLogMessage(LogLevel::LOG_LV_FATAL, file, line);
}

} /* namespace LoggerUtil */
} /* namespace sn */
