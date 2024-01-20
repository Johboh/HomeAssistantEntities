#include "AttributeVariants.h"

bool Attributes::toJson(nlohmann::json &doc, Attributes::Map attributes, std::set<std::string> forbidden_keys) {
  // Add known attributes.
  auto size_before = doc.size();
  for (const auto &attribute : attributes) {
    // Keys with name "event_type" is not allowed.
    auto key = attribute.first;

    if (forbidden_keys.find(key) != forbidden_keys.end()) {
      continue;
    }

    auto value = attribute.second;
    if (std::holds_alternative<double>(value)) {
      doc[key] = std::get<double>(value);
    } else if (std::holds_alternative<float>(value)) {
      doc[key] = std::get<float>(value);
    } else if (std::holds_alternative<bool>(value)) {
      doc[key] = std::get<bool>(value);
    } else if (std::holds_alternative<int>(value)) {
      doc[key] = std::get<int>(value);
    } else if (std::holds_alternative<std::string>(value)) {
      doc[key] = std::get<std::string>(value);
    } else if (std::holds_alternative<const char *>(value)) {
      doc[key] = std::get<const char *>(value);
    }
  }
  return doc.size() > size_before;
}