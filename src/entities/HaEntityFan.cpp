// Copyright (c) Johan Böhlin
// SPDX-License-Identifier: MIT

#include "HaEntityFan.h"
#include <HaUtilities.h>
#include <IJson.h>
#include <algorithm>
#include <cstdlib>
#include <utility>

#define COMPONENT "fan"
#define OBJECT_ID "fan"
#define OBJECT_ID_ONOFF "onoff"
#define OBJECT_ID_SPEED "speed"
#define OBJECT_ID_PRESET "preset"
#define OBJECT_ID_DIRECTION "direction"
#define OBJECT_ID_OSCILLATION "oscillation"

HaEntityFan::HaEntityFan(HaBridge &ha_bridge, std::string name, std::string child_object_id,
                         Configuration configuration)
    : _name(homeassistantentities::trim(name)), _ha_bridge(ha_bridge), _child_object_id(std::move(child_object_id)),
      _configuration(std::move(configuration)) {
  if (_configuration.speed_range_min < 1) {
    _configuration.speed_range_min = 1;
  }
  if (_configuration.speed_range_max < _configuration.speed_range_min) {
    _configuration.speed_range_max = _configuration.speed_range_min;
  }
}

void HaEntityFan::publishConfiguration() {
  IJsonDocument doc;

  if (!_name.empty()) {
    doc["name"] = _name;
  } else {
    doc["name"] = nullptr;
  }

  doc["platform"] = "fan";
  doc["retain"] = _configuration.retain;

  if (_configuration.with_direction) {
    doc["direction_state_topic"] =
        _ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, _child_object_id, OBJECT_ID_DIRECTION);
    doc["direction_command_topic"] =
        _ha_bridge.getTopic(HaBridge::TopicType::Command, COMPONENT, _child_object_id, OBJECT_ID_DIRECTION);
  }

  if (_configuration.with_oscillation) {
    doc["oscillation_state_topic"] =
        _ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, _child_object_id, OBJECT_ID_OSCILLATION);
    doc["oscillation_command_topic"] =
        _ha_bridge.getTopic(HaBridge::TopicType::Command, COMPONENT, _child_object_id, OBJECT_ID_OSCILLATION);
  }

  if (_configuration.with_speed) {
    doc["percentage_state_topic"] =
        _ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, _child_object_id, OBJECT_ID_SPEED);
    doc["percentage_command_topic"] =
        _ha_bridge.getTopic(HaBridge::TopicType::Command, COMPONENT, _child_object_id, OBJECT_ID_SPEED);
    doc["speed_range_min"] = _configuration.speed_range_min;
    doc["speed_range_max"] = _configuration.speed_range_max;
  }

  if (!_configuration.presets.empty()) {
    JsonArrayType preset_modes_array = createJsonArray(doc, "preset_modes");
    for (const std::string &preset : _configuration.presets) {
      addToJsonArray(preset_modes_array, preset);
    }
    doc["preset_mode_state_topic"] =
        _ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, _child_object_id, OBJECT_ID_PRESET);
    doc["preset_mode_command_topic"] =
        _ha_bridge.getTopic(HaBridge::TopicType::Command, COMPONENT, _child_object_id, OBJECT_ID_PRESET);
  }

  doc["state_topic"] = _ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, _child_object_id, OBJECT_ID_ONOFF);
  doc["command_topic"] =
      _ha_bridge.getTopic(HaBridge::TopicType::Command, COMPONENT, _child_object_id, OBJECT_ID_ONOFF);

  _ha_bridge.publishConfiguration(COMPONENT, OBJECT_ID, _child_object_id, doc);
}

void HaEntityFan::republishState() {
  if (_on) {
    publishIsOn(*_on);
  }
  if (_speed) {
    publishSpeed(*_speed);
  }
  if (_preset) {
    publishPreset(*_preset);
  }
  if (_direction) {
    publishDirection(*_direction);
  }
  if (_oscillation) {
    publishOscillation(*_oscillation);
  }
}

void HaEntityFan::publishDirection(const std::string &direction) {
  if (!_configuration.with_direction) {
    return;
  }
  _direction = direction;
  _ha_bridge.publishMessage(
      _ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, _child_object_id, OBJECT_ID_DIRECTION), direction);
}

void HaEntityFan::updateDirection(const std::string &direction) {
  if (!_direction || *_direction != direction) {
    publishDirection(direction);
  }
}

bool HaEntityFan::setOnDirection(std::function<void(std::string)> callback) {
  if (!_configuration.with_direction || !callback) {
    return false;
  }
  return _ha_bridge.remote().subscribe(
      _ha_bridge.getTopic(HaBridge::TopicType::Command, COMPONENT, _child_object_id, OBJECT_ID_DIRECTION),
      [callback](std::string, std::string message) { callback(message); });
}

//--------------------------------------

void HaEntityFan::publishOscillation(bool oscillation) {
  if (!_configuration.with_oscillation) {
    return;
  }
  _oscillation = oscillation;
  _ha_bridge.publishMessage(
      _ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, _child_object_id, OBJECT_ID_OSCILLATION),
      oscillation ? "oscillate_on" : "oscillate_off");
}

void HaEntityFan::updateOscillation(bool oscillation) {
  if (!_oscillation || *_oscillation != oscillation) {
    publishOscillation(oscillation);
  }
}

bool HaEntityFan::setOnOscillation(std::function<void(bool)> callback) {
  if (!_configuration.with_oscillation || !callback) {
    return false;
  }
  return _ha_bridge.remote().subscribe(
      _ha_bridge.getTopic(HaBridge::TopicType::Command, COMPONENT, _child_object_id, OBJECT_ID_OSCILLATION),
      [callback](std::string, std::string message) {
        callback(message == "ON" || message == "on" || message == "true" || message == "1" ||
                 message == "oscillate_on");
      });
}

//--------------------------------------

uint32_t HaEntityFan::clampSpeed(uint32_t speed) const {
  // In Home Assistant MQTT Fan, speed_range_min - 1 represents 0% (off).
  uint32_t min_with_off = _configuration.speed_range_min > 0 ? (_configuration.speed_range_min - 1) : 0;
  return std::clamp(speed, min_with_off, _configuration.speed_range_max);
}

void HaEntityFan::publishSpeed(uint32_t speed) {
  if (!_configuration.with_speed) {
    return;
  }
  speed = clampSpeed(speed);
  _speed = speed;
  _ha_bridge.publishMessage(
      _ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, _child_object_id, OBJECT_ID_SPEED),
      std::to_string(speed));
}

void HaEntityFan::updateSpeed(uint32_t speed) {
  if (!_configuration.with_speed) {
    return;
  }
  speed = clampSpeed(speed);
  if (!_speed || *_speed != speed) {
    publishSpeed(speed);
  }
}

bool HaEntityFan::setOnSpeed(std::function<void(uint32_t)> callback) {
  if (!_configuration.with_speed || !callback) {
    return false;
  }
  uint32_t min_with_off = _configuration.speed_range_min > 0 ? (_configuration.speed_range_min - 1) : 0;
  uint32_t max_speed = _configuration.speed_range_max;
  return _ha_bridge.remote().subscribe(
      _ha_bridge.getTopic(HaBridge::TopicType::Command, COMPONENT, _child_object_id, OBJECT_ID_SPEED),
      [callback, min_with_off, max_speed](std::string, std::string message) {
        char *end = nullptr;
        unsigned long parsed = std::strtoul(message.c_str(), &end, 10);
        if (end != message.c_str() && *end == '\0' && message.find('-') == std::string::npos) {
          uint32_t clamped = parsed > max_speed ? max_speed : std::max(static_cast<uint32_t>(parsed), min_with_off);
          callback(clamped);
        }
      });
}

//--------------------------------------

void HaEntityFan::publishPreset(const std::string &preset) {
  const auto &presets = _configuration.presets;
  if (presets.empty()) {
    return;
  }
  if (std::find(presets.begin(), presets.end(), preset) == presets.end()) {
    return;
  }
  _preset = preset;
  _ha_bridge.publishMessage(
      _ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, _child_object_id, OBJECT_ID_PRESET), preset);
}

void HaEntityFan::updatePreset(const std::string &preset) {
  if (!_preset || *_preset != preset) {
    publishPreset(preset);
  }
}

bool HaEntityFan::setOnPreset(std::function<void(std::string)> callback) {
  if (_configuration.presets.empty() || !callback) {
    return false;
  }
  return _ha_bridge.remote().subscribe(
      _ha_bridge.getTopic(HaBridge::TopicType::Command, COMPONENT, _child_object_id, OBJECT_ID_PRESET),
      [callback, presets = _configuration.presets](std::string, std::string message) {
        if (std::find(presets.begin(), presets.end(), message) != presets.end()) {
          callback(message);
        }
      });
}

//--------------------------------------

void HaEntityFan::publishIsOn(bool on) {
  _on = on;
  _ha_bridge.publishMessage(
      _ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, _child_object_id, OBJECT_ID_ONOFF), on ? "ON" : "OFF");
}

void HaEntityFan::updateIsOn(bool on) {
  if (!_on || *_on != on) {
    publishIsOn(on);
  }
}

bool HaEntityFan::setOnState(std::function<void(bool)> callback) {
  if (!callback) {
    return false;
  }
  return _ha_bridge.remote().subscribe(
      _ha_bridge.getTopic(HaBridge::TopicType::Command, COMPONENT, _child_object_id, OBJECT_ID_ONOFF),
      [callback](std::string, std::string message) {
        callback(message == "ON" || message == "on" || message == "true" || message == "1");
      });
}
