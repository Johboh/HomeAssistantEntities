// Copyright (c) Johan Böhlin
// SPDX-License-Identifier: MIT

#ifndef __HA_ENTITY_CLIMATE_H__
#define __HA_ENTITY_CLIMATE_H__

#include "AttributeVariants.h"
#include <HaBridge.h>
#include <HaEntity.h>
#include <cstdint>
#include <functional>
#include <optional>
#include <string>
#include <vector>

/**
 * @brief Represents an MQTT Climate / HVAC entity (`climate.mqtt`) for thermostats, air conditioners, and heaters.
 *
 * Supports HVAC operation modes, ambient temperature reporting, target temperature (setpoint) control or dual
 * setpoint range, operating action tracking, fan modes, swing modes, preset modes, and target humidity control.
 *
 * See https://www.home-assistant.io/integrations/climate.mqtt/
 *
 * @par Example
 * @code
 * HaEntityClimate thermostat(ha_bridge, "Thermostat", "living_room", {
 *     .modes = {"off", "heat", "cool", "auto"},
 *     .min_temp = 10.0,
 *     .max_temp = 32.0,
 *     .temp_step = 0.5,
 *     .fan_modes = {"auto", "low", "medium", "high"},
 *     .preset_modes = {"eco", "comfort", "boost"},
 * });
 *
 * thermostat.onMode([&](HaEntityClimate::Mode mode) {
 *   setHvacMode(mode);
 *   thermostat.publishMode(mode);
 * });
 * thermostat.onTargetTemperature([&](double temp) {
 *   setTargetSetpoint(temp);
 *   thermostat.publishTargetTemperature(temp);
 * });
 * thermostat.onFanMode([&](const std::string &fan_mode) {
 *   setFanSpeed(fan_mode);
 *   thermostat.publishFanMode(fan_mode);
 * });
 *
 * // Once MQTT is connected:
 * thermostat.publishConfiguration();
 * thermostat.publishMode(HaEntityClimate::Mode::Heat);
 * thermostat.publishCurrentTemperature(21.5);
 * thermostat.publishTargetTemperature(22.0);
 * thermostat.publishAction(HaEntityClimate::Action::Heating);
 * @endcode
 */
class HaEntityClimate : public HaEntity {
public:
  enum class Mode {
    Auto,
    Off,
    Cool,
    Heat,
    Dry,
    FanOnly,
    Custom,
  };

  enum class Action {
    Off,
    Heating,
    Cooling,
    Drying,
    Idle,
    Fan,
    Unknown,
  };

  static std::string modeToString(Mode mode);
  static std::optional<Mode> stringToMode(const std::string &str);
  static std::string actionToString(Action action);
  static std::optional<Action> stringToAction(const std::string &str);

  struct Configuration {
    /**
     * @brief Supported HVAC modes. Defaults to `{"off", "heat", "cool", "auto"}`.
     * Common modes in Home Assistant: `"off"`, `"heat"`, `"cool"`, `"auto"`, `"dry"`, `"fan_only"`.
     */
    std::vector<std::string> modes = {"off", "heat", "cool", "auto"};

    /**
     * @brief Minimum settable target temperature (default `7.0`).
     */
    double min_temp = 7.0;

    /**
     * @brief Maximum settable target temperature (default `35.0`).
     */
    double max_temp = 35.0;

    /**
     * @brief Step used for the target temperature (default `0.5`).
     */
    double temp_step = 0.5;

    /**
     * @brief Temperature unit: `"C"` (Celsius, default) or `"F"` (Fahrenheit).
     */
    std::string temperature_unit = "C";

    /**
     * @brief Optional display precision for the temperature (e.g. `0.1`, `0.5`, `1.0`).
     */
    std::optional<double> precision = std::nullopt;

    /**
     * @brief Whether target temperature setpoint is enabled (default `true`).
     */
    bool with_target_temperature = true;

    /**
     * @brief If `true`, enables dual setpoints (`target_temperature_low` and `target_temperature_high`) for heat_cool.
     */
    bool with_target_temperature_range = false;

    /**
     * @brief Whether ambient current temperature reporting is enabled (default `true`).
     */
    bool with_current_temperature = true;

    /**
     * @brief Whether current operating action reporting (`heating`, `cooling`, `idle`, etc.) is enabled (default
     * `true`).
     */
    bool with_action = true;

    /**
     * @brief Supported fan modes (e.g. `{"auto", "low", "medium", "high"}`). Leave empty if unsupported.
     */
    std::vector<std::string> fan_modes = {};

    /**
     * @brief Supported swing modes (e.g. `{"on", "off"}` or `{"both", "vertical", "horizontal"}`). Leave empty if
     * unsupported.
     */
    std::vector<std::string> swing_modes = {};

    /**
     * @brief Supported preset modes (e.g. `{"eco", "away", "boost", "comfort", "sleep"}`). Leave empty if unsupported.
     */
    std::vector<std::string> preset_modes = {};

    /**
     * @brief If `true`, reports ambient current humidity.
     */
    bool with_current_humidity = false;

    /**
     * @brief If `true`, supports target humidity setpoint.
     */
    bool with_target_humidity = false;

    /**
     * @brief Minimum target humidity (default `30`).
     */
    uint8_t min_humidity = 30;

    /**
     * @brief Maximum target humidity (default `99`).
     */
    uint8_t max_humidity = 99;

    /**
     * @brief If `true`, configure `json_attributes_topic` for publishing custom JSON attributes.
     */
    bool with_attributes = false;

    /**
     * @brief If `true`, publishes state/command messages with retain flag set.
     */
    bool retain = false;

    /**
     * @brief Defines whether the climate entity works in optimistic mode in Home Assistant.
     */
    bool optimistic = false;

    /**
     * @brief Optional custom icon (e.g. `"mdi:thermostat"`).
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

  inline static Configuration _default = {.modes = {"off", "heat", "cool", "auto"},
                                          .min_temp = 7.0,
                                          .max_temp = 35.0,
                                          .temp_step = 0.5,
                                          .temperature_unit = "C",
                                          .precision = std::nullopt,
                                          .with_target_temperature = true,
                                          .with_target_temperature_range = false,
                                          .with_current_temperature = true,
                                          .with_action = true,
                                          .fan_modes = {},
                                          .swing_modes = {},
                                          .preset_modes = {},
                                          .with_current_humidity = false,
                                          .with_target_humidity = false,
                                          .min_humidity = 30,
                                          .max_humidity = 99,
                                          .with_attributes = false,
                                          .retain = false,
                                          .optimistic = false,
                                          .icon = "",
                                          .entity_category = "",
                                          .enabled_by_default = std::nullopt};

  /**
   * @brief Construct a new HaEntityClimate object.
   *
   * @param ha_bridge Bridge for MQTT and Home Assistant.
   * @param name Human-readable name used for the entity in Home Assistant. If a device is set on `ha_bridge`, the
   * friendly name displayed in the UI is the device name plus this name (e.g. device `"Living Room"` + name
   * `"Climate"` -> `"Living Room Climate"`). Can be empty if a device is set and this is the main (or only) entity of
   * the device.
   * @param child_object_id Optional machine-readable child identifier (`[a-zA-Z0-9_-]`) in case there are several
   * climate entities on the same node ID. Pass `std::nullopt` or `""` when not needed.
   * @param configuration Configuration for this climate entity.
   */
  HaEntityClimate(HaBridge &ha_bridge, std::string name, std::optional<std::string> child_object_id = std::nullopt,
                  Configuration configuration = _default);

public:
  void publishConfiguration() override;
  void unpublishConfiguration() override;
  void republishState() override;

  /**
   * @brief Publish the HVAC mode as a string. Also see `updateMode()`.
   */
  void publishMode(const std::string &mode);

  /**
   * @brief Publish the HVAC mode using typed `Mode`. Also see `updateMode()`.
   */
  void publishMode(Mode mode);

  /**
   * @brief Publish the HVAC mode only if the value has changed. Also see `publishMode()`.
   */
  void updateMode(const std::string &mode);

  /**
   * @brief Publish the HVAC mode only if the value has changed. Also see `publishMode()`.
   */
  void updateMode(Mode mode);

  /**
   * @brief Set callback for receiving HVAC mode commands from Home Assistant.
   */
  bool onMode(std::function<void(const std::string &)> callback);

  /**
   * @brief Set callback for receiving HVAC mode commands from Home Assistant using typed `Mode`.
   */
  bool onMode(std::function<void(Mode)> callback);

  //--------------------------------------

  /**
   * @brief Publish the current ambient room temperature regardless of whether it has changed. Also see
   * `updateCurrentTemperature()`.
   */
  void publishCurrentTemperature(double temperature);

  /**
   * @brief Publish the current ambient room temperature only if it has changed. Also see `publishCurrentTemperature()`.
   */
  void updateCurrentTemperature(double temperature);

  //--------------------------------------

  /**
   * @brief Publish the target (setpoint) temperature regardless of whether it has changed. Also see
   * `updateTargetTemperature()`.
   */
  void publishTargetTemperature(double temperature);

  /**
   * @brief Publish the target (setpoint) temperature only if it has changed. Also see `publishTargetTemperature()`.
   */
  void updateTargetTemperature(double temperature);

  /**
   * @brief Set callback for receiving target temperature setpoint commands from Home Assistant.
   */
  bool onTargetTemperature(std::function<void(double)> callback);

  //--------------------------------------

  /**
   * @brief Publish the target temperature range (low and high) when `with_target_temperature_range` is enabled. Also
   * see `updateTargetTemperatureRange()`.
   */
  void publishTargetTemperatureRange(double low, double high);

  /**
   * @brief Publish the target temperature range only if either value has changed. Also see
   * `publishTargetTemperatureRange()`.
   */
  void updateTargetTemperatureRange(double low, double high);

  /**
   * @brief Set callback for receiving dual setpoint commands from Home Assistant.
   */
  bool onTargetTemperatureRange(std::function<void(double low, double high)> callback);

  //--------------------------------------

  /**
   * @brief Publish the current HVAC action (`"off"`, `"heating"`, `"cooling"`, `"idle"`, `"drying"`, `"fan"`). Also
   * see `updateAction()`.
   */
  void publishAction(const std::string &action);

  /**
   * @brief Publish the current HVAC action using typed `Action`. Also see `updateAction()`.
   */
  void publishAction(Action action);

  /**
   * @brief Publish the current HVAC action only if it has changed. Also see `publishAction()`.
   */
  void updateAction(const std::string &action);

  /**
   * @brief Publish the current HVAC action only if it has changed. Also see `publishAction()`.
   */
  void updateAction(Action action);

  //--------------------------------------

  /**
   * @brief Publish the active fan mode. Also see `updateFanMode()`.
   */
  void publishFanMode(const std::string &mode);

  /**
   * @brief Publish the active fan mode only if it has changed. Also see `publishFanMode()`.
   */
  void updateFanMode(const std::string &mode);

  /**
   * @brief Set callback for receiving fan mode commands from Home Assistant.
   */
  bool onFanMode(std::function<void(const std::string &)> callback);

  //--------------------------------------

  /**
   * @brief Publish the active swing mode. Also see `updateSwingMode()`.
   */
  void publishSwingMode(const std::string &mode);

  /**
   * @brief Publish the active swing mode only if it has changed. Also see `publishSwingMode()`.
   */
  void updateSwingMode(const std::string &mode);

  /**
   * @brief Set callback for receiving swing mode commands from Home Assistant.
   */
  bool onSwingMode(std::function<void(const std::string &)> callback);

  //--------------------------------------

  /**
   * @brief Publish the active preset mode. Also see `updatePresetMode()`.
   */
  void publishPresetMode(const std::string &mode);

  /**
   * @brief Publish the active preset mode only if it has changed. Also see `publishPresetMode()`.
   */
  void updatePresetMode(const std::string &mode);

  /**
   * @brief Set callback for receiving preset mode commands from Home Assistant.
   */
  bool onPresetMode(std::function<void(const std::string &)> callback);

  //--------------------------------------

  /**
   * @brief Publish the current ambient relative humidity (`0..100%`). Also see `updateCurrentHumidity()`.
   */
  void publishCurrentHumidity(uint8_t humidity);

  /**
   * @brief Publish the current ambient relative humidity only if it has changed. Also see `publishCurrentHumidity()`.
   */
  void updateCurrentHumidity(uint8_t humidity);

  /**
   * @brief Publish the target relative humidity setpoint (`min_humidity..max_humidity`). Also see
   * `updateTargetHumidity()`.
   */
  void publishTargetHumidity(uint8_t humidity);

  /**
   * @brief Publish the target relative humidity setpoint only if it has changed. Also see `publishTargetHumidity()`.
   */
  void updateTargetHumidity(uint8_t humidity);

  /**
   * @brief Set callback for receiving target humidity setpoint commands from Home Assistant.
   */
  bool onTargetHumidity(std::function<void(uint8_t)> callback);

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

  std::optional<std::string> _mode;
  std::optional<double> _current_temp;
  std::optional<double> _target_temp;
  std::optional<double> _target_temp_low;
  std::optional<double> _target_temp_high;
  std::optional<std::string> _action;
  std::optional<std::string> _fan_mode;
  std::optional<std::string> _swing_mode;
  std::optional<std::string> _preset_mode;
  std::optional<uint8_t> _current_humidity;
  std::optional<uint8_t> _target_humidity;
  std::optional<Attributes::Map> _attributes;

  std::function<void(double low, double high)> _range_callback;
};

#endif // __HA_ENTITY_CLIMATE_H__
