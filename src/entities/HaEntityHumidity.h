// Copyright (c) Johan Böhlin
// SPDX-License-Identifier: MIT

#ifndef __HA_ENTITY_HUMIDITY_H__
#define __HA_ENTITY_HUMIDITY_H__

#include "HaDeviceClasses.h"
#include "HaEntitySensor.h"
#include <HaBridge.h>
#include <HaEntity.h>
#include <cstdint>
#include <optional>
#include <string>

/**
 * @brief Represents a Relative Air Humidity MQTT Sensor (`sensor.mqtt` with `device_class: "humidity"`, unit `"%"`).
 *
 * See https://www.home-assistant.io/integrations/sensor.mqtt/
 *
 * @par Example
 * @code
 * HaEntityHumidity humidity(ha_bridge, "Humidity", std::nullopt, {
 *     .suggested_display_precision = 1,
 * });
 *
 * // Once MQTT is connected:
 * humidity.publishConfiguration();
 * humidity.update(48.5); // 48.5 %
 * @endcode
 */
class HaEntityHumidity : public HaEntity {
public:
  struct Configuration {
    /**
     * @brief In Home Assistant, trigger state events even if the sensor's state has not changed. Useful for graphing
     * repeated identical measurements or triggering automations on every incoming state message.
     */
    bool force_update = false;

    /**
     * @brief Optional custom icon (e.g. `"mdi:water-percent"`).
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
   * @brief Construct a new HaEntityHumidity object.
   *
   * @param ha_bridge Bridge for MQTT and Home Assistant.
   * @param name Human-readable name used for the entity in Home Assistant. If a device is set on `ha_bridge`, the
   * friendly name displayed in the UI is the device name plus this name (e.g. device `"Bathroom"` + name `"Humidity"`
   * -> `"Bathroom Humidity"`). Can be empty if a device is set and this is the main (or only) entity of the device. See
   * https://developers.home-assistant.io/docs/core/entity/#entity-naming for details.
   * @param child_object_id Optional machine-readable child identifier (`[a-zA-Z0-9_-]`) in case there are several
   * humidity sensors on the same node ID. Pass `std::nullopt` or `""` when not needed.
   * @param configuration Configuration for this sensor.
   */
  HaEntityHumidity(HaBridge &ha_bridge, std::string name, std::optional<std::string> child_object_id = std::nullopt,
                   Configuration configuration = _default)
      : _ha_entity_sensor(ha_bridge, name, child_object_id,
                          HaEntitySensor::Configuration{
                              .device_class = homeassistantentities::Sensor::Humidity{},
                              .unit_of_measurement = homeassistantentities::Sensor::Humidity::Unit::Percent,
                              .icon = configuration.icon,
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
   * @brief Publish the relative humidity regardless of whether the value has changed. Also see `update()`.
   *
   * @param humidity Relative humidity in `%`.
   */
  void publish(double humidity) { _ha_entity_sensor.publish(humidity); }

  /**
   * @brief Publish the relative humidity only if the value has changed. Also see `publish()`.
   *
   * @param humidity Relative humidity in `%`.
   */
  void update(double humidity) { _ha_entity_sensor.update(humidity); }

protected:
  bool buildComponentConfiguration(ComponentConfig &out) override {
    return buildChildComponentConfiguration(_ha_entity_sensor, out);
  }

private:
  HaEntitySensor _ha_entity_sensor;
};

#endif // __HA_ENTITY_HUMIDITY_H__
