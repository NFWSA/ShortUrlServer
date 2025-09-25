#ifndef SN_SHORT_URL_SERVER_CONFIG_UTIL_H
#define SN_SHORT_URL_SERVER_CONFIG_UTIL_H

#include <string>
#include <vector>
#include <map>
#include <set>

namespace sn {
namespace ConfigUtil {

struct ConfigMap{
    ConfigMap(const std::map<std::string, std::map<std::string, std::string>> &cfg);
    ConfigMap(const std::string &file_path_or_content, bool is_filename = true);
    ~ConfigMap();

    bool                     ReadConfigBool(  const std::string &key, const std::string &field = "") const;
    int                      ReadConfigInt(   const std::string &key, const std::string &field = "") const;
    long                     ReadConfigLong(  const std::string &key, const std::string &field = "") const;
    double                   ReadConfigDouble(const std::string &key, const std::string &field = "") const;
    std::string              ReadConfigString(const std::string &key, const std::string &field = "") const;
    std::set<std::string>    ReadConfigSet(   const std::string &key, const std::string &delimiter = ",", const std::string &field = "") const;
    std::vector<std::string> ReadConfigVector(const std::string &key, const std::string &delimiter = ",", const std::string &field = "") const;

    bool TryReadConfig(bool &val,                     const std::string &key, const std::string &field = "") const;
    bool TryReadConfig(int &val,                      const std::string &key, const std::string &field = "") const;
    bool TryReadConfig(long &val,                     const std::string &key, const std::string &field = "") const;
    bool TryReadConfig(double &val,                   const std::string &key, const std::string &field = "") const;
    bool TryReadConfig(std::string &val,              const std::string &key, const std::string &field = "") const;
    bool TryReadConfig(std::set<std::string> &val,    const std::string &key, const std::string &delimiter = ",", const std::string &field = "") const;
    bool TryReadConfig(std::vector<std::string> &val, const std::string &key, const std::string &delimiter = ",", const std::string &field = "") const;

private:
    bool TryReadConfigImpl(std::string &val, const std::string &key, const std::string &field = "") const;

    std::map<std::string, std::map<std::string, std::string>> cfg_;
};

extern bool ReadConfigKey(const std::map<std::string, std::string> &key2val, const std::string &key, std::string &val);
extern bool ReadConfigKey(const std::map<std::string, std::string> &key2val,
        const std::string &key, std::vector<std::string> &val, const std::string &delimiter = ",");
extern bool ReadConfigKey(const std::map<std::string, std::string> &key2val,
        const std::string &key, std::set<std::string> &val, const std::string &delimiter = ",");
extern bool ReadConfigKey(const std::map<std::string, std::string> &key2val, const std::string &key, bool &val);
extern bool ReadConfigKey(const std::map<std::string, std::string> &key2val, const std::string &key, int &val);
extern bool ReadConfigKey(const std::map<std::string, std::string> &key2val, const std::string &key, double &val);

} /* namespace ConfigUtil */
} /* namespace sn */

#endif // SN_SHORT_URL_SERVER_CONFIG_UTIL_H
