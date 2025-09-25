#include "task.h"
#include "util/FileUtil.h"
#include "util/LoggerUtil.h"
#include "util/md5.h"

#include <cstdint>
#include <mutex>
#include <string>
#include <thread>

using namespace sn;

ShortUrlMgr::ShortUrlMgr(): backuping_(false), modified_(false), hash_width_(12) {

}

ShortUrlMgr::~ShortUrlMgr() {

}

std::string ShortUrlMgr::AddUrl(const std::string &url) {
    std::unique_lock<std::shared_mutex> guard(mtx_);
    auto info_it = url2recs_.find(url);
    if (info_it != url2recs_.end()) {
        auto &info = info_it->second;
        if (backuping_ && extra_deleted_hashs_.count(info->hash_) > 0) {
            extra_deleted_hashs_.erase(info->hash_);
            modified_ = true;
        }
        return info->hash_;
    }
    if (backuping_) {
        auto extra_it = extra_url2recs_.find(url);
        if (extra_it != extra_url2recs_.end())
            return extra_it->second->hash_;
    }
    auto hash = GenerateHash(url);
    auto info = std::make_shared<ShortUrlRecord>(url, hash, 0);
    if (backuping_) {
        extra_url2recs_[url] = info;
        extra_hash2recs_[hash] = info;
    }
    else {
        url2recs_[url] = info;
        hash2recs_[hash] = info;
    }
    modified_ = true;
    LOGUTIL_LOG_I() << "add " << hash << " = " << url;
    return hash;
}
bool ShortUrlMgr::DelUrl(const std::string &url) {
    std::unique_lock<std::shared_mutex> guard(mtx_);
    auto info_it = url2recs_.find(url);
    if (info_it != url2recs_.end()) {
        auto info = info_it->second;
        if (backuping_) {
            if (extra_deleted_hashs_.count(info->hash_) <= 0)
                extra_deleted_hashs_.insert(info->hash_);
            modified_ = true;
            LOGUTIL_LOG_I() << "del url " << info->url_;
            return true;
        }
        hash2recs_.erase(info->hash_);
        url2recs_.erase(info->url_);
        modified_ = true;
        LOGUTIL_LOG_I() << "del url " << info->url_;
        return true;
    }
    if (backuping_) {
        auto extra_it = extra_url2recs_.find(url);
        if (extra_it != extra_url2recs_.end()) {
            auto info = extra_it->second;
            extra_hash2recs_.erase(info->hash_);
            extra_url2recs_.erase(info->url_);
            modified_ = true;
            LOGUTIL_LOG_I() << "del url " << info->url_;
            return true;
        }
    }
    return false;
}
bool ShortUrlMgr::DelHash(const std::string &hash) {
    std::unique_lock<std::shared_mutex> guard(mtx_);
    auto info_it = hash2recs_.find(hash);
    if (info_it != hash2recs_.end()) {
        if (backuping_) {
            if (extra_deleted_hashs_.count(hash) <= 0)
                extra_deleted_hashs_.insert(hash);
            modified_ = true;
            LOGUTIL_LOG_I() << "del hash " << hash;
            return true;
        }
        auto url = info_it->second->url_;
        hash2recs_.erase(hash);
        url2recs_.erase(url);
        modified_ = true;
        LOGUTIL_LOG_I() << "del hash " << hash;
        return true;
    }
    if (backuping_) {
        auto extra_it = extra_hash2recs_.find(hash);
        if (extra_it != extra_hash2recs_.end()) {
            auto url = extra_it->second->url_;
            extra_hash2recs_.erase(hash);
            extra_url2recs_.erase(url);
            modified_ = true;
            LOGUTIL_LOG_I() << "del hash " << hash;
            return true;
        }
    }
    return false;
}
ShortUrlRecord ShortUrlMgr::GetUrlInfo(const std::string &key, bool is_hash) {
    std::shared_lock<std::shared_mutex> guard(mtx_);
    auto &key2recs = is_hash ? hash2recs_ : url2recs_;
    auto &extra_key2recs = is_hash ? extra_hash2recs_ : extra_url2recs_;
    auto info_it = key2recs.find(key);
    if (info_it != key2recs.end()) {
        auto &info = info_it->second;
        if (backuping_ && extra_deleted_hashs_.count(info->hash_) > 0)
            return ShortUrlRecord("", "", 0);
        return *info;
    }
    if (backuping_) {
        auto extra_it = extra_key2recs.find(key);
        if (extra_it != extra_key2recs.end())
            return *extra_it->second;
    }
    return ShortUrlRecord("", "", 0);
}
std::string ShortUrlMgr::GetUrl(const std::string &hash) {
    auto info = GetUrlInfo(hash, true);
    return info.url_;
}
std::string ShortUrlMgr::GetHash(const std::string &url) {
    auto info = GetUrlInfo(url, false);
    return info.hash_;
}
std::string ShortUrlMgr::GenerateHash(const std::string &url) const {
    auto tmp_url = url;
    std::string ret;
    do {
        md5::MD5 hash(tmp_url);
        ret = hash.toString().substr(0, hash_width_);
        tmp_url += ' ';
    } while (hash2recs_.count(ret) > 0 || (backuping_ && extra_hash2recs_.count(ret) > 0));
    return ret;
}

void ShortUrlMgr::SaveRecordsSync(const std::string &save_path) {
    if (!sn::FileUtil::IsFolderExist(save_path))
        sn::FileUtil::CreateFolder(save_path);
    std::unique_lock<std::shared_mutex> guard(mtx_);
    std::ofstream fout(save_path + "/urls.txt");
    for (auto &info_pair : hash2recs_) {
        auto &info = info_pair.second;
        fout << info->timestamp_ << " " << info->hash_ << "\n" << info->url_ << "\n";
    }
    fout.close();
    modified_ = false;
    LOGUTIL_LOG_I() << "sync save finished.";
}
void ShortUrlMgr::SaveRecordsAsync(const std::string &save_path) {
    if (!sn::FileUtil::IsFolderExist(save_path))
        sn::FileUtil::CreateFolder(save_path);
    {
        std::unique_lock<std::shared_mutex> guard(mtx_);
        backuping_ = true;
    }
    std::thread thr([this, save_path]() {
        std::ofstream fout(save_path + "/urls.txt");
        for (auto &info_pair : hash2recs_) {
            auto &info = info_pair.second;
            fout << info->timestamp_ << " " << info->hash_ << "\n" << info->url_ << "\n";
        }
        std::unordered_map<std::string, std::shared_ptr<ShortUrlRecord>> add_hash2infos;
        std::unordered_set<std::string> rm_hashs;
        do {
            for (auto &info_pair : add_hash2infos) {
                auto &info = info_pair.second;
                fout << info->timestamp_ << " " << info->hash_ << "\n" << info->url_ << "\n";
            }
            for (auto &hash : rm_hashs) {
                fout << 0 << " " << "----" << "\n" << hash << "\n";
            }
            add_hash2infos.clear();
            rm_hashs.clear();
            {
                std::unique_lock<std::shared_mutex> guard(mtx_);
                for (auto &hash : extra_deleted_hashs_) {
                    auto info_it = hash2recs_.find(hash);
                    if (info_it == hash2recs_.end())
                        continue;
                    auto info = info_it->second;
                    url2recs_.erase(info->url_);
                    hash2recs_.erase(info->hash_);
                }
                url2recs_.insert(extra_url2recs_.begin(), extra_url2recs_.end());
                hash2recs_.insert(extra_hash2recs_.begin(), extra_hash2recs_.end());

                extra_url2recs_.clear();
                std::swap(add_hash2infos, extra_hash2recs_);
                std::swap(rm_hashs, extra_deleted_hashs_);
                if (add_hash2infos.empty() && rm_hashs.empty()) {
                    backuping_ = false;
                    break;
                }
            }
        } while (!add_hash2infos.empty() || !rm_hashs.empty());
        modified_ = false;
        LOGUTIL_LOG_I() << "async save finished.";
    });
    thr.detach();
}

void ShortUrlMgr::LoadRecords(const std::string &save_path) {
    if (!sn::FileUtil::IsFolderExist(save_path))
        return;
    auto file_path = save_path + "/urls.txt";
    if (!FileUtil::IsFileExist(file_path))
        return;
    std::unique_lock<std::shared_mutex> guard(mtx_);
    std::ifstream fin(file_path);
    std::int64_t tm;
    std::string hash, url;
    while (fin >> tm >> hash) {
        std::getline(fin, url);
        while (url.empty())
            std::getline(fin, url);
        if (tm == 0 && hash == "----") {
            auto rec_it = hash2recs_.find(url);
            if (rec_it != hash2recs_.end()) {
                auto info = rec_it->second;
                url2recs_.erase(info->url_);
                hash2recs_.erase(info->hash_);
            }
            continue;
        }
        auto info = std::make_shared<ShortUrlRecord>(url, hash, tm);
        url2recs_[url] = info;
        hash2recs_[hash] = info;
    }
}
