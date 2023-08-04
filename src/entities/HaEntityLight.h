#ifndef __HA_ENTITY_LIGHT_H__
#define __HA_ENTITY_LIGHT_H__

#include <Arduino.h>
#include <HaBridge.h>
#include <HaEntity.h>
#include <functional>
#include <set>

/**
 * @brief Represent a Light that can be controlled from Home Assistant. It goes two ways, as the light can be changed
 * independently and will report back state to Home Assistant. Supports brightness. But not things like color, white
 * balance and so on. Add support if needed. See https://www.home-assistant.io/integrations/light.mqtt/
 */
class HaEntityLight : public HaEntity {
public:
  struct Capabilities {
    bool with_brightness = false;
    bool with_rgb_color = false;
    // if non empty, the supported effects.
    std::set<String> effects;
  };

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
   * @param capabilities what this light supports.
   */
  HaEntityLight(HaBridge &ha_bridge, String name, String child_object_id, Capabilities &capabilities);

public:
  void publishConfiguration() override;
  void republishState() override;

  /**
   * @brief Publish the current on state.
   *
   * @param on if light is on or off.
   */
  void publishIsOn(bool on);

  /**
   * @brief Publish the current brightness.
   *
   * @param brightness of the light, between 0 and 255. Will only be published if the light is setup with this
   * capability in the constructor.
   */
  void publishBrightness(uint8_t brightness);

  /**
   * @brief Publish the current selected effect.
   *
   * @param effect currently selected. Should be any of the effects from the Capabilities. Will only be published if the
   * light is setup with this capability in the constructor.
   */
  void publishEffect(String &effect);

  struct RGB {
    uint8_t r;
    uint8_t g;
    uint8_t b;

    bool operator==(const RGB &other) const { return (r == other.r) && (g == other.g) && (b == other.b); }

    bool operator!=(const RGB &other) const { return !(*this == other); }
  };

  /**
   * @brief Publish the current RGB value.
   *
   * @param rgb currently in affect. Will only be published if the light is setup with this capability in the
   * constructor.
   */
  void publishRgb(uint8_t r, uint8_t g, uint8_t b) { publishRgb({r, g, b}); }

  /**
   * @brief Publish the current RGB value.
   *
   * @param rgb currently in affect. Will only be published if the light is setup with this capability in the
   * constructor.
   */
  void publishRgb(RGB rgb);

  /**
   * @brief Set callback for receving callbacks when there is a new on state that should be set.
   */
  bool setOnOn(std::function<void(bool)> on_state_callback);

  /**
   * @brief Set callback for receving callbacks when there is a new brightness that should be set. Will only be
   * respected if the light is setup with this capability.
   */
  bool setOnBrightness(std::function<void(uint8_t)> brightness_callback);

  /**
   * @brief Set callback for receving callbacks when there is a new effect that should be set. Will only be
   * respected if the light is setup with this capability.
   */
  bool setOnEffect(std::function<void(String)> effect_callback);

  /**
   * @brief Set callback for receving callbacks when there is a new RGB value that should be set. Will only be
   * respected if the light is setup with this capability.
   */
  bool setOnRgb(std::function<void(RGB)> effect_callback);

private:
  String _name;
  HaBridge &_ha_bridge;
  String _child_object_id;
  Capabilities _capabilities;

private:
  std::optional<bool> _on;
  std::optional<RGB> _rgb;
  std::optional<String> _effect;
  std::optional<uint8_t> _brightness;
};

#endif // __HA_ENTITY_LIGHT_H__