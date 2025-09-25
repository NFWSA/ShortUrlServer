#ifndef SN_SHORT_URL_SERVER_JSON_UTIL_H
#define SN_SHORT_URL_SERVER_JSON_UTIL_H

#include <set>
#include <string>
#include <vector>
#include <map>
#include <functional>
#include <memory>

#include <Poco/JSON/Object.h>

namespace sn {
namespace JsonUtil {

template<typename T>
struct JsonOption{
    T val_;
    bool is_null_;
    JsonOption(const T &val) : val_(val), is_null_(false) {}
    JsonOption(T &&val) : val_(std::move(val)), is_null_(false) {}
    JsonOption(const std::nullptr_t&) : is_null_(true) {}
    JsonOption() : is_null_(true) {}
    JsonOption(const JsonOption<T> &val) : val_(val.val_), is_null_(val.is_null_) {}
    JsonOption(JsonOption<T> &&val) : val_(std::move(val.val_)), is_null_(val.is_null_) {}

    JsonOption<T>& operator=(const std::nullptr_t&) {
        is_null_ = true;
        return *this;
    }
    JsonOption<T>& operator=(const T &val) {
        is_null_ = false;
        val_ = val;
        return *this;
    }
    JsonOption<T>& operator=(T &&val) {
        is_null_ = false;
        val_ = std::move(val);
        return *this;
    }
    JsonOption<T>& operator=(const JsonOption<T> &val) {
        val_ = val.val_;
        is_null_ = val.is_null_;
        return *this;
    }
    JsonOption<T>& operator=(JsonOption<T> &&val) {
        val_ = std::move(val.val_);
        is_null_ = val.is_null_;
        return *this;
    }
    bool operator==(const std::nullptr_t&) {
        return is_null_;
    }
    bool operator!=(const std::nullptr_t&) {
        return !is_null_;
    }
    const T* operator->() const {
        return &val_;
    }
    T* operator->() {
        return &val_;
    }
    const T& operator*() const {
        return val_;
    }
    T& operator*() {
        return val_;
    }

    std::shared_ptr<T> AsShared() const {
        return is_null_ ? nullptr : std::make_shared<T>(val_);
    }
};

struct JsonValue;
namespace inner {
    struct JsonValueDict{
        // type: dict
        std::map<std::string, std::string> strs_;                 // dict[key] = type: str
        std::map<std::string, bool> bools_;                       // dict[key] = type: bool
        std::map<std::string, long> ints_;                        // dict[key] = type: int
        std::map<std::string, double> dbls_;                      // dict[key] = type: float
        std::set<std::string> nulls_;                             // dict[key] = type: null
        std::map<std::string, std::shared_ptr<JsonValue>> objs_;  // dict[key] = type: dict | list

    };

} /* namespace inner */
struct JsonValue{
    using Ptr = std::shared_ptr<JsonValue>;
    enum Type{
        Dict = 0,
        List,
        Bool,
        Int,
        Float,
        Str,
        Null,
        Unknown,
    };
    static inline std::string TypeToName(const Type &type) {
        static std::vector<std::string> names{ "对象", "列表", "布尔", "整数", "浮点数", "字符串", "null" };
        auto idx = static_cast<int>(type);
        if (idx < 0 || idx >= names.size())
            return "未知";
        return names[idx];
    }
    Type type_;

    std::string val_s_;                             // type: str
    long val_l_;                                    // type: int | bool
    double val_d_;                                  // type: float
    std::vector<Ptr> arrs_;                         // type: list
    std::shared_ptr<inner::JsonValueDict> dict_;    // type: dict

    JsonValue() : type_(Type::Null) {}
    JsonValue(bool val) : type_(Type::Bool), val_l_(val ? 1 : 0) {}
    JsonValue(long val) : type_(Type::Int), val_l_(val) {}
    JsonValue(double val) : type_(Type::Float), val_d_(val) {}
    JsonValue(const std::string &val) : type_(Type::Str), val_s_(val) {}

    bool Insert(const std::string &key, const Poco::Dynamic::Var &val, const std::string &type = "");
    bool Insert(const std::string &key, const JsonValue::Ptr &val);
    bool Insert(const std::string &key, const bool val);
    bool Insert(const std::string &key, const int val);
    bool Insert(const std::string &key, const long val);
    bool Insert(const std::string &key, const float val);
    bool Insert(const std::string &key, const double val);
    bool Insert(const std::string &key, const std::string &val);
    bool Insert(const std::string &key, const std::nullptr_t);
    bool PushBack(const JsonValue::Ptr &val);
    bool PushBack(const bool val);
    bool PushBack(const int val);
    bool PushBack(const long val);
    bool PushBack(const float val);
    bool PushBack(const double val);
    bool PushBack(const std::string &val);
    bool PushBack(const std::nullptr_t&);

    bool IsType(const Type type) const { return type == type_; }
    bool IsType(const std::set<Type> &types) const { return types.count(type_) > 0; }
    bool IsBool() const { return IsType(Type::Bool); }
    bool IsInt() const { return IsType(Type::Int); }
    bool IsFloat() const { return IsType(Type::Float); }
    bool IsNumber() const { return IsType({Type::Int, Type::Float}); }
    bool IsString() const { return IsType(Type::Str); }
    bool IsNull() const { return IsType(Type::Null); }
    bool IsDict() const { return IsType(Type::Dict); }
    bool IsArray() const { return IsType(Type::List); }
    bool IsDictOrArray() const { return IsType({Type::Dict, Type::List}); }

    JsonOption<bool> TryToBool() const;
    JsonOption<long> TryToInt() const;
    JsonOption<double> TryToFloat() const;
    JsonOption<double> TryToNumber() const;
    JsonOption<std::string> TryToString() const;
    bool ToBool(bool default_val = false) const;
    long ToInt(long default_val = 0) const;
    double ToFloat(double default_val = 0) const;
    double ToNumber(double default_val = 0) const;
    std::string ToString(const std::string &default_val = "") const;

    bool HasBool(const std::string &key) const;
    bool HasInt(const std::string &key) const;
    bool HasFloat(const std::string &key) const;
    bool HasNumber(const std::string &key) const;
    bool HasString(const std::string &key) const;
    bool HasNull(const std::string &key) const;
    bool HasDict(const std::string &key) const;
    bool HasArray(const std::string &key) const;
    bool HasDictOrArray(const std::string &key) const;
    bool HasKey(const std::string &key) const;

    std::vector<std::pair<std::string, Type>> GetKeys() const;
    Type GetKeyType(const std::string &key) const;
    JsonOption<bool> TryGetBool(const std::string &key) const;
    JsonOption<long> TryGetInt(const std::string &key) const;
    JsonOption<double> TryGetFloat(const std::string &key) const;
    JsonOption<double> TryGetNumber(const std::string &key) const;
    JsonOption<std::string> TryGetString(const std::string &key) const;
    bool TryGetBool(const std::string &key, bool &val) const;
    bool TryGetInt(const std::string &key, int &val) const;
    bool TryGetInt(const std::string &key, long &val) const;
    bool TryGetFloat(const std::string &key, double &val) const;
    bool TryGetNumber(const std::string &key, double &val) const;
    bool TryGetString(const std::string &key, std::string &val) const;
    bool GetBool(const std::string &key, bool default_val = false) const;
    long GetInt(const std::string &key, long default_val = 0) const;
    double GetFloat(const std::string &key, double default_val = 0) const;
    double GetNumber(const std::string &key, double default_val = 0) const;
    std::string GetString(const std::string &key, const std::string &default_val = "") const;
    JsonValue::Ptr GetDict(const std::string &key, const JsonValue::Ptr &default_val = nullptr) const;
    JsonValue::Ptr GetArray(const std::string &key, const JsonValue::Ptr &default_val = nullptr) const;
    JsonValue::Ptr GetDictOrArray(const std::string &key, const JsonValue::Ptr &default_val = nullptr) const;
    JsonValue::Ptr GetDictRecurisive(const std::vector<std::string> &keys,
        const JsonValue::Ptr &default_val = nullptr) const;

    std::string GetDumpString() const;
    std::ostream& DumpToStream(std::ostream &os) const;
    // std::ostream& operator<<(std::ostream &os) const;
};
using JsonType = JsonValue::Type;

template<typename T>
std::vector<T> JsonArrToVec(const JsonValue::Ptr &arr,const std::function<JsonOption<T>(const JsonValue::Ptr&)> &conv_func) {
    std::vector<T> ret;
    if (arr == nullptr || conv_func == nullptr || arr->type_ != JsonType::List)
        return ret;
    for (auto &ele : arr->arrs_) {
        auto val = conv_func(ele);
        if (val != nullptr)
            ret.emplace_back(std::move(*val));
    }
    return ret;
}

extern Poco::Dynamic::Var LoadJsonString(const std::string &str);
extern Poco::Dynamic::Var LoadJsonFile(const std::string &filename);
extern Poco::JSON::Object::Ptr LoadObjJson(const std::string &filename);
extern Poco::JSON::Array::Ptr LoadArrJson(const std::string &filename);

extern std::string ToJsonString(const std::string &str);
extern JsonValue::Ptr LoadJsonBoolean(const std::string &key, const Poco::Dynamic::Var &val);
extern JsonValue::Ptr LoadJsonInterger(const std::string &key, const Poco::Dynamic::Var &val);
extern JsonValue::Ptr LoadJsonDouble(const std::string &key, const Poco::Dynamic::Var &val);
extern JsonValue::Ptr LoadJsonString(const std::string &key, const Poco::Dynamic::Var &val);
extern JsonValue::Ptr LoadJsonDict(const std::string &key, const Poco::Dynamic::Var &val);
extern JsonValue::Ptr LoadJsonList(const std::string &key, const Poco::Dynamic::Var &val);
extern JsonValue::Ptr LoadJsonValue(const std::string &key, const Poco::Dynamic::Var &val);
extern JsonValue::Ptr LoadJsonValue(const std::string &filename);
extern JsonValue::Ptr LoadJsonValueString(const std::string &str);

} // namespace JsonUtil
} /* namespace sn */

#endif // SN_SHORT_URL_SERVER_JSON_UTIL_H
