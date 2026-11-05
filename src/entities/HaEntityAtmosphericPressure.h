// Copyright (c) Johan Böhlin
// SPDX-License-Identifier: MIT

#ifndef __HA_ENTITY_ATMOSPHERIC_PRESSURE_H__
#define __HA_ENTITY_ATMOSPHERIC_PRESSURE_H__

#include "HaDeviceClasses.h"
#include "HaEntitySensor.h"
#include <HaBridge.h>
#include <HaEntity.h>
#include <cstdint>
#include <optional>
#include <string>

/**
 * @brief Represent an Atmospheric pressure sensor (see
 * homeassistantentities::Sensor::AtmosphericPressure::Unit in HaDeviceClasses.h).
 */
class HaEntityAtmosphericPressure : public HaEntity {
public:
  using Unit = homeassistantentities::Sensor::AtmosphericPressure::Unit;

  struct Configuration {
    /**
     * @brief the unit of measurement reported for this sensor. Make sure that the value you publish is of this unit.
     */
    Unit unit = Unit::hPa;

    /**
     * @brief In Home Assistant, trigger events even if the sensor's state hasn't changed. Useful if you want
     * to have meaningful value graphs in history or want to create an automation that triggers on every incoming state
     * message (not only when the sensor’s new state is different to the current one).
     */
    bool force_update = false;
  };

  inline static Configuration _default = {.unit = Unit::hPa, .force_update = false};

  /**
   * @brief Construct a new Ha Entity AtmosphericPressure object
   *
   * @param name this is the human readable name that will be used for the entity in Home Assistant. If a device is set
   * when creating the [HaBridge], the friendly named displayed in the UI will be the device name plus this name.
   * Example: if device name is "Bathroom" and entity name "pressure", friendly name will be "Bathroom pressure". If no
   * device, friendly name will be just "pressure". If a device is set, this name can be left empty if this entity is
   * the one main entity (or only) entity of this device. See
   * https://developers.home-assistant.io/docs/core/entity/#entity-naming for more
   * information.
   * @param child_object_id optional child identifier for this entity in case there are several sensors of the same
   * entity type for the same node ID. Leave as empty string for no child object ID. Valid characters
   * are [a-zA-Z0-9_-] (machine readable, not human readable)
   * @param configuration the configuration for this entity.
   */
  HaEntityAtmosphericPressure(HaBridge &ha_bridge, std::string name,
                              std::optional<std::string> child_object_id = std::nullopt,
                              Configuration configuration = _default)
      : _ha_entity_sensor(ha_bridge, name, child_object_id,
                          HaEntitySensor::Configuration{
                              .device_class = homeassistantentities::Sensor::AtmosphericPressure{},
                              .unit_of_measurement = configuration.unit,
                              .force_update = configuration.force_update,
                          }) {}

public:
  void publishConfiguration() override { _ha_entity_sensor.publishConfiguration(); }
  void unpublishConfiguration() override { _ha_entity_sensor.unpublishConfiguration(); }
  void republishState() override { _ha_entity_sensor.republishState(); }

  /**
   * @brief Publish the pressure. This will publish to MQTT regardless if the value has changed. Also see
   * updateAtmosphericPressure().
   *
   * @param pressure pressure in the unit specified in the configuration.
   */
  void publishAtmosphericPressure(double pressure) { _ha_entity_sensor.publishValue(pressure); }

  /**
   * @brief Publish the pressure, but only if the value has changed. Also see publishAtmosphericPressure().
   *
   * @param pressure pressure in the unit specified in the configuration.
   */
  void updateAtmosphericPressure(double pressure) { _ha_entity_sensor.updateValue(pressure); }

protected:
  bool buildComponentConfiguration(ComponentConfig &out) override {
    return buildChildComponentConfiguration(_ha_entity_sensor, out);
  }

private:
  HaEntitySensor _ha_entity_sensor;
};

#endif // __HA_ENTITY_ATMOSPHERIC_PRESSURE_H__
