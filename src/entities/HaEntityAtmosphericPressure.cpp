#include "HaEntityAtmosphericPressure.h"
#include <HaUtilities.h>
#include <nlohmann/json.hpp>

#define COMPONENT "sensor"
#define OBJECT_ID "atmospheric_pressure"

HaEntityAtmosphericPressure::HaEntityAtmosphericPressure(HaBridge &ha_bridge, std::string name,
                                                         std::string child_object_id, Configuration configuration)
    : _name(homeassistantentities::trim(name)), _ha_bridge(ha_bridge), _child_object_id(child_object_id),
      _configuration(configuration) {}

void HaEntityAtmosphericPressure::publishConfiguration() {
  nlohmann::json doc;

  if (!_name.empty()) {
    doc["name"] = _name;
  } else {
    doc["name"] = nullptr;
  }
  doc["unit_of_measurement"] = "hPa";
  doc["force_update"] = _configuration.force_update;
  doc["device_class"] = "atmospheric_pressure";
  doc["state_topic"] = _ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, OBJECT_ID, _child_object_id);
  _ha_bridge.publishConfiguration(COMPONENT, OBJECT_ID, _child_object_id, doc);
}

void HaEntityAtmosphericPressure::republishState() {
  if (_pressure) {
    publishAtmosphericPressure(*_pressure);
  }
}

void HaEntityAtmosphericPressure::publishAtmosphericPressure(double pressure) {
  _ha_bridge.publishMessage(_ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, OBJECT_ID, _child_object_id),
                            std::to_string(pressure));
  _pressure = pressure;
}
