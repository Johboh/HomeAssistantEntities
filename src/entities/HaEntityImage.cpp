// Copyright (c) Johan Böhlin
// SPDX-License-Identifier: MIT

#include "HaEntityImage.h"
#include <HaUtilities.h>
#include <IJson.h>
#include <utility>

using namespace homeassistantentities;

#define COMPONENT "image"
#define OBJECT_ID "image"
#define OBJECT_ID_IMAGE "image"
#define OBJECT_ID_URL "url"

HaEntityImage::HaEntityImage(HaBridge &ha_bridge, std::string name, std::optional<std::string> child_object_id,
                             Configuration configuration)
    : _name(homeassistantentities::trim(name)), _ha_bridge(ha_bridge), _child_object_id(child_object_id.value_or("")),
      _configuration(std::move(configuration)) {}

bool HaEntityImage::buildComponentConfiguration(ComponentConfig &out) {
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

  doc["content_type"] = _configuration.content_type;

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

  if (_configuration.url_mode) {
    doc["url_topic"] = _ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, _child_object_id, OBJECT_ID_URL);
  } else {
    doc["image_topic"] = _ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, _child_object_id, OBJECT_ID_IMAGE);
  }

  return true;
}

void HaEntityImage::publishConfiguration() {
  ComponentConfig cfg;
  if (buildComponentConfiguration(cfg)) {
    _ha_bridge.publishConfiguration(cfg.component, cfg.object_id, cfg.child_object_id, cfg.doc);
  }
}

void HaEntityImage::unpublishConfiguration() {
  _ha_bridge.unpublishConfiguration(COMPONENT, OBJECT_ID, _child_object_id);
}

void HaEntityImage::republishState() {
  if (_url && _configuration.url_mode) {
    publishUrl(*_url);
  }
}

void HaEntityImage::publishUrl(const std::string &url) {
  _url = url;
  _ha_bridge.publishMessage(_ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, _child_object_id, OBJECT_ID_URL),
                            url);
}

void HaEntityImage::publishBytes(const uint8_t *data, size_t length) {
  if (!data || length == 0) {
    return;
  }
  publishBytes(std::string(reinterpret_cast<const char *>(data), length));
}

void HaEntityImage::publishBytes(const std::string &data) {
  _ha_bridge.publishMessage(
      _ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, _child_object_id, OBJECT_ID_IMAGE), data);
}
