#include "HaEntitySensor.h"
#include <HaUtilities.h>
#include <IJson.h>

using namespace homeassistantentities;

HaEntitySensor::HaEntitySensor(HaBridge &ha_bridge, std::string name, std::optional<std::string> child_object_id,
                               Configuration configuration)
    : _name(trim(name)), _ha_bridge(ha_bridge), _object_id(configuration.device_class.objectId()),
      _configuration(configuration) {
  _component = configuration.device_class.sensorType() == DeviceClass::SensorType::Sensor ? "sensor" : "binary_sensor";

  if (child_object_id) {
    auto coid = trim(*child_object_id);
    _child_object_id = coid.empty() ? "" : coid;
  } else {
    _child_object_id = "";
  }
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
    auto state_class = trim(*_configuration.state_class);
    if (!state_class.empty()) {
      doc["state_class"] = state_class;
    }
  }
  auto device_class = _configuration.device_class.deviceClass();
  if (device_class) {
    auto trimmed_device_class = trim(*device_class);
    if (!trimmed_device_class.empty()) {
      doc["device_class"] = trimmed_device_class;
    }
  }
  if (_configuration.icon) {
    doc["icon"] = *_configuration.icon;
  }
  doc["force_update"] = _configuration.force_update;

  if (_configuration.unit_of_measurement) {
    auto unit_of_measurement = _configuration.device_class.unitOfMeasurement(*_configuration.unit_of_measurement);
    if (unit_of_measurement) {
      doc["unit_of_measurement"] = *unit_of_measurement;
    }
  }

  doc["state_topic"] = _ha_bridge.getTopic(HaBridge::TopicType::State, _component, _object_id, _child_object_id);

  if (_configuration.with_attributes) {
    doc["json_attributes_topic"] =
        _ha_bridge.getTopic(HaBridge::TopicType::Attributes, _component, _object_id, _child_object_id);
  }

  _ha_bridge.publishConfiguration(_component, _object_id, _child_object_id, doc);
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