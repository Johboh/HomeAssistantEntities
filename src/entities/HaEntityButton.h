#ifndef __HA_ENTITY_BUTTON_H__
#define __HA_ENTITY_BUTTON_H__

#include <HaBridge.h>
#include <HaEntity.h>
#include <cstdint>
#include <functional>
#include <optional>
#include <string>

/**
 * @brief Represent a Button that can be set/pressed by Home Assistant (one way only).
 * See more at https://www.home-assistant.io/integrations/button.mqtt/
 */
class HaEntityButton : public HaEntity {
public:
  /**
   * @brief Construct a new Ha Entity Button object
   *
   * @param name this is the human readable name that will be used for the entity in Home Assistant. If a device is set
   * when creating the [HaBridge], the friendly named displayed in the UI will be the device name plus this name.
   * Example: if device name is "Bathroom" and entity name "button", friendly name will be "Bathroom button". If no
   * device, friendly name will be just "button". If a device is set, this name can be left empty if this entity is the
   * one main entity (or only) entity of this device. See
   * https://developers.home-assistant.io/docs/core/entity/#entity-naming for more
   * information.
   * @param child_object_id non optional child identifier for this entity in case there are several sensors of the same
   * entity type for the same node ID. Example: If you have a lock for the node ID "door", the home asisstant
   * configuration path will be "homeassistant/binary_sensor/door/lock/config". This works if you only have one lock on
   * your door, but if you have two locks, you want to add a child object ID to them. By setting the child_object_id to
   * say "upper", the configuration will be "homeassistant/binary_sensor/door/lock/upper/config". This also apply for
   * all state/command topics and so on. Leave as empty string for no child object ID. Valid characters
   * are [a-zA-Z0-9_-] (machine readable, not human readable)
   */
  HaEntityButton(HaBridge &ha_bridge, std::string name, std::string child_object_id);

public:
  void publishConfiguration() override;
  void republishState() override;

  /**
   * @brief Set callback for receving callbacks when button is "pressed".
   */
  bool setOnPressed(std::function<void(void)> callback);

private:
  std::string _name;
  HaBridge &_ha_bridge;
  std::string _child_object_id;
};

#endif // __HA_ENTITY_BUTTON_H__