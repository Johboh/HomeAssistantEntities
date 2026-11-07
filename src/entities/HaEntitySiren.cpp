// Copyright (c) Johan Böhlin
// SPDX-License-Identifier: MIT

#include "HaEntitySiren.h"
#include <HaUtilities.h>
#include <IJson.h>
#include <cstdlib>
#include <utility>

using namespace homeassistantentities;

#define COMPONENT "siren"
#define OBJECT_ID "siren"
#define OBJECT_ID_ONOFF "onoff"

HaEntitySiren::HaEntitySiren(HaBridge &ha_bridge, std::string name, std::optional<std::string> child_object_id,
                             Configuration configuration)
    : _name(homeassistantentities::trim(name)), _ha_bridge(ha_bridge), _child_object_id(child_object_id.value_or("")),
      _configuration(std::move(configuration)) {}

bool HaEntitySiren::buildComponentConfiguration(ComponentConfig &out) {
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
  if (_configuration.optimistic) {
    doc["optimistic"] = true;
  }
  if (_configuration.enabled_by_default) {
    doc["enabled_by_default"] = *_configuration.enabled_by_default;
  }

  doc["retain"] = _configuration.retain;

  if (!_configuration.available_tones.empty()) {
    JsonArrayType tones_array = createJsonArray(doc, "available_tones");
    for (const auto &tone : _configuration.available_tones) {
      addToJsonArray(tones_array, tone);
    }
  }

  doc["support_duration"] = _configuration.support_duration;
  doc["support_volume_set"] = _configuration.support_volume_set;

  doc["state_topic"] = _ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, _child_object_id, OBJECT_ID_ONOFF);
  doc["command_topic"] =
      _ha_bridge.getTopic(HaBridge::TopicType::Command, COMPONENT, _child_object_id, OBJECT_ID_ONOFF);

  if (_configuration.with_attributes) {
    doc["json_attributes_topic"] =
        _ha_bridge.getTopic(HaBridge::TopicType::Attributes, COMPONENT, _child_object_id, "");
  }

  return true;
}

void HaEntitySiren::publishConfiguration() {
  ComponentConfig cfg;
  if (buildComponentConfiguration(cfg)) {
    _ha_bridge.publishConfiguration(cfg.component, cfg.object_id, cfg.child_object_id, cfg.doc);
  }
}

void HaEntitySiren::unpublishConfiguration() {
  _ha_bridge.unpublishConfiguration(COMPONENT, OBJECT_ID, _child_object_id);
}

void HaEntitySiren::republishState() {
  if (_on) {
    publish(*_on);
  }
  if (_attributes) {
    publishAttributes(*_attributes);
  }
}

void HaEntitySiren::publish(bool on) {
  _on = on;
  _ha_bridge.publishMessage(
      _ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, _child_object_id, OBJECT_ID_ONOFF), on ? "ON" : "OFF");
}

void HaEntitySiren::update(bool on) {
  if (!_on || *_on != on) {
    publish(on);
  }
}

bool HaEntitySiren::onCommand(std::function<void(bool on)> callback) {
  if (!callback) {
    return false;
  }
  return _ha_bridge.remote().subscribe(
      _ha_bridge.getTopic(HaBridge::TopicType::Command, COMPONENT, _child_object_id, OBJECT_ID_ONOFF),
      [this, callback](std::string, std::string message) {
        if (!message.empty() && message.front() == '{') {
          // JSON command
          bool is_on = true;
          TurnOnParams params;
#if __has_include("nlohmann/json.hpp")
          auto j = nlohmann::json::parse(message, nullptr, false);
          if (!j.is_discarded()) {
            if (j.contains("state") && j["state"].is_string()) {
              is_on = (j["state"].get<std::string>() == "ON");
            }
            if (j.contains("tone") && j["tone"].is_string()) {
              params.tone = j["tone"].get<std::string>();
            }
            if (j.contains("duration") && j["duration"].is_number()) {
              params.duration = j["duration"].get<uint32_t>();
            }
            if (j.contains("volume_level") && j["volume_level"].is_number()) {
              params.volume_level = j["volume_level"].get<double>();
            }
          }
#elif __has_include(<ArduinoJson.h>)
          JsonDocument jdoc;
          if (deserializeJson(jdoc, message) == DeserializationError::Ok) {
            if (jdoc["state"].is<const char *>()) {
              is_on = (jdoc["state"].as<std::string>() == "ON");
            }
            if (jdoc["tone"].is<const char *>()) {
              params.tone = jdoc["tone"].as<std::string>();
            }
            if (jdoc["duration"].is<uint32_t>()) {
              params.duration = jdoc["duration"].as<uint32_t>();
            }
            if (jdoc["volume_level"].is<double>()) {
              params.volume_level = jdoc["volume_level"].as<double>();
            }
          }
#endif
          callback(is_on);
          if (is_on && _turn_on_callback) {
            _turn_on_callback(params);
          }
        } else {
          bool on = (message == "ON" || message == "on" || message == "true" || message == "1");
          callback(on);
          if (on && _turn_on_callback) {
            _turn_on_callback(TurnOnParams{});
          }
        }
      });
}

bool HaEntitySiren::onTurnOn(std::function<void(const TurnOnParams &params)> callback) {
  _turn_on_callback = std::move(callback);
  return true;
}

void HaEntitySiren::publishAttributes(const Attributes::Map &attributes) {
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
