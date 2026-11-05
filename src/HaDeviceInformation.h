// Copyright (c) Johan Böhlin
// SPDX-License-Identifier: MIT

#ifndef __HA_DEVICE_INFORMATION_H__
#define __HA_DEVICE_INFORMATION_H__

#include <HaBridge.h>
#include <IJson.h>
#include <cstdint>
#include <optional>
#include <string>

/**
 * @brief Platform helpers for extracting hardware, firmware, MAC, and IP information (via ESP-IDF / Arduino ESP32 /
 * ESP8266 APIs when available) and populating Home Assistant `device` and `origin` discovery metadata.
 */
namespace HaDeviceInformation {

struct Options {
  /**
   * @brief Manufacturer string (defaults to "Espressif" on ESP32/ESP8266 targets when left empty).
   */
  std::string manufacturer = "";

  /**
   * @brief Optional HTTP port used to construct `configuration_url` (`http://<local_ip>` for port 80, or
   * `http://<local_ip>:<port>` otherwise) when a valid local IPv4 address is available on the device.
   * Leave as `std::nullopt` if the device does not expose a web interface.
   */
  std::optional<uint16_t> configuration_http_port = std::nullopt;

  /**
   * @brief Explicit `configuration_url` override (takes precedence over `configuration_http_port`).
   */
  std::string configuration_url = "";

  /**
   * @brief Optional Home Assistant `suggested_area` (e.g. "Living Room").
   */
  std::string suggested_area = "";

  /**
   * @brief Optional `via_device` identifier when routed through a hub/gateway.
   */
  std::string via_device = "";

  /**
   * @brief If true, populate `serial_number` from the factory MAC / chip ID when available.
   */
  bool include_serial_number = false;
};

/**
 * @brief Return the detected hardware chip/model name (e.g. "ESP32-C6", "ESP32-S3", "ESP8266"), or "" on host.
 */
std::string getChipModel();

/**
 * @brief Return the detected hardware version string (e.g. "ESP32-C6 revision v0.1", "ESP8266 (0x12ab34)"), or "" on
 * host.
 */
std::string getHardwareVersion();

/**
 * @brief Return the full detected firmware/SDK version string (from `esp_app_get_description()` on ESP-IDF / ESP32, or
 * Core/SDK version on ESP8266), or "" on host.
 */
std::string getFirmwareVersion();

/**
 * @brief Return the application version string only (`app_description->version` on ESP-IDF / ESP32), or "" if
 * unavailable.
 */
std::string getAppVersion();

/**
 * @brief Return the firmware project name (`app_description->project_name` on ESP-IDF / ESP32), or "" if unavailable.
 */
std::string getProjectName();

/**
 * @brief Return the primary network/factory MAC address formatted as lowercase "aa:bb:cc:dd:ee:ff", or "" if
 * unavailable.
 */
std::string getMacAddress();

/**
 * @brief Return the current local IPv4 address (e.g. "192.168.1.42"), or "" if disconnected / 0.0.0.0.
 */
std::string getLocalIpAddress();

/**
 * @brief Build a `HaBridge::DeviceInfo` populated from platform APIs (ESP-IDF / Arduino ESP32 / ESP8266) where
 * available.
 *
 * @param name Human-readable device name in Home Assistant (e.g. "Livingroom").
 * @param identifier Primary device identifier. If left empty, falls back to `"esp_<mac_hex>"` when a MAC address is
 * available.
 * @param options Optional settings/overrides.
 */
HaBridge::DeviceInfo detectDeviceInfo(const std::string &name, const std::string &identifier = "",
                                      const Options &options = {});

/**
 * @brief Populate an `IJsonDocument` with Home Assistant `device` metadata extracted from platform APIs.
 */
void populateDeviceJson(IJsonDocument &doc, const std::string &name, const std::string &identifier = "",
                        const Options &options = {});

/**
 * @brief Overload matching `(name, identifier, doc, options)` parameter order.
 */
inline void populateDeviceJson(const std::string &name, const std::string &identifier, IJsonDocument &doc,
                               const Options &options = {}) {
  populateDeviceJson(doc, name, identifier, options);
}

/**
 * @brief Build a `HaBridge::OriginInfo` populated from firmware project name and app version when available.
 */
HaBridge::OriginInfo detectOriginInfo(const std::string &fallback_name = "HomeAssistantEntities",
                                      const std::string &support_url = "");

} // namespace HaDeviceInformation

#endif // __HA_DEVICE_INFORMATION_H__
