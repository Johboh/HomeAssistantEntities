#include "HaEntityTemperature.h"
#include "ArduinoJson.h"

#define COMPONENT "sensor"
#define OBJECT_ID "temperature"

HaEntityTemperature::HaEntityTemperature(HaBridge &ha_bridge, String name, String child_object_id, bool force_update,
                                         Unit unit)
    : _unit(unit), _name(name), _force_update(force_update), _ha_bridge(ha_bridge), _child_object_id(child_object_id) {}

void HaEntityTemperature::publishConfiguration() {
  DynamicJsonDocument doc(512);
  doc["name"] = _name;
  switch (_unit) {
  case Unit::C:
    doc["unit_of_measurement"] = "°C";
    break;
  case Unit::F:
    doc["unit_of_measurement"] = "°F";
    break;
  }
  doc["device_class"] = "temperature";
  doc["force_update"] = _force_update;
  doc["state_topic"] = _ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, OBJECT_ID, _child_object_id);
  _ha_bridge.publishConfiguration(COMPONENT, OBJECT_ID, _child_object_id, doc);
}

void HaEntityTemperature::republishState() {
  if (_temperature) {
    publishTemperature(*_temperature);
  }
}

void HaEntityTemperature::publishTemperature(double temperature) {
  _ha_bridge.publishMessage(_ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, OBJECT_ID, _child_object_id),
                            String(temperature));
  _temperature = std::optional<double>{temperature};
}
