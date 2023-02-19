#ifndef __HA_ENTITY_SOUND_H__
#define __HA_ENTITY_SOUND_H__

#include "HaBridge.h"
#include "HaEntity.h"
#include <Arduino.h>

/**
 * @brief Represent a Sound binary sensor (if a sound is detected or not).
 */
class HaEntitySound : public HaEntity {
public:
  /**
   * @brief Construct a new Ha Entity Sound object
   *
   * @param name this is the human readable name that will be used for the entity in Home Assistant. Example: "Front
   * sound"
   * @param child_object_id optional child identifier for this entity in case there are several sensors of the same
   * entity type for the same node ID. Example: If you have a lock for the node ID "door", the home asisstant
   * configuration path will be "homeassistant/binary_sensor/door/lock/config". This works if you only have one lock on
   * your door, but if you have two locks, you want to add a child object ID to them. By setting the child_object_id to
   * say "upper", the configuration will be "homeassistant/binary_sensor/door/lock/upper/config". This also apply for
   * all state/command topics and so on. Leave as empty string for no child object ID.
   */
  HaEntitySound(HaBridge &ha_bridge, String name, String child_object_id = "");

public:
  void publishConfiguration() override;
  void republishState() override;

  /**
   * @brief Publish the sound.
   *
   * @param detected true if sound was detected, false if not.
   */
  void publishSound(bool detected);

private:
  String _name;
  HaBridge &_ha_bridge;
  String _child_object_id;

private:
  std::optional<bool> _detected;
};

#endif // __HA_ENTITY_SOUND_H__