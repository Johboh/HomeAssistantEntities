// Copyright (c) Johan Böhlin
// SPDX-License-Identifier: MIT

#include "HaDeviceInformation.h"
#include <HaUtilities.h>
#include <cstdio>

#if defined(ESP8266) || defined(ARDUINO_ARCH_ESP8266)
#define HA_PLATFORM_ESP8266 1
#include <Arduino.h>
#if __has_include(<ESP8266WiFi.h>)
#include <ESP8266WiFi.h>
#define HA_HAS_ESP8266_WIFI 1
#else
#include <user_interface.h>
#endif
#elif defined(ESP_PLATFORM) || defined(ESP32) || defined(ARDUINO_ARCH_ESP32)
#define HA_PLATFORM_ESP32 1
#ifdef ARDUINO
#include <Arduino.h>
#endif
#include <esp_chip_info.h>
#include <esp_idf_version.h>
#include <esp_mac.h>
#include <esp_netif.h>
#if __has_include(<esp_app_desc.h>)
#include <esp_app_desc.h>
#define HA_HAS_ESP_APP_GET_DESC 1
#elif __has_include(<esp_ota_ops.h>)
#include <esp_ota_ops.h>
#define HA_HAS_ESP_OTA_GET_DESC 1
#endif
#endif

namespace HaDeviceInformation {

namespace {
#if defined(HA_PLATFORM_ESP32)
const esp_app_desc_t *getEspAppDesc() {
#if defined(HA_HAS_ESP_APP_GET_DESC)
  return esp_app_get_description();
#elif defined(HA_HAS_ESP_OTA_GET_DESC)
  return esp_ota_get_app_description();
#else
  return nullptr;
#endif
}
#endif

std::string stripColons(const std::string &mac) {
  std::string out;
  out.reserve(mac.size());
  for (char c : mac) {
    if (c != ':') {
      out.push_back(c);
    }
  }
  return out;
}
} // namespace

std::string getChipModel() {
#if defined(HA_PLATFORM_ESP32)
  esp_chip_info_t chip_info{};
  esp_chip_info(&chip_info);
  switch (chip_info.model) {
  case CHIP_ESP32:
    return "ESP32";
  case CHIP_ESP32S2:
    return "ESP32-S2";
  case CHIP_ESP32S3:
    return "ESP32-S3";
  case CHIP_ESP32C3:
    return "ESP32-C3";
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 0, 0)
  case CHIP_ESP32C2:
    return "ESP32-C2";
  case CHIP_ESP32C6:
    return "ESP32-C6";
  case CHIP_ESP32H2:
    return "ESP32-H2";
#endif
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 3, 0)
  case CHIP_ESP32P4:
    return "ESP32-P4";
  case CHIP_ESP32C61:
    return "ESP32-C61";
#endif
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 4, 0)
  case CHIP_ESP32C5:
    return "ESP32-C5";
#endif
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 5, 0)
  case CHIP_ESP32H21:
    return "ESP32-H21";
  case CHIP_ESP32H4:
    return "ESP32-H4";
#endif
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(6, 1, 0)
  case CHIP_ESP32S31:
    return "ESP32-S31";
#endif
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 4, 0)
  case CHIP_POSIX_LINUX:
    return "linux emulator";
#endif
  default:
#ifdef ARDUINO
    return std::string(ESP.getChipModel());
#else
    return "ESP32";
#endif
  }
#elif defined(HA_PLATFORM_ESP8266)
  return "ESP8266";
#else
  return "";
#endif
}

std::string getHardwareVersion() {
#if defined(HA_PLATFORM_ESP32)
  esp_chip_info_t chip_info{};
  esp_chip_info(&chip_info);
  char rev_str[16];
  int major = chip_info.revision / 100;
  int minor = chip_info.revision % 100;
  std::snprintf(rev_str, sizeof(rev_str), "v%d.%d", major, minor);
  std::string chip = getChipModel();
  return chip.empty() ? std::string(rev_str) : (chip + " revision " + rev_str);
#elif defined(HA_PLATFORM_ESP8266)
  char buf[32];
  std::snprintf(buf, sizeof(buf), "ESP8266 (0x%06x)", static_cast<unsigned int>(ESP.getChipId()));
  return std::string(buf);
#else
  return "";
#endif
}

std::string getAppVersion() {
#if defined(HA_PLATFORM_ESP32)
  const auto *desc = getEspAppDesc();
  if (desc != nullptr && desc->version[0] != '\0') {
    return homeassistantentities::trim(std::string(desc->version));
  }
#endif
  return "";
}

std::string getFirmwareVersion() {
#if defined(HA_PLATFORM_ESP32)
  const auto *desc = getEspAppDesc();
  if (desc != nullptr) {
    std::string out = homeassistantentities::trim(std::string(desc->version));
    std::string date = homeassistantentities::trim(std::string(desc->date));
    std::string time = homeassistantentities::trim(std::string(desc->time));
    std::string idf = homeassistantentities::trim(std::string(desc->idf_ver));
    for (const auto &part : {date, time, idf}) {
      if (!part.empty()) {
        if (!out.empty()) {
          out += " ";
        }
        out += part;
      }
    }
    if (!out.empty()) {
      return out;
    }
  }
#ifdef ARDUINO
  return std::string(ESP.getSdkVersion());
#else
  return std::string(esp_get_idf_version());
#endif
#elif defined(HA_PLATFORM_ESP8266)
  std::string core = ESP.getCoreVersion().c_str();
  const char *sdk = ESP.getSdkVersion();
  if (!core.empty() && sdk != nullptr && sdk[0] != '\0') {
    return "Core " + core + " / SDK " + std::string(sdk);
  }
  return sdk != nullptr ? std::string(sdk) : core;
#else
  return "";
#endif
}

std::string getProjectName() {
#if defined(HA_PLATFORM_ESP32)
  const auto *desc = getEspAppDesc();
  if (desc != nullptr && desc->project_name[0] != '\0') {
    return homeassistantentities::trim(std::string(desc->project_name));
  }
#endif
  return "";
}

std::string getMacAddress() {
#if defined(HA_PLATFORM_ESP32)
  uint8_t mac[6] = {0};
  bool ok = false;
#ifdef ESP_MAC_WIFI_STA
  esp_chip_info_t chip_info{};
  esp_chip_info(&chip_info);
  if ((chip_info.features & CHIP_FEATURE_WIFI_BGN) != 0 && esp_read_mac(mac, ESP_MAC_WIFI_STA) == ESP_OK) {
    ok = true;
  }
#endif
  if (!ok && esp_efuse_mac_get_default(mac) == ESP_OK) {
    ok = true;
  }
  if (!ok) {
    return "";
  }
  char mac_str[18];
  std::snprintf(mac_str, sizeof(mac_str), "%02x:%02x:%02x:%02x:%02x:%02x", mac[0], mac[1], mac[2], mac[3], mac[4],
                mac[5]);
  return std::string(mac_str);
#elif defined(HA_PLATFORM_ESP8266)
  uint8_t mac[6] = {0};
#if defined(HA_HAS_ESP8266_WIFI)
  WiFi.macAddress(mac);
#else
  if (!wifi_get_macaddr(STATION_IF, mac)) {
    return "";
  }
#endif
  char mac_str[18];
  std::snprintf(mac_str, sizeof(mac_str), "%02x:%02x:%02x:%02x:%02x:%02x", mac[0], mac[1], mac[2], mac[3], mac[4],
                mac[5]);
  return std::string(mac_str);
#else
  return "";
#endif
}

std::string getLocalIpAddress() {
#if defined(HA_PLATFORM_ESP32)
  esp_netif_ip_info_t ip_info{};
  esp_netif_t *netif = esp_netif_get_handle_from_ifkey("WIFI_STA_DEF");
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 0, 0)
  if (netif == nullptr) {
    netif = esp_netif_get_default_netif();
  }
#endif
  if (netif == nullptr) {
    netif = esp_netif_get_handle_from_ifkey("ETH_DEF");
  }
  if (netif != nullptr && esp_netif_get_ip_info(netif, &ip_info) == ESP_OK && ip_info.ip.addr != 0) {
    char ip_str[16];
    std::snprintf(ip_str, sizeof(ip_str), IPSTR, IP2STR(&ip_info.ip));
    return std::string(ip_str);
  }
  return "";
#elif defined(HA_PLATFORM_ESP8266)
#if defined(HA_HAS_ESP8266_WIFI)
  IPAddress ip = WiFi.localIP();
  if (ip[0] == 0 && ip[1] == 0 && ip[2] == 0 && ip[3] == 0) {
    return "";
  }
  return std::string(ip.toString().c_str());
#else
  struct ip_info info = {};
  if (wifi_get_ip_info(STATION_IF, &info) && info.ip.addr != 0) {
    uint32_t addr = info.ip.addr;
    char ip_str[16];
    std::snprintf(ip_str, sizeof(ip_str), "%u.%u.%u.%u", static_cast<unsigned int>(addr & 0xFF),
                  static_cast<unsigned int>((addr >> 8) & 0xFF), static_cast<unsigned int>((addr >> 16) & 0xFF),
                  static_cast<unsigned int>((addr >> 24) & 0xFF));
    return std::string(ip_str);
  }
  return "";
#endif
#else
  return "";
#endif
}

HaBridge::DeviceInfo detectDeviceInfo(const std::string &name, const std::string &identifier, const Options &options) {
  HaBridge::DeviceInfo info;
  info.name = homeassistantentities::trim(name);

  std::string mac = getMacAddress();
  std::string clean_id = homeassistantentities::trim(identifier);
  if (!clean_id.empty()) {
    info.identifiers.push_back(clean_id);
  } else if (!mac.empty()) {
    info.identifiers.push_back("esp_" + stripColons(mac));
  }

  std::string mfg = homeassistantentities::trim(options.manufacturer);
  if (!mfg.empty()) {
    info.manufacturer = mfg;
  } else {
#if defined(HA_PLATFORM_ESP32) || defined(HA_PLATFORM_ESP8266)
    info.manufacturer = "Espressif";
#endif
  }

  info.model = getChipModel();
  info.model_id = getProjectName();
  info.sw_version = getFirmwareVersion();
  info.hw_version = getHardwareVersion();
  info.suggested_area = homeassistantentities::trim(options.suggested_area);
  info.via_device = homeassistantentities::trim(options.via_device);

  if (!mac.empty()) {
    info.connections.emplace_back("mac", mac);
    if (options.include_serial_number) {
      info.serial_number = stripColons(mac);
    }
  }

  std::string explicit_url = homeassistantentities::trim(options.configuration_url);
  if (!explicit_url.empty()) {
    info.configuration_url = explicit_url;
  } else if (options.configuration_http_port) {
    std::string ip = getLocalIpAddress();
    if (!ip.empty()) {
      uint16_t port = *options.configuration_http_port;
      info.configuration_url = (port == 80) ? ("http://" + ip) : ("http://" + ip + ":" + std::to_string(port));
    }
  }

  return info;
}

void populateDeviceJson(IJsonDocument &doc, const std::string &name, const std::string &identifier,
                        const Options &options) {
  HaBridge::populateDeviceJson(doc, detectDeviceInfo(name, identifier, options));
}

HaBridge::OriginInfo detectOriginInfo(const std::string &fallback_name, const std::string &support_url) {
  std::string proj = getProjectName();
  std::string name = !proj.empty() ? proj : homeassistantentities::trim(fallback_name);
  if (name.empty()) {
    name = "HomeAssistantEntities";
  }
  return HaBridge::OriginInfo{std::move(name), getAppVersion(), homeassistantentities::trim(support_url)};
}

} // namespace HaDeviceInformation
