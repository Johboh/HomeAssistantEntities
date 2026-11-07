// Copyright (c) Johan Böhlin
// SPDX-License-Identifier: MIT

#ifndef __HA_ENTITY_WATER_HEATER_H__
#define __HA_ENTITY_WATER_HEATER_H__

#include "AttributeVariants.h"
#include <HaBridge.h>
#include <HaEntity.h>
#include <functional>
#include <optional>
#include <string>
#include <vector>

/**
 * @brief Represents an MQTT Water Heater entity (`water_heater.mqtt`) for boilers, heat pumps, and water heaters.
 *
 * Supports target temperature control, current water temperature reporting, operation modes, and optional away mode.
 *
 * See https://www.home-assistant.io/integrations/water_heater.mqtt/
 */
class HaEntityWaterHeater : public HaEntity {
public:
  struct Configuration {
    /**
     * @brief Minimum settable target temperature (default `40.0`).
     */
    double min_temp = 40.0;

    /**
     * @brief Maximum settable target temperature (default `65.0`).
     */
    double max_temp = 65.0;

    /**
     * @brief Step used for the target temperature (default `1.0`).
     */
    double temp_step = 1.0;

    /**
     * @brief Temperature unit: `"C"` (Celsius, default) or `"F"` (Fahrenheit).
     */
    std::string temperature_unit = "C";

    /**
     * @brief List of supported operation modes (e.g. `{"eco", "electric", "gas", "heat_pump", "high_demand",
     * "performance", "off"}`).
     */
    std::vector<std::string> modes = {"eco", "electric", "performance", "off"};

    /**
     * @brief Whether away mode (`away_mode_state_topic` / `away_mode_command_topic`) is enabled.
     */
    bool with_away_mode = false;

    /**
     * @brief Whether ambient current water temperature reporting is enabled (default `true`).
     */
    bool with_current_temperature = true;

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
     * @brief Optional custom icon (e.g. `"mdi:water-boiler"`).
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

  inline static Configuration _default = {.min_temp = 40.0,
                                          .max_temp = 65.0,
                                          .temp_step = 1.0,
                                          .temperature_unit = "C",
                                          .modes = {"eco", "electric", "performance", "off"},
                                          .with_away_mode = false,
                                          .with_current_temperature = true,
                                          .with_attributes = false,
                                          .retain = false,
                                          .optimistic = false,
                                          .icon = "",
                                          .entity_category = "",
                                          .enabled_by_default = std::nullopt};

  HaEntityWaterHeater(HaBridge &ha_bridge, std::string name, std::optional<std::string> child_object_id = std::nullopt,
                      Configuration configuration = _default);

public:
  void publishConfiguration() override;
  void unpublishConfiguration() override;
  void republishState() override;

  /**
   * @brief Publish the target water temperature setpoint. Also see `updateTargetTemperature()`.
   */
  void publishTargetTemperature(double temperature);

  /**
   * @brief Publish the target water temperature setpoint only if it has changed. Also see
   * `publishTargetTemperature()`.
   */
  void updateTargetTemperature(double temperature);

  /**
   * @brief Set callback for receiving target temperature setpoint commands from Home Assistant.
   */
  bool onTargetTemperature(std::function<void(double temperature)> callback);

  //--------------------------------------

  /**
   * @brief Publish the current measured water temperature. Also see `updateCurrentTemperature()`.
   */
  void publishCurrentTemperature(double temperature);

  /**
   * @brief Publish the current measured water temperature only if it has changed. Also see
   * `publishCurrentTemperature()`.
   */
  void updateCurrentTemperature(double temperature);

  //--------------------------------------

  /**
   * @brief Publish the active operation mode. Also see `updateMode()`.
   */
  void publishMode(const std::string &mode);

  /**
   * @brief Publish the active operation mode only if it has changed. Also see `publishMode()`.
   */
  void updateMode(const std::string &mode);

  /**
   * @brief Set callback for receiving operation mode commands from Home Assistant.
   */
  bool onMode(std::function<void(const std::string &mode)> callback);

  //--------------------------------------

  /**
   * @brief Publish away mode (`true` for ON, `false` for OFF). Also see `updateAwayMode()`.
   */
  void publishAwayMode(bool away);

  /**
   * @brief Publish away mode only if it has changed. Also see `publishAwayMode()`.
   */
  void updateAwayMode(bool away);

  /**
   * @brief Set callback for receiving away mode commands from Home Assistant.
   */
  bool onAwayMode(std::function<void(bool away)> callback);

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

  std::optional<double> _target_temp;
  std::optional<double> _current_temp;
  std::optional<std::string> _mode;
  std::optional<bool> _away;
  std::optional<Attributes::Map> _attributes;
};

#endif // __HA_ENTITY_WATER_HEATER_H__
