#include "HaEntitySelect.h"
#include "ArduinoJson.h"

#define COMPONENT "select"

HaEntitySelect::HaEntitySelect(HaBridge &ha_bridge, String name, String object_id, std::set<String> options)
    : _name(name), _ha_bridge(ha_bridge), _object_id(object_id), _options(options) {
  // Calculate total string length of all options to to use when creating JSON document.
  _total_string_length_of_options = 0;
  for (const String &option : options) {
    _total_string_length_of_options += option.length();
  }
}

void HaEntitySelect::publishConfiguration() {
  DynamicJsonDocument doc(512 + _total_string_length_of_options); // This might be problematic.
  _name.trim();
  if (!_name.isEmpty()) {
    doc["name"] = _name;
  } else {
    doc["name"] = (char *)NULL;
  }
  // TODO (johboh): Allow setting doc["device_class"]?
  doc["state_topic"] = _ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, _object_id);
  doc["command_topic"] = _ha_bridge.getTopic(HaBridge::TopicType::Command, COMPONENT, _object_id);
  auto options = doc["options"].to<JsonArray>();
  for (const String &option : _options) {
    options.add(option);
  }
  _ha_bridge.publishConfiguration(COMPONENT, _object_id, "", doc);
}

void HaEntitySelect::republishState() {
  if (_selection) {
    publishSelection(*_selection);
  }
}

void HaEntitySelect::publishSelection(String option) {
  _ha_bridge.publishMessage(_ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, _object_id), option);
  _selection = std::optional<String>{option};
}

bool HaEntitySelect::setOnSelected(std::function<void(String)> select_callback) {
  return _ha_bridge.remote().subscribe(
      _ha_bridge.getTopic(HaBridge::TopicType::Command, COMPONENT, _object_id),
      [select_callback](const char *topic, const char *message) { select_callback(String(message)); });
}
