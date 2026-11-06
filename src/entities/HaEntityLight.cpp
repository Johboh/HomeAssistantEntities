// Copyright (c) Johan Böhlin
// SPDX-License-Identifier: MIT

#include "HaEntityLight.h"
#include <HaUtilities.h>
#include <IJson.h>
#include <algorithm>
#include <cstdlib>
#include <string>
#include <utility>

#define COMPONENT "light"
#define OBJECT_ID "light"
#define OBJECT_ID_RGB "rgb"
#define OBJECT_ID_ONOFF "onoff"
#define OBJECT_ID_EFFECT "effect"
#define OBJECT_ID_BRIGHTNESS "brightness"
#define OBJECT_ID_COLOR_TEMPERATURE "color_temperature"

namespace {
bool parseRgb(const std::string &input, HaEntityLight::RGB &color) {
  const char *p = input.c_str();
  char *end = nullptr;

  long r = std::strtol(p, &end, 10);
  if (end == p || *end != ',' || r < 0 || r > 255) {
    return false;
  }

  p = end + 1;
  long g = std::strtol(p, &end, 10);
  if (end == p || *end != ',' || g < 0 || g > 255) {
    return false;
  }

  p = end + 1;
  long b = std::strtol(p, &end, 10);
  if (end == p || *end != '\0' || b < 0 || b > 255) {
    return false;
  }

  color = {static_cast<uint8_t>(r), static_cast<uint8_t>(g), static_cast<uint8_t>(b)};
  return true;
}
} // namespace

// NOTE! We have swapped object ID and child object ID to get a nicer state/command topic path.

HaEntityLight::HaEntityLight(HaBridge &ha_bridge, std::string name, std::string child_object_id,
                             Configuration configuration)
    : _name(homeassistantentities::trim(name)), _ha_bridge(ha_bridge), _child_object_id(std::move(child_object_id)),
      _configuration(std::move(configuration)) {
  if (_configuration.min_color_temperature && *_configuration.min_color_temperature == 0) {
    _configuration.min_color_temperature = std::nullopt;
  }
  if (_configuration.max_color_temperature && *_configuration.max_color_temperature == 0) {
    _configuration.max_color_temperature = std::nullopt;
  }
  if (_configuration.min_color_temperature && _configuration.max_color_temperature &&
      *_configuration.min_color_temperature > *_configuration.max_color_temperature) {
    std::swap(_configuration.min_color_temperature, _configuration.max_color_temperature);
  }
}

bool HaEntityLight::buildComponentConfiguration(ComponentConfig &out) {
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

  doc["state_topic"] = _ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, _child_object_id, OBJECT_ID_ONOFF);
  doc["command_topic"] =
      _ha_bridge.getTopic(HaBridge::TopicType::Command, COMPONENT, _child_object_id, OBJECT_ID_ONOFF);
  if (_configuration.with_brightness) {
    doc["brightness_state_topic"] =
        _ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, _child_object_id, OBJECT_ID_BRIGHTNESS);
    doc["brightness_command_topic"] =
        _ha_bridge.getTopic(HaBridge::TopicType::Command, COMPONENT, _child_object_id, OBJECT_ID_BRIGHTNESS);
  }
  if (_configuration.with_color_temperature != Configuration::ColorTemperature::None) {
    doc["color_temp_state_topic"] =
        _ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, _child_object_id, OBJECT_ID_COLOR_TEMPERATURE);
    doc["color_temp_command_topic"] =
        _ha_bridge.getTopic(HaBridge::TopicType::Command, COMPONENT, _child_object_id, OBJECT_ID_COLOR_TEMPERATURE);
    if (_configuration.with_color_temperature == Configuration::ColorTemperature::Kelvin) {
      doc["color_temp_kelvin"] = true;
      if (_configuration.min_color_temperature) {
        doc["min_kelvin"] = *_configuration.min_color_temperature;
      }
      if (_configuration.max_color_temperature) {
        doc["max_kelvin"] = *_configuration.max_color_temperature;
      }
    } else {
      if (_configuration.min_color_temperature) {
        doc["min_mireds"] = *_configuration.min_color_temperature;
      }
      if (_configuration.max_color_temperature) {
        doc["max_mireds"] = *_configuration.max_color_temperature;
      }
    }
  }
  if (_configuration.with_rgb_color) {
    doc["rgb_state_topic"] =
        _ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, _child_object_id, OBJECT_ID_RGB);
    doc["rgb_command_topic"] =
        _ha_bridge.getTopic(HaBridge::TopicType::Command, COMPONENT, _child_object_id, OBJECT_ID_RGB);
  }
  if (!_configuration.effects.empty()) {
    doc["effect_state_topic"] =
        _ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, _child_object_id, OBJECT_ID_EFFECT);
    doc["effect_command_topic"] =
        _ha_bridge.getTopic(HaBridge::TopicType::Command, COMPONENT, _child_object_id, OBJECT_ID_EFFECT);

    JsonArrayType effect_list_array = createJsonArray(doc, "effect_list");
    for (const std::string &effect : _configuration.effects) {
      addToJsonArray(effect_list_array, effect);
    }
  }

  return true;
}

void HaEntityLight::publishConfiguration() {
  ComponentConfig cfg;
  if (buildComponentConfiguration(cfg)) {
    _ha_bridge.publishConfiguration(cfg.component, cfg.object_id, cfg.child_object_id, cfg.doc);
  }
}

void HaEntityLight::unpublishConfiguration() {
  _ha_bridge.unpublishConfiguration(COMPONENT, OBJECT_ID, _child_object_id);
}

void HaEntityLight::republishState() {
  if (_on) {
    publishIsOn(*_on);
  }
  if (_brightness) {
    publishBrightness(*_brightness);
  }
  if (_color_temperature) {
    publishColorTemperature(*_color_temperature);
  }
  if (_rgb) {
    publishRgb(*_rgb);
  }
  if (_effect) {
    publishEffect(*_effect);
  }
}

void HaEntityLight::publishIsOn(bool on) {
  _ha_bridge.publishMessage(
      _ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, _child_object_id, OBJECT_ID_ONOFF), on ? "ON" : "OFF");
  _on = on;
}

void HaEntityLight::publishBrightness(uint8_t brightness) {
  if (_configuration.with_brightness) {
    _ha_bridge.publishMessage(
        _ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, _child_object_id, OBJECT_ID_BRIGHTNESS),
        std::to_string(brightness));
    _brightness = brightness;
  }
}

uint16_t HaEntityLight::clampColorTemperature(uint16_t temperature) const {
  uint16_t lo = _configuration.min_color_temperature.value_or(1);
  uint16_t hi = _configuration.max_color_temperature.value_or(65535);
  return std::clamp(temperature, lo, hi);
}

void HaEntityLight::publishColorTemperature(uint16_t temperature) {
  if (_configuration.with_color_temperature != Configuration::ColorTemperature::None && temperature > 0) {
    uint16_t clamped = clampColorTemperature(temperature);
    _ha_bridge.publishMessage(
        _ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, _child_object_id, OBJECT_ID_COLOR_TEMPERATURE),
        std::to_string(clamped));
    _color_temperature = clamped;
  }
}

void HaEntityLight::publishRgb(RGB rgb) {
  if (_configuration.with_rgb_color) {
    _ha_bridge.publishMessage(
        _ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, _child_object_id, OBJECT_ID_RGB),
        std::to_string(rgb.r) + "," + std::to_string(rgb.g) + "," + std::to_string(rgb.b));
    _rgb = rgb;
  }
}

void HaEntityLight::publishEffect(const std::string &effect) {
  const auto &effects = _configuration.effects;
  if (effects.empty() || std::find(effects.begin(), effects.end(), effect) == effects.end()) {
    return;
  }
  _ha_bridge.publishMessage(
      _ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, _child_object_id, OBJECT_ID_EFFECT), effect);
  _effect = effect;
}

void HaEntityLight::updateIsOn(bool on) {
  if (!_on || *_on != on) {
    publishIsOn(on);
  }
}

void HaEntityLight::updateBrightness(uint8_t brightness) {
  if (!_brightness || *_brightness != brightness) {
    publishBrightness(brightness);
  }
}

void HaEntityLight::updateColorTemperature(uint16_t temperature) {
  if (_configuration.with_color_temperature == Configuration::ColorTemperature::None || temperature == 0) {
    return;
  }
  uint16_t clamped = clampColorTemperature(temperature);
  if (!_color_temperature || *_color_temperature != clamped) {
    publishColorTemperature(clamped);
  }
}

void HaEntityLight::updateRgb(RGB rgb) {
  if (!_rgb || *_rgb != rgb) {
    publishRgb(rgb);
  }
}

void HaEntityLight::updateEffect(const std::string &effect) {
  if (!_effect || *_effect != effect) {
    publishEffect(effect);
  }
}

bool HaEntityLight::setOnOn(std::function<void(bool)> state_callback) {
  if (!state_callback) {
    return false;
  }
  return _ha_bridge.remote().subscribe(
      _ha_bridge.getTopic(HaBridge::TopicType::Command, COMPONENT, _child_object_id, OBJECT_ID_ONOFF),
      [state_callback](std::string, std::string message) { state_callback(message == "ON"); });
}

bool HaEntityLight::setOnBrightness(std::function<void(uint8_t)> callback) {
  if (!_configuration.with_brightness || !callback) {
    return false;
  }

  return _ha_bridge.remote().subscribe(
      _ha_bridge.getTopic(HaBridge::TopicType::Command, COMPONENT, _child_object_id, OBJECT_ID_BRIGHTNESS),
      [callback](std::string, std::string message) {
        char *end = nullptr;
        long val = std::strtol(message.c_str(), &end, 10);
        if (end != message.c_str() && *end == '\0' && val >= 0 && val <= 255) {
          callback(static_cast<uint8_t>(val));
        }
      });
}

bool HaEntityLight::setOnColorTemperature(std::function<void(uint16_t)> callback) {
  if (_configuration.with_color_temperature == Configuration::ColorTemperature::None || !callback) {
    return false;
  }

  uint16_t lo = _configuration.min_color_temperature.value_or(1);
  uint16_t hi = _configuration.max_color_temperature.value_or(65535);
  return _ha_bridge.remote().subscribe(
      _ha_bridge.getTopic(HaBridge::TopicType::Command, COMPONENT, _child_object_id, OBJECT_ID_COLOR_TEMPERATURE),
      [callback, lo, hi](std::string, std::string message) {
        char *end = nullptr;
        long val = std::strtol(message.c_str(), &end, 10);
        if (end != message.c_str() && *end == '\0' && val >= lo && val <= hi) {
          callback(static_cast<uint16_t>(val));
        }
      });
}

bool HaEntityLight::setOnRgb(std::function<void(RGB)> callback) {
  if (!_configuration.with_rgb_color || !callback) {
    return false;
  }

  return _ha_bridge.remote().subscribe(
      _ha_bridge.getTopic(HaBridge::TopicType::Command, COMPONENT, _child_object_id, OBJECT_ID_RGB),
      [callback](std::string, std::string message) {
        RGB rgb{};
        if (parseRgb(message, rgb)) {
          callback(rgb);
        }
      });
}

bool HaEntityLight::setOnEffect(std::function<void(std::string)> callback) {
  if (_configuration.effects.empty() || !callback) {
    return false;
  }

  return _ha_bridge.remote().subscribe(
      _ha_bridge.getTopic(HaBridge::TopicType::Command, COMPONENT, _child_object_id, OBJECT_ID_EFFECT),
      [callback, effects = _configuration.effects](std::string, std::string message) {
        if (std::find(effects.begin(), effects.end(), message) != effects.end()) {
          callback(message);
        }
      });
}
