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
#include <entities/HaEntityLight.h>
#ifdef ESP32
#include <WiFi.h>
#elif ESP8266
#include <ESP8266WiFi.h>
#else
#error "Unsupported hardware. Sorry!"
#endif

/**
 * This example setup a device in Home Assistant with lights.
 */

// Change this to your LED pin
#ifdef ESP32
#define LED_PIN GPIO_NUM_15
#elif ESP8266
#define LED_PIN D2
#else
#error "Unsupported hardware. Sorry!"
#endif

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

// Create the two lights with the "Human readable" strings. This what will show up in Home Assistant.
// As we have two entities of the same type (light) for the same device, we need to add a child object
// id to separate them.
HaEntityLight _ha_entity_light_left_bench(ha_bridge, "left bench", "left_bench", {.with_brightness = true});
HaEntityLight _ha_entity_light_right_bench(ha_bridge, "right bench", "right_bench", {.with_brightness = true});

bool _was_connected = false;
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
    // Publish Home Assistant Configuration for both lights once connected to MQTT.
    _ha_entity_light_left_bench.publishConfiguration();
    _ha_entity_light_right_bench.publishConfiguration();

    // Subscribe to new light "on" state pushed by Home Assistant.
    _ha_entity_light_left_bench.setOnOn([&](bool on) { digitalWrite(LED_PIN, on); });
    _ha_entity_light_right_bench.setOnBrightness(
        [&](uint8_t brightness) { Serial.println("Got brightness " + String(brightness) + " for right light"); });
  });
}

void loop() {
  _mqtt_remote.handle();

  // Publish current light status every 10 seconds.
  auto now = millis();
  if (now - _last_publish_ms > 10000) {
    bool on = digitalRead(LED_PIN);
    // Left light is state of LED, and we publish brightess as well as we "know" it at this time.
    // Right light is inverse state of LED, but we don't publish brightness as we don't "know" it as this time.
    _ha_entity_light_left_bench.publishIsOn(on);
    _ha_entity_light_left_bench.publishBrightness(255);
    _ha_entity_light_right_bench.publishIsOn(!on);
    _last_publish_ms = now;
  }
}
