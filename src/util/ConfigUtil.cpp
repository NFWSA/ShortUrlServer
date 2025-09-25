#include "util/ConfigUtil.h"
#include "util/StringUtil.h"
#include "util/LoggerUtil.h"
#include "util/FileUtil.h"

#include <map>
#include <string>

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

bool ConfigMap::ReadConfigBool(const std::string &key, const bool default_val, const std::string &field) const {
    bool ret = default_val;
    TryReadConfig(ret, key, field);
    return ret;
}

int ConfigMap::ReadConfigInt(const std::string &key, const int default_val, const std::string &field) const {
    int ret = default_val;
    TryReadConfig(ret, key, field);
    return ret;
}

long ConfigMap::ReadConfigLong(const std::string &key, const long default_val, const std::string &field) const {
    long ret = default_val;
    TryReadConfig(ret, key, field);
    return ret;
}

long long ConfigMap::ReadConfigLLong(const std::string &key, const long long default_val, const std::string &field) const {
    long long ret = default_val;
    TryReadConfig(ret, key, field);
    return ret;
}

unsigned long ConfigMap::ReadConfigULong(const std::string &key, const unsigned long default_val, const std::string &field) const {
    unsigned long ret = default_val;
    TryReadConfig(ret, key, field);
    return ret;
}

unsigned long long ConfigMap::ReadConfigULLong(const std::string &key, const unsigned long long default_val, const std::string &field) const {
    unsigned long long ret = default_val;
    TryReadConfig(ret, key, field);
    return ret;
}

float ConfigMap::ReadConfigFloat(const std::string &key, const float default_val, const std::string &field) const {
    float ret = default_val;
    TryReadConfig(ret, key, field);
    return ret;
}

double ConfigMap::ReadConfigDouble(const std::string &key, const double default_val, const std::string &field) const {
    double ret = default_val;
    TryReadConfig(ret, key, field);
    return ret;
}

long double ConfigMap::ReadConfigLDouble(const std::string &key, const long double default_val, const std::string &field) const {
    long double ret = default_val;
    TryReadConfig(ret, key, field);
    return ret;
}

std::string ConfigMap::ReadConfigString(const std::string &key, const std::string &default_val, const std::string &field) const {
    std::string ret = default_val;
    TryReadConfig(ret, key, field);
    return ret;
}

std::set<std::string> ConfigMap::ReadConfigSet(const std::string &key, const std::set<std::string> &default_val,
        const std::string &delimiter, const std::string &field) const {
    std::set<std::string> ret = default_val;
    TryReadConfig(ret, key, field);
    return ret;
}

std::vector<std::string> ConfigMap::ReadConfigVector(const std::string &key, const std::vector<std::string> &default_val,
        const std::string &delimiter, const std::string &field) const {
    std::vector<std::string> ret = default_val;
    TryReadConfig(ret, key, field);
    return ret;
}

bool ConfigMap::TryReadConfig(std::int8_t &val, const std::string &key, const std::string &field) const {
    std::string field_val;
    if (!TryReadConfigImpl(field_val, key, field))
        return false;
    long exp_val = 0;
    auto ret = StringUtil::TryStrToLong(field_val, exp_val);
    if (ret) {
        val = exp_val;
        LOGUTIL_LOG_I() << "[CFG] read cfg int8 " << field << (field.empty() ? "" : ":") << key << " = " << val;
    }
    return ret;
}
bool ConfigMap::TryReadConfig(std::int16_t &val, const std::string &key, const std::string &field) const {
    std::string field_val;
    if (!TryReadConfigImpl(field_val, key, field))
        return false;
    long exp_val = 0;
    auto ret = StringUtil::TryStrToLong(field_val, exp_val);
    if (ret) {
        val = exp_val;
        LOGUTIL_LOG_I() << "[CFG] read cfg int16 " << field << (field.empty() ? "" : ":") << key << " = " << val;
    }
    return ret;
}
bool ConfigMap::TryReadConfig(std::int32_t &val, const std::string &key, const std::string &field) const {
    std::string field_val;
    if (!TryReadConfigImpl(field_val, key, field))
        return false;
    long exp_val = 0;
    auto ret = StringUtil::TryStrToLong(field_val, exp_val);
    if (ret) {
        val = exp_val;
        LOGUTIL_LOG_I() << "[CFG] read cfg int32 " << field << (field.empty() ? "" : ":") << key << " = " << val;
    }
    return ret;
}
bool ConfigMap::TryReadConfig(std::int64_t &val, const std::string &key, const std::string &field) const {
    std::string field_val;
    if (!TryReadConfigImpl(field_val, key, field))
        return false;
    long long exp_val = 0;
    auto ret = StringUtil::TryStrToLLong(field_val, exp_val);
    if (ret) {
        val = exp_val;
        LOGUTIL_LOG_I() << "[CFG] read cfg int64 " << field << (field.empty() ? "" : ":") << key << " = " << val;
    }
    return ret;
}
bool ConfigMap::TryReadConfig(std::uint8_t &val, const std::string &key, const std::string &field) const {
    std::string field_val;
    if (!TryReadConfigImpl(field_val, key, field))
        return false;
    unsigned long exp_val = 0;
    auto ret = StringUtil::TryStrToULong(field_val, exp_val);
    if (ret) {
        val = exp_val;
        LOGUTIL_LOG_I() << "[CFG] read cfg uint8 " << field << (field.empty() ? "" : ":") << key << " = " << val;
    }
    return ret;
}
bool ConfigMap::TryReadConfig(std::uint16_t &val, const std::string &key, const std::string &field) const {
    std::string field_val;
    if (!TryReadConfigImpl(field_val, key, field))
        return false;
    unsigned long exp_val = 0;
    auto ret = StringUtil::TryStrToULong(field_val, exp_val);
    if (ret) {
        val = exp_val;
        LOGUTIL_LOG_I() << "[CFG] read cfg uint16 " << field << (field.empty() ? "" : ":") << key << " = " << val;
    }
    return ret;
}
bool ConfigMap::TryReadConfig(std::uint32_t &val, const std::string &key, const std::string &field) const {
    std::string field_val;
    if (!TryReadConfigImpl(field_val, key, field))
        return false;
    unsigned long exp_val = 0;
    auto ret = StringUtil::TryStrToULong(field_val, exp_val);
    if (ret) {
        val = exp_val;
        LOGUTIL_LOG_I() << "[CFG] read cfg uint32 " << field << (field.empty() ? "" : ":") << key << " = " << val;
    }
    return ret;
}
bool ConfigMap::TryReadConfig(std::uint64_t &val, const std::string &key, const std::string &field) const {
    std::string field_val;
    if (!TryReadConfigImpl(field_val, key, field))
        return false;
    unsigned long long exp_val = 0;
    auto ret = StringUtil::TryStrToULLong(field_val, exp_val);
    if (ret) {
        val = exp_val;
        LOGUTIL_LOG_I() << "[CFG] read cfg uint64 " << field << (field.empty() ? "" : ":") << key << " = " << val;
    }
    return ret;
}

#if __WORDSIZE != 64
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
bool ConfigMap::TryReadConfig(unsigned long &val, const std::string &key, const std::string &field) const {
    std::string field_val;
    if (!TryReadConfigImpl(field_val, key, field))
        return false;
    auto ret = StringUtil::TryStrToULong(field_val, val);
    if (ret) {
        LOGUTIL_LOG_I() << "[CFG] read cfg unsigned long " << field << (field.empty() ? "" : ":") << key << " = " << val;
    }
    return ret;
}
#else
bool ConfigMap::TryReadConfig(long long &val, const std::string &key, const std::string &field) const {
    std::string field_val;
    if (!TryReadConfigImpl(field_val, key, field))
        return false;
    auto ret = StringUtil::TryStrToLLong(field_val, val);
    if (ret) {
        LOGUTIL_LOG_I() << "[CFG] read cfg long long " << field << (field.empty() ? "" : ":") << key << " = " << val;
    }
    return ret;
}
bool ConfigMap::TryReadConfig(unsigned long long &val, const std::string &key, const std::string &field) const {
    std::string field_val;
    if (!TryReadConfigImpl(field_val, key, field))
        return false;
    auto ret = StringUtil::TryStrToULLong(field_val, val);
    if (ret) {
        LOGUTIL_LOG_I() << "[CFG] read cfg unsigned long long " << field << (field.empty() ? "" : ":") << key << " = " << val;
    }
    return ret;
}
#endif

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

bool ConfigMap::TryReadConfig(float &val, const std::string &key, const std::string &field) const {
    std::string field_val;
    if (!TryReadConfigImpl(field_val, key, field))
        return false;
    auto ret = StringUtil::TryStrToFloat(field_val, val);
    if (ret) {
        LOGUTIL_LOG_I() << "[CFG] read cfg double " << field << (field.empty() ? "" : ":") << key << " = " << val;
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

bool ConfigMap::TryReadConfig(long double &val, const std::string &key, const std::string &field) const {
    std::string field_val;
    if (!TryReadConfigImpl(field_val, key, field))
        return false;
    auto ret = StringUtil::TryStrToLDouble(field_val, val);
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

} /* namespace ConfigUtil */
} /* namespace sn */
