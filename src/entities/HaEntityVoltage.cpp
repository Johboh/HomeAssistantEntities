#include "HaEntityVoltage.h"
#include <HaUtilities.h>
#include <IJson.h>

#define COMPONENT "sensor"
#define OBJECT_ID "voltage"

HaEntityVoltage::HaEntityVoltage(HaBridge &ha_bridge, std::string name, std::string child_object_id,
                                 Configuration configuration)
    : _name(homeassistantentities::trim(name)), _ha_bridge(ha_bridge), _child_object_id(child_object_id),
      _configuration(configuration) {}

void HaEntityVoltage::publishConfiguration() {
  IJsonDocument doc;

  if (!_name.empty()) {
    doc["name"] = _name;
  } else {
    doc["name"] = nullptr;
  }
  doc["device_class"] = "voltage";
  doc["force_update"] = _configuration.force_update;
  switch (_configuration.unit) {
  case Unit::V:
    doc["unit_of_measurement"] = "V";
    break;
  case Unit::mV:
    doc["unit_of_measurement"] = "mV";
    break;
  }
  doc["state_topic"] = _ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, OBJECT_ID, _child_object_id);
  _ha_bridge.publishConfiguration(COMPONENT, OBJECT_ID, _child_object_id, doc);
}

void HaEntityVoltage::republishState() {
  if (_voltage) {
    publishVoltage(*_voltage);
  }
}

void HaEntityVoltage::publishVoltage(double voltage) {
  _ha_bridge.publishMessage(_ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, OBJECT_ID, _child_object_id),
                            std::to_string(voltage));
  _voltage = voltage;
}
