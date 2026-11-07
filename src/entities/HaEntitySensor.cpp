// Copyright (c) Johan Böhlin
// SPDX-License-Identifier: MIT

#include "HaEntitySensor.h"
#include <HaUtilities.h>
#include <IJson.h>
#include <algorithm>
#include <cmath>

using namespace homeassistantentities;

HaEntitySensor::HaEntitySensor(HaBridge &ha_bridge, std::string name, std::optional<std::string> child_object_id,
                               const Configuration &configuration)
    : _name(trim(name)), _ha_bridge(ha_bridge), _object_id(configuration.device_class.objectId()),
      _icon(configuration.icon), _expire_after(configuration.expire_after),
      _enabled_by_default(configuration.enabled_by_default), _options(configuration.options),
      _with_attributes(configuration.with_attributes), _force_update(configuration.force_update) {
  _component = configuration.device_class.sensorType() == DeviceClass::SensorType::Sensor ? "sensor" : "binary_sensor";

  if (_component == "sensor" && configuration.suggested_display_precision) {
    _suggested_display_precision = configuration.suggested_display_precision;
  }

  if (configuration.entity_category) {
    auto ec = trim(*configuration.entity_category);
    if (!ec.empty()) {
      _entity_category = ec;
    }
  }
  if (_component == "binary_sensor" && configuration.off_delay) {
    _off_delay = configuration.off_delay;
  }

  if (child_object_id) {
    auto coid = trim(*child_object_id);
    _child_object_id = coid.empty() ? "" : coid;
  } else {
    _child_object_id = "";
  }

  auto dc = configuration.device_class.deviceClass();
  if (dc) {
    auto trimmed_dc = trim(*dc);
    if (!trimmed_dc.empty()) {
      _device_class = trimmed_dc;
    }
  }

  bool forbids_unit =
      (_component != "sensor" || _device_class == "date" || _device_class == "enum" || _device_class == "timestamp" ||
       _device_class == "uptime" || _device_class == "aqi" || _device_class == "ph");
  if (!forbids_unit) {
    if (configuration.unit_of_measurement) {
      _unit_of_measurement = configuration.device_class.unitOfMeasurement(*configuration.unit_of_measurement);
    } else if (configuration.custom_unit_of_measurement) {
      auto custom_unit = trim(*configuration.custom_unit_of_measurement);
      if (!custom_unit.empty()) {
        _unit_of_measurement = custom_unit;
      }
    }
  }

  // Only "sensor" supports state_class in Home Assistant, and each device_class restricts allowed state_classes.
  auto default_sc = configuration.device_class.defaultStateClass();
  if (_component == "sensor" && configuration.state_class && default_sc) {
    auto sc = trim(*configuration.state_class);
    if (!sc.empty()) {
      if (_device_class == "monetary") {
        _state_class = "total";
      } else if (_device_class == "wind_direction") {
        _state_class = "measurement_angle";
      } else if (sc == "measurement" && *default_sc != "measurement") {
        _state_class = *default_sc;
      } else {
        _state_class = sc;
      }
    }
  }
}

bool HaEntitySensor::buildComponentConfiguration(ComponentConfig &out) {
  if (_device_class == "enum" && _options.empty()) {
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

  if (_state_class) {
    doc["state_class"] = *_state_class;
  }
  if (_device_class) {
    doc["device_class"] = *_device_class;
  }
  if (_icon) {
    doc["icon"] = *_icon;
  }
  if (_entity_category) {
    doc["entity_category"] = *_entity_category;
  }
  if (_expire_after) {
    doc["expire_after"] = *_expire_after;
  }
  if (_component == "binary_sensor" && _off_delay) {
    doc["off_delay"] = *_off_delay;
  }
  if (_component == "sensor" && _suggested_display_precision) {
    doc["suggested_display_precision"] = *_suggested_display_precision;
  }
  if (_enabled_by_default) {
    doc["enabled_by_default"] = *_enabled_by_default;
  }
  doc["force_update"] = _force_update;

  if (_unit_of_measurement) {
    doc["unit_of_measurement"] = *_unit_of_measurement;
  }

  if (_component == "sensor" && !_options.empty()) {
    JsonArrayType options_array = createJsonArray(doc, "options");
    for (const std::string &option : _options) {
      addToJsonArray(options_array, option);
    }
  }

  doc["state_topic"] = _ha_bridge.getTopic(HaBridge::TopicType::State, _component, _object_id, _child_object_id);

  if (_with_attributes) {
    doc["json_attributes_topic"] =
        _ha_bridge.getTopic(HaBridge::TopicType::Attributes, _component, _object_id, _child_object_id);
  }

  return true;
}

void HaEntitySensor::publishConfiguration() {
  ComponentConfig cfg;
  if (buildComponentConfiguration(cfg)) {
    _ha_bridge.publishConfiguration(cfg.component, cfg.object_id, cfg.child_object_id, cfg.doc);
  }
}

void HaEntitySensor::unpublishConfiguration() {
  _ha_bridge.unpublishConfiguration(_component, _object_id, _child_object_id);
}

void HaEntitySensor::republishState() {
  if (_value) {
    publish(*_value);
  }
  if (_attributes) {
    publishAttributes(*_attributes);
  }
}

void HaEntitySensor::publish(double value, const Attributes::Map &attributes) {
  if (!std::isfinite(value)) {
    return;
  }
  publish(formatNumber(value), attributes);
}

void HaEntitySensor::publish(const std::string &value, const Attributes::Map &attributes) {
  if (_device_class == "enum" && !_options.empty() && value != "None" &&
      std::find(_options.begin(), _options.end(), value) == _options.end()) {
    return;
  }
  _ha_bridge.publishMessage(_ha_bridge.getTopic(HaBridge::TopicType::State, _component, _object_id, _child_object_id),
                            value);
  _value = value;

  if (!attributes.empty()) {
    publishAttributes(attributes);
  }
}

void HaEntitySensor::publishAttributes(const Attributes::Map &attributes) {
  if (!_with_attributes) {
    return;
  }

  IJsonDocument doc;
  if (Attributes::toJson(doc, attributes)) {
    _attributes = attributes;
    auto message = toJsonString(doc);
    _ha_bridge.publishMessage(
        _ha_bridge.getTopic(HaBridge::TopicType::Attributes, _component, _object_id, _child_object_id), message);
  }
}

void HaEntitySensor::update(double value, const Attributes::Map &attributes) {
  if (!std::isfinite(value)) {
    return;
  }
  update(formatNumber(value), attributes);
}

void HaEntitySensor::update(const std::string &value, const Attributes::Map &attributes) {
  if (!_value || *_value != value) {
    publish(value, {});
  }

  if (!attributes.empty()) {
    updateAttributes(attributes);
  }
}

void HaEntitySensor::updateAttributes(const Attributes::Map &attributes) {
  if (!_attributes || *_attributes != attributes) {
    publishAttributes(attributes);
  }
}
