#include "HaEntityString.h"
#include <HaUtilities.h>
#include <nlohmann/json.hpp>

#define COMPONENT "sensor"
#define OBJECT_ID "string"

HaEntityString::HaEntityString(HaBridge &ha_bridge, std::string name, std::string child_object_id,
                               Configuration configuration)
    : _name(homeassistantentities::trim(name)), _ha_bridge(ha_bridge), _child_object_id(child_object_id),
      _configuration(configuration) {}

void HaEntityString::publishConfiguration() {
  nlohmann::json doc;

  if (!_name.empty()) {
    doc["name"] = _name;
  } else {
    doc["name"] = nullptr;
  }

  auto device_class = homeassistantentities::trim(_configuration.device_class);
  if (!device_class.empty()) {
    doc["device_class"] = device_class;
  }

  doc["force_update"] = _configuration.force_update;
  doc["state_topic"] = _ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, OBJECT_ID, _child_object_id);

  if (_configuration.with_attributes) {
    doc["json_attributes_topic"] =
        _ha_bridge.getTopic(HaBridge::TopicType::Attributes, COMPONENT, OBJECT_ID, _child_object_id);
  }

  _ha_bridge.publishConfiguration(COMPONENT, OBJECT_ID, _child_object_id, doc);
}

void HaEntityString::republishState() {
  if (_str) {
    publishString(*_str);
  }
  if (_attributes) {
    publishAttributes(*_attributes);
  }
}

void HaEntityString::publishString(std::string str, Attributes::Map attributes) {
  _str = str;
  _ha_bridge.publishMessage(_ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, OBJECT_ID, _child_object_id),
                            str);

  if (!attributes.empty()) {
    publishAttributes(attributes);
  }
}

void HaEntityString::publishAttributes(Attributes::Map attributes) {
  if (!_configuration.with_attributes) {
    return;
  }
  _attributes = attributes;

  nlohmann::json doc;
  if (Attributes::toJson(doc, attributes)) {
    auto message = doc.dump();
    _ha_bridge.publishMessage(
        _ha_bridge.getTopic(HaBridge::TopicType::Attributes, COMPONENT, OBJECT_ID, _child_object_id), message);
  }
}