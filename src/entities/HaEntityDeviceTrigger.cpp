// Copyright (c) Johan Böhlin
// SPDX-License-Identifier: MIT

#include "HaEntityDeviceTrigger.h"
#include <HaUtilities.h>
#include <IJson.h>
#include <utility>

#define COMPONENT "device_automation"

HaEntityDeviceTrigger::HaEntityDeviceTrigger(HaBridge &ha_bridge, std::string object_id, Configuration configuration)
    : _ha_bridge(ha_bridge), _object_id(homeassistantentities::trim(object_id)),
      _configuration(std::move(configuration)) {
  _configuration.type = homeassistantentities::trim(_configuration.type);
  _configuration.subtype = homeassistantentities::trim(_configuration.subtype);
}

void HaEntityDeviceTrigger::publishConfiguration() {
  if (_configuration.type.empty() || _configuration.subtype.empty()) {
    return;
  }

  IJsonDocument doc;

  doc["automation_type"] = "trigger";
  const std::string &payload = _configuration.payload ? *_configuration.payload : _configuration.subtype;
  if (!payload.empty()) {
    doc["payload"] = payload;
  }
  doc["platform"] = "device_automation";
  doc["type"] = _configuration.type;
  doc["subtype"] = _configuration.subtype;

  doc["topic"] = _ha_bridge.getTopic(HaBridge::TopicType::Command, COMPONENT, _object_id);

  _ha_bridge.publishConfiguration(COMPONENT, _object_id, "", doc);
}

void HaEntityDeviceTrigger::republishState() {
  // Device triggers are never republished.
}

void HaEntityDeviceTrigger::publishTrigger() {
  if (_configuration.type.empty() || _configuration.subtype.empty()) {
    return;
  }
  const std::string &payload = _configuration.payload ? *_configuration.payload : _configuration.subtype;
  _ha_bridge.publishMessage(_ha_bridge.getTopic(HaBridge::TopicType::Command, COMPONENT, _object_id), payload);
}
