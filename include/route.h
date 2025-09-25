#ifndef SN_NS_SHORT_URL_SERVER_ROUTE_H
#define SN_NS_SHORT_URL_SERVER_ROUTE_H

#include "Poco/Net/HTTPServer.h"
#include "util/StringUtil.h"
#include <memory>

#define DECLARE_REQUEST_HANDLER(handler_class_name, INST_T) \
    struct handler_class_name : public sn::BaseHandlerTemplate<INST_T> { \
        handler_class_name(const INST_T *inst, sn::RequestContext &&ctx) : sn::BaseHandlerTemplate<INST_T>(inst, std::move(ctx)) {} \
        virtual void handleRequest(Poco::Net::HTTPServerRequest &req, Poco::Net::HTTPServerResponse &res); \
    };

#define DEFINE_REQUEST_HANDLER(handler_class_name) \
    void handler_class_name::handleRequest(Poco::Net::HTTPServerRequest &req, Poco::Net::HTTPServerResponse &res)

#define DECLARE_IMPL_REQUEST_HANDLER(handler_class_name, INST_T) \
    DECLARE_REQUEST_HANDLER(handler_class_name, INST_T) \
    DEFINE_REQUEST_HANDLER(handler_class_name)

#include <string>
#include <map>
#include <functional>
#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"
#include "Poco/Net/HTTPRequestHandler.h"
#include "Poco/Net/HTTPRequestHandlerFactory.h"

namespace sn {

struct BaseServerConfig {
    std::string log_path_;
    int max_num_;
    int port_;

    Poco::Net::HTTPServer *svr_;
};

struct RequestContext {
    std::vector<std::string> keys_;
};

template <typename INST_T>
struct BaseHandlerTemplate : public Poco::Net::HTTPRequestHandler {
    BaseHandlerTemplate(const INST_T *inst, RequestContext &&ctx) : inst_(inst), ctx_(std::move(ctx)) {}
    // BaseHandlerTemplate(const INST_T *inst, const RequestContext &ctx) : inst_(inst), ctx_(ctx) {}
    virtual void handleRequest(Poco::Net::HTTPServerRequest &req, Poco::Net::HTTPServerResponse &res) = 0;
protected:
    const INST_T *inst_;
    RequestContext ctx_;
};

DECLARE_REQUEST_HANDLER(DefaultRequestErrorHandler, BaseServerConfig);
DECLARE_REQUEST_HANDLER(DefaultRequestOptionsHandler, BaseServerConfig);

template <typename INST_T>
struct HandlerFactory : public Poco::Net::HTTPRequestHandlerFactory {
    typedef std::function<Poco::Net::HTTPRequestHandler*(RequestContext&&)> HandlerCreateFunc;
    HandlerFactory(const INST_T *inst) : inst_(inst) {}

private:
    struct NextJump {
        NextJump *next_;
        HandlerCreateFunc func_;
        int size_;
        std::string key_;
    };
    std::vector<std::string> SplitKeyParts(const std::string &path) {
        return StringUtil::SplitStringVec(path.substr(1), "/");
    }
    Poco::Net::HTTPRequestHandler* FindMethodHdl(const std::string &uri, const Poco::Net::HTTPServerRequest &req,
            std::map<std::string, HandlerCreateFunc> &method_hdls, std::map<std::string, std::vector<NextJump*>> &method_jumps) {
        RequestContext ctx;
        auto hdl_it = method_hdls.find(uri);
        if (hdl_it != method_hdls.end())
            return hdl_it->second(std::move(ctx));
        if (method_jumps.empty())
            return nullptr;
        auto keys = SplitKeyParts(uri);
        auto jumps_it = method_jumps.find(keys.front());
        if (jumps_it == method_jumps.end())
            return nullptr;
        for (auto &jump_head : jumps_it->second) {
            if (jump_head->size_ != keys.size() + 1)
                continue;
            auto ptr_jump = jump_head;
            bool match = true;
            for (auto &key : keys) {
                if (ptr_jump->key_.empty()) {
                    ctx.keys_.emplace_back(key);
                    ptr_jump = ptr_jump->next_;
                    continue;
                }
                if (ptr_jump->key_ != key) {
                    match = false;
                    break;
                }
                ptr_jump = ptr_jump->next_;
            }
            if (match)
                return (ptr_jump->func_(std::move(ctx)));
        }
        return nullptr;
    }

public:
    virtual Poco::Net::HTTPRequestHandler* createRequestHandler(const Poco::Net::HTTPServerRequest &req) override {
        auto &method = req.getMethod();
        auto &uri = req.getURI();
        RequestContext ctx;
        // LoggerUtil::LogInfo() << "proc req method:" << method << " uri:" << uri << "\n  - client:" << req.clientAddress().toString() <<
        //         " server:" << req.serverAddress().toString();
        if (method == "OPTIONS") {
            return new DefaultRequestOptionsHandler(inst_, std::move(ctx));
        }
        else if (method == "GET") {
            auto ret = FindMethodHdl(uri, req, get_hdls_, get2jumps_);
            if (ret != nullptr)
                return ret;
        }
        else if (method == "POST") {
            auto ret = FindMethodHdl(uri, req, post_hdls_, post2jumps_);
            if (ret != nullptr)
                return ret;
        }
        auto ret = FindMethodHdl(uri, req, any_hdls_, any2jumps_);
        if (ret != nullptr)
            return ret;
        return new DefaultRequestErrorHandler(inst_, std::move(ctx));
    }

// protected:
    template<typename T> void HandlePath(const std::string &path,
            std::map<std::string, HandlerCreateFunc> &hdls, std::map<std::string, std::vector<NextJump*>> &dynamic_hdls) {
        auto &inst = inst_;
        auto func = [&inst](RequestContext &&ctx) {
            return new T(inst, std::move(ctx));
        };
        if (path.find("*") == path.npos) {
            hdls.insert(make_pair(path, func));
            return;
        }

        std::vector<std::shared_ptr<NextJump>> cached_jumps;
        auto keys = SplitKeyParts(path);
        for (auto &key : keys) {
            auto jump = std::make_shared<NextJump>();
            if (key != "*" && key != "**")
                jump->key_ = key;
            cached_jumps.emplace_back(std::move(jump));
        }
        auto func_jump = std::make_shared<NextJump>();
        func_jump->func_ = func;
        cached_jumps.emplace_back(func_jump);
        NextJump *last = nullptr;
        for (auto it = cached_jumps.rbegin(); it != cached_jumps.rend(); ++it) {
            auto &jump = *it;
            jump->next_ = last;
            jump->size_ = cached_jumps.size();
            last = jump.get();
        }
        dynamic_hdls[last->key_].emplace_back(last);
        cached_jumps_.insert(cached_jumps_.end(), cached_jumps.cbegin(), cached_jumps.cend());
    }
    template<typename T> void HandleGet(const std::string &path) {
        HandlePath<T>(path, get_hdls_, get2jumps_);
    }
    template<typename T> void HandlePost(const std::string &path) {
        HandlePath<T>(path, post_hdls_, post2jumps_);
    }
    template<typename T> void HandleAny(const std::string &path) {
        HandlePath<T>(path, any_hdls_, any2jumps_);
    }


private:
    std::map<std::string, HandlerCreateFunc> get_hdls_;
    std::map<std::string, HandlerCreateFunc> post_hdls_;
    std::map<std::string, HandlerCreateFunc> any_hdls_;

    std::vector<std::shared_ptr<NextJump>> cached_jumps_;
    std::map<std::string, std::vector<NextJump*>> get2jumps_;
    std::map<std::string, std::vector<NextJump*>> post2jumps_;
    std::map<std::string, std::vector<NextJump*>> any2jumps_;
    const INST_T *inst_;
};


} /* namespace sn */

#endif // SN_NS_SHORT_URL_SERVER_ROUTE_H
