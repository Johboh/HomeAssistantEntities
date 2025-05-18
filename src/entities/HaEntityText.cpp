#include "HaEntityText.h"
#include <HaUtilities.h>
#include <IJson.h>

#define COMPONENT "text"
#define OBJECT_ID "text"
#define OBJECT_ID_TEXT "text"

HaEntityText::HaEntityText(HaBridge &ha_bridge, std::string name, std::string child_object_id,
                           Configuration configuration)
    : _name(homeassistantentities::trim(name)), _ha_bridge(ha_bridge), _child_object_id(child_object_id),
      _configuration(configuration) {}

void HaEntityText::publishConfiguration() {
  IJsonDocument doc;

  if (!_name.empty()) {
    doc[_ha_bridge.useAbbreviations() ? "name" : "name"] = _name;
  } else {
    doc[_ha_bridge.useAbbreviations() ? "name" : "name"] = nullptr;
  }

  doc[_ha_bridge.useAbbreviations() ? "min" : "min"] = _configuration.min_text_length;
  doc[_ha_bridge.useAbbreviations() ? "max" : "max"] = _configuration.max_text_length;
  doc[_ha_bridge.useAbbreviations() ? "frc_upd" : "force_update"] = _configuration.force_update;
  doc[_ha_bridge.useAbbreviations() ? "ret" : "retain"] = _configuration.retain;

  if (_configuration.is_password) {
    doc[_ha_bridge.useAbbreviations() ? "mode" : "mode"] = "password";
  } else {
    doc[_ha_bridge.useAbbreviations() ? "mode" : "mode"] = "text";
  }

  if (_configuration.with_state_topic) {
    doc[_ha_bridge.useAbbreviations() ? "stat_t" : "state_topic"] =
        _ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, OBJECT_ID, _child_object_id);
  }

  doc[_ha_bridge.useAbbreviations() ? "cmd_t" : "command_topic"] =
      _ha_bridge.getTopic(HaBridge::TopicType::Command, COMPONENT, _child_object_id, OBJECT_ID_TEXT);

  _ha_bridge.publishConfiguration(COMPONENT, OBJECT_ID, _child_object_id, doc);
}

void HaEntityText::republishState() {
  if (_str) {
    publishText(*_str);
  }
}

void HaEntityText::publishText(std::string str) {
  if (!_configuration.with_state_topic) {
    return;
  }
  _str = str;
  _ha_bridge.publishMessage(_ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, OBJECT_ID, _child_object_id),
                            str);
}

void HaEntityText::updateText(std::string str) {
  if (!_str || *_str != str) {
    publishText(str);
  }
}

bool HaEntityText::setOnText(std::function<void(std::string)> callback) {
  return _ha_bridge.remote().subscribe(
      _ha_bridge.getTopic(HaBridge::TopicType::Command, COMPONENT, _child_object_id, OBJECT_ID_TEXT),
      [callback](std::string topic, std::string message) { callback(message); });
}