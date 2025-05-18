#include "HaEntityNumber.h"
#include <HaUtilities.h>
#include <IJson.h>

#define COMPONENT "number"

HaEntityNumber::HaEntityNumber(HaBridge &ha_bridge, std::string name, std::string object_id,
                               Configuration configuration)
    : _name(homeassistantentities::trim(name)), _ha_bridge(ha_bridge), _object_id(object_id),
      _configuration(configuration) {}

void HaEntityNumber::publishConfiguration() {
  IJsonDocument doc;

  if (!_name.empty()) {
    doc[_ha_bridge.useAbbreviations() ? "name" : "name"] = _name;
  } else {
    doc[_ha_bridge.useAbbreviations() ? "name" : "name"] = nullptr;
  }

  doc[_ha_bridge.useAbbreviations() ? "p" : "platform"] = "number";
  doc[_ha_bridge.useAbbreviations() ? "min" : "min"] = _configuration.min_value;
  doc[_ha_bridge.useAbbreviations() ? "max" : "max"] = _configuration.max_value;
  doc[_ha_bridge.useAbbreviations() ? "frc_upd" : "force_update"] = _configuration.force_update;
  doc[_ha_bridge.useAbbreviations() ? "ret" : "retain"] = _configuration.retain;

  if (!_configuration.unit.empty()) {
    doc[_ha_bridge.useAbbreviations() ? "unit_of_meas" : "unit_of_measurement"] = _configuration.unit;
  }

  if (!_configuration.device_class.empty()) {
    doc[_ha_bridge.useAbbreviations() ? "dev_cla" : "device_class"] = _configuration.device_class;
  }

  doc[_ha_bridge.useAbbreviations() ? "stat_t" : "state_topic"] =
      _ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, _object_id);
  doc[_ha_bridge.useAbbreviations() ? "cmd_t" : "command_topic"] =
      _ha_bridge.getTopic(HaBridge::TopicType::Command, COMPONENT, _object_id);

  _ha_bridge.publishConfiguration(COMPONENT, _object_id, "", doc);
}

void HaEntityNumber::republishState() {
  if (_number) {
    publishNumber(*_number);
  }
}

void HaEntityNumber::publishNumber(float number) {
  // numbered == OFF
  _ha_bridge.publishMessage(_ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, _object_id),
                            std::to_string(number));
  _number = number;
}

void HaEntityNumber::updateNumber(float number) {
  if (!_number || *_number != number) {
    publishNumber(number);
  }
}

bool HaEntityNumber::setOnNumber(std::function<void(float)> callback) {
  return _ha_bridge.remote().subscribe(
      _ha_bridge.getTopic(HaBridge::TopicType::Command, COMPONENT, _object_id),
      [callback](std::string topic, std::string message) { callback(std::stof(message)); });
}