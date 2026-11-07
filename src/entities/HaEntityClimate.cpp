// Copyright (c) Johan Böhlin
// SPDX-License-Identifier: MIT

#include "HaEntityClimate.h"
#include <HaUtilities.h>
#include <IJson.h>
#include <algorithm>
#include <cstdlib>
#include <utility>

using namespace homeassistantentities;

#define COMPONENT "climate"
#define OBJECT_ID "climate"

#define SUB_MODE "mode"
#define SUB_TARGET_TEMP "target_temperature"
#define SUB_TARGET_TEMP_LOW "target_temp_low"
#define SUB_TARGET_TEMP_HIGH "target_temp_high"
#define SUB_CURRENT_TEMP "current_temperature"
#define SUB_ACTION "action"
#define SUB_FAN_MODE "fan_mode"
#define SUB_SWING_MODE "swing_mode"
#define SUB_PRESET_MODE "preset_mode"
#define SUB_CURRENT_HUMIDITY "current_humidity"
#define SUB_TARGET_HUMIDITY "target_humidity"

HaEntityClimate::HaEntityClimate(HaBridge &ha_bridge, std::string name, std::optional<std::string> child_object_id,
                                 Configuration configuration)
    : _name(homeassistantentities::trim(name)), _ha_bridge(ha_bridge), _child_object_id(child_object_id.value_or("")),
      _configuration(std::move(configuration)) {}

std::string HaEntityClimate::modeToString(Mode mode) {
  switch (mode) {
  case Mode::Auto:
    return "auto";
  case Mode::Off:
    return "off";
  case Mode::Cool:
    return "cool";
  case Mode::Heat:
    return "heat";
  case Mode::Dry:
    return "dry";
  case Mode::FanOnly:
    return "fan_only";
  case Mode::Custom:
    return "";
  }
  return "";
}

std::optional<HaEntityClimate::Mode> HaEntityClimate::stringToMode(const std::string &str) {
  if (str == "auto") {
    return Mode::Auto;
  }
  if (str == "off") {
    return Mode::Off;
  }
  if (str == "cool") {
    return Mode::Cool;
  }
  if (str == "heat") {
    return Mode::Heat;
  }
  if (str == "dry") {
    return Mode::Dry;
  }
  if (str == "fan_only") {
    return Mode::FanOnly;
  }
  return std::nullopt;
}

std::string HaEntityClimate::actionToString(Action action) {
  switch (action) {
  case Action::Off:
    return "off";
  case Action::Heating:
    return "heating";
  case Action::Cooling:
    return "cooling";
  case Action::Drying:
    return "drying";
  case Action::Idle:
    return "idle";
  case Action::Fan:
    return "fan";
  case Action::Unknown:
    return "";
  }
  return "";
}

std::optional<HaEntityClimate::Action> HaEntityClimate::stringToAction(const std::string &str) {
  if (str == "off") {
    return Action::Off;
  }
  if (str == "heating") {
    return Action::Heating;
  }
  if (str == "cooling") {
    return Action::Cooling;
  }
  if (str == "drying") {
    return Action::Drying;
  }
  if (str == "idle") {
    return Action::Idle;
  }
  if (str == "fan") {
    return Action::Fan;
  }
  return std::nullopt;
}

bool HaEntityClimate::buildComponentConfiguration(ComponentConfig &out) {
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

  if (_configuration.precision) {
    doc["precision"] = *_configuration.precision;
  }

  if (!_configuration.modes.empty()) {
    JsonArrayType modes_array = createJsonArray(doc, "modes");
    for (const std::string &mode : _configuration.modes) {
      addToJsonArray(modes_array, mode);
    }
    doc["mode_state_topic"] = _ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, _child_object_id, SUB_MODE);
    doc["mode_command_topic"] =
        _ha_bridge.getTopic(HaBridge::TopicType::Command, COMPONENT, _child_object_id, SUB_MODE);
  }

  if (_configuration.with_current_temperature) {
    doc["current_temperature_topic"] =
        _ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, _child_object_id, SUB_CURRENT_TEMP);
  }

  if (_configuration.with_target_temperature) {
    doc["temperature_state_topic"] =
        _ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, _child_object_id, SUB_TARGET_TEMP);
    doc["temperature_command_topic"] =
        _ha_bridge.getTopic(HaBridge::TopicType::Command, COMPONENT, _child_object_id, SUB_TARGET_TEMP);
  }

  if (_configuration.with_target_temperature_range) {
    doc["target_temperature_low_state_topic"] =
        _ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, _child_object_id, SUB_TARGET_TEMP_LOW);
    doc["target_temperature_low_command_topic"] =
        _ha_bridge.getTopic(HaBridge::TopicType::Command, COMPONENT, _child_object_id, SUB_TARGET_TEMP_LOW);
    doc["target_temperature_high_state_topic"] =
        _ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, _child_object_id, SUB_TARGET_TEMP_HIGH);
    doc["target_temperature_high_command_topic"] =
        _ha_bridge.getTopic(HaBridge::TopicType::Command, COMPONENT, _child_object_id, SUB_TARGET_TEMP_HIGH);
  }

  if (_configuration.with_action) {
    doc["action_topic"] = _ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, _child_object_id, SUB_ACTION);
  }

  if (!_configuration.fan_modes.empty()) {
    JsonArrayType fan_modes_array = createJsonArray(doc, "fan_modes");
    for (const std::string &fm : _configuration.fan_modes) {
      addToJsonArray(fan_modes_array, fm);
    }
    doc["fan_mode_state_topic"] =
        _ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, _child_object_id, SUB_FAN_MODE);
    doc["fan_mode_command_topic"] =
        _ha_bridge.getTopic(HaBridge::TopicType::Command, COMPONENT, _child_object_id, SUB_FAN_MODE);
  }

  if (!_configuration.swing_modes.empty()) {
    JsonArrayType swing_modes_array = createJsonArray(doc, "swing_modes");
    for (const std::string &sm : _configuration.swing_modes) {
      addToJsonArray(swing_modes_array, sm);
    }
    doc["swing_mode_state_topic"] =
        _ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, _child_object_id, SUB_SWING_MODE);
    doc["swing_mode_command_topic"] =
        _ha_bridge.getTopic(HaBridge::TopicType::Command, COMPONENT, _child_object_id, SUB_SWING_MODE);
  }

  if (!_configuration.preset_modes.empty()) {
    JsonArrayType preset_modes_array = createJsonArray(doc, "preset_modes");
    for (const std::string &pm : _configuration.preset_modes) {
      addToJsonArray(preset_modes_array, pm);
    }
    doc["preset_mode_state_topic"] =
        _ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, _child_object_id, SUB_PRESET_MODE);
    doc["preset_mode_command_topic"] =
        _ha_bridge.getTopic(HaBridge::TopicType::Command, COMPONENT, _child_object_id, SUB_PRESET_MODE);
  }

  if (_configuration.with_current_humidity) {
    doc["current_humidity_topic"] =
        _ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, _child_object_id, SUB_CURRENT_HUMIDITY);
  }

  if (_configuration.with_target_humidity) {
    doc["target_humidity_state_topic"] =
        _ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, _child_object_id, SUB_TARGET_HUMIDITY);
    doc["target_humidity_command_topic"] =
        _ha_bridge.getTopic(HaBridge::TopicType::Command, COMPONENT, _child_object_id, SUB_TARGET_HUMIDITY);
    doc["min_humidity"] = _configuration.min_humidity;
    doc["max_humidity"] = _configuration.max_humidity;
  }

  if (_configuration.with_attributes) {
    doc["json_attributes_topic"] =
        _ha_bridge.getTopic(HaBridge::TopicType::Attributes, COMPONENT, _child_object_id, "");
  }

  return true;
}

void HaEntityClimate::publishConfiguration() {
  ComponentConfig cfg;
  if (buildComponentConfiguration(cfg)) {
    _ha_bridge.publishConfiguration(cfg.component, cfg.object_id, cfg.child_object_id, cfg.doc);
  }
}

void HaEntityClimate::unpublishConfiguration() {
  _ha_bridge.unpublishConfiguration(COMPONENT, OBJECT_ID, _child_object_id);
}

void HaEntityClimate::republishState() {
  if (_mode) {
    publishMode(*_mode);
  }
  if (_current_temp) {
    publishCurrentTemperature(*_current_temp);
  }
  if (_target_temp) {
    publishTargetTemperature(*_target_temp);
  }
  if (_target_temp_low && _target_temp_high) {
    publishTargetTemperatureRange(*_target_temp_low, *_target_temp_high);
  }
  if (_action) {
    publishAction(*_action);
  }
  if (_fan_mode) {
    publishFanMode(*_fan_mode);
  }
  if (_swing_mode) {
    publishSwingMode(*_swing_mode);
  }
  if (_preset_mode) {
    publishPresetMode(*_preset_mode);
  }
  if (_current_humidity) {
    publishCurrentHumidity(*_current_humidity);
  }
  if (_target_humidity) {
    publishTargetHumidity(*_target_humidity);
  }
  if (_attributes) {
    publishAttributes(*_attributes);
  }
}

//--------------------------------------

void HaEntityClimate::publishMode(const std::string &mode) {
  _mode = mode;
  _ha_bridge.publishMessage(_ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, _child_object_id, SUB_MODE),
                            mode);
}

void HaEntityClimate::publishMode(Mode mode) { publishMode(modeToString(mode)); }

void HaEntityClimate::updateMode(const std::string &mode) {
  if (!_mode || *_mode != mode) {
    publishMode(mode);
  }
}

void HaEntityClimate::updateMode(Mode mode) { updateMode(modeToString(mode)); }

bool HaEntityClimate::onMode(std::function<void(const std::string &)> callback) {
  if (_configuration.modes.empty() || !callback) {
    return false;
  }
  return _ha_bridge.remote().subscribe(
      _ha_bridge.getTopic(HaBridge::TopicType::Command, COMPONENT, _child_object_id, SUB_MODE),
      [callback, modes = _configuration.modes](std::string, std::string message) {
        if (std::find(modes.begin(), modes.end(), message) != modes.end()) {
          callback(message);
        }
      });
}

bool HaEntityClimate::onMode(std::function<void(Mode)> callback) {
  if (!callback) {
    return false;
  }
  return onMode([callback](const std::string &mode_str) {
    auto mode = stringToMode(mode_str);
    if (mode) {
      callback(*mode);
    }
  });
}

//--------------------------------------

void HaEntityClimate::publishCurrentTemperature(double temperature) {
  _current_temp = temperature;
  _ha_bridge.publishMessage(
      _ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, _child_object_id, SUB_CURRENT_TEMP),
      formatNumber(temperature));
}

void HaEntityClimate::updateCurrentTemperature(double temperature) {
  if (!_current_temp || *_current_temp != temperature) {
    publishCurrentTemperature(temperature);
  }
}

//--------------------------------------

void HaEntityClimate::publishTargetTemperature(double temperature) {
  _target_temp = temperature;
  _ha_bridge.publishMessage(
      _ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, _child_object_id, SUB_TARGET_TEMP),
      formatNumber(temperature));
}

void HaEntityClimate::updateTargetTemperature(double temperature) {
  if (!_target_temp || *_target_temp != temperature) {
    publishTargetTemperature(temperature);
  }
}

bool HaEntityClimate::onTargetTemperature(std::function<void(double)> callback) {
  if (!_configuration.with_target_temperature || !callback) {
    return false;
  }
  return _ha_bridge.remote().subscribe(
      _ha_bridge.getTopic(HaBridge::TopicType::Command, COMPONENT, _child_object_id, SUB_TARGET_TEMP),
      [callback](std::string, std::string message) {
        char *end = nullptr;
        double temp = std::strtod(message.c_str(), &end);
        if (end != message.c_str() && *end == '\0') {
          callback(temp);
        }
      });
}

//--------------------------------------

void HaEntityClimate::publishTargetTemperatureRange(double low, double high) {
  _target_temp_low = low;
  _target_temp_high = high;
  _ha_bridge.publishMessage(
      _ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, _child_object_id, SUB_TARGET_TEMP_LOW),
      formatNumber(low));
  _ha_bridge.publishMessage(
      _ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, _child_object_id, SUB_TARGET_TEMP_HIGH),
      formatNumber(high));
}

void HaEntityClimate::updateTargetTemperatureRange(double low, double high) {
  if (!_target_temp_low || !_target_temp_high || *_target_temp_low != low || *_target_temp_high != high) {
    publishTargetTemperatureRange(low, high);
  }
}

bool HaEntityClimate::onTargetTemperatureRange(std::function<void(double low, double high)> callback) {
  if (!_configuration.with_target_temperature_range || !callback) {
    return false;
  }
  _range_callback = std::move(callback);

  bool ok1 = _ha_bridge.remote().subscribe(
      _ha_bridge.getTopic(HaBridge::TopicType::Command, COMPONENT, _child_object_id, SUB_TARGET_TEMP_LOW),
      [this](std::string, std::string message) {
        char *end = nullptr;
        double val = std::strtod(message.c_str(), &end);
        if (end != message.c_str() && *end == '\0') {
          _target_temp_low = val;
          if (_range_callback && _target_temp_high) {
            _range_callback(*_target_temp_low, *_target_temp_high);
          }
        }
      });

  bool ok2 = _ha_bridge.remote().subscribe(
      _ha_bridge.getTopic(HaBridge::TopicType::Command, COMPONENT, _child_object_id, SUB_TARGET_TEMP_HIGH),
      [this](std::string, std::string message) {
        char *end = nullptr;
        double val = std::strtod(message.c_str(), &end);
        if (end != message.c_str() && *end == '\0') {
          _target_temp_high = val;
          if (_range_callback && _target_temp_low) {
            _range_callback(*_target_temp_low, *_target_temp_high);
          }
        }
      });

  return ok1 && ok2;
}

//--------------------------------------

void HaEntityClimate::publishAction(const std::string &action) {
  _action = action;
  _ha_bridge.publishMessage(_ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, _child_object_id, SUB_ACTION),
                            action);
}

void HaEntityClimate::publishAction(Action action) { publishAction(actionToString(action)); }

void HaEntityClimate::updateAction(const std::string &action) {
  if (!_action || *_action != action) {
    publishAction(action);
  }
}

void HaEntityClimate::updateAction(Action action) { updateAction(actionToString(action)); }

//--------------------------------------

void HaEntityClimate::publishFanMode(const std::string &mode) {
  _fan_mode = mode;
  _ha_bridge.publishMessage(_ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, _child_object_id, SUB_FAN_MODE),
                            mode);
}

void HaEntityClimate::updateFanMode(const std::string &mode) {
  if (!_fan_mode || *_fan_mode != mode) {
    publishFanMode(mode);
  }
}

bool HaEntityClimate::onFanMode(std::function<void(const std::string &)> callback) {
  if (_configuration.fan_modes.empty() || !callback) {
    return false;
  }
  return _ha_bridge.remote().subscribe(
      _ha_bridge.getTopic(HaBridge::TopicType::Command, COMPONENT, _child_object_id, SUB_FAN_MODE),
      [callback, fan_modes = _configuration.fan_modes](std::string, std::string message) {
        if (std::find(fan_modes.begin(), fan_modes.end(), message) != fan_modes.end()) {
          callback(message);
        }
      });
}

//--------------------------------------

void HaEntityClimate::publishSwingMode(const std::string &mode) {
  _swing_mode = mode;
  _ha_bridge.publishMessage(
      _ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, _child_object_id, SUB_SWING_MODE), mode);
}

void HaEntityClimate::updateSwingMode(const std::string &mode) {
  if (!_swing_mode || *_swing_mode != mode) {
    publishSwingMode(mode);
  }
}

bool HaEntityClimate::onSwingMode(std::function<void(const std::string &)> callback) {
  if (_configuration.swing_modes.empty() || !callback) {
    return false;
  }
  return _ha_bridge.remote().subscribe(
      _ha_bridge.getTopic(HaBridge::TopicType::Command, COMPONENT, _child_object_id, SUB_SWING_MODE),
      [callback, swing_modes = _configuration.swing_modes](std::string, std::string message) {
        if (std::find(swing_modes.begin(), swing_modes.end(), message) != swing_modes.end()) {
          callback(message);
        }
      });
}

//--------------------------------------

void HaEntityClimate::publishPresetMode(const std::string &mode) {
  _preset_mode = mode;
  _ha_bridge.publishMessage(
      _ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, _child_object_id, SUB_PRESET_MODE), mode);
}

void HaEntityClimate::updatePresetMode(const std::string &mode) {
  if (!_preset_mode || *_preset_mode != mode) {
    publishPresetMode(mode);
  }
}

bool HaEntityClimate::onPresetMode(std::function<void(const std::string &)> callback) {
  if (_configuration.preset_modes.empty() || !callback) {
    return false;
  }
  return _ha_bridge.remote().subscribe(
      _ha_bridge.getTopic(HaBridge::TopicType::Command, COMPONENT, _child_object_id, SUB_PRESET_MODE),
      [callback, preset_modes = _configuration.preset_modes](std::string, std::string message) {
        if (std::find(preset_modes.begin(), preset_modes.end(), message) != preset_modes.end()) {
          callback(message);
        }
      });
}

//--------------------------------------

void HaEntityClimate::publishCurrentHumidity(uint8_t humidity) {
  _current_humidity = humidity;
  _ha_bridge.publishMessage(
      _ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, _child_object_id, SUB_CURRENT_HUMIDITY),
      std::to_string(humidity));
}

void HaEntityClimate::updateCurrentHumidity(uint8_t humidity) {
  if (!_current_humidity || *_current_humidity != humidity) {
    publishCurrentHumidity(humidity);
  }
}

void HaEntityClimate::publishTargetHumidity(uint8_t humidity) {
  _target_humidity = humidity;
  _ha_bridge.publishMessage(
      _ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, _child_object_id, SUB_TARGET_HUMIDITY),
      std::to_string(humidity));
}

void HaEntityClimate::updateTargetHumidity(uint8_t humidity) {
  if (!_target_humidity || *_target_humidity != humidity) {
    publishTargetHumidity(humidity);
  }
}

bool HaEntityClimate::onTargetHumidity(std::function<void(uint8_t)> callback) {
  if (!_configuration.with_target_humidity || !callback) {
    return false;
  }
  return _ha_bridge.remote().subscribe(
      _ha_bridge.getTopic(HaBridge::TopicType::Command, COMPONENT, _child_object_id, SUB_TARGET_HUMIDITY),
      [callback](std::string, std::string message) {
        char *end = nullptr;
        long val = std::strtol(message.c_str(), &end, 10);
        if (end != message.c_str() && *end == '\0' && val >= 0 && val <= 100) {
          callback(static_cast<uint8_t>(val));
        }
      });
}

//--------------------------------------

void HaEntityClimate::publishAttributes(const Attributes::Map &attributes) {
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
