// Copyright (c) Johan Böhlin
// SPDX-License-Identifier: MIT

#include "credentials.h"
#include <HaBridge.h>
#include <MQTTRemote.h>
#include <driver/gpio.h>
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <functional>
#include <nlohmann/json.hpp>

#include <entities/HaEntityAtmosphericPressure.h>
#include <entities/HaEntityBoolean.h>
#include <entities/HaEntityBrightness.h>
#include <entities/HaEntityButton.h>
#include <entities/HaEntityCarbonDioxide.h>
#include <entities/HaEntityCover.h>
#include <entities/HaEntityCurrent.h>
#include <entities/HaEntityDeviceTrigger.h>
#include <entities/HaEntityDoor.h>
#include <entities/HaEntityEvent.h>
#include <entities/HaEntityFan.h>
#include <entities/HaEntityHumidity.h>
#include <entities/HaEntityJson.h>
#include <entities/HaEntityLight.h>
#include <entities/HaEntityLock.h>
#include <entities/HaEntityMotion.h>
#include <entities/HaEntityNumber.h>
#include <entities/HaEntityParticulateMatter.h>
#include <entities/HaEntityPower.h>
#include <entities/HaEntitySelect.h>
#include <entities/HaEntitySensor.h>
#include <entities/HaEntitySignalStrength.h>
#include <entities/HaEntitySound.h>
#include <entities/HaEntityString.h>
#include <entities/HaEntitySwitch.h>
#include <entities/HaEntityTemperature.h>
#include <entities/HaEntityText.h>
#include <entities/HaEntityTimestamp.h>
#include <entities/HaEntityUnitConcentration.h>
#include <entities/HaEntityVolatileOrganicCompounds.h>
#include <entities/HaEntityVoltage.h>
#include <entities/HaEntityWeight.h>

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
                        {.rx_buffer_size = 256, .tx_buffer_size = 1024, .task_size = 8192, .keep_alive_s = 10});

// Create the Home Assistant bridge. This is shared across all entities.
// We only have one per device/hardware. In our example, the name of our device is "livingroom".
// See constructor of HaBridge for more documentation.
bool verbose = false;
// This is optional, only here for the sake of integration tests.
auto availability_topic = [](IMQTTRemote &remote) { return remote.clientId() + "/my_custom_status_path"; };
// This is optional, only here for the sake of integration tests.
auto unique_id = [](IMQTTRemote &remote, std::string &node_id) {
  return remote.clientId() + "_custom_extras_" + node_id;
};
HaBridge ha_bridge(_mqtt_remote, "livingroom", _json_this_device_doc, verbose, availability_topic, unique_id);

// All entities.
HaEntityAtmosphericPressure _ha_entity_atm(ha_bridge, "pressure", std::nullopt,
                                           HaEntityAtmosphericPressure::Configuration{.force_update = false});
HaEntityBoolean _ha_entity_bool(ha_bridge, "bool", std::nullopt, {.with_attributes = true, .force_update = false});
HaEntityBrightness _ha_entity_bright(ha_bridge, "brightness", std::nullopt, {.force_update = false});
HaEntityButton _ha_entity_button(ha_bridge, "button", "");
HaEntityCarbonDioxide _ha_entity_carbon_dioxide(ha_bridge, "Carbon dioxide");
HaEntityCover _ha_entity_cover(ha_bridge, "cover", "left_window", {.device_class = "shade"});
HaEntityCurrent _ha_entity_current(ha_bridge, "current", std::nullopt,
                                   {.unit = HaEntityCurrent::Unit::mA, .force_update = false});
HaEntityDeviceTrigger _ha_entity_device_trigger(ha_bridge, "device_trigger",
                                                {.type = "button_short_press", .subtype = "button_1"});
HaEntityDoor _ha_entity_door(ha_bridge, "door", "");
HaEntityEvent _ha_entity_event(ha_bridge, "event", "party",
                               {.event_types = {"button_press"}, .device_class = HaEntityEvent::DeviceClass::Button});
HaEntityFan _ha_entity_fan(ha_bridge, "fan", "ceiling",
                           {.with_direction = true,
                            .with_oscillation = true,
                            .with_speed = true,
                            .speed_range_min = 1,
                            .speed_range_max = 100,
                            .presets = {"eco", "boost"},
                            .retain = false});
HaEntityHumidity _ha_entity_humidity(ha_bridge, "humidity", std::nullopt, {.force_update = false});
HaEntityJson _ha_entity_json(ha_bridge, "json", std::nullopt, {.force_update = false});
HaEntityLight _ha_entity_light(ha_bridge, "light", "roof",
                               {.with_brightness = true,
                                .with_color_temperature = HaEntityLight::Configuration::ColorTemperature::Kelvin,
                                .with_rgb_color = true,
                                .effects = {"colorloop"},
                                .retain = false});
HaEntityLock _ha_entity_lock(ha_bridge, "lock");
HaEntityMotion _ha_entity_motion(ha_bridge, "motion");
HaEntityNumber _ha_entity_number(
    ha_bridge, "number", "danceability",
    {.min_value = 0, .max_value = 100, .step = 0.1f, .unit = "ms", .device_class = "duration", .retain = false});
HaEntityParticulateMatter _ha_entity_particulate_matter(ha_bridge, "Particulate matter", std::nullopt,
                                                        {.size = HaEntityParticulateMatter::Size::pm25});
HaEntityPower _ha_entity_power(ha_bridge, "power", std::nullopt,
                               {.unit = HaEntityPower::Unit::W, .force_update = false});
HaEntitySelect _ha_entity_select(ha_bridge, "select", "playlist", {.options = {"option1", "option2"}, .retain = false});
HaEntitySound _ha_entity_sound(ha_bridge, "sound", "");
homeassistantentities::Sensor::Precipitation _precipitation;
HaEntitySensor _ha_entity_sensor(ha_bridge, "sensor", std::nullopt,
                                 HaEntitySensor::Configuration{
                                     .device_class = _precipitation,
                                     .unit_of_measurement = homeassistantentities::Sensor::Precipitation::Unit::mm,
                                     .with_attributes = false,
                                     .force_update = false});
HaEntityString _ha_entity_string(ha_bridge, "string", std::nullopt, {.with_attributes = true, .force_update = false});
HaEntitySwitch _ha_entity_switch(ha_bridge, "switch", "party", {.retain = false});
HaEntityTemperature _ha_entity_temperature(ha_bridge, "temperature", std::nullopt,
                                           {.unit = HaEntityTemperature::Unit::C, .force_update = false});
HaEntityText _ha_entity_text(
    ha_bridge, "text", "billboard",
    {.min_text_length = 0, .max_text_length = 255, .with_state_topic = true, .is_password = false, .retain = false});
HaEntityTimestamp _ha_entity_timestamp(ha_bridge, "timestamp", std::nullopt,
                                       {.with_attributes = false, .force_update = false});
HaEntityUnitConcentration _ha_entity_unit_concentration(ha_bridge, "concentration", std::nullopt,
                                                        {.unit = HaEntityUnitConcentration::Unit::mL});
HaEntityVolatileOrganicCompounds _ha_entity_volatile_organic_compounds(
    ha_bridge, "volatile organic compounds", std::nullopt,
    {.unit = HaEntityVolatileOrganicCompounds::Unit::Concentration, .force_update = false});
HaEntityVoltage _ha_entity_voltage(ha_bridge, "voltage", std::nullopt,
                                   {.unit = HaEntityVoltage::Unit::mV, .force_update = false});
HaEntityWeight _ha_entity_weight(ha_bridge, "weight", std::nullopt,
                                 {.unit = HaEntityWeight::Unit::kg, .force_update = false});
HaEntitySignalStrength _ha_entity_signal_strength(ha_bridge, "signal_strength", std::nullopt,
                                                  {.unit = HaEntitySignalStrength::Unit::dBm, .force_update = false});

void haStateTask(void *pvParameters) {
  nlohmann::json jsn;
  jsn["hello"] = "world";

  while (1) {
    _ha_entity_atm.publish(1001.5);
    _ha_entity_bool.publish(true);
    _ha_entity_bool.publishAttributes({{"attr1", "value1"}, {"attr2", "value2"}});
    _ha_entity_bright.publish(55.0);
    _ha_entity_carbon_dioxide.publish(55.0);
    _ha_entity_cover.publish(HaEntityCover::State::Opening, 50);
    _ha_entity_current.publish(10);
    _ha_entity_device_trigger.publish();
    _ha_entity_door.publish(true);
    _ha_entity_event.publish("button_press", {{"attr1", "value1"}, {"attr2", "value2"}});
    _ha_entity_fan.publish(true);
    _ha_entity_fan.publishSpeed(50);
    _ha_entity_fan.publishPreset("eco");
    _ha_entity_fan.publishDirection("forward");
    _ha_entity_fan.publishOscillation(true);
    _ha_entity_humidity.publish(55.0);
    _ha_entity_json.publish(jsn);
    _ha_entity_light.publish(true);
    _ha_entity_light.publishBrightness(255);
    _ha_entity_light.publishEffect("colorloop");
    _ha_entity_light.publishRgb(255, 255, 255);
    _ha_entity_lock.publish(true);
    _ha_entity_motion.publish(true);
    _ha_entity_number.publish(55.0f);
    _ha_entity_particulate_matter.publish(55.0);
    _ha_entity_power.publish(10);
    _ha_entity_select.publish("option1");
    _ha_entity_sensor.publish(55.0);
    _ha_entity_sound.publish(true);
    _ha_entity_string.publish("string", {{"attr1", "value1"}, {"attr2", "value2"}});
    _ha_entity_switch.publish(true);
    _ha_entity_temperature.publish(55.0);
    _ha_entity_text.publish("text");
    _ha_entity_timestamp.publish("2025-05-06T15:43:00Z");
    _ha_entity_unit_concentration.publish(55.0);
    _ha_entity_volatile_organic_compounds.publish(55.0);
    _ha_entity_voltage.publish(55.0);
    _ha_entity_weight.publish(55.0);
    _ha_entity_signal_strength.publish(-67);
    vTaskDelay(10000 / portTICK_PERIOD_MS);
  }
}

extern "C" {
void app_main();
}

void publishAllConfigurations() {
  _ha_entity_atm.publishConfiguration();
  _ha_entity_bool.publishConfiguration();
  _ha_entity_bright.publishConfiguration();
  _ha_entity_button.publishConfiguration();
  _ha_entity_carbon_dioxide.publishConfiguration();
  _ha_entity_cover.publishConfiguration();
  _ha_entity_current.publishConfiguration();
  _ha_entity_device_trigger.publishConfiguration();
  _ha_entity_door.publishConfiguration();
  _ha_entity_event.publishConfiguration();
  _ha_entity_fan.publishConfiguration();
  _ha_entity_humidity.publishConfiguration();
  _ha_entity_json.publishConfiguration();
  _ha_entity_light.publishConfiguration();
  _ha_entity_lock.publishConfiguration();
  _ha_entity_motion.publishConfiguration();
  _ha_entity_number.publishConfiguration();
  _ha_entity_particulate_matter.publishConfiguration();
  _ha_entity_power.publishConfiguration();
  _ha_entity_select.publishConfiguration();
  _ha_entity_sensor.publishConfiguration();
  _ha_entity_sound.publishConfiguration();
  _ha_entity_string.publishConfiguration();
  _ha_entity_switch.publishConfiguration();
  _ha_entity_temperature.publishConfiguration();
  _ha_entity_text.publishConfiguration();
  _ha_entity_timestamp.publishConfiguration();
  _ha_entity_unit_concentration.publishConfiguration();
  _ha_entity_volatile_organic_compounds.publishConfiguration();
  _ha_entity_voltage.publishConfiguration();
  _ha_entity_weight.publishConfiguration();
  _ha_entity_signal_strength.publishConfiguration();
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

    // Register for callbacks.
    _ha_entity_button.onCommand([]() {});
    _ha_entity_cover.onCommand([](HaEntityCover::Action action) {});
    _ha_entity_cover.onPosition([](uint8_t position) {});
    _ha_entity_fan.onCommand([](bool on) {});
    _ha_entity_fan.onSpeed([](uint32_t speed) {});
    _ha_entity_fan.onPreset([](std::string preset) {});
    _ha_entity_fan.onDirection([](std::string direction) {});
    _ha_entity_fan.onOscillation([](bool oscillation) {});
    _ha_entity_light.onCommand([](bool on) {});
    _ha_entity_light.onBrightness([](uint8_t brightness) {});
    _ha_entity_light.onColorTemperature([](uint16_t color_temp) {});
    _ha_entity_light.onRgb([](HaEntityLight::RGB rgb) {});
    _ha_entity_light.onEffect([](std::string effect) {});
    _ha_entity_number.onCommand([](float number) {});
    _ha_entity_select.onCommand([](std::string selected) {});
    _ha_entity_text.onCommand([](std::string text) {});
    _ha_entity_switch.onCommand([](bool on) {});

    // Re-publish configurations if Home Assistant restarts while we stay connected to MQTT.
    ha_bridge.onHomeAssistantBirth([]() { publishAllConfigurations(); });

    // Start MQTT (also publish on initial/re-established MQTT connection since HA's birth message is not retained by
    // default).
    _mqtt_remote.start([](bool connected) {
      if (connected) {
        publishAllConfigurations();
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
