#include "HaEntitySelect.h"
#include <HaUtilities.h>
#include <IJson.h>

#define COMPONENT "select"

HaEntitySelect::HaEntitySelect(HaBridge &ha_bridge, std::string name, std::string object_id,
                               Configuration configuration)
    : _name(homeassistantentities::trim(name)), _ha_bridge(ha_bridge), _object_id(object_id),
      _configuration(configuration) {}

void HaEntitySelect::publishConfiguration() {
  IJsonDocument doc;

  if (!_name.empty()) {
    doc[_ha_bridge.useAbbreviations() ? "name" : "name"] = _name;
  } else {
    doc[_ha_bridge.useAbbreviations() ? "name" : "name"] = nullptr;
  }

  doc[_ha_bridge.useAbbreviations() ? "ret" : "retain"] = _configuration.retain;

  doc[_ha_bridge.useAbbreviations() ? "stat_t" : "state_topic"] =
      _ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, _object_id);
  doc[_ha_bridge.useAbbreviations() ? "cmd_t" : "command_topic"] =
      _ha_bridge.getTopic(HaBridge::TopicType::Command, COMPONENT, _object_id);

  JsonArrayType options_array = createJsonArray(doc, _ha_bridge.useAbbreviations() ? "ops" : "options");
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

void HaEntitySelect::publishSelection(std::string option) {
  _ha_bridge.publishMessage(_ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, _object_id), option);
  _selection = option;
}

void HaEntitySelect::updateSelection(std::string option) {
  if (!_selection || *_selection != option) {
    publishSelection(option);
  }
}

bool HaEntitySelect::setOnSelected(std::function<void(std::string)> select_callback) {
  return _ha_bridge.remote().subscribe(
      _ha_bridge.getTopic(HaBridge::TopicType::Command, COMPONENT, _object_id),
      [select_callback](std::string topic, std::string message) { select_callback(message); });
}
