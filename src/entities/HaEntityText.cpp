#include "HaEntityText.h"
#include <HaUtilities.h>
#include <IJson.h>
#include <utility>

// NOTE! We have swapped object ID and child object ID to get a nicer state/command topic path.
#define COMPONENT "text"
#define OBJECT_ID "text"
#define OBJECT_ID_TEXT "text"

HaEntityText::HaEntityText(HaBridge &ha_bridge, std::string name, std::string child_object_id,
                           Configuration configuration)
    : _name(homeassistantentities::trim(name)), _ha_bridge(ha_bridge), _child_object_id(std::move(child_object_id)),
      _configuration(std::move(configuration)) {}

void HaEntityText::publishConfiguration() {
  IJsonDocument doc;

  if (!_name.empty()) {
    doc["name"] = _name;
  } else {
    doc["name"] = nullptr;
  }

  doc["min"] = _configuration.min_text_length;
  doc["max"] = _configuration.max_text_length;
  doc["retain"] = _configuration.retain;

  if (_configuration.is_password) {
    doc["mode"] = "password";
  } else {
    doc["mode"] = "text";
  }

  if (_configuration.with_state_topic) {
    doc["state_topic"] = _ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, _child_object_id, OBJECT_ID_TEXT);
  }

  doc["command_topic"] = _ha_bridge.getTopic(HaBridge::TopicType::Command, COMPONENT, _child_object_id, OBJECT_ID_TEXT);

  _ha_bridge.publishConfiguration(COMPONENT, OBJECT_ID, _child_object_id, doc);
}

void HaEntityText::republishState() {
  if (_str) {
    publishText(*_str);
  }
}

void HaEntityText::publishText(const std::string &str) {
  if (!_configuration.with_state_topic) {
    return;
  }
  _str = str;
  _ha_bridge.publishMessage(
      _ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, _child_object_id, OBJECT_ID_TEXT), str);
}

void HaEntityText::updateText(const std::string &str) {
  if (!_str || *_str != str) {
    publishText(str);
  }
}

bool HaEntityText::setOnText(std::function<void(std::string)> callback) {
  if (!callback) {
    return false;
  }
  return _ha_bridge.remote().subscribe(
      _ha_bridge.getTopic(HaBridge::TopicType::Command, COMPONENT, _child_object_id, OBJECT_ID_TEXT),
      [callback](std::string, std::string message) { callback(message); });
}
