// Copyright (c) Johan Böhlin
// SPDX-License-Identifier: MIT

#ifndef __HA_ENTITY_PARTICULATE_MATTER_H__
#define __HA_ENTITY_PARTICULATE_MATTER_H__

#include "HaDeviceClasses.h"
#include "HaEntitySensor.h"
#include <HaBridge.h>
#include <HaEntity.h>
#include <cstdint>
#include <optional>
#include <string>

/**
 * @brief Represent a particulate matter sensor (e.g. PMS5003), μg/m³.
 */
class HaEntityParticulateMatter : public HaEntity {
public:
  enum class Size {
    pm1,  // Concentration of particulate matter less than 1 micrometer in µg/m³
    pm25, // Concentration of particulate matter less than 2.5 micrometers in µg/m³
    pm10, // Concentration of particulate matter less than 10 micrometers in µg/m³
    pm4,  // Concentration of particulate matter less than 4 micrometers in µg/m³
  };

  struct Configuration {
    /**
     * @brief particulate matter size.
     */
    Size size = Size::pm10;

    /**
     * In Home Assistant, trigger events even if the sensor's state hasn't changed. Useful if you want
     * to have meaningful value graphs in history or want to create an automation that triggers on every incoming state
     * message (not only when the sensor’s new state is different to the current one).
     */
    bool force_update = false;
  };

  inline static Configuration _default = {.size = Size::pm10, .force_update = false};

  /**
   * @brief Construct a new Ha Entity Particulate Matter object
   *
   * @param name this is the human readable name that will be used for the entity in Home Assistant. If a device is set
   * when creating the [HaBridge], the friendly named displayed in the UI will be the device name plus this name.
   * Example: if device name is "Bathroom" and entity name "particles", friendly name will be "Bathroom particles".
   * If no device, friendly name will be just "particles". If a device is set, this name can be left empty if this
   * entity is the one main entity (or only) entity of this device. See
   * https://developers.home-assistant.io/docs/core/entity/#entity-naming for more
   * information.
   * @param child_object_id optional child identifier for this entity in case there are several sensors of the same
   * entity type for the same node ID. Example: If you have a lock for the node ID "door", the home assistant
   * configuration path will be "homeassistant/binary_sensor/door/lock/config". This works if you only have one lock on
   * your door, but if you have two locks, you want to add a child object ID to them. By setting the child_object_id to
   * say "upper", the configuration will be "homeassistant/binary_sensor/door/lock/upper/config". This also apply for
   * all state/command topics and so on. Leave as empty string for no child object ID. Valid characters
   * are [a-zA-Z0-9_-] (machine readable, not human readable)
   * @param configuration the configuration for this entity.
   */
  HaEntityParticulateMatter(HaBridge &ha_bridge, std::string name,
                            std::optional<std::string> child_object_id = std::nullopt,
                            Configuration configuration = _default)
      : _ha_entity_sensor(makeSensor(ha_bridge, name, child_object_id, configuration)) {}

public:
  void publishConfiguration() override { _ha_entity_sensor.publishConfiguration(); }
  void republishState() override { _ha_entity_sensor.republishState(); }

  /**
   * @brief Publish the particle concentration. This will publish to MQTT regardless if the value has changed. Also see
   * updateConcentration().
   *
   * @param concentration concentration of particles.
   */
  void publishConcentration(double concentration) { _ha_entity_sensor.publishValue(concentration); }

  /**
   * @brief Publish the particle concentration, but only if the value has changed. Also see publishConcentration().
   *
   * @param concentration concentration of particles.
   */
  void updateConcentration(double concentration) { _ha_entity_sensor.updateValue(concentration); }

private:
  static HaEntitySensor makeSensor(HaBridge &ha_bridge, const std::string &name,
                                   const std::optional<std::string> &child_object_id,
                                   const Configuration &configuration) {
    switch (configuration.size) {
    case Size::pm1:
      return HaEntitySensor(ha_bridge, name, child_object_id,
                            HaEntitySensor::Configuration{
                                .device_class = homeassistantentities::Sensor::Pm1{},
                                .unit_of_measurement = homeassistantentities::Sensor::Pm1::Unit::ug_m3,
                                .force_update = configuration.force_update,
                            });
    case Size::pm25:
      return HaEntitySensor(ha_bridge, name, child_object_id,
                            HaEntitySensor::Configuration{
                                .device_class = homeassistantentities::Sensor::Pm25{},
                                .unit_of_measurement = homeassistantentities::Sensor::Pm25::Unit::ug_m3,
                                .force_update = configuration.force_update,
                            });
    case Size::pm4:
      return HaEntitySensor(ha_bridge, name, child_object_id,
                            HaEntitySensor::Configuration{
                                .device_class = homeassistantentities::Sensor::Pm4{},
                                .unit_of_measurement = homeassistantentities::Sensor::Pm4::Unit::ug_m3,
                                .force_update = configuration.force_update,
                            });
    case Size::pm10:
      break;
    }
    return HaEntitySensor(ha_bridge, name, child_object_id,
                          HaEntitySensor::Configuration{
                              .device_class = homeassistantentities::Sensor::Pm10{},
                              .unit_of_measurement = homeassistantentities::Sensor::Pm10::Unit::ug_m3,
                              .force_update = configuration.force_update,
                          });
  }

private:
  HaEntitySensor _ha_entity_sensor;
};

#endif // __HA_ENTITY_PARTICULATE_MATTER_H__
