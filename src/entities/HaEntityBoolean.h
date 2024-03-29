#ifndef __HA_ENTITY_BOOLEAN_H__
#define __HA_ENTITY_BOOLEAN_H__

#include "AttributeVariants.h"
#include <HaBridge.h>
#include <HaEntity.h>
#include <cstdint>
#include <optional>
#include <string>

/**
 * @brief Represent a boolean binary sensor.
 */
class HaEntityBoolean : public HaEntity {
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

  inline static Configuration _default = Configuration{.with_attributes = false, .force_update = false};

  /**
   * @brief Construct a new Ha Entity Boolean object
   *
   * @param name this is the human readable name that will be used for the entity in Home Assistant. If a device is set
   * when creating the [HaBridge], the friendly named displayed in the UI will be the device name plus this name.
   * Example: if device name is "Bathroom" and entity name "boolean", friendly name will be "Bathroom boolean". If no
   * device, friendly name will be just "boolean". If a device is set, this name can be left empty if this entity is the
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
  HaEntityBoolean(HaBridge &ha_bridge, std::string name, std::string child_object_id = "",
                  Configuration configuration = _default);

public:
  void publishConfiguration() override;
  void republishState() override;

  /**
   * @brief Publish the boolean value for the binary sensor.
   * @param attributes optional attributes to send with the value. with_attributes in constructor must be set.
   *
   * @param value the value to publish.
   * @param attributes optional attributes to publish.
   */
  void publishBoolean(bool value, Attributes::Map attributes = {});

  /**
   * @brief Publish attributes only. with_attributes in constructor must be set.
   *
   * @param attributes attributes to publish.
   */
  void publishAttributes(Attributes::Map attributes);

private:
  std::string _name;
  HaBridge &_ha_bridge;
  std::string _child_object_id;
  Configuration _configuration;

private:
  std::optional<bool> _value;
  std::optional<Attributes::Map> _attributes;
};

#endif // __HA_ENTITY_BOOLEAN_H__