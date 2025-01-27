#ifndef __HA_ENTITY_SOUND_H__
#define __HA_ENTITY_SOUND_H__

#include "HaDeviceClasses.h"
#include "HaEntitySensor.h"
#include <HaBridge.h>
#include <HaEntity.h>
#include <cstdint>
#include <optional>
#include <string>

/**
 * @brief Represent a Sound binary sensor (if a sound is detected or not).
 */
class HaEntitySound : public HaEntity {
public:
  /**
   * @brief Construct a new Ha Entity Sound object
   *
   * @param name this is the human readable name that will be used for the entity in Home Assistant. If a device is set
   * when creating the [HaBridge], the friendly named displayed in the UI will be the device name plus this name.
   * Example: if device name is "Bathroom" and entity name "sound", friendly name will be "Bathroom sound". If no
   * device, friendly name will be just "sound". If a device is set, this name can be left empty if this entity is the
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
  HaEntitySound(HaBridge &ha_bridge, std::string name, std::optional<std::string> child_object_id = std::nullopt)
      : _ha_entity_sensor(HaEntitySensor(ha_bridge, name, child_object_id,
                                         HaEntitySensor::Configuration{
                                             .device_class = _sound,
                                             .state_class = std::nullopt,
                                         })) {}

public:
  void publishConfiguration() override { _ha_entity_sensor.publishConfiguration(); }
  void republishState() override { _ha_entity_sensor.republishState(); }

  /**
   * @brief Publish the sound.
   *
   * @param detected true if sound was detected, false if not.
   */
  void publishSound(bool detected) { _ha_entity_sensor.publishValue(detected ? "ON" : "OFF"); }

private:
  const homeassistantentities::BinarySensor::Sound _sound;
  HaEntitySensor _ha_entity_sensor;
};

#endif // __HA_ENTITY_SOUND_H__