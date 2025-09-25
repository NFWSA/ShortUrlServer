#ifndef SN_SHORT_URL_SERVER_CONFIG_UTIL_H
#define SN_SHORT_URL_SERVER_CONFIG_UTIL_H

#include <string>
#include <vector>
#include <map>
#include <set>
#include <cstdint>

namespace sn {
namespace ConfigUtil {

struct ConfigMap{
    ConfigMap(const std::map<std::string, std::map<std::string, std::string>> &cfg);
    ConfigMap(const std::string &file_path_or_content, bool is_filename = true);
    ~ConfigMap();

    bool                     ReadConfigBool(   const std::string &key, const bool default_val = false,              const std::string &field = "") const;
    int                      ReadConfigInt(    const std::string &key, const int default_val = 0,                   const std::string &field = "") const;
    long                     ReadConfigLong(   const std::string &key, const long default_val = 0l,                 const std::string &field = "") const;
    long long                ReadConfigLLong(  const std::string &key, const long long default_val = 0ll,           const std::string &field = "") const;
    unsigned long            ReadConfigULong(  const std::string &key, const unsigned long default_val = 0ul,       const std::string &field = "") const;
    unsigned long long       ReadConfigULLong( const std::string &key, const unsigned long long default_val = 0ull, const std::string &field = "") const;
    float                    ReadConfigFloat(  const std::string &key, const float default_val = 0.f,               const std::string &field = "") const;
    double                   ReadConfigDouble( const std::string &key, const double default_val = 0.,               const std::string &field = "") const;
    long double              ReadConfigLDouble(const std::string &key, const long double default_val = 0.,          const std::string &field = "") const;
    std::string              ReadConfigString( const std::string &key, const std::string &default_val = "",         const std::string &field = "") const;
    std::set<std::string>    ReadConfigSet(    const std::string &key, const std::set<std::string> &default_val = {},
                                                                                const std::string &delimiter = ",", const std::string &field = "") const;
    std::vector<std::string> ReadConfigVector( const std::string &key, const std::vector<std::string> &default_val = {},
                                                                                const std::string &delimiter = ",", const std::string &field = "") const;

    bool TryReadConfig(std::int8_t &val,              const std::string &key, const std::string &field = "") const;
    bool TryReadConfig(std::int16_t &val,             const std::string &key, const std::string &field = "") const;
    bool TryReadConfig(std::int32_t &val,             const std::string &key, const std::string &field = "") const;
    bool TryReadConfig(std::int64_t &val,             const std::string &key, const std::string &field = "") const;
    bool TryReadConfig(std::uint8_t &val,             const std::string &key, const std::string &field = "") const;
    bool TryReadConfig(std::uint16_t &val,            const std::string &key, const std::string &field = "") const;
    bool TryReadConfig(std::uint32_t &val,            const std::string &key, const std::string &field = "") const;
    bool TryReadConfig(std::uint64_t &val,            const std::string &key, const std::string &field = "") const;
#if __WORDSIZE != 64
    bool TryReadConfig(long &val,                     const std::string &key, const std::string &field = "") const;
    bool TryReadConfig(unsigned long &val,            const std::string &key, const std::string &field = "") const;
#else
    bool TryReadConfig(long long &val,                const std::string &key, const std::string &field = "") const;
    bool TryReadConfig(unsigned long long &val,       const std::string &key, const std::string &field = "") const;
#endif
    bool TryReadConfig(bool &val,                     const std::string &key, const std::string &field = "") const;
    bool TryReadConfig(float &val,                    const std::string &key, const std::string &field = "") const;
    bool TryReadConfig(double &val,                   const std::string &key, const std::string &field = "") const;
    bool TryReadConfig(long double &val,              const std::string &key, const std::string &field = "") const;
    bool TryReadConfig(std::string &val,              const std::string &key, const std::string &field = "") const;
    bool TryReadConfig(std::set<std::string> &val,    const std::string &key, const std::string &delimiter = ",", const std::string &field = "") const;
    bool TryReadConfig(std::vector<std::string> &val, const std::string &key, const std::string &delimiter = ",", const std::string &field = "") const;

private:
    bool TryReadConfigImpl(std::string &val, const std::string &key, const std::string &field = "") const;

    std::map<std::string, std::map<std::string, std::string>> cfg_;
};

} /* namespace ConfigUtil */
} /* namespace sn */

#endif // SN_SHORT_URL_SERVER_CONFIG_UTIL_H
