#include "HaEntityButton.h"
#include <HaUtilities.h>
#include <IJson.h>

#define COMPONENT "button"
#define OBJECT_ID "button"
#define OBJECT_ID_COMMAND "command"
#define PAYLOAD_PRESS "PRESS"

// NOTE! We have swapped object ID and child object ID to get a nicer state/command topic path.

HaEntityButton::HaEntityButton(HaBridge &ha_bridge, std::string name, std::string child_object_id)
    : _name(homeassistantentities::trim(name)), _ha_bridge(ha_bridge), _child_object_id(child_object_id) {}

void HaEntityButton::publishConfiguration() {
  IJsonDocument doc;

  if (!_name.empty()) {
    doc["name"] = _name;
  } else {
    doc["name"] = nullptr;
  }
  doc["payload_press"] = PAYLOAD_PRESS;
  doc["command_topic"] =
      _ha_bridge.getTopic(HaBridge::TopicType::Command, COMPONENT, _child_object_id, OBJECT_ID_COMMAND);

  _ha_bridge.publishConfiguration(COMPONENT, OBJECT_ID, _child_object_id, doc);
}

void HaEntityButton::republishState() {}

bool HaEntityButton::setOnPressed(std::function<void(void)> callback) {
  return _ha_bridge.remote().subscribe(
      _ha_bridge.getTopic(HaBridge::TopicType::Command, COMPONENT, _child_object_id, OBJECT_ID_COMMAND),
      [callback](std::string topic, std::string message) {
        if (message == PAYLOAD_PRESS) {
          callback();
        }
      });
}
