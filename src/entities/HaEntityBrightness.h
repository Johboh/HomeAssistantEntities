// Copyright (c) Johan Böhlin
// SPDX-License-Identifier: MIT

#ifndef __HA_ENTITY_BRIGHTNESS_H__
#define __HA_ENTITY_BRIGHTNESS_H__

#include "HaEntitySensor.h"
#include <HaBridge.h>
#include <HaEntity.h>
#include <cstdint>
#include <optional>
#include <string>

/**
 * @brief Represents a relative Brightness percentage MQTT Sensor (`sensor.mqtt`, unit `"%"`, default icon
 * `"mdi:brightness-percent"`).
 *
 * For absolute light level in lux (`lx`), see `HaEntityIlluminance`.
 * See https://www.home-assistant.io/integrations/sensor.mqtt/
 *
 * @par Example
 * @code
 * HaEntityBrightness brightness(ha_bridge, "Brightness");
 *
 * // Once MQTT is connected:
 * brightness.publishConfiguration();
 * brightness.update(75.0); // 75 %
 * @endcode
 */
class HaEntityBrightness : public HaEntity {
public:
  struct Configuration {
    /**
     * @brief In Home Assistant, trigger state events even if the sensor's state has not changed. Useful for graphing
     * repeated identical measurements or triggering automations on every incoming state message.
     */
    bool force_update = false;

    /**
     * @brief Optional custom icon (defaults to `"mdi:brightness-percent"` when unset).
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
     * @brief Number of decimals to use in the Home Assistant frontend when displaying the value.
     */
    std::optional<uint8_t> suggested_display_precision = std::nullopt;

    /**
     * @brief Defines whether the entity should be enabled when first added to Home Assistant. Defaults to `true` in
     * Home Assistant when omitted.
     */
    std::optional<bool> enabled_by_default = std::nullopt;
  };

  inline static Configuration _default = {.force_update = false,
                                          .icon = std::nullopt,
                                          .entity_category = std::nullopt,
                                          .expire_after = std::nullopt,
                                          .suggested_display_precision = std::nullopt,
                                          .enabled_by_default = std::nullopt};

  /**
   * @brief Construct a new HaEntityBrightness object.
   *
   * @param ha_bridge Bridge for MQTT and Home Assistant.
   * @param name Human-readable name used for the entity in Home Assistant. If a device is set on `ha_bridge`, the
   * friendly name displayed in the UI is the device name plus this name (e.g. device `"Living Room"` + name
   * `"Brightness"` -> `"Living Room Brightness"`). Can be empty if a device is set and this is the main (or only)
   * entity of the device. See https://developers.home-assistant.io/docs/core/entity/#entity-naming for details.
   * @param child_object_id Optional machine-readable child identifier (`[a-zA-Z0-9_-]`) in case there are several
   * brightness sensors on the same node ID. For example, for node `"room"`, the default discovery topic is
   * `"homeassistant/sensor/room/brightness/config"`; setting `child_object_id` to `"window"` yields
   * `"homeassistant/sensor/room/brightness_window/config"`. Pass `std::nullopt` or `""` when not needed.
   * @param configuration Configuration for this sensor.
   */
  HaEntityBrightness(HaBridge &ha_bridge, std::string name, std::optional<std::string> child_object_id = std::nullopt,
                     Configuration configuration = _default)
      : _ha_entity_sensor(
            ha_bridge, name, child_object_id,
            HaEntitySensor::Configuration{
                .device_class = homeassistantentities::Sensor::Undefined::Brightness{},
                .unit_of_measurement = homeassistantentities::Sensor::Undefined::Brightness::Unit::Percent,
                .icon = configuration.icon.has_value() ? configuration.icon : "mdi:brightness-percent",
                .force_update = configuration.force_update,
                .entity_category = configuration.entity_category,
                .expire_after = configuration.expire_after,
                .suggested_display_precision = configuration.suggested_display_precision,
                .enabled_by_default = configuration.enabled_by_default,
            }) {}

public:
  void publishConfiguration() override { _ha_entity_sensor.publishConfiguration(); }
  void unpublishConfiguration() override { _ha_entity_sensor.unpublishConfiguration(); }
  void republishState() override { _ha_entity_sensor.republishState(); }

  /**
   * @brief Publish the brightness percentage regardless of whether the value has changed. Also see `update()`.
   *
   * @param brightness Brightness in `%`.
   */
  void publish(double brightness) { _ha_entity_sensor.publish(brightness); }

  /**
   * @brief Publish the brightness percentage only if the value has changed. Also see `publish()`.
   *
   * @param brightness Brightness in `%`.
   */
  void update(double brightness) { _ha_entity_sensor.update(brightness); }

protected:
  bool buildComponentConfiguration(ComponentConfig &out) override {
    return buildChildComponentConfiguration(_ha_entity_sensor, out);
  }

private:
  HaEntitySensor _ha_entity_sensor;
};

#endif // __HA_ENTITY_BRIGHTNESS_H__
