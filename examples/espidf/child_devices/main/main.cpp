// Copyright (c) Johan Böhlin
// SPDX-License-Identifier: MIT

#include "credentials.h"
#include <HaBridge.h>
#include <HaDeviceInformation.h>
#include <MQTTRemote.h>
#include <entities/HaEntityButton.h>
#include <entities/HaEntityDeviceTrigger.h>
#include <entities/HaEntityPower.h>
#include <entities/HaEntitySensor.h>
#include <entities/HaEntitySwitch.h>
#include <entities/HaEntityTemperature.h>
#include <esp_log.h>
#include <esp_system.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <functional>
#include <nlohmann/json.hpp>
#include <vector>

#define TAG "child_devices_example"

/**
 * Smart Power Strip example for ESP-IDF demonstrating:
 * - Automatic ESP-IDF firmware/chip/MAC/IP extraction via HaDeviceInformation::populateDeviceJson()
 * - Origin metadata via HaDeviceInformation::detectOriginInfo()
 * - Logical child devices (individual outlets on one physical strip) via HaBridge::childDevice()
 * - Unified device-based discovery (<discovery_prefix>/device/<node_id>/config) via publishDeviceConfiguration()
 * - Home Assistant birth message handling (homeassistant/status) via onHomeAssistantBirth()
 * - Entity categories ("config" / "diagnostic"), device_class, icon, and expire_after
 */

nlohmann::json _json_this_device_doc;
MQTTRemote _mqtt_remote(mqtt_client_id, mqtt_host, 1883, mqtt_username, mqtt_password,
                        {.rx_buffer_size = 512, .tx_buffer_size = 2048, .task_size = 6144, .keep_alive_s = 10});

// Parent bridge representing the physical power strip.
HaBridge ha_bridge(_mqtt_remote, "power_strip", _json_this_device_doc);

// Child bridges representing logical sub-units (individual outlets) on the same physical power strip.
HaBridge outlet_1_bridge = ha_bridge.childDevice("outlet_1", "power_strip_outlet_1", "Outlet 1");
HaBridge outlet_2_bridge = ha_bridge.childDevice("outlet_2", "power_strip_outlet_2", "Outlet 2");

// Parent device entities.
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
HaEntityDeviceTrigger _main_button_trigger(ha_bridge, "main_button_short",
                                           {.type = "button_short_press", .subtype = "button_1"});

// Child device 1 entities (Outlet 1).
HaEntitySwitch _outlet_1_switch(outlet_1_bridge, "Switch", "",
                                {.retain = false, .device_class = "outlet", .icon = "mdi:power-socket-eu"});
HaEntityPower _outlet_1_power(outlet_1_bridge, "Power");

// Child device 2 entities (Outlet 2).
HaEntitySwitch _outlet_2_switch(outlet_2_bridge, "Switch", "",
                                {.retain = false, .device_class = "outlet", .icon = "mdi:power-socket-eu"});
HaEntityPower _outlet_2_power(outlet_2_bridge, "Power");

std::vector<std::reference_wrapper<HaEntity>> _all_entities = {
    _board_temperature, _wifi_rssi,      _restart_button,  _main_button_trigger,
    _outlet_1_switch,   _outlet_1_power, _outlet_2_switch, _outlet_2_power,
};

void publishAllDiscoveryAndStates() {
  ha_bridge.publishDeviceConfiguration(_all_entities);
  for (auto &entity : _all_entities) {
    entity.get().republishState();
  }
}

void haStateTask(void *pvParameters) {
  while (1) {
    _board_temperature.update(39.2);
    _wifi_rssi.update(-61);
    _outlet_1_power.update(48.5);
    _outlet_2_power.update(0.0);
    vTaskDelay(10000 / portTICK_PERIOD_MS);
  }
}

extern "C" void app_main(void) {
  // Connect to Wi-Fi before starting MQTT (e.g. via https://github.com/johboh/ConnectionHelper).
  auto connected = true;
  if (!connected) {
    ESP_LOGE(TAG, "Failed to connect");
    return;
  }

  // Extract firmware version, project_name, silicon revision, MAC, and local IP automatically via ESP-IDF APIs.
  HaDeviceInformation::populateDeviceJson(
      _json_this_device_doc, "Smart Power Strip", "power_strip_" + std::string(mqtt_client_id),
      {.configuration_http_port = 80, .suggested_area = "Office", .include_serial_number = true});
  ha_bridge.setOrigin(
      HaDeviceInformation::detectOriginInfo("SmartPowerStrip", "https://github.com/Johboh/HomeAssistantEntities"));

  _restart_button.onCommand([]() { esp_restart(); });
  _outlet_1_switch.onCommand([](bool on) { _outlet_1_switch.publish(on); });
  _outlet_2_switch.onCommand([](bool on) { _outlet_2_switch.publish(on); });

  // Subscribe once to Home Assistant's birth topic (homeassistant/status) before starting MQTT so discovery and live
  // states are re-published whenever Home Assistant restarts. Also publish on MQTT (re)connection below since Home
  // Assistant's birth message is published with retain=false by default.
  ha_bridge.onHomeAssistantBirth([]() { publishAllDiscoveryAndStates(); });

  _mqtt_remote.start([](bool mqtt_connected) {
    if (mqtt_connected) {
      publishAllDiscoveryAndStates();
    }
  });

  xTaskCreate(haStateTask, "haStateTask", 4096, NULL, 15, NULL);

  while (1) {
    vTaskDelay(500 / portTICK_PERIOD_MS);
  }
}
