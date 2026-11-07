// Copyright (c) Johan Böhlin
// SPDX-License-Identifier: MIT

#include <Arduino.h>
#ifndef PLATFORMIO
#include <ArduinoJson.h>
#endif
#include <HaBridge.h>
#include <HaDeviceInformation.h>
#include <IJson.h>
#include <MQTTRemote.h>
#include <entities/HaEntityButton.h>
#include <entities/HaEntityEnergy.h>
#include <entities/HaEntityPower.h>
#include <entities/HaEntitySensor.h>
#include <entities/HaEntitySwitch.h>
#include <entities/HaEntityTemperature.h>
#include <entities/HaEntityUpdate.h>
#include <functional>
#include <vector>
#ifdef ESP32
#include <WiFi.h>
#elif ESP8266
#include <ESP8266WiFi.h>
#else
#error "Unsupported hardware. Sorry!"
#endif

/**
 * Smart Power Strip example demonstrating:
 * - Automatic hardware/firmware/MAC/IP extraction via HaDeviceInformation::populateDeviceJson()
 * - Origin metadata via HaDeviceInformation::detectOriginInfo()
 * - Logical child devices (individual outlets on a single physical power strip) via HaBridge::childDevice()
 * - Unified device-based discovery (<discovery_prefix>/device/<node_id>/config) via publishDeviceConfiguration()
 * - Home Assistant birth message handling (homeassistant/status) via onHomeAssistantBirth()
 * - Entity categories ("config" / "diagnostic"), device_class, icon, and expire_after
 */

const char wifi_ssid[] = "my-wifi-ssid";
const char wifi_password[] = "my-wifi-password";
const char mqtt_client_id[] = "power-strip";
const char mqtt_host[] = "192.168.1.1";
const char mqtt_username[] = "my-username";
const char mqtt_password[] = "my-password";

IJsonDocument _json_this_device_doc;
MQTTRemote _mqtt_remote(mqtt_client_id, mqtt_host, 1883, mqtt_username, mqtt_password);

// Parent bridge representing the physical power strip itself.
HaBridge ha_bridge(_mqtt_remote, "power_strip", _json_this_device_doc);

// Child bridges representing logical sub-units (individual outlets) on the same physical power strip.
HaBridge outlet_1_bridge = ha_bridge.childDevice("outlet_1", "power_strip_outlet_1", "Outlet 1");
HaBridge outlet_2_bridge = ha_bridge.childDevice("outlet_2", "power_strip_outlet_2", "Outlet 2");

// Parent device entities (board temperature, diagnostic Wi-Fi RSSI, and config restart button).
HaEntityTemperature _board_temperature(ha_bridge, "Board Temperature", "board");
homeassistantentities::Sensor::SignalStrength _signal_strength;
HaEntitySensor _wifi_rssi(ha_bridge, "WiFi Signal", std::nullopt,
                          {
                              .device_class = _signal_strength,
                              .unit_of_measurement = homeassistantentities::Sensor::SignalStrength::Unit::dBm,
                              .entity_category = "diagnostic",
                              .expire_after = 60,
                          });
HaEntityButton _restart_button(ha_bridge, "Restart", "restart",
                               {.device_class = "restart", .icon = "mdi:restart", .entity_category = "config"});
HaEntityUpdate _firmware_update(ha_bridge, "Firmware");

// Child device 1 entities (Outlet 1 switch + power + energy meter).
HaEntitySwitch _outlet_1_switch(outlet_1_bridge, "Switch", "",
                                {.retain = false, .device_class = "outlet", .icon = "mdi:power-socket-eu"});
HaEntityPower _outlet_1_power(outlet_1_bridge, "Power");
HaEntityEnergy _outlet_1_energy(outlet_1_bridge, "Energy");

// Child device 2 entities (Outlet 2 switch + power meter).
HaEntitySwitch _outlet_2_switch(outlet_2_bridge, "Switch", "",
                                {.retain = false,
                                 .device_class = homeassistantentities::Switch::Outlet{},
                                 .icon = "mdi:power-socket-eu"});
HaEntityPower _outlet_2_power(outlet_2_bridge, "Power");

std::vector<std::reference_wrapper<HaEntity>> _all_entities = {
    _board_temperature, _wifi_rssi,       _restart_button,  _firmware_update, _outlet_1_switch,
    _outlet_1_power,    _outlet_1_energy, _outlet_2_switch, _outlet_2_power,
};

unsigned long _last_publish_ms = 0;

void publishAllDiscoveryAndStates() {
  // Publish a single unified device discovery payload containing both parent "components" and "child_devices".
  ha_bridge.publishDeviceConfiguration(_all_entities);
  for (auto &entity : _all_entities) {
    entity.get().republishState();
  }
}

void setup() {
  Serial.begin(115200);

  WiFi.begin(wifi_ssid, wifi_password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }

  // Populate device JSON after Wi-Fi connects so local IP can be included in configuration_url (port 80).
  HaDeviceInformation::populateDeviceJson(_json_this_device_doc, "Smart Power Strip",
                                          "power_strip_" + std::string(mqtt_client_id),
                                          {.configuration_http_port = 80, .suggested_area = "Office"});
  ha_bridge.setOrigin(
      HaDeviceInformation::detectOriginInfo("SmartPowerStrip", "https://github.com/Johboh/HomeAssistantEntities"));

  // Register command and Home Assistant birth subscriptions once before starting MQTT.
  _restart_button.onCommand([]() { ESP.restart(); });
  _firmware_update.onCommand([]() { Serial.println("Starting OTA firmware update..."); });
  _outlet_1_switch.onCommand([](bool on) { _outlet_1_switch.publish(on); });
  _outlet_2_switch.onCommand([](bool on) { _outlet_2_switch.publish(on); });

  // Re-publish discovery and live states whenever Home Assistant restarts (homeassistant/status -> "online").
  // Because Home Assistant's birth message is not retained by default, we also publish on MQTT (re)connection below.
  ha_bridge.onHomeAssistantBirth([]() { publishAllDiscoveryAndStates(); });

#if defined(ESP32) && defined(PLATFORMIO)
  _mqtt_remote.start([](bool connected) {
    if (connected) {
      publishAllDiscoveryAndStates();
    }
  });
#else
  _mqtt_remote.setOnConnectionChange([](bool connected) {
    if (connected) {
      publishAllDiscoveryAndStates();
    }
  });
#endif
}

void loop() {
#if !defined(ESP32) || !defined(PLATFORMIO)
  _mqtt_remote.handle();
#endif

  auto now = millis();
  if (now - _last_publish_ms > 10000) {
    _board_temperature.update(38.5);
    _wifi_rssi.update(WiFi.RSSI());
    _firmware_update.update("1.0.0", "1.0.0");
    _outlet_1_power.update(42.0);
    _outlet_1_energy.update(1.25);
    _outlet_2_power.update(0.0);
    _last_publish_ms = now;
  }
}
