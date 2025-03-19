#ifndef __HA_ENTITY_LIGHT_H__
#define __HA_ENTITY_LIGHT_H__

#include <HaBridge.h>
#include <HaEntity.h>
#include <cstdint>
#include <functional>
#include <optional>
#include <set>
#include <string>

/**
 * @brief Represent a Light that can be controlled from Home Assistant. It goes two ways, as the light can be changed
 * independently and will report back state to Home Assistant. Supports brightness. But not things like color, white
 * balance and so on. Add support if needed. See https://www.home-assistant.io/integrations/light.mqtt/
 */
class HaEntityLight : public HaEntity {
public:
  struct Configuration {
    /**
     * @brief If true, the light supports brightness.
     */
    bool with_brightness = false;

    enum class ColorTemperature {
      None,
      Mireds, // Color temperature is defined as mireds, value between 153 to 500.
      Kelvin, // Color temperature is defined as Kelvin, value between 2000 to 6535.
    };

    /**
     * @brief if set, the light supports color temperature.
     */
    ColorTemperature with_color_temperature = ColorTemperature::None;

    /**
     * @brief if True, the light supports RGB color.
     */
    bool with_rgb_color = false;

    /**
     * @brief if non empty, the supported effects.
     */
    std::set<std::string> effects = {};

    /**
     * @brief If true, this tells Home Assistant to publish the message on the command topic with retain set to true.
     */
    bool retain = false;
  };

  /**
   * @brief Construct a new Ha Entity Light object
   *
   * @param name this is the human readable name that will be used for the entity in Home Assistant. If a device is set
   * when creating the [HaBridge], the friendly named displayed in the UI will be the device name plus this name.
   * Example: if device name is "Bathroom" and entity name "light", friendly name will be "Bathroom light". If no
   * device, friendly name will be just "light". If a device is set, this name can be left empty if this entity is the
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
   * @param configuration what this light supports.
   */
  HaEntityLight(HaBridge &ha_bridge, std::string name, std::string child_object_id, Configuration configuration);

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
   * capability in the Configuration.
   */
  void publishBrightness(uint8_t brightness);

  /**
   * @brief Publish the current color temperature.
   *
   * @param temperature mireds or Kelvin, depending on what was specified in the Configuration. Will only be published
   * if the light is setup with this capability in the Configuration.
   */
  void publishColorTemperature(uint16_t temperature);

  /**
   * @brief Publish the current selected effect.
   *
   * @param effect currently selected. Should be any of the effects from the Capabilities. Will only be published if the
   * light is setup with this capability in the Configuration.
   */
  void publishEffect(std::string effect);

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
   * Configuration.
   */
  void publishRgb(uint8_t r, uint8_t g, uint8_t b) { publishRgb({r, g, b}); }

  /**
   * @brief Publish the current RGB value.
   *
   * @param rgb currently in affect. Will only be published if the light is setup with this capability in the
   * Configuration.
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
   * @brief Set callback for receving callbacks when there is a new color temperature that should be set. Will only be
   * respected if the light is setup with this capability. In mireds or Kelvin, depending on what was selected in the
   * Configuration.
   */
  bool setOnColorTemperature(std::function<void(uint16_t)> color_temperature_callback);

  /**
   * @brief Set callback for receving callbacks when there is a new effect that should be set. Will only be
   * respected if the light is setup with this capability.
   */
  bool setOnEffect(std::function<void(std::string)> effect_callback);

  /**
   * @brief Set callback for receving callbacks when there is a new RGB value that should be set. Will only be
   * respected if the light is setup with this capability.
   */
  bool setOnRgb(std::function<void(RGB)> effect_callback);

private:
  std::string _name;
  HaBridge &_ha_bridge;
  std::string _child_object_id;
  Configuration _configuration;

private:
  std::optional<bool> _on;
  std::optional<RGB> _rgb;
  std::optional<std::string> _effect;
  std::optional<uint8_t> _brightness;
  std::optional<uint16_t> _color_temperature;
};

#endif // __HA_ENTITY_LIGHT_H__