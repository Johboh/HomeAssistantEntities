#include "HaEntityWeight.h"
#include <HaUtilities.h>
#include <nlohmann/json.hpp>

#define COMPONENT "sensor"
#define OBJECT_ID "weight"

HaEntityWeight::HaEntityWeight(HaBridge &ha_bridge, std::string name, std::string child_object_id, bool force_update,
                               Unit unit)
    : _unit(unit), _name(homeassistantentities::trim(name)), _force_update(force_update), _ha_bridge(ha_bridge),
      _child_object_id(child_object_id) {}

void HaEntityWeight::publishConfiguration() {
  nlohmann::json doc;

  if (!_name.empty()) {
    doc["name"] = _name;
  } else {
    doc["name"] = nullptr;
  }
  doc["device_class"] = "weight";
  doc["force_update"] = _force_update;
  switch (_unit) {
  case Unit::kg:
    doc["unit_of_measurement"] = "kg";
    break;
  case Unit::g:
    doc["unit_of_measurement"] = "g";
    break;
  }
  doc["state_topic"] = _ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, OBJECT_ID, _child_object_id);
  _ha_bridge.publishConfiguration(COMPONENT, OBJECT_ID, _child_object_id, doc);
}

void HaEntityWeight::republishState() {
  if (_weight) {
    publishWeight(*_weight);
  }
}

void HaEntityWeight::publishWeight(double weight) {
  _ha_bridge.publishMessage(_ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, OBJECT_ID, _child_object_id),
                            std::to_string(weight));
  _weight = weight;
}
