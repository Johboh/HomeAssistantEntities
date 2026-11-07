// Copyright (c) Johan Böhlin
// SPDX-License-Identifier: MIT

#ifndef __HA_ENTITY_NUMBER_H__
#define __HA_ENTITY_NUMBER_H__

#include "HaDeviceClasses.h"
#include "HaEntityActuator.h"
#include <HaBridge.h>
#include <HaEntity.h>
#include <cstdint>
#include <functional>
#include <optional>
#include <string>

/**
 * @brief Represents a controllable MQTT Number entity (`number.mqtt`) that reports a numeric value to Home Assistant
 * and receives numeric set commands within configured bounds.
 *
 * See https://www.home-assistant.io/integrations/number.mqtt/
 *
 * @par Example
 * @code
 * HaEntityNumber target_temp(ha_bridge, "Target Temperature", "target_temp", {
 *     .min_value = 15.0f,
 *     .max_value = 30.0f,
 *     .step = 0.5f,
 *     .unit = "°C",
 *     .device_class = homeassistantentities::Sensor::Temperature{},
 *     .mode = HaEntityNumber::Configuration::Mode::Slider,
 * });
 *
 * target_temp.onCommand([&](float value) {
 *   setThermostatSetpoint(value);
 *   target_temp.publish(value);
 * });
 *
 * // Once MQTT is connected:
 * target_temp.publishConfiguration();
 * target_temp.publish(21.0f);
 * @endcode
 */
class HaEntityNumber : public HaEntity {
public:
  struct Configuration {
    /**
     * @brief Minimum numeric value allowed (default `1.0f`).
     */
    float min_value = 1.0f;

    /**
     * @brief Maximum numeric value allowed (default `100.0f`).
     */
    float max_value = 100.0f;

    /**
     * @brief Step increment for changes in Home Assistant (smallest allowed value is `0.001f`).
     */
    float step = 1.0f;

    /**
     * @brief Optional unit of measurement reported for this number (e.g. `"°C"`, `"%"`, `"s"`). Ensure published values
     * match this unit.
     */
    std::string unit = "";

    /**
     * @brief Optional device class for this number (e.g. `"duration"`, `"temperature"`, or a class from
     * `homeassistantentities::Sensor::*`).
     * See https://www.home-assistant.io/integrations/number/#device-class
     */
    std::string device_class = "";

    /**
     * @brief If `true`, tells Home Assistant to publish command messages with the MQTT retain flag set.
     */
    bool retain = false;

    enum class Mode {
      Auto,
      Box,
      Slider,
    };

    /**
     * @brief Controls how the number input is rendered in the Home Assistant UI (`"auto"`, `"box"`, or `"slider"`).
     */
    Mode mode = Mode::Auto;

    /**
     * @brief Optional custom icon (e.g. `"mdi:speedometer"`).
     */
    std::string icon = "";

    /**
     * @brief Optional entity category (`"config"` or `"diagnostic"`).
     */
    std::string entity_category = "";

    /**
     * @brief Defines whether the number works in optimistic mode in Home Assistant.
     */
    bool optimistic = false;

    /**
     * @brief Number of decimals to use in the Home Assistant frontend when displaying the value.
     */
    std::optional<uint8_t> suggested_display_precision = std::nullopt;

    /**
     * @brief Defines whether the entity should be enabled when first added to Home Assistant. Defaults to `true` in
     * Home Assistant when omitted.
     */
    std::optional<bool> enabled_by_default = std::nullopt;
  };

  inline static Configuration _default = {.min_value = 1.0f,
                                          .max_value = 100.0f,
                                          .step = 1.0f,
                                          .unit = "",
                                          .device_class = "",
                                          .retain = false,
                                          .mode = Configuration::Mode::Auto,
                                          .icon = "",
                                          .entity_category = "",
                                          .optimistic = false,
                                          .suggested_display_precision = std::nullopt,
                                          .enabled_by_default = std::nullopt};

  /**
   * @brief Construct a new HaEntityNumber object.
   *
   * @param ha_bridge Bridge for MQTT and Home Assistant.
   * @param name Human-readable name used for the entity in Home Assistant. If a device is set on `ha_bridge`, the
   * friendly name displayed in the UI is the device name plus this name (e.g. device `"Balcony"` + name `"LED Speed"`
   * -> `"Balcony LED Speed"`). Can be empty if a device is set and this is the main (or only) entity of the device. See
   * https://developers.home-assistant.io/docs/core/entity/#entity-naming for details.
   * @param object_id Unique machine-readable object ID (`[a-zA-Z0-9_-]`) for this number on this node/device (e.g.
   * `"balcony_leds_speed"`).
   * @param configuration Configuration for this number entity.
   */
  HaEntityNumber(HaBridge &ha_bridge, std::string name, std::string object_id, Configuration configuration = _default);

public:
  void publishConfiguration() override;
  void unpublishConfiguration() override;
  void republishState() override;

  /**
   * @brief Publish the current numeric value regardless of whether the value has changed. Values are clamped to
   * `[min_value, max_value]`; non-finite values (`NaN`/`Inf`) are ignored. Also see `update()`.
   *
   * @param number Numeric value to publish.
   */
  void publish(float number);

  /**
   * @brief Publish the current numeric value only if the value has changed. Values are clamped to
   * `[min_value, max_value]`; non-finite values (`NaN`/`Inf`) are ignored. Also see `publish()`.
   *
   * @param number Numeric value to publish.
   */
  void update(float number);

  /**
   * @brief Set callback for receiving numeric set commands from Home Assistant.
   *
   * @param callback Callback invoked with the requested value (clamped to `[min_value, max_value]`).
   * @return true if the subscription succeeded, false otherwise.
   */
  bool onCommand(std::function<void(float)> callback);

protected:
  bool buildComponentConfiguration(ComponentConfig &out) override;

private:
  static Configuration sanitizeConfiguration(Configuration configuration);

private:
  Configuration _configuration;
  HaEntityActuator _ha_entity_actuator;
};

#endif // __HA_ENTITY_NUMBER_H__
