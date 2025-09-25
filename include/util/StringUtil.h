#ifndef SN_SHORT_URL_SERVER_STRING_UTIL_H
#define SN_SHORT_URL_SERVER_STRING_UTIL_H

#include <set>
#include <string>
#include <vector>
#include <regex>

namespace sn {
namespace StringUtil {

extern void StringExtract(const std::string& input, std::regex reg, std::vector<std::string>& output);

extern bool IsInteger(const std::string& integer_str);
extern bool IsDigit(const std::string& diget_str);

extern void Replace(std::string &str, const std::string &src, const std::string &dst);
extern std::string ReplaceString(const std::string &str, const std::string &src, const std::string &dst);

extern std::string Upper(const std::string &str);
extern std::string Lower(const std::string &str);
extern std::string Trim(const std::string &str, const char trim_char);
extern std::string Trim(const std::string &str, const std::string &trim_chars);

extern bool TrySplitByDelimiter(std::string &part_left, std::string &part_right, const std::string &str, const std::string &delimiter = ",");
extern std::pair<std::string, std::string> SplitByDelimiter(const std::string &str, const std::string &delimiter = ",");

extern std::vector<std::string> SplitStringVec(const std::string &str, const std::string &delimiter = ",");
extern std::set<std::string>    SplitStringSet(const std::string &str, const std::string &delimiter = ",");
extern bool TrySplitString(const std::string &str, std::vector<std::string> &vec, const std::string &delimiter = ",");
extern bool TrySplitString(const std::string &str, std::set<std::string> &vec, const std::string &delimiter = ",");

template<typename T>
static std::string SetToString(const std::set<T> &values, const std::string &delimiter = ",",
        std::function<std::string(const T&)> to_str_func = [](const T &v) { return std::to_string(v); }) {
    if (values.empty())
        return "";
    std::set<std::string> str_set;
    int len = 0, cnt = values.size();
    for (auto &ele : values) {
        auto str = to_str_func(ele);
        len += str.size();
        str_set.emplace(std::move(str));
    }
    std::string ret;
    ret.reserve(len + (cnt - 1) * delimiter.size());
    for (auto &str : str_set)
        ret += (ret.empty() ? "" : delimiter) + str;
    return ret;
}

static std::string SetToString(const std::set<std::string> &values, const std::string &delimiter = ",") {
    return SetToString<std::string>(values, delimiter, [](const std::string &v) { return v; });
}

template<typename T>
static std::string VecToString(const std::vector<T> &values, const std::string &delimiter = ",",
        const std::function<std::string(const T&)> &to_str_func = [](const T &v) { return std::to_string(v); }) {
    if (values.empty())
        return "";
    std::vector<std::string> str_vec;
    str_vec.reserve(values.size());
    int len = 0, cnt = values.size();
    for (auto &ele : values) {
        auto str = to_str_func(ele);
        len += str.size();
        str_vec.emplace_back(std::move(str));
    }
    std::string ret;
    ret.reserve(len + (cnt - 1) * delimiter.size());
    for (auto &str : str_vec)
        ret += (ret.empty() ? "" : delimiter) + str;
    return ret;
}

static std::string VecToString(const std::vector<std::string> &values, const std::string &delimiter = ",") {
    return VecToString<std::string>(values, delimiter, [](const std::string &v) { return v; });
}

template<typename NUMBER>
struct NumberRange {
    NumberRange(const std::string &str, const std::string &delimiter, const std::string &range_char,
            std::function<NUMBER(const std::string&)> to_number) {
        auto num_pairs = SplitStringSet(str, delimiter);
        for (auto &nums : num_pairs) {
            if (nums.find(range_char) == nums.npos) {
                vals_.insert(to_number(nums));
                continue;
            }
            auto num_strs = SplitByDelimiter(nums, range_char);
            auto min = to_number(num_strs.first);
            auto max = to_number(num_strs.second);
            if (max < min) {
                // LOGUTIL_LOG_D << "numbers:" << numbers << " ele:" << nums << " max must greater than min!";
                continue;
            }
            ranges_[min] = max;
        }
    }
    NumberRange(const std::set<NUMBER> &vals = {}, const std::map<NUMBER, NUMBER> &ranges = {}): vals_(vals), ranges_(ranges) {
    }

    bool Include(const NUMBER val) const {
        if (vals_.count(val) > 0)
            return true;
        for (auto &range : ranges_) {
            if (val >= range.first && val <= range.second)
                return true;
        }
        return false;
    }
    bool Exclude(const NUMBER val) const {
        return !Include(val);
    }
    std::string ToString(const std::string &delimiter = ",", const std::string &range_char = "~") const {
        std::set<std::string> strs;
        for (auto &range : ranges_)
            strs.insert(std::to_string(range.first) + range_char + std::to_string(range.second));
        return SetToString({ SetToString(vals_, delimiter), SetToString(strs, delimiter)}, delimiter);
    }

private:
    std::set<NUMBER> vals_;
    std::map<NUMBER, NUMBER> ranges_;
};
extern NumberRange<long> ParseIntegerRanges(const std::string &str, const std::string &delimiter = ",", const std::string &range_char = "~");
extern NumberRange<double> ParseDoubleRanges(const std::string &str, const std::string &delimiter = ",", const std::string &range_char = "~");

extern std::string Format(const std::string &format, const std::vector<std::string> &fields = {});

extern bool TryStrToBool(const std::string &str, bool &val);
extern bool TryStrToInt(const std::string &str, int &val);
extern bool TryStrToLong(const std::string &str, long &val);
extern bool TryStrToDouble(const std::string &str, double &val);

extern bool   StrToBool(  const std::string &str, const bool default_val = false);
extern int    StrToInt(   const std::string &str, const int default_val = 0);
extern long   StrToLong(  const std::string &str, const long default_val = 0l);
extern double StrToDouble(const std::string &str, const double default_val = 0.);

extern long   SToL_Log(const std::string &str, const std::string &file, int line, const long default_val = 0l,
    const std::string &desc = "", const std::vector<std::string> &fields = {}, bool throw_excp = true);
extern double SToD_Log(const std::string &str, const std::string &file, int line, const double default_val = 0.,
    const std::string &desc = "", const std::vector<std::string> &fields = {}, bool throw_excp = true);

extern std::string DoubleToString(double dbl, int width = -1);

} /* namespace StringUtil */
} /* namespace sn */

#define StringUtil_STOL_LOG(str, ...) sn::StringUtil::stol_log((str), __FILE__, __LINE__, __VA_ARGS__)
#define StringUtil_STOD_LOG(str, ...) sn::StringUtil::stod_log((str), __FILE__, __LINE__, __VA_ARGS__)

#endif // SN_SHORT_URL_SERVER_STRING_UTIL_H
