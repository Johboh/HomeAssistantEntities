#include "HaEntitySensor.h"
#include <HaUtilities.h>
#include <IJson.h>

#define COMPONENT "sensor"

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
}

void HaEntitySensor::publishConfiguration() {
  IJsonDocument doc;

  if (!_name.empty()) {
    doc["name"] = _name;
  } else {
    doc["name"] = nullptr;
  }
  doc["state_class"] = "measurement";
  if (_configuration.device_class) {
    auto device_class = homeassistantentities::trim(*_configuration.device_class);
    if (!device_class.empty()) {
      doc["device_class"] = device_class;
    }
  }
  doc["force_update"] = _configuration.force_update;
  if (_configuration.unit_of_measurement) {
    doc["unit_of_measurement"] = *_configuration.unit_of_measurement;
  }
  doc["state_topic"] = _ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, _object_id, _child_object_id);
  _ha_bridge.publishConfiguration(COMPONENT, _object_id, _child_object_id, doc);
}

void HaEntitySensor::republishState() {
  if (_value) {
    publishValue(*_value);
  }
}

void HaEntitySensor::publishValue(double value) {
  _ha_bridge.publishMessage(_ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, _object_id, _child_object_id),
                            std::to_string(value));
  _value = value;
}
