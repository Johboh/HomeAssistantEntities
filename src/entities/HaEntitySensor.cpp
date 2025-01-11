#include "HaEntitySensor.h"
#include <HaUtilities.h>
#include <IJson.h>

HaEntitySensor::HaEntitySensor(HaBridge &ha_bridge, std::string name, std::string child_object_id,
                               Configuration configuration)
    : _name(homeassistantentities::trim(name)), _ha_bridge(ha_bridge), _child_object_id(child_object_id),
      _configuration(configuration) {
  if (_configuration.device_class) {
    auto device_class = homeassistantentities::trim(*_configuration.device_class);
    _object_id = device_class.empty() ? "unknown_device_class" : device_class;
  } else {
    _object_id = "unknown_device_class";
  }
  _component = configuration.sensor_type == SensorType::Sensor ? "sensor" : "binary_sensor";
}

void HaEntitySensor::publishConfiguration() {
  IJsonDocument doc;

  if (!_name.empty()) {
    doc["name"] = _name;
  } else {
    doc["name"] = nullptr;
  }
  doc["platform"] = _component; // we have validated this to be either sensor or binary_sensor

  if (_configuration.state_class) {
    auto state_class = homeassistantentities::trim(*_configuration.state_class);
    if (!state_class.empty()) {
      doc["state_class"] = state_class;
    }
  }
  if (_configuration.device_class) {
    auto device_class = homeassistantentities::trim(*_configuration.device_class);
    if (!device_class.empty()) {
      doc["device_class"] = device_class;
    }
  }
  if (_configuration.icon) {
    doc["icon"] = *_configuration.icon;
  }
  doc["force_update"] = _configuration.force_update;
  if (_configuration.unit_of_measurement) {
    doc["unit_of_measurement"] = *_configuration.unit_of_measurement;
  }
  doc["state_topic"] = _ha_bridge.getTopic(HaBridge::TopicType::State, _component, _object_id, _child_object_id);
  _ha_bridge.publishConfiguration(_component, _object_id, _child_object_id, doc);

  if (_configuration.with_attributes) {
    doc["json_attributes_topic"] =
        _ha_bridge.getTopic(HaBridge::TopicType::Attributes, _component, _object_id, _child_object_id);
  }
}

void HaEntitySensor::republishState() {
  if (_value) {
    publishValue(*_value);
  }
  if (_attributes) {
    publishAttributes(*_attributes);
  }
}

void HaEntitySensor::publishValue(double value, Attributes::Map attributes) {
  auto value_str = std::to_string(value);
  publishValue(value_str, attributes);
}

void HaEntitySensor::publishValue(std::string value, Attributes::Map attributes) {
  _ha_bridge.publishMessage(_ha_bridge.getTopic(HaBridge::TopicType::State, _component, _object_id, _child_object_id),
                            value);
  _value = value;

  if (!attributes.empty()) {
    publishAttributes(attributes);
  }
}

void HaEntitySensor::publishAttributes(Attributes::Map attributes) {
  if (!_configuration.with_attributes) {
    return;
  }
  _attributes = attributes;

  IJsonDocument doc;
  if (Attributes::toJson(doc, attributes)) {
    auto message = toJsonString(doc);
    _ha_bridge.publishMessage(
        _ha_bridge.getTopic(HaBridge::TopicType::Attributes, _component, _object_id, _child_object_id), message);
  }
}