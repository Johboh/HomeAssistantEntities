// Copyright (c) Johan Böhlin
// SPDX-License-Identifier: MIT

#include "HaEntityDateTime.h"
#include <cstdio>
#include <utility>

#define OBJECT_ID_DATETIME "datetime"

static std::string formatDateTime(int year, int month, int day, int hour, int minute, int second) {
  char buf[32];
  std::snprintf(buf, sizeof(buf), "%04d-%02d-%02dT%02d:%02d:%02d", year, month, day, hour, minute, second);
  return std::string(buf);
}

HaEntityDateTime::HaEntityDateTime(HaBridge &ha_bridge, std::string name, std::optional<std::string> child_object_id,
                                   Configuration configuration)
    : _configuration(std::move(configuration)),
      _ha_entity_actuator(ha_bridge, std::move(name), std::move(child_object_id),
                          HaEntityActuator::Configuration{
                              .device_class = homeassistantentities::DateTime::Undefined::Empty{},
                              .sub_topic = OBJECT_ID_DATETIME,
                              .with_state_topic = _configuration.with_state_topic,
                              .with_command_topic = true,
                              .retain = _configuration.retain,
                              .optimistic = _configuration.optimistic,
                              .icon = _configuration.icon,
                              .entity_category = _configuration.entity_category,
                              .enabled_by_default = _configuration.enabled_by_default,
                          }) {}

bool HaEntityDateTime::buildComponentConfiguration(ComponentConfig &out) {
  return buildChildComponentConfiguration(_ha_entity_actuator, out);
}

void HaEntityDateTime::publishConfiguration() { _ha_entity_actuator.publishConfiguration(); }

void HaEntityDateTime::unpublishConfiguration() { _ha_entity_actuator.unpublishConfiguration(); }

void HaEntityDateTime::republishState() { _ha_entity_actuator.republishState(); }

void HaEntityDateTime::publish(const std::string &datetime_str) { _ha_entity_actuator.publish(datetime_str); }

void HaEntityDateTime::publish(int year, int month, int day, int hour, int minute, int second) {
  publish(formatDateTime(year, month, day, hour, minute, second));
}

void HaEntityDateTime::update(const std::string &datetime_str) { _ha_entity_actuator.update(datetime_str); }

void HaEntityDateTime::update(int year, int month, int day, int hour, int minute, int second) {
  update(formatDateTime(year, month, day, hour, minute, second));
}

bool HaEntityDateTime::onCommand(std::function<void(const std::string &datetime_str)> callback) {
  return _ha_entity_actuator.onCommand(std::move(callback));
}
