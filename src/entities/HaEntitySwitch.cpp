// Copyright (c) Johan Böhlin
// SPDX-License-Identifier: MIT

#include "HaEntitySwitch.h"
#include <utility>

#define OBJECT_ID_ONOFF "onoff"

// NOTE! We swap object ID and child object ID via sub_topic to get a nicer state/command topic path.

HaEntitySwitch::HaEntitySwitch(HaBridge &ha_bridge, std::string name, std::string child_object_id,
                               Configuration configuration)
    : _ha_entity_actuator(ha_bridge, std::move(name), std::move(child_object_id),
                          HaEntityActuator::Configuration{
                              .device_class = homeassistantentities::Switch::Undefined::Empty{},
                              .device_class_override = configuration.device_class,
                              .sub_topic = OBJECT_ID_ONOFF,
                              .retain = configuration.retain,
                              .optimistic = configuration.optimistic,
                              .icon = configuration.icon,
                              .entity_category = configuration.entity_category,
                              .enabled_by_default = configuration.enabled_by_default,
                          }) {}

bool HaEntitySwitch::buildComponentConfiguration(ComponentConfig &out) {
  return buildChildComponentConfiguration(_ha_entity_actuator, out);
}

void HaEntitySwitch::publishConfiguration() { _ha_entity_actuator.publishConfiguration(); }

void HaEntitySwitch::unpublishConfiguration() { _ha_entity_actuator.unpublishConfiguration(); }

void HaEntitySwitch::republishState() { _ha_entity_actuator.republishState(); }

void HaEntitySwitch::publish(bool on) { _ha_entity_actuator.publish(on ? "ON" : "OFF"); }

void HaEntitySwitch::update(bool on) { _ha_entity_actuator.update(on ? "ON" : "OFF"); }

bool HaEntitySwitch::onCommand(std::function<void(bool)> callback) {
  if (!callback) {
    return false;
  }
  return _ha_entity_actuator.onCommand([callback](const std::string &message) { callback(message == "ON"); });
}
