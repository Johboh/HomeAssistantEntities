#include "credentials.h"
#include <HaBridge.h>
#include <MQTTRemote.h>
#include <driver/gpio.h>
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <nlohmann/json.hpp>

#include <entities/HaEntityAtmosphericPressure.h>
#include <entities/HaEntityBoolean.h>
#include <entities/HaEntityBrightness.h>
#include <entities/HaEntityButton.h>
#include <entities/HaEntityCarbonDioxide.h>
#include <entities/HaEntityCurtain.h>
#include <entities/HaEntityDoor.h>
#include <entities/HaEntityEvent.h>
#include <entities/HaEntityHumidity.h>
#include <entities/HaEntityJson.h>
#include <entities/HaEntityLight.h>
#include <entities/HaEntityLock.h>
#include <entities/HaEntityMotion.h>
#include <entities/HaEntityNumber.h>
#include <entities/HaEntityParticulateMatter.h>
#include <entities/HaEntitySelect.h>
#include <entities/HaEntitySensor.h>
#include <entities/HaEntitySound.h>
#include <entities/HaEntityString.h>
#include <entities/HaEntitySwitch.h>
#include <entities/HaEntityTemperature.h>
#include <entities/HaEntityText.h>
#include <entities/HaEntityUnitConcentration.h>
#include <entities/HaEntityVolatileOrganicCompounds.h>
#include <entities/HaEntityVoltage.h>
#include <entities/HaEntityWeight.h>

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

// ALl entities.
HaEntityAtmosphericPressure _ha_entity_atm(ha_bridge, "pressure", std::nullopt,
                                           HaEntityAtmosphericPressure::Configuration{.force_update = false});
HaEntityBoolean _ha_entity_bool(ha_bridge, "bool", std::nullopt, {.with_attributes = false, .force_update = false});
HaEntityBrightness _ha_entity_bright(ha_bridge, "brightness", std::nullopt, {.force_update = false});
HaEntityButton _ha_entity_button(ha_bridge, "button", "");
HaEntityCarbonDioxide _ha_entity_carbon_dioxide(ha_bridge, "Carbon dioxide");
HaEntityCurtain _ha_entity_curtain(ha_bridge, "curtain", "left_window");
HaEntityDoor _ha_entity_door(ha_bridge, "door", "");
HaEntityEvent _ha_entity_event(ha_bridge, "event", "party",
                               {.event_types = {"button_press"}, .device_class = HaEntityEvent::DeviceClass::Button});
HaEntityHumidity _ha_entity_humidity(ha_bridge, "humidity", std::nullopt, {.force_update = false});
HaEntityJson _ha_entity_json(ha_bridge, "json", std::nullopt, {.force_update = false});
HaEntityLight
    _ha_entity_light(ha_bridge, "light", "roof",
                     {.with_brightness = false, .with_rgb_color = false, .effects = {"colorloop"}, .retain = false});
HaEntityLock _ha_entity_lock(ha_bridge, "lock");
HaEntityMotion _ha_entity_motion(ha_bridge, "motion");
HaEntityNumber _ha_entity_number(ha_bridge, "number", "danceability",
                                 {.min_value = 0, .max_value = 100, .force_update = false, .retain = false});
HaEntityParticulateMatter _ha_entity_particulate_matter(ha_bridge, "Particulate matter", std::nullopt,
                                                        {.size = HaEntityParticulateMatter::Size::pm25});
HaEntitySelect _ha_entity_select(ha_bridge, "select", "playlist", {.options = {"option1", "option2"}, .retain = false});
HaEntitySound _ha_entity_sound(ha_bridge, "sound", "");
homeassistantentities::Sensor::Precipitation _precipitation;
HaEntitySensor _ha_entity_sensor(ha_bridge, "sensor", std::nullopt,
                                 HaEntitySensor::Configuration{
                                     .device_class = _precipitation,
                                     .unit_of_measurement = homeassistantentities::Sensor::Precipitation::Unit::mm,
                                     .with_attributes = false,
                                     .force_update = false});
HaEntityString _ha_entity_string(ha_bridge, "string", std::nullopt,
                                 {.device_class = "enum", .with_attributes = false, .force_update = false});
HaEntitySwitch _ha_entity_switch(ha_bridge, "switch", "party", {.retain = false});
HaEntityTemperature _ha_entity_temperature(ha_bridge, "temperature", std::nullopt,
                                           {.unit = HaEntityTemperature::Unit::C, .force_update = false});
HaEntityText _ha_entity_text(ha_bridge, "text", "billboard",
                             {.min_text_length = 0,
                              .max_text_length = 255,
                              .with_state_topic = false,
                              .is_password = false,
                              .force_update = false,
                              .retain = false});
HaEntityUnitConcentration _ha_entity_unit_concentration(ha_bridge, "concentration", std::nullopt,
                                                        {.unit = HaEntityUnitConcentration::Unit::mL});
HaEntityVolatileOrganicCompounds _ha_entity_volatile_organic_compounds(
    ha_bridge, "volatile organic compounds", std::nullopt,
    {.unit = HaEntityVolatileOrganicCompounds::Unit::Concentration, .force_update = false});
HaEntityVoltage _ha_entity_voltage(ha_bridge, "voltage", std::nullopt,
                                   {.unit = HaEntityVoltage::Unit::mV, .force_update = false});
HaEntityWeight _ha_entity_weight(ha_bridge, "weight", std::nullopt,
                                 {.unit = HaEntityWeight::Unit::kg, .force_update = false});

void haStateTask(void *pvParameters) {
  nlohmann::json jsn;
  jsn["hello"] = "world";

  while (1) {
    _ha_entity_atm.publishAtmosphericPressure(1001.5);
    _ha_entity_bool.publishBoolean(true);
    _ha_entity_bool.publishAttributes({{"attr1", "value1"}, {"attr2", "value2"}});
    _ha_entity_bright.publishBrightness(55.0);
    _ha_entity_carbon_dioxide.publishConcentration(55.0);
    _ha_entity_curtain.publishCurtain(HaEntityCurtain::State::Opening, 50);
    _ha_entity_door.publishDoor(true);
    _ha_entity_event.publishEvent("button_press", {{"attr1", "value1"}, {"attr2", "value2"}});
    _ha_entity_humidity.publishHumidity(55.0);
    _ha_entity_json.publishJson(jsn);
    _ha_entity_light.publishIsOn(true);
    _ha_entity_light.publishBrightness(255);
    _ha_entity_light.publishEffect("effect");
    _ha_entity_light.publishRgb(255, 255, 255);
    _ha_entity_lock.publishLock(true);
    _ha_entity_motion.publishMotion(true);
    _ha_entity_number.publishNumber(55.0);
    _ha_entity_particulate_matter.publishConcentration(55.0);
    _ha_entity_select.publishSelection("option");
    _ha_entity_sensor.publishValue(55.0);
    _ha_entity_sound.publishSound(true);
    _ha_entity_string.publishString("string", {{"attr1", "value1"}, {"attr2", "value2"}});
    _ha_entity_switch.publishSwitch(true);
    _ha_entity_temperature.publishTemperature(55.0);
    _ha_entity_text.publishText("text");
    _ha_entity_unit_concentration.publishConcentration(55.0);
    _ha_entity_volatile_organic_compounds.publishConcentration(55.0);
    _ha_entity_voltage.publishVoltage(55.0);
    _ha_entity_weight.publishWeight(55.0);
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
      // Publish Home Assistant Configuration once connected to MQTT.
      _ha_entity_atm.publishConfiguration();
      _ha_entity_bool.publishConfiguration();
      _ha_entity_bright.publishConfiguration();
      _ha_entity_button.publishConfiguration();
      _ha_entity_carbon_dioxide.publishConfiguration();
      _ha_entity_curtain.publishConfiguration();
      _ha_entity_door.publishConfiguration();
      _ha_entity_event.publishConfiguration();
      _ha_entity_humidity.publishConfiguration();
      _ha_entity_json.publishConfiguration();
      _ha_entity_light.publishConfiguration();
      _ha_entity_lock.publishConfiguration();
      _ha_entity_motion.publishConfiguration();
      _ha_entity_number.publishConfiguration();
      _ha_entity_particulate_matter.publishConfiguration();
      _ha_entity_select.publishConfiguration();
      _ha_entity_sensor.publishConfiguration();
      _ha_entity_sound.publishConfiguration();
      _ha_entity_string.publishConfiguration();
      _ha_entity_switch.publishConfiguration();
      _ha_entity_temperature.publishConfiguration();
      _ha_entity_text.publishConfiguration();
      _ha_entity_unit_concentration.publishConfiguration();
      _ha_entity_volatile_organic_compounds.publishConfiguration();
      _ha_entity_voltage.publishConfiguration();
      _ha_entity_weight.publishConfiguration();
    });

    // Register for callbacks.
    _ha_entity_button.setOnPressed([]() {});
    _ha_entity_curtain.setOnState([](HaEntityCurtain::Action action) {});
    _ha_entity_curtain.setOnPosition([](uint8_t postition) {});
    _ha_entity_light.setOnOn([](bool on) {});
    _ha_entity_light.setOnBrightness([](uint8_t brightness) {});
    _ha_entity_light.setOnRgb([](HaEntityLight::RGB rgb) {});
    _ha_entity_light.setOnEffect([](std::string effect) {});
    _ha_entity_number.setOnNumber([](float number) {});
    _ha_entity_select.setOnSelected([](std::string selected) {});
    _ha_entity_text.setOnText([](std::string text) {});
    _ha_entity_switch.setOnState([](bool on) {});

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
