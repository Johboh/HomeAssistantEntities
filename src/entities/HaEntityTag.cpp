// Copyright (c) Johan Böhlin
// SPDX-License-Identifier: MIT

#include "HaEntityTag.h"
#include <HaUtilities.h>
#include <IJson.h>
#include <utility>

using namespace homeassistantentities;

HaEntityTag::HaEntityTag(HaBridge &ha_bridge, std::string name, std::optional<std::string> child_object_id,
                         Configuration configuration)
    : _name(homeassistantentities::trim(name)), _ha_bridge(ha_bridge), _child_object_id(child_object_id.value_or("")),
      _configuration(std::move(configuration)) {}

void HaEntityTag::publishTag(const std::string &tag_id) {
  if (tag_id.empty()) {
    return;
  }
  IJsonDocument doc;
  doc["tag_id"] = tag_id;
  std::string device_id = sanitizePath(_ha_bridge.nodeId());
  if (!_child_object_id.empty()) {
    device_id += "_" + sanitizePath(_child_object_id);
  }
  doc["device_id"] = device_id;

  std::string topic = _configuration.tag_prefix + "/tag/" + device_id + "/tag_scanned";
  _ha_bridge.publishMessage(topic, toJsonString(doc));
}
