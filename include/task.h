#ifndef SN_SHORT_URL_SERVER_TASK_H
#define SN_SHORT_URL_SERVER_TASK_H

#include "route.h"
#include <shared_mutex>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <memory>
#include <atomic>

namespace sn {

struct ShortUrlRecord {
    std::int64_t timestamp_;
    std::string url_;
    std::string hash_;
    ShortUrlRecord(const std::string &url, const std::string &hash, const std::int64_t tm) : url_(url), hash_(hash), timestamp_(tm) {}
};

class ShortUrlMgr {
public:
    ShortUrlMgr();
    ~ShortUrlMgr();

    std::string AddUrl(const std::string &url);
    bool DelUrl(const std::string &url);
    bool DelHash(const std::string &hash);
    ShortUrlRecord GetUrlInfo(const std::string &key, bool is_hash);
    std::string GetUrl(const std::string &hash);
    std::string GetHash(const std::string &url);
    std::string GenerateHash(const std::string &url) const;

    void SetHashWidth(int width) { hash_width_ = width; }
    int GetHashWidth() { return hash_width_; }

    void SaveRecordsSync(const std::string &save_path);
    void SaveRecordsAsync(const std::string &save_path);
    bool IsAsyncSaveing() const { return backuping_; }
    void LoadRecords(const std::string &save_path);
    bool IsModified() const { return modified_; }

private:
    std::atomic<bool> backuping_;
    std::atomic<bool> modified_;
    std::shared_mutex mtx_;
    int hash_width_;
    std::unordered_map<std::string, std::shared_ptr<ShortUrlRecord>> url2recs_;
    std::unordered_map<std::string, std::shared_ptr<ShortUrlRecord>> hash2recs_;

    std::unordered_map<std::string, std::shared_ptr<ShortUrlRecord>> extra_url2recs_;
    std::unordered_map<std::string, std::shared_ptr<ShortUrlRecord>> extra_hash2recs_;
    std::unordered_set<std::string> extra_deleted_hashs_;
};

struct ServerConfig : public BaseServerConfig {
    std::string data_path_;
    std::string webpage_html_;
    std::int64_t save_internal_;
    bool save_async_;
    int hash_width_;

    sn::ShortUrlMgr *mgr_;
};

} /* namespace sn */

#endif // SN_SHORT_URL_SERVER_TASK_H
