// Copyright (c) Johan Böhlin
// SPDX-License-Identifier: MIT

#include "HaEntityButton.h"
#include <HaUtilities.h>
#include <IJson.h>
#include <utility>

#define COMPONENT "button"
#define OBJECT_ID "button"
#define OBJECT_ID_COMMAND "command"
#define PAYLOAD_PRESS "PRESS"

// NOTE! We have swapped object ID and child object ID to get a nicer state/command topic path.

HaEntityButton::HaEntityButton(HaBridge &ha_bridge, std::string name, std::string child_object_id,
                               Configuration configuration)
    : _name(homeassistantentities::trim(name)), _ha_bridge(ha_bridge), _child_object_id(std::move(child_object_id)),
      _configuration(std::move(configuration)) {}

bool HaEntityButton::buildComponentConfiguration(ComponentConfig &out) {
  out.component = COMPONENT;
  out.object_id = OBJECT_ID;
  out.child_object_id = _child_object_id;
  out.bridge = &_ha_bridge;

  auto &doc = out.doc;
  if (!_name.empty()) {
    doc["name"] = _name;
  } else {
    doc["name"] = nullptr;
  }

  auto dc = homeassistantentities::trim(_configuration.device_class);
  if (!dc.empty()) {
    doc["device_class"] = dc;
  }
  auto icon = homeassistantentities::trim(_configuration.icon);
  if (!icon.empty()) {
    doc["icon"] = icon;
  }
  auto ec = homeassistantentities::trim(_configuration.entity_category);
  if (!ec.empty()) {
    doc["entity_category"] = ec;
  }
  if (_configuration.enabled_by_default) {
    doc["enabled_by_default"] = *_configuration.enabled_by_default;
  }
  if (_configuration.retain) {
    doc["retain"] = true;
  }

  doc["payload_press"] = PAYLOAD_PRESS;
  doc["command_topic"] =
      _ha_bridge.getTopic(HaBridge::TopicType::Command, COMPONENT, _child_object_id, OBJECT_ID_COMMAND);
  return true;
}

void HaEntityButton::publishConfiguration() {
  ComponentConfig cfg;
  if (buildComponentConfiguration(cfg)) {
    _ha_bridge.publishConfiguration(cfg.component, cfg.object_id, cfg.child_object_id, cfg.doc);
  }
}

void HaEntityButton::unpublishConfiguration() {
  _ha_bridge.unpublishConfiguration(COMPONENT, OBJECT_ID, _child_object_id);
}

void HaEntityButton::republishState() {}

bool HaEntityButton::setOnPressed(std::function<void(void)> callback) {
  if (!callback) {
    return false;
  }
  return _ha_bridge.remote().subscribe(
      _ha_bridge.getTopic(HaBridge::TopicType::Command, COMPONENT, _child_object_id, OBJECT_ID_COMMAND),
      [callback](std::string, std::string message) {
        if (message == PAYLOAD_PRESS) {
          callback();
        }
      });
}
