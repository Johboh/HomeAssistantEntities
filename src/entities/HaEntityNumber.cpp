// Copyright (c) Johan Böhlin
// SPDX-License-Identifier: MIT

#include "HaEntityNumber.h"
#include <IJson.h>
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <utility>

HaEntityNumber::Configuration HaEntityNumber::sanitizeConfiguration(Configuration configuration) {
  if (!std::isfinite(configuration.min_value)) {
    configuration.min_value = 1.0f;
  }
  if (!std::isfinite(configuration.max_value)) {
    configuration.max_value = 100.0f;
  }
  if (configuration.min_value > configuration.max_value) {
    std::swap(configuration.min_value, configuration.max_value);
  }
  if (!std::isfinite(configuration.step) || configuration.step <= 0.0f) {
    configuration.step = 1.0f;
  }
  return configuration;
}

HaEntityNumber::HaEntityNumber(HaBridge &ha_bridge, std::string name, std::string object_id,
                               Configuration configuration)
    : _configuration(sanitizeConfiguration(std::move(configuration))),
      _ha_entity_actuator(ha_bridge, std::move(name), std::nullopt,
                          HaEntityActuator::Configuration{
                              .device_class = homeassistantentities::Number::Undefined::Empty{},
                              .custom_unit_of_measurement = _configuration.unit,
                              .object_id_override = std::move(object_id),
                              .device_class_override = _configuration.device_class,
                              .retain = _configuration.retain,
                              .optimistic = _configuration.optimistic,
                              .icon = _configuration.icon,
                              .entity_category = _configuration.entity_category,
                              .suggested_display_precision = _configuration.suggested_display_precision,
                              .enabled_by_default = _configuration.enabled_by_default,
                              .extra_configuration =
                                  [cfg = _configuration](IJsonDocument &doc) {
                                    doc["step"] = cfg.step;
                                    doc["min"] = cfg.min_value;
                                    doc["max"] = cfg.max_value;
                                    if (cfg.mode == Configuration::Mode::Box) {
                                      doc["mode"] = "box";
                                    } else if (cfg.mode == Configuration::Mode::Slider) {
                                      doc["mode"] = "slider";
                                    }
                                  },
                          }) {}

bool HaEntityNumber::buildComponentConfiguration(ComponentConfig &out) {
  return buildChildComponentConfiguration(_ha_entity_actuator, out);
}

void HaEntityNumber::publishConfiguration() { _ha_entity_actuator.publishConfiguration(); }

void HaEntityNumber::unpublishConfiguration() { _ha_entity_actuator.unpublishConfiguration(); }

void HaEntityNumber::republishState() { _ha_entity_actuator.republishState(); }

void HaEntityNumber::publish(float number) {
  if (!std::isfinite(number)) {
    return;
  }
  float clamped = std::clamp(number, _configuration.min_value, _configuration.max_value);
  _ha_entity_actuator.publish(static_cast<double>(clamped));
}

void HaEntityNumber::update(float number) {
  if (!std::isfinite(number)) {
    return;
  }
  float clamped = std::clamp(number, _configuration.min_value, _configuration.max_value);
  _ha_entity_actuator.update(static_cast<double>(clamped));
}

bool HaEntityNumber::onCommand(std::function<void(float)> callback) {
  if (!callback) {
    return false;
  }
  float min_val = _configuration.min_value;
  float max_val = _configuration.max_value;
  return _ha_entity_actuator.onCommand([callback, min_val, max_val](const std::string &message) {
    char *end = nullptr;
    float num = std::strtof(message.c_str(), &end);
    if (end != message.c_str() && *end == '\0' && std::isfinite(num) && num >= min_val && num <= max_val) {
      callback(num);
    }
  });
}
