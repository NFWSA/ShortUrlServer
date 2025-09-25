#include "rc_common.h"

#include <map>
#include <string>

using std::string;
using std::map;

namespace sn {

extern bool IsValidCode(int rc) {
    auto &descs = GetServerCodeStringMap();
    return descs.count(rc) > 0;
}

extern const std::map<int, std::string>& GetServerCodeStringMap(bool need_cn) {
    static std::map<int, std::string> en_descs{
        { ALL_OK,                       "ok" },

        { INTERNAL_UNKNOWN_ERROR,       "unknown error" },
        { INTERNAL_REQUEST_ERROR,       "network error" },

        { REQ_JSON_ERROR,               "json is wrong" },
        { REQ_PARAMS_ERROR,             "invalid json params" },
        { REQ_INVALID_HASH,             "invalid hash" },
        { REQ_INVALID_URL,              "invalid url" },
    }, cn_descs{
        { ALL_OK,                       "一切正常" },

        { INTERNAL_UNKNOWN_ERROR,       "未知错误" },
        { INTERNAL_REQUEST_ERROR,       "网络问题，请求错误" },

        { REQ_JSON_ERROR,               "请求 json 格式错误" },
        { REQ_PARAMS_ERROR,             "请求 json 中参数错误" },
        { REQ_INVALID_HASH,             "无效的 hash" },
        { REQ_INVALID_URL,              "无效的 url" },
    };
    return need_cn ? cn_descs : en_descs;
}

std::string ServerCodeToString(int rc, bool need_cn) {
    auto &descs = GetServerCodeStringMap(need_cn);
    // if (rc == 0)
    //     return "ok";
    auto desc_it = descs.find(rc);
    if (desc_it == descs.end())
        return descs.at(INTERNAL_UNKNOWN_ERROR);
    return desc_it->second;
}

} /* namespace sn */
