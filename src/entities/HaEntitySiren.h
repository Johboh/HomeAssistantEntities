// Copyright (c) Johan Böhlin
// SPDX-License-Identifier: MIT

#ifndef __HA_ENTITY_SIREN_H__
#define __HA_ENTITY_SIREN_H__

#include "AttributeVariants.h"
#include <HaBridge.h>
#include <HaEntity.h>
#include <cstdint>
#include <functional>
#include <optional>
#include <string>
#include <vector>

/**
 * @brief Represents an MQTT Siren entity (`siren.mqtt`) for alarms, buzzers, and warning chimes.
 *
 * Supports on/off control, available tones, duration, and volume settings.
 *
 * See https://www.home-assistant.io/integrations/siren.mqtt/
 */
class HaEntitySiren : public HaEntity {
public:
  struct TurnOnParams {
    std::optional<std::string> tone;
    std::optional<uint32_t> duration;
    std::optional<double> volume_level;
  };

  struct Configuration {
    /**
     * @brief List of available tones supported by the siren.
     */
    std::vector<std::string> available_tones = {};

    /**
     * @brief If `true`, the siren supports specifying a duration in seconds.
     */
    bool support_duration = true;

    /**
     * @brief If `true`, the siren supports volume adjustment (`0.0..1.0`).
     */
    bool support_volume_set = true;

    /**
     * @brief If `true`, configure `json_attributes_topic` for publishing custom JSON attributes.
     */
    bool with_attributes = false;

    /**
     * @brief If `true`, publishes messages with the MQTT retain flag set.
     */
    bool retain = false;

    /**
     * @brief Defines whether the entity works in optimistic mode in Home Assistant.
     */
    bool optimistic = false;

    /**
     * @brief Optional custom icon (e.g. `"mdi:bullhorn"`).
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

  inline static Configuration _default = {.available_tones = {},
                                          .support_duration = true,
                                          .support_volume_set = true,
                                          .with_attributes = false,
                                          .retain = false,
                                          .optimistic = false,
                                          .icon = "",
                                          .entity_category = "",
                                          .enabled_by_default = std::nullopt};

  HaEntitySiren(HaBridge &ha_bridge, std::string name, std::optional<std::string> child_object_id = std::nullopt,
                Configuration configuration = _default);

public:
  void publishConfiguration() override;
  void unpublishConfiguration() override;
  void republishState() override;

  /**
   * @brief Publish the siren state (`true` for ON, `false` for OFF). Also see `update()`.
   */
  void publish(bool on);

  /**
   * @brief Publish the siren state only if it has changed. Also see `publish()`.
   */
  void update(bool on);

  /**
   * @brief Set callback for receiving simple ON / OFF commands from Home Assistant.
   */
  bool onCommand(std::function<void(bool on)> callback);

  /**
   * @brief Set callback for receiving turn-on parameters (tone, duration, volume) from Home Assistant.
   */
  bool onTurnOn(std::function<void(const TurnOnParams &params)> callback);

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

  std::optional<bool> _on;
  std::optional<Attributes::Map> _attributes;

  std::function<void(const TurnOnParams &params)> _turn_on_callback;
};

#endif // __HA_ENTITY_SIREN_H__
