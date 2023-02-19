#include "HaEntityRetries.h"
#include "ArduinoJson.h"

#define COMPONENT "sensor"
#define OBJECT_ID "retries"

HaEntityRetries::HaEntityRetries(HaBridge &ha_bridge) : _ha_bridge(ha_bridge) {}

void HaEntityRetries::publishConfiguration() {
  // TODO no Home Assistant Configuration at this point.
}

void HaEntityRetries::republishState() {
  // We don't want to republish anything.
}

void HaEntityRetries::publishRetries(uint8_t retries) {
  _ha_bridge.publishMessage(_ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, OBJECT_ID, ""), String(retries));
}
