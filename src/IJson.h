// Copyright (c) Johan Böhlin
// SPDX-License-Identifier: MIT

#ifndef __IJSON_H__
#define __IJSON_H__

#include <string>
#include <vector>

/**
 * This is a compatibility layer for nlohmann-json and ArduinoJson, to allow the library to use either of them.
 * Any other JSON library can be added here as well.
 */

#if __has_include("nlohmann/json.hpp")
#include <nlohmann/json.hpp>

#define IJsonDocument nlohmann::json

#define IJsonIteratorBegin(doc) doc.items()

#define IJsonConstIteratorBegin(doc) doc.items()

#define toJsonString(doc) doc.dump()

#define JsonArrayType auto &

#define JsonObjectType auto &

#define createJsonArray(doc, name) (doc[name] = nlohmann::json::array())

#define createJsonObject(doc, name) (doc[name] = nlohmann::json::object())

#define addToJsonArray(doc, value) doc.push_back(value)

#define addObjectToJsonArray(arr) (arr.push_back(nlohmann::json::object()), arr.back())

namespace homeassistantentities {
inline void extractJsonStrings(const nlohmann::json &val, std::vector<std::string> &out) {
  if (val.is_string()) {
    out.push_back(val.get<std::string>());
  } else if (val.is_array()) {
    for (const auto &elem : val) {
      if (elem.is_string()) {
        out.push_back(elem.get<std::string>());
      }
    }
  }
}

inline bool jsonHasNonEmptyArrayOrString(const nlohmann::json &val) {
  if (val.is_string()) {
    return !val.get<std::string>().empty();
  }
  if (val.is_array()) {
    return !val.empty();
  }
  return false;
}
} // namespace homeassistantentities

#elif __has_include("ArduinoJson.h")
#include <ArduinoJson.h>

#define IJsonDocument JsonDocument

#define IJsonIteratorBegin(doc) doc.as<JsonObject>()

#define IJsonConstIteratorBegin(doc) doc.as<JsonObjectConst>()

#define toJsonString(doc) doc.as<std::string>()

#define JsonArrayType auto

#define JsonObjectType auto

#define createJsonArray(doc, name) doc[name].to<JsonArray>()

#define createJsonObject(doc, name) homeassistantentities::haCreateJsonObject(doc, name)

#define addToJsonArray(doc, value) doc.add(value)

#define addObjectToJsonArray(arr) arr.add<JsonObject>()

namespace homeassistantentities {
template <typename Doc, typename Key> inline JsonObject haCreateJsonObject(Doc &&doc, const Key &name) {
  return doc[name].template to<JsonObject>();
}
inline void extractJsonStrings(JsonVariantConst val, std::vector<std::string> &out) {
  if (val.is<const char *>()) {
    const char *s = val.as<const char *>();
    if (s != nullptr) {
      out.emplace_back(s);
    }
  } else if (val.is<JsonArrayConst>()) {
    for (JsonVariantConst elem : val.as<JsonArrayConst>()) {
      if (elem.is<const char *>()) {
        const char *s = elem.as<const char *>();
        if (s != nullptr) {
          out.emplace_back(s);
        }
      }
    }
  }
}

inline bool jsonHasNonEmptyArrayOrString(JsonVariantConst val) {
  if (val.is<const char *>()) {
    const char *s = val.as<const char *>();
    return s != nullptr && s[0] != '\0';
  }
  if (val.is<JsonArrayConst>()) {
    return val.as<JsonArrayConst>().size() > 0;
  }
  return false;
}
} // namespace homeassistantentities

#else
#error                                                                                                                 \
    "No JSON library found. You need to install either https://github.com/Johboh/nlohmann-json OR https://github.com/bblanchon/ArduinoJson, see README.md"
#endif

#endif // __IJSON_H__
