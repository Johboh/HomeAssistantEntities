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

bool HaEntityDeviceTrigger::buildComponentConfiguration(ComponentConfig &out) {
  if (_configuration.type.empty() || _configuration.subtype.empty()) {
    return false;
  }

  out.component = COMPONENT;
  out.object_id = _object_id;
  out.child_object_id = "";
  out.bridge = &_ha_bridge;

  auto &doc = out.doc;
  doc["automation_type"] = "trigger";
  const std::string &payload = _configuration.payload ? *_configuration.payload : _configuration.subtype;
  if (!payload.empty()) {
    doc["payload"] = payload;
  }
  doc["platform"] = "device_automation";
  doc["type"] = _configuration.type;
  doc["subtype"] = _configuration.subtype;

  doc["topic"] = _ha_bridge.getTopic(HaBridge::TopicType::Command, COMPONENT, _object_id);
  return true;
}

void HaEntityDeviceTrigger::publishConfiguration() {
  ComponentConfig cfg;
  if (buildComponentConfiguration(cfg)) {
    _ha_bridge.publishConfiguration(cfg.component, cfg.object_id, cfg.child_object_id, cfg.doc);
  }
}

void HaEntityDeviceTrigger::unpublishConfiguration() { _ha_bridge.unpublishConfiguration(COMPONENT, _object_id, ""); }

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
