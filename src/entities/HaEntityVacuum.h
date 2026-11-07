// Copyright (c) Johan Böhlin
// SPDX-License-Identifier: MIT

#ifndef __HA_ENTITY_VACUUM_H__
#define __HA_ENTITY_VACUUM_H__

#include "AttributeVariants.h"
#include <HaBridge.h>
#include <HaEntity.h>
#include <cstdint>
#include <functional>
#include <optional>
#include <string>
#include <vector>

/**
 * @brief Represents an MQTT Vacuum entity (`vacuum.mqtt`) for robotic vacuums.
 *
 * Supports activity states (`cleaning`, `docked`, `paused`, `idle`, `returning`, `error`), battery level reporting,
 * fan speeds, and standard commands (`start`, `pause`, `stop`, `return_to_base`, `clean_spot`, `locate`).
 *
 * See https://www.home-assistant.io/integrations/vacuum.mqtt/
 */
class HaEntityVacuum : public HaEntity {
public:
  enum class State {
    Cleaning,
    Docked,
    Paused,
    Idle,
    Returning,
    Error,
  };

  enum class Command {
    Start,
    Pause,
    Stop,
    ReturnToBase,
    CleanSpot,
    Locate,
    Unknown,
  };

  static std::string stateToString(State state);
  static std::optional<State> stringToState(const std::string &str);
  static std::string commandToString(Command command);
  static Command stringToCommand(const std::string &str);

  struct Configuration {
    /**
     * @brief Supported features list. Defaults to standard vacuum commands.
     */
    std::vector<std::string> supported_features = {"start",   "pause",  "stop",   "return_home",
                                                   "battery", "status", "locate", "clean_spot"};

    /**
     * @brief List of supported fan speeds (e.g. `{"eco", "standard", "high", "max"}`). Leave empty if unsupported.
     */
    std::vector<std::string> fan_speed_list = {};

    /**
     * @brief If `true`, configure `json_attributes_topic` for publishing custom JSON attributes.
     */
    bool with_attributes = false;

    /**
     * @brief If `true`, publishes messages with the MQTT retain flag set.
     */
    bool retain = false;

    /**
     * @brief Optional custom icon (e.g. `"mdi:robot-vacuum"`).
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

  inline static Configuration _default = {
      .supported_features = {"start", "pause", "stop", "return_home", "battery", "status", "locate", "clean_spot"},
      .fan_speed_list = {},
      .with_attributes = false,
      .retain = false,
      .icon = "",
      .entity_category = "",
      .enabled_by_default = std::nullopt};

  HaEntityVacuum(HaBridge &ha_bridge, std::string name, std::optional<std::string> child_object_id = std::nullopt,
                 Configuration configuration = _default);

public:
  void publishConfiguration() override;
  void unpublishConfiguration() override;
  void republishState() override;

  /**
   * @brief Publish the vacuum state using typed `State`. Also see `update()`.
   */
  void publish(State state);

  /**
   * @brief Publish the vacuum state string. Also see `update()`.
   */
  void publish(const std::string &state);

  /**
   * @brief Publish the vacuum state only if it has changed. Also see `publish()`.
   */
  void update(State state);

  /**
   * @brief Publish the vacuum state only if it has changed. Also see `publish()`.
   */
  void update(const std::string &state);

  /**
   * @brief Publish the battery level (`0..100%`). Also see `updateBattery()`.
   */
  void publishBattery(uint8_t battery);

  /**
   * @brief Publish the battery level only if it has changed. Also see `publishBattery()`.
   */
  void updateBattery(uint8_t battery);

  /**
   * @brief Publish the current fan speed string. Also see `updateFanSpeed()`.
   */
  void publishFanSpeed(const std::string &speed);

  /**
   * @brief Publish the current fan speed only if it has changed. Also see `publishFanSpeed()`.
   */
  void updateFanSpeed(const std::string &speed);

  /**
   * @brief Set callback for receiving vacuum commands (`start`, `pause`, `stop`, `return_to_base`, etc.).
   */
  bool onCommand(std::function<void(Command command)> callback);

  /**
   * @brief Set callback for receiving fan speed selection commands.
   */
  bool onFanSpeed(std::function<void(const std::string &speed)> callback);

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
  std::optional<uint8_t> _battery;
  std::optional<std::string> _fan_speed;
  std::optional<Attributes::Map> _attributes;
};

#endif // __HA_ENTITY_VACUUM_H__
