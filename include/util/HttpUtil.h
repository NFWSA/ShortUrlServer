#ifndef SN_SHORT_URL_SERVER_HTTP_UTIL_H
#define SN_SHORT_URL_SERVER_HTTP_UTIL_H

#include <string>
#include <map>

namespace sn {
namespace HttpUtil {

extern bool SendRequest(const std::string &uri, std::string *rsp_str = nullptr,
        const std::map<std::string, std::string> &headers = {},
        const std::string &body = "", const std::string &method = "GET");
extern bool SendRequestRetry(const std::string &uri, const int max_retry_times = 3,
        std::string *rsp_str = nullptr, const std::map<std::string, std::string> &headers = {},
        const std::string &body = "", const std::string &method = "GET");

extern std::string UncompressGzip(const std::string &ctx);

} /* namespace HttpUtil */
} /* namespace sn */

#endif // SN_SHORT_URL_SERVER_HTTP_UTIL_H
