#include "util/ConfigUtil.h"
#include "util/StringUtil.h"
#include "util/LoggerUtil.h"
#include "util/FileUtil.h"

#include <map>

static const std::map<std::string, std::string> g_empty_map;

static inline std::map<std::string, std::map<std::string, std::string>> ParseSimpleConfigLines(const std::vector<std::string> &lines,
        const std::string &delimter = "=", const std::string &comment_head = ";",
        const std::string &segment_beg = "[", const std::string &segment_end = "]") {
    std::map<std::string, std::map<std::string, std::string>> ret;
    std::string trim_chrs = " \t\n\r";
    auto del_len = delimter.size();
    auto cmt_len = comment_head.size();
    auto seg_len_beg = segment_beg.size();
    auto seg_len_end = segment_end.size();
    std::string cur_segment = "";
    for (auto &line : lines) {
        auto del_pos = line.find(delimter);
        if (del_pos == line.npos ||
            line.substr(0, cmt_len) == comment_head)
            continue;
        if (line.size() > seg_len_beg + seg_len_end &&
            line.substr(0, seg_len_beg) == segment_beg &&
            line.substr(line.size() - seg_len_end) == segment_end) {
            cur_segment = sn::StringUtil::Trim(line.substr(seg_len_beg, line.size() - seg_len_beg - seg_len_end), trim_chrs);
        }
        int key_beg = 0, key_end = del_pos;
        int val_beg = del_pos + del_len, val_end = line.size();
        while (key_beg < key_end) {
            if (trim_chrs.find(line[key_beg]) == trim_chrs.npos)
                break;
            ++key_beg;
        }
        while (key_beg < key_end) {
            if (trim_chrs.find(line[key_end - 1]) == trim_chrs.npos)
                break;
            --key_end;
        }
        while (val_beg < val_end) {
            if (trim_chrs.find(line[val_beg]) == trim_chrs.npos)
                break;
            ++val_beg;
        }
        while (val_beg < val_end) {
            if (trim_chrs.find(line[val_end - 1]) == trim_chrs.npos)
                break;
            --val_end;
        }
        auto key = line.substr(key_beg, key_end - key_beg);
        auto val = line.substr(val_beg, val_end - val_beg);

        ret[cur_segment][std::move(key)] = std::move(val);
    }
    return ret;
}


namespace sn {
namespace ConfigUtil {

ConfigMap::ConfigMap(const std::map<std::string, std::map<std::string, std::string>> &cfg) : cfg_(cfg) {

}
ConfigMap::ConfigMap(const std::string &file_path_or_content, bool is_filename) {
    auto lines = is_filename ? FileUtil::LoadFileLines(file_path_or_content) : StringUtil::SplitStringVec(file_path_or_content, "\n");
    cfg_ = ParseSimpleConfigLines(lines);
}
ConfigMap::~ConfigMap() {}

bool ConfigMap::ReadConfigBool(const std::string &key, const std::string &field) const {
    bool ret = false;
    TryReadConfig(ret, key, field);
    return ret;
}

int ConfigMap::ReadConfigInt(const std::string &key, const std::string &field) const {
    int ret = 0;
    TryReadConfig(ret, key, field);
    return ret;
}

long ConfigMap::ReadConfigLong(const std::string &key, const std::string &field) const {
    long ret = 0;
    TryReadConfig(ret, key, field);
    return ret;
}

double ConfigMap::ReadConfigDouble(const std::string &key, const std::string &field) const {
    double ret = 0;
    TryReadConfig(ret, key, field);
    return ret;
}

std::string ConfigMap::ReadConfigString(const std::string &key, const std::string &field) const {
    std::string ret;
    TryReadConfig(ret, key, field);
    return ret;
}

std::set<std::string> ConfigMap::ReadConfigSet(const std::string &key, const std::string &delimiter, const std::string &field) const {
    std::set<std::string> ret;
    TryReadConfig(ret, key, field);
    return ret;
}

std::vector<std::string> ConfigMap::ReadConfigVector(const std::string &key, const std::string &delimiter, const std::string &field) const {
    std::vector<std::string> ret;
    TryReadConfig(ret, key, field);
    return ret;
}

bool ConfigMap::TryReadConfig(bool &val, const std::string &key, const std::string &field) const {
    std::string field_val;
    if (!TryReadConfigImpl(field_val, key, field))
        return false;
    auto ret = StringUtil::TryStrToBool(field_val, val);
    if (ret) {
        LOGUTIL_LOG_I() << "[CFG] read cfg bool " << field << (field.empty() ? "" : ":") << key << " = " << val;
    }
    return ret;
}

bool ConfigMap::TryReadConfig(int &val, const std::string &key, const std::string &field) const {
    std::string field_val;
    if (!TryReadConfigImpl(field_val, key, field))
        return false;
    auto ret = StringUtil::TryStrToInt(field_val, val);
    if (ret) {
        LOGUTIL_LOG_I() << "[CFG] read cfg int " << field << (field.empty() ? "" : ":") << key << " = " << val;
    }
    return ret;
}

bool ConfigMap::TryReadConfig(long &val, const std::string &key, const std::string &field) const {
    std::string field_val;
    if (!TryReadConfigImpl(field_val, key, field))
        return false;
    auto ret = StringUtil::TryStrToLong(field_val, val);
    if (ret) {
        LOGUTIL_LOG_I() << "[CFG] read cfg long " << field << (field.empty() ? "" : ":") << key << " = " << val;
    }
    return ret;
}

bool ConfigMap::TryReadConfig(double &val, const std::string &key, const std::string &field) const {
    std::string field_val;
    if (!TryReadConfigImpl(field_val, key, field))
        return false;
    auto ret = StringUtil::TryStrToDouble(field_val, val);
    if (ret) {
        LOGUTIL_LOG_I() << "[CFG] read cfg double " << field << (field.empty() ? "" : ":") << key << " = " << val;
    }
    return ret;
}

bool ConfigMap::TryReadConfig(std::string &val, const std::string &key, const std::string &field) const {
    if (!TryReadConfigImpl(val, key, field))
        return false;
    LOGUTIL_LOG_I() << "[CFG] read cfg double " << field << (field.empty() ? "" : ":") << key << " = " << val;
    return true;
}

bool ConfigMap::TryReadConfig(std::set<std::string> &val, const std::string &key, const std::string &delimiter, const std::string &field) const {
    std::string field_val;
    if (!TryReadConfigImpl(field_val, key, field))
        return false;
    auto ret = StringUtil::TrySplitString(field_val, val, delimiter);
    if (ret) {
        LOGUTIL_LOG_I() << "[CFG] read cfg set " << field << (field.empty() ? "" : ":") << key << " = " << StringUtil::SetToString(val);
    }
    return ret;
}

bool ConfigMap::TryReadConfig(std::vector<std::string> &val, const std::string &key, const std::string &delimiter, const std::string &field) const {
    std::string field_val;
    if (!TryReadConfigImpl(field_val, key, field))
        return false;
    auto ret = StringUtil::TrySplitString(field_val, val, delimiter);
    if (ret) {
        LOGUTIL_LOG_I() << "[CFG] read cfg vec " << field << (field.empty() ? "" : ":") << key << " = " << StringUtil::VecToString(val);
    }
    return ret;
}

bool ConfigMap::TryReadConfigImpl(std::string &val, const std::string &key, const std::string &field) const {
    auto field_map_it = cfg_.find(key);
    auto &field_map = field_map_it == cfg_.end() ? g_empty_map : field_map_it->second;
    auto val_it = field_map.find(key);
    if (val_it == field_map.end())
        return false;
    val = val_it->second;
    return true;
}


bool ReadConfigKey(const std::map<std::string, std::string> &key2val, const std::string &key, std::string &val) {
    auto val_it = key2val.find(key);
    if (val_it != key2val.end()) {
        val = val_it->second;
        LOGUTIL_LOG_I() << "[CFG] change config " << key << " = " << val;
        return true;
    }
    return false;
}
bool ReadConfigKey(const std::map<std::string, std::string> &key2val,
        const std::string &key, std::vector<std::string> &val, const std::string &delimiter) {
    auto val_it = key2val.find(key);
    if (val_it != key2val.end()) {
        val = StringUtil::SplitStringVec(val_it->second, delimiter);
        LOGUTIL_LOG_I() << "[CFG] change config " << key << " = " << StringUtil::VecToString(val);
        return true;
    }
    return false;
}
bool ReadConfigKey(const std::map<std::string, std::string> &key2val,
        const std::string &key, std::set<std::string> &val, const std::string &delimiter) {
    auto val_it = key2val.find(key);
    if (val_it != key2val.end()) {
        val = StringUtil::SplitStringSet(val_it->second, delimiter);
        LOGUTIL_LOG_I() << "[CFG] change config " << key << " = " << StringUtil::SetToString(val);
        return true;
    }
    return false;
}
bool ReadConfigKey(const std::map<std::string, std::string> &key2val, const std::string &key, bool &val) {
    auto val_it = key2val.find(key);
    if (val_it != key2val.end()) {
        auto &cmp_val = val_it->second;
        val = cmp_val == "true";
        LOGUTIL_LOG_I() << "[CFG] change config " << key << " = " << (val ? "true" : "false");
        return true;
    }
    return false;
}
bool ReadConfigKey(const std::map<std::string, std::string> &key2val, const std::string &key, int &val) {
    auto val_it = key2val.find(key);
    if (val_it != key2val.end()) {
        val = StringUtil::StrToInt(val_it->second);
        LOGUTIL_LOG_I() << "[CFG] change config " << key << " = " << val;
        return true;
    }
    return false;
}
bool ReadConfigKey(const std::map<std::string, std::string> &key2val, const std::string &key, double &val) {
    auto val_it = key2val.find(key);
    if (val_it != key2val.end()) {
        val = StringUtil::StrToDouble(val_it->second);
        LOGUTIL_LOG_I() << "[CFG] change config " << key << " = " << val;
        return true;
    }
    return false;
}

} /* namespace ConfigUtil */
} /* namespace sn */
