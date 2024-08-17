#include "HaEntityBoolean.h"
#include <HaUtilities.h>
#include <IJson.h>

#define COMPONENT "binary_sensor"
#define OBJECT_ID "boolean"

HaEntityBoolean::HaEntityBoolean(HaBridge &ha_bridge, std::string name, std::string child_object_id,
                                 Configuration configuration)
    : _name(homeassistantentities::trim(name)), _ha_bridge(ha_bridge), _child_object_id(child_object_id),
      _configuration(configuration) {}

void HaEntityBoolean::publishConfiguration() {
  IJsonDocument doc;

  if (!_name.empty()) {
    doc["name"] = _name;
  } else {
    doc["name"] = nullptr;
  }

  doc["force_update"] = _configuration.force_update;
  doc["state_topic"] = _ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, OBJECT_ID, _child_object_id);

  if (_configuration.with_attributes) {
    doc["json_attributes_topic"] =
        _ha_bridge.getTopic(HaBridge::TopicType::Attributes, COMPONENT, OBJECT_ID, _child_object_id);
  }

  _ha_bridge.publishConfiguration(COMPONENT, OBJECT_ID, _child_object_id, doc);
}

void HaEntityBoolean::republishState() {
  if (_value) {
    publishBoolean(*_value);
  }
  if (_attributes) {
    publishAttributes(*_attributes);
  }
}

void HaEntityBoolean::publishBoolean(bool value, Attributes::Map attributes) {
  _value = value;

  _ha_bridge.publishMessage(_ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, OBJECT_ID, _child_object_id),
                            value ? "ON" : "OFF");

  if (!attributes.empty()) {
    publishAttributes(attributes);
  }
}

void HaEntityBoolean::publishAttributes(Attributes::Map attributes) {
  if (!_configuration.with_attributes) {
    return;
  }
  _attributes = attributes;

  IJsonDocument doc;
  if (Attributes::toJson(doc, attributes)) {
    auto message = toJsonString(doc);
    _ha_bridge.publishMessage(
        _ha_bridge.getTopic(HaBridge::TopicType::Attributes, COMPONENT, OBJECT_ID, _child_object_id), message);
  }
}