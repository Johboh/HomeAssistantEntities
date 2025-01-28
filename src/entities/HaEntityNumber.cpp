#include "HaEntityNumber.h"
#include <HaUtilities.h>
#include <IJson.h>

#define COMPONENT "number"

HaEntityNumber::HaEntityNumber(HaBridge &ha_bridge, std::string name, std::string object_id,
                               Configuration configuration)
    : _name(homeassistantentities::trim(name)), _ha_bridge(ha_bridge), _object_id(object_id),
      _configuration(configuration) {}

void HaEntityNumber::publishConfiguration() {
  IJsonDocument doc;

  if (!_name.empty()) {
    doc["name"] = _name;
  } else {
    doc["name"] = nullptr;
  }

  doc["platform"] = "number";
  doc["min"] = _configuration.min_value;
  doc["max"] = _configuration.max_value;
  doc["force_update"] = _configuration.force_update;
  doc["retain"] = _configuration.retain;

  // TODO (johboh): Allow setting doc["device_class"]?
  doc["state_topic"] = _ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, _object_id);
  doc["command_topic"] = _ha_bridge.getTopic(HaBridge::TopicType::Command, COMPONENT, _object_id);

  _ha_bridge.publishConfiguration(COMPONENT, _object_id, "", doc);
}

void HaEntityNumber::republishState() {
  if (_number) {
    publishNumber(*_number);
  }
}

void HaEntityNumber::publishNumber(float number) {
  // numbered == OFF
  _ha_bridge.publishMessage(_ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, _object_id),
                            std::to_string(number));
  _number = number;
}

bool HaEntityNumber::setOnNumber(std::function<void(float)> callback) {
  return _ha_bridge.remote().subscribe(
      _ha_bridge.getTopic(HaBridge::TopicType::Command, COMPONENT, _object_id),
      [callback](std::string topic, std::string message) { callback(std::stof(message)); });
}