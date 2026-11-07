// Copyright (c) Johan Böhlin
// SPDX-License-Identifier: MIT

#ifndef __HA_ENTITY_HUMIDIFIER_H__
#define __HA_ENTITY_HUMIDIFIER_H__

#include "AttributeVariants.h"
#include <HaBridge.h>
#include <HaEntity.h>
#include <cstdint>
#include <functional>
#include <optional>
#include <string>
#include <vector>

/**
 * @brief Represents an MQTT Humidifier or Dehumidifier entity (`humidifier.mqtt`).
 *
 * Supports on/off control, target humidity setpoint, current ambient humidity reporting,
 * operating action tracking (`humidifying`, `drying`, `idle`, `off`), and operation modes.
 *
 * See https://www.home-assistant.io/integrations/humidifier.mqtt/
 */
class HaEntityHumidifier : public HaEntity {
public:
  enum class DeviceClass {
    Humidifier,
    Dehumidifier,
  };

  enum class Action {
    Off,
    Humidifying,
    Drying,
    Idle,
    Unknown,
  };

  static std::string actionToString(Action action);
  static std::optional<Action> stringToAction(const std::string &str);

  struct Configuration {
    /**
     * @brief Device class: `DeviceClass::Humidifier` (default) or `DeviceClass::Dehumidifier`.
     */
    DeviceClass device_class = DeviceClass::Humidifier;

    /**
     * @brief Minimum target humidity percentage (default `30`).
     */
    uint8_t min_humidity = 30;

    /**
     * @brief Maximum target humidity percentage (default `80`).
     */
    uint8_t max_humidity = 80;

    /**
     * @brief List of supported modes (e.g. `{"normal", "eco", "boost", "auto"}`).
     */
    std::vector<std::string> modes = {};

    /**
     * @brief Whether ambient current humidity reporting is enabled (default `true`).
     */
    bool with_current_humidity = true;

    /**
     * @brief Whether operating action reporting (`humidifying`, `drying`, `idle`, `off`) is enabled (default `true`).
     */
    bool with_action = true;

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
     * @brief Optional custom icon (e.g. `"mdi:air-humidifier"`).
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

  inline static Configuration _default = {.device_class = DeviceClass::Humidifier,
                                          .min_humidity = 30,
                                          .max_humidity = 80,
                                          .modes = {},
                                          .with_current_humidity = true,
                                          .with_action = true,
                                          .with_attributes = false,
                                          .retain = false,
                                          .optimistic = false,
                                          .icon = "",
                                          .entity_category = "",
                                          .enabled_by_default = std::nullopt};

  HaEntityHumidifier(HaBridge &ha_bridge, std::string name, std::optional<std::string> child_object_id = std::nullopt,
                     Configuration configuration = _default);

public:
  void publishConfiguration() override;
  void unpublishConfiguration() override;
  void republishState() override;

  /**
   * @brief Publish the power state (`true` for ON, `false` for OFF). Also see `update()`.
   */
  void publish(bool on);

  /**
   * @brief Publish the power state only if it has changed. Also see `publish()`.
   */
  void update(bool on);

  /**
   * @brief Set callback for receiving power ON/OFF commands from Home Assistant.
   */
  bool onCommand(std::function<void(bool on)> callback);

  //--------------------------------------

  /**
   * @brief Publish the target humidity setpoint (`min_humidity..max_humidity`). Also see `updateTargetHumidity()`.
   */
  void publishTargetHumidity(uint8_t humidity);

  /**
   * @brief Publish the target humidity setpoint only if it has changed. Also see `publishTargetHumidity()`.
   */
  void updateTargetHumidity(uint8_t humidity);

  /**
   * @brief Set callback for receiving target humidity setpoint commands from Home Assistant.
   */
  bool onTargetHumidity(std::function<void(uint8_t humidity)> callback);

  //--------------------------------------

  /**
   * @brief Publish the current ambient humidity (`0..100%`). Also see `updateCurrentHumidity()`.
   */
  void publishCurrentHumidity(uint8_t humidity);

  /**
   * @brief Publish the current ambient humidity only if it has changed. Also see `publishCurrentHumidity()`.
   */
  void updateCurrentHumidity(uint8_t humidity);

  //--------------------------------------

  /**
   * @brief Publish the active operating action (`Action::Humidifying`, `Action::Drying`, `Action::Idle`,
   * `Action::Off`). Also see `updateAction()`.
   */
  void publishAction(Action action);

  /**
   * @brief Publish the operating action string. Also see `updateAction()`.
   */
  void publishAction(const std::string &action);

  /**
   * @brief Publish the operating action only if it has changed. Also see `publishAction()`.
   */
  void updateAction(Action action);

  /**
   * @brief Publish the operating action only if it has changed. Also see `publishAction()`.
   */
  void updateAction(const std::string &action);

  //--------------------------------------

  /**
   * @brief Publish the active mode. Also see `updateMode()`.
   */
  void publishMode(const std::string &mode);

  /**
   * @brief Publish the active mode only if it has changed. Also see `publishMode()`.
   */
  void updateMode(const std::string &mode);

  /**
   * @brief Set callback for receiving mode commands from Home Assistant.
   */
  bool onMode(std::function<void(const std::string &mode)> callback);

  //--------------------------------------

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
  std::optional<uint8_t> _target_humidity;
  std::optional<uint8_t> _current_humidity;
  std::optional<std::string> _action;
  std::optional<std::string> _mode;
  std::optional<Attributes::Map> _attributes;
};

#endif // __HA_ENTITY_HUMIDIFIER_H__
