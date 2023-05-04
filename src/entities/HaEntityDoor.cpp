#include "HaEntityDoor.h"
#include "ArduinoJson.h"

#define COMPONENT "binary_sensor"
#define OBJECT_ID "door"

HaEntityDoor::HaEntityDoor(HaBridge &ha_bridge, String name, String child_object_id)
    : _name(name), _ha_bridge(ha_bridge), _child_object_id(child_object_id) {}

void HaEntityDoor::publishConfiguration() {
  DynamicJsonDocument doc(512);
  doc["name"] = _name;
  doc["device_class"] = "door";
  doc["state_topic"] = _ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, OBJECT_ID, _child_object_id);
  _ha_bridge.publishConfiguration(COMPONENT, OBJECT_ID, _child_object_id, doc);
}

void HaEntityDoor::republishState() {
  if (_open) {
    publishDoor(*_open);
  }
}

void HaEntityDoor::publishDoor(bool open) {
  _ha_bridge.publishMessage(_ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, OBJECT_ID, _child_object_id),
                            open ? "ON" : "OFF");
  _open = std::optional<bool>{open};
}
