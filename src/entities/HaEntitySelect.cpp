#include "HaEntitySelect.h"
#include <HaUtilities.h>
#include <nlohmann/json.hpp>

#define COMPONENT "select"

HaEntitySelect::HaEntitySelect(HaBridge &ha_bridge, std::string name, std::string object_id,
                               Configuration configuration)
    : _name(homeassistantentities::trim(name)), _ha_bridge(ha_bridge), _object_id(object_id),
      _configuration(configuration) {}

void HaEntitySelect::publishConfiguration() {
  nlohmann::json doc;

  if (!_name.empty()) {
    doc["name"] = _name;
  } else {
    doc["name"] = nullptr;
  }
  // TODO (johboh): Allow setting doc["device_class"]?
  doc["state_topic"] = _ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, _object_id);
  doc["command_topic"] = _ha_bridge.getTopic(HaBridge::TopicType::Command, COMPONENT, _object_id);

  for (const std::string &option : _configuration.options) {
    doc["options"].push_back(option);
  }
  _ha_bridge.publishConfiguration(COMPONENT, _object_id, "", doc);
}

void HaEntitySelect::republishState() {
  if (_selection) {
    publishSelection(*_selection);
  }
}

void HaEntitySelect::publishSelection(std::string option) {
  _ha_bridge.publishMessage(_ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, _object_id), option);
  _selection = option;
}

bool HaEntitySelect::setOnSelected(std::function<void(std::string)> select_callback) {
  return _ha_bridge.remote().subscribe(
      _ha_bridge.getTopic(HaBridge::TopicType::Command, COMPONENT, _object_id),
      [select_callback](std::string topic, std::string message) { select_callback(message); });
}
