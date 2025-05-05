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
#define OBJECT_ID_COLOR_TEMPERATURE "color_temperature"

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
    doc[_ha_bridge.useAbbreviations() ? "name" : "name"] = _name;
  } else {
    doc[_ha_bridge.useAbbreviations() ? "name" : "name"] = nullptr;
  }

  doc[_ha_bridge.useAbbreviations() ? "ret" : "retain"] = _configuration.retain;

  doc[_ha_bridge.useAbbreviations() ? "stat_t" : "state_topic"] =
      _ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, _child_object_id, OBJECT_ID_ONOFF);
  doc[_ha_bridge.useAbbreviations() ? "cmd_t" : "command_topic"] =
      _ha_bridge.getTopic(HaBridge::TopicType::Command, COMPONENT, _child_object_id, OBJECT_ID_ONOFF);
  if (_configuration.with_brightness) {
    doc[_ha_bridge.useAbbreviations() ? "bri_stat_t" : "brightness_state_topic"] =
        _ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, _child_object_id, OBJECT_ID_BRIGHTNESS);
    doc[_ha_bridge.useAbbreviations() ? "bri_cmd_t" : "brightness_command_topic"] =
        _ha_bridge.getTopic(HaBridge::TopicType::Command, COMPONENT, _child_object_id, OBJECT_ID_BRIGHTNESS);
  }
  if (_configuration.with_color_temperature != Configuration::ColorTemperature::None) {
    doc[_ha_bridge.useAbbreviations() ? "clr_temp_stat_t" : "color_temp_state_topic"] =
        _ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, _child_object_id, OBJECT_ID_COLOR_TEMPERATURE);
    doc[_ha_bridge.useAbbreviations() ? "clr_temp_cmd_t" : "color_temp_command_topic"] =
        _ha_bridge.getTopic(HaBridge::TopicType::Command, COMPONENT, _child_object_id, OBJECT_ID_COLOR_TEMPERATURE);
    if (_configuration.with_color_temperature == Configuration::ColorTemperature::Kelvin) {
      doc[_ha_bridge.useAbbreviations() ? "clr_temp_k" : "color_temp_kelvin"] = true;
    }
  }
  if (_configuration.with_rgb_color) {
    doc[_ha_bridge.useAbbreviations() ? "rgb_stat_t" : "rgb_state_topic"] =
        _ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, _child_object_id, OBJECT_ID_RGB);
    doc[_ha_bridge.useAbbreviations() ? "rgb_cmd_t" : "rgb_command_topic"] =
        _ha_bridge.getTopic(HaBridge::TopicType::Command, COMPONENT, _child_object_id, OBJECT_ID_RGB);
  }
  if (!_configuration.effects.empty()) {
    doc[_ha_bridge.useAbbreviations() ? "fx_stat_t" : "effect_state_topic"] =
        _ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, _child_object_id, OBJECT_ID_EFFECT);
    doc[_ha_bridge.useAbbreviations() ? "fx_cmd_t" : "effect_command_topic"] =
        _ha_bridge.getTopic(HaBridge::TopicType::Command, COMPONENT, _child_object_id, OBJECT_ID_EFFECT);

    JsonArrayType effect_list_array = createJsonArray(doc, _ha_bridge.useAbbreviations() ? "fx_list" : "effect_list");
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
  if (_color_temperature) {
    publishColorTemperature(*_color_temperature);
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

void HaEntityLight::publishColorTemperature(uint16_t temperature) {
  if (_configuration.with_color_temperature != Configuration::ColorTemperature::None) {
    _ha_bridge.publishMessage(
        _ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, _child_object_id, OBJECT_ID_COLOR_TEMPERATURE),
        std::to_string(temperature));
    _color_temperature = temperature;
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

void HaEntityLight::updateIsOn(bool on) {
  if (!_on || *_on != on) {
    publishIsOn(on);
  }
}

void HaEntityLight::updateBrightness(uint8_t brightness) {
  if (!_brightness || *_brightness != brightness) {
    publishBrightness(brightness);
  }
}

void HaEntityLight::updateColorTemperature(uint16_t temperature) {
  if (!_color_temperature || *_color_temperature != temperature) {
    publishColorTemperature(temperature);
  }
}

void HaEntityLight::updateRgb(RGB rgb) {
  if (!_rgb || *_rgb != rgb) {
    publishRgb(rgb);
  }
}

void HaEntityLight::updateEffect(std::string effect) {
  if (!_effect || *_effect != effect) {
    publishEffect(effect);
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

bool HaEntityLight::setOnColorTemperature(std::function<void(uint16_t)> callback) {
  if (_configuration.with_color_temperature == Configuration::ColorTemperature::None) {
    return false;
  }

  return _ha_bridge.remote().subscribe(
      _ha_bridge.getTopic(HaBridge::TopicType::Command, COMPONENT, _child_object_id, OBJECT_ID_COLOR_TEMPERATURE),
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
