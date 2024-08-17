#ifndef __IJSON_H__
#define __IJSON_H__

#include <string>

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

#define createJsonArray(doc, name) doc[name]

#define addToJsonArray(doc, value) doc.push_back(value)

#elif __has_include("ArduinoJson.h")
#include <ArduinoJson.h>

#define IJsonDocument JsonDocument

#define IJsonIteratorBegin(doc) doc.as<JsonObject>()

#define IJsonConstIteratorBegin(doc) doc.as<JsonObjectConst>()

#define toJsonString(doc) doc.as<std::string>()

#define JsonArrayType auto

#define createJsonArray(doc, name) doc[name].to<JsonArray>()

#define addToJsonArray(doc, value) doc.add(value)

#else
#error                                                                                                                 \
    "No JSON library found. You need to install either https://github.com/Johboh/nlohmann-json OR https://github.com/bblanchon/ArduinoJson, see README.md"
#endif

#endif // __IJSON_H__