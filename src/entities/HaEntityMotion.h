#ifndef __HA_ENTITY_MOTION_H__
#define __HA_ENTITY_MOTION_H__

#include <Arduino.h>
#include <HaBridge.h>
#include <HaEntity.h>

/**
 * @brief Represent a Motion binary sensor.
 */
class HaEntityMotion : public HaEntity {
public:
  /**
   * @brief Construct a new Ha Entity Motion object
   *
   * @param name this is the human readable name that will be used for the entity in Home Assistant. Example: "Bathroom
   * motion"
   * @param child_object_id optional child identifier for this entity in case there are several sensors of the same
   * entity type for the same node ID. Example: If you have a lock for the node ID "door", the home asisstant
   * configuration path will be "homeassistant/binary_sensor/door/lock/config". This works if you only have one lock on
   * your door, but if you have two locks, you want to add a child object ID to them. By setting the child_object_id to
   * say "upper", the configuration will be "homeassistant/binary_sensor/door/lock/upper/config". This also apply for
   * all state/command topics and so on. Leave as empty string for no child object ID.
   */
  HaEntityMotion(HaBridge &ha_bridge, String name, String child_object_id = "");

public:
  void publishConfiguration() override;
  void republishState() override;

  /**
   * @brief Publish the motion.
   *
   * @param detected true or false if motion is detected.
   */
  void publishMotion(bool detected);

private:
  String _name;
  HaBridge &_ha_bridge;
  String _child_object_id;

private:
  std::optional<bool> _detected;
};

#endif // __HA_ENTITY_MOVEMENT_H__