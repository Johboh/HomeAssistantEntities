#include "HaEntityWeight.h"
#include "ArduinoJson.h"

#define COMPONENT "sensor"
#define OBJECT_ID "weight"

HaEntityWeight::HaEntityWeight(HaBridge &ha_bridge, String name, String child_object_id, bool force_update, Unit unit)
    : _unit(unit), _name(name), _force_update(force_update), _ha_bridge(ha_bridge), _child_object_id(child_object_id) {}

void HaEntityWeight::publishConfiguration() {
  DynamicJsonDocument doc(256);
  doc["name"] = _name;
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
                            String(weight));
  _weight = std::optional<double>{weight};
}
