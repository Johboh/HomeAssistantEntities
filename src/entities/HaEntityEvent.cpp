#include "HaEntityEvent.h"
#include "ArduinoJson.h"

#define COMPONENT "event"

HaEntityEvent::HaEntityEvent(HaBridge &ha_bridge, String name, String object_id, std::set<String> event_types)
    : _name(name), _ha_bridge(ha_bridge), _object_id(object_id), _event_types(event_types) {
  // Calculate total string length of all events to to use when creating JSON document.
  _total_string_length_of_event_types = 0;
  for (const String &event_type : event_types) {
    _total_string_length_of_event_types += event_type.length();
  }
}

void HaEntityEvent::publishConfiguration() {
  DynamicJsonDocument doc(512 + _total_string_length_of_event_types); // This might be problematic.
  _name.trim();
  if (!_name.isEmpty()) {
    doc["name"] = _name;
  } else {
    doc["name"] = (char *)NULL;
  }
  // TODO (johboh): Allow setting doc["device_class"]?
  doc["state_topic"] = _ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, _object_id);
  auto event_types = doc["event_types"].to<JsonArray>();
  for (const String &event_type : _event_types) {
    event_types.add(event_type);
  }
  _ha_bridge.publishConfiguration(COMPONENT, _object_id, "", doc);
}

void HaEntityEvent::republishState() {
  // Events are never republished.
}

void HaEntityEvent::publishEvent(String event, std::map<std::string, ATTRIBUTE_VARIANTS> attributes) {
  // Calculate approx. how much json document size we need.
  unsigned long additional_attribute_size = 0;
  for (const std::pair<std::string, ATTRIBUTE_VARIANTS> &attribute : attributes) {
    additional_attribute_size += attribute.first.size() + 10; // Arbritrary value size.
  }

  DynamicJsonDocument doc(256 + additional_attribute_size);
  doc["event_type"] = event;

  // Add known attributes.
  for (const std::pair<std::string, ATTRIBUTE_VARIANTS> &attribute : attributes) {
    // Keys with name "event_type" is not allowed.
    auto key = attribute.first;
    if (key == "event_type") {
      continue;
    }

    auto value = attribute.second;
    if (std::holds_alternative<double>(value)) {
      doc[key] = std::get<double>(value);
    } else if (std::holds_alternative<float>(value)) {
      doc[key] = std::get<float>(value);
    } else if (std::holds_alternative<bool>(value)) {
      doc[key] = std::get<bool>(value);
    } else if (std::holds_alternative<int>(value)) {
      doc[key] = std::get<int>(value);
    } else if (std::holds_alternative<String>(value)) {
      doc[key] = std::get<String>(value);
    } else if (std::holds_alternative<std::string>(value)) {
      doc[key] = std::get<std::string>(value);
    } else if (std::holds_alternative<const char *>(value)) {
      doc[key] = std::get<const char *>(value);
    }
  }

  String message;
  serializeJson(doc, message);
  _ha_bridge.publishMessage(_ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, _object_id), message);
}
