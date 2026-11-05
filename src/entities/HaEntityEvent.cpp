// Copyright (c) Johan Böhlin
// SPDX-License-Identifier: MIT

#include "HaEntityEvent.h"
#include <HaUtilities.h>
#include <IJson.h>
#include <algorithm>
#include <utility>

#define COMPONENT "event"

HaEntityEvent::HaEntityEvent(HaBridge &ha_bridge, std::string name, std::string object_id, Configuration configuration)
    : _name(homeassistantentities::trim(name)), _ha_bridge(ha_bridge), _object_id(std::move(object_id)),
      _configuration(std::move(configuration)) {}

void HaEntityEvent::publishConfiguration() {
  IJsonDocument doc;

  if (!_name.empty()) {
    doc["name"] = _name;
  } else {
    doc["name"] = nullptr;
  }
  switch (_configuration.device_class) {
  case DeviceClass::Button:
    doc["device_class"] = "button";
    break;
  case DeviceClass::Motion:
    doc["device_class"] = "motion";
    break;
  case DeviceClass::Doorbell:
    doc["device_class"] = "doorbell";
    break;
  case DeviceClass::None:
    break;
  }

  doc["state_topic"] = _ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, _object_id);

  JsonArrayType event_types_array = createJsonArray(doc, "event_types");
  for (const std::string &event_type : _configuration.event_types) {
    addToJsonArray(event_types_array, event_type);
  }

  _ha_bridge.publishConfiguration(COMPONENT, _object_id, "", doc);
}

void HaEntityEvent::republishState() {
  // Events are never republished.
}

void HaEntityEvent::publishEvent(const std::string &event, const Attributes::Map &attributes) {
  const auto &event_types = _configuration.event_types;
  if (std::find(event_types.begin(), event_types.end(), event) == event_types.end()) {
    return;
  }

  IJsonDocument doc;
  doc["event_type"] = event;

  Attributes::toJson(doc, attributes, {"event_type"});

  auto message = toJsonString(doc);
  _ha_bridge.publishMessage(_ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, _object_id), message);
}
