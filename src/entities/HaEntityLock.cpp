#include "HaEntityLock.h"
#include "ArduinoJson.h"

#define COMPONENT "binary_sensor"
#define OBJECT_ID "lock"

HaEntityLock::HaEntityLock(HaBridge &ha_bridge, String name, String child_object_id)
    : _name(name), _ha_bridge(ha_bridge), _child_object_id(child_object_id) {}

void HaEntityLock::publishConfiguration() {
  DynamicJsonDocument doc(512);
  _name.trim();
  if (!_name.isEmpty()) {
    doc["name"] = _name;
  } else {
    doc["name"] = (char *)NULL;
  }
  doc["device_class"] = "lock";
  doc["state_topic"] = _ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, OBJECT_ID, _child_object_id);
  _ha_bridge.publishConfiguration(COMPONENT, OBJECT_ID, _child_object_id, doc);
}

void HaEntityLock::republishState() {
  if (_locked) {
    publishLock(*_locked);
  }
}

void HaEntityLock::publishLock(bool locked) {
  // locked == OFF
  _ha_bridge.publishMessage(_ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, OBJECT_ID, _child_object_id),
                            locked ? "OFF" : "ON");
  _locked = std::optional<bool>{locked};
}
