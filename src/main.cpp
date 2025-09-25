#include "rc_common.h"
#include "task.h"
#include "util/ConfigUtil.h"
#include "util/FileUtil.h"
#include "route.h"
#include "handler.h"

#include "Poco/Net/HTTPServerParams.h"
#include "util/LoggerUtil.h"
#include <Poco/Net/HTTPServer.h>
#include <Poco/Net/StreamSocket.h>
#include <iomanip>
#include <chrono>

using sn::ServerConfig;
using sn::HandlerFactory;
using namespace sn;
using namespace std;

struct CustomConnectionFilter : public Poco::Net::TCPServerConnectionFilter {
    virtual bool accept(const Poco::Net::StreamSocket& socket) override {
        LOGUTIL_LOG_I() << "in addr:" << socket.address().toString() << " peer:" << socket.peerAddress();
        return true;
    }
};

int main(int argc, char *argv[])
{
    ShortUrlMgr mgr;
    sn::ServerConfig cfg{
        .data_path_ = "data/",
        .webpage_html_ = FileUtil::LoadFile("webpage.html"),
        .save_internal_ = 60,
        .save_async_ = false,
        .hash_width_ = 6,
        .mgr_ = &mgr,
    };
    {
        cfg.log_path_ = "log/";
        cfg.max_num_ = -1;
        cfg.port_ = 8080;

        auto cfg_map = ConfigUtil::ConfigMap("short-url-server.ini");
        cfg_map.TryReadConfig(cfg.log_path_, "log_path");
        cfg_map.TryReadConfig(cfg.max_num_, "max_num");
        cfg_map.TryReadConfig(cfg.port_, "port");

        cfg_map.TryReadConfig(cfg.data_path_, "data_path");
        // cfg_map.TryReadConfig(cfg.webpage_html_, "webpage_html_filename");
        cfg_map.TryReadConfig(cfg.save_internal_, "save_internal");
        cfg_map.TryReadConfig(cfg.save_async_, "save_async");
        cfg_map.TryReadConfig(cfg.hash_width_, "hash_width");
    }

    LoggerUtil::InitLogRotation(argv[0], cfg.log_path_);
    mgr.LoadRecords(cfg.data_path_);
    mgr.SaveRecordsSync(cfg.data_path_);
    mgr.SetHashWidth(cfg.hash_width_);

    auto hdl_factory = new HandlerFactory<sn::ServerConfig>(&cfg);
    hdl_factory->HandlePost<HdlShortUrlAdd>("/add");
    hdl_factory->HandlePost<HdlShortUrlDel>("/del");
    hdl_factory->HandlePost<HdlShortUrlGet>("/get");
    hdl_factory->HandleGet<HdlShortUrlJump>("/j/*");
    hdl_factory->HandleGet<HdlShortUrlWebpage>("/webpage");
    // hdl_factory->HandleGet<HdlShortUrlWebpage>("/static/**");
    // hdl_factory->HandlePost<DCHdlRunBydTaskSync>("/keys");
    // hdl_factory->HandlePost<DCHdlRunBydTaskSync>("/clear");
    // hdl_factory->HandlePost<DCHdlRunBydTaskSync>("/save");
    hdl_factory->HandleAny<HdlShortUrlInfo>("/info");

    auto server_params = new Poco::Net::HTTPServerParams();
    server_params->setTimeout(Poco::Timespan(120, 0));
    // server_params->setKeepAlive(true);
    // server_params->setKeepAliveTimeout(Poco::Timespan(60, 0));
    // server_params->setServerName("0.0.0.0:" + to_string(cfg.port_));
    Poco::Net::HTTPServer server(hdl_factory, cfg.port_, server_params);
    cfg.svr_ = &server;
    // auto filter = new CustomConnectionFilter();
    // server.setConnectionFilter(filter);
    // LOG_COMPILE_INFO();
    {
        for (int i = 0; i < 2; ++i) {
            LOGUTIL_LOG_I() << " ==== ==== ===> ret code desc <=== ==== ====";
            auto &descs = GetServerCodeStringMap(i != 0);
            for (auto &code_pair : descs) {
                auto &code = code_pair.first;
                auto &desc = code_pair.second;
                LOGUTIL_LOG_I() << std::setfill(' ') << std::setw(6) << code << std::setw(0) << " : " <<
                    std::setw(48) << std::left << desc << std::right << std::setw(0);
            }
        }
    }

    server.start();
    LOGUTIL_LOG_I() << "[SVR] Server started ...";
    LOGUTIL_LOG_I() << "[SVR] server name:" << server_params->getServerName();
    LOGUTIL_LOG_I() << "[SVR] server filter:" << server.getConnectionFilter().get();
    LOGUTIL_LOG_I() << "[SVR] server max concurrent connections:" << server.maxConcurrentConnections();
    LOGUTIL_LOG_I() << "[SVR] server refused connections:" << server.refusedConnections();
    LOGUTIL_LOG_I() << "[SVR] server total connections:" << server.totalConnections();
    LOGUTIL_LOG_I() << "[SVR] server queued connections:" << server.queuedConnections();

    auto last_save_time = std::chrono::high_resolution_clock::now();
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        auto now_time = std::chrono::high_resolution_clock::now();
        auto delta = std::chrono::duration_cast<chrono::seconds>(now_time - last_save_time).count();
        if (delta >= cfg.save_internal_ && mgr.IsModified()) {
            last_save_time = now_time;
            if (cfg.save_async_)
                cfg.mgr_->SaveRecordsAsync(cfg.data_path_);
            else
                cfg.mgr_->SaveRecordsSync(cfg.data_path_);
        }
    }

    server.stop();
    return 0;
}
