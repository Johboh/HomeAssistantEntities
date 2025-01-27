#include <Arduino.h>
// This is for the Arduino IDE, where we always build with ArduinoJson. arduino-cli will not build/include libraries
// that are not included anywhere. So we must include ArduinoJson.h so its available for IJson.h later.
// For Platform IO, this is not the case and these examples are built both with ArduinoJson and nlohmann-json.
#ifndef PLATFORMIO
#include <ArduinoJson.h>
#endif
#include <HaBridge.h>
// This is here for CI only. You can remove this include and include ArduinoJson.h or nlohmann/json.hpp directly.
#include <IJson.h>
#include <MQTTRemote.h>
#include <entities/HaEntityBrightness.h>
#include <entities/HaEntitySensor.h>
#include <entities/HaEntityTemperature.h>
#ifdef ESP32
#include <WiFi.h>
#elif ESP8266
#include <ESP8266WiFi.h>
#else
#error "Unsupported hardware. Sorry!"
#endif

/**
 * This example setup a device in Home Assistant with two sensors,
 * a brightness sensor and a temperature sensor.
 */

const char wifi_ssid[] = "my-wifi-ssid";
const char wifi_password[] = "my-wifi-password";
const char mqtt_client_id[] = "my-client";
const char mqtt_host[] = "192.168.1.1";
const char mqtt_username[] = "my-username";
const char mqtt_password[] = "my-password";

// Information about this device.
// All these keys will be added to a "device" key in the Home Assistant configuration for each entity.
// Only a flat layout structure is supported, no nesting.
// We call the setupJsonForThisDevice() from the ardunio setup() function to populate the Json document.
// IJsonDocument can be replaced with nlohmann-json::json or ArduinoJson::JsonDocument
IJsonDocument _json_this_device_doc;
void setupJsonForThisDevice() {
  _json_this_device_doc["identifiers"] = "my_hardware_" + std::string(mqtt_client_id);
  _json_this_device_doc["name"] = "Livingroom";
  _json_this_device_doc["sw_version"] = "1.0.0";
  _json_this_device_doc["model"] = "my_hardware";
  _json_this_device_doc["manufacturer"] = "custom inc.";
}
MQTTRemote _mqtt_remote(mqtt_client_id, mqtt_host, 1883, mqtt_username, mqtt_password);

// Create the Home Assistant bridge. This is shared across all entities.
// We only have one per device/hardware. In our example, the name of our device is "livingroom".
// See constructor of HaBridge for more documentation.
HaBridge ha_bridge(_mqtt_remote, "livingroom", _json_this_device_doc);

// Create the three sensors with the "Human readable" strings. This what will show up in Home Assistant.
HaEntityBrightness _ha_entity_brightness(ha_bridge, "brightness");

// For multiple sensors with the same time for the same device, we need to add a child object id to separate them (third
// parameter).
HaEntityTemperature _ha_entity_temperature_inside(ha_bridge, "temperature inside", "inside");
HaEntityTemperature _ha_entity_temperature_outside(ha_bridge, "temperature outside", "outside");

// Precipitation sensor using the generic sensor, as there is no specific class for precipitation (yet).
homeassistantentities::Sensor::Precipitation _precipitation;
HaEntitySensor
    _ha_entity_generic_sensor(ha_bridge, "precipitation", std::nullopt,
                              {
                                  .device_class = _precipitation,
                                  .unit_of_measurement = homeassistantentities::Sensor::Precipitation::Unit::mm,
                              });

unsigned long _last_publish_ms = 0;

void setup() {
  Serial.begin(115200);
  setupJsonForThisDevice(); // Populate json dict.

  // Setup WiFI
  WiFi.begin(wifi_ssid, wifi_password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }
  Serial.println("have wifi");
  Serial.print("IP number: ");
  Serial.println(WiFi.localIP());

  _mqtt_remote.setOnConnected([]() {
    // Publish Home Assistant Configuration for the sensors once connected to MQTT.
    _ha_entity_brightness.publishConfiguration();
    _ha_entity_generic_sensor.publishConfiguration();
    _ha_entity_temperature_inside.publishConfiguration();
    _ha_entity_temperature_outside.publishConfiguration();
  });
}

void loop() {
  _mqtt_remote.handle();

  // Publish temperature and brightness status every 10 seconds.
  auto now = millis();
  if (now - _last_publish_ms > 10000) {
    _ha_entity_brightness.publishBrightness(128);
    _ha_entity_generic_sensor.publishValue(100.0);
    _ha_entity_temperature_inside.publishTemperature(22.5);
    _ha_entity_temperature_outside.publishTemperature(6.8);
    _last_publish_ms = now;
  }
}
