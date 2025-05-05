#include "HaEntitySwitch.h"
#include <HaUtilities.h>
#include <IJson.h>

#define COMPONENT "switch"
#define OBJECT_ID "switch"
#define OBJECT_ID_ONOFF "onoff"

// NOTE! We have swapped object ID and child object ID to get a nicer state/command topic path.

HaEntitySwitch::HaEntitySwitch(HaBridge &ha_bridge, std::string name, std::string child_object_id,
                               Configuration configuration)
    : _name(homeassistantentities::trim(name)), _ha_bridge(ha_bridge), _child_object_id(child_object_id),
      _configuration(configuration) {}

void HaEntitySwitch::publishConfiguration() {
  IJsonDocument doc;

  if (!_name.empty()) {
    doc[_ha_bridge.useAbbreviations() ? "name" : "name"] = _name;
  } else {
    doc[_ha_bridge.useAbbreviations() ? "name" : "name"] = nullptr;
  }

  doc[_ha_bridge.useAbbreviations() ? "ret" : "retain"] = _configuration.retain;

  doc[_ha_bridge.useAbbreviations() ? "stat_t" : "state_topic"] =
      _ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, _child_object_id, OBJECT_ID_ONOFF);
  doc[_ha_bridge.useAbbreviations() ? "cmd_t" : "command_topic"] =
      _ha_bridge.getTopic(HaBridge::TopicType::Command, COMPONENT, _child_object_id, OBJECT_ID_ONOFF);

  _ha_bridge.publishConfiguration(COMPONENT, OBJECT_ID, _child_object_id, doc);
}

void HaEntitySwitch::republishState() {
  if (_on) {
    publishSwitch(*_on);
  }
}

void HaEntitySwitch::publishSwitch(bool on) {
  _ha_bridge.publishMessage(
      _ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, _child_object_id, OBJECT_ID_ONOFF),
      std::string(on ? "ON" : "OFF"));
  _on = on;
}

void HaEntitySwitch::updateSwitch(bool on) {
  if (!_on || *_on != on) {
    publishSwitch(on);
  }
}

bool HaEntitySwitch::setOnState(std::function<void(bool)> state_callback) {
  return _ha_bridge.remote().subscribe(
      _ha_bridge.getTopic(HaBridge::TopicType::Command, COMPONENT, _child_object_id, OBJECT_ID_ONOFF),
      [state_callback](std::string topic, std::string message) { state_callback(message == "ON"); });
}
