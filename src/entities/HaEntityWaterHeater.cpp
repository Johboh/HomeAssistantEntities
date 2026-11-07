// Copyright (c) Johan Böhlin
// SPDX-License-Identifier: MIT

#include "HaEntityWaterHeater.h"
#include <HaUtilities.h>
#include <IJson.h>
#include <algorithm>
#include <cstdlib>
#include <utility>

using namespace homeassistantentities;

#define COMPONENT "water_heater"
#define OBJECT_ID "water_heater"
#define OBJECT_ID_CURRENT_TEMP "current_temperature"
#define OBJECT_ID_TARGET_TEMP "target_temperature"
#define OBJECT_ID_MODE "mode"
#define OBJECT_ID_AWAY "away"

HaEntityWaterHeater::HaEntityWaterHeater(HaBridge &ha_bridge, std::string name,
                                         std::optional<std::string> child_object_id, Configuration configuration)
    : _name(homeassistantentities::trim(name)), _ha_bridge(ha_bridge), _child_object_id(child_object_id.value_or("")),
      _configuration(std::move(configuration)) {}

bool HaEntityWaterHeater::buildComponentConfiguration(ComponentConfig &out) {
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

  doc["retain"] = _configuration.retain;
  doc["min_temp"] = _configuration.min_temp;
  doc["max_temp"] = _configuration.max_temp;
  doc["temp_step"] = _configuration.temp_step;
  doc["temperature_unit"] = _configuration.temperature_unit.empty() ? "C" : _configuration.temperature_unit;

  if (_configuration.with_current_temperature) {
    doc["current_temperature_topic"] =
        _ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, _child_object_id, OBJECT_ID_CURRENT_TEMP);
  }

  doc["temperature_state_topic"] =
      _ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, _child_object_id, OBJECT_ID_TARGET_TEMP);
  doc["temperature_command_topic"] =
      _ha_bridge.getTopic(HaBridge::TopicType::Command, COMPONENT, _child_object_id, OBJECT_ID_TARGET_TEMP);

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

  if (_configuration.with_away_mode) {
    doc["away_mode_state_topic"] =
        _ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, _child_object_id, OBJECT_ID_AWAY);
    doc["away_mode_command_topic"] =
        _ha_bridge.getTopic(HaBridge::TopicType::Command, COMPONENT, _child_object_id, OBJECT_ID_AWAY);
  }

  if (_configuration.with_attributes) {
    doc["json_attributes_topic"] =
        _ha_bridge.getTopic(HaBridge::TopicType::Attributes, COMPONENT, _child_object_id, "");
  }

  return true;
}

void HaEntityWaterHeater::publishConfiguration() {
  ComponentConfig cfg;
  if (buildComponentConfiguration(cfg)) {
    _ha_bridge.publishConfiguration(cfg.component, cfg.object_id, cfg.child_object_id, cfg.doc);
  }
}

void HaEntityWaterHeater::unpublishConfiguration() {
  _ha_bridge.unpublishConfiguration(COMPONENT, OBJECT_ID, _child_object_id);
}

void HaEntityWaterHeater::republishState() {
  if (_target_temp) {
    publishTargetTemperature(*_target_temp);
  }
  if (_current_temp) {
    publishCurrentTemperature(*_current_temp);
  }
  if (_mode) {
    publishMode(*_mode);
  }
  if (_away) {
    publishAwayMode(*_away);
  }
  if (_attributes) {
    publishAttributes(*_attributes);
  }
}

void HaEntityWaterHeater::publishTargetTemperature(double temperature) {
  _target_temp = temperature;
  _ha_bridge.publishMessage(
      _ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, _child_object_id, OBJECT_ID_TARGET_TEMP),
      formatNumber(temperature));
}

void HaEntityWaterHeater::updateTargetTemperature(double temperature) {
  if (!_target_temp || *_target_temp != temperature) {
    publishTargetTemperature(temperature);
  }
}

bool HaEntityWaterHeater::onTargetTemperature(std::function<void(double temperature)> callback) {
  if (!callback) {
    return false;
  }
  return _ha_bridge.remote().subscribe(
      _ha_bridge.getTopic(HaBridge::TopicType::Command, COMPONENT, _child_object_id, OBJECT_ID_TARGET_TEMP),
      [callback](std::string, std::string message) {
        try {
          callback(std::stod(message));
        } catch (...) {
        }
      });
}

void HaEntityWaterHeater::publishCurrentTemperature(double temperature) {
  _current_temp = temperature;
  _ha_bridge.publishMessage(
      _ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, _child_object_id, OBJECT_ID_CURRENT_TEMP),
      formatNumber(temperature));
}

void HaEntityWaterHeater::updateCurrentTemperature(double temperature) {
  if (!_current_temp || *_current_temp != temperature) {
    publishCurrentTemperature(temperature);
  }
}

void HaEntityWaterHeater::publishMode(const std::string &mode) {
  _mode = mode;
  _ha_bridge.publishMessage(
      _ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, _child_object_id, OBJECT_ID_MODE), mode);
}

void HaEntityWaterHeater::updateMode(const std::string &mode) {
  if (!_mode || *_mode != mode) {
    publishMode(mode);
  }
}

bool HaEntityWaterHeater::onMode(std::function<void(const std::string &mode)> callback) {
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

void HaEntityWaterHeater::publishAwayMode(bool away) {
  _away = away;
  _ha_bridge.publishMessage(
      _ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, _child_object_id, OBJECT_ID_AWAY),
      away ? "ON" : "OFF");
}

void HaEntityWaterHeater::updateAwayMode(bool away) {
  if (!_away || *_away != away) {
    publishAwayMode(away);
  }
}

bool HaEntityWaterHeater::onAwayMode(std::function<void(bool away)> callback) {
  if (!_configuration.with_away_mode || !callback) {
    return false;
  }
  return _ha_bridge.remote().subscribe(
      _ha_bridge.getTopic(HaBridge::TopicType::Command, COMPONENT, _child_object_id, OBJECT_ID_AWAY),
      [callback](std::string, std::string message) {
        callback(message == "ON" || message == "on" || message == "true" || message == "1");
      });
}

void HaEntityWaterHeater::publishAttributes(const Attributes::Map &attributes) {
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
