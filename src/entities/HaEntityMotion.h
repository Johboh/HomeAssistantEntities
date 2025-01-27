#ifndef __HA_ENTITY_MOTION_H__
#define __HA_ENTITY_MOTION_H__

#include "HaDeviceClasses.h"
#include "HaEntitySensor.h"
#include <HaBridge.h>
#include <HaEntity.h>
#include <cstdint>
#include <optional>
#include <string>

/**
 * @brief Represent a Motion binary sensor.
 */
class HaEntityMotion : public HaEntity {
public:
  /**
   * @brief Construct a new Ha Entity Motion object
   *
   * @param name this is the human readable name that will be used for the entity in Home Assistant. If a device is set
   * when creating the [HaBridge], the friendly named displayed in the UI will be the device name plus this name.
   * Example: if device name is "Bathroom" and entity name "motion", friendly name will be "Bathroom motion". If no
   * device, friendly name will be just "motion". If a device is set, this name can be left empty if this entity is the
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
  HaEntityMotion(HaBridge &ha_bridge, std::string name, std::optional<std::string> child_object_id = std::nullopt)
      : _ha_entity_sensor(HaEntitySensor(ha_bridge, name, child_object_id,
                                         HaEntitySensor::Configuration{
                                             .device_class = _motion,
                                             .state_class = std::nullopt,
                                         })) {}

public:
  void publishConfiguration() override { _ha_entity_sensor.publishConfiguration(); }
  void republishState() override { _ha_entity_sensor.republishState(); }

  /**
   * @brief Publish the motion.
   *
   * @param detected true or false if motion is detected.
   */
  void publishMotion(bool detected) { _ha_entity_sensor.publishValue(detected ? "ON" : "OFF"); }

private:
  const homeassistantentities::BinarySensor::Motion _motion;
  HaEntitySensor _ha_entity_sensor;
};

#endif // __HA_ENTITY_MOTION_H__