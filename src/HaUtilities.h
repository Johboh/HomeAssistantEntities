#ifndef __HA_UTILITIES_H__
#define __HA_UTILITIES_H__

#include <algorithm>
#include <cctype>
#include <string>

namespace homeassistantentities {

inline std::string trim(const std::string &str) {
  auto first = std::find_if_not(str.begin(), str.end(), [](int c) { return std::isspace(c); });
  auto last = std::find_if_not(str.rbegin(), str.rend(), [](int c) { return std::isspace(c); }).base();
  return (first == last ? std::string() : std::string(first, last));
}

/**
 * @brief Given a string that is supposed to be in an MQTT path, return a valid path. Only [a-zA-Z0-9_-] are allowed.
 * Everyting else is replaced by _.
 */
inline std::string santitizePath(const std::string &str) {
  std::string result;
  for (char c : str) {
    if (std::isalnum(c) || c == '-' || c == '_') {
      result += c;
    } else {
      result += '_';
    }
  }
  return result;
}

}; // namespace homeassistantentities

#endif // __HA_UTILITIES_H__