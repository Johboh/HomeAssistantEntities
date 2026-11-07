// Copyright (c) Johan Böhlin
// SPDX-License-Identifier: MIT

#include "HaEntityVacuum.h"
#include <HaUtilities.h>
#include <IJson.h>
#include <algorithm>
#include <utility>

using namespace homeassistantentities;

#define COMPONENT "vacuum"
#define OBJECT_ID "vacuum"
#define OBJECT_ID_STATE "state"
#define OBJECT_ID_COMMAND "command"
#define OBJECT_ID_BATTERY "battery"
#define OBJECT_ID_FAN_SPEED "fan_speed"

HaEntityVacuum::HaEntityVacuum(HaBridge &ha_bridge, std::string name, std::optional<std::string> child_object_id,
                               Configuration configuration)
    : _name(homeassistantentities::trim(name)), _ha_bridge(ha_bridge), _child_object_id(child_object_id.value_or("")),
      _configuration(std::move(configuration)) {}

std::string HaEntityVacuum::stateToString(State state) {
  switch (state) {
  case State::Cleaning:
    return "cleaning";
  case State::Docked:
    return "docked";
  case State::Paused:
    return "paused";
  case State::Idle:
    return "idle";
  case State::Returning:
    return "returning";
  case State::Error:
    return "error";
  }
  return "docked";
}

std::optional<HaEntityVacuum::State> HaEntityVacuum::stringToState(const std::string &str) {
  if (str == "cleaning")
    return State::Cleaning;
  if (str == "docked")
    return State::Docked;
  if (str == "paused")
    return State::Paused;
  if (str == "idle")
    return State::Idle;
  if (str == "returning")
    return State::Returning;
  if (str == "error")
    return State::Error;
  return std::nullopt;
}

std::string HaEntityVacuum::commandToString(Command command) {
  switch (command) {
  case Command::Start:
    return "start";
  case Command::Pause:
    return "pause";
  case Command::Stop:
    return "stop";
  case Command::ReturnToBase:
    return "return_to_base";
  case Command::CleanSpot:
    return "clean_spot";
  case Command::Locate:
    return "locate";
  case Command::Unknown:
    return "";
  }
  return "";
}

HaEntityVacuum::Command HaEntityVacuum::stringToCommand(const std::string &str) {
  if (str == "start")
    return Command::Start;
  if (str == "pause")
    return Command::Pause;
  if (str == "stop")
    return Command::Stop;
  if (str == "return_to_base")
    return Command::ReturnToBase;
  if (str == "clean_spot")
    return Command::CleanSpot;
  if (str == "locate")
    return Command::Locate;
  return Command::Unknown;
}

bool HaEntityVacuum::buildComponentConfiguration(ComponentConfig &out) {
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

  doc["retain"] = _configuration.retain;

  if (!_configuration.supported_features.empty()) {
    JsonArrayType feat_array = createJsonArray(doc, "supported_features");
    for (const auto &f : _configuration.supported_features) {
      addToJsonArray(feat_array, f);
    }
  }

  doc["state_topic"] = _ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, _child_object_id, OBJECT_ID_STATE);
  doc["command_topic"] =
      _ha_bridge.getTopic(HaBridge::TopicType::Command, COMPONENT, _child_object_id, OBJECT_ID_COMMAND);
  doc["battery_level_topic"] =
      _ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, _child_object_id, OBJECT_ID_BATTERY);

  if (!_configuration.fan_speed_list.empty()) {
    JsonArrayType speed_array = createJsonArray(doc, "fan_speed_list");
    for (const auto &s : _configuration.fan_speed_list) {
      addToJsonArray(speed_array, s);
    }
    doc["fan_speed_topic"] =
        _ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, _child_object_id, OBJECT_ID_FAN_SPEED);
    doc["set_fan_speed_topic"] =
        _ha_bridge.getTopic(HaBridge::TopicType::Command, COMPONENT, _child_object_id, OBJECT_ID_FAN_SPEED);
  }

  if (_configuration.with_attributes) {
    doc["json_attributes_topic"] =
        _ha_bridge.getTopic(HaBridge::TopicType::Attributes, COMPONENT, _child_object_id, "");
  }

  return true;
}

void HaEntityVacuum::publishConfiguration() {
  ComponentConfig cfg;
  if (buildComponentConfiguration(cfg)) {
    _ha_bridge.publishConfiguration(cfg.component, cfg.object_id, cfg.child_object_id, cfg.doc);
  }
}

void HaEntityVacuum::unpublishConfiguration() {
  _ha_bridge.unpublishConfiguration(COMPONENT, OBJECT_ID, _child_object_id);
}

void HaEntityVacuum::republishState() {
  if (_state) {
    publish(*_state);
  }
  if (_battery) {
    publishBattery(*_battery);
  }
  if (_fan_speed) {
    publishFanSpeed(*_fan_speed);
  }
  if (_attributes) {
    publishAttributes(*_attributes);
  }
}

void HaEntityVacuum::publish(State state) { publish(stateToString(state)); }

void HaEntityVacuum::publish(const std::string &state) {
  _state = state;
  _ha_bridge.publishMessage(
      _ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, _child_object_id, OBJECT_ID_STATE), state);
}

void HaEntityVacuum::update(State state) { update(stateToString(state)); }

void HaEntityVacuum::update(const std::string &state) {
  if (!_state || *_state != state) {
    publish(state);
  }
}

void HaEntityVacuum::publishBattery(uint8_t battery) {
  _battery = battery;
  _ha_bridge.publishMessage(
      _ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, _child_object_id, OBJECT_ID_BATTERY),
      std::to_string(battery));
}

void HaEntityVacuum::updateBattery(uint8_t battery) {
  if (!_battery || *_battery != battery) {
    publishBattery(battery);
  }
}

void HaEntityVacuum::publishFanSpeed(const std::string &speed) {
  _fan_speed = speed;
  _ha_bridge.publishMessage(
      _ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, _child_object_id, OBJECT_ID_FAN_SPEED), speed);
}

void HaEntityVacuum::updateFanSpeed(const std::string &speed) {
  if (!_fan_speed || *_fan_speed != speed) {
    publishFanSpeed(speed);
  }
}

bool HaEntityVacuum::onCommand(std::function<void(Command command)> callback) {
  if (!callback) {
    return false;
  }
  return _ha_bridge.remote().subscribe(
      _ha_bridge.getTopic(HaBridge::TopicType::Command, COMPONENT, _child_object_id, OBJECT_ID_COMMAND),
      [callback](std::string, std::string message) { callback(stringToCommand(message)); });
}

bool HaEntityVacuum::onFanSpeed(std::function<void(const std::string &speed)> callback) {
  if (_configuration.fan_speed_list.empty() || !callback) {
    return false;
  }
  return _ha_bridge.remote().subscribe(
      _ha_bridge.getTopic(HaBridge::TopicType::Command, COMPONENT, _child_object_id, OBJECT_ID_FAN_SPEED),
      [callback, speeds = _configuration.fan_speed_list](std::string, std::string message) {
        if (std::find(speeds.begin(), speeds.end(), message) != speeds.end()) {
          callback(message);
        }
      });
}

void HaEntityVacuum::publishAttributes(const Attributes::Map &attributes) {
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
