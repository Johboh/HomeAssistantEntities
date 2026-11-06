// Copyright (c) Johan Böhlin
// SPDX-License-Identifier: MIT

#ifndef __HA_ENTITY_FAN_H__
#define __HA_ENTITY_FAN_H__

#include <HaBridge.h>
#include <HaEntity.h>
#include <cstdint>
#include <functional>
#include <optional>
#include <string>
#include <vector>

/**
 * @brief Represent a fan with speed, direction, oscillation, and presets.
 * See https://www.home-assistant.io/integrations/fan.mqtt/
 */
class HaEntityFan : public HaEntity {
public:
  struct Configuration {

    /**
     * @brief If this fan can change direction of flow.
     */
    bool with_direction = false;

    /**
     * @brief If this fan supports oscillation (back and forth movements).
     */
    bool with_oscillation = false;

    /**
     * @brief If this fan supports controlling the speed, compared to being only an on/off fan.
     */
    bool with_speed = false;

    /**
     * @brief If with_speed is set, the minimum of numeric output range (off not included, so speed_range_min - 1
     * represents 0 %). The percentage_step is defined by 100 / the number of speeds within the speed range.
     */
    uint32_t speed_range_min = 1;

    /**
     * @brief If with_speed is set, the maximum of numeric output range (representing 100 %). The percentage_step is
     * defined by 100 / the number of speeds within the speed range.
     */
    uint32_t speed_range_max = 100;

    /**
     * @brief If non empty, the presets this fan supports (order preserved in Home Assistant UI).
     */
    std::vector<std::string> presets = {};

    /**
     * @brief If true, this tells Home Assistant to publish the message on the command topic with retain set to true.
     */
    bool retain = false;

    /**
     * @brief Optional custom icon (e.g. "mdi:fan").
     */
    std::string icon = "";

    /**
     * @brief Optional entity category ("config" or "diagnostic").
     */
    std::string entity_category = "";

    /**
     * @brief Flag that defines if fan works in optimistic mode.
     */
    bool optimistic = false;

    /**
     * @brief Flag which defines if the entity should be enabled when first added to Home Assistant. Defaults to true
     * in Home Assistant when omitted.
     */
    std::optional<bool> enabled_by_default = std::nullopt;
  };

  inline static Configuration _default = {.with_direction = false,
                                          .with_oscillation = false,
                                          .with_speed = false,
                                          .speed_range_min = 1,
                                          .speed_range_max = 100,
                                          .presets = {},
                                          .retain = false,
                                          .icon = "",
                                          .entity_category = "",
                                          .optimistic = false,
                                          .enabled_by_default = std::nullopt};

  /**
   * @brief Construct a new Ha Entity Fan object
   *
   * @param name this is the human readable name that will be used for the entity in Home Assistant. If a device is set
   * when creating the [HaBridge], the friendly named displayed in the UI will be the device name plus this name.
   * Example: if device name is "Bathroom" and entity name "fan", friendly name will be "Bathroom fan". If no
   * device, friendly name will be just "fan". If a device is set, this name can be left empty if this entity is the
   * one main entity (or only) entity of this device. See
   * https://developers.home-assistant.io/docs/core/entity/#entity-naming for more
   * information.
   * @param child_object_id child identifier for this entity in case there are several fans of the same
   * entity type for the same node ID. Leave as empty string for no child object ID. Valid characters
   * are [a-zA-Z0-9_-] (machine readable, not human readable)
   * @param configuration the configuration for this entity.
   */
  HaEntityFan(HaBridge &ha_bridge, std::string name, std::string child_object_id,
              Configuration configuration = _default);

public:
  enum class Direction {
    Forward,
    Reverse,
  };

  void publishConfiguration() override;
  void unpublishConfiguration() override;
  void republishState() override;

  /**
   * @brief Publish the direction. This will publish to MQTT regardless if the value has changed. Also see
   * updateDirection().
   *
   * with_direction in Configuration must be set.
   *
   * @param direction the direction to publish ("forward" or "reverse").
   */
  void publishDirection(const std::string &direction);
  void publishDirection(Direction direction) {
    publishDirection(direction == Direction::Forward ? "forward" : "reverse");
  }

  /**
   * @brief Publish the direction, but only if the value has changed. Also see publishDirection().
   *
   * with_direction in Configuration must be set.
   *
   * @param direction the direction to publish ("forward" or "reverse").
   */
  void updateDirection(const std::string &direction);
  void updateDirection(Direction direction) {
    updateDirection(direction == Direction::Forward ? "forward" : "reverse");
  }

  /**
   * @brief Set callback for receiving direction commands ("forward" or "reverse") from Home Assistant.
   *
   * with_direction in Configuration must be set.
   */
  bool onDirection(std::function<void(std::string)> callback);

  //--------------------------------------

  /**
   * @brief Publish the oscillation. This will publish to MQTT regardless if the value has changed. Also see
   * updateOscillation().
   *
   * with_oscillation in Configuration must be set.
   *
   * @param oscillation if oscillation should be on or off.
   */
  void publishOscillation(bool oscillation);

  /**
   * @brief Publish the oscillation, but only if the value has changed. Also see publishOscillation().
   *
   * with_oscillation in Configuration must be set.
   *
   * @param oscillation if oscillation should be on or off.
   */
  void updateOscillation(bool oscillation);

  /**
   * @brief Set callback for receiving oscillation commands from Home Assistant.
   *
   * with_oscillation in Configuration must be set.
   */
  bool onOscillation(std::function<void(bool)> callback);

  //--------------------------------------

  /**
   * @brief Publish the speed. This will publish to MQTT regardless if the value has changed. Also see
   * updateSpeed().
   *
   * with_speed in Configuration must be set.
   *
   * @param speed the speed, value between speed_range_min - 1 (representing 0%) and speed_range_max.
   */
  void publishSpeed(uint32_t speed);

  /**
   * @brief Publish the speed, but only if the value has changed. Also see publishSpeed().
   *
   * with_speed in Configuration must be set.
   *
   * @param speed the speed, value between speed_range_min - 1 (representing 0%) and speed_range_max.
   */
  void updateSpeed(uint32_t speed);

  /**
   * @brief Set callback for receiving speed commands from Home Assistant. Value between speed_range_min - 1
   * (representing 0%) and speed_range_max set in the Configuration.
   *
   * with_speed in Configuration must be set.
   */
  bool onSpeed(std::function<void(uint32_t)> callback);

  //--------------------------------------

  /**
   * @brief Publish a preset. This will publish to MQTT regardless if the value has changed. Also see
   * updatePreset().
   *
   * presets in Configuration must be set to a non empty list.
   *
   * @param preset the preset.
   */
  void publishPreset(const std::string &preset);

  /**
   * @brief Publish a preset, but only if the value has changed. Also see publishPreset().
   *
   * presets in Configuration must be set to a non empty list.
   *
   * @param preset the preset.
   */
  void updatePreset(const std::string &preset);

  /**
   * @brief Set callback for receiving preset commands from Home Assistant.
   *
   * presets in Configuration must be set to a non empty list.
   */
  bool onPreset(std::function<void(std::string)> callback);

  //--------------------------------------

  /**
   * @brief Publish the fan state. This will publish to MQTT regardless if the value has changed. Also see
   * update().
   *
   * @param on if the fan is on or off.
   */
  void publish(bool on);

  /**
   * @brief Publish the fan state, but only if the value has changed. Also see publish().
   *
   * @param on if the fan is on or off.
   */
  void update(bool on);

  /**
   * @brief Set callback for receiving on/off state commands from Home Assistant.
   */
  bool onCommand(std::function<void(bool)> callback);

protected:
  bool buildComponentConfiguration(ComponentConfig &out) override;

private:
  uint32_t clampSpeed(uint32_t speed) const;

private:
  std::string _name;
  HaBridge &_ha_bridge;
  std::string _child_object_id;
  Configuration _configuration;

private:
  std::optional<bool> _on;
  std::optional<uint32_t> _speed;
  std::optional<bool> _oscillation;
  std::optional<std::string> _preset;
  std::optional<std::string> _direction;
};

#endif // __HA_ENTITY_FAN_H__
