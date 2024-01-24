#include "HaEntityTemperature.h"
#include <HaUtilities.h>
#include <nlohmann/json.hpp>

#define COMPONENT "sensor"
#define OBJECT_ID "temperature"

HaEntityTemperature::HaEntityTemperature(HaBridge &ha_bridge, std::string name, std::string child_object_id,
                                         Configuration configuration)
    : _name(homeassistantentities::trim(name)), _ha_bridge(ha_bridge), _child_object_id(child_object_id),
      _configuration(configuration) {}

void HaEntityTemperature::publishConfiguration() {
  nlohmann::json doc;

  if (!_name.empty()) {
    doc["name"] = _name;
  } else {
    doc["name"] = nullptr;
  }
  switch (_configuration.unit) {
  case Unit::C:
    doc["unit_of_measurement"] = "°C";
    break;
  case Unit::F:
    doc["unit_of_measurement"] = "°F";
    break;
  }
  doc["device_class"] = "temperature";
  doc["force_update"] = _configuration.force_update;
  doc["state_topic"] = _ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, OBJECT_ID, _child_object_id);
  _ha_bridge.publishConfiguration(COMPONENT, OBJECT_ID, _child_object_id, doc);
}

void HaEntityTemperature::republishState() {
  if (_temperature) {
    publishTemperature(*_temperature);
  }
}

void HaEntityTemperature::publishTemperature(double temperature) {
  _ha_bridge.publishMessage(_ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, OBJECT_ID, _child_object_id),
                            std::to_string(temperature));
  _temperature = temperature;
}
