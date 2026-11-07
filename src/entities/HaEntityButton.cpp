// Copyright (c) Johan Böhlin
// SPDX-License-Identifier: MIT

#include "HaEntityButton.h"
#include <IJson.h>
#include <utility>

#define PAYLOAD_PRESS "PRESS"

HaEntityButton::HaEntityButton(HaBridge &ha_bridge, std::string name, std::string child_object_id,
                               Configuration configuration)
    : _ha_entity_actuator(ha_bridge, std::move(name), std::move(child_object_id),
                          HaEntityActuator::Configuration{
                              .device_class = homeassistantentities::Button::Undefined::Empty{},
                              .device_class_override = configuration.device_class,
                              .sub_topic = "",
                              .with_state_topic = false,
                              .with_command_topic = true,
                              .retain = configuration.retain,
                              .emit_retain_when_false = false,
                              .icon = configuration.icon,
                              .entity_category = configuration.entity_category,
                              .enabled_by_default = configuration.enabled_by_default,
                              .extra_configuration = [](IJsonDocument &doc) { doc["payload_press"] = PAYLOAD_PRESS; },
                          }) {}

bool HaEntityButton::buildComponentConfiguration(ComponentConfig &out) {
  return buildChildComponentConfiguration(_ha_entity_actuator, out);
}

void HaEntityButton::publishConfiguration() { _ha_entity_actuator.publishConfiguration(); }

void HaEntityButton::unpublishConfiguration() { _ha_entity_actuator.unpublishConfiguration(); }

void HaEntityButton::republishState() { _ha_entity_actuator.republishState(); }

bool HaEntityButton::onCommand(std::function<void(void)> callback) {
  if (!callback) {
    return false;
  }
  return _ha_entity_actuator.onCommand([callback](const std::string &message) {
    if (message == PAYLOAD_PRESS) {
      callback();
    }
  });
}
