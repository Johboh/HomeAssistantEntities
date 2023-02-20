#include "HaEntityLight.h"
#include "ArduinoJson.h"

#define COMPONENT "light"
#define OBJECT_ID "light"
#define OBJECT_ID_ONOFF "onoff"
#define OBJECT_ID_BRIGHTNESS "brightness"

// NOTE! We have swapped object ID and child object ID to get a nicer state/command topic path.

HaEntityLight::HaEntityLight(HaBridge &ha_bridge, String name, String child_object_id, bool support_brightness)
    : _name(name), _ha_bridge(ha_bridge), _child_object_id(child_object_id), _support_brightness(support_brightness) {}

void HaEntityLight::publishConfiguration() {
  DynamicJsonDocument doc(256);
  doc["name"] = _name;
  doc["state_topic"] = _ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, _child_object_id, OBJECT_ID_ONOFF);
  doc["command_topic"] =
      _ha_bridge.getTopic(HaBridge::TopicType::Command, COMPONENT, _child_object_id, OBJECT_ID_ONOFF);
  if (_support_brightness) {
    doc["brightness_state_topic"] =
        _ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, _child_object_id, OBJECT_ID_BRIGHTNESS);
    doc["brightness_command_topic"] =
        _ha_bridge.getTopic(HaBridge::TopicType::Command, COMPONENT, _child_object_id, OBJECT_ID_BRIGHTNESS);
  }
  _ha_bridge.publishConfiguration(COMPONENT, OBJECT_ID, _child_object_id, doc);
}

void HaEntityLight::republishState() { publishLight(_on, _brightness); }

void HaEntityLight::publishLight(std::optional<bool> on, std::optional<uint8_t> brightness) {
  if (on) {
    _ha_bridge.publishMessage(
        _ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, _child_object_id, OBJECT_ID_ONOFF),
        String(*on ? "ON" : "OFF"));
    _on = on;
  }
  if (_support_brightness && brightness) {
    _ha_bridge.publishMessage(
        _ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, _child_object_id, OBJECT_ID_BRIGHTNESS),
        String(*brightness));
    _brightness = brightness;
  }
}

bool HaEntityLight::setOnState(std::function<void(bool)> state_callback) {
  return _ha_bridge.remote().subscribe(
      _ha_bridge.getTopic(HaBridge::TopicType::Command, COMPONENT, _child_object_id, OBJECT_ID_ONOFF),
      [state_callback](const char *topic, const char *message) { state_callback(String(message) == "ON"); });
}

bool HaEntityLight::setOnBrightness(std::function<void(uint8_t)> brightness_callback) {
  if (!_support_brightness) {
    Serial.println("HaEntityLight: Trying to setup brightness callback but no brightness capability available.");
    return false;
  }

  return _ha_bridge.remote().subscribe(
      _ha_bridge.getTopic(HaBridge::TopicType::Command, COMPONENT, _child_object_id, OBJECT_ID_BRIGHTNESS),
      [brightness_callback](const char *topic, const char *message) { brightness_callback(String(message).toInt()); });
}
