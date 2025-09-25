#include "handler.h"

#include "Poco/Net/HTTPServerResponse.h"
#include "util/JsonUtil.h"
#include "Poco/JSON/Parser.h"

#include "util/LoggerUtil.h"
#include "util/StringUtil.h"
#include "rc_common.h"
#include "task.h"

#define LOG_REQ_INFO() LOGUTIL_LOG_D() << "proc req method:" << req.getMethod() << " uri:" << req.getURI() << "\n  - client:" \
                                << req.clientAddress().toString() << " server:" << req.serverAddress().toString();

using std::to_string;
using std::string;


static inline void QuickResponse(Poco::Net::HTTPServerResponse &res, int rc, const std::string &extra_data = "", bool log = true) {
    auto rc_str = to_string(rc);
    auto rc_msg = sn::ServerCodeToString(rc);
    auto extra_head = extra_data.empty() ? "" : R"(,"data":)";
    // if (log)
    //     LOGUTIL_LOG_I() << " > rsp rc:" << rc << " msg:" << rc_msg;
    res.setContentType("application/json");
    res.setStatus(Poco::Net::HTTPResponse::HTTPStatus::HTTP_OK);
    res.send() << sn::StringUtil::Format(R"({"code":%,"msg":"%"%%})", { rc_str, rc_msg, extra_head, extra_data });
}

namespace sn {

DEFINE_REQUEST_HANDLER(HdlShortUrlAdd) {
    // LOG_REQ_INFO();
    Poco::JSON::Parser parser;
    auto json = JsonUtil::LoadJsonValue("", parser.parse(req.stream()));
    int rc    = ServerErrorCode::ALL_OK;
    auto url = json->GetString("url");
    auto hash = inst_->mgr_->AddUrl(url);
    if (hash.empty()) {
        QuickResponse(res, ServerErrorCode::REQ_JSON_ERROR);
        return;
    }
    QuickResponse(res, rc, JsonUtil::ToJsonString(hash));
}
DEFINE_REQUEST_HANDLER(HdlShortUrlDel) {
    // LOG_REQ_INFO();
    Poco::JSON::Parser parser;
    auto json = JsonUtil::LoadJsonValue("", parser.parse(req.stream()));
    int rc = ServerErrorCode::ALL_OK;
    auto hash = json->GetString("hash");
    auto url = json->GetString("url");
    if (hash.empty() && url.empty()) {
        QuickResponse(res, ServerErrorCode::REQ_JSON_ERROR);
        return;
    }
    auto succ = hash.empty() ? inst_->mgr_->DelUrl(url) : inst_->mgr_->DelHash(hash);
    QuickResponse(res, succ ? ServerErrorCode::ALL_OK : ServerErrorCode::REQ_JSON_ERROR);
}
DEFINE_REQUEST_HANDLER(HdlShortUrlGet) {
    // LOG_REQ_INFO();
    Poco::JSON::Parser parser;
    auto json = JsonUtil::LoadJsonValue("", parser.parse(req.stream()));
    int rc = ServerErrorCode::ALL_OK;
    auto hash = json->GetString("hash");
    if (hash.empty()) {
        QuickResponse(res, ServerErrorCode::REQ_JSON_ERROR);
        return;
    }
    auto url = inst_->mgr_->GetUrl(hash);
    QuickResponse(res,
        url.empty() ? ServerErrorCode::REQ_JSON_ERROR : ServerErrorCode::ALL_OK,
        url.empty() ? "" : JsonUtil::ToJsonString(url));
}
DEFINE_REQUEST_HANDLER(HdlShortUrlJump) {
    // LOG_REQ_INFO();
    int rc = ServerErrorCode::ALL_OK;
    if (ctx_.keys_.empty() || ctx_.keys_.front().empty()) {
        res.setStatus(Poco::Net::HTTPResponse::HTTPStatus::HTTP_NOT_FOUND);
        res.send() << R"(<html><body>404 Not Found</body></html>)";
        return;
    }
    auto &hash = ctx_.keys_.front();
    auto url = inst_->mgr_->GetUrl(hash);
    if (url.empty()) {
        res.setStatus(Poco::Net::HTTPResponse::HTTPStatus::HTTP_NOT_FOUND);
        res.send() << R"(<html><body>404 Not Found</body></html>)";
        return;
    }
    res.redirect(url);
}
DEFINE_REQUEST_HANDLER(HdlShortUrlWebpage) {
    res.setContentType("text/html");
    res.setStatus(Poco::Net::HTTPResponse::HTTPStatus::HTTP_OK);
    res.send() << inst_->webpage_html_;
}
DEFINE_REQUEST_HANDLER(HdlShortUrlCfgGet) {
    auto js = StringUtil::Format(R"(
// 服务器基础URL
const API_BASE_URL = '%';
const SHOW_API_BASE_URL = '%:%';
)", { "", (inst_->bind_ip_ == "::1" ? "localhost" : inst_->bind_ip_), to_string(inst_->port_) });
    res.setContentType("text/javascript");
    res.setStatus(Poco::Net::HTTPResponse::HTTPStatus::HTTP_OK);
    res.send() << js;
}
DEFINE_REQUEST_HANDLER(HdlShortUrlStatic) {
    res.setContentType("text/html");
    res.setStatus(Poco::Net::HTTPResponse::HTTPStatus::HTTP_OK);
    res.send() << inst_->webpage_html_;
}

DEFINE_REQUEST_HANDLER(HdlShortUrlInfo) {
    QuickResponse(res, ServerErrorCode::ALL_OK, "", false);
}

} /* namespace sn */
