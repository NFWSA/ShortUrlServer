#include "util/HttpUtil.h"
#include "util/LoggerUtil.h"

#include "Poco/StreamCopier.h"
#include "Poco/URI.h"
#include "Poco/Net/HTTPClientSession.h"
#include "Poco/Net/HTTPRequest.h"
#include "Poco/Net/HTTPResponse.h"
#include <zlib.h>

namespace sn {
namespace HttpUtil {

bool SendRequest(const std::string &uri, std::string *rsp_str,
        const std::map<std::string, std::string> &headers,
        const std::string &body, const std::string &method) {
    Poco::URI uri_obj(uri);
    Poco::Net::HTTPClientSession session(uri_obj.getHost(), uri_obj.getPort());
    // session.setKeepAlive(false);

    bool must_post = method == "POST" || !body.empty();
    Poco::Net::HTTPRequest req(must_post ? Poco::Net::HTTPRequest::HTTP_POST : Poco::Net::HTTPRequest::HTTP_GET,
        uri_obj.getPathAndQuery(), Poco::Net::HTTPRequest::HTTP_1_1);
    if (must_post)
        req.setContentType("application/json");
    for (auto &kv_pair : headers)
        req.add(kv_pair.first, kv_pair.second);
    // req.add("Accept", "application/json");
    req.setContentLength(body.size());

    try {
        session.sendRequest(req) << body;

        Poco::Net::HTTPResponse rsp;
        auto &is = session.receiveResponse(rsp);
        // LOG(INFO) << "result:" << rsp.getStatus() << " reason:" << rsp.getReason() << std::endl;
        if (rsp_str != nullptr)
            Poco::StreamCopier::copyToString(is, *rsp_str);

        // auto recv_body = rsp.get("body");
        // LOG(INFO) << "body : \n" << recv_body << std::endl;
        return rsp.getStatus() == Poco::Net::HTTPResponse::HTTPStatus::HTTP_OK;
    }
    catch (std::exception &e) {
        LOGUTIL_LOG_E() << e.what();
        return false;
    }
}

bool SendRequestRetry(const std::string &uri, const int max_retry_times,
        std::string *rsp_str, const std::map<std::string, std::string> &headers,
        const std::string &body, const std::string &method) {
    int retry_times = 0;
    do {
        if (SendRequest(uri, rsp_str, headers, body, method))
            return true;
        ++retry_times;
    } while (retry_times >= max_retry_times);
    return false;
}

std::string Decompress(const std::string &zip_data) {
    std::string unp_data;
    const int buf_len = 1024 * 1024;
    unsigned char buf[buf_len];

    z_stream strm = {0};
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    strm.avail_in = 0;
    strm.next_in  = Z_NULL;
    int ret = inflateInit(&strm);
    if (ret != Z_OK) {
        // TODO
        return "";
    }

    long offset = 0;
    do {
        strm.avail_in = zip_data.size() - offset;
        strm.next_in = ((Bytef*)zip_data.data()) + offset;
        if (strm.avail_in == 0)
            break;
        /* run inflate() on input until output buffer not full */
        do {
            strm.avail_out = buf_len;
            strm.next_out = buf;
            ret = inflate(&strm, Z_NO_FLUSH);
            // assert(ret != Z_STREAM_ERROR);  /* state not clobbered */
            switch (ret) {
            case Z_NEED_DICT:
                // ret = Z_DATA_ERROR;     /* and fall through */
            case Z_STREAM_ERROR:
            case Z_DATA_ERROR:
            case Z_MEM_ERROR:
                inflateEnd(&strm);
                return "";
            }
            unp_data += std::string(reinterpret_cast<char*>(buf), buf_len - strm.avail_out);
        } while (strm.avail_out == 0);
        /* done when inflate() says it's done */
    } while (ret != Z_STREAM_END);
    /* clean up and return */
    inflateEnd(&strm);
    return unp_data;



    // // strm.next_out = reinterpret_cast<Bytef*>(buf);
    // if (inflateInit(&strm) != Z_OK) {
    //     // TODO
    //     return "";
    // }
    // strm.avail_in = zip_data.size();;
    // strm.next_in = (Bytef*)zip_data.data();
    // // strm.total_in = strm.avail_in = zip_data.size();
    // // strm.total_out = strm.avail_out = buf_len;
    // // strm.next_in = (Bytef*)zip_data.data();
    // int ret = 0;
    // do {
    //     strm.avail_out = buf_len;
    //     strm.next_out = reinterpret_cast<Bytef*>(buf);
    //     ret = inflate(&strm, Z_NO_FLUSH);
    //     if (err_codes.count(ret) > 0 || strm.avail_out <= 0) {
    //         // TODO
    //         inflateEnd(&strm);
    //         return "";
    //     }
    //     unp_data += std::string(reinterpret_cast<char*>(buf), buf_len - strm.avail_out);
    // // } while (ret != Z_STREAM_END);
    // } while (strm.avail_out == 0);
    // inflateEnd(&strm);
    // return unp_data;
}

std::string UncompressGzip(const std::string &ctx) {
    auto &zip_data = ctx;
    std::string unp_data;
    const int buf_len = 1024 * 1024;
    unsigned char buf[buf_len];

    z_stream strm = {0};
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    // strm.avail_in = 0;
    // strm.next_in  = Z_NULL;
    strm.total_in = zip_data.size();
    strm.avail_in = zip_data.size();
    strm.next_in = (Bytef*)zip_data.data();
    // int ret = inflateInit(&strm);
    int ret = inflateInit2(&strm, MAX_WBITS + 16);
    if (ret != Z_OK) {
        // TODO
        return "";
    }

    // long offset = 0;
    do {
        if (strm.avail_in == 0)
            break;
        /* run inflate() on input until output buffer not full */
        do {
            strm.total_out = buf_len;
            strm.avail_out = buf_len;
            strm.next_out = buf;
            ret = inflate(&strm, Z_NO_FLUSH);
            // assert(ret != Z_STREAM_ERROR);  /* state not clobbered */
            switch (ret) {
            case Z_NEED_DICT:
                // ret = Z_DATA_ERROR;     /* and fall through */
            case Z_STREAM_ERROR:
            case Z_DATA_ERROR:
            case Z_MEM_ERROR:
                inflateEnd(&strm);
                return "";
            }
            auto have = buf_len - strm.avail_out;
            unp_data += std::string(reinterpret_cast<char*>(buf), have);
            // offset += have;
        } while (strm.avail_out == 0);
        /* done when inflate() says it's done */
    } while (ret != Z_STREAM_END);
    /* clean up and return */
    inflateEnd(&strm);
    return unp_data;
    // z_stream strm = {0};
    // strm.zalloc = Z_NULL;
    // strm.zfree = Z_NULL;
    // strm.opaque = Z_NULL;
    // std::set<int> err_codes{ Z_STREAM_ERROR, Z_NEED_DICT, Z_MEM_ERROR, Z_DATA_ERROR };

    // std::string unp_data;
    // const int buf_len = 1024 * 1024;
    // unsigned char buf[buf_len];
    // strm.total_in = strm.avail_in = zip_data.size();
    // strm.total_out = strm.avail_out = buf_len;
    // strm.next_in = (Bytef*)zip_data.data();
    // strm.next_out = reinterpret_cast<Bytef*>(buf);
    // if (inflateInit2(&strm, (MAX_WBITS + 16)) != Z_OK) {
    //     // TODO
    //     return "";
    // }
    // int ret = 0;
    // do {
    //     ret = inflate(&strm, Z_NO_FLUSH);
    //     if (err_codes.count(ret) > 0 || strm.avail_out <= 0) {
    //         // TODO
    //         inflateEnd(&strm);
    //         return "";
    //     }
    //     unp_data += std::string(reinterpret_cast<char*>(buf), strm.avail_out);
    // } while (ret != Z_STREAM_END);
    // inflateEnd(&strm);
    // return unp_data;
}

} /* namespace HttpUtil */
} /* namespace sn */
