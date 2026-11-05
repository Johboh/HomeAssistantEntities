// Copyright (c) Johan Böhlin
// SPDX-License-Identifier: MIT

#include "HaEntitySensor.h"
#include <HaUtilities.h>
#include <IJson.h>
#include <algorithm>

using namespace homeassistantentities;

HaEntitySensor::HaEntitySensor(HaBridge &ha_bridge, std::string name, std::optional<std::string> child_object_id,
                               const Configuration &configuration)
    : _name(trim(name)), _ha_bridge(ha_bridge), _object_id(configuration.device_class.objectId()),
      _icon(configuration.icon), _options(configuration.options), _with_attributes(configuration.with_attributes),
      _force_update(configuration.force_update) {
  _component = configuration.device_class.sensorType() == DeviceClass::SensorType::Sensor ? "sensor" : "binary_sensor";

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

void HaEntitySensor::publishConfiguration() {
  if (_device_class == "enum" && _options.empty()) {
    return;
  }

  IJsonDocument doc;

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

  _ha_bridge.publishConfiguration(_component, _object_id, _child_object_id, doc);
}

void HaEntitySensor::republishState() {
  if (_value) {
    publishValue(*_value);
  }
  if (_attributes) {
    publishAttributes(*_attributes);
  }
}

void HaEntitySensor::publishValue(double value, const Attributes::Map &attributes) {
  publishValue(formatNumber(value), attributes);
}

void HaEntitySensor::publishValue(const std::string &value, const Attributes::Map &attributes) {
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

void HaEntitySensor::updateValue(double value, const Attributes::Map &attributes) {
  updateValue(formatNumber(value), attributes);
}

void HaEntitySensor::updateValue(const std::string &value, const Attributes::Map &attributes) {
  if (!_value || *_value != value) {
    publishValue(value, {});
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
