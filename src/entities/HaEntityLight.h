#ifndef __HA_ENTITY_LIGHT_H__
#define __HA_ENTITY_LIGHT_H__

#include <Arduino.h>
#include <HaBridge.h>
#include <HaEntity.h>
#include <functional>

/**
 * @brief Represent a Light that can be controlled from Home Assistant. It goes two ways, as the light can be changed
 * independently and will report back state to Home Assistant. Supports brightness. But not things like color, white
 * balance and so on. Add support if needed. See https://www.home-assistant.io/integrations/light.mqtt/
 */
class HaEntityLight : public HaEntity {
public:
  /**
   * @brief Construct a new Ha Entity Light object
   *
   * @param name this is the human readable name that will be used for the entity in Home Assistant. Example: "Bathroom
   * light"
   * @param child_object_id non optional child identifier for this entity in case there are several sensors of the same
   * entity type for the same node ID. Example: If you have a lock for the node ID "door", the home asisstant
   * configuration path will be "homeassistant/binary_sensor/door/lock/config". This works if you only have one lock on
   * your door, but if you have two locks, you want to add a child object ID to them. By setting the child_object_id to
   * say "upper", the configuration will be "homeassistant/binary_sensor/door/lock/upper/config". This also apply for
   * all state/command topics and so on. Leave as empty string for no child object ID.
   * @param support_brightness if true, light supports brightness. If we add more capabilites, consider changing the
   * bool inte a list of capabilities.
   */
  HaEntityLight(HaBridge &ha_bridge, String name, String child_object_id, bool support_brightness = false);

public:
  void publishConfiguration() override;
  void republishState() override;

  /**
   * @brief Publish the light.
   *
   * @param on if light is on or off. Or absent (std::nullopt) if state is unknown.
   * @param brightness of the light, between 0 and 255. Or absent (std::nullopt) if brightness is unknown or this
   * capability is not supported. Will only be posted if the light is setup with this capability in the constructor.
   */
  void publishLight(std::optional<bool> on, std::optional<uint8_t> brightness = std::nullopt);

  /**
   * @brief Set callback for receving callbacks when there is a new state that should be set.
   */
  bool setOnState(std::function<void(bool)> state_callback);

  /**
   * @brief Set callback for receving callbacks when there is a new brightness that should be set. Will only be
   * respected if the light is setup with this capability in the constructor
   */
  bool setOnBrightness(std::function<void(uint8_t)> brightness_callback);

private:
  String _name;
  HaBridge &_ha_bridge;
  String _child_object_id;
  bool _support_brightness;

private:
  std::optional<bool> _on;
  std::optional<uint8_t> _brightness;
};

#endif // __HA_ENTITY_LIGHT_H__