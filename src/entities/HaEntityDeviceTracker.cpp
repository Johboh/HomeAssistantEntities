// Copyright (c) Johan Böhlin
// SPDX-License-Identifier: MIT

#include "HaEntityDeviceTracker.h"
#include <HaUtilities.h>
#include <IJson.h>
#include <utility>

using namespace homeassistantentities;

#define COMPONENT "device_tracker"
#define OBJECT_ID "device_tracker"
#define OBJECT_ID_STATE "state"

HaEntityDeviceTracker::HaEntityDeviceTracker(HaBridge &ha_bridge, std::string name,
                                             std::optional<std::string> child_object_id, Configuration configuration)
    : _name(homeassistantentities::trim(name)), _ha_bridge(ha_bridge), _child_object_id(child_object_id.value_or("")),
      _configuration(std::move(configuration)) {}

std::string HaEntityDeviceTracker::sourceTypeToString(SourceType type) {
  switch (type) {
  case SourceType::GPS:
    return "gps";
  case SourceType::Router:
    return "router";
  case SourceType::Bluetooth:
    return "bluetooth";
  case SourceType::BluetoothLE:
    return "bluetooth_le";
  }
  return "gps";
}

bool HaEntityDeviceTracker::buildComponentConfiguration(ComponentConfig &out) {
  out.component = COMPONENT;
  out.object_id = OBJECT_ID;
  out.child_object_id = _child_object_id;
  out.bridge = &_ha_bridge;

  auto &doc = out.doc;
  if (!_name.empty()) {
    doc["name"] = _name;
  } else {
    doc["name"] = nullptr;
  }

  doc["source_type"] = sourceTypeToString(_configuration.source_type);
  doc["payload_home"] = _configuration.payload_home;
  doc["payload_not_home"] = _configuration.payload_not_home;

  auto icon = homeassistantentities::trim(_configuration.icon);
  if (!icon.empty()) {
    doc["icon"] = icon;
  }
  auto ec = homeassistantentities::trim(_configuration.entity_category);
  if (!ec.empty()) {
    doc["entity_category"] = ec;
  }
  if (_configuration.enabled_by_default) {
    doc["enabled_by_default"] = *_configuration.enabled_by_default;
  }

  doc["retain"] = _configuration.retain;

  doc["state_topic"] = _ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, _child_object_id, OBJECT_ID_STATE);

  if (_configuration.with_attributes) {
    doc["json_attributes_topic"] =
        _ha_bridge.getTopic(HaBridge::TopicType::Attributes, COMPONENT, _child_object_id, "");
  }

  return true;
}

void HaEntityDeviceTracker::publishConfiguration() {
  ComponentConfig cfg;
  if (buildComponentConfiguration(cfg)) {
    _ha_bridge.publishConfiguration(cfg.component, cfg.object_id, cfg.child_object_id, cfg.doc);
  }
}

void HaEntityDeviceTracker::unpublishConfiguration() {
  _ha_bridge.unpublishConfiguration(COMPONENT, OBJECT_ID, _child_object_id);
}

void HaEntityDeviceTracker::republishState() {
  if (_state) {
    publishState(*_state);
  }
  if (_attributes) {
    publishAttributes(*_attributes);
  }
}

void HaEntityDeviceTracker::publishHome(bool is_home) {
  publishState(is_home ? _configuration.payload_home : _configuration.payload_not_home);
}

void HaEntityDeviceTracker::updateHome(bool is_home) {
  updateState(is_home ? _configuration.payload_home : _configuration.payload_not_home);
}

void HaEntityDeviceTracker::publishState(const std::string &state) {
  _state = state;
  _ha_bridge.publishMessage(
      _ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, _child_object_id, OBJECT_ID_STATE), state);
}

void HaEntityDeviceTracker::updateState(const std::string &state) {
  if (!_state || *_state != state) {
    publishState(state);
  }
}

void HaEntityDeviceTracker::publishLocation(double latitude, double longitude, std::optional<double> gps_accuracy,
                                            std::optional<uint8_t> battery) {
  Attributes::Map attr;
  attr["latitude"] = latitude;
  attr["longitude"] = longitude;
  if (gps_accuracy) {
    attr["gps_accuracy"] = *gps_accuracy;
  }
  if (battery) {
    attr["battery_level"] = static_cast<int>(*battery);
  }
  publishAttributes(attr);
}

void HaEntityDeviceTracker::publishAttributes(const Attributes::Map &attributes) {
  if (!_configuration.with_attributes) {
    return;
  }
  IJsonDocument doc;
  if (!Attributes::toJson(doc, attributes)) {
    return;
  }
  _attributes = attributes;
  _ha_bridge.publishMessage(_ha_bridge.getTopic(HaBridge::TopicType::Attributes, COMPONENT, _child_object_id, ""),
                            toJsonString(doc));
}
