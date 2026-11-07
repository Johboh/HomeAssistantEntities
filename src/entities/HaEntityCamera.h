// Copyright (c) Johan Böhlin
// SPDX-License-Identifier: MIT

#ifndef __HA_ENTITY_CAMERA_H__
#define __HA_ENTITY_CAMERA_H__

#include <HaBridge.h>
#include <HaEntity.h>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>

/**
 * @brief Represents an MQTT Camera entity (`camera.mqtt`).
 *
 * Publishes raw binary image frames (such as JPEG snapshots from an ESP32-CAM) directly over MQTT to Home Assistant.
 *
 * See https://www.home-assistant.io/integrations/camera.mqtt/
 */
class HaEntityCamera : public HaEntity {
public:
  struct Configuration {
    /**
     * @brief Optional custom icon (e.g. `"mdi:camera"`).
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

  inline static Configuration _default = {.icon = "", .entity_category = "", .enabled_by_default = std::nullopt};

  HaEntityCamera(HaBridge &ha_bridge, std::string name, std::optional<std::string> child_object_id = std::nullopt,
                 Configuration configuration = _default);

public:
  void publishConfiguration() override;
  void unpublishConfiguration() override;
  void republishState() override {}

  /**
   * @brief Publish a binary image frame (e.g. JPEG bytes) to Home Assistant.
   */
  void publishImage(const uint8_t *data, size_t length);

  /**
   * @brief Publish an image frame string to Home Assistant.
   */
  void publishImage(const std::string &data);

protected:
  bool buildComponentConfiguration(ComponentConfig &out) override;

private:
  std::string _name;
  HaBridge &_ha_bridge;
  std::string _child_object_id;
  Configuration _configuration;
};

#endif // __HA_ENTITY_CAMERA_H__
