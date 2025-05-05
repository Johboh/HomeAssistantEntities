#ifndef __HA_ENTITY_CURRENT_H__
#define __HA_ENTITY_CURRENT_H__

#include "HaDeviceClasses.h"
#include "HaEntitySensor.h"
#include <HaBridge.h>
#include <HaEntity.h>
#include <cstdint>
#include <optional>
#include <string>

/**
 * @brief Represent a Current sensor (see homeassistantentities::Sensor::Current:Unit in HaDeviceClasses.h).
 */
class HaEntityCurrent : public HaEntity {
public:
  using Unit = homeassistantentities::Sensor::Current::Unit;

  struct Configuration {
    /**
     * @brief the unit of measurement reported for this sensor. Make sure that the value you publish is of this unit.
     */
    Unit unit = Unit::A;

    /**
     * In Home Assistant, trigger events even if the sensor's state hasn't changed. Useful if you want
     * to have meaningful value graphs in history or want to create an automation that triggers on every incoming state
     * message (not only when the sensor’s new state is different to the current one).
     */
    bool force_update = false;
  };

  inline static Configuration _default = {.unit = Unit::A, .force_update = false};

  /**
   * @brief Construct a new Ha Entity Current object
   *
   * @param name this is the human readable name that will be used for the entity in Home Assistant. If a device is set
   * when creating the [HaBridge], the friendly named displayed in the UI will be the device name plus this name.
   * Example: if device name is "Bathroom" and entity name "current", friendly name will be "Bathroom current". If no
   * device, friendly name will be just "current". If a device is set, this name can be left empty if this entity is the
   * one main entity (or only) entity of this device. See
   * https://developers.home-assistant.io/docs/core/entity/#entity-naming for more
   * information.
   * @param child_object_id optional child identifier for this entity in case there are several sensors of the same
   * entity type for the same node ID. Example: If you have a lock for the node ID "door", the home asisstant
   * configuration path will be "homeassistant/binary_sensor/door/lock/config". This works if you only have one lock on
   * your door, but if you have two locks, you want to add a child object ID to them. By setting the child_object_id to
   * say "upper", the configuration will be "homeassistant/binary_sensor/door/lock/upper/config". This also apply for
   * all state/command topics and so on. Leave as empty string for no child object ID. Valid characters
   * are [a-zA-Z0-9_-] (machine readable, not human readable)
   * @param configuration the configuration for this entity.
   */
  HaEntityCurrent(HaBridge &ha_bridge, std::string name, std::optional<std::string> child_object_id = std::nullopt,
                  Configuration configuration = _default)
      : _ha_entity_sensor(HaEntitySensor(ha_bridge, name, child_object_id,
                                         HaEntitySensor::Configuration{
                                             .device_class = _current,
                                             .unit_of_measurement = configuration.unit,
                                             .force_update = configuration.force_update,
                                         })) {}

public:
  void publishConfiguration() override { _ha_entity_sensor.publishConfiguration(); }
  void republishState() override { _ha_entity_sensor.republishState(); }

  /**
   * @brief Publish the current. This will publish to MQTT regardless if the value has changed. Also see
   * updateCurrent().
   *
   * @param current current in the unit specified in the configuration.
   */
  void publishCurrent(double current) { _ha_entity_sensor.publishValue(current); }

  /**
   * @brief Publish the current, but only if the value has changed. Also see publishCurrent().
   *
   * @param current current in the unit specified in the configuration.
   */
  void updateCurrent(double current) { _ha_entity_sensor.updateValue(current); }

private:
  const homeassistantentities::Sensor::Current _current;
  HaEntitySensor _ha_entity_sensor;
};

#endif // __HA_ENTITY_CURRENT_H__