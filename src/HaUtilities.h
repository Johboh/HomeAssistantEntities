// Copyright (c) Johan Böhlin
// SPDX-License-Identifier: MIT

#ifndef __HA_UTILITIES_H__
#define __HA_UTILITIES_H__

#include <algorithm>
#include <cctype>
#include <cstdio>
#include <iterator>
#include <string>

namespace homeassistantentities {

inline std::string trim(const std::string &str) {
  auto first = std::find_if_not(str.begin(), str.end(), [](unsigned char c) { return std::isspace(c); });
  auto last = std::find_if_not(str.rbegin(), std::make_reverse_iterator(first), [](unsigned char c) {
                return std::isspace(c);
              }).base();
  return std::string(first, last);
}

/**
 * @brief Given a string that is supposed to be in an MQTT path, return a valid path. Only [a-zA-Z0-9_-] are allowed.
 * Everything else is replaced by _.
 */
inline std::string sanitizePath(const std::string &str) {
  std::string result;
  result.reserve(str.size());
  for (unsigned char c : str) {
    if (std::isalnum(c) || c == '-' || c == '_') {
      result += static_cast<char>(c);
    } else {
      result += '_';
    }
  }
  return result;
}

inline std::string santitizePath(const std::string &str) { return sanitizePath(str); }

inline std::string formatNumber(double value) {
  if (value == 0.0) {
    value = 0.0; // Avoid "-0"
  }
  char buf[32];
  std::snprintf(buf, sizeof(buf), "%.9g", value);
  return std::string(buf);
}

inline std::string formatNumber(float value) {
  if (value == 0.0f) {
    value = 0.0f; // Avoid "-0"
  }
  char buf[24];
  std::snprintf(buf, sizeof(buf), "%.7g", static_cast<double>(value));
  return std::string(buf);
}

/**
 * @brief Count UTF-8 code points in a string (matching Home Assistant's character-length validation).
 */
inline std::size_t utf8CodePointCount(const std::string &str) {
  std::size_t count = 0;
  for (unsigned char c : str) {
    if ((c & 0xC0) != 0x80) {
      ++count;
    }
  }
  return count;
}

}; // namespace homeassistantentities

#endif // __HA_UTILITIES_H__
