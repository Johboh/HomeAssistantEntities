// Copyright (c) Johan Böhlin
// SPDX-License-Identifier: MIT

#include "HaEntityActuator.h"
#include <HaUtilities.h>
#include <IJson.h>
#include <algorithm>
#include <cmath>
#include <utility>

using namespace homeassistantentities;

HaEntityActuator::HaEntityActuator(HaBridge &ha_bridge, std::string name, const Configuration &configuration)
    : HaEntityActuator(ha_bridge, std::move(name), std::nullopt, configuration) {}

HaEntityActuator::HaEntityActuator(HaBridge &ha_bridge, std::string name, std::optional<std::string> child_object_id,
                                   const Configuration &configuration)
    : _name(trim(name)), _ha_bridge(ha_bridge), _sub_topic(configuration.sub_topic),
      _suggested_display_precision(configuration.suggested_display_precision),
      _enabled_by_default(configuration.enabled_by_default), _options(configuration.options),
      _extra_configuration(configuration.extra_configuration), _with_state_topic(configuration.with_state_topic),
      _with_command_topic(configuration.with_command_topic), _with_attributes(configuration.with_attributes),
      _retain(configuration.retain), _emit_retain_when_false(configuration.emit_retain_when_false),
      _optimistic(configuration.optimistic) {
  if (configuration.component_override) {
    auto comp = trim(*configuration.component_override);
    _component = comp.empty() ? configuration.device_class.component() : comp;
  } else {
    _component = configuration.device_class.component();
  }

  if (configuration.object_id_override) {
    auto oid = trim(*configuration.object_id_override);
    _object_id = oid.empty() ? configuration.device_class.objectId() : oid;
  } else {
    _object_id = configuration.device_class.objectId();
  }

  if (child_object_id) {
    auto coid = trim(*child_object_id);
    _child_object_id = coid.empty() ? "" : coid;
  } else {
    _child_object_id = "";
  }

  if (configuration.device_class_override) {
    auto dc = trim(*configuration.device_class_override);
    if (!dc.empty()) {
      _device_class = dc;
    }
  } else {
    auto dc = configuration.device_class.deviceClass();
    if (dc) {
      auto trimmed_dc = trim(*dc);
      if (!trimmed_dc.empty()) {
        _device_class = trimmed_dc;
      }
    }
  }

  if (configuration.unit_of_measurement) {
    _unit_of_measurement = configuration.device_class.unitOfMeasurement(*configuration.unit_of_measurement);
  } else if (configuration.custom_unit_of_measurement) {
    auto custom_unit = trim(*configuration.custom_unit_of_measurement);
    if (!custom_unit.empty()) {
      _unit_of_measurement = custom_unit;
    }
  }

  if (configuration.icon) {
    auto ic = trim(*configuration.icon);
    if (!ic.empty()) {
      _icon = ic;
    }
  }

  if (configuration.entity_category) {
    auto ec = trim(*configuration.entity_category);
    if (!ec.empty()) {
      _entity_category = ec;
    }
  }
}

std::string HaEntityActuator::resolveTopic(HaBridge::TopicType type) const {
  if (_sub_topic) {
    return _ha_bridge.getTopic(type, _component, _child_object_id, *_sub_topic);
  }
  return _ha_bridge.getTopic(type, _component, _object_id, _child_object_id);
}

std::string HaEntityActuator::stateTopic() const { return resolveTopic(HaBridge::TopicType::State); }

std::string HaEntityActuator::commandTopic() const { return resolveTopic(HaBridge::TopicType::Command); }

std::string HaEntityActuator::attributesTopic() const { return resolveTopic(HaBridge::TopicType::Attributes); }

bool HaEntityActuator::buildComponentConfiguration(ComponentConfig &out) {
  if (_component.empty()) {
    return false;
  }

  out.component = _component;
  out.object_id = _object_id;
  out.child_object_id = _child_object_id;
  out.bridge = &_ha_bridge;

  auto &doc = out.doc;
  if (!_name.empty()) {
    doc["name"] = _name;
  } else {
    doc["name"] = nullptr;
  }

  if (_device_class) {
    doc["device_class"] = *_device_class;
  }
  if (_unit_of_measurement) {
    doc["unit_of_measurement"] = *_unit_of_measurement;
  }
  if (_icon) {
    doc["icon"] = *_icon;
  }
  if (_entity_category) {
    doc["entity_category"] = *_entity_category;
  }
  if (_optimistic) {
    doc["optimistic"] = true;
  }
  if (_suggested_display_precision) {
    doc["suggested_display_precision"] = *_suggested_display_precision;
  }
  if (_enabled_by_default) {
    doc["enabled_by_default"] = *_enabled_by_default;
  }
  if (_with_command_topic && (_retain || _emit_retain_when_false)) {
    doc["retain"] = _retain;
  }

  if (_component == "select" || !_options.empty()) {
    JsonArrayType options_array = createJsonArray(doc, "options");
    for (const std::string &option : _options) {
      addToJsonArray(options_array, option);
    }
  }

  if (_with_state_topic) {
    doc["state_topic"] = stateTopic();
  }
  if (_with_command_topic) {
    doc["command_topic"] = commandTopic();
  }
  if (_with_attributes) {
    doc["json_attributes_topic"] = attributesTopic();
  }

  if (_extra_configuration) {
    _extra_configuration(doc);
  }

  return true;
}

void HaEntityActuator::publishConfiguration() {
  ComponentConfig cfg;
  if (buildComponentConfiguration(cfg)) {
    _ha_bridge.publishConfiguration(cfg.component, cfg.object_id, cfg.child_object_id, cfg.doc);
  }
}

void HaEntityActuator::unpublishConfiguration() {
  _ha_bridge.unpublishConfiguration(_component, _object_id, _child_object_id);
}

void HaEntityActuator::republishState() {
  if (_value) {
    publish(*_value);
  }
  if (_attributes) {
    publishAttributes(*_attributes);
  }
}

void HaEntityActuator::publish(double value, const Attributes::Map &attributes) {
  if (!std::isfinite(value)) {
    return;
  }
  publish(formatNumber(value), attributes);
}

void HaEntityActuator::update(double value, const Attributes::Map &attributes) {
  if (!std::isfinite(value)) {
    return;
  }
  update(formatNumber(value), attributes);
}

void HaEntityActuator::publish(const std::string &value, const Attributes::Map &attributes) {
  if (!_with_state_topic) {
    return;
  }
  if ((_component == "select" || !_options.empty()) &&
      std::find(_options.begin(), _options.end(), value) == _options.end()) {
    return;
  }
  _ha_bridge.publishMessage(stateTopic(), value);
  _value = value;

  if (!attributes.empty()) {
    publishAttributes(attributes);
  }
}

void HaEntityActuator::update(const std::string &value, const Attributes::Map &attributes) {
  if (!_with_state_topic) {
    return;
  }
  if ((_component == "select" || !_options.empty()) &&
      std::find(_options.begin(), _options.end(), value) == _options.end()) {
    return;
  }
  if (!_value || *_value != value) {
    publish(value, {});
  }

  if (!attributes.empty()) {
    updateAttributes(attributes);
  }
}

void HaEntityActuator::publishAttributes(const Attributes::Map &attributes) {
  if (!_with_attributes) {
    return;
  }

  IJsonDocument doc;
  if (Attributes::toJson(doc, attributes)) {
    _attributes = attributes;
    auto message = toJsonString(doc);
    _ha_bridge.publishMessage(attributesTopic(), message);
  }
}

void HaEntityActuator::updateAttributes(const Attributes::Map &attributes) {
  if (!_attributes || *_attributes != attributes) {
    publishAttributes(attributes);
  }
}

bool HaEntityActuator::onCommand(std::function<void(const std::string &)> callback) {
  if (!_with_command_topic || !callback) {
    return false;
  }
  auto options = _options;
  bool enforce_options = (_component == "select" || !_options.empty());
  return _ha_bridge.remote().subscribe(
      commandTopic(), [callback, options, enforce_options](std::string, std::string message) {
        if (enforce_options && std::find(options.begin(), options.end(), message) == options.end()) {
          return;
        }
        callback(message);
      });
}
