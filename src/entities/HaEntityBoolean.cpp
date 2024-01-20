#include "HaEntityBoolean.h"
#include <HaUtilities.h>
#include <nlohmann/json.hpp>

#define COMPONENT "binary_sensor"
#define OBJECT_ID "boolean"

HaEntityBoolean::HaEntityBoolean(HaBridge &ha_bridge, std::string name, std::string child_object_id, bool force_update,
                                 bool with_attributes)
    : _name(homeassistantentities::trim(name)), _force_update(force_update), _ha_bridge(ha_bridge),
      _with_attributes(with_attributes), _child_object_id(child_object_id) {}

void HaEntityBoolean::publishConfiguration() {
  nlohmann::json doc;

  if (!_name.empty()) {
    doc["name"] = _name;
  } else {
    doc["name"] = nullptr;
  }

  doc["force_update"] = _force_update;
  doc["state_topic"] = _ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, OBJECT_ID, _child_object_id);

  if (_with_attributes) {
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
  if (!_with_attributes) {
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