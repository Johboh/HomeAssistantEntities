// Copyright (c) Johan Böhlin
// SPDX-License-Identifier: MIT

#include "HaEntityNotify.h"
#include <HaUtilities.h>
#include <IJson.h>
#include <utility>

using namespace homeassistantentities;

#define COMPONENT "notify"
#define OBJECT_ID "notify"
#define OBJECT_ID_COMMAND "set"

HaEntityNotify::HaEntityNotify(HaBridge &ha_bridge, std::string name, std::optional<std::string> child_object_id,
                               Configuration configuration)
    : _name(homeassistantentities::trim(name)), _ha_bridge(ha_bridge), _child_object_id(child_object_id.value_or("")),
      _configuration(std::move(configuration)) {}

bool HaEntityNotify::buildComponentConfiguration(ComponentConfig &out) {
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

  doc["command_topic"] =
      _ha_bridge.getTopic(HaBridge::TopicType::Command, COMPONENT, _child_object_id, OBJECT_ID_COMMAND);

  return true;
}

void HaEntityNotify::publishConfiguration() {
  ComponentConfig cfg;
  if (buildComponentConfiguration(cfg)) {
    _ha_bridge.publishConfiguration(cfg.component, cfg.object_id, cfg.child_object_id, cfg.doc);
  }
}

void HaEntityNotify::unpublishConfiguration() {
  _ha_bridge.unpublishConfiguration(COMPONENT, OBJECT_ID, _child_object_id);
}

bool HaEntityNotify::onNotify(std::function<void(const std::string &message, const std::string &title)> callback) {
  if (!callback) {
    return false;
  }
  return _ha_bridge.remote().subscribe(
      _ha_bridge.getTopic(HaBridge::TopicType::Command, COMPONENT, _child_object_id, OBJECT_ID_COMMAND),
      [callback](std::string, std::string message) {
        std::string text = message;
        std::string title = "";
        if (!message.empty() && message.front() == '{') {
#if __has_include("nlohmann/json.hpp")
          try {
            auto j = nlohmann::json::parse(message);
            if (j.contains("message") && j["message"].is_string()) {
              text = j["message"].get<std::string>();
            }
            if (j.contains("title") && j["title"].is_string()) {
              title = j["title"].get<std::string>();
            }
          } catch (...) {
          }
#elif __has_include(<ArduinoJson.h>)
          JsonDocument jdoc;
          if (deserializeJson(jdoc, message) == DeserializationError::Ok) {
            if (jdoc["message"].is<const char *>())
              text = jdoc["message"].as<std::string>();
            if (jdoc["title"].is<const char *>())
              title = jdoc["title"].as<std::string>();
          }
#endif
        }
        callback(text, title);
      });
}

bool HaEntityNotify::onNotify(std::function<void(const std::string &message)> callback) {
  if (!callback) {
    return false;
  }
  return onNotify([callback](const std::string &message, const std::string &) { callback(message); });
}
