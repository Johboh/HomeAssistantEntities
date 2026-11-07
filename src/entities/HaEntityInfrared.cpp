// Copyright (c) Johan Böhlin
// SPDX-License-Identifier: MIT

#include "HaEntityInfrared.h"
#include <HaUtilities.h>
#include <IJson.h>
#include <utility>

using namespace homeassistantentities;

#define COMPONENT "infrared"
#define OBJECT_ID "infrared"
#define OBJECT_ID_COMMAND "command"
#define OBJECT_ID_STATE "state"

HaEntityInfrared::HaEntityInfrared(HaBridge &ha_bridge, std::string name, std::optional<std::string> child_object_id,
                                   Configuration configuration)
    : _name(homeassistantentities::trim(name)), _ha_bridge(ha_bridge), _child_object_id(child_object_id.value_or("")),
      _configuration(std::move(configuration)) {}

bool HaEntityInfrared::buildComponentConfiguration(ComponentConfig &out) {
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
  if (_configuration.enabled_by_default) {
    doc["enabled_by_default"] = *_configuration.enabled_by_default;
  }

  if (_configuration.type == Type::Emitter || _configuration.type == Type::Transceiver) {
    doc["command_topic"] =
        _ha_bridge.getTopic(HaBridge::TopicType::Command, COMPONENT, _child_object_id, OBJECT_ID_COMMAND);
    if (!_configuration.command_template.empty()) {
      doc["command_template"] = _configuration.command_template;
    }
  }

  if (_configuration.type == Type::Receiver || _configuration.type == Type::Transceiver) {
    doc["state_topic"] = _ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, _child_object_id, OBJECT_ID_STATE);
    if (!_configuration.value_template.empty()) {
      doc["value_template"] = _configuration.value_template;
    }
  }

  return true;
}

void HaEntityInfrared::publishConfiguration() {
  ComponentConfig cfg;
  if (buildComponentConfiguration(cfg)) {
    _ha_bridge.publishConfiguration(cfg.component, cfg.object_id, cfg.child_object_id, cfg.doc);
  }
}

void HaEntityInfrared::unpublishConfiguration() {
  _ha_bridge.unpublishConfiguration(COMPONENT, OBJECT_ID, _child_object_id);
}

void HaEntityInfrared::republishState() {
  if (_last_code) {
    publish(*_last_code);
  }
}

void HaEntityInfrared::publish(const std::string &code) {
  if (_configuration.type != Type::Receiver && _configuration.type != Type::Transceiver) {
    return;
  }
  _last_code = code;
  _ha_bridge.publishMessage(
      _ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, _child_object_id, OBJECT_ID_STATE), code);
}

bool HaEntityInfrared::onCommand(std::function<void(const std::string &code)> callback) {
  if ((_configuration.type != Type::Emitter && _configuration.type != Type::Transceiver) || !callback) {
    return false;
  }
  return _ha_bridge.remote().subscribe(
      _ha_bridge.getTopic(HaBridge::TopicType::Command, COMPONENT, _child_object_id, OBJECT_ID_COMMAND),
      [callback](std::string, std::string message) { callback(message); });
}
