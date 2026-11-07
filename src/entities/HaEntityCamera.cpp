// Copyright (c) Johan Böhlin
// SPDX-License-Identifier: MIT

#include "HaEntityCamera.h"
#include <HaUtilities.h>
#include <IJson.h>
#include <utility>

using namespace homeassistantentities;

#define COMPONENT "camera"
#define OBJECT_ID "camera"
#define OBJECT_ID_IMAGE "image"

HaEntityCamera::HaEntityCamera(HaBridge &ha_bridge, std::string name, std::optional<std::string> child_object_id,
                               Configuration configuration)
    : _name(homeassistantentities::trim(name)), _ha_bridge(ha_bridge), _child_object_id(child_object_id.value_or("")),
      _configuration(std::move(configuration)) {}

bool HaEntityCamera::buildComponentConfiguration(ComponentConfig &out) {
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

  doc["topic"] = _ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, _child_object_id, OBJECT_ID_IMAGE);

  return true;
}

void HaEntityCamera::publishConfiguration() {
  ComponentConfig cfg;
  if (buildComponentConfiguration(cfg)) {
    _ha_bridge.publishConfiguration(cfg.component, cfg.object_id, cfg.child_object_id, cfg.doc);
  }
}

void HaEntityCamera::unpublishConfiguration() {
  _ha_bridge.unpublishConfiguration(COMPONENT, OBJECT_ID, _child_object_id);
}

void HaEntityCamera::publishImage(const uint8_t *data, size_t length) {
  if (!data || length == 0) {
    return;
  }
  publishImage(std::string(reinterpret_cast<const char *>(data), length));
}

void HaEntityCamera::publishImage(const std::string &data) {
  _ha_bridge.publishMessage(
      _ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, _child_object_id, OBJECT_ID_IMAGE), data);
}
