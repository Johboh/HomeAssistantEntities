#include "HaEntityLight.h"
#include "ArduinoJson.h"
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

HaEntityLight::HaEntityLight(HaBridge &ha_bridge, String name, String child_object_id, Capabilities &capabilities)
    : _name(name), _ha_bridge(ha_bridge), _child_object_id(child_object_id), _capabilities(capabilities) {}

void HaEntityLight::publishConfiguration() {
  DynamicJsonDocument doc(1024);
  doc["name"] = _name;
  doc["state_topic"] = _ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, _child_object_id, OBJECT_ID_ONOFF);
  doc["command_topic"] =
      _ha_bridge.getTopic(HaBridge::TopicType::Command, COMPONENT, _child_object_id, OBJECT_ID_ONOFF);
  if (_capabilities.with_brightness) {
    doc["brightness_state_topic"] =
        _ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, _child_object_id, OBJECT_ID_BRIGHTNESS);
    doc["brightness_command_topic"] =
        _ha_bridge.getTopic(HaBridge::TopicType::Command, COMPONENT, _child_object_id, OBJECT_ID_BRIGHTNESS);
  }
  if (_capabilities.with_rgb_color) {
    doc["rgb_state_topic"] =
        _ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, _child_object_id, OBJECT_ID_RGB);
    doc["rgb_command_topic"] =
        _ha_bridge.getTopic(HaBridge::TopicType::Command, COMPONENT, _child_object_id, OBJECT_ID_RGB);
  }
  if (!_capabilities.effects.empty()) {
    doc["effect_state_topic"] =
        _ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, _child_object_id, OBJECT_ID_EFFECT);
    doc["effect_command_topic"] =
        _ha_bridge.getTopic(HaBridge::TopicType::Command, COMPONENT, _child_object_id, OBJECT_ID_EFFECT);
    auto effects = doc["effect_list"].to<JsonArray>();
    for (std::set<String>::iterator it = _capabilities.effects.begin(); it != _capabilities.effects.end(); ++it) {
      effects.add(*it);
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
      String(on ? "ON" : "OFF"));
  _on = on;
}

void HaEntityLight::publishBrightness(uint8_t brightness) {
  if (_capabilities.with_brightness) {
    _ha_bridge.publishMessage(
        _ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, _child_object_id, OBJECT_ID_BRIGHTNESS),
        String(brightness));
    _brightness = brightness;
  }
}

void HaEntityLight::publishRgb(RGB rgb) {
  if (_capabilities.with_rgb_color) {
    _ha_bridge.publishMessage(
        _ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, _child_object_id, OBJECT_ID_RGB),
        String(rgb.r) + "," + String(rgb.g) + "," + String(rgb.b));
    _rgb = rgb;
  }
}

void HaEntityLight::publishEffect(String &effect) {
  if (!_capabilities.effects.empty()) {
    _ha_bridge.publishMessage(
        _ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, _child_object_id, OBJECT_ID_EFFECT), effect);
    _effect = effect;
  }
}

bool HaEntityLight::setOnOn(std::function<void(bool)> state_callback) {
  return _ha_bridge.remote().subscribe(
      _ha_bridge.getTopic(HaBridge::TopicType::Command, COMPONENT, _child_object_id, OBJECT_ID_ONOFF),
      [state_callback](const char *topic, const char *message) { state_callback(String(message) == "ON"); });
}

bool HaEntityLight::setOnBrightness(std::function<void(uint8_t)> callback) {
  if (!_capabilities.with_brightness) {
    Serial.println("HaEntityLight: Trying to setup brightness callback but no brightness capability available.");
    return false;
  }

  return _ha_bridge.remote().subscribe(
      _ha_bridge.getTopic(HaBridge::TopicType::Command, COMPONENT, _child_object_id, OBJECT_ID_BRIGHTNESS),
      [callback](const char *topic, const char *message) { callback(String(message).toInt()); });
}

bool HaEntityLight::setOnRgb(std::function<void(RGB)> callback) {
  if (!_capabilities.with_rgb_color) {
    Serial.println("HaEntityLight: Trying to setup RGB callback but no RGB capability available.");
    return false;
  }

  return _ha_bridge.remote().subscribe(
      _ha_bridge.getTopic(HaBridge::TopicType::Command, COMPONENT, _child_object_id, OBJECT_ID_RGB),
      [callback](const char *topic, const char *message) {
        auto str = std::string(message);
        RGB rgb = extractColor(str);
        callback(rgb);
      });
}

bool HaEntityLight::setOnEffect(std::function<void(String)> callback) {
  if (_capabilities.effects.empty()) {
    Serial.println("HaEntityLight: Trying to setup effect callback but no effect capability available.");
    return false;
  }

  return _ha_bridge.remote().subscribe(
      _ha_bridge.getTopic(HaBridge::TopicType::Command, COMPONENT, _child_object_id, OBJECT_ID_EFFECT),
      [callback](const char *topic, const char *message) { callback(message); });
}
