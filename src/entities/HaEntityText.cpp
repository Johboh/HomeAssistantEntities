// Copyright (c) Johan Böhlin
// SPDX-License-Identifier: MIT

#include "HaEntityText.h"
#include <HaUtilities.h>
#include <IJson.h>
#include <algorithm>
#include <utility>

#define OBJECT_ID_TEXT "text"

HaEntityText::Configuration HaEntityText::sanitizeConfiguration(Configuration configuration) {
  if (configuration.min_text_length > configuration.max_text_length) {
    std::swap(configuration.min_text_length, configuration.max_text_length);
  }
  return configuration;
}

HaEntityText::HaEntityText(HaBridge &ha_bridge, std::string name, std::string child_object_id,
                           Configuration configuration)
    : _configuration(sanitizeConfiguration(std::move(configuration))),
      _ha_entity_actuator(ha_bridge, std::move(name), std::move(child_object_id),
                          HaEntityActuator::Configuration{
                              .device_class = homeassistantentities::Text::Undefined::Empty{},
                              .sub_topic = OBJECT_ID_TEXT,
                              .with_state_topic = _configuration.with_state_topic,
                              .with_command_topic = true,
                              .retain = _configuration.retain,
                              .optimistic = _configuration.optimistic,
                              .icon = _configuration.icon,
                              .entity_category = _configuration.entity_category,
                              .enabled_by_default = _configuration.enabled_by_default,
                              .extra_configuration =
                                  [cfg = _configuration](IJsonDocument &doc) {
                                    doc["min"] = cfg.min_text_length;
                                    doc["max"] = cfg.max_text_length;
                                    if (!cfg.pattern.empty()) {
                                      doc["pattern"] = cfg.pattern;
                                    }
                                    doc["mode"] = cfg.is_password ? "password" : "text";
                                  },
                          }) {}

bool HaEntityText::buildComponentConfiguration(ComponentConfig &out) {
  return buildChildComponentConfiguration(_ha_entity_actuator, out);
}

void HaEntityText::publishConfiguration() { _ha_entity_actuator.publishConfiguration(); }

void HaEntityText::unpublishConfiguration() { _ha_entity_actuator.unpublishConfiguration(); }

void HaEntityText::republishState() { _ha_entity_actuator.republishState(); }

void HaEntityText::publish(const std::string &str) {
  if (!_configuration.with_state_topic) {
    return;
  }
  auto len = homeassistantentities::utf8CodePointCount(str);
  if (len < _configuration.min_text_length || len > _configuration.max_text_length) {
    return;
  }
  _ha_entity_actuator.publish(str);
}

void HaEntityText::update(const std::string &str) {
  if (!_configuration.with_state_topic) {
    return;
  }
  auto len = homeassistantentities::utf8CodePointCount(str);
  if (len < _configuration.min_text_length || len > _configuration.max_text_length) {
    return;
  }
  _ha_entity_actuator.update(str);
}

bool HaEntityText::onCommand(std::function<void(std::string)> callback) {
  if (!callback) {
    return false;
  }
  uint8_t min_len = _configuration.min_text_length;
  uint8_t max_len = _configuration.max_text_length;
  return _ha_entity_actuator.onCommand([callback, min_len, max_len](const std::string &message) {
    auto len = homeassistantentities::utf8CodePointCount(message);
    if (len >= min_len && len <= max_len) {
      callback(message);
    }
  });
}
