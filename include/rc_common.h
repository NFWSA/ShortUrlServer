
#ifndef SN_SHORT_URL_SERVER_RC_COMMON_H
#define SN_SHORT_URL_SERVER_RC_COMMON_H

#include <string>
#include <map>

namespace sn {

enum ServerErrorCode {
    ALL_OK                      = 0,        // 一切正常

    INTERNAL_UNKNOWN_ERROR      = 501,      // 未知错误
    INTERNAL_REQUEST_ERROR,                 // 网络问题，请求错误

    REQ_JSON_ERROR              = 1001,     // 请求 json 格式错误
    REQ_PARAMS_ERROR,                       // 请求 json 中参数错误
    REQ_INVALID_HASH,                       // 无效的 hash
    REQ_INVALID_URL,                        // 无效的 url
};

extern bool IsValidCode(int rc);
extern const std::map<int, std::string>& GetServerCodeStringMap(bool need_cn = false);
extern std::string ServerCodeToString(int rc, bool need_cn = false);

} /* namespace sn */

#endif // SN_SHORT_URL_SERVER_RC_COMMON_H
