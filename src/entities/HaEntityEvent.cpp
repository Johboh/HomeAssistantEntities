#include "HaEntityEvent.h"
#include <HaUtilities.h>
#include <nlohmann/json.hpp>

#define COMPONENT "event"

HaEntityEvent::HaEntityEvent(HaBridge &ha_bridge, std::string name, std::string object_id,
                             std::set<std::string> event_types, HaEntityEvent::DeviceClass device_class)
    : _name(homeassistantentities::trim(name)), _ha_bridge(ha_bridge), _object_id(object_id), _event_types(event_types),
      _device_class(device_class) {}

void HaEntityEvent::publishConfiguration() {
  nlohmann::json doc;

  if (!_name.empty()) {
    doc["name"] = _name;
  } else {
    doc["name"] = nullptr;
  }
  switch (_device_class) {
  case DeviceClass::Button:
    doc["device_class"] = "button";
    break;
  case DeviceClass::Motion:
    doc["device_class"] = "motion";
    break;
  case DeviceClass::Dorrbell:
    doc["device_class"] = "doorbell";
    break;
  case DeviceClass::None:
    break;
  }

  doc["state_topic"] = _ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, _object_id);

  for (const std::string &event_type : _event_types) {
    doc["event_types"].push_back(event_type);
  }
  _ha_bridge.publishConfiguration(COMPONENT, _object_id, "", doc);
}

void HaEntityEvent::republishState() {
  // Events are never republished.
}

void HaEntityEvent::publishEvent(std::string event, std::map<std::string, ATTRIBUTE_VARIANTS> attributes) {
  nlohmann::json doc;
  doc["event_type"] = event;

  // Add known attributes.
  for (const auto &attribute : attributes) {
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
    } else if (std::holds_alternative<std::string>(value)) {
      doc[key] = std::get<std::string>(value);
    } else if (std::holds_alternative<const char *>(value)) {
      doc[key] = std::get<const char *>(value);
    }
  }

  auto message = doc.dump();
  _ha_bridge.publishMessage(_ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, _object_id), message);
}
