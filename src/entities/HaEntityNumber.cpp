// Copyright (c) Johan Böhlin
// SPDX-License-Identifier: MIT

#include "HaEntityNumber.h"
#include <HaUtilities.h>
#include <IJson.h>
#include <cmath>
#include <cstdlib>
#include <utility>

#define COMPONENT "number"

using namespace homeassistantentities;

HaEntityNumber::HaEntityNumber(HaBridge &ha_bridge, std::string name, std::string object_id,
                               Configuration configuration)
    : _name(trim(name)), _ha_bridge(ha_bridge), _object_id(std::move(object_id)),
      _configuration(std::move(configuration)) {}

void HaEntityNumber::publishConfiguration() {
  IJsonDocument doc;

  if (!_name.empty()) {
    doc["name"] = _name;
  } else {
    doc["name"] = nullptr;
  }

  doc["platform"] = "number";
  doc["step"] = _configuration.step;
  doc["min"] = _configuration.min_value;
  doc["max"] = _configuration.max_value;
  doc["retain"] = _configuration.retain;

  auto unit = trim(_configuration.unit);
  if (!unit.empty()) {
    doc["unit_of_measurement"] = unit;
  }

  auto device_class = trim(_configuration.device_class);
  if (!device_class.empty()) {
    doc["device_class"] = device_class;
  }

  doc["state_topic"] = _ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, _object_id);
  doc["command_topic"] = _ha_bridge.getTopic(HaBridge::TopicType::Command, COMPONENT, _object_id);

  _ha_bridge.publishConfiguration(COMPONENT, _object_id, "", doc);
}

void HaEntityNumber::republishState() {
  if (_number) {
    publishNumber(*_number);
  }
}

void HaEntityNumber::publishNumber(float number) {
  if (!std::isfinite(number)) {
    return;
  }
  _ha_bridge.publishMessage(_ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, _object_id),
                            formatNumber(number));
  _number = number;
}

void HaEntityNumber::updateNumber(float number) {
  if (!std::isfinite(number)) {
    return;
  }
  if (!_number || *_number != number) {
    publishNumber(number);
  }
}

bool HaEntityNumber::setOnNumber(std::function<void(float)> callback) {
  if (!callback) {
    return false;
  }
  return _ha_bridge.remote().subscribe(_ha_bridge.getTopic(HaBridge::TopicType::Command, COMPONENT, _object_id),
                                       [callback](std::string, std::string message) {
                                         char *end = nullptr;
                                         float num = std::strtof(message.c_str(), &end);
                                         if (end != message.c_str() && *end == '\0' && std::isfinite(num)) {
                                           callback(num);
                                         }
                                       });
}
