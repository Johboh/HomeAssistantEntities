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
// We call the setupJsonForThisDevice() from the ardunio setup() function to populate the Json document.
nlohmann::json _json_this_device_doc;
void setupJsonForThisDevice() {
  _json_this_device_doc["identifiers"] = "my_hardware_" + std::string(mqtt_client_id);
  _json_this_device_doc["name"] = "Livingroom";
  _json_this_device_doc["sw_version"] = "1.0.0";
  _json_this_device_doc["model"] = "my_hardware";
  _json_this_device_doc["manufacturer"] = "custom inc.";
}

// Setup MQTT
MQTTRemote _mqtt_remote(mqtt_client_id, mqtt_host, 1883, mqtt_username, mqtt_password, 2048, 10);

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

void haStateTask(void *pvParameters) {
  while (1) {
    _ha_entity_brightness.publishBrightness(128);
    _ha_entity_generic_sensor.publishValue(100.0);
    _ha_entity_temperature_inside.publishTemperature(22.5);
    _ha_entity_temperature_outside.publishTemperature(6.8);
    vTaskDelay(10000 / portTICK_PERIOD_MS);
  }
}

extern "C" {
void app_main();
}

void app_main(void) {
  // TODO (you): You need to connect to WiFi here first.
  // For a simple one line utility, see https://github.com/johboh/ConnectionHelper
  // Once connected to wifi, continue with below.

  // Connect to WIFI
  auto connected = true; // TODO (you): You need to connect to WiFi here first.
  if (connected) {
    // Connected to WIFI.

    // Start MQTT
    _mqtt_remote.start([]() {
      // Publish Home Assistant Configuration for the sensors once connected to MQTT.
      _ha_entity_brightness.publishConfiguration();
      _ha_entity_generic_sensor.publishConfiguration();
      _ha_entity_temperature_inside.publishConfiguration();
      _ha_entity_temperature_outside.publishConfiguration();
    });

    // Start task for periodically publishing state.
    xTaskCreate(haStateTask, "haStateTask", 2048, NULL, 15, NULL);

  } else {
    ESP_LOGE(TAG, "Failed to connect");
  }

  // Run forever.
  while (1) {
    vTaskDelay(500 / portTICK_PERIOD_MS);
    fflush(stdout);
  }
}
