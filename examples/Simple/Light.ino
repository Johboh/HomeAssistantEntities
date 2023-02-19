#include <Ardunio.h>
#include <HaBridge.h>
#include <HaEntityLight.h>
#include <MQTTRemote.h>

#define LED_PIN D1

const char mqtt_client_id[] = "my-client";
const char mqtt_host[] = "192.168.1.1";
const char mqtt_username[] = "my-username";
const char mqtt_password[] = "my-password";

// Information about this device, e.g. the router.
// All these keys will be added to a "device" key in the Home Assistant configuration for each entity.
// Only a flat layout structure is supported, no nesting.
// We call this from setup function below before we setup the remote.
StaticJsonDocument<256> _json_this_device_doc;
void setupJsonForThisDevice() {
  _json_this_device_doc["identifiers"] = "esp32s2_" + String(mqtt_client_id);
  _json_this_device_doc["name"] = mqtt_client_id;
  _json_this_device_doc["sw_version"] = "1.0.0";
  _json_this_device_doc["model"] = "esp32s2";
  _json_this_device_doc["manufacturer"] = "custom inc.";
}

MQTTRemote _mqtt_remote(mqtt_client_id, mqtt_host, 1883, mqtt_username, mqtt_password);
HaBridge ha_bridge(_mqtt_remote, _json_this_device_doc, "kitchen");
HaEntityLight _ha_entity_light(ha_bridge, "Kitchen bench", "bench", true);

bool _was_connected = false;
unsigned long _last_publish_ms = 0;

void setup() {
  Serial.begin(115200);
  setupJsonForThisDevice();
}

void loop() {
  _mqtt_remote.handle();

  // Subscribe to the topic "interesting/topic" on new connection.
  auto connected = _mqtt_remote.connected();
  if (!_was_connected && connected) {
    _ha_entity_light.setOnState([&](bool on) { digitalWrite(LED_PIN, on); });
  }
  _was_connected = connected;

  // Publish light status every 2 second.
  auto now = millis();
  if (_last_publish_ms - now > 2000) {
    bool on = digitalRead(LED_PIN);
    _ha_entity_light.publishLight(std::optional<bool>{on}, std::optional<uint8_t>{255});
    _last_publish_ms = now;
  }
}
