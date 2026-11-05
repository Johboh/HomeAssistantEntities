// Copyright (c) Johan Böhlin
// SPDX-License-Identifier: MIT

#include "AttributeVariants.h"
#include <type_traits>

namespace Attributes {

void addKeyValue(IJsonDocument &doc, const std::string &key, const Attributes::Variants &value) {
  std::visit(
      [&doc, &key](const auto &val) {
        using T = std::decay_t<decltype(val)>;
        if constexpr (std::is_same_v<T, Attributes::InnerSet>) {
          JsonArrayType array = createJsonArray(doc, key);
          for (const auto &elem : val) {
            addToJsonArray(array, elem);
          }
        } else {
          doc[key] = val;
        }
      },
      value);
}

bool toJson(IJsonDocument &doc, const Attributes::Map &attributes, const std::set<std::string> &forbidden_keys) {
  auto size_before = doc.size();
  for (const auto &attribute : attributes) {
    const auto &key = attribute.first;

    if (forbidden_keys.find(key) != forbidden_keys.end()) {
      continue;
    }

    addKeyValue(doc, key, attribute.second);
  }
  return doc.size() > size_before;
}

} // namespace Attributes
