#ifndef __HA_ENTITY_DOOR_H__
#define __HA_ENTITY_DOOR_H__

#include "HaDeviceClasses.h"
#include "HaEntitySensor.h"
#include <HaBridge.h>
#include <HaEntity.h>
#include <cstdint>
#include <optional>
#include <string>

/**
 * @brief Represent a Door binary sensor (if a door is open or closed).
 */
class HaEntityDoor : public HaEntity {
public:
  /**
   * @brief Construct a new Ha Entity Door object
   *
   * @param name this is the human readable name that will be used for the entity in Home Assistant. If a device is set
   * when creating the [HaBridge], the friendly named displayed in the UI will be the device name plus this name.
   * Example: if device name is "Bathroom" and entity name "door", friendly name will be "Bathroom door". If no
   * device, friendly name will be just "door". If a device is set, this name can be left empty if this entity is the
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
   */
  HaEntityDoor(HaBridge &ha_bridge, std::string name, std::optional<std::string> child_object_id = std::nullopt)
      : _ha_entity_sensor(HaEntitySensor(ha_bridge, name, child_object_id,
                                         HaEntitySensor::Configuration{
                                             .device_class = _door,
                                             .state_class = std::nullopt,
                                         })) {}

public:
  void publishConfiguration() override { _ha_entity_sensor.publishConfiguration(); }
  void republishState() override { _ha_entity_sensor.republishState(); }

  /**
   * @brief Publish the door. This will publish to MQTT regardless if the value has changed. Also see
   * updateDoor().
   *
   * @param open true or false if door is open or not.
   */
  void publishDoor(bool open) { _ha_entity_sensor.publishValue(open ? "ON" : "OFF"); }

  /**
   * @brief Publish the door, but only if the value has changed. Also see publishDoor().
   *
   * @param open true or false if door is open or not.
   */
  void updateDoor(bool open) { _ha_entity_sensor.updateValue(open ? "ON" : "OFF"); }

private:
  const homeassistantentities::BinarySensor::Door _door;
  HaEntitySensor _ha_entity_sensor;
};

#endif // __HA_ENTITY_DOOR_H__