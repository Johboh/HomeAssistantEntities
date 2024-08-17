#ifndef __ATTRIBUTE_VARIANTS_H__
#define __ATTRIBUTE_VARIANTS_H__

#include <IJson.h>
#include <map>
#include <set>
#include <string>
#include <variant>

namespace Attributes {
using InnerSet = std::set<std::string>;
using Variants =
    std::variant<uint64_t, uint32_t, uint16_t, uint8_t, int, float, double, bool, std::string, const char *, InnerSet>;
using Map = std::map<std::string, Variants>;

bool toJson(IJsonDocument &doc, Attributes::Map attributes, std::set<std::string> forbidden_keys = {});

}; // namespace Attributes

#endif // __ATTRIBUTE_VARIANTS_H__