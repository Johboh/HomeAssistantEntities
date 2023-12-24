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

}; // namespace homeassistantentities

#endif // __HA_UTILITIES_H__