// Copyright (c) Johan Böhlin
// SPDX-License-Identifier: MIT

#include "credentials.h"
#include <HaBridge.h>
#include <HaDeviceInformation.h>
#include <MQTTRemote.h>
#include <entities/HaDeviceClasses.h>
#include <entities/HaEntitySensor.h>
#include <entities/HaEntityTemperature.h>
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <functional>
#include <nlohmann/json.hpp>
#include <vector>

#define TAG "gateway_example"

/**
 * Gateway (`via_device`) and Discovery Lifecycle example for ESP-IDF demonstrating:
 * - Hub/gateway + remote physical sub-device topology using `DeviceInfo::via_device` and `connections`
 * - Migrating existing entities from single-component discovery to unified device-based discovery via
 *   `migrateToDeviceConfiguration()`
 * - Removing/unpublishing individual entities (`unpublishConfiguration()`) or a whole device
 *   (`unpublishDeviceConfiguration()`)
 * - Custom availability payloads (`setAvailabilityPayloads()`) and discovery prefix (`setDiscoveryPrefix()`)
 * - Binary sensor `off_delay` & `expire_after`, `Sensor::Enum` options, and `Sensor::Monetary` custom currency unit
 * - Logging resolved topics (`getTopic()`, `getDeviceDiscoveryTopic()`, `getAvailabilityTopic()`) for manual
 *   Home Assistant MQTT Subentry setup
 */

MQTTRemote _mqtt_remote(mqtt_client_id, mqtt_host, 1883, mqtt_username, mqtt_password,
                        {.rx_buffer_size = 512, .tx_buffer_size = 2048, .task_size = 6144, .keep_alive_s = 10});

// 1. Gateway device (this ESP hardware acting as a BLE/ESP-NOW-to-MQTT bridge).
nlohmann::json _gateway_device_doc;
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
nlohmann::json _remote_sensor_doc;
HaBridge garden_sensor_bridge(_mqtt_remote, "garden_sensor", _remote_sensor_doc);

HaEntityTemperature _garden_temperature(garden_sensor_bridge, "Temperature");
homeassistantentities::BinarySensor::Motion _motion_class;
HaEntitySensor _garden_pir(garden_sensor_bridge, "Motion", std::nullopt,
                           {
                               .device_class = _motion_class,
                               .expire_after = 300,
                               .off_delay = 15,
                           });
homeassistantentities::Sensor::Monetary _monetary_class;
HaEntitySensor _energy_cost(garden_sensor_bridge, "Daily Cost", "cost",
                            {
                                .device_class = _monetary_class,
                                .custom_unit_of_measurement = "EUR",
                            });
HaEntityTemperature _deprecated_sensor(garden_sensor_bridge, "Legacy Probe", "legacy");

void haStateTask(void *pvParameters) {
  while (1) {
    _garden_temperature.update(18.4);
    _garden_pir.publish("ON");
    _energy_cost.update(1.42);
    vTaskDelay(10000 / portTICK_PERIOD_MS);
  }
}

extern "C" void app_main(void) {
  auto connected = true;
  if (!connected) {
    ESP_LOGE(TAG, "Failed to connect");
    return;
  }

  // Auto-populate gateway hardware/firmware metadata via ESP-IDF APIs.
  HaDeviceInformation::populateDeviceJson(_gateway_device_doc, "BLE Gateway", "ble_gateway_01",
                                          {.include_serial_number = true});
  gateway_bridge.setDiscoveryPrefix("homeassistant");
  gateway_bridge.setAvailabilityPayloads("online", "offline");

  // Populate separate remote wireless sensor metadata linking back to `ble_gateway_01` via `via_device`.
  HaBridge::populateDeviceJson(_remote_sensor_doc, {.identifiers = {"garden_ble_sensor_01"},
                                                    .connections = {{"mac", "aa:bb:cc:11:22:33"}},
                                                    .name = "Garden Climate Node",
                                                    .manufacturer = "Acme Wireless",
                                                    .model = "TH-BLE-Mini",
                                                    .sw_version = "1.4.2",
                                                    .via_device = "ble_gateway_01",
                                                    .suggested_area = "Garden"});

  // Log resolved MQTT topics for manual Home Assistant MQTT Subentry setup or debugging.
  ESP_LOGI(TAG, "Availability topic: %s", garden_sensor_bridge.getAvailabilityTopic().c_str());
  ESP_LOGI(TAG, "Device discovery topic: %s", garden_sensor_bridge.getDeviceDiscoveryTopic().c_str());
  ESP_LOGI(TAG, "Temperature state topic: %s",
           garden_sensor_bridge.getTopic(HaBridge::TopicType::State, "sensor", "temperature").c_str());

  // Multiple bridges sharing the same MQTTRemote can each register an onHomeAssistantBirth handler.
  gateway_bridge.onHomeAssistantBirth([]() {
    gateway_bridge.publishDeviceConfiguration({_gateway_state});
    _gateway_state.publish("scanning");
  });
  garden_sensor_bridge.onHomeAssistantBirth([]() {
    garden_sensor_bridge.publishDeviceConfiguration({_garden_temperature, _garden_pir, _energy_cost});
    _garden_temperature.republishState();
    _energy_cost.republishState();
  });

  _mqtt_remote.start([](bool mqtt_connected) {
    if (!mqtt_connected) {
      return;
    }
    gateway_bridge.publishDeviceConfiguration({_gateway_state});
    _gateway_state.publish("scanning");

    // Migrate active entities from single-component discovery to unified device-based discovery,
    // and remove a deprecated entity's retained discovery message.
    garden_sensor_bridge.migrateToDeviceConfiguration({_garden_temperature, _garden_pir, _energy_cost});
    _deprecated_sensor.unpublishConfiguration();
  });

  xTaskCreate(haStateTask, "haStateTask", 4096, NULL, 15, NULL);

  while (1) {
    vTaskDelay(500 / portTICK_PERIOD_MS);
  }
}
