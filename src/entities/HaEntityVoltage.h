#ifndef __HA_ENTITY_VOLTAGE_H__
#define __HA_ENTITY_VOLTAGE_H__

#include "HaDeviceClasses.h"
#include "HaEntitySensor.h"
#include <HaBridge.h>
#include <HaEntity.h>
#include <cstdint>
#include <optional>
#include <string>

/**
 * @brief Represent a Voltage sensor (see homeassistantentities::Sensor::Voltage::Unit in HaDeviceClasses.h).
 */
class HaEntityVoltage : public HaEntity {
public:
  using Unit = homeassistantentities::Sensor::Voltage::Unit;

  struct Configuration {
    /**
     * @brief the unit of measurement reported for this sensor. Make sure that the value you publish is of this unit.
     */
    Unit unit = Unit::V;

    /**
     * In Home Assistant, trigger events even if the sensor's state hasn't changed. Useful if you want
     * to have meaningful value graphs in history or want to create an automation that triggers on every incoming state
     * message (not only when the sensor’s new state is different to the current one).
     */
    bool force_update = false;
  };

  inline static Configuration _default = {.unit = Unit::V, .force_update = false};

  /**
   * @brief Construct a new Ha Entity Voltage object
   *
   * @param name this is the human readable name that will be used for the entity in Home Assistant. If a device is set
   * when creating the [HaBridge], the friendly named displayed in the UI will be the device name plus this name.
   * Example: if device name is "Bathroom" and entity name "voltage", friendly name will be "Bathroom voltage". If no
   * device, friendly name will be just "voltage". If a device is set, this name can be left empty if this entity is the
   * one main entity (or only) entity of this device. See
   * https://developers.home-assistant.io/docs/core/entity/#entity-naming for more
   * information.
   * @param child_object_id optional child identifier for this entity in case there are several sensors of the same
   * entity type for the same node ID. Leave as empty string for no child object ID. Valid characters
   * are [a-zA-Z0-9_-] (machine readable, not human readable)
   * @param configuration the configuration for this entity.
   */
  HaEntityVoltage(HaBridge &ha_bridge, std::string name, std::optional<std::string> child_object_id = std::nullopt,
                  Configuration configuration = _default)
      : _ha_entity_sensor(ha_bridge, name, child_object_id,
                          HaEntitySensor::Configuration{
                              .device_class = homeassistantentities::Sensor::Voltage{},
                              .unit_of_measurement = configuration.unit,
                              .force_update = configuration.force_update,
                          }) {}

public:
  void publishConfiguration() override { _ha_entity_sensor.publishConfiguration(); }
  void republishState() override { _ha_entity_sensor.republishState(); }

  /**
   * @brief Publish the voltage. This will publish to MQTT regardless if the value has changed. Also see
   * updateVoltage().
   *
   * @param voltage voltage in the unit specified in the configuration.
   */
  void publishVoltage(double voltage) { _ha_entity_sensor.publishValue(voltage); }

  /**
   * @brief Publish the voltage, but only if the value has changed. Also see publishVoltage().
   *
   * @param voltage voltage in the unit specified in the configuration.
   */
  void updateVoltage(double voltage) { _ha_entity_sensor.updateValue(voltage); }

private:
  HaEntitySensor _ha_entity_sensor;
};

#endif // __HA_ENTITY_VOLTAGE_H__
