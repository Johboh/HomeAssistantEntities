#include "HaEntityJson.h"
#include <HaUtilities.h>
#include <nlohmann/json.hpp>

#define COMPONENT "sensor"
#define OBJECT_ID "json"

HaEntityJson::HaEntityJson(HaBridge &ha_bridge, std::string name, std::string child_object_id,
                           Configuration configuration)
    : _name(homeassistantentities::trim(name)), _ha_bridge(ha_bridge), _child_object_id(child_object_id),
      _configuration(configuration) {}

void HaEntityJson::publishConfiguration() {
  nlohmann::json doc;

  if (!_name.empty()) {
    doc["name"] = _name;
  } else {
    doc["name"] = nullptr;
  }
  doc["force_update"] = _configuration.force_update;
  doc["state_topic"] = _ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, OBJECT_ID, _child_object_id);
  _ha_bridge.publishConfiguration(COMPONENT, OBJECT_ID, _child_object_id, doc);
}

void HaEntityJson::republishState() {
  if (_json_message) {
    publishMessage(*_json_message);
  }
}

void HaEntityJson::publishJson(nlohmann::json &json_doc) {
  auto message = json_doc.dump();
  publishMessage(message);
  _json_message = message;
}

void HaEntityJson::publishMessage(std::string &message) {
  _ha_bridge.publishMessage(_ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, OBJECT_ID, _child_object_id),
                            message);
}