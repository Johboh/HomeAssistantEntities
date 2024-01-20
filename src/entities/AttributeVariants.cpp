#include "AttributeVariants.h"

namespace Attributes {

void addValue(nlohmann::json &doc, Attributes::Variants value) {
  if (std::holds_alternative<double>(value)) {
    doc = std::get<double>(value);
  } else if (std::holds_alternative<float>(value)) {
    doc = std::get<float>(value);
  } else if (std::holds_alternative<bool>(value)) {
    doc = std::get<bool>(value);
  } else if (std::holds_alternative<int>(value)) {
    doc = std::get<int>(value);
  } else if (std::holds_alternative<std::string>(value)) {
    doc = std::get<std::string>(value);
  } else if (std::holds_alternative<const char *>(value)) {
    doc = std::get<const char *>(value);
  }
}

void addKeyValue(nlohmann::json &doc, std::string key, Attributes::Variants value) {
  if (std::holds_alternative<Attributes::InnerSet>(value)) {
    auto set = std::get<Attributes::InnerSet>(value);
    // Recursive call, but only one level deeper.
    for (const auto &item : set) {
      doc[key].push_back("temp");
      Attributes::addValue(doc[key][doc[key].size() - 1], item);
    }
  } else {
    Attributes::addValue(doc[key], value);
  }
}

bool toJson(nlohmann::json &doc, Attributes::Map attributes, std::set<std::string> forbidden_keys) {
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
