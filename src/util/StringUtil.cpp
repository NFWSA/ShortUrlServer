#include "util/StringUtil.h"
#include "util/LoggerUtil.h"

#include <algorithm>
#include <cctype>
#include <string>
#include <vector>
#include <iomanip>

namespace sn {
namespace StringUtil {

void StringExtract(const std::string& input,
                               std::regex reg,
                               std::vector<std::string>& output) {
    std::string::const_iterator start = input.begin();
    std::string::const_iterator end = input.end();
    std::smatch result;
    while (regex_search(start, end, result, reg)) {
        output.emplace_back(result[0]);
        start = result[0].second;	//更新搜索起始位置,搜索剩下的字符串
    }
}

bool IsInteger(const std::string &integer_str){
    bool find_dot = false;
    for (int i = 0; i < integer_str.size(); ++i) {
        auto &chr = integer_str[i];
        if (chr == '-' && i == 0)  // 有可能出现负数
            continue;
        if (chr > '9' || chr < '0')
            return false;
    }
    return !integer_str.empty();
}

bool IsDigit(const std::string &diget_str) {
    //  state machine
    // 0 : begin  1 : -  2 : .  3 : 0-9  4 : .  5 : 0-9  6 : end
    // .1234  : 0 -> 2 -> 5 -> 6
    // 1234   : 0 -> 3 -> 6
    // 1234.  : 0 -> 3 -> 4 -> 6
    // 12.34  : 0 -> 3 -> 4 -> 5 -> 6
    // -.1234 : 0 -> 1 -> 2 -> 5 -> 6
    // -1234  : 0 -> 1 -> 3 -> 6
    // -1234. : 0 -> 1 -> 3 -> 4 -> 6
    // -12.34 : 0 -> 1 -> 3 -> 4 -> 5 -> 6
    // -   : 0 -> 1
    // .   : 0,1 -> 2  3 -> 4
    // 0-9 : 0,1,3 -> 3  2,4,5  -> 5
    // end : 3,4,5 -> 6
    int state = 0;
    for (auto &ch : diget_str) {
        if (ch == '-') {
            if (state == 0)
                state = 1;
            else
                return false;
        }
        else if (ch == '.') {
            if (state == 0 || state == 1)
                state = 2;
            else if (state == 3)
                state = 4;
            else
                return false;
        }
        else if (ch >= '0' && ch <= '9') {
            if (state == 3 || state == 5)
                continue;
            else if (state == 0 || state == 1)
                state = 3;
            else if (state == 2 || state == 4)
                state = 5;
            else
                return false;
        }
        else
            return false;
    }
    return state >= 3 && state <= 5;
}

void Replace(std::string &str, const std::string &src, const std::string &dst) {
    std::string::size_type pos = 0;
    auto len_src = src.size();
    auto len_dst = dst.size();

    if (0 == len_src)
        return;

    while ((pos = str.find(src, pos)) != str.npos) {
        str.replace(pos, len_src, dst);
        pos += len_dst;
    }
}

std::string ReplaceString(const std::string &str, const std::string &src, const std::string &dst) {
    auto ret = str;
    Replace(ret, src, dst);
    return ret;
}

std::string Upper(const std::string &str) {
    std::string upper = str;
    std::transform(upper.begin(), upper.end(), upper.begin(),
        [](unsigned char c) -> unsigned char { return std::toupper(c); });
    return upper;
}

std::string Lower(const std::string &str) {
    std::string lower = str;
    std::transform(lower.begin(), lower.end(), lower.begin(),
        [](unsigned char c) -> unsigned char { return std::tolower(c); });
    return lower;
}

std::string Trim(const std::string &str, const char trim_char) {
    int trim_beg = 0, trim_end = str.size();
    while (trim_beg < trim_end) {
        if (str[trim_beg] != trim_char)
            break;
        ++trim_beg;
    }
    while (trim_beg < trim_end) {
        if (str[trim_end - 1] != trim_char)
            break;
        --trim_end;
    }
    return str.substr(trim_beg, trim_end - trim_beg);
}

std::string Trim(const std::string &str, const std::string &trim_chars) {
    int trim_beg = 0, trim_end = str.size();
    while (trim_beg < trim_end) {
        if (trim_chars.find(str[trim_beg]) == trim_chars.npos)
            break;
        ++trim_beg;
    }
    while (trim_beg < trim_end) {
        if (trim_chars.find(str[trim_end - 1]) == trim_chars.npos)
            break;
        --trim_end;
    }
    return str.substr(trim_beg, trim_end - trim_beg);
}

bool TrySplitByDelimiter(std::string &part_left, std::string &part_right, const std::string &str, const std::string &delimiter) {
    auto del_pos = str.find(delimiter);
    if (del_pos == std::string::npos)
        return false;
    part_left = str.substr(0, del_pos);
    part_right = str.substr(del_pos + delimiter.size());
    return true;
}

std::pair<std::string, std::string> SplitByDelimiter(const std::string &str, const std::string &delimiter) {
    std::pair<std::string, std::string> parts;
    TrySplitByDelimiter(parts.first, parts.second, str, delimiter);
    return parts;
}

std::vector<std::string> SplitStringVec(const std::string &str, const std::string &delimiter) {
    std::vector<std::string> ret;
    TrySplitString(str, ret, delimiter);
    return ret;
}

std::set<std::string> SplitStringSet(const std::string &str, const std::string &delimiter) {
    std::set<std::string> ret;
    TrySplitString(str, ret, delimiter);
    return ret;
}

bool TrySplitString(const std::string &str, std::vector<std::string> &vec, const std::string &delimiter) {
    if (str.empty() || delimiter.empty())
        return false;
    auto del_sz = delimiter.size();
    vec.clear();
    std::size_t head = 0;
    while (head <= str.size()) {
        auto dit = str.find(delimiter, head);
        if (dit == str.npos) {
            vec.emplace_back(str.substr(head));
            break;
        }
        vec.emplace_back(str.substr(head, dit - head));
        head = dit + del_sz;
    }
    return true;
}

bool TrySplitString(const std::string &str, std::set<std::string> &vec, const std::string &delimiter) {
    std::vector<std::string> tmp_vec;
    if (!TrySplitString(str, tmp_vec, delimiter))
        return false;
    vec = std::set<std::string>(tmp_vec.cbegin(), tmp_vec.cend());
    return true;
}

NumberRange<long> ParseIntegerRanges(const std::string &str, const std::string &delimiter, const std::string &range_char) {
    return NumberRange<long>(str, delimiter, range_char, [](const std::string &s) { return StrToLong(s); });
}

NumberRange<double> ParseDoubleRanges(const std::string &str, const std::string &delimiter, const std::string &range_char) {
    return NumberRange<double>(str, delimiter, range_char, [](const std::string &s) { return StrToDouble(s); });
}

std::string Format(const std::string &format, const std::vector<std::string> &fields) {
    auto descs = StringUtil::SplitStringVec(format.c_str(), "%");
    int total = 0;
    for (auto &val : fields)
        total += val.size();
    for (auto &val : descs)
        total += val.size();
    std::string ret;
    ret.reserve(total + 1);

    auto desc_it = descs.begin();
    auto field_it = fields.begin();
    ret += *(desc_it++);
    while (desc_it != descs.end()) {
        if (field_it != fields.end())
            ret += *(field_it++);
        ret += *(desc_it++);
    }
    return ret;
}

bool TryStrToBool(const std::string &str, bool &val) {
    static std::set<std::string> set_true { "1", "true", "on", "open" };
    static std::set<std::string> set_false{ "0", "false", "off", "close" };
    auto lower_val = Lower(str);
    if (set_true.count(lower_val) > 0) {
        val = true;
        return true;
    }
    else if (set_false.count(lower_val) > 0) {
        val = false;
        return true;
    }
    return false;
}

bool TryStrToInt(const std::string &str, int &val) {
    try {
        val = std::stoi(str);
        return true;
    }
    catch (std::exception &e) {
    }
    return false;
}
bool TryStrToLong(const std::string &str, long &val) {
    try {
        val = std::stol(str);
        return true;
    }
    catch (std::exception &e) {
    }
    return false;
}
bool TryStrToLLong(const std::string &str, long long &val) {
    try {
        val = std::stoll(str);
        return true;
    }
    catch (std::exception &e) {
    }
    return false;
}

bool TryStrToULong(const std::string &str, unsigned long &val) {
    try {
        val = std::stoul(str);
        return true;
    }
    catch (std::exception &e) {
    }
    return false;
}
bool TryStrToULLong(const std::string &str, unsigned long long &val) {
    try {
        val = std::stoull(str);
        return true;
    }
    catch (std::exception &e) {
    }
    return false;
}

bool TryStrToFloat(const std::string &str, float &val) {
    try {
        val = std::stof(str);
        return true;
    }
    catch (std::exception &e) {
    }
    return false;
}
bool TryStrToDouble(const std::string &str, double &val) {
    try {
        val = std::stod(str);
        return true;
    }
    catch (std::exception &e) {
    }
    return false;
}
bool TryStrToLDouble(const std::string &str, long double &val) {
    try {
        val = std::stold(str);
        return true;
    }
    catch (std::exception &e) {
    }
    return false;
}


bool StrToBool(const std::string &str, const bool default_val) {
    bool ret = default_val;
    if (TryStrToBool(str, ret))
        return ret;
    return default_val;
}

int StrToInt(const std::string &str, const int default_val) {
    int ret = default_val;
    if (TryStrToInt(str, ret))
        return ret;
    return default_val;
}
long StrToLong(const std::string &str, const long default_val) {
    long ret = default_val;
    if (TryStrToLong(str, ret))
        return ret;
    return default_val;
}
long long StrToLLong(const std::string &str, const long long default_val) {
    long long ret = default_val;
    if (TryStrToLLong(str, ret))
        return ret;
    return default_val;
}

unsigned long StrToULong(const std::string &str, const unsigned long default_val) {
    unsigned long ret = default_val;
    if (TryStrToULong(str, ret))
        return ret;
    return default_val;
}
unsigned long long StrToULLong(const std::string &str, const unsigned long long default_val) {
    unsigned long long ret = default_val;
    if (TryStrToULLong(str, ret))
        return ret;
    return default_val;
}

float StrToFloat(const std::string &str, const float default_val) {
    float ret = default_val;
    if (TryStrToFloat(str, ret))
        return ret;
    return default_val;
}
double StrToDouble(const std::string &str, const double default_val) {
    double ret = default_val;
    if (TryStrToDouble(str, ret))
        return ret;
    return default_val;
}
long double StrToLongDouble(const std::string &str, const long double default_val) {
    long double ret = default_val;
    if (TryStrToLDouble(str, ret))
        return ret;
    return default_val;
}


int SToI_Log(const std::string &str, const std::string &file, int line, const int default_val,
        const std::string &desc, const std::vector<std::string> &fields, bool need_log, bool throw_excp) {
    try {
        return std::stoi(str);
    }
    catch (std::exception &e) {
        if (need_log)
            sn::LoggerUtil::SingleLogMessage(sn::LoggerUtil::LogLevel::LOG_LV_ERROR, file, line).stream()
                << "stoi exception:\"" << e.what() << " str:\"" << str << "\"\n desc:" << Format(desc, fields);
        if (throw_excp)
            throw e;
        return default_val;
    }
}

long SToL_Log(const std::string &str, const std::string &file, int line, const long default_val,
        const std::string &desc, const std::vector<std::string> &fields, bool need_log, bool throw_excp) {
    try {
        return std::stol(str);
    }
    catch (std::exception &e) {
        if (need_log)
            sn::LoggerUtil::SingleLogMessage(sn::LoggerUtil::LogLevel::LOG_LV_ERROR, file, line).stream()
                << "stol exception:\"" << e.what() << " str:\"" << str << "\"\n desc:" << Format(desc, fields);
        if (throw_excp)
            throw e;
        return default_val;
    }
}

long long SToLL_Log(const std::string &str, const std::string &file, int line, const long long default_val,
        const std::string &desc, const std::vector<std::string> &fields, bool need_log, bool throw_excp) {
    try {
        return std::stoll(str);
    }
    catch (std::exception &e) {
        if (need_log)
            sn::LoggerUtil::SingleLogMessage(sn::LoggerUtil::LogLevel::LOG_LV_ERROR, file, line).stream()
                << "stoll exception:\"" << e.what() << " str:\"" << str << "\"\n desc:" << Format(desc, fields);
        if (throw_excp)
            throw e;
        return default_val;
    }
}

unsigned long SToUL_Log(const std::string &str, const std::string &file, int line, const unsigned long default_val,
        const std::string &desc, const std::vector<std::string> &fields, bool need_log, bool throw_excp) {
    try {
        return std::stoul(str);
    }
    catch (std::exception &e) {
        if (need_log)
            sn::LoggerUtil::SingleLogMessage(sn::LoggerUtil::LogLevel::LOG_LV_ERROR, file, line).stream()
                << "stoul exception:\"" << e.what() << " str:\"" << str << "\"\n desc:" << Format(desc, fields);
        if (throw_excp)
            throw e;
        return default_val;
    }
}

unsigned long long SToULL_Log(const std::string &str, const std::string &file, int line, const unsigned long long default_val,
        const std::string &desc, const std::vector<std::string> &fields, bool need_log, bool throw_excp) {
    try {
        return std::stoull(str);
    }
    catch (std::exception &e) {
        if (need_log)
            sn::LoggerUtil::SingleLogMessage(sn::LoggerUtil::LogLevel::LOG_LV_ERROR, file, line).stream()
                << "stoull exception:\"" << e.what() << " str:\"" << str << "\"\n desc:" << Format(desc, fields);
        if (throw_excp)
            throw e;
        return default_val;
    }
}

float SToF_Log(const std::string &str, const std::string &file, int line, const float default_val,
        const std::string &desc, const std::vector<std::string> &fields, bool need_log, bool throw_excp) {
    try {
        return std::stof(str);
    }
    catch (std::exception &e) {
        if (need_log)
            sn::LoggerUtil::SingleLogMessage(sn::LoggerUtil::LogLevel::LOG_LV_ERROR, file, line).stream()
                << "stof exception:\"" << e.what() << " str:\"" << str << "\"\n desc:" << Format(desc, fields);
        if (throw_excp)
            throw e;
        return default_val;
    }
}

double SToD_Log(const std::string &str, const std::string &file, int line, const double default_val,
        const std::string &desc, const std::vector<std::string> &fields, bool need_log, bool throw_excp) {
    try {
        return std::stod(str);
    }
    catch (std::exception &e) {
        if (need_log)
            sn::LoggerUtil::SingleLogMessage(sn::LoggerUtil::LogLevel::LOG_LV_ERROR, file, line).stream()
                << "stod exception:\"" << e.what() << " str:\"" << str << "\"\n desc:" << Format(desc, fields);
        if (throw_excp)
            throw e;
        return default_val;
    }
}

long double SToLD_Log(const std::string &str, const std::string &file, int line, const long double default_val,
        const std::string &desc, const std::vector<std::string> &fields, bool need_log, bool throw_excp) {
    try {
        return std::stold(str);
    }
    catch (std::exception &e) {
        if (need_log)
            sn::LoggerUtil::SingleLogMessage(sn::LoggerUtil::LogLevel::LOG_LV_ERROR, file, line).stream()
                << "stold exception:\"" << e.what() << " str:\"" << str << "\"\n desc:" << Format(desc, fields);
        if (throw_excp)
            throw e;
        return default_val;
    }
}

std::string DoubleToString(double dbl, int width) {
    std::stringstream ss;
    if (width < 0)
        ss << std::defaultfloat;
    else
        ss << std::setprecision(width);
    ss << dbl;
    return ss.str();
}

} /* namespace StringUtil */
} /* namespace sn */
