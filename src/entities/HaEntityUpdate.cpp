// Copyright (c) Johan Böhlin
// SPDX-License-Identifier: MIT

#include "HaEntityUpdate.h"
#include <HaUtilities.h>
#include <IJson.h>
#include <algorithm>
#include <utility>

#define PAYLOAD_INSTALL "INSTALL"

HaEntityUpdate::HaEntityUpdate(HaBridge &ha_bridge, std::string name, std::optional<std::string> child_object_id,
                               Configuration configuration)
    : _can_install(configuration.can_install),
      _ha_entity_actuator(ha_bridge, std::move(name), std::move(child_object_id),
                          HaEntityActuator::Configuration{
                              .device_class = homeassistantentities::Update::Undefined::Empty{},
                              .device_class_override = configuration.device_class,
                              .sub_topic = "",
                              .with_state_topic = true,
                              .with_command_topic = configuration.can_install,
                              .with_attributes = configuration.with_attributes,
                              .retain = configuration.retain,
                              .emit_retain_when_false = false,
                              .icon = configuration.icon,
                              .entity_category = configuration.entity_category,
                              .enabled_by_default = configuration.enabled_by_default,
                              .extra_configuration =
                                  [can_install = configuration.can_install](IJsonDocument &doc) {
                                    if (can_install) {
                                      doc["payload_install"] = PAYLOAD_INSTALL;
                                    }
                                  },
                          }) {}

std::optional<std::string> HaEntityUpdate::encodeState(const State &state) {
  auto installed = homeassistantentities::trim(state.installed_version);
  if (installed.empty()) {
    return std::nullopt;
  }
  auto latest = homeassistantentities::trim(state.latest_version);

  IJsonDocument doc;
  doc["installed_version"] = installed;
  doc["latest_version"] = latest.empty() ? installed : latest;

  if (!state.title.empty()) {
    doc["title"] = state.title;
  }
  if (!state.release_summary.empty()) {
    doc["release_summary"] = state.release_summary;
  }
  if (!state.release_url.empty()) {
    doc["release_url"] = state.release_url;
  }
  if (!state.entity_picture.empty()) {
    doc["entity_picture"] = state.entity_picture;
  }
  if (state.in_progress) {
    doc["in_progress"] = *state.in_progress;
  }
  if (state.update_percentage) {
    doc["update_percentage"] = std::min<uint8_t>(*state.update_percentage, 100);
  }
  return toJsonString(doc);
}

bool HaEntityUpdate::buildComponentConfiguration(ComponentConfig &out) {
  return buildChildComponentConfiguration(_ha_entity_actuator, out);
}

void HaEntityUpdate::publishConfiguration() { _ha_entity_actuator.publishConfiguration(); }

void HaEntityUpdate::unpublishConfiguration() { _ha_entity_actuator.unpublishConfiguration(); }

void HaEntityUpdate::republishState() { _ha_entity_actuator.republishState(); }

void HaEntityUpdate::publish(const State &state, const Attributes::Map &attributes) {
  auto payload = encodeState(state);
  if (payload) {
    _ha_entity_actuator.publish(*payload, attributes);
  }
}

void HaEntityUpdate::update(const State &state, const Attributes::Map &attributes) {
  auto payload = encodeState(state);
  if (payload) {
    _ha_entity_actuator.update(*payload, attributes);
  }
}

void HaEntityUpdate::publish(const std::string &installed_version, const std::string &latest_version,
                             const Attributes::Map &attributes) {
  publish(State{.installed_version = installed_version, .latest_version = latest_version}, attributes);
}

void HaEntityUpdate::update(const std::string &installed_version, const std::string &latest_version,
                            const Attributes::Map &attributes) {
  update(State{.installed_version = installed_version, .latest_version = latest_version}, attributes);
}

bool HaEntityUpdate::onCommand(std::function<void(void)> callback) {
  if (!_can_install || !callback) {
    return false;
  }
  return _ha_entity_actuator.onCommand([callback](const std::string &message) {
    if (message == PAYLOAD_INSTALL) {
      callback();
    }
  });
}
