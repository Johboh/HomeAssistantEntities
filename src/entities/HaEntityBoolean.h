// Copyright (c) Johan Böhlin
// SPDX-License-Identifier: MIT

#ifndef __HA_ENTITY_BOOLEAN_H__
#define __HA_ENTITY_BOOLEAN_H__

#include "AttributeVariants.h"
#include "HaEntitySensor.h"
#include <HaBridge.h>
#include <HaEntity.h>
#include <cstdint>
#include <optional>
#include <string>

/**
 * @brief Represents a generic boolean MQTT Binary Sensor (`binary_sensor.mqtt` with no specific `device_class`).
 *
 * Publishes `"ON"` when `true` and `"OFF"` when `false`, with optional JSON attributes. For typed binary sensors, see
 * `HaEntityDoor`, `HaEntityMotion`, `HaEntitySound`, or `HaEntitySensor` with `BinarySensor::*`.
 * See https://www.home-assistant.io/integrations/binary_sensor.mqtt/
 *
 * @par Example
 * @code
 * HaEntityBoolean leak_alarm(ha_bridge, "Filter Active", "filter", {
 *     .with_attributes = true,
 * });
 *
 * // Once MQTT is connected:
 * leak_alarm.publishConfiguration();
 * leak_alarm.update(true, {{"runtime_s", 3600U}});
 * @endcode
 */
class HaEntityBoolean : public HaEntity {
public:
  struct Configuration {
    /**
     * @brief If `true`, set up a JSON attributes topic (`json_attributes_topic`) so attributes can be published along
     * with the boolean state or separately.
     */
    bool with_attributes = false;

    /**
     * @brief In Home Assistant, trigger state events even if the sensor's state has not changed. Useful for graphing
     * repeated states or triggering automations on every incoming state message.
     */
    bool force_update = false;

    /**
     * @brief Optional custom icon (e.g. `"mdi:check-circle"`).
     */
    std::optional<std::string> icon = std::nullopt;

    /**
     * @brief Optional entity category (`"config"` or `"diagnostic"`).
     */
    std::optional<std::string> entity_category = std::nullopt;

    /**
     * @brief Optional number of seconds after which the sensor's state expires if not updated.
     */
    std::optional<uint32_t> expire_after = std::nullopt;

    /**
     * @brief Number of seconds after turning on before Home Assistant automatically resets the binary sensor state back
     * to off.
     */
    std::optional<uint32_t> off_delay = std::nullopt;

    /**
     * @brief Defines whether the entity should be enabled when first added to Home Assistant. Defaults to `true` in
     * Home Assistant when omitted.
     */
    std::optional<bool> enabled_by_default = std::nullopt;
  };

  inline static Configuration _default = {.with_attributes = false,
                                          .force_update = false,
                                          .icon = std::nullopt,
                                          .entity_category = std::nullopt,
                                          .expire_after = std::nullopt,
                                          .off_delay = std::nullopt,
                                          .enabled_by_default = std::nullopt};

  /**
   * @brief Construct a new HaEntityBoolean object.
   *
   * @param ha_bridge Bridge for MQTT and Home Assistant.
   * @param name Human-readable name used for the entity in Home Assistant. If a device is set on `ha_bridge`, the
   * friendly name displayed in the UI is the device name plus this name (e.g. device `"Bathroom"` + name `"Active"` ->
   * `"Bathroom Active"`). Can be empty if a device is set and this is the main (or only) entity of the device. See
   * https://developers.home-assistant.io/docs/core/entity/#entity-naming for details.
   * @param child_object_id Optional machine-readable child identifier (`[a-zA-Z0-9_-]`) in case there are several
   * boolean binary sensors on the same node ID. For example, for node `"pump"`, the default discovery topic is
   * `"homeassistant/binary_sensor/pump/boolean/config"`; setting `child_object_id` to `"aux"` yields
   * `"homeassistant/binary_sensor/pump/boolean_aux/config"`. Pass `std::nullopt` or `""` when not needed.
   * @param configuration Configuration for this binary sensor.
   */
  HaEntityBoolean(HaBridge &ha_bridge, std::string name, std::optional<std::string> child_object_id = std::nullopt,
                  Configuration configuration = _default)
      : _ha_entity_sensor(ha_bridge, name, child_object_id,
                          HaEntitySensor::Configuration{
                              .device_class = homeassistantentities::BinarySensor::Undefined::Boolean{},
                              .state_class = std::nullopt,
                              .with_attributes = configuration.with_attributes,
                              .icon = configuration.icon,
                              .force_update = configuration.force_update,
                              .entity_category = configuration.entity_category,
                              .expire_after = configuration.expire_after,
                              .off_delay = configuration.off_delay,
                              .enabled_by_default = configuration.enabled_by_default,
                          }) {}

public:
  void publishConfiguration() override { _ha_entity_sensor.publishConfiguration(); }
  void unpublishConfiguration() override { _ha_entity_sensor.unpublishConfiguration(); }
  void republishState() override { _ha_entity_sensor.republishState(); }

  /**
   * @brief Publish the boolean state regardless of whether the value has changed. Also see `update()`.
   *
   * @param value Boolean state to publish (`true` -> `"ON"`, `false` -> `"OFF"`).
   * @param attributes Optional JSON attributes to send with the state (`Configuration::with_attributes` must be
   * `true`).
   */
  void publish(bool value, const Attributes::Map &attributes = {}) {
    _ha_entity_sensor.publish(value ? "ON" : "OFF", attributes);
  }

  /**
   * @brief Publish the boolean state only if the value (or attributes) has changed. Also see `publish()`.
   *
   * @param value Boolean state to publish (`true` -> `"ON"`, `false` -> `"OFF"`).
   * @param attributes Optional JSON attributes to send with the state (`Configuration::with_attributes` must be
   * `true`).
   */
  void update(bool value, const Attributes::Map &attributes = {}) {
    _ha_entity_sensor.update(value ? "ON" : "OFF", attributes);
  }

  /**
   * @brief Publish JSON attributes only (`Configuration::with_attributes` must be `true`), regardless of whether they
   * have changed. Also see `updateAttributes()`.
   *
   * @param attributes Attributes map to publish.
   */
  void publishAttributes(const Attributes::Map &attributes) { _ha_entity_sensor.publishAttributes(attributes); }

  /**
   * @brief Publish JSON attributes only if they have changed (`Configuration::with_attributes` must be `true`). Also
   * see `publishAttributes()`.
   *
   * @param attributes Attributes map to publish.
   */
  void updateAttributes(const Attributes::Map &attributes) { _ha_entity_sensor.updateAttributes(attributes); }

protected:
  bool buildComponentConfiguration(ComponentConfig &out) override {
    return buildChildComponentConfiguration(_ha_entity_sensor, out);
  }

private:
  HaEntitySensor _ha_entity_sensor;
};

#endif // __HA_ENTITY_BOOLEAN_H__
