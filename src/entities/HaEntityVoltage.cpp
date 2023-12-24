#include "HaEntityVoltage.h"
#include <HaUtilities.h>
#include <nlohmann/json.hpp>

#define COMPONENT "sensor"
#define OBJECT_ID "voltage"

HaEntityVoltage::HaEntityVoltage(HaBridge &ha_bridge, std::string name, std::string child_object_id, bool force_update,
                                 Unit unit)
    : _unit(unit), _name(homeassistantentities::trim(name)), _force_update(force_update), _ha_bridge(ha_bridge),
      _child_object_id(child_object_id) {}

void HaEntityVoltage::publishConfiguration() {
  nlohmann::json doc;

  if (!_name.empty()) {
    doc["name"] = _name;
  } else {
    doc["name"] = (char *)NULL;
  }
  doc["device_class"] = "voltage";
  doc["force_update"] = _force_update;
  switch (_unit) {
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
