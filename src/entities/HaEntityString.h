#ifndef __HA_ENTITY_STRING_H__
#define __HA_ENTITY_STRING_H__

#include "AttributeVariants.h"
#include "HaEntitySensor.h"
#include <HaBridge.h>
#include <HaEntity.h>
#include <cstdint>
#include <optional>
#include <string>

/**
 * @brief Represent a raw String sensor with a state topic on which you post your string. From HA, read only. See
 * [HaEntityText] for a read/write version. Also see [HaEntityJson] which is very similar. TODO(johboh): merge?
 */
class HaEntityString : public HaEntity {
public:
  struct Configuration {
    /**
     * @brief if true, setup an attribute topic attributes will be published to. With this set, attributes
     * can be published when the message is published, or using a separate call.
     */
    bool with_attributes = false;

    /**
     * In Home Assistant, trigger events even if the sensor's state hasn't changed. Useful if you want
     * to have meaningful value graphs in history or want to create an automation that triggers on every incoming state
     * message (not only when the sensor’s new state is different to the current one).
     */
    bool force_update = false;
  };

  inline static Configuration _default = {.with_attributes = false, .force_update = false};

  /**
   * @brief Construct a new Ha Entity String object
   *
   * @param name this is the human readable name that will be used for the entity in Home Assistant. If a device is set
   * when creating the [HaBridge], the friendly named displayed in the UI will be the device name plus this name.
   * Example: if device name is "Bathroom" and entity name "string", friendly name will be "Bathroom string". If no
   * device, friendly name will be just "string". If a device is set, this name can be left empty if this entity is the
   * one main entity (or only) entity of this device. See
   * https://developers.home-assistant.io/docs/core/entity/#entity-naming for more
   * information.
   * @param child_object_id optional child identifier for this entity in case there are several sensors of the same
   * entity type for the same node ID. Example: If you have a lock for the node ID "door", the home asisstant
   * configuration path will be "homeassistant/binary_sensor/door/lock/config". This works if you only have one lock on
   * your door, but if you have two locks, you want to add a child object ID to them. By setting the child_object_id to
   * say "upper", the configuration will be "homeassistant/binary_sensor/door/lock/upper/config". This also apply for
   * all state/command topics and so on. Leave as empty string for no child object ID. Valid characters
   * are [a-zA-Z0-9_-] (machine readable, not human readable)
   * @param configuration the configuration for this entity.
   */
  HaEntityString(HaBridge &ha_bridge, std::string name, std::optional<std::string> child_object_id = std::nullopt,
                 Configuration configuration = _default)
      : _ha_entity_sensor(HaEntitySensor(ha_bridge, name, child_object_id,
                                         HaEntitySensor::Configuration{
                                             .device_class = _string,
                                             .state_class = std::nullopt,
                                             .with_attributes = configuration.with_attributes,
                                             .force_update = configuration.force_update,
                                         })) {}

public:
  void publishConfiguration() override { _ha_entity_sensor.publishConfiguration(); }
  void republishState() override { _ha_entity_sensor.republishState(); }

  /**
   * @brief Publish the string. This will publish to MQTT regardless if the string has changed. Also see
   * updateString().
   *
   * @param str the string to publish.
   * @param attributes optional attributes to send with the string. with_attributes in constructor must be set.
   */
  void publishString(std::string str, Attributes::Map attributes = {}) {
    _ha_entity_sensor.publishValue(str, attributes);
  }

  /**
   * @brief Publish the string, but only if the string has changed. Also see publishString().
   *
   * @param str the string to publish.
   * @param attributes optional attributes to send with the string. with_attributes in constructor must be set.
   */
  void updateString(std::string str, Attributes::Map attributes = {}) {
    _ha_entity_sensor.updateValue(str, attributes);
  }

  /**
   * @brief Publish attributes only. with_attributes in constructor must be set.
   *
   * @param attributes
   */
  void publishAttributes(Attributes::Map attributes) { _ha_entity_sensor.publishAttributes(attributes); }

private:
  const homeassistantentities::Sensor::Undefined::String _string;
  HaEntitySensor _ha_entity_sensor;
};

#endif // __HA_ENTITY_STRING_H__