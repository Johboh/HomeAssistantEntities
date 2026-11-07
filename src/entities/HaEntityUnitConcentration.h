// Copyright (c) Johan Böhlin
// SPDX-License-Identifier: MIT

#ifndef __HA_ENTITY_UNIT_CONCENTRATION_H__
#define __HA_ENTITY_UNIT_CONCENTRATION_H__

#include "HaDeviceClasses.h"
#include "HaEntitySensor.h"
#include <HaBridge.h>
#include <HaEntity.h>
#include <cstdint>
#include <optional>
#include <string>

/**
 * @brief Represents a particle/number concentration per volume MQTT Sensor (`sensor.mqtt`).
 *
 * Defaults to `Unit::dL` (`"/dL"`, e.g. particle count per deciliter from PMS5003); see
 * `homeassistantentities::Sensor::Undefined::UnitConcentration::Unit` in `HaDeviceClasses.h` (`mL`, `dL`, `L`).
 * See https://www.home-assistant.io/integrations/sensor.mqtt/
 *
 * @par Example
 * @code
 * HaEntityUnitConcentration particle_count(ha_bridge, "Particles >0.3um", "p03", {
 *     .unit = HaEntityUnitConcentration::Unit::dL,
 *     .suggested_display_precision = 0,
 * });
 *
 * // Once MQTT is connected:
 * particle_count.publishConfiguration();
 * particle_count.update(512.0); // 512 /dL
 * @endcode
 */
class HaEntityUnitConcentration : public HaEntity {
public:
  using Unit = homeassistantentities::Sensor::Undefined::UnitConcentration::Unit;

  struct Configuration {
    /**
     * @brief Unit of measurement reported for this sensor (default `Unit::dL`). Ensure published values match this
     * unit.
     */
    Unit unit = Unit::dL;

    /**
     * @brief In Home Assistant, trigger state events even if the sensor's state has not changed. Useful for graphing
     * repeated identical measurements or triggering automations on every incoming state message.
     */
    bool force_update = false;

    /**
     * @brief Optional custom icon (e.g. `"mdi:blur"`).
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

  inline static Configuration _default = {.unit = Unit::dL,
                                          .force_update = false,
                                          .icon = std::nullopt,
                                          .entity_category = std::nullopt,
                                          .expire_after = std::nullopt,
                                          .suggested_display_precision = std::nullopt,
                                          .enabled_by_default = std::nullopt};

  /**
   * @brief Construct a new HaEntityUnitConcentration object.
   *
   * @param ha_bridge Bridge for MQTT and Home Assistant.
   * @param name Human-readable name used for the entity in Home Assistant. If a device is set on `ha_bridge`, the
   * friendly name displayed in the UI is the device name plus this name (e.g. device `"Air Monitor"` + name
   * `"Particles"` -> `"Air Monitor Particles"`). Can be empty if a device is set and this is the main (or only) entity
   * of the device. See https://developers.home-assistant.io/docs/core/entity/#entity-naming for details.
   * @param child_object_id Optional machine-readable child identifier (`[a-zA-Z0-9_-]`) in case there are several
   * unit-concentration sensors on the same node ID. Pass `std::nullopt` or `""` when not needed.
   * @param configuration Configuration for this sensor.
   */
  HaEntityUnitConcentration(HaBridge &ha_bridge, std::string name,
                            std::optional<std::string> child_object_id = std::nullopt,
                            Configuration configuration = _default)
      : _ha_entity_sensor(ha_bridge, name, child_object_id,
                          HaEntitySensor::Configuration{
                              .device_class = homeassistantentities::Sensor::Undefined::UnitConcentration{},
                              .unit_of_measurement = configuration.unit,
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
   * @brief Publish the concentration regardless of whether the value has changed. Also see `update()`.
   *
   * @param concentration Concentration in the unit specified in `Configuration::unit`.
   */
  void publish(double concentration) { _ha_entity_sensor.publish(concentration); }

  /**
   * @brief Publish the concentration only if the value has changed. Also see `publish()`.
   *
   * @param concentration Concentration in the unit specified in `Configuration::unit`.
   */
  void update(double concentration) { _ha_entity_sensor.update(concentration); }

protected:
  bool buildComponentConfiguration(ComponentConfig &out) override {
    return buildChildComponentConfiguration(_ha_entity_sensor, out);
  }

private:
  HaEntitySensor _ha_entity_sensor;
};

#endif // __HA_ENTITY_UNIT_CONCENTRATION_H__
