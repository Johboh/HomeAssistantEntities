// Copyright (c) Johan Böhlin
// SPDX-License-Identifier: MIT

#include "HaEntityAlarmControlPanel.h"
#include <HaUtilities.h>
#include <IJson.h>
#include <algorithm>
#include <utility>

using namespace homeassistantentities;

#define COMPONENT "alarm_control_panel"
#define OBJECT_ID "alarm_control_panel"
#define OBJECT_ID_STATE "state"
#define OBJECT_ID_COMMAND "command"

HaEntityAlarmControlPanel::HaEntityAlarmControlPanel(HaBridge &ha_bridge, std::string name,
                                                     std::optional<std::string> child_object_id,
                                                     Configuration configuration)
    : _name(homeassistantentities::trim(name)), _ha_bridge(ha_bridge), _child_object_id(child_object_id.value_or("")),
      _configuration(std::move(configuration)) {}

std::string HaEntityAlarmControlPanel::stateToString(State state) {
  switch (state) {
  case State::Disarmed:
    return "disarmed";
  case State::ArmedHome:
    return "armed_home";
  case State::ArmedAway:
    return "armed_away";
  case State::ArmedNight:
    return "armed_night";
  case State::ArmedVacation:
    return "armed_vacation";
  case State::ArmedCustomBypass:
    return "armed_custom_bypass";
  case State::Pending:
    return "pending";
  case State::Arming:
    return "arming";
  case State::Disarming:
    return "disarming";
  case State::Triggered:
    return "triggered";
  }
  return "disarmed";
}

std::optional<HaEntityAlarmControlPanel::State> HaEntityAlarmControlPanel::stringToState(const std::string &str) {
  if (str == "disarmed")
    return State::Disarmed;
  if (str == "armed_home")
    return State::ArmedHome;
  if (str == "armed_away")
    return State::ArmedAway;
  if (str == "armed_night")
    return State::ArmedNight;
  if (str == "armed_vacation")
    return State::ArmedVacation;
  if (str == "armed_custom_bypass")
    return State::ArmedCustomBypass;
  if (str == "pending")
    return State::Pending;
  if (str == "arming")
    return State::Arming;
  if (str == "disarming")
    return State::Disarming;
  if (str == "triggered")
    return State::Triggered;
  return std::nullopt;
}

std::string HaEntityAlarmControlPanel::commandToString(Command command) {
  switch (command) {
  case Command::ArmHome:
    return "ARM_HOME";
  case Command::ArmAway:
    return "ARM_AWAY";
  case Command::ArmNight:
    return "ARM_NIGHT";
  case Command::ArmVacation:
    return "ARM_VACATION";
  case Command::ArmCustomBypass:
    return "ARM_CUSTOM_BYPASS";
  case Command::Disarm:
    return "DISARM";
  case Command::Unknown:
    return "";
  }
  return "";
}

HaEntityAlarmControlPanel::Command HaEntityAlarmControlPanel::stringToCommand(const std::string &str) {
  if (str == "ARM_HOME")
    return Command::ArmHome;
  if (str == "ARM_AWAY")
    return Command::ArmAway;
  if (str == "ARM_NIGHT")
    return Command::ArmNight;
  if (str == "ARM_VACATION")
    return Command::ArmVacation;
  if (str == "ARM_CUSTOM_BYPASS")
    return Command::ArmCustomBypass;
  if (str == "DISARM")
    return Command::Disarm;
  return Command::Unknown;
}

bool HaEntityAlarmControlPanel::buildComponentConfiguration(ComponentConfig &out) {
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

  if (!_configuration.code.empty()) {
    doc["code"] = _configuration.code;
  }
  if (_configuration.code_arm_required) {
    doc["code_arm_required"] = true;
  }
  if (_configuration.code_disarm_required) {
    doc["code_disarm_required"] = true;
  }
  if (!_configuration.code_format.empty()) {
    doc["code_format"] = _configuration.code_format;
  }

  if (!_configuration.supported_features.empty()) {
    JsonArrayType features_array = createJsonArray(doc, "supported_features");
    for (const auto &feat : _configuration.supported_features) {
      addToJsonArray(features_array, feat);
    }
  }

  doc["state_topic"] = _ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, _child_object_id, OBJECT_ID_STATE);
  doc["command_topic"] =
      _ha_bridge.getTopic(HaBridge::TopicType::Command, COMPONENT, _child_object_id, OBJECT_ID_COMMAND);

  if (_configuration.with_attributes) {
    doc["json_attributes_topic"] =
        _ha_bridge.getTopic(HaBridge::TopicType::Attributes, COMPONENT, _child_object_id, "");
  }

  return true;
}

void HaEntityAlarmControlPanel::publishConfiguration() {
  ComponentConfig cfg;
  if (buildComponentConfiguration(cfg)) {
    _ha_bridge.publishConfiguration(cfg.component, cfg.object_id, cfg.child_object_id, cfg.doc);
  }
}

void HaEntityAlarmControlPanel::unpublishConfiguration() {
  _ha_bridge.unpublishConfiguration(COMPONENT, OBJECT_ID, _child_object_id);
}

void HaEntityAlarmControlPanel::republishState() {
  if (_state) {
    publish(*_state);
  }
  if (_attributes) {
    publishAttributes(*_attributes);
  }
}

void HaEntityAlarmControlPanel::publish(State state) { publish(stateToString(state)); }

void HaEntityAlarmControlPanel::publish(const std::string &state) {
  _state = state;
  _ha_bridge.publishMessage(
      _ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, _child_object_id, OBJECT_ID_STATE), state);
}

void HaEntityAlarmControlPanel::update(State state) { update(stateToString(state)); }

void HaEntityAlarmControlPanel::update(const std::string &state) {
  if (!_state || *_state != state) {
    publish(state);
  }
}

bool HaEntityAlarmControlPanel::onCommand(std::function<void(Command command, const std::string &code)> callback) {
  if (!callback) {
    return false;
  }
  return _ha_bridge.remote().subscribe(
      _ha_bridge.getTopic(HaBridge::TopicType::Command, COMPONENT, _child_object_id, OBJECT_ID_COMMAND),
      [callback](std::string, std::string message) {
        // Payload could be raw command string (e.g. "DISARM") or JSON formatted object `{"action": "...", "code":
        // "..."}`
        std::string cmd_str = message;
        std::string code_str = "";
        if (!message.empty() && message.front() == '{') {
// Parse JSON if needed
#if __has_include("nlohmann/json.hpp")
          try {
            auto j = nlohmann::json::parse(message);
            if (j.contains("action") && j["action"].is_string())
              cmd_str = j["action"].get<std::string>();
            if (j.contains("code") && j["code"].is_string())
              code_str = j["code"].get<std::string>();
          } catch (...) {
          }
#elif __has_include(<ArduinoJson.h>)
      JsonDocument jdoc;
      if (deserializeJson(jdoc, message) == DeserializationError::Ok) {
        if (jdoc["action"].is<const char *>())
          cmd_str = jdoc["action"].as<std::string>();
        if (jdoc["code"].is<const char *>())
          code_str = jdoc["code"].as<std::string>();
      }
#endif
        }
        callback(stringToCommand(cmd_str), code_str);
      });
}

bool HaEntityAlarmControlPanel::onCommand(std::function<void(Command command)> callback) {
  if (!callback) {
    return false;
  }
  return onCommand([callback](Command cmd, const std::string &) { callback(cmd); });
}

void HaEntityAlarmControlPanel::publishAttributes(const Attributes::Map &attributes) {
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
