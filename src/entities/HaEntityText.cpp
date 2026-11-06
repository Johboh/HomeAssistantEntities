// Copyright (c) Johan Böhlin
// SPDX-License-Identifier: MIT

#include "HaEntityText.h"
#include <HaUtilities.h>
#include <IJson.h>
#include <algorithm>
#include <utility>

// NOTE! We have swapped object ID and child object ID to get a nicer state/command topic path.
#define COMPONENT "text"
#define OBJECT_ID "text"
#define OBJECT_ID_TEXT "text"

HaEntityText::HaEntityText(HaBridge &ha_bridge, std::string name, std::string child_object_id,
                           Configuration configuration)
    : _name(homeassistantentities::trim(name)), _ha_bridge(ha_bridge), _child_object_id(std::move(child_object_id)),
      _configuration(std::move(configuration)) {
  if (_configuration.min_text_length > _configuration.max_text_length) {
    std::swap(_configuration.min_text_length, _configuration.max_text_length);
  }
}

bool HaEntityText::buildComponentConfiguration(ComponentConfig &out) {
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

  doc["min"] = _configuration.min_text_length;
  doc["max"] = _configuration.max_text_length;
  doc["retain"] = _configuration.retain;

  if (!_configuration.pattern.empty()) {
    doc["pattern"] = _configuration.pattern;
  }

  if (_configuration.is_password) {
    doc["mode"] = "password";
  } else {
    doc["mode"] = "text";
  }

  if (_configuration.with_state_topic) {
    doc["state_topic"] = _ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, _child_object_id, OBJECT_ID_TEXT);
  }

  doc["command_topic"] = _ha_bridge.getTopic(HaBridge::TopicType::Command, COMPONENT, _child_object_id, OBJECT_ID_TEXT);

  return true;
}

void HaEntityText::publishConfiguration() {
  ComponentConfig cfg;
  if (buildComponentConfiguration(cfg)) {
    _ha_bridge.publishConfiguration(cfg.component, cfg.object_id, cfg.child_object_id, cfg.doc);
  }
}

void HaEntityText::unpublishConfiguration() {
  _ha_bridge.unpublishConfiguration(COMPONENT, OBJECT_ID, _child_object_id);
}

void HaEntityText::republishState() {
  if (_str) {
    publish(*_str);
  }
}

void HaEntityText::publish(const std::string &str) {
  if (!_configuration.with_state_topic) {
    return;
  }
  auto len = homeassistantentities::utf8CodePointCount(str);
  if (len < _configuration.min_text_length || len > _configuration.max_text_length) {
    return;
  }
  _str = str;
  _ha_bridge.publishMessage(
      _ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, _child_object_id, OBJECT_ID_TEXT), str);
}

void HaEntityText::update(const std::string &str) {
  auto len = homeassistantentities::utf8CodePointCount(str);
  if (len < _configuration.min_text_length || len > _configuration.max_text_length) {
    return;
  }
  if (!_str || *_str != str) {
    publish(str);
  }
}

bool HaEntityText::onCommand(std::function<void(std::string)> callback) {
  if (!callback) {
    return false;
  }
  uint8_t min_len = _configuration.min_text_length;
  uint8_t max_len = _configuration.max_text_length;
  return _ha_bridge.remote().subscribe(
      _ha_bridge.getTopic(HaBridge::TopicType::Command, COMPONENT, _child_object_id, OBJECT_ID_TEXT),
      [callback, min_len, max_len](std::string, std::string message) {
        auto len = homeassistantentities::utf8CodePointCount(message);
        if (len >= min_len && len <= max_len) {
          callback(message);
        }
      });
}
