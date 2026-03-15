#include "HaEntityDeviceTrigger.h"
#include <HaUtilities.h>
#include <IJson.h>

#define COMPONENT "device_automation"

HaEntityDeviceTrigger::HaEntityDeviceTrigger(HaBridge &ha_bridge, std::string object_id, Configuration configuration)
  : _ha_bridge(ha_bridge), _object_id(object_id), _configuration(configuration) {}

void HaEntityDeviceTrigger::publishConfiguration() {
  IJsonDocument doc;

  doc["automation_type"] = "trigger";
  doc["payload"] = _configuration.subtype;
  doc["platform"] = "device_automation";
  doc["type"] = _configuration.type;
  doc["subtype"] = _configuration.subtype;
  
  doc["topic"] = _ha_bridge.getTopic(HaBridge::TopicType::Command, COMPONENT, _object_id);

  _ha_bridge.publishConfiguration(COMPONENT, _object_id, "", doc);
}

void HaEntityDeviceTrigger::republishState() {
  // Device triggers are never republished.
}

void HaEntityDeviceTrigger::publishTrigger() {
  _ha_bridge.publishMessage(_ha_bridge.getTopic(HaBridge::TopicType::Command, COMPONENT, _object_id), _configuration.subtype);
}