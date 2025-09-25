#include "util/JsonUtil.h"
#include "util/StringUtil.h"

#include <Poco/Dynamic/Var.h>
#include <Poco/JSON/Object.h>
#include <Poco/JSON/Parser.h>

#include <exception>
#include <iomanip>
#include <fstream>
#include <memory>

namespace sn {
namespace JsonUtil {

bool JsonValue::Insert(const std::string &key, const Poco::Dynamic::Var &val, const std::string &type) {
    if (dict_ == nullptr)
        dict_ = std::make_shared<inner::JsonValueDict>();
    if (!type.empty()) {
        if (type == "bool")
            dict_->bools_[key] = val.extract<bool>();
        else if (type == "int")
            dict_->ints_[key] = val.extract<long>();
        else if (type == "float")
            dict_->dbls_[key] = val.extract<double>();
        else if (type == "str")
            dict_->strs_[key] = val.extract<std::string>();
        else if (type == "none")
            dict_->nulls_.insert(key);
        else
            return false;
        type_ = Type::Dict;
        return true;
    }
    if (val.isBoolean())
        dict_->bools_[key] = val.extract<bool>();
    else if (val.isInteger())
        dict_->ints_[key] = val.extract<long>();
    else if (val.isSigned())
        dict_->dbls_[key] = val.extract<double>();
    else if (val.isString())
        dict_->strs_[key] = val.extract<std::string>();
    else if (val.isEmpty())
        dict_->nulls_.insert(key);
    else
        return false;
    type_ = Type::Dict;
    return true;
}
bool JsonValue::Insert(const std::string &key, const JsonValue::Ptr &val) {
    type_ = Type::Dict;
    if (dict_ == nullptr)
        dict_ = std::make_shared<inner::JsonValueDict>();
    if (val == nullptr || val->type_ == Type::Null)
        dict_->nulls_.insert(key);
    else if (val->type_ == Type::Dict || val->type_ == Type::List)
        dict_->objs_[key] = val;
    else if (val->type_ == Type::Bool)
        dict_->bools_[key] = val->val_l_ == 1;
    else if (val->type_ == Type::Int)
        dict_->ints_[key] = val->val_l_;
    else if (val->type_ == Type::Float)
        dict_->dbls_[key] = val->val_d_;
    else if (val->type_ == Type::Str)
        dict_->strs_[key] = val->val_s_;
    return true;
}
bool JsonValue::Insert(const std::string &key, const bool val) {
    type_ = Type::Dict;
    if (dict_ == nullptr)
        dict_ = std::make_shared<inner::JsonValueDict>();
    dict_->bools_[key] = val == 1;
    return true;
}
bool JsonValue::Insert(const std::string &key, const int val) {
    type_ = Type::Dict;
    if (dict_ == nullptr)
        dict_ = std::make_shared<inner::JsonValueDict>();
    dict_->ints_[key] = val;
    return true;
}
bool JsonValue::Insert(const std::string &key, const long val) {
    type_ = Type::Dict;
    if (dict_ == nullptr)
        dict_ = std::make_shared<inner::JsonValueDict>();
    dict_->ints_[key] = val;
    return true;
}
bool JsonValue::Insert(const std::string &key, const float val) {
    type_ = Type::Dict;
    if (dict_ == nullptr)
        dict_ = std::make_shared<inner::JsonValueDict>();
    dict_->dbls_[key] = val;
    return true;
}
bool JsonValue::Insert(const std::string &key, const double val) {
    type_ = Type::Dict;
    if (dict_ == nullptr)
        dict_ = std::make_shared<inner::JsonValueDict>();
    dict_->dbls_[key] = val;
    return true;
}
bool JsonValue::Insert(const std::string &key, const std::string &val) {
    type_ = Type::Dict;
    if (dict_ == nullptr)
        dict_ = std::make_shared<inner::JsonValueDict>();
    dict_->strs_[key] = val;
    return true;
}
bool JsonValue::Insert(const std::string &key, const std::nullptr_t) {
    type_ = Type::Dict;
    if (dict_ == nullptr)
        dict_ = std::make_shared<inner::JsonValueDict>();
    dict_->nulls_.insert(key);
    return true;
}
bool JsonValue::PushBack(const JsonValue::Ptr &val) {
    type_ = Type::List;
    dict_ = nullptr;
    arrs_.emplace_back(val);
    return true;
}
bool JsonValue::PushBack(const bool val) {
    type_ = Type::List;
    dict_ = nullptr;
    arrs_.emplace_back(std::make_shared<JsonValue>(val));
    return true;
}
bool JsonValue::PushBack(const int val) {
    type_ = Type::List;
    dict_ = nullptr;
    arrs_.emplace_back(std::make_shared<JsonValue>(static_cast<long>(val)));
    return true;
}
bool JsonValue::PushBack(const long val) {
    type_ = Type::List;
    dict_ = nullptr;
    arrs_.emplace_back(std::make_shared<JsonValue>(val));
    return true;
}
bool JsonValue::PushBack(const float val) {
    type_ = Type::List;
    dict_ = nullptr;
    arrs_.emplace_back(std::make_shared<JsonValue>(val));
    return true;
}
bool JsonValue::PushBack(const double val) {
    type_ = Type::List;
    dict_ = nullptr;
    arrs_.emplace_back(std::make_shared<JsonValue>(val));
    return true;
}
bool JsonValue::PushBack(const std::string &val) {
    type_ = Type::List;
    dict_ = nullptr;
    arrs_.emplace_back(std::make_shared<JsonValue>(val));
    return true;
}
bool JsonValue::PushBack(const std::nullptr_t&) {
    type_ = Type::List;
    dict_ = nullptr;
    arrs_.emplace_back(std::make_shared<JsonValue>());
    return true;
}

JsonOption<bool> JsonValue::TryToBool() const {
    if (IsType(Type::Bool))
        return val_l_ == 1;
    return nullptr;
}
JsonOption<long> JsonValue::TryToInt() const {
    if (IsType(Type::Int))
        return val_l_;
    return nullptr;
}
JsonOption<double> JsonValue::TryToFloat() const {
    if (IsType(Type::Float))
        return val_d_;
    return nullptr;
}
JsonOption<double> JsonValue::TryToNumber() const {
    if (IsType(Type::Int))
        return val_l_;
    if (IsType(Type::Float))
        return val_d_;
    return nullptr;
}
JsonOption<std::string> JsonValue::TryToString() const {
    if (IsType(Type::Str))
        return val_s_;
    return nullptr;
}
bool JsonValue::ToBool(bool default_val) const {
    auto opt = TryToBool();
    return opt == nullptr ? default_val : opt.val_;
}
long JsonValue::ToInt(long default_val) const {
    auto opt = TryToInt();
    return opt == nullptr ? default_val : opt.val_;
}
double JsonValue::ToFloat(double default_val) const {
    auto opt = TryToFloat();
    return opt == nullptr ? default_val : opt.val_;
}
double JsonValue::ToNumber(double default_val) const {
    auto opt = TryToNumber();
    return opt == nullptr ? default_val : opt.val_;
}
std::string JsonValue::ToString(const std::string &default_val) const {
    auto opt = TryToString();
    return opt == nullptr ? default_val : opt.val_;
}

bool JsonValue::HasBool(const std::string &key) const {
    return IsType(Type::Dict) && dict_ != nullptr && dict_->bools_.count(key) > 0;
}
bool JsonValue::HasInt(const std::string &key) const {
    return IsType(Type::Dict) && dict_ != nullptr && dict_->ints_.count(key) > 0;
}
bool JsonValue::HasFloat(const std::string &key) const {
    return IsType(Type::Dict) && dict_ != nullptr && dict_->dbls_.count(key) > 0;
}
bool JsonValue::HasNumber(const std::string &key) const {
    return IsType(Type::Dict) && dict_ != nullptr && (dict_->ints_.count(key) > 0 || dict_->dbls_.count(key) > 0);
}
bool JsonValue::HasString(const std::string &key) const {
    return IsType(Type::Dict) && dict_ != nullptr && dict_->strs_.count(key) > 0;
}
bool JsonValue::HasNull(const std::string &key) const {
    return IsType(Type::Dict) && dict_ != nullptr && dict_->nulls_.count(key) > 0;
}
bool JsonValue::HasDict(const std::string &key) const {
    if (!IsType(Type::Dict) || dict_ == nullptr)
        return false;
    auto obj_it = dict_->objs_.find(key);
    return obj_it != dict_->objs_.end() && obj_it->second->IsType(Type::Dict);
}
bool JsonValue::HasArray(const std::string &key) const {
    if (!IsType(Type::Dict) || dict_ == nullptr)
        return false;
    auto obj_it = dict_->objs_.find(key);
    return obj_it != dict_->objs_.end() && obj_it->second->IsType(Type::List);
}
bool JsonValue::HasDictOrArray(const std::string &key) const {
    return IsType(Type::Dict) && dict_ != nullptr && dict_->objs_.count(key) > 0;
}
bool JsonValue::HasKey(const std::string &key) const {
    if (!IsType(Type::Dict))
        return false;
    return HasBool(key) ||
        HasInt(key) ||
        HasFloat(key) ||
        HasString(key) ||
        HasNull(key) ||
        HasDictOrArray(key);
}
std::vector<std::pair<std::string, JsonType>> JsonValue::GetKeys() const {
    std::vector<std::pair<std::string, JsonType>> ret;
    if (dict_ == nullptr)
        return ret;
    for (auto &kv : dict_->bools_)
        ret.emplace_back(make_pair(kv.first, JsonType::Bool));
    for (auto &kv : dict_->ints_)
        ret.emplace_back(make_pair(kv.first, JsonType::Int));
    for (auto &kv : dict_->dbls_)
        ret.emplace_back(make_pair(kv.first, JsonType::Float));
    for (auto &kv : dict_->strs_)
        ret.emplace_back(make_pair(kv.first, JsonType::Str));
    for (auto &key : dict_->nulls_)
        ret.emplace_back(make_pair(key, JsonType::Null));
    for (auto &obj_pair : dict_->objs_)
        ret.emplace_back(make_pair(obj_pair.first, obj_pair.second->type_));
    return ret;
}
JsonType JsonValue::GetKeyType(const std::string &key) const {
    if (dict_ == nullptr)
        return JsonType::Unknown;
    if (dict_->bools_.count(key) > 0)
        return JsonType::Bool;
    if (dict_->ints_.count(key) > 0)
        return JsonType::Int;
    if (dict_->dbls_.count(key) > 0)
        return JsonType::Float;
    if (dict_->strs_.count(key) > 0)
        return JsonType::Str;
    if (dict_->nulls_.count(key) > 0)
        return JsonType::Null;
    auto obj_it = dict_->objs_.find(key);
    if (obj_it != dict_->objs_.end())
        return obj_it->second->type_;
    return JsonType::Unknown;
}
JsonOption<bool> JsonValue::TryGetBool(const std::string &key) const {
    if (!IsType(Type::Dict) || dict_ == nullptr)
        return nullptr;
    auto bool_it = dict_->bools_.find(key);
    if (bool_it == dict_->bools_.end())
        return nullptr;
    return bool_it->second;
}
JsonOption<long> JsonValue::TryGetInt(const std::string &key) const {
    if (!IsType(Type::Dict) || dict_ == nullptr)
        return nullptr;
    auto int_it = dict_->ints_.find(key);
    if (int_it == dict_->ints_.end())
        return nullptr;
    return int_it->second;
}
JsonOption<double> JsonValue::TryGetFloat(const std::string &key) const {
    if (!IsType(Type::Dict) || dict_ == nullptr)
        return nullptr;
    auto dbl_it = dict_->dbls_.find(key);
    if (dbl_it == dict_->dbls_.end())
        return nullptr;
    return dbl_it->second;
}
JsonOption<double> JsonValue::TryGetNumber(const std::string &key) const {
    if (!IsType(Type::Dict) || dict_ == nullptr)
        return nullptr;
    auto dbl_it = dict_->dbls_.find(key);
    if (dbl_it != dict_->dbls_.end())
        return dbl_it->second;
    auto int_it = dict_->ints_.find(key);
    if (int_it != dict_->ints_.end())
        return int_it->second;
    return nullptr;
}
JsonOption<std::string> JsonValue::TryGetString(const std::string &key) const {
    if (!IsType(Type::Dict) || dict_ == nullptr)
        return nullptr;
    auto str_it = dict_->strs_.find(key);
    if (str_it == dict_->strs_.end())
        return nullptr;
    return str_it->second;
}
bool JsonValue::TryGetBool(const std::string &key, bool &val) const {
    auto opt = TryGetBool(key);
    if (opt != nullptr)
        val = opt.val_;
    return opt != nullptr;
}
bool JsonValue::TryGetInt(const std::string &key, int &val) const {
    auto opt = TryGetInt(key);
    if (opt != nullptr)
        val = opt.val_;
    return opt != nullptr;
}
bool JsonValue::TryGetInt(const std::string &key, long &val) const {
    auto opt = TryGetInt(key);
    if (opt != nullptr)
        val = opt.val_;
    return opt != nullptr;
}
bool JsonValue::TryGetFloat(const std::string &key, double &val) const {
    auto opt = TryGetFloat(key);
    if (opt != nullptr)
        val = opt.val_;
    return opt != nullptr;
}
bool JsonValue::TryGetNumber(const std::string &key, double &val) const {
    auto opt = TryGetNumber(key);
    if (opt != nullptr)
        val = opt.val_;
    return opt != nullptr;
}
bool JsonValue::TryGetString(const std::string &key, std::string &val) const {
    auto opt = TryGetString(key);
    if (opt != nullptr)
        val = opt.val_;
    return opt != nullptr;
}
bool JsonValue::GetBool(const std::string &key, bool default_val) const {
    auto opt = TryGetBool(key);
    return opt == nullptr ? default_val : opt.val_;
}
long JsonValue::GetInt(const std::string &key, long default_val) const {
    auto opt = TryGetInt(key);
    return opt == nullptr ? default_val : opt.val_;
}
double JsonValue::GetFloat(const std::string &key, double default_val) const {
    auto opt = TryGetFloat(key);
    return opt == nullptr ? default_val : opt.val_;
}
double JsonValue::GetNumber(const std::string &key, double default_val) const {
    auto opt = TryGetNumber(key);
    return opt == nullptr ? default_val : opt.val_;
}
std::string JsonValue::GetString(const std::string &key, const std::string &default_val) const {
    auto opt = TryGetString(key);
    return opt == nullptr ? default_val : opt.val_;
}
JsonValue::Ptr JsonValue::GetDict(const std::string &key, const JsonValue::Ptr &default_val) const {
    if (!IsType(Type::Dict) || dict_ == nullptr)
        return default_val;
    auto obj_it = dict_->objs_.find(key);
    if (obj_it == dict_->objs_.end() || !obj_it->second->IsType(Type::Dict))
        return default_val;
    return obj_it->second;
}
JsonValue::Ptr JsonValue::GetArray(const std::string &key, const JsonValue::Ptr &default_val) const {
    if (!IsType(Type::Dict) || dict_ == nullptr)
        return default_val;
    auto obj_it = dict_->objs_.find(key);
    if (obj_it == dict_->objs_.end() || !obj_it->second->IsType(Type::List))
        return default_val;
    return obj_it->second;
}
JsonValue::Ptr JsonValue::GetDictOrArray(const std::string &key, const JsonValue::Ptr &default_val) const {
    if (!IsType(Type::Dict) || dict_ == nullptr)
        return default_val;
    auto obj_it = dict_->objs_.find(key);
    if (obj_it == dict_->objs_.end())
        return default_val;
    return obj_it->second;
}
JsonValue::Ptr JsonValue::GetDictRecurisive(const std::vector<std::string> &keys,
        const JsonValue::Ptr &default_val) const {
    if (!IsType(Type::Dict) || dict_ == nullptr || keys.empty())
        return default_val;
    auto root = std::make_shared<JsonValue>(*this);
    for (auto &key : keys) {
        auto dict = root->GetDict(key, nullptr);
        if (dict == nullptr)
            return default_val;
        root = std::move(dict);
    }
    return root;
}

std::string JsonValue::GetDumpString() const {
    std::stringstream ss;
    DumpToStream(ss);
    return ss.str();
    // switch (type_) {
    //     case kd::dc::JsonUtil::JsonType::Dict:
    //         {
    //             vector<string> lines;
    //             for (auto &val_pair : dict_->bools_)
    //                 lines.emplace_back(StringUtil::Format("%:%",
    //                     { ToJsonString(val_pair.first), val_pair.second ? "true" : "false" }));
    //             for (auto &val_pair : dict_->ints_)
    //                 lines.emplace_back(StringUtil::Format("%:%",
    //                     { ToJsonString(val_pair.first), to_string(val_pair.second) }));
    //             for (auto &val_pair : dict_->dbls_)
    //                 lines.emplace_back(StringUtil::Format("%:%",
    //                     { ToJsonString(val_pair.first), to_string(val_pair.second) }));
    //             for (auto &val_pair : dict_->strs_)
    //                 lines.emplace_back(StringUtil::Format("%:%",
    //                     { ToJsonString(val_pair.first), ToJsonString(val_pair.second) }));
    //             for (auto &val_key : dict_->nulls_)
    //                 lines.emplace_back(StringUtil::Format("%:null", { ToJsonString(val_key) }));
    //             for (auto &val_pair : dict_->objs_)
    //                 lines.emplace_back(StringUtil::Format("%:%",
    //                     { ToJsonString(val_pair.first), val_pair.second->GetDumpString() }));
    //             return StringUtil::Format("{%}", { StringUtil::VecToString(lines, ",") });
    //         }
    //     case kd::dc::JsonUtil::JsonType::List:
    //         {
    //             vector<string> lines;
    //             for (auto &ele : arrs_) {
    //                 auto line = ele->GetDumpString();
    //                 lines.emplace_back(line);
    //             }
    //             return StringUtil::Format("[%]", { StringUtil::VecToString(lines, ",") });
    //         }
    //         break;
    //     case kd::dc::JsonUtil::JsonType::Bool:
    //         return val_l_ == 1 ? "true" : "false";
    //     case kd::dc::JsonUtil::JsonType::Int:
    //         return to_string(val_l_);
    //     case kd::dc::JsonUtil::JsonType::Float:
    //         return to_string(val_d_);
    //     case kd::dc::JsonUtil::JsonType::Str:
    //         return ToJsonString(val_s_);
    //     case kd::dc::JsonUtil::JsonType::Null:
    //         return "null";
    //     case kd::dc::JsonUtil::JsonType::Unknown:
    //         break;
    // }
    // return "";
}

std::ostream& JsonValue::DumpToStream(std::ostream &os) const {
    const int dbl_precision = 20;
    switch (type_) {
        case JsonUtil::JsonType::Dict:
            {
                os << "{";
                bool is_first = true;
                if (dict_ != nullptr) {
                    for (auto &val_pair : dict_->bools_) {
                        if (!is_first)
                            os << ",";
                        is_first = false;
                        os << ToJsonString(val_pair.first) << ":" << (val_pair.second ? "true" : "false");
                    }
                    for (auto &val_pair : dict_->ints_) {
                        if (!is_first)
                            os << ",";
                        is_first = false;
                        os << ToJsonString(val_pair.first) << ":" << val_pair.second;
                    }
                    auto old_precision = os.precision();
                    os << std::setprecision(dbl_precision);
                    for (auto &val_pair : dict_->dbls_) {
                        if (!is_first)
                            os << ",";
                        is_first = false;
                        os << ToJsonString(val_pair.first) << ":" << val_pair.second;
                    }
                    os << std::setprecision(old_precision);
                    for (auto &val_pair : dict_->strs_) {
                        if (!is_first)
                            os << ",";
                        is_first = false;
                        os << ToJsonString(val_pair.first) << ":" << ToJsonString(val_pair.second);
                    }
                    for (auto &val_key : dict_->nulls_) {
                        if (!is_first)
                            os << ",";
                        is_first = false;
                        os << ToJsonString(val_key) << ":null";
                    }
                    for (auto &val_pair : dict_->objs_) {
                        if (!is_first)
                            os << ",";
                        is_first = false;
                        os << ToJsonString(val_pair.first) << ":";
                        val_pair.second->DumpToStream(os);
                    }
                }
                os << "}";
            }
            break;
        case JsonUtil::JsonType::List:
            {
                os << "[";
                bool is_first = true;
                for (auto &ele : arrs_) {
                    if (!is_first)
                        os << ",";
                    is_first = false;
                    ele->DumpToStream(os);
                }
                os << "]";
            }
            break;
        case JsonUtil::JsonType::Bool:
            os << (val_l_ == 1 ? "true" : "false");
            break;
        case JsonUtil::JsonType::Int:
            os << val_l_;
            break;
        case JsonUtil::JsonType::Float:
            {
                auto old_precision = os.precision();
                os << std::setprecision(dbl_precision) << val_d_ << std::setprecision(old_precision);
            }
            break;
        case JsonUtil::JsonType::Str:
            os << ToJsonString(val_s_);
            break;
        case JsonUtil::JsonType::Null:
            os << "null";
            break;
        case JsonUtil::JsonType::Unknown:
            break;
    }
    return os;
}

bool IsVarTypeOk(const Poco::Dynamic::Var &var, const JsonType &type) {
    if (type == JsonType::Dict)
        return var.type() == typeid(Poco::JSON::Object::Ptr);
    else if (type == JsonType::List)
        return var.type() == typeid(Poco::JSON::Array::Ptr);
    else if (type == JsonType::Bool)
        return var.isBoolean();
    else if (type == JsonType::Int)
        return var.isInteger() && !var.isBoolean();
    else if (type == JsonType::Float)
        return var.isSigned() && !var.isInteger();
    else if (type == JsonType::Str)
        return var.isString();
    else if (type == JsonType::Null)
        return var.isEmpty();
    return false;
}


Poco::Dynamic::Var LoadJsonString(const std::string &str) {
    try {
        Poco::JSON::Parser parser;
        return parser.parse(str);
    }
    catch (std::exception &e) {
        // LOG(ERROR) << "parse json failed:" << e.what();
    }
    return nullptr;
}

Poco::Dynamic::Var LoadJsonFile(const std::string &filename) {
    std::ifstream fin(filename, std::ios::in);
    if (!fin) {
        // LOG(ERROR) << "open geojson file '" << filename << "' error";
        return nullptr;
    }
    try {
        Poco::JSON::Parser parser;
        return parser.parse(fin);
    }
    catch (std::exception &e) {
        // LOG(ERROR) << "parse json failed:" << e.what();
    }
    return nullptr;
}

Poco::JSON::Object::Ptr LoadObjJson(const std::string &filename) {
    auto result = LoadJsonFile(filename);
    if (result.type() != typeid (Poco::JSON::Object::Ptr))
        return nullptr;
    return result.extract<Poco::JSON::Object::Ptr>();
}
Poco::JSON::Array::Ptr LoadArrJson(const std::string &filename) {
    auto result = LoadJsonFile(filename);
    if (result.type() != typeid (Poco::JSON::Array::Ptr))
        return nullptr;
    return result.extract<Poco::JSON::Array::Ptr>();
}


std::string ToJsonString(const std::string &str) {
    auto val = str;
    StringUtil::Replace(val, "\\", "\\\\");
    StringUtil::Replace(val, "\"", "\\\"");
    StringUtil::Replace(val, "\n", "\\n");
    StringUtil::Replace(val, "\r", "\\r");
    StringUtil::Replace(val, "\t", "\\t");
    return StringUtil::Format("\"%\"", { val });
}

JsonValue::Ptr LoadJsonBoolean(const std::string &key, const Poco::Dynamic::Var &val) {
    if (!val.isBoolean())
        return nullptr;
    return std::make_shared<JsonValue>(val.extract<bool>());
}
JsonValue::Ptr LoadJsonInterger(const std::string &key, const Poco::Dynamic::Var &val) {
    if (!val.isInteger() || val.isBoolean())
        return nullptr;
    return std::make_shared<JsonValue>(val.extract<long>());
}
JsonValue::Ptr LoadJsonDouble(const std::string &key, const Poco::Dynamic::Var &val) {
    if (!val.isSigned() || val.isInteger())
        return nullptr;
    return std::make_shared<JsonValue>(val.extract<double>());
}
JsonValue::Ptr LoadJsonString(const std::string &key, const Poco::Dynamic::Var &val) {
    if (!val.isString())
        return nullptr;
    return std::make_shared<JsonValue>(val.extract<std::string>());
}
JsonValue::Ptr LoadJsonDict(const std::string &key, const Poco::Dynamic::Var &val) {
    if (val.type() != typeid(Poco::JSON::Object::Ptr))
        return nullptr;
    auto obj = val.extract<Poco::JSON::Object::Ptr>();

    auto json_val = std::make_shared<JsonValue>();
    for (auto &key_pair : *obj) {
        auto &key_name = key_pair.first;
        auto val = LoadJsonValue(key_name, obj->get(key_name));
        if (val != nullptr)
            json_val->Insert(key_name, val);
    }
    return json_val;
}
JsonValue::Ptr LoadJsonList(const std::string &key, const Poco::Dynamic::Var &val) {
    if (val.type() != typeid(Poco::JSON::Array::Ptr))
        return nullptr;
    auto arr = val.extract<Poco::JSON::Array::Ptr>();

    auto json_val = std::make_shared<JsonValue>();
    json_val->type_ = JsonType::List;
    int ele_idx = 0;
    for (auto &ele : *arr) {
        auto obj = LoadJsonValue(key, ele);
        if (obj != nullptr)
            json_val->PushBack(obj);
        ++ele_idx;
    }
    return json_val;
}
JsonValue::Ptr LoadJsonValue(const std::string &key, const Poco::Dynamic::Var &val) {
    auto bval = LoadJsonBoolean(key, val);
    if (bval != nullptr)
        return bval;
    auto ival = LoadJsonInterger(key, val);
    if (ival != nullptr)
        return ival;
    auto dval = LoadJsonDouble(key, val);
    if (dval != nullptr)
        return dval;
    auto sval = LoadJsonString(key, val);
    if (sval != nullptr)
        return sval;
    auto list_val = LoadJsonList(key, val);
    if (list_val != nullptr)
        return list_val;
    auto obj_val = LoadJsonDict(key, val);
    if (obj_val != nullptr)
        return obj_val;
    return nullptr;
}
JsonValue::Ptr LoadJsonValue(const std::string &filename) {
    auto result = LoadJsonFile(filename);
    return LoadJsonValue("", result);
}
JsonValue::Ptr LoadJsonValueString(const std::string &str) {
    auto result = LoadJsonString(str);
    return LoadJsonValue("", result);
}
extern JsonValue::Ptr LoadJsonValueString(const std::string &str);

} /* namespace JsonUtil */
} /* namespace sn */
