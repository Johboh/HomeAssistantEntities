// Copyright (c) Johan Böhlin
// SPDX-License-Identifier: MIT

#include "HaEntitySelect.h"
#include <utility>

HaEntitySelect::HaEntitySelect(HaBridge &ha_bridge, std::string name, std::string object_id,
                               Configuration configuration)
    : _ha_entity_actuator(ha_bridge, std::move(name), std::nullopt,
                          HaEntityActuator::Configuration{
                              .device_class = homeassistantentities::Select::Undefined::Empty{},
                              .object_id_override = std::move(object_id),
                              .retain = configuration.retain,
                              .optimistic = configuration.optimistic,
                              .icon = configuration.icon,
                              .entity_category = configuration.entity_category,
                              .enabled_by_default = configuration.enabled_by_default,
                              .options = std::move(configuration.options),
                          }) {}

bool HaEntitySelect::buildComponentConfiguration(ComponentConfig &out) {
  return buildChildComponentConfiguration(_ha_entity_actuator, out);
}

void HaEntitySelect::publishConfiguration() { _ha_entity_actuator.publishConfiguration(); }

void HaEntitySelect::unpublishConfiguration() { _ha_entity_actuator.unpublishConfiguration(); }

void HaEntitySelect::republishState() { _ha_entity_actuator.republishState(); }

void HaEntitySelect::publish(const std::string &option) { _ha_entity_actuator.publish(option); }

void HaEntitySelect::update(const std::string &option) { _ha_entity_actuator.update(option); }

bool HaEntitySelect::onCommand(std::function<void(std::string)> callback) {
  if (!callback) {
    return false;
  }
  return _ha_entity_actuator.onCommand([callback](const std::string &message) { callback(message); });
}
