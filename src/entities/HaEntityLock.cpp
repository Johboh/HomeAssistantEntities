// Copyright (c) Johan Böhlin
// SPDX-License-Identifier: MIT

#include "HaEntityLock.h"
#include <HaUtilities.h>
#include <IJson.h>
#include <utility>

HaEntityLock::HaEntityLock(HaBridge &ha_bridge, std::string name, std::optional<std::string> child_object_id,
                           Configuration configuration)
    : _with_open(configuration.with_open),
      _ha_entity_actuator(
          ha_bridge, std::move(name), std::move(child_object_id),
          HaEntityActuator::Configuration{
              .device_class = homeassistantentities::Lock::Undefined::Empty{},
              .sub_topic = "",
              .with_state_topic = true,
              .with_command_topic = true,
              .with_attributes = configuration.with_attributes,
              .retain = configuration.retain,
              .optimistic = configuration.optimistic,
              .icon = configuration.icon,
              .entity_category = configuration.entity_category,
              .enabled_by_default = configuration.enabled_by_default,
              .extra_configuration =
                  [with_open = configuration.with_open,
                   code_fmt = homeassistantentities::trim(configuration.code_format)](IJsonDocument &doc) {
                    doc["payload_lock"] = "LOCK";
                    doc["payload_unlock"] = "UNLOCK";
                    if (with_open) {
                      doc["payload_open"] = "OPEN";
                    }
                    if (!code_fmt.empty()) {
                      doc["code_format"] = code_fmt;
                    }
                  },
          }) {}

const char *HaEntityLock::stateToPayload(State state) {
  switch (state) {
  case State::Locked:
    return "LOCKED";
  case State::Unlocked:
    return "UNLOCKED";
  case State::Locking:
    return "LOCKING";
  case State::Unlocking:
    return "UNLOCKING";
  case State::Jammed:
    return "JAMMED";
  case State::Opening:
    return "OPENING";
  case State::Open:
    return "OPEN";
  }
  return "UNKNOWN";
}

bool HaEntityLock::buildComponentConfiguration(ComponentConfig &out) {
  return buildChildComponentConfiguration(_ha_entity_actuator, out);
}

void HaEntityLock::publishConfiguration() { _ha_entity_actuator.publishConfiguration(); }

void HaEntityLock::unpublishConfiguration() { _ha_entity_actuator.unpublishConfiguration(); }

void HaEntityLock::republishState() { _ha_entity_actuator.republishState(); }

void HaEntityLock::publish(bool locked, const Attributes::Map &attributes) {
  publish(locked ? State::Locked : State::Unlocked, attributes);
}

void HaEntityLock::update(bool locked, const Attributes::Map &attributes) {
  update(locked ? State::Locked : State::Unlocked, attributes);
}

void HaEntityLock::publish(State state, const Attributes::Map &attributes) {
  _ha_entity_actuator.publish(std::string(stateToPayload(state)), attributes);
}

void HaEntityLock::update(State state, const Attributes::Map &attributes) {
  _ha_entity_actuator.update(std::string(stateToPayload(state)), attributes);
}

void HaEntityLock::publishAttributes(const Attributes::Map &attributes) {
  _ha_entity_actuator.publishAttributes(attributes);
}

void HaEntityLock::updateAttributes(const Attributes::Map &attributes) {
  _ha_entity_actuator.updateAttributes(attributes);
}

bool HaEntityLock::onCommand(std::function<void(Action)> callback) {
  if (!callback) {
    return false;
  }
  bool with_open = _with_open;
  return _ha_entity_actuator.onCommand([callback, with_open](const std::string &message) {
    Action action = Action::Unknown;
    if (message == "LOCK") {
      action = Action::Lock;
    } else if (message == "UNLOCK") {
      action = Action::Unlock;
    } else if (with_open && message == "OPEN") {
      action = Action::Open;
    }
    if (action != Action::Unknown) {
      callback(action);
    }
  });
}
