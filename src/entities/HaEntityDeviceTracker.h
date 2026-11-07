// Copyright (c) Johan Böhlin
// SPDX-License-Identifier: MIT

#ifndef __HA_ENTITY_DEVICE_TRACKER_H__
#define __HA_ENTITY_DEVICE_TRACKER_H__

#include "AttributeVariants.h"
#include <HaBridge.h>
#include <HaEntity.h>
#include <cstdint>
#include <optional>
#include <string>

/**
 * @brief Represents an MQTT Device Tracker entity (`device_tracker.mqtt`) for presence and location tracking.
 *
 * Supports zone-based presence (`home` / `not_home`) as well as geographic coordinates (latitude, longitude, GPS
 * accuracy, and battery level).
 *
 * See https://www.home-assistant.io/integrations/device_tracker.mqtt/
 */
class HaEntityDeviceTracker : public HaEntity {
public:
  enum class SourceType {
    GPS,
    Router,
    Bluetooth,
    BluetoothLE,
  };

  static std::string sourceTypeToString(SourceType type);

  struct Configuration {
    /**
     * @brief Source type of the tracking information (default `SourceType::GPS`).
     */
    SourceType source_type = SourceType::GPS;

    /**
     * @brief Payload representing the home state (default `"home"`).
     */
    std::string payload_home = "home";

    /**
     * @brief Payload representing the away state (default `"not_home"`).
     */
    std::string payload_not_home = "not_home";

    /**
     * @brief If `true`, configure `json_attributes_topic` for publishing custom JSON attributes.
     */
    bool with_attributes = true;

    /**
     * @brief If `true`, publishes messages with the MQTT retain flag set.
     */
    bool retain = false;

    /**
     * @brief Optional custom icon (e.g. `"mdi:crosshairs-gps"`).
     */
    std::string icon = "";

    /**
     * @brief Optional entity category (`"config"` or `"diagnostic"`).
     */
    std::string entity_category = "";

    /**
     * @brief Defines whether the entity should be enabled when first added to Home Assistant.
     */
    std::optional<bool> enabled_by_default = std::nullopt;
  };

  inline static Configuration _default = {.source_type = SourceType::GPS,
                                          .payload_home = "home",
                                          .payload_not_home = "not_home",
                                          .with_attributes = true,
                                          .retain = false,
                                          .icon = "",
                                          .entity_category = "",
                                          .enabled_by_default = std::nullopt};

  HaEntityDeviceTracker(HaBridge &ha_bridge, std::string name,
                        std::optional<std::string> child_object_id = std::nullopt,
                        Configuration configuration = _default);

public:
  void publishConfiguration() override;
  void unpublishConfiguration() override;
  void republishState() override;

  /**
   * @brief Publish binary presence (`true` for home, `false` for not_home). Also see `updateHome()`.
   */
  void publishHome(bool is_home);

  /**
   * @brief Publish binary presence only if it has changed. Also see `publishHome()`.
   */
  void updateHome(bool is_home);

  /**
   * @brief Publish location name or zone string (e.g. `"home"`, `"work"`). Also see `updateState()`.
   */
  void publishState(const std::string &state);

  /**
   * @brief Publish location name or zone string only if it has changed. Also see `publishState()`.
   */
  void updateState(const std::string &state);

  /**
   * @brief Publish geographic coordinates, GPS accuracy, and battery percentage.
   */
  void publishLocation(double latitude, double longitude, std::optional<double> gps_accuracy = std::nullopt,
                       std::optional<uint8_t> battery = std::nullopt);

  /**
   * @brief Publish custom JSON attributes (`json_attributes_topic`).
   */
  void publishAttributes(const Attributes::Map &attributes);

protected:
  bool buildComponentConfiguration(ComponentConfig &out) override;

private:
  std::string _name;
  HaBridge &_ha_bridge;
  std::string _child_object_id;
  Configuration _configuration;

  std::optional<std::string> _state;
  std::optional<Attributes::Map> _attributes;
};

#endif // __HA_ENTITY_DEVICE_TRACKER_H__
