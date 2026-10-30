#include "HaEntitySelect.h"
#include <HaUtilities.h>
#include <IJson.h>
#include <algorithm>
#include <utility>

#define COMPONENT "select"

HaEntitySelect::HaEntitySelect(HaBridge &ha_bridge, std::string name, std::string object_id,
                               Configuration configuration)
    : _name(homeassistantentities::trim(name)), _ha_bridge(ha_bridge), _object_id(std::move(object_id)),
      _configuration(std::move(configuration)) {}

void HaEntitySelect::publishConfiguration() {
  IJsonDocument doc;

  if (!_name.empty()) {
    doc["name"] = _name;
  } else {
    doc["name"] = nullptr;
  }

  doc["retain"] = _configuration.retain;

  doc["state_topic"] = _ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, _object_id);
  doc["command_topic"] = _ha_bridge.getTopic(HaBridge::TopicType::Command, COMPONENT, _object_id);

  JsonArrayType options_array = createJsonArray(doc, "options");
  for (const std::string &option : _configuration.options) {
    addToJsonArray(options_array, option);
  }

  _ha_bridge.publishConfiguration(COMPONENT, _object_id, "", doc);
}

void HaEntitySelect::republishState() {
  if (_selection) {
    publishSelection(*_selection);
  }
}

void HaEntitySelect::publishSelection(const std::string &option) {
  const auto &options = _configuration.options;
  if (std::find(options.begin(), options.end(), option) == options.end()) {
    return;
  }
  _ha_bridge.publishMessage(_ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, _object_id), option);
  _selection = option;
}

void HaEntitySelect::updateSelection(const std::string &option) {
  if (!_selection || *_selection != option) {
    publishSelection(option);
  }
}

bool HaEntitySelect::setOnSelected(std::function<void(std::string)> select_callback) {
  if (!select_callback) {
    return false;
  }
  return _ha_bridge.remote().subscribe(
      _ha_bridge.getTopic(HaBridge::TopicType::Command, COMPONENT, _object_id),
      [select_callback, options = _configuration.options](std::string, std::string message) {
        if (std::find(options.begin(), options.end(), message) != options.end()) {
          select_callback(message);
        }
      });
}
