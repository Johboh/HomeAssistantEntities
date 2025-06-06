#include "HaEntityEvent.h"
#include <HaUtilities.h>
#include <IJson.h>

#define COMPONENT "event"

HaEntityEvent::HaEntityEvent(HaBridge &ha_bridge, std::string name, std::string object_id, Configuration configuration)
    : _name(homeassistantentities::trim(name)), _ha_bridge(ha_bridge), _object_id(object_id),
      _configuration(configuration) {}

void HaEntityEvent::publishConfiguration() {
  IJsonDocument doc;

  if (!_name.empty()) {
    doc[_ha_bridge.useAbbreviations() ? "name" : "name"] = _name;
  } else {
    doc[_ha_bridge.useAbbreviations() ? "name" : "name"] = nullptr;
  }
  switch (_configuration.device_class) {
  case DeviceClass::Button:
    doc[_ha_bridge.useAbbreviations() ? "dev_cla" : "device_class"] = "button";
    break;
  case DeviceClass::Motion:
    doc[_ha_bridge.useAbbreviations() ? "dev_cla" : "device_class"] = "motion";
    break;
  case DeviceClass::Doorbell:
    doc[_ha_bridge.useAbbreviations() ? "dev_cla" : "device_class"] = "doorbell";
    break;
  case DeviceClass::None:
    break;
  }

  doc[_ha_bridge.useAbbreviations() ? "stat_t" : "state_topic"] =
      _ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, _object_id);

  JsonArrayType event_types_array = createJsonArray(doc, _ha_bridge.useAbbreviations() ? "evt_typ" : "event_types");
  for (const std::string &event_type : _configuration.event_types) {
    addToJsonArray(event_types_array, event_type);
  }

  _ha_bridge.publishConfiguration(COMPONENT, _object_id, "", doc);
}

void HaEntityEvent::republishState() {
  // Events are never republished.
}

void HaEntityEvent::publishEvent(std::string event, Attributes::Map attributes) {
  IJsonDocument doc;
  doc[_ha_bridge.useAbbreviations() ? "evt_typ" : "event_type"] = event;

  Attributes::toJson(doc, attributes, {"event_type"});

  auto message = toJsonString(doc);
  _ha_bridge.publishMessage(_ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, _object_id), message);
}
