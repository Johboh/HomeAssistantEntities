// Copyright (c) Johan Böhlin
// SPDX-License-Identifier: MIT

#include "HaEntitySwitch.h"
#include <HaUtilities.h>
#include <IJson.h>
#include <utility>

#define COMPONENT "switch"
#define OBJECT_ID "switch"
#define OBJECT_ID_ONOFF "onoff"

// NOTE! We have swapped object ID and child object ID to get a nicer state/command topic path.

HaEntitySwitch::HaEntitySwitch(HaBridge &ha_bridge, std::string name, std::string child_object_id,
                               Configuration configuration)
    : _name(homeassistantentities::trim(name)), _ha_bridge(ha_bridge), _child_object_id(std::move(child_object_id)),
      _configuration(std::move(configuration)) {}

bool HaEntitySwitch::buildComponentConfiguration(ComponentConfig &out) {
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
  if (_configuration.optimistic) {
    doc["optimistic"] = true;
  }
  if (_configuration.enabled_by_default) {
    doc["enabled_by_default"] = *_configuration.enabled_by_default;
  }

  doc["retain"] = _configuration.retain;

  doc["state_topic"] = _ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, _child_object_id, OBJECT_ID_ONOFF);
  doc["command_topic"] =
      _ha_bridge.getTopic(HaBridge::TopicType::Command, COMPONENT, _child_object_id, OBJECT_ID_ONOFF);

  return true;
}

void HaEntitySwitch::publishConfiguration() {
  ComponentConfig cfg;
  if (buildComponentConfiguration(cfg)) {
    _ha_bridge.publishConfiguration(cfg.component, cfg.object_id, cfg.child_object_id, cfg.doc);
  }
}

void HaEntitySwitch::unpublishConfiguration() {
  _ha_bridge.unpublishConfiguration(COMPONENT, OBJECT_ID, _child_object_id);
}

void HaEntitySwitch::republishState() {
  if (_on) {
    publishSwitch(*_on);
  }
}

void HaEntitySwitch::publishSwitch(bool on) {
  _ha_bridge.publishMessage(
      _ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, _child_object_id, OBJECT_ID_ONOFF), on ? "ON" : "OFF");
  _on = on;
}

void HaEntitySwitch::updateSwitch(bool on) {
  if (!_on || *_on != on) {
    publishSwitch(on);
  }
}

bool HaEntitySwitch::setOnState(std::function<void(bool)> state_callback) {
  if (!state_callback) {
    return false;
  }
  return _ha_bridge.remote().subscribe(
      _ha_bridge.getTopic(HaBridge::TopicType::Command, COMPONENT, _child_object_id, OBJECT_ID_ONOFF),
      [state_callback](std::string, std::string message) { state_callback(message == "ON"); });
}
