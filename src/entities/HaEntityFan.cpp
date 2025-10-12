#include "HaEntityFan.h"
#include <HaUtilities.h>
#include <IJson.h>

#define COMPONENT "fan"
#define OBJECT_ID "fan"
#define OBJECT_ID_ONOFF "onoff"
#define OBJECT_ID_SPEED "speed"
#define OBJECT_ID_PRESET "preset"
#define OBJECT_ID_DIRECTION "direction"
#define OBJECT_ID_OSCILLATION "oscillation"

HaEntityFan::HaEntityFan(HaBridge &ha_bridge, std::string name, std::string child_object_id,
                         Configuration configuration)
    : _name(homeassistantentities::trim(name)), _ha_bridge(ha_bridge), _child_object_id(child_object_id),
      _configuration(configuration) {}

void HaEntityFan::publishConfiguration() {
  IJsonDocument doc;

  if (!_name.empty()) {
    doc["name"] = _name;
  } else {
    doc["name"] = nullptr;
  }

  doc["platform"] = "fan";
  doc["force_update"] = _configuration.force_update;
  doc["retain"] = _configuration.retain;

  if (_configuration.with_direction) {
    doc["direction_state_topic"] =
        _ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, _child_object_id, OBJECT_ID_DIRECTION);
    doc["direction_command_topic"] =
        _ha_bridge.getTopic(HaBridge::TopicType::Command, COMPONENT, _child_object_id, OBJECT_ID_DIRECTION);
  }

  if (_configuration.with_oscillation) {
    doc["oscillation_state_topic"] =
        _ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, _child_object_id, OBJECT_ID_OSCILLATION);
    doc["oscillation_command_topic"] =
        _ha_bridge.getTopic(HaBridge::TopicType::Command, COMPONENT, _child_object_id, OBJECT_ID_OSCILLATION);
  }

  if (_configuration.with_speed) {
    doc["percentage_state_topic"] =
        _ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, _child_object_id, OBJECT_ID_SPEED);
    doc["percentage_command_topic"] =
        _ha_bridge.getTopic(HaBridge::TopicType::Command, COMPONENT, _child_object_id, OBJECT_ID_SPEED);
    doc["speed_range_min"] = _configuration.speed_range_min;
    doc["speed_range_max"] = _configuration.speed_range_max;
  }

  if (!_configuration.presets.empty()) {
    JsonArrayType preset_modes_array = createJsonArray(doc, "preset_modes");
    for (const std::string &preset : _configuration.presets) {
      addToJsonArray(preset_modes_array, preset);
    }
    doc["preset_mode_state_topic"] =
        _ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, _child_object_id, OBJECT_ID_PRESET);
    doc["preset_mode_command_topic"] =
        _ha_bridge.getTopic(HaBridge::TopicType::Command, COMPONENT, _child_object_id, OBJECT_ID_PRESET);
  }

  doc["state_topic"] = _ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, _child_object_id, OBJECT_ID_ONOFF);
  doc["command_topic"] =
      _ha_bridge.getTopic(HaBridge::TopicType::Command, COMPONENT, _child_object_id, OBJECT_ID_ONOFF);

  _ha_bridge.publishConfiguration(COMPONENT, OBJECT_ID, _child_object_id, doc);
}

void HaEntityFan::republishState() {
  if (_on) {
    publishIsOn(*_on);
  }
  if (_speed) {
    publishSpeed(*_speed);
  }
  if (_preset) {
    publishPreset(*_preset);
  }
  if (_direction) {
    publishDirection(*_direction);
  }
  if (_oscillation) {
    publishOscillation(*_oscillation);
  }
}

void HaEntityFan::publishDirection(std::string direction) {
  if (!_configuration.with_direction) {
    return;
  }
  _direction = direction;
  _ha_bridge.publishMessage(
      _ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, _child_object_id, OBJECT_ID_DIRECTION), direction);
}

void HaEntityFan::updateDirection(std::string direction) {
  if (!_direction || *_direction != direction) {
    publishDirection(direction);
  }
}

bool HaEntityFan::setOnDirection(std::function<void(std::string)> callback) {
  if (!_configuration.with_direction) {
    return false;
  }
  return _ha_bridge.remote().subscribe(
      _ha_bridge.getTopic(HaBridge::TopicType::Command, COMPONENT, _child_object_id, OBJECT_ID_DIRECTION),
      [callback](std::string, std::string message) { callback(message); });
}

//--------------------------------------

void HaEntityFan::publishOscillation(bool oscillation) {
  if (!_configuration.with_oscillation) {
    return;
  }
  _oscillation = oscillation;
  _ha_bridge.publishMessage(
      _ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, _child_object_id, OBJECT_ID_OSCILLATION),
      oscillation ? "oscillate_on" : "oscillate_off");
}

void HaEntityFan::updateOscillation(bool oscillation) {
  if (!_oscillation || *_oscillation != oscillation) {
    publishOscillation(oscillation);
  }
}

bool HaEntityFan::setOnOscillation(std::function<void(bool)> callback) {
  if (!_configuration.with_oscillation)
    return false;
  return _ha_bridge.remote().subscribe(
      _ha_bridge.getTopic(HaBridge::TopicType::Command, COMPONENT, _child_object_id, OBJECT_ID_OSCILLATION),
      [callback](std::string, std::string message) {
        callback(message == "ON" || message == "on" || message == "true" || message == "1" ||
                 message == "oscillate_on");
      });
}

//--------------------------------------

void HaEntityFan::publishSpeed(uint32_t speed) {
  if (!_configuration.with_speed) {
    return;
  }
  speed = std::clamp(speed, _configuration.speed_range_min, _configuration.speed_range_max);
  _speed = speed;
  _ha_bridge.publishMessage(
      _ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, _child_object_id, OBJECT_ID_SPEED),
      std::to_string(speed));
}

void HaEntityFan::updateSpeed(uint32_t speed) {
  if (!_speed || *_speed != speed) {
    publishSpeed(speed);
  }
}

bool HaEntityFan::setOnSpeed(std::function<void(uint32_t)> callback) {
  if (!_configuration.with_speed) {
    return false;
  }
  return _ha_bridge.remote().subscribe(
      _ha_bridge.getTopic(HaBridge::TopicType::Command, COMPONENT, _child_object_id, OBJECT_ID_SPEED),
      [callback, config = _configuration](std::string, std::string message) {
        uint32_t speed = std::clamp(static_cast<uint32_t>(std::atoi(message.c_str())), config.speed_range_min,
                                    config.speed_range_max);
        callback(speed);
      });
}

//--------------------------------------

void HaEntityFan::publishPreset(std::string preset) {
  auto presets = _configuration.presets;
  if (presets.empty()) {
    return;
  }
  // Check if preset is in the list of allowed presets
  if (std::find(presets.begin(), presets.end(), preset) == presets.end()) {
    return;
  }
  _preset = preset;
  _ha_bridge.publishMessage(
      _ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, _child_object_id, OBJECT_ID_PRESET), preset);
}

void HaEntityFan::updatePreset(std::string preset) {
  if (!_preset || *_preset != preset) {
    publishPreset(preset);
  }
}

bool HaEntityFan::setOnPreset(std::function<void(std::string)> callback) {
  if (_configuration.presets.empty()) {
    return false;
  }
  return _ha_bridge.remote().subscribe(
      _ha_bridge.getTopic(HaBridge::TopicType::Command, COMPONENT, _child_object_id, OBJECT_ID_PRESET),
      [callback](std::string, std::string message) { callback(message); });
}

//--------------------------------------

void HaEntityFan::publishIsOn(bool on) {
  _on = on;
  _ha_bridge.publishMessage(
      _ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, _child_object_id, OBJECT_ID_ONOFF), on ? "ON" : "OFF");
}

void HaEntityFan::updateIsOn(bool on) {
  if (!_on || *_on != on) {
    publishIsOn(on);
  }
}

bool HaEntityFan::setOnState(std::function<void(bool)> callback) {
  return _ha_bridge.remote().subscribe(
      _ha_bridge.getTopic(HaBridge::TopicType::Command, COMPONENT, _child_object_id, OBJECT_ID_ONOFF),
      [callback](std::string, std::string message) {
        callback(message == "ON" || message == "on" || message == "true" || message == "1");
      });
}