#include "HaEntityBoolean.h"
#include <HaUtilities.h>
#include <nlohmann/json.hpp>

#define COMPONENT "binary_sensor"
#define OBJECT_ID "boolean"

HaEntityBoolean::HaEntityBoolean(HaBridge &ha_bridge, std::string name, std::string child_object_id)
    : _name(homeassistantentities::trim(name)), _ha_bridge(ha_bridge), _child_object_id(child_object_id) {}

void HaEntityBoolean::publishConfiguration() {
  nlohmann::json doc;

  if (!_name.empty()) {
    doc["name"] = _name;
  } else {
    doc["name"] = nullptr;
  }
  doc["state_topic"] = _ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, OBJECT_ID, _child_object_id);
  _ha_bridge.publishConfiguration(COMPONENT, OBJECT_ID, _child_object_id, doc);
}

void HaEntityBoolean::republishState() {
  if (_value) {
    publishBoolean(*_value);
  }
}

void HaEntityBoolean::publishBoolean(bool detected) {
  _ha_bridge.publishMessage(_ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, OBJECT_ID, _child_object_id),
                            detected ? "ON" : "OFF");
  _value = detected;
}
