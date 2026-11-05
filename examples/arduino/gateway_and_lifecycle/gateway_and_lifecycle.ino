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
#include <entities/HaDeviceClasses.h>
#include <entities/HaEntitySensor.h>
#include <entities/HaEntityTemperature.h>
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
 * Gateway (`via_device`) and Discovery Lifecycle example demonstrating:
 * - Hub/gateway + remote physical sub-device topology using `DeviceInfo::via_device` and `connections`
 * - Migrating existing entities from single-component discovery to unified device-based discovery via
 *   `migrateToDeviceConfiguration()`
 * - Removing/unpublishing individual entities (`unpublishConfiguration()`) or a whole device
 *   (`unpublishDeviceConfiguration()`)
 * - Custom availability payloads (`setAvailabilityPayloads()`) and discovery prefix (`setDiscoveryPrefix()`)
 * - Binary sensor `off_delay` & `expire_after`, and `Sensor::Enum` options
 * - Inspecting resolved topics (`getTopic()`, `getDeviceDiscoveryTopic()`, `getAvailabilityTopic()`) for manual
 *   Home Assistant MQTT Subentry setup
 */

const char wifi_ssid[] = "my-wifi-ssid";
const char wifi_password[] = "my-wifi-password";
const char mqtt_client_id[] = "ble-gateway";
const char mqtt_host[] = "192.168.1.1";
const char mqtt_username[] = "my-username";
const char mqtt_password[] = "my-password";

MQTTRemote _mqtt_remote(mqtt_client_id, mqtt_host, 1883, mqtt_username, mqtt_password);

// 1. Gateway device (this ESP hardware acting as a BLE-to-MQTT bridge).
IJsonDocument _gateway_device_doc;
HaBridge gateway_bridge(_mqtt_remote, "ble_gateway", _gateway_device_doc);

homeassistantentities::Sensor::Enum _enum_class;
HaEntitySensor _gateway_state(gateway_bridge, "Gateway State", "status",
                              {
                                  .device_class = _enum_class,
                                  .state_class = std::nullopt,
                                  .options = {"scanning", "idle", "error"},
                                  .entity_category = "diagnostic",
                              });

// 2. Separate physical wireless sensor routed through the gateway via `via_device`.
IJsonDocument _remote_sensor_doc;
HaBridge garden_sensor_bridge(_mqtt_remote, "garden_sensor", _remote_sensor_doc);

HaEntityTemperature _garden_temperature(garden_sensor_bridge, "Temperature");
homeassistantentities::BinarySensor::Motion _motion_class;
HaEntitySensor _garden_pir(garden_sensor_bridge, "Motion", std::nullopt,
                           {
                               .device_class = _motion_class,
                               .expire_after = 300,
                               .off_delay = 15, // Automatically resets to OFF after 15 seconds in Home Assistant
                           });
// Legacy diagnostic sensor that we will remove with unpublishConfiguration().
HaEntityTemperature _deprecated_sensor(garden_sensor_bridge, "Legacy Probe", "legacy");

unsigned long _last_publish_ms = 0;

void publishGatewayDiscoveryAndState() {
  gateway_bridge.publishDeviceConfiguration({_gateway_state});
  _gateway_state.publishValue("scanning");
}

void publishGardenNodeDiscoveryAndState() {
  garden_sensor_bridge.publishDeviceConfiguration({_garden_temperature, _garden_pir});
  _garden_temperature.republishState();
}

void setup() {
  Serial.begin(115200);

  WiFi.begin(wifi_ssid, wifi_password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }

  // Populate gateway metadata from ESP hardware/firmware APIs.
  HaDeviceInformation::populateDeviceJson(_gateway_device_doc, "BLE Gateway", "ble_gateway_01",
                                          {.include_serial_number = true});
  gateway_bridge.setDiscoveryPrefix("homeassistant");
  gateway_bridge.setAvailabilityPayloads("online", "offline");

  // Populate separate remote BLE sensor metadata linking back to the gateway via `via_device`.
  HaBridge::populateDeviceJson(_remote_sensor_doc, {.identifiers = {"garden_ble_sensor_01"},
                                                    .connections = {{"mac", "aa:bb:cc:11:22:33"}},
                                                    .name = "Garden Climate Node",
                                                    .manufacturer = "Acme Wireless",
                                                    .model = "TH-BLE-Mini",
                                                    .sw_version = "1.4.2",
                                                    .via_device = "ble_gateway_01",
                                                    .suggested_area = "Garden"});

  // Print resolved MQTT topics (useful if configuring a manual MQTT Subentry in Home Assistant).
  Serial.println(("Availability topic: " + garden_sensor_bridge.getAvailabilityTopic()).c_str());
  Serial.println(("Device discovery topic: " + garden_sensor_bridge.getDeviceDiscoveryTopic()).c_str());
  Serial.println(
      ("Temp state topic: " + garden_sensor_bridge.getTopic(HaBridge::TopicType::State, "sensor", "temperature"))
          .c_str());

  // Multiple bridges sharing the same MQTTRemote can each register an onHomeAssistantBirth handler.
  gateway_bridge.onHomeAssistantBirth([]() { publishGatewayDiscoveryAndState(); });
  garden_sensor_bridge.onHomeAssistantBirth([]() { publishGardenNodeDiscoveryAndState(); });

  auto onInitialOrReconnect = [](bool connected) {
    if (!connected) {
      return;
    }
    publishGatewayDiscoveryAndState();

    // Migrate the remote sensor's active entities from single-component discovery to unified device-based discovery,
    // and remove a deprecated entity's old single-component configuration.
    garden_sensor_bridge.migrateToDeviceConfiguration({_garden_temperature, _garden_pir});
    _deprecated_sensor.unpublishConfiguration();
  };

#if defined(ESP32) && defined(PLATFORMIO)
  _mqtt_remote.start(onInitialOrReconnect);
#else
  _mqtt_remote.setOnConnectionChange(onInitialOrReconnect);
#endif
}

void loop() {
#if !defined(ESP32) || !defined(PLATFORMIO)
  _mqtt_remote.handle();
#endif

  auto now = millis();
  if (now - _last_publish_ms > 10000) {
    _garden_temperature.updateTemperature(18.4);
    _garden_pir.publishValue("ON");
    _last_publish_ms = now;
  }
}
