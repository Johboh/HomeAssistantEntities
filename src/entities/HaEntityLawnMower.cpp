// Copyright (c) Johan Böhlin
// SPDX-License-Identifier: MIT

#include "HaEntityLawnMower.h"
#include <HaUtilities.h>
#include <IJson.h>
#include <utility>

using namespace homeassistantentities;

#define COMPONENT "lawn_mower"
#define OBJECT_ID "lawn_mower"
#define OBJECT_ID_ACTIVITY "activity"
#define OBJECT_ID_START "start_mowing"
#define OBJECT_ID_PAUSE "pause"
#define OBJECT_ID_DOCK "dock"

HaEntityLawnMower::HaEntityLawnMower(HaBridge &ha_bridge, std::string name, std::optional<std::string> child_object_id,
                                     Configuration configuration)
    : _name(homeassistantentities::trim(name)), _ha_bridge(ha_bridge), _child_object_id(child_object_id.value_or("")),
      _configuration(std::move(configuration)) {}

std::string HaEntityLawnMower::activityToString(Activity activity) {
  switch (activity) {
  case Activity::Mowing:
    return "mowing";
  case Activity::Docked:
    return "docked";
  case Activity::Paused:
    return "paused";
  case Activity::Error:
    return "error";
  }
  return "docked";
}

std::optional<HaEntityLawnMower::Activity> HaEntityLawnMower::stringToActivity(const std::string &str) {
  if (str == "mowing")
    return Activity::Mowing;
  if (str == "docked")
    return Activity::Docked;
  if (str == "paused")
    return Activity::Paused;
  if (str == "error")
    return Activity::Error;
  return std::nullopt;
}

bool HaEntityLawnMower::buildComponentConfiguration(ComponentConfig &out) {
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

  doc["retain"] = _configuration.retain;

  doc["activity_state_topic"] =
      _ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, _child_object_id, OBJECT_ID_ACTIVITY);
  doc["start_mowing_command_topic"] =
      _ha_bridge.getTopic(HaBridge::TopicType::Command, COMPONENT, _child_object_id, OBJECT_ID_START);
  doc["pause_command_topic"] =
      _ha_bridge.getTopic(HaBridge::TopicType::Command, COMPONENT, _child_object_id, OBJECT_ID_PAUSE);
  doc["dock_command_topic"] =
      _ha_bridge.getTopic(HaBridge::TopicType::Command, COMPONENT, _child_object_id, OBJECT_ID_DOCK);

  if (_configuration.with_attributes) {
    doc["json_attributes_topic"] =
        _ha_bridge.getTopic(HaBridge::TopicType::Attributes, COMPONENT, _child_object_id, "");
  }

  return true;
}

void HaEntityLawnMower::publishConfiguration() {
  ComponentConfig cfg;
  if (buildComponentConfiguration(cfg)) {
    _ha_bridge.publishConfiguration(cfg.component, cfg.object_id, cfg.child_object_id, cfg.doc);
  }
}

void HaEntityLawnMower::unpublishConfiguration() {
  _ha_bridge.unpublishConfiguration(COMPONENT, OBJECT_ID, _child_object_id);
}

void HaEntityLawnMower::republishState() {
  if (_activity) {
    publish(*_activity);
  }
  if (_attributes) {
    publishAttributes(*_attributes);
  }
}

void HaEntityLawnMower::publish(Activity activity) { publish(activityToString(activity)); }

void HaEntityLawnMower::publish(const std::string &activity) {
  _activity = activity;
  _ha_bridge.publishMessage(
      _ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, _child_object_id, OBJECT_ID_ACTIVITY), activity);
}

void HaEntityLawnMower::update(Activity activity) { update(activityToString(activity)); }

void HaEntityLawnMower::update(const std::string &activity) {
  if (!_activity || *_activity != activity) {
    publish(activity);
  }
}

bool HaEntityLawnMower::onCommand(std::function<void(Action action)> callback) {
  if (!callback) {
    return false;
  }
  bool ok1 = _ha_bridge.remote().subscribe(
      _ha_bridge.getTopic(HaBridge::TopicType::Command, COMPONENT, _child_object_id, OBJECT_ID_START),
      [callback](std::string, std::string) { callback(Action::StartMowing); });
  bool ok2 = _ha_bridge.remote().subscribe(
      _ha_bridge.getTopic(HaBridge::TopicType::Command, COMPONENT, _child_object_id, OBJECT_ID_PAUSE),
      [callback](std::string, std::string) { callback(Action::Pause); });
  bool ok3 = _ha_bridge.remote().subscribe(
      _ha_bridge.getTopic(HaBridge::TopicType::Command, COMPONENT, _child_object_id, OBJECT_ID_DOCK),
      [callback](std::string, std::string) { callback(Action::Dock); });
  return ok1 && ok2 && ok3;
}

void HaEntityLawnMower::publishAttributes(const Attributes::Map &attributes) {
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
