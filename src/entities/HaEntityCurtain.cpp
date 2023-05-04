#include "HaEntityCurtain.h"
#include "ArduinoJson.h"

#define COMPONENT "cover"
#define OBJECT_ID "curtain"
#define OBJECT_ID_STATE "state"
#define OBJECT_ID_POSITION "position"

// NOTE! We have swapped object ID and child object ID to get a nicer state/command topic path.

HaEntityCurtain::HaEntityCurtain(HaBridge &ha_bridge, String name, String child_object_id)
    : _name(name), _ha_bridge(ha_bridge), _child_object_id(child_object_id) {}

void HaEntityCurtain::publishConfiguration() {
  DynamicJsonDocument doc(512);
  doc["name"] = _name;
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
    String str = "";
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
      Serial.println("HaEntityCurtain: Err: Unknown state to publish.");
      break;
    }
    if (str.length() > 0) {
      _ha_bridge.publishMessage(
          _ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, _child_object_id, OBJECT_ID_STATE), str);
      _state = std::optional<State>{state};
    }
  }
  if (position) {
    _ha_bridge.publishMessage(
        _ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, _child_object_id, OBJECT_ID_POSITION),
        String(std::min((uint8_t)100, *position)));
    _position = std::optional<uint8_t>{position};
  }
}

bool HaEntityCurtain::setOnState(std::function<void(Action)> state_callback) {
  return _ha_bridge.remote().subscribe(
      _ha_bridge.getTopic(HaBridge::TopicType::Command, COMPONENT, _child_object_id, OBJECT_ID_STATE),
      [state_callback](const char *topic, const char *message) {
        Action state = Action::Unknown;
        auto message_str = String(message);
        if (message_str == "OPEN") {
          state = Action::Open;
        } else if (message_str == "CLOSE") {
          state = Action::Close;
        } else if (message_str == "STOP") {
          state = Action::Stop;
        }

        state_callback(state);
      });
}

bool HaEntityCurtain::setOnPosition(std::function<void(uint8_t)> position_callback) {
  return _ha_bridge.remote().subscribe(
      _ha_bridge.getTopic(HaBridge::TopicType::Command, COMPONENT, _child_object_id, OBJECT_ID_POSITION),
      [position_callback](const char *topic, const char *message) { position_callback(String(message).toInt()); });
}
