#include "HaEntityString.h"
#include <HaUtilities.h>
#include <nlohmann/json.hpp>

#define COMPONENT "sensor"
#define OBJECT_ID "string"

HaEntityString::HaEntityString(HaBridge &ha_bridge, std::string name, std::string child_object_id, bool force_update)
    : _name(homeassistantentities::trim(name)), _force_update(force_update), _ha_bridge(ha_bridge),
      _child_object_id(child_object_id) {}

void HaEntityString::publishConfiguration() {
  nlohmann::json doc;

  if (!_name.empty()) {
    doc["name"] = _name;
  } else {
    doc["name"] = (char *)NULL;
  }
  doc["force_update"] = _force_update;
  doc["state_topic"] = _ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, OBJECT_ID, _child_object_id);
  _ha_bridge.publishConfiguration(COMPONENT, OBJECT_ID, _child_object_id, doc);
}

void HaEntityString::republishState() {
  if (_message) {
    publishMessage(*_message);
  }
}

void HaEntityString::publishMessage(std::string &message) {
  _message = message;
  _ha_bridge.publishMessage(_ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, OBJECT_ID, _child_object_id),
                            message);
}