#include "HaEntityBrightness.h"
#include <HaUtilities.h>
#include <IJson.h>

#define COMPONENT "sensor"
#define OBJECT_ID "brightness"

HaEntityBrightness::HaEntityBrightness(HaBridge &ha_bridge, std::string name, std::string child_object_id,
                                       Configuration configuration)
    : _name(homeassistantentities::trim(name)), _ha_bridge(ha_bridge), _child_object_id(child_object_id),
      _configuration(configuration) {}

void HaEntityBrightness::publishConfiguration() {
  IJsonDocument doc;

  if (!_name.empty()) {
    doc["name"] = _name;
  } else {
    doc["name"] = nullptr;
  }
  doc["unit_of_measurement"] = "%";
  doc["force_update"] = _configuration.force_update;
  doc["icon"] = "mdi:brightness-percent";
  doc["state_topic"] = _ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, OBJECT_ID, _child_object_id);
  _ha_bridge.publishConfiguration(COMPONENT, OBJECT_ID, _child_object_id, doc);
}

void HaEntityBrightness::republishState() {
  if (_brightness) {
    publishBrightness(*_brightness);
  }
}

void HaEntityBrightness::publishBrightness(double brightness) {
  _ha_bridge.publishMessage(_ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, OBJECT_ID, _child_object_id),
                            std::to_string(brightness));
  _brightness = brightness;
}
