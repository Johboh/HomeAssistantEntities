#ifndef __HA_ENTITY_CURTAIN_H__
#define __HA_ENTITY_CURTAIN_H__

#include "HaBridge.h"
#include "HaEntity.h"
#include <Arduino.h>
#include <functional>

/**
 * @brief Represent a Curtain that can be controlled from Home Assistant. It goes two ways, as the curtain can be
 * changed independently and will report back state to Home Assistant. See
 * https://www.home-assistant.io/integrations/cover.mqtt/
 */
class HaEntityCurtain : public HaEntity {
public:
  /**
   * @brief Construct a new Ha Entity Curtain object
   *
   * @param name this is the human readable name that will be used for the entity in Home Assistant. Example: "Bathroom
   * curtain"
   * @param child_object_id non optional child identifier for this entity in case there are several sensors of the same
   * entity type for the same node ID. Example: If you have a lock for the node ID "door", the home asisstant
   * configuration path will be "homeassistant/binary_sensor/door/lock/config". This works if you only have one lock on
   * your door, but if you have two locks, you want to add a child object ID to them. By setting the child_object_id to
   * say "upper", the configuration will be "homeassistant/binary_sensor/door/lock/upper/config". This also apply for
   * all state/command topics and so on. Leave as empty string for no child object ID.
   */
  HaEntityCurtain(HaBridge &ha_bridge, String name, String child_object_id);

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
   * @brief Publish the curtain.
   *
   * @param state the curtain state. Or absent (std::nullopt) if state is unknown.
   * @param position of the curtain, between 0 and 100. Or absent (std::nullopt) if position is unknown.
   */
  void publishCurtain(std::optional<State> state, std::optional<uint8_t> position = std::nullopt);

  enum class Action {
    Open,
    Close,
    Stop,
    Unknown,
  };

  /**
   * @brief Set callback for receving callbacks when there is a new state that should be set.
   */
  bool setOnState(std::function<void(Action)> state_callback);

  /**
   * @brief Set callback for receving callbacks when there is a new position that should be set.
   */
  bool setOnPosition(std::function<void(uint8_t)> position_callback);

private:
  String _name;
  HaBridge &_ha_bridge;
  String _child_object_id;

private:
  std::optional<State> _state;
  std::optional<uint8_t> _position;
};

#endif // __HA_ENTITY_CURTAIN_H__