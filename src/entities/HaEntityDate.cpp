// Copyright (c) Johan Böhlin
// SPDX-License-Identifier: MIT

#include "HaEntityDate.h"
#include <cstdio>
#include <utility>

#define OBJECT_ID_DATE "date"

static std::string formatDate(int year, int month, int day) {
  char buf[16];
  std::snprintf(buf, sizeof(buf), "%04d-%02d-%02d", year, month, day);
  return std::string(buf);
}

HaEntityDate::HaEntityDate(HaBridge &ha_bridge, std::string name, std::optional<std::string> child_object_id,
                           Configuration configuration)
    : _configuration(std::move(configuration)),
      _ha_entity_actuator(ha_bridge, std::move(name), std::move(child_object_id),
                          HaEntityActuator::Configuration{
                              .device_class = homeassistantentities::Date::Undefined::Empty{},
                              .sub_topic = OBJECT_ID_DATE,
                              .with_state_topic = _configuration.with_state_topic,
                              .with_command_topic = true,
                              .retain = _configuration.retain,
                              .optimistic = _configuration.optimistic,
                              .icon = _configuration.icon,
                              .entity_category = _configuration.entity_category,
                              .enabled_by_default = _configuration.enabled_by_default,
                          }) {}

bool HaEntityDate::buildComponentConfiguration(ComponentConfig &out) {
  return buildChildComponentConfiguration(_ha_entity_actuator, out);
}

void HaEntityDate::publishConfiguration() { _ha_entity_actuator.publishConfiguration(); }

void HaEntityDate::unpublishConfiguration() { _ha_entity_actuator.unpublishConfiguration(); }

void HaEntityDate::republishState() { _ha_entity_actuator.republishState(); }

void HaEntityDate::publish(const std::string &date_str) { _ha_entity_actuator.publish(date_str); }

void HaEntityDate::publish(int year, int month, int day) { publish(formatDate(year, month, day)); }

void HaEntityDate::update(const std::string &date_str) { _ha_entity_actuator.update(date_str); }

void HaEntityDate::update(int year, int month, int day) { update(formatDate(year, month, day)); }

bool HaEntityDate::onCommand(std::function<void(const std::string &date_str)> callback) {
  return _ha_entity_actuator.onCommand(std::move(callback));
}

bool HaEntityDate::onCommand(std::function<void(int year, int month, int day)> callback) {
  if (!callback) {
    return false;
  }
  return _ha_entity_actuator.onCommand([callback](const std::string &date_str) {
    int y = 0, m = 0, d = 0;
    if (std::sscanf(date_str.c_str(), "%d-%d-%d", &y, &m, &d) == 3) {
      callback(y, m, d);
    }
  });
}
