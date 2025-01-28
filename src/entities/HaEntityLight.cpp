#include "HaEntityLight.h"
#include <HaUtilities.h>
#include <IJson.h>
#include <regex>
#include <string>

#define COMPONENT "light"
#define OBJECT_ID "light"
#define OBJECT_ID_RGB "rgb"
#define OBJECT_ID_ONOFF "onoff"
#define OBJECT_ID_EFFECT "effect"
#define OBJECT_ID_BRIGHTNESS "brightness"

HaEntityLight::RGB extractColor(std::string &input) {
  HaEntityLight::RGB color;
  std::regex pattern(R"((\d+),(\d+),(\d+))");
  std::smatch matches;

  if (std::regex_match(input, matches, pattern)) {
    color.r = static_cast<uint8_t>(std::stoi(matches[1]));
    color.g = static_cast<uint8_t>(std::stoi(matches[2]));
    color.b = static_cast<uint8_t>(std::stoi(matches[3]));
  } else {
    // Handle invalid input
    color.r = color.g = color.b = 0;
  }

  return color;
}

// NOTE! We have swapped object ID and child object ID to get a nicer state/command topic path.

HaEntityLight::HaEntityLight(HaBridge &ha_bridge, std::string name, std::string child_object_id,
                             Configuration configuration)
    : _name(homeassistantentities::trim(name)), _ha_bridge(ha_bridge), _child_object_id(child_object_id),
      _configuration(configuration) {}

void HaEntityLight::publishConfiguration() {
  IJsonDocument doc;

  if (!_name.empty()) {
    doc["name"] = _name;
  } else {
    doc["name"] = nullptr;
  }

  doc["retain"] = _configuration.retain;

  doc["state_topic"] = _ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, _child_object_id, OBJECT_ID_ONOFF);
  doc["command_topic"] =
      _ha_bridge.getTopic(HaBridge::TopicType::Command, COMPONENT, _child_object_id, OBJECT_ID_ONOFF);
  if (_configuration.with_brightness) {
    doc["brightness_state_topic"] =
        _ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, _child_object_id, OBJECT_ID_BRIGHTNESS);
    doc["brightness_command_topic"] =
        _ha_bridge.getTopic(HaBridge::TopicType::Command, COMPONENT, _child_object_id, OBJECT_ID_BRIGHTNESS);
  }
  if (_configuration.with_rgb_color) {
    doc["rgb_state_topic"] =
        _ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, _child_object_id, OBJECT_ID_RGB);
    doc["rgb_command_topic"] =
        _ha_bridge.getTopic(HaBridge::TopicType::Command, COMPONENT, _child_object_id, OBJECT_ID_RGB);
  }
  if (!_configuration.effects.empty()) {
    doc["effect_state_topic"] =
        _ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, _child_object_id, OBJECT_ID_EFFECT);
    doc["effect_command_topic"] =
        _ha_bridge.getTopic(HaBridge::TopicType::Command, COMPONENT, _child_object_id, OBJECT_ID_EFFECT);

    JsonArrayType effect_list_array = createJsonArray(doc, "effect_list");
    for (const std::string &effect : _configuration.effects) {
      addToJsonArray(effect_list_array, effect);
    }
  }

  _ha_bridge.publishConfiguration(COMPONENT, OBJECT_ID, _child_object_id, doc);
}

void HaEntityLight::republishState() {
  if (_on) {
    publishIsOn(*_on);
  }
  if (_brightness) {
    publishBrightness(*_brightness);
  }
  if (_rgb) {
    publishRgb(*_rgb);
  }
  if (_effect) {
    publishEffect(*_effect);
  }
}

void HaEntityLight::publishIsOn(bool on) {
  _ha_bridge.publishMessage(
      _ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, _child_object_id, OBJECT_ID_ONOFF),
      std::string(on ? "ON" : "OFF"));
  _on = on;
}

void HaEntityLight::publishBrightness(uint8_t brightness) {
  if (_configuration.with_brightness) {
    _ha_bridge.publishMessage(
        _ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, _child_object_id, OBJECT_ID_BRIGHTNESS),
        std::to_string(brightness));
    _brightness = brightness;
  }
}

void HaEntityLight::publishRgb(RGB rgb) {
  if (_configuration.with_rgb_color) {
    _ha_bridge.publishMessage(
        _ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, _child_object_id, OBJECT_ID_RGB),
        std::to_string(rgb.r) + "," + std::to_string(rgb.g) + "," + std::to_string(rgb.b));
    _rgb = rgb;
  }
}

void HaEntityLight::publishEffect(std::string effect) {
  if (!_configuration.effects.empty()) {
    _ha_bridge.publishMessage(
        _ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, _child_object_id, OBJECT_ID_EFFECT), effect);
    _effect = effect;
  }
}

bool HaEntityLight::setOnOn(std::function<void(bool)> state_callback) {
  return _ha_bridge.remote().subscribe(
      _ha_bridge.getTopic(HaBridge::TopicType::Command, COMPONENT, _child_object_id, OBJECT_ID_ONOFF),
      [state_callback](std::string topic, std::string message) { state_callback(message == "ON"); });
}

bool HaEntityLight::setOnBrightness(std::function<void(uint8_t)> callback) {
  if (!_configuration.with_brightness) {
    return false;
  }

  return _ha_bridge.remote().subscribe(
      _ha_bridge.getTopic(HaBridge::TopicType::Command, COMPONENT, _child_object_id, OBJECT_ID_BRIGHTNESS),
      [callback](std::string topic, std::string message) { callback(std::stoi(message)); });
}

bool HaEntityLight::setOnRgb(std::function<void(RGB)> callback) {
  if (!_configuration.with_rgb_color) {
    return false;
  }

  return _ha_bridge.remote().subscribe(
      _ha_bridge.getTopic(HaBridge::TopicType::Command, COMPONENT, _child_object_id, OBJECT_ID_RGB),
      [callback](std::string topic, std::string message) {
        RGB rgb = extractColor(message);
        callback(rgb);
      });
}

bool HaEntityLight::setOnEffect(std::function<void(std::string)> callback) {
  if (_configuration.effects.empty()) {
    return false;
  }

  return _ha_bridge.remote().subscribe(
      _ha_bridge.getTopic(HaBridge::TopicType::Command, COMPONENT, _child_object_id, OBJECT_ID_EFFECT),
      [callback](std::string topic, std::string message) { callback(message); });
}
