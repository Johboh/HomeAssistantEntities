#include "HaEntityMotion.h"
#include "ArduinoJson.h"

#define COMPONENT "binary_sensor"
#define OBJECT_ID "motion"

HaEntityMotion::HaEntityMotion(HaBridge &ha_bridge, String name, String child_object_id)
    : _name(name), _ha_bridge(ha_bridge), _child_object_id(child_object_id) {}

void HaEntityMotion::publishConfiguration() {
  DynamicJsonDocument doc(256);
  doc["name"] = _name;
  doc["device_class"] = "motion";
  doc["state_topic"] = _ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, OBJECT_ID, _child_object_id);
  _ha_bridge.publishConfiguration(COMPONENT, OBJECT_ID, _child_object_id, doc);
}

void HaEntityMotion::republishState() {
  if (_detected) {
    publishMotion(*_detected);
  }
}

void HaEntityMotion::publishMotion(bool detected) {
  _ha_bridge.publishMessage(_ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, OBJECT_ID, _child_object_id),
                            detected ? "ON" : "OFF");
  _detected = std::optional<bool>{detected};
}
