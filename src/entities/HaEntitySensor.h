// Copyright (c) Johan Böhlin
// SPDX-License-Identifier: MIT

#ifndef __HA_ENTITY_SENSOR_H__
#define __HA_ENTITY_SENSOR_H__

#include "AttributeVariants.h"
#include "HaDeviceClasses.h"
#include <HaBridge.h>
#include <HaEntity.h>
#include <cstdint>
#include <optional>
#include <string>
#include <vector>

/**
 * @brief Generic composable MQTT sensor or binary sensor (`sensor.mqtt` / `binary_sensor.mqtt`).
 *
 * Prefer using one of the dedicated typed wrappers (such as `HaEntityTemperature`, `HaEntityVoltage`, `HaEntityDoor`,
 * etc.) when available, or instantiate `HaEntitySensor` directly with any `Sensor::*` or `BinarySensor::*` device
 * class from `HaDeviceClasses.h`.
 * See https://www.home-assistant.io/integrations/sensor.mqtt/ and
 * https://www.home-assistant.io/integrations/binary_sensor.mqtt/.
 *
 * @par Example
 * @code
 * HaEntitySensor battery(ha_bridge, "Battery", std::nullopt, {
 *     .device_class = homeassistantentities::Sensor::Battery{},
 *     .unit_of_measurement = homeassistantentities::Sensor::Battery::Unit::Percent,
 *     .suggested_display_precision = 0,
 * });
 *
 * // Once MQTT is connected:
 * battery.publishConfiguration();
 * battery.update(87.0); // Publishes only when the value changes; use publish() to force
 * @endcode
 */
class HaEntitySensor : public HaEntity {
public:
  struct Configuration {
    /**
     * @brief The device class to use (one of the `Sensor::*` or `BinarySensor::*` classes in `HaDeviceClasses.h`).
     */
    const homeassistantentities::DeviceClass &device_class;

    /**
     * @brief The unit of measurement reported by the sensor. Should be a unit provided by `device_class`, or
     * `std::nullopt` if unitless.
     */
    std::optional<homeassistantentities::UnitType> unit_of_measurement = std::nullopt;

    /**
     * @brief The state class to use for this sensor (e.g. `"measurement"`, `"total"`, `"total_increasing"`, or
     * `std::nullopt` to omit). Ignored for binary sensors.
     * See https://developers.home-assistant.io/docs/core/entity/sensor/#available-state-classes
     */
    std::optional<std::string> state_class = "measurement";

    /**
     * @brief If true, set up a JSON attributes topic (`json_attributes_topic`) so attributes can be published along
     * with the state or via `publishAttributes()` / `updateAttributes()`.
     */
    bool with_attributes = false;

    /**
     * @brief Optional custom icon for the sensor (e.g. `"mdi:brightness-percent"`). When omitted, Home Assistant
     * selects a default icon based on `device_class`.
     */
    std::optional<std::string> icon = std::nullopt;

    /**
     * @brief In Home Assistant, trigger state events even if the sensor's state has not changed. Useful for graphing
     * repeated identical measurements or triggering automations on every incoming state message.
     */
    bool force_update = false;

    /**
     * @brief Optional custom unit of measurement string (e.g. ISO 4217 currency code such as `"EUR"` or `"USD"` for
     * `Sensor::Monetary`, or a custom unit for `Sensor::Undefined::Empty`). Used when `unit_of_measurement` is not set.
     */
    std::optional<std::string> custom_unit_of_measurement = std::nullopt;

    /**
     * @brief Valid option strings when using `Sensor::Enum` (`device_class: "enum"`). Required by Home Assistant for
     * enum sensors.
     */
    std::vector<std::string> options = {};

    /**
     * @brief Optional entity category (`"config"` or `"diagnostic"`).
     */
    std::optional<std::string> entity_category = std::nullopt;

    /**
     * @brief Optional number of seconds after which the sensor's state expires if not updated (becomes `"unavailable"`
     * in Home Assistant).
     */
    std::optional<uint32_t> expire_after = std::nullopt;

    /**
     * @brief For binary sensors only: number of seconds after turning on before Home Assistant automatically resets the
     * binary sensor state back to off (useful for PIR motion sensors that only send `"ON"` events).
     */
    std::optional<uint32_t> off_delay = std::nullopt;

    /**
     * @brief Number of decimals to use in the Home Assistant frontend when displaying numeric values.
     * Only supported for `sensor` entities (not `binary_sensor`).
     */
    std::optional<uint8_t> suggested_display_precision = std::nullopt;

    /**
     * @brief Defines whether the entity should be enabled when first added to Home Assistant. Defaults to `true` in
     * Home Assistant when omitted.
     */
    std::optional<bool> enabled_by_default = std::nullopt;
  };

  /**
   * @brief Construct a new HaEntitySensor object.
   *
   * @param ha_bridge Bridge for MQTT and Home Assistant.
   * @param name Human-readable name used for the entity in Home Assistant. If a device is set on `ha_bridge`, the
   * friendly name displayed in the UI is the device name plus this name (e.g. device `"Bathroom"` + name `"Voltage"` ->
   * `"Bathroom Voltage"`). Can be empty if a device is set and this is the main (or only) entity of the device. See
   * https://developers.home-assistant.io/docs/core/entity/#entity-naming for details.
   * @param child_object_id Optional machine-readable child identifier (`[a-zA-Z0-9_-]`) to distinguish multiple sensors
   * of the same type on the same node. For example, for node `"door"` and `BinarySensor::Door` (`"door"`), the default
   * discovery topic is `"homeassistant/binary_sensor/door/door/config"`; setting `child_object_id` to `"upper"` yields
   * `"homeassistant/binary_sensor/door/door_upper/config"` and state topic `"door/binary_sensor/door/upper/state"`.
   * Pass `std::nullopt` or `""` when not needed.
   * @param configuration Configuration for this sensor.
   */
  HaEntitySensor(HaBridge &ha_bridge, std::string name, std::optional<std::string> child_object_id,
                 const Configuration &configuration);

public:
  void publishConfiguration() override;
  void unpublishConfiguration() override;
  void republishState() override;

  /**
   * @brief Publish a numeric sensor value regardless of whether the value has changed. Non-finite values (`NaN`/`Inf`)
   * are ignored. Also see `update()`.
   *
   * @param value Value in the unit specified in `Configuration`.
   * @param attributes Optional JSON attributes to publish with the value (`Configuration::with_attributes` must be
   * `true`).
   */
  void publish(double value, const Attributes::Map &attributes = {});

  /**
   * @brief Publish a numeric sensor value only if the value (or attributes) has changed. Non-finite values
   * (`NaN`/`Inf`) are ignored. Also see `publish()`.
   *
   * @param value Value in the unit specified in `Configuration`.
   * @param attributes Optional JSON attributes to publish with the value (`Configuration::with_attributes` must be
   * `true`).
   */
  void update(double value, const Attributes::Map &attributes = {});

  /**
   * @brief Publish a string sensor value regardless of whether the value has changed. Also see `update()`.
   *
   * @param value String payload (e.g. `"ON"`/`"OFF"` for binary sensors, ISO 8601 timestamp, enum option, or text).
   * @param attributes Optional JSON attributes to publish with the value (`Configuration::with_attributes` must be
   * `true`).
   */
  void publish(const std::string &value, const Attributes::Map &attributes = {});

  /**
   * @brief Publish a string sensor value only if the value (or attributes) has changed. Also see `publish()`.
   *
   * @param value String payload (e.g. `"ON"`/`"OFF"` for binary sensors, ISO 8601 timestamp, enum option, or text).
   * @param attributes Optional JSON attributes to publish with the value (`Configuration::with_attributes` must be
   * `true`).
   */
  void update(const std::string &value, const Attributes::Map &attributes = {});

  /**
   * @brief Publish JSON attributes only (`Configuration::with_attributes` must be `true`), regardless of whether they
   * have changed. Also see `updateAttributes()`.
   *
   * @param attributes Attributes map to publish.
   */
  void publishAttributes(const Attributes::Map &attributes);

  /**
   * @brief Publish JSON attributes only if they have changed (`Configuration::with_attributes` must be `true`). Also
   * see `publishAttributes()`.
   *
   * @param attributes Attributes map to publish.
   */
  void updateAttributes(const Attributes::Map &attributes);

protected:
  bool buildComponentConfiguration(ComponentConfig &out) override;

private:
  std::string _name;
  HaBridge &_ha_bridge;
  std::string _object_id;
  std::string _component;
  std::string _child_object_id;
  std::optional<std::string> _device_class;
  std::optional<std::string> _unit_of_measurement;
  std::optional<std::string> _state_class;
  std::optional<std::string> _icon;
  std::optional<std::string> _entity_category;
  std::optional<uint32_t> _expire_after;
  std::optional<uint32_t> _off_delay;
  std::optional<uint8_t> _suggested_display_precision;
  std::optional<bool> _enabled_by_default;
  std::vector<std::string> _options;
  bool _with_attributes;
  bool _force_update;

private:
  std::optional<std::string> _value;
  std::optional<Attributes::Map> _attributes;
};

#endif // __HA_ENTITY_SENSOR_H__
