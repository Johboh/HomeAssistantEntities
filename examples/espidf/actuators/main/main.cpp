#include "credentials.h"
#include <HaBridge.h>
#include <MQTTRemote.h>
#include <driver/gpio.h>
#include <entities/HaEntityLight.h>
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <nlohmann/json.hpp>

#define TAG "example"

#define PIN_LED GPIO_NUM_14

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
MQTTRemote _mqtt_remote(mqtt_client_id, mqtt_host, 1883, mqtt_username, mqtt_password,
                        {.rx_buffer_size = 256, .tx_buffer_size = 1024, .keep_alive_s = 10});

// Create the Home Assistant bridge. This is shared across all entities.
// We only have one per device/hardware. In our example, the name of our device is "livingroom".
// See constructor of HaBridge for more documentation.
HaBridge ha_bridge(_mqtt_remote, "livingroom", _json_this_device_doc);

// Create the two lights with the "Human readable" strings. This what will show up in Home Assistant.
// As we have two entities of the same type (light) for the same device, we need to add a child object
// id to separate them.
HaEntityLight _ha_entity_light_left_bench(ha_bridge, "left bench", "left_bench", {.with_brightness = true});
HaEntityLight _ha_entity_light_right_bench(ha_bridge, "right bench", "right_bench", {.with_brightness = true});

void haStateTask(void *pvParameters) {
  while (1) {
    bool on = gpio_get_level(PIN_LED);
    // Left light is state of LED, and we publish brightess as well as we "know" it at this time.
    // Right light is inverse state of LED, but we don't publish brightness as we don't "know" it as this time.
    _ha_entity_light_left_bench.publishIsOn(on);
    _ha_entity_light_left_bench.publishBrightness(255);
    _ha_entity_light_right_bench.publishIsOn(!on);
    vTaskDelay(10000 / portTICK_PERIOD_MS);
  }
}

extern "C" {
void app_main();
}

void app_main(void) {
  // Setup led
  gpio_set_direction(PIN_LED, GPIO_MODE_OUTPUT);
  gpio_set_level(PIN_LED, 1);

  // TODO (you): You need to connect to WiFi here first.
  // For a simple one line utility, see https://github.com/johboh/ConnectionHelper
  // Once connected to wifi, continue with below.

  // Connect to WIFI
  auto connected = true; // TODO (you): You need to connect to WiFi here first.
  if (connected) {
    // Connected to WIFI.

    // Start MQTT
    _mqtt_remote.start([](bool connected) {
      // Publish Home Assistant Configuration for both lights once connected to MQTT.
      _ha_entity_light_left_bench.publishConfiguration();
      _ha_entity_light_right_bench.publishConfiguration();

      // Subscribe to new light "on" state pushed by Home Assistant.
      _ha_entity_light_left_bench.setOnOn([&](bool on) { gpio_set_level(PIN_LED, on); });
      _ha_entity_light_right_bench.setOnBrightness(
          [&](uint8_t brightness) { ESP_LOGI(TAG, "Got brightness %d for left light", brightness); });
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
