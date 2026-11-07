// Copyright (c) Johan Böhlin
// SPDX-License-Identifier: MIT

#include "HaEntityHumidifier.h"
#include <HaUtilities.h>
#include <IJson.h>
#include <algorithm>
#include <cstdlib>
#include <utility>

using namespace homeassistantentities;

#define COMPONENT "humidifier"
#define OBJECT_ID "humidifier"
#define OBJECT_ID_ONOFF "onoff"
#define OBJECT_ID_TARGET_HUMIDITY "target_humidity"
#define OBJECT_ID_CURRENT_HUMIDITY "current_humidity"
#define OBJECT_ID_ACTION "action"
#define OBJECT_ID_MODE "mode"

HaEntityHumidifier::HaEntityHumidifier(HaBridge &ha_bridge, std::string name,
                                       std::optional<std::string> child_object_id, Configuration configuration)
    : _name(homeassistantentities::trim(name)), _ha_bridge(ha_bridge), _child_object_id(child_object_id.value_or("")),
      _configuration(std::move(configuration)) {}

std::string HaEntityHumidifier::actionToString(Action action) {
  switch (action) {
  case Action::Off:
    return "off";
  case Action::Humidifying:
    return "humidifying";
  case Action::Drying:
    return "drying";
  case Action::Idle:
    return "idle";
  case Action::Unknown:
    return "";
  }
  return "";
}

std::optional<HaEntityHumidifier::Action> HaEntityHumidifier::stringToAction(const std::string &str) {
  if (str == "off")
    return Action::Off;
  if (str == "humidifying")
    return Action::Humidifying;
  if (str == "drying")
    return Action::Drying;
  if (str == "idle")
    return Action::Idle;
  return std::nullopt;
}

bool HaEntityHumidifier::buildComponentConfiguration(ComponentConfig &out) {
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

  doc["device_class"] = (_configuration.device_class == DeviceClass::Dehumidifier) ? "dehumidifier" : "humidifier";

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
  doc["min_humidity"] = _configuration.min_humidity;
  doc["max_humidity"] = _configuration.max_humidity;

  doc["state_topic"] = _ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, _child_object_id, OBJECT_ID_ONOFF);
  doc["command_topic"] =
      _ha_bridge.getTopic(HaBridge::TopicType::Command, COMPONENT, _child_object_id, OBJECT_ID_ONOFF);

  doc["target_humidity_state_topic"] =
      _ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, _child_object_id, OBJECT_ID_TARGET_HUMIDITY);
  doc["target_humidity_command_topic"] =
      _ha_bridge.getTopic(HaBridge::TopicType::Command, COMPONENT, _child_object_id, OBJECT_ID_TARGET_HUMIDITY);

  if (_configuration.with_current_humidity) {
    doc["current_humidity_topic"] =
        _ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, _child_object_id, OBJECT_ID_CURRENT_HUMIDITY);
  }

  if (_configuration.with_action) {
    doc["action_topic"] =
        _ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, _child_object_id, OBJECT_ID_ACTION);
  }

  if (!_configuration.modes.empty()) {
    JsonArrayType modes_array = createJsonArray(doc, "modes");
    for (const auto &m : _configuration.modes) {
      addToJsonArray(modes_array, m);
    }
    doc["mode_state_topic"] =
        _ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, _child_object_id, OBJECT_ID_MODE);
    doc["mode_command_topic"] =
        _ha_bridge.getTopic(HaBridge::TopicType::Command, COMPONENT, _child_object_id, OBJECT_ID_MODE);
  }

  if (_configuration.with_attributes) {
    doc["json_attributes_topic"] =
        _ha_bridge.getTopic(HaBridge::TopicType::Attributes, COMPONENT, _child_object_id, "");
  }

  return true;
}

void HaEntityHumidifier::publishConfiguration() {
  ComponentConfig cfg;
  if (buildComponentConfiguration(cfg)) {
    _ha_bridge.publishConfiguration(cfg.component, cfg.object_id, cfg.child_object_id, cfg.doc);
  }
}

void HaEntityHumidifier::unpublishConfiguration() {
  _ha_bridge.unpublishConfiguration(COMPONENT, OBJECT_ID, _child_object_id);
}

void HaEntityHumidifier::republishState() {
  if (_on) {
    publish(*_on);
  }
  if (_target_humidity) {
    publishTargetHumidity(*_target_humidity);
  }
  if (_current_humidity) {
    publishCurrentHumidity(*_current_humidity);
  }
  if (_action) {
    publishAction(*_action);
  }
  if (_mode) {
    publishMode(*_mode);
  }
  if (_attributes) {
    publishAttributes(*_attributes);
  }
}

void HaEntityHumidifier::publish(bool on) {
  _on = on;
  _ha_bridge.publishMessage(
      _ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, _child_object_id, OBJECT_ID_ONOFF), on ? "ON" : "OFF");
}

void HaEntityHumidifier::update(bool on) {
  if (!_on || *_on != on) {
    publish(on);
  }
}

bool HaEntityHumidifier::onCommand(std::function<void(bool on)> callback) {
  if (!callback) {
    return false;
  }
  return _ha_bridge.remote().subscribe(
      _ha_bridge.getTopic(HaBridge::TopicType::Command, COMPONENT, _child_object_id, OBJECT_ID_ONOFF),
      [callback](std::string, std::string message) {
        callback(message == "ON" || message == "on" || message == "true" || message == "1");
      });
}

void HaEntityHumidifier::publishTargetHumidity(uint8_t humidity) {
  _target_humidity = humidity;
  _ha_bridge.publishMessage(
      _ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, _child_object_id, OBJECT_ID_TARGET_HUMIDITY),
      std::to_string(humidity));
}

void HaEntityHumidifier::updateTargetHumidity(uint8_t humidity) {
  if (!_target_humidity || *_target_humidity != humidity) {
    publishTargetHumidity(humidity);
  }
}

bool HaEntityHumidifier::onTargetHumidity(std::function<void(uint8_t humidity)> callback) {
  if (!callback) {
    return false;
  }
  return _ha_bridge.remote().subscribe(
      _ha_bridge.getTopic(HaBridge::TopicType::Command, COMPONENT, _child_object_id, OBJECT_ID_TARGET_HUMIDITY),
      [callback](std::string, std::string message) {
        char *end = nullptr;
        long val = std::strtol(message.c_str(), &end, 10);
        if (end != message.c_str() && *end == '\0' && val >= 0 && val <= 100) {
          callback(static_cast<uint8_t>(val));
        }
      });
}

void HaEntityHumidifier::publishCurrentHumidity(uint8_t humidity) {
  _current_humidity = humidity;
  _ha_bridge.publishMessage(
      _ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, _child_object_id, OBJECT_ID_CURRENT_HUMIDITY),
      std::to_string(humidity));
}

void HaEntityHumidifier::updateCurrentHumidity(uint8_t humidity) {
  if (!_current_humidity || *_current_humidity != humidity) {
    publishCurrentHumidity(humidity);
  }
}

void HaEntityHumidifier::publishAction(Action action) { publishAction(actionToString(action)); }

void HaEntityHumidifier::publishAction(const std::string &action) {
  _action = action;
  _ha_bridge.publishMessage(
      _ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, _child_object_id, OBJECT_ID_ACTION), action);
}

void HaEntityHumidifier::updateAction(Action action) { updateAction(actionToString(action)); }

void HaEntityHumidifier::updateAction(const std::string &action) {
  if (!_action || *_action != action) {
    publishAction(action);
  }
}

void HaEntityHumidifier::publishMode(const std::string &mode) {
  _mode = mode;
  _ha_bridge.publishMessage(
      _ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, _child_object_id, OBJECT_ID_MODE), mode);
}

void HaEntityHumidifier::updateMode(const std::string &mode) {
  if (!_mode || *_mode != mode) {
    publishMode(mode);
  }
}

bool HaEntityHumidifier::onMode(std::function<void(const std::string &mode)> callback) {
  if (_configuration.modes.empty() || !callback) {
    return false;
  }
  return _ha_bridge.remote().subscribe(
      _ha_bridge.getTopic(HaBridge::TopicType::Command, COMPONENT, _child_object_id, OBJECT_ID_MODE),
      [callback, modes = _configuration.modes](std::string, std::string message) {
        if (std::find(modes.begin(), modes.end(), message) != modes.end()) {
          callback(message);
        }
      });
}

void HaEntityHumidifier::publishAttributes(const Attributes::Map &attributes) {
  if (!_configuration.with_attributes) {
    return;
  }
  IJsonDocument doc;
  if (!Attributes::toJson(doc, attributes)) {
    return;
  }
  _attributes = attributes;
  _ha_bridge.publishMessage(_ha_bridge.getTopic(HaBridge::TopicType::Attributes, COMPONENT, _child_object_id, ""),
                            toJsonString(doc));
}
