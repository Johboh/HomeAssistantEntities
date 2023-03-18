#include <Arduino.h>
#include <HaBridge.h>
#include <MQTTRemote.h>
#include <entities/HaEntityBrightness.h>
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
StaticJsonDocument<256> _json_this_device_doc;
void setupJsonForThisDevice() {
  _json_this_device_doc["identifiers"] = "my_hardware_" + String(mqtt_client_id);
  _json_this_device_doc["name"] = mqtt_client_id;
  _json_this_device_doc["sw_version"] = "1.0.0";
  _json_this_device_doc["model"] = "my_hardware";
  _json_this_device_doc["manufacturer"] = "custom inc.";
}
MQTTRemote _mqtt_remote(mqtt_client_id, mqtt_host, 1883, mqtt_username, mqtt_password);

// Create the Home Assistant bridge. This is shared across all entities.
// We only have one per device/hardware. In our example, the name of our device is "kitchen".
// See constructor of HaBridge for more documentation.
HaBridge ha_bridge(_mqtt_remote, _json_this_device_doc, "kitchen");

// Create the two sensors with the "Human readable" strings. This what will show up in Home Assistant.
HaEntityBrightness _ha_entity_brightness(ha_bridge, "Kitchen brightness");
HaEntityTemperature _ha_entity_temperature(ha_bridge, "Kitchen temperature");

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
}

void loop() {
  _mqtt_remote.handle();

  auto connected = _mqtt_remote.connected();
  if (!_was_connected && connected) {
    // Publish Home Assistant Configuration for both sensors once connected to MQTT.
    _ha_entity_brightness.publishConfiguration();
    _ha_entity_temperature.publishConfiguration();
  }
  _was_connected = connected;

  // Publish temperature and brightness status every 10 seconds.
  auto now = millis();
  if (now - _last_publish_ms > 10000) {
    _ha_entity_brightness.publishBrightness(128);
    _ha_entity_temperature.publishTemperature(25.5);
    _last_publish_ms = now;
  }
}
