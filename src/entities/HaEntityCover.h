#ifndef __HA_ENTITY_COVER_H__
#define __HA_ENTITY_COVER_H__

#include <HaBridge.h>
#include <HaEntity.h>
#include <cstdint>
#include <functional>
#include <optional>
#include <string>

/**
 * @brief Represent a Cover that can be controlled from Home Assistant. It goes two ways, as the cover can be
 * changed independently and will report back state to Home Assistant. See
 * https://www.home-assistant.io/integrations/cover.mqtt/
 */
class HaEntityCover : public HaEntity {
public:
  struct Configuration {
    /**
     * @brief the device class for this cover. Should be any of
     * https://www.home-assistant.io/integrations/cover/#device_class
     * Will eventually be replaced by a typed device class.
     */
    std::string device_class = "curtain";

    /**
     * If set to true, this device is only reporting its state and position, but cannot be controlled by Home Assistant.
     * Usually, Home Assitant can open and close a cover/curtain/awning by posting the position to the position topic.
     * If this set to false, the position topic will be created and Home Assistant can post the position.
     */
    bool read_only = false;

    /**
     * From Home Assistant perspective:
     * MQTT cover expects position and tilt values to be in range of 0 to 100, where 0 indicates closed position and 100
     * indicates fully open position. If position min or max are set to a different range (e.g. 40 to 140), when sending
     * command to the device the range will be adjusted to the device range (position 0 will send a value of 40 to
     * device) and when position payload is received from the device it will be adjusted back to the 0 to 100 range
     * (device value of 40 will report cover position 0). min and max can also be used to reverse the direction of the
     * device, if min is set to 100 and max is set to 0 device operation will be inverted (e.g. when setting position to
     * 40, a value of 60 will be sent to device).
     *
     * Source: https://www.home-assistant.io/integrations/cover.mqtt/
     */
    std::optional<uint8_t> position_closed = std::nullopt;
    /**
     * see position_closed.
     */
    std::optional<uint8_t> position_open = std::nullopt;
  };

  inline static Configuration _default = {
      .device_class = "curtain", .read_only = false, .position_closed = std::nullopt, .position_open = std::nullopt};

  /**
   * @brief Construct a new Ha Entity Cover object
   *
   * @param name this is the human readable name that will be used for the entity in Home Assistant. If a device is set
   * when creating the [HaBridge], the friendly named displayed in the UI will be the device name plus this name.
   * Example: if device name is "Bathroom" and entity name "cover", friendly name will be "Bathroom cover". If no
   * device, friendly name will be just "cover". If a device is set, this name can be left empty if this entity is the
   * one main entity (or only) entity of this device. See
   * https://developers.home-assistant.io/docs/core/entity/#entity-naming for more
   * information.
   * @param child_object_id non optional child identifier for this entity in case there are several sensors of the same
   * entity type for the same node ID. Example: If you have a lock for the node ID "door", the home asisstant
   * configuration path will be "homeassistant/binary_sensor/door/lock/config". This works if you only have one lock on
   * your door, but if you have two locks, you want to add a child object ID to them. By setting the child_object_id to
   * say "upper", the configuration will be "homeassistant/binary_sensor/door/lock/upper/config". This also apply for
   * all state/command topics and so on. Leave as empty string for no child object ID.  Valid characters
   * are [a-zA-Z0-9_-] (machine readable, not human readable)
   * @param configuration the configuration for this entity.
   */
  HaEntityCover(HaBridge &ha_bridge, std::string name, std::string child_object_id,
                Configuration configuration = _default);

public:
  void publishConfiguration() override;
  void republishState() override;

  enum class State {
    Open,
    Opening,
    Closed,
    Closing,
    Stopped,
    Unknown,
  };

  /**
   * @brief Publish the cover values. This will publish to MQTT regardless if the values has changed. Also see
   * update().
   *
   * @param state the cover state. Or absent (std::nullopt) if state is unknown.
   * @param position of the cover, between 0 (fully closed) and 100 (fully open). Or absent (std::nullopt) if position
   * is unknown.
   */
  void publish(std::optional<State> state, std::optional<uint8_t> position = std::nullopt);

  /**
   * @brief Publish the cover values, but only if the values has changed. Also see publish().
   *
   * @param state the cover state. Or absent (std::nullopt) if state is unknown.
   * @param position of the cover, between 0 (fully closed) and 100 (fully open). Or absent (std::nullopt) if position
   * is unknown.
   */
  void update(std::optional<State> state, std::optional<uint8_t> position = std::nullopt);

  enum class Action {
    Open,
    Close,
    Stop,
    Unknown,
  };

  /**
   * @brief Set callback for receving callbacks when there is a new state that should be set.
   * Will not be called if read_only is true in Configuration.
   */
  bool setOnState(std::function<void(Action)> state_callback);

  /**
   * @brief Set callback for receving callbacks when there is a new position that should be set, value between 0 (fully
   * closed) and 100 (fully open).
   * Will not be called if read_only is true in Configuration.
   */
  bool setOnPosition(std::function<void(uint8_t)> position_callback);

private:
  void publishState(std::optional<State> state);
  void publishPosition(std::optional<uint8_t> position);

private:
  std::string _name;
  HaBridge &_ha_bridge;
  std::string _child_object_id;
  Configuration _configuration;

private:
  std::optional<State> _state;
  std::optional<uint8_t> _position;
};

#endif // __HA_ENTITY_COVER_H__