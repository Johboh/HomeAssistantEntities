#ifndef __HA_ENTITY_FAN_H__
#define __HA_ENTITY_FAN_H__

#include <HaBridge.h>
#include <HaEntity.h>
#include <cstdint>
#include <optional>
#include <set>
#include <string>

/**
 * @brief Represent a fan with speed.
 */
class HaEntityFan : public HaEntity {
public:
  struct Configuration {

    /**
     * @brief If this fan can change direction of flow.
     */
    bool with_direction = false;

    /**
     * @brief If this fan supports oscillation (back and forth movements).
     */
    bool with_oscillation = false;

    /**
     * @brief If this fan supports controlling the speed, compared to being only an on/off fan.
     */
    bool with_speed = false;

    /**
     * @brief If with_speed is set, the minimum of numeric output range (off not included, so speed_range_min - 1
     * represents 0 %). The percentage_step is defined by 100 / the number of speeds within the speed range.
     *
     */
    uint32_t speed_range_min = 1;

    /**
     * @brief If with_speed is set, the maximum of numeric output range (representing 100 %). The percentage_step is
     * defined by 100 / the number of speeds within the speed range.
     *
     */
    uint32_t speed_range_max = 100;

    /**
     * @brief If non empty, the presets this fan supports.
     */
    std::set<std::string> presets = {};

    /**
     * In Home Assistant, trigger events even if the sensor's state hasn't changed. Useful if you want
     * to have meaningful value graphs in history or want to create an automation that triggers on every incoming state
     * message (not only when the sensor’s new state is different to the current one).
     */
    bool force_update = false;

    /**
     * @brief If true, this tells Home Assistant to publish the message on the command topic with retain set to true.
     */
    bool retain = false;
  };

  inline static Configuration _default = {.with_direction = false,
                                          .with_oscillation = false,
                                          .with_speed = false,
                                          .speed_range_min = 1,
                                          .speed_range_max = 100,
                                          .presets = {},
                                          .force_update = false,
                                          .retain = false};

  /**
   * @brief Construct a new Ha Entity Fan object
   *
   * @param name this is the human readable name that will be used for the entity in Home Assistant. If a device is set
   * when creating the [HaBridge], the friendly named displayed in the UI will be the device name plus this name.
   * Example: if device name is "Bathroom" and entity name "fan", friendly name will be "Bathroom fan". If no
   * device, friendly name will be just "fan". If a device is set, this name can be left empty if this entity is the
   * one main entity (or only) entity of this device. See
   * https://developers.home-assistant.io/docs/core/entity/#entity-naming for more
   * information.
   * @param child_object_id child identifier for this entity in case there are several fans of the same
   * entity type for the same node ID. Example: If you have a lock for the node ID "door", the home assistant
   * configuration path will be "homeassistant/binary_sensor/door/lock/config". This works if you only have one lock on
   * your door, but if you have two locks, you want to add a child object ID to them. By setting the child_object_id to
   * say "upper", the configuration will be "homeassistant/binary_sensor/door/lock/upper/config". This also apply for
   * all state/command topics and so on. Leave as empty string for no child object ID. Valid characters
   * are [a-zA-Z0-9_-] (machine readable, not human readable)
   * @param configuration the configuration for this entity.
   */
  HaEntityFan(HaBridge &ha_bridge, std::string name, std::string child_object_id,
              Configuration configuration = _default);

public:
  void publishConfiguration() override;
  void republishState() override;

  /**
   * @brief Publish the direction. This will publish to MQTT regardless if the value has changed. Also see
   * updateDirection().
   *
   * with_direction in Configuration must be set.
   *
   * @param direction the direction to publish. Direction is a free form string.
   */
  void publishDirection(std::string direction);

  /**
   * @brief Publish the direction, but only if the value has changed. Also see publishDirection().
   *
   * with_direction in Configuration must be set.
   *
   * @param direction the direction to publish. Direction is a free form string.
   */
  void updateDirection(std::string direction);

  /**
   * @brief Set callback for receiving callbacks when there is a new direction that should be set. Direction is a free
   * form string.
   *
   * with_direction in Configuration must be set.
   */
  bool setOnDirection(std::function<void(std::string)> callback);

  //--------------------------------------

  /**
   * @brief Publish the oscillation. This will publish to MQTT regardless if the value has changed. Also see
   * updateOscillation().
   *
   * with_oscillation in Configuration must be set.
   *
   * @param oscillation if oscillation should be on or off.
   */
  void publishOscillation(bool oscillation);

  /**
   * @brief Publish the oscillation, but only if the value has changed. Also see publishOscillation().
   *
   * with_oscillation in Configuration must be set.
   *
   * @param oscillation if oscillation should be on or off.
   */
  void updateOscillation(bool oscillation);

  /**
   * @brief Set callback for receiving callbacks when oscillation should be turned on or off.
   *
   * with_oscillation in Configuration must be set.
   */
  bool setOnOscillation(std::function<void(bool)> callback);

  //--------------------------------------

  /**
   * @brief Publish the speed. This will publish to MQTT regardless if the value has changed. Also see
   * updateSpeed().
   *
   * with_speed in Configuration must be set.
   *
   * @param speed the speed, value between the min/max set in the Configuration.
   */
  void publishSpeed(uint32_t speed);

  /**
   * @brief Publish the speed, but only if the value has changed. Also see publishSpeed().
   *
   * with_speed in Configuration must be set.
   *
   * @param speed the speed, value between the min/max set in the Configuration.
   */
  void updateSpeed(uint32_t speed);

  /**
   * @brief Set callback for receiving callbacks when a new speed should be set. Value between the min/max set in the
   * Configuration.
   *
   * with_speed in Configuration must be set.
   */
  bool setOnSpeed(std::function<void(uint32_t)> callback);

  //--------------------------------------

  /**
   * @brief Publish a preset. This will publish to MQTT regardless if the value has changed. Also see
   * updatePreset().
   *
   * presets in Configuration must be set to a non empty set.
   *
   * @param preset the preset.
   */
  void publishPreset(std::string preset);

  /**
   * @brief Publish a preset, but only if the value has changed. Also see publishPreset().
   *
   * presets in Configuration must be set to a non empty set.
   *
   * @param preset the preset.
   */
  void updatePreset(std::string preset);

  /**
   * @brief Set callback for receiving callbacks when a new preset should be set.
   *
   * presets in Configuration must be set to a non empty set.
   */
  bool setOnPreset(std::function<void(std::string)> callback);

  //--------------------------------------

  /**
   * @brief Publish the fan state. This will publish to MQTT regardless if the value has changed. Also see
   * updateIsOn().
   *
   * @param on if the fan is on or off.
   */
  void publishIsOn(bool on);

  /**
   * @brief Publish the fan state, but only if the value has changed. Also see publishIsOn().
   *
   * @param on if the fan is on or off.
   */
  void updateIsOn(bool on);

  /**
   * @brief Set callback for receiving callbacks when a new state should be set.
   */
  bool setOnState(std::function<void(bool)> callback);

private:
  std::string _name;
  HaBridge &_ha_bridge;
  std::string _child_object_id;
  Configuration _configuration;

private:
  std::optional<bool> _on;
  std::optional<uint32_t> _speed;
  std::optional<bool> _oscillation;
  std::optional<std::string> _preset;
  std::optional<std::string> _direction;
};

#endif // __HA_ENTITY_FAN_H__