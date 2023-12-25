#include "HaEntityCurtain.h"
#include <HaUtilities.h>
#include <nlohmann/json.hpp>

#define COMPONENT "cover"
#define OBJECT_ID "curtain"
#define OBJECT_ID_STATE "state"
#define OBJECT_ID_POSITION "position"

// NOTE! We have swapped object ID and child object ID to get a nicer state/command topic path.

HaEntityCurtain::HaEntityCurtain(HaBridge &ha_bridge, std::string name, std::string child_object_id)
    : _name(homeassistantentities::trim(name)), _ha_bridge(ha_bridge), _child_object_id(child_object_id) {}

void HaEntityCurtain::publishConfiguration() {
  nlohmann::json doc;

  if (!_name.empty()) {
    doc["name"] = _name;
  } else {
    doc["name"] = nullptr;
  }
  doc["device_class"] = "curtain";
  doc["state_topic"] = _ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, _child_object_id, OBJECT_ID_STATE);
  doc["command_topic"] =
      _ha_bridge.getTopic(HaBridge::TopicType::Command, COMPONENT, _child_object_id, OBJECT_ID_STATE);
  doc["position_topic"] =
      _ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, _child_object_id, OBJECT_ID_POSITION);
  doc["set_position_topic"] =
      _ha_bridge.getTopic(HaBridge::TopicType::Command, COMPONENT, _child_object_id, OBJECT_ID_POSITION);
  _ha_bridge.publishConfiguration(COMPONENT, OBJECT_ID, _child_object_id, doc);
}

void HaEntityCurtain::republishState() { publishCurtain(_state, _position); }

void HaEntityCurtain::publishCurtain(std::optional<State> state, std::optional<uint8_t> position) {
  if (state) {
    std::string str = "";
    switch (*state) {
    case State::Open:
      str = "open";
      break;
    case State::Opening:
      str = "opening";
      break;
    case State::Closed:
      str = "closed";
      break;
    case State::Closing:
      str = "closing";
      break;
    case State::Stopped:
      str = "stopped";
      break;
    case State::Unknown:
      break;
    }
    if (str.length() > 0) {
      _ha_bridge.publishMessage(
          _ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, _child_object_id, OBJECT_ID_STATE), str);
      _state = state;
    }
  }
  if (position) {
    _ha_bridge.publishMessage(
        _ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, _child_object_id, OBJECT_ID_POSITION),
        std::to_string(std::min((uint8_t)100, *position)));
    _position = position;
  }
}

bool HaEntityCurtain::setOnState(std::function<void(Action)> state_callback) {
  return _ha_bridge.remote().subscribe(
      _ha_bridge.getTopic(HaBridge::TopicType::Command, COMPONENT, _child_object_id, OBJECT_ID_STATE),
      [state_callback](std::string topic, std::string message) {
        Action state = Action::Unknown;
        if (message == "OPEN") {
          state = Action::Open;
        } else if (message == "CLOSE") {
          state = Action::Close;
        } else if (message == "STOP") {
          state = Action::Stop;
        }

        state_callback(state);
      });
}

bool HaEntityCurtain::setOnPosition(std::function<void(uint8_t)> position_callback) {
  return _ha_bridge.remote().subscribe(
      _ha_bridge.getTopic(HaBridge::TopicType::Command, COMPONENT, _child_object_id, OBJECT_ID_POSITION),
      [position_callback](std::string topic, std::string message) { position_callback(std::stoi(message)); });
}
