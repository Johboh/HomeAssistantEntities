#include "AttributeVariants.h"

namespace Attributes {

void addValue(IJsonDocument &doc, std::string key, Attributes::Variants value) {
  if (std::holds_alternative<double>(value)) {
    doc[key] = std::get<double>(value);
  } else if (std::holds_alternative<float>(value)) {
    doc[key] = std::get<float>(value);
  } else if (std::holds_alternative<bool>(value)) {
    doc[key] = std::get<bool>(value);
  } else if (std::holds_alternative<uint64_t>(value)) {
    doc[key] = std::get<uint64_t>(value);
  } else if (std::holds_alternative<uint32_t>(value)) {
    doc[key] = std::get<uint32_t>(value);
  } else if (std::holds_alternative<uint16_t>(value)) {
    doc[key] = std::get<uint16_t>(value);
  } else if (std::holds_alternative<uint8_t>(value)) {
    doc[key] = std::get<uint8_t>(value);
  } else if (std::holds_alternative<int>(value)) {
    doc[key] = std::get<int>(value);
  } else if (std::holds_alternative<std::string>(value)) {
    doc[key] = std::get<std::string>(value);
  } else if (std::holds_alternative<const char *>(value)) {
    doc[key] = std::get<const char *>(value);
  }
}

void addKeyValue(IJsonDocument &doc, std::string key, Attributes::Variants value) {
  if (std::holds_alternative<Attributes::InnerSet>(value)) {
    auto set = std::get<Attributes::InnerSet>(value);
    JsonArrayType array = createJsonArray(doc, key);
    for (const auto &value : set) {
      IJsonDocument temp_doc;
      addValue(temp_doc, "temp", value);
      addToJsonArray(array, temp_doc["temp"]);
    }
  } else {
    Attributes::addValue(doc, key, value);
  }
}

bool toJson(IJsonDocument &doc, Attributes::Map attributes, std::set<std::string> forbidden_keys) {
  // Add known attributes.
  auto size_before = doc.size();
  for (const auto &attribute : attributes) {
    // Keys with name "event_type" is not allowed.
    auto key = attribute.first;

    if (forbidden_keys.find(key) != forbidden_keys.end()) {
      continue;
    }

    auto value = attribute.second;
    addKeyValue(doc, key, value);
  }
  return doc.size() > size_before;
}

} // namespace Attributes
