// Copyright (c) Johan Böhlin
// SPDX-License-Identifier: MIT

#include "credentials.h"
#include <HaBridge.h>
#include <MQTTRemote.h>
#include <driver/gpio.h>
#include <entities/HaEntityBrightness.h>
#include <entities/HaEntitySensor.h>
#include <entities/HaEntityTemperature.h>
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <nlohmann/json.hpp>

#define TAG "example"

// Information about this device.
// All these keys will be added to a "device" key in the Home Assistant configuration for each entity.
// Only a flat layout structure is supported, no nesting.
// We call setupJsonForThisDevice() from app_main() to populate the Json document.
nlohmann::json _json_this_device_doc;
void setupJsonForThisDevice() {
  _json_this_device_doc["identifiers"] = "my_hardware_" + std::string(mqtt_client_id);
  _json_this_device_doc["name"] = "Livingroom";
  _json_this_device_doc["sw_version"] = "1.0.0";
  _json_this_device_doc["model"] = "my_hardware";
  _json_this_device_doc["manufacturer"] = "custom inc.";
}

// Setup MQTT
MQTTRemote _mqtt_remote(mqtt_client_id, mqtt_host, 1883, mqtt_username, mqtt_password,
                        {.rx_buffer_size = 256, .tx_buffer_size = 1024, .task_size = 6144, .keep_alive_s = 10});

// Create the Home Assistant bridge. This is shared across all entities.
// We only have one per device/hardware. In our example, the name of our device is "livingroom".
// See constructor of HaBridge for more documentation.
HaBridge ha_bridge(_mqtt_remote, "livingroom", _json_this_device_doc);

// Create the sensors with the "Human readable" strings. This what will show up in Home Assistant.
HaEntityBrightness _ha_entity_brightness(ha_bridge, "brightness");

// For multiple sensors of the same type for the same device, we need to add a child object id to separate them (third
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

void haStateTask(void *pvParameters) {
  while (1) {
    _ha_entity_brightness.publish(128);
    _ha_entity_generic_sensor.publish(100.0);
    _ha_entity_temperature_inside.publish(22.5);
    _ha_entity_temperature_outside.publish(6.8);
    vTaskDelay(10000 / portTICK_PERIOD_MS);
  }
}

extern "C" {
void app_main();
}

void publishConfiguration() {
  _ha_entity_brightness.publishConfiguration();
  _ha_entity_generic_sensor.publishConfiguration();
  _ha_entity_temperature_inside.publishConfiguration();
  _ha_entity_temperature_outside.publishConfiguration();
}

void app_main(void) {
  setupJsonForThisDevice();

  // TODO (you): You need to connect to WiFi here first.
  // For a simple one line utility, see https://github.com/johboh/ConnectionHelper
  // Once connected to wifi, continue with below.

  // Connect to WIFI
  auto connected = true; // TODO (you): You need to connect to WiFi here first.
  if (connected) {
    // Connected to WIFI.

    // Re-publish discovery configuration if Home Assistant restarts while we stay connected to the MQTT broker.
    // Note: Home Assistant's birth message (homeassistant/status -> "online") is published with retain=false by
    // default, so we also publish on initial MQTT (re)connection below for when Home Assistant is already running.
    ha_bridge.onHomeAssistantBirth([]() { publishConfiguration(); });

    // Start MQTT
    _mqtt_remote.start([](bool connected) {
      if (connected) {
        publishConfiguration();
      }
    });

    // Start task for periodically publishing state.
    xTaskCreate(haStateTask, "haStateTask", 4096, NULL, 15, NULL);

  } else {
    ESP_LOGE(TAG, "Failed to connect");
  }

  // Run forever.
  while (1) {
    vTaskDelay(500 / portTICK_PERIOD_MS);
    fflush(stdout);
  }
}
