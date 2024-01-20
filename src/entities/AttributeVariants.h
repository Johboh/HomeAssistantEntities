#ifndef __ATTRIBUTE_VARIANTS_H__
#define __ATTRIBUTE_VARIANTS_H__

#include <map>
#include <nlohmann/json.hpp>
#include <set>
#include <string>
#include <variant>

namespace Attributes {
typedef std::variant<int, float, double, bool, std::string, const char *> Variants;
typedef std::map<std::string, Attributes::Variants> Map;

bool toJson(nlohmann::json &json, Attributes::Map attributes, std::set<std::string> forbidden_keys = {});

}; // namespace Attributes

#endif // __ATTRIBUTE_VARIANTS_H__