// Copyright (c) Johan Böhlin
// SPDX-License-Identifier: MIT

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

bool HaEntitySelect::buildComponentConfiguration(ComponentConfig &out) {
  out.component = COMPONENT;
  out.object_id = _object_id;
  out.child_object_id = "";
  out.bridge = &_ha_bridge;

  auto &doc = out.doc;
  if (!_name.empty()) {
    doc["name"] = _name;
  } else {
    doc["name"] = nullptr;
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

  doc["state_topic"] = _ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, _object_id);
  doc["command_topic"] = _ha_bridge.getTopic(HaBridge::TopicType::Command, COMPONENT, _object_id);

  JsonArrayType options_array = createJsonArray(doc, "options");
  for (const std::string &option : _configuration.options) {
    addToJsonArray(options_array, option);
  }

  return true;
}

void HaEntitySelect::publishConfiguration() {
  ComponentConfig cfg;
  if (buildComponentConfiguration(cfg)) {
    _ha_bridge.publishConfiguration(cfg.component, cfg.object_id, cfg.child_object_id, cfg.doc);
  }
}

void HaEntitySelect::unpublishConfiguration() { _ha_bridge.unpublishConfiguration(COMPONENT, _object_id, ""); }

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
