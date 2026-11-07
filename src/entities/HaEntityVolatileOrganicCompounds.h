// Copyright (c) Johan Böhlin
// SPDX-License-Identifier: MIT

#ifndef __HA_ENTITY_VOLATILE_ORGANIC_COMPOUNDS_H__
#define __HA_ENTITY_VOLATILE_ORGANIC_COMPOUNDS_H__

#include "HaDeviceClasses.h"
#include "HaEntitySensor.h"
#include <HaBridge.h>
#include <HaEntity.h>
#include <cstdint>
#include <optional>
#include <string>

/**
 * @brief Represents a Volatile Organic Compounds (VOC) MQTT Sensor (`sensor.mqtt`), supporting either parts per billion
 * (`Unit::Parts` -> `device_class: "volatile_organic_compounds_parts"`, `"ppb"`) or mass concentration
 * (`Unit::Concentration` -> `device_class: "volatile_organic_compounds"`, `"µg/m³"`).
 *
 * See https://www.home-assistant.io/integrations/sensor.mqtt/
 *
 * @par Example
 * @code
 * HaEntityVolatileOrganicCompounds tvoc(ha_bridge, "TVOC", std::nullopt, {
 *     .unit = HaEntityVolatileOrganicCompounds::Unit::Parts,
 *     .suggested_display_precision = 0,
 * });
 *
 * // Once MQTT is connected:
 * tvoc.publishConfiguration();
 * tvoc.update(125.0); // 125 ppb
 * @endcode
 */
class HaEntityVolatileOrganicCompounds : public HaEntity {
public:
  enum class Unit {
    Concentration, // Concentration of volatile organic compounds in µg/m³
    Parts,         // Ratio of volatile organic compounds in ppb (parts per billion)
  };

  struct Configuration {
    /**
     * @brief Unit mode for this VOC sensor: either mass concentration (`Unit::Concentration`, `"µg/m³"`) or parts ratio
     * (`Unit::Parts`, `"ppb"`, default).
     */
    Unit unit = Unit::Parts;

    /**
     * @brief In Home Assistant, trigger state events even if the sensor's state has not changed. Useful for graphing
     * repeated identical measurements or triggering automations on every incoming state message.
     */
    bool force_update = false;

    /**
     * @brief Optional custom icon (e.g. `"mdi:molecule"`).
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

  inline static Configuration _default = {.unit = Unit::Parts,
                                          .force_update = false,
                                          .icon = std::nullopt,
                                          .entity_category = std::nullopt,
                                          .expire_after = std::nullopt,
                                          .suggested_display_precision = std::nullopt,
                                          .enabled_by_default = std::nullopt};

  /**
   * @brief Construct a new HaEntityVolatileOrganicCompounds object.
   *
   * @param ha_bridge Bridge for MQTT and Home Assistant.
   * @param name Human-readable name used for the entity in Home Assistant. If a device is set on `ha_bridge`, the
   * friendly name displayed in the UI is the device name plus this name (e.g. device `"Office"` + name `"TVOC"` ->
   * `"Office TVOC"`). Can be empty if a device is set and this is the main (or only) entity of the device. See
   * https://developers.home-assistant.io/docs/core/entity/#entity-naming for details.
   * @param child_object_id Optional machine-readable child identifier (`[a-zA-Z0-9_-]`) in case there are several VOC
   * sensors of the same `Unit` on the same node ID. Pass `std::nullopt` or `""` when not needed.
   * @param configuration Configuration for this sensor.
   */
  HaEntityVolatileOrganicCompounds(HaBridge &ha_bridge, std::string name,
                                   std::optional<std::string> child_object_id = std::nullopt,
                                   Configuration configuration = _default)
      : _ha_entity_sensor(makeSensor(ha_bridge, name, child_object_id, configuration)) {}

public:
  void publishConfiguration() override { _ha_entity_sensor.publishConfiguration(); }
  void unpublishConfiguration() override { _ha_entity_sensor.unpublishConfiguration(); }
  void republishState() override { _ha_entity_sensor.republishState(); }

  /**
   * @brief Publish the VOC concentration or parts ratio regardless of whether the value has changed. Also see
   * `update()`.
   *
   * @param concentration VOC value in `"µg/m³"` (`Unit::Concentration`) or `"ppb"` (`Unit::Parts`) according to
   * `Configuration::unit`.
   */
  void publish(double concentration) { _ha_entity_sensor.publish(concentration); }

  /**
   * @brief Publish the VOC concentration or parts ratio only if the value has changed. Also see `publish()`.
   *
   * @param concentration VOC value in `"µg/m³"` (`Unit::Concentration`) or `"ppb"` (`Unit::Parts`) according to
   * `Configuration::unit`.
   */
  void update(double concentration) { _ha_entity_sensor.update(concentration); }

private:
  static HaEntitySensor makeSensor(HaBridge &ha_bridge, const std::string &name,
                                   const std::optional<std::string> &child_object_id,
                                   const Configuration &configuration) {
    auto makeConfig = [&](const homeassistantentities::DeviceClass &dc, homeassistantentities::UnitType unit) {
      return HaEntitySensor::Configuration{
          .device_class = dc,
          .unit_of_measurement = unit,
          .icon = configuration.icon,
          .force_update = configuration.force_update,
          .entity_category = configuration.entity_category,
          .expire_after = configuration.expire_after,
          .suggested_display_precision = configuration.suggested_display_precision,
          .enabled_by_default = configuration.enabled_by_default,
      };
    };

    if (configuration.unit == Unit::Concentration) {
      return HaEntitySensor(ha_bridge, name, child_object_id,
                            makeConfig(homeassistantentities::Sensor::VolatileOrganicCompounds{},
                                       homeassistantentities::Sensor::VolatileOrganicCompounds::Unit::ug_m3));
    }
    return HaEntitySensor(ha_bridge, name, child_object_id,
                          makeConfig(homeassistantentities::Sensor::VolatileOrganicCompoundsParts{},
                                     homeassistantentities::Sensor::VolatileOrganicCompoundsParts::Unit::ppb));
  }

protected:
  bool buildComponentConfiguration(ComponentConfig &out) override {
    return buildChildComponentConfiguration(_ha_entity_sensor, out);
  }

private:
  HaEntitySensor _ha_entity_sensor;
};

#endif // __HA_ENTITY_VOLATILE_ORGANIC_COMPOUNDS_H__
