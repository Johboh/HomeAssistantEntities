#ifndef __HA_ENTITY_EVENT_H__
#define __HA_ENTITY_EVENT_H__

#include <Arduino.h>
#include <HaBridge.h>
#include <HaEntity.h>
#include <functional>
#include <map>
#include <set>
#include <variant>

#define ATTRIBUTE_VARIANTS std::variant<int, float, double, bool, String, std::string, const char *>

/**
 * @brief Represent an Event that can be sent to Home Assistant.
 * See more at https://www.home-assistant.io/integrations/event.mqtt/
 */
class HaEntityEvent : public HaEntity {
public:
  enum class DeviceClass {
    None,
    Button,
    Motion,
    Dorrbell,
  };

  /**
   * @brief Construct a new Ha Entity Event object
   *
   * @param name this is the human readable name that will be used for the entity in Home Assistant. If a device is set
   * when creating the [HaBridge], the friendly named displayed in the UI will be the device name plus this name.
   * Example: if device name is "Bathroom" and entity name "event", friendly name will be "Bathroom event". If no
   * device, friendly name will be just "event". If a device is set, this name can be left empty if this entity is the
   * one main entity (or only) entity of this device. See
   * https://developers.home-assistant.io/docs/core/entity/#entity-naming for more
   * information.
   * @param object_id object ID for this event. Should be unique for each event for this node/device. Example:
   * "balcony_leds_event". Valid characters are [a-zA-Z0-9_-] (machine readable, not human readable)
   * @param event_types A set of valid event_type strings.
   * @param device_class the device class for for this event. None to not specify (default).
   */
  HaEntityEvent(HaBridge &ha_bridge, String name, String object_id, std::set<String> event_types,
                DeviceClass device_class = DeviceClass::None);

public:
  void publishConfiguration() override;
  void republishState() override;

  /**
   * @brief Publish an event. Must be one of the event types in the event_types list.
   *
   * @param event the event.
   * @param attributes optional attributes to send with the event.
   */
  void publishEvent(String event, std::map<std::string, ATTRIBUTE_VARIANTS> attributes = {});

private:
  String _name;
  HaBridge &_ha_bridge;
  String _object_id;
  std::set<String> _event_types;
  DeviceClass _device_class;
  unsigned long _total_string_length_of_event_types = 0;
};

#endif // __HA_ENTITY_EVENT_H__