#include "HaEntitySound.h"
#include <HaUtilities.h>
#include <IJson.h>

#define COMPONENT "binary_sensor"
#define OBJECT_ID "sound"

HaEntitySound::HaEntitySound(HaBridge &ha_bridge, std::string name, std::string child_object_id)
    : _name(homeassistantentities::trim(name)), _ha_bridge(ha_bridge), _child_object_id(child_object_id) {}

void HaEntitySound::publishConfiguration() {
  IJsonDocument doc;

  if (!_name.empty()) {
    doc["name"] = _name;
  } else {
    doc["name"] = nullptr;
  }
  doc["device_class"] = "sound";
  doc["state_topic"] = _ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, OBJECT_ID, _child_object_id);
  _ha_bridge.publishConfiguration(COMPONENT, OBJECT_ID, _child_object_id, doc);
}

void HaEntitySound::republishState() {
  if (_detected) {
    publishSound(*_detected);
  }
}

void HaEntitySound::publishSound(bool detected) {
  _ha_bridge.publishMessage(_ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, OBJECT_ID, _child_object_id),
                            detected ? "ON" : "OFF");
  _detected = detected;
}
