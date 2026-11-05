// Copyright (c) Johan Böhlin
// SPDX-License-Identifier: MIT

#include "HaEntityNumber.h"
#include <HaUtilities.h>
#include <IJson.h>
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <utility>

#define COMPONENT "number"

using namespace homeassistantentities;

HaEntityNumber::HaEntityNumber(HaBridge &ha_bridge, std::string name, std::string object_id,
                               Configuration configuration)
    : _name(trim(name)), _ha_bridge(ha_bridge), _object_id(std::move(object_id)),
      _configuration(std::move(configuration)) {
  if (!std::isfinite(_configuration.min_value)) {
    _configuration.min_value = 1.0f;
  }
  if (!std::isfinite(_configuration.max_value)) {
    _configuration.max_value = 100.0f;
  }
  if (_configuration.min_value > _configuration.max_value) {
    std::swap(_configuration.min_value, _configuration.max_value);
  }
  if (!std::isfinite(_configuration.step) || _configuration.step <= 0.0f) {
    _configuration.step = 1.0f;
  }
}

bool HaEntityNumber::buildComponentConfiguration(ComponentConfig &out) {
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

  doc["step"] = _configuration.step;
  doc["min"] = _configuration.min_value;
  doc["max"] = _configuration.max_value;
  doc["retain"] = _configuration.retain;

  if (_configuration.mode == Configuration::Mode::Box) {
    doc["mode"] = "box";
  } else if (_configuration.mode == Configuration::Mode::Slider) {
    doc["mode"] = "slider";
  }

  auto unit = trim(_configuration.unit);
  if (!unit.empty()) {
    doc["unit_of_measurement"] = unit;
  }

  auto device_class = trim(_configuration.device_class);
  if (!device_class.empty()) {
    doc["device_class"] = device_class;
  }

  auto icon = trim(_configuration.icon);
  if (!icon.empty()) {
    doc["icon"] = icon;
  }
  auto ec = trim(_configuration.entity_category);
  if (!ec.empty()) {
    doc["entity_category"] = ec;
  }

  doc["state_topic"] = _ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, _object_id);
  doc["command_topic"] = _ha_bridge.getTopic(HaBridge::TopicType::Command, COMPONENT, _object_id);

  return true;
}

void HaEntityNumber::publishConfiguration() {
  ComponentConfig cfg;
  if (buildComponentConfiguration(cfg)) {
    _ha_bridge.publishConfiguration(cfg.component, cfg.object_id, cfg.child_object_id, cfg.doc);
  }
}

void HaEntityNumber::unpublishConfiguration() { _ha_bridge.unpublishConfiguration(COMPONENT, _object_id, ""); }

void HaEntityNumber::republishState() {
  if (_number) {
    publishNumber(*_number);
  }
}

void HaEntityNumber::publishNumber(float number) {
  if (!std::isfinite(number)) {
    return;
  }
  float clamped = std::clamp(number, _configuration.min_value, _configuration.max_value);
  _ha_bridge.publishMessage(_ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, _object_id),
                            formatNumber(clamped));
  _number = clamped;
}

void HaEntityNumber::updateNumber(float number) {
  if (!std::isfinite(number)) {
    return;
  }
  float clamped = std::clamp(number, _configuration.min_value, _configuration.max_value);
  if (!_number || *_number != clamped) {
    publishNumber(clamped);
  }
}

bool HaEntityNumber::setOnNumber(std::function<void(float)> callback) {
  if (!callback) {
    return false;
  }
  float min_val = _configuration.min_value;
  float max_val = _configuration.max_value;
  return _ha_bridge.remote().subscribe(_ha_bridge.getTopic(HaBridge::TopicType::Command, COMPONENT, _object_id),
                                       [callback, min_val, max_val](std::string, std::string message) {
                                         char *end = nullptr;
                                         float num = std::strtof(message.c_str(), &end);
                                         if (end != message.c_str() && *end == '\0' && std::isfinite(num) &&
                                             num >= min_val && num <= max_val) {
                                           callback(num);
                                         }
                                       });
}
