#include "HaEntityNumber.h"
#include "ArduinoJson.h"

#define COMPONENT "number"

HaEntityNumber::HaEntityNumber(HaBridge &ha_bridge, String name, String object_id, float min_value, float max_value)
    : _name(name), _ha_bridge(ha_bridge), _object_id(object_id), _min_value(min_value), _max_value(max_value) {}

void HaEntityNumber::publishConfiguration() {
  DynamicJsonDocument doc(512);
  doc["name"] = _name;
  // TODO (johboh): Allow setting doc["device_class"]?
  doc["state_topic"] = _ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, _object_id);
  doc["command_topic"] = _ha_bridge.getTopic(HaBridge::TopicType::Command, COMPONENT, _object_id);
  doc["min"] = _min_value;
  doc["max"] = _max_value;
  _ha_bridge.publishConfiguration(COMPONENT, _object_id, "", doc);
}

void HaEntityNumber::republishState() {
  if (_number) {
    publishNumber(*_number);
  }
}

void HaEntityNumber::publishNumber(float number) {
  // numbered == OFF
  _ha_bridge.publishMessage(_ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, _object_id), String(number));
  _number = std::optional<float>{number};
}

bool HaEntityNumber::setOnNumber(std::function<void(float)> callback) {
  return _ha_bridge.remote().subscribe(
      _ha_bridge.getTopic(HaBridge::TopicType::Command, COMPONENT, _object_id),
      [callback](const char *topic, const char *message) { callback(String(message).toFloat()); });
}